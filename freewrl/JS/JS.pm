# Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart, Ayla Khan CRC Canada
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.
#
# $Id: JS.pm,v 1.18 2003/04/08 18:15:42 ayla Exp $
#
#
#


package VRML::JS;

BEGIN {
    if ($^V lt v5.6.0) {
        # Perl voodoo to stop interpreters < v5.6.0 from complaining about
        # using our:
        sub our { return; }
    }
}

our $VERSION = '0.20';

require Exporter;
require DynaLoader;

our @ISA = qw{Exporter DynaLoader};
our @EXPORT = qw{
				 new,
				 initialize,
				 sendeventsproc,
				 sendevent,
				 cleanup
			 };

bootstrap VRML::JS $VERSION;


## Debug:
#$VRML::verbose::js=1;


if ($VRML::verbose::js) {
	setVerbose(1);
}

our $ECMAScriptNative = qr{^SF(?:Bool|Float|Time|Int32|String)$};

## See VRML97, section 4.12 (Scripting)
my $DefaultScriptMethods = "function initialize() {}; function shutdown() {}; function eventsProcessed() {}; TRUE=true; FALSE=false;";

my $eventInArg = "__tmp_arg_";
my $tmpFieldKind = "____tmp_f_";

## see jsUtils.h
my $browserSFNodeHandle = "__node.__handle";
my $browserRetval = "Browser.__ret";


sub new {
	my ($type, $text, $node, $browser) = @_;
	my $this = bless {
					  Node => $node,
					  Browser => $browser,
					  BrowserIntern => undef,
					  JSContext => undef,
					  JSGlobal => undef
					 }, $type;
	print "VRML::JS::new: $this $this->{Node}{TypeName} ",
		VRML::NodeIntern::dump_name($node),
				"\n$text\n" if $VRML::verbose::js;
	${$this->{Browser}}{JSCleanup} = \&{cleanup};

	init($this->{JSContext}, $this->{JSGlobal}, $this->{BrowserIntern}, $this);
	print "\tcontext $this->{JSContext}, global object $this->{JSGlobal}\n"
		if $VRML::verbose::js;

	my ($rs, $rval);

	if (!runScript($this->{JSContext},
				   $this->{JSGlobal},
				   $DefaultScriptMethods,
				   $rs,
				   $rval)) {
		cleanupDie("runScript failed in VRML::JS");
	}

	if (!runScript($this->{JSContext}, $this->{JSGlobal}, $text, $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS");
	}

	# Initialize fields.
	my $nt = $node->{Type};
	my @k = keys %{$nt->{Defaults}};
	my ($type, $ftype, $value, $constr, $v);

	print "\tFields: \n" if $VRML::verbose::js;
	for (@k) {
		next if $_ eq "url" or
			$_ eq "directOutput" or
			$_ eq "mustEvaluate";

		$this->initScriptFields($node, $_);
	}
	# Ignore all events we may have sent while building
	$this->gatherSentEvents(1);

	return $this;
}

sub initScriptFields {
	my ($this, $node, $field) = @_;
	my $nt = $node->{Type};
	my $type = $nt->{FieldTypes}{$field};
	my $ftype = "VRML::Field::$type";
	my $fkind = $nt->{FieldKinds}{$field};
	my ($rstr, $v);

	print "VRML::JS::initScriptFields: ", $nt->{FieldKinds}{$field},
		" $type $field\n" if $VRML::verbose::js;

	if ($fkind eq "eventIn") {
		if ($type !~ /$ECMAScriptNative/) {
			$constr = $this->constrString($type, 0);
			if (!addGlobalAssignProperty($this->{JSContext}, $this->{JSGlobal},
								   "$eventInArg"."$field", $constr)) {
				$this->cleanupDie("addGlobalAssignProperty failed in initScriptFields");
			}
		}
	} elsif ($fkind eq "eventOut") {
		if ($type =~ /$ECMAScriptNative/) {
			if (!addGlobalECMANativeProperty($this->{JSContext}, $this->{JSGlobal},
											 $field)) {
				$this->cleanupDie("addGlobalECMANativeProperty failed in initScriptFields");
			}
		} else {
			if ($type eq "SFNode") {
				$value = $node->{RFields}{$field};
				print "\tJS field property $field, value ",
					VRML::Debug::toString($value), "\n"
							if $VRML::verbose::js;
				$constr = $this->constrString($type, $value);
				$this->initSFNodeFields($field, $value);
			} else {
				$constr = $this->constrString($type, 0);
			}
			if (!addGlobalAssignProperty($this->{JSContext}, $this->{JSGlobal},
								   $field, $constr)) {
				$this->cleanupDie("addGlobalAssignProperty failed in initScriptFields");
			}
		}
	} elsif ($fkind eq "field") {
		$value = $node->{RFields}{$field};
		print "\tJS field property $field, value ",
			VRML::Debug::toString($value), "\n"
					if $VRML::verbose::js;

		if ($type =~ /$ECMAScriptNative/) {
			if (!addGlobalECMANativeProperty($this->{JSContext}, $this->{JSGlobal},
											 $field)) {
				$this->cleanupDie("addGlobalECMANativeProperty failed in initScriptFields");
			}
			if (!runScript($this->{JSContext}, $this->{JSGlobal},
						   "$field=".$ftype->as_string($value, 1), $rstr, $v)) {
				$this->cleanupDie("runScript failed in initScriptFields");
			}
		} else {
			$constr = $this->constrString($type, $value);
			if (!addGlobalAssignProperty($this->{JSContext}, $this->{JSGlobal},
								   $field, $constr)) {
				$this->cleanupDie("addGlobalAssignProperty failed in initScriptFields");
			}
			if ($type eq "SFNode") {
				$this->initSFNodeFields($field, $value);
			}
		}
	} else {
		warn("Invalid field $fkind $field for $node->{TypeName} in initScriptFields");
	}
}

sub initSFNodeFields {
	my ($this, $nodeName, $node) = @_;
	my $nt = $node->{Type};
	my $ntn = $node->{TypeName};
	my ($constr, $fkind, $ftype, $type, $value);
	my @fields = keys %{$node->{Fields}};

	print "VRML::JS::initSFNodeFields: $ntn $nodeName fields ",
		VRML::Debug::toString(\@fields), "\n"
				if $VRML::verbose::js;

	for (@fields) {
		next if $_ eq "url" or
			$_ eq "directOutput" or
			$_ eq "mustEvaluate";

		$fkind = $nt->{FieldKinds}{$_};
		$type = $nt->{FieldTypes}{$_};
		$ftype = "VRML::Field::$type";

		if ($fkind eq "eventIn") { ## correct???
			if ($type !~ /$ECMAScriptNative/) {
				$constr = $this->constrString($type, 0);
				if (!addSFNodeProperty($this->{JSContext}, $this->{JSGlobal},
									   $nodeName, $_, $constr)) {
					$this->cleanupDie("addSFNodeProperty failed in initSFNodeFields");
				}
			}
		} elsif ($fkind eq "eventOut") {
			$value = $node->{RFields}{$_};
			print "\tJS field property $_, value ",
				VRML::Debug::toString($value), "\n"
						if $VRML::verbose::js;
			if ($type !~ /$ECMAScriptNative/) {
				if ($type eq "SFNode") {
					$constr = $this->constrString($type, $value);
				} else {
					$constr = $this->constrString($type, 0);
				}
				if (!addSFNodeProperty($this->{JSContext}, $this->{JSGlobal},
									   $nodeName, $_, $constr)) {
					$this->cleanupDie("addSFNodeProperty failed in initSFNodeFields");
				}
			}
		} elsif ($fkind =~ /^(?:exposed)??[Ff]ield$/) {
			$value = $node->{RFields}{$_};
			print "\tJS field property $_, value ",
				VRML::Debug::toString($value), "\n"
						if $VRML::verbose::js;
			if ($type !~ /$ECMAScriptNative/) {
				$constr = $this->constrString($type, $value);
				if (!addSFNodeProperty($this->{JSContext}, $this->{JSGlobal},
									   $nodeName, $_, $constr)) {
					$this->cleanupDie("addSFNodeProperty failed in initSFNodeFields");
				}
			}
		} else {
			warn("Invalid field $fkind $_ for $ntn in initSFNodeFields");
			return;
		}
	}
}

sub constrString {
	my ($this, $ft, $v) = @_;
	my ($i, $l, $sft, $c);

	if ($v) {
		$c = "new $ft(";
		if ($ft =~ /^SFNode/) {
			if (VRML::Handles::check($v)) {
				$h = VRML::Handles::get($v);
			} else {
				$h = VRML::Handles::reserve($v);
			}
			$c .= "'".VRML::Field::SFNode->as_string($v)."','".$h."'";
		} elsif ($ft =~ /^MFString/) {
			$l = scalar(@{$v});
			for ($i = 0; $i < $l; $i++) {
				$c .= "'".$v->[$i]."'";
				$c .= "," unless ($i == ($l - 1));
			}
		} elsif ($ft =~ /^MFNode/) {
			$l = scalar(@{$v});
			for ($i = 0; $i < $l; $i++) {
				if (VRML::Handles::check($v->[$i])) {
					$h = VRML::Handles::get($v->[$i]);
				} else {
					$h = VRML::Handles::reserve($v->[$i]);
				}
				$c .= "new SFNode('".VRML::Field::SFNode->as_string($v->[$i])."','".$h."')";
				$c .= "," unless ($i == ($l - 1));
			}
		} elsif ($ft =~ /^MF(?:Color|Rotation|Vec2f|Vec3f)$/) {
			$sft = $ft;
			$sft =~ s/^MF/SF/;
			$l = scalar(@{$v});
			for ($i = 0; $i < $l; $i++) {
				if (ref($v->[$i]) eq "ARRAY") {
					$h = join(",", @{$v->[$i]});
					$c .= "new $sft(".$h.")";
					$c .= "," unless ($i == ($l - 1));
				}
			}
		} elsif (ref($v) eq "ARRAY") {
			$c .= join(",", @{$v});
		} else {
			$c .= $v;
		}
		$c .= ")";
	} else {
		$c = "new $ft()";
	}
	print "VRML::JS::constrString: $c\n" if $VRML::verbose::js;
	return $c;
}


sub cleanupDie {
	my ($this, $msg) = @_;
	cleanup();
	die($msg);
}

sub cleanup {
	my ($this) = @_;
	cleanupJS($this->{JSContext}, $this->{BrowserIntern});
}

sub initialize {
	my ($this) = @_;
	my ($rs, $val);
	print "VRML::JS::initialize: $this\n" if $VRML::verbose::js;

	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "initialize()", $rs, $val)) {
		cleanupDie("runScript failed in VRML::JS::initialize");
	}

	return $this->gatherSentEvents();
}

sub sendeventsproc {
	my($this) = @_;
	my ($rs, $rval);
	print "VRML::JS::sendeventproc: $this\n" if $VRML::verbose::js;
	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "eventsProcessed()", $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::sendeventproc");
	}

	return $this->gatherSentEvents();
}

sub gatherSentEvents {
	my ($this, $ignore) = @_;
	my $node = $this->{Node};
	my $t = $node->{Type};
	my @k = keys %{$t->{Defaults}};
	my @a;
	my ($rstr, $rnum, $runused, $propval);
	print "VRML::JS::gatherSentEvents: ",
		VRML::Debug::toString(\@_), "\n"
				if $VRML::verbose::js;
	for (@k) {
		next if $_ eq "url";
		my $type = $t->{FieldTypes}{$_};
		my $ftyp = $type;
		if ($t->{FieldKinds}{$_} eq "eventOut") {
			print "\teventOut $_\n" if $VRML::verbose::js;

			if ($type =~ /^MF/) {
				if (!runScript($this->{JSContext}, $this->{JSGlobal},
							   "$_.__touched_flag", $rstr, $rnum)) {
					cleanupDie("runScript failed in VRML::JS::gatherSentEvents");
				}
				if (!runScript($this->{JSContext}, $this->{JSGlobal},
							   "$_.__touched_flag=0", $rstr, $runused)) {
					cleanupDie("runScript failed in VRML::JS::gatherSentEvents");
				}
			} elsif ($ftyp =~ /$ECMAScriptNative/) {
				if (!runScript($this->{JSContext}, $this->{JSGlobal},
							   "_${_}_touched", $rstr, $rnum)) {
					cleanupDie("runScript failed in VRML::JS::gatherSentEvents");
				}
				if (!runScript($this->{JSContext}, $this->{JSGlobal},
							   "_${_}_touched=0", $rstr, $runused)) {
					cleanupDie("runScript failed in VRML::JS::gatherSentEvents");
				}
			} else {
				if (!runScript($this->{JSContext}, $this->{JSGlobal},
							   "$_.__touched()", $rstr, $rnum)) {
					cleanupDie("runScript failed in VRML::JS::gatherSentEvents");
				}
			}
			if ($rnum and !$ignore) {
				$propval = $this->getProperty($type, $_);
				push @a, [$node, $_, $propval];
			}
		}
	}
	return @a;
}

## Called from VRML::NodeIntern::receive_event.
sub sendevent {
	my ($this, $node, $event, $value, $timestamp) = @_;
	my $type = $node->{Type}{FieldTypes}{$event};
	my $ftype = "VRML::Field::$type";
	my $eventArg = "$eventInArg"."$event";
	my ($rs, $rval);


	print "VRML::JS::sendevent: ", VRML::Debug::toString(\@_),
		" field type $type\n"
			if $VRML::verbose::js;

	$this->setProperty($event, $value, $eventArg);

	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "$event($eventArg,$timestamp)", $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::sendevent");
	}
	return $this->gatherSentEvents();
}


## Assign values set in FreeWRL to properties stored in and used by the JS
## engine in preparation for calling eventIn(args) functions.
sub setProperty {
	my ($this, $f, $value, $prop) = @_;
	my ($ftype, $rs, $i, $rval);

	if ($f =~ s/^$tmpFieldKind//) { # recurse hack
		$ftype = $f;
	} else {
		$ftype = $this->{Node}{Type}{FieldTypes}{$f};
	}
	$vftype = "VRML::Field::$ftype";

	print "VRML::JS::setProperty: ", VRML::Debug::toString(\@_),
		" field type $ftype\n"
			if $VRML::verbose::js;

	if ($ftype =~ /^MF/) {
		my $length = scalar(@{$value});
		my $stype = $ftype;
		$stype =~ s/^MF/SF/;

		my $l;
		my $constr;
		if (!runScript($this->{JSContext}, $this->{JSGlobal},
					   "$prop.length", $rs, $l)) {
			cleanupDie("runScript failed in VRML::JS::getProperty for \"$prop.length\"");
		}
		print "\trunScript returned length $l for MFNode\n" if $VRML::verbose::js;
		for ($i = 0; $i < $length; $i++) {
			if ($i >= $l) {
				$constr = $this->constrString($stype, $value->[$i]);
				if (!runScript($this->{JSContext}, $this->{JSGlobal},
							   "$prop"."[$i]=$constr", $rs, $rval)) {
					cleanupDie("runScript failed in VRML::JS::setProperty");
				}
			} else { ## correct to assume property exists at index $i???
				$this->setProperty("$tmpFieldKind"."$stype", $value->[$i], "$prop"."[$i]");
			}
		}

		if (!runScript($this->{JSContext}, $this->{JSGlobal},
					   "$prop.__touched_flag=0", $rs, $rval)) {
			cleanupDie("runScript failed in VRML::JS::setProperty");
		}
	} elsif ($ftype =~ /$ECMAScriptNative/) {
		if (!runScript($this->{JSContext}, $this->{JSGlobal},
					   "$prop=".$vftype->as_string($value, 1), $rs, $rval)) {
			cleanupDie("runScript failed in VRML::JS::setProperty");
		}
		if (!runScript($this->{JSContext}, $this->{JSGlobal},
					   "_${prop}__touched=0", $rs, $rval)) {
			cleanupDie("runScript failed in VRML::JS::setProperty");
		}
	} elsif ($ftype eq "SFNode") {
		print "VRML::JS::setProperty: do nothing for SFNode for now.\n";
	} else {
		my $fxn = "js$ftype"."Set";
		&{$fxn}($this->{JSContext}, $this->{JSGlobal}, $prop, $value);
		if (!runScript($this->{JSContext}, $this->{JSGlobal},
					   "$prop.__touched()", $rs, $rval)) {
			cleanupDie("runScript failed in VRML::JS::setProperty");
		}
	}
}


sub getProperty {
	my ($this, $type, $prop) = @_;
	my ($rstr, $rval, $l, $i);
	my @res;

	print "VRML::JS::getProperty: ", VRML::Debug::toString(\@_), "\n"
		if $VRML::verbose::js;

	if ($type eq "SFNode") {
		if (!runScript($this->{JSContext}, $this->{JSGlobal},
					   "$prop.__handle", $rstr, $rval)) {
			cleanupDie("runScript failed in VRML::JS::getProperty");
		}
		return VRML::Handles::get($rstr);
	} elsif ($type =~ /$ECMAScriptNative/) {
		if (!runScript($this->{JSContext}, $this->{JSGlobal},
					   "_".$prop."_touched=0; $prop", $rstr, $rval)) {
			cleanupDie("runScript failed in VRML::JS::getProperty");
		}
		return $rval;
	} elsif ($type eq "MFNode") {
		if (!runScript($this->{JSContext}, $this->{JSGlobal},
					   "$prop.length", $rstr, $l)) {
			cleanupDie("runScript failed in VRML::JS::getProperty for \"$prop.length\"");
		}
		print "\trunScript returned length $l for MFNode\n"
			if $VRML::verbose::js;
		for ($i = 0; $i < $l; $i++) {
			if (!runScript($this->{JSContext}, $this->{JSGlobal},
						   "$prop"."[$i].__handle", $rstr, $rval)) {
				cleanupDie("runScript failed in VRML::JS::getProperty");
			}
			if ($rstr !~ /^undef/) {
				push @res, VRML::Handles::get($rstr);
			}
		}
		print "\treturn ", VRML::Debug::toString(\@res), "\n"
			if $VRML::verbose::js;
		return \@res;
	} else {
		if (!runScript($this->{JSContext}, $this->{JSGlobal}, "$prop", $rstr, $rval)) {
			cleanupDie("runScript failed in VRML::JS::getProperty");
		}
		print "\trunScript returned \"$rstr\" for $type.\n"
			if $VRML::verbose::js;
		(pos $rstr) = 0;
		return "VRML::Field::$type"->parse($this->{Browser}{Scene}, $rstr);
	}
}


sub addRemoveChildren {
	my ($this, $node, $field, $c) = @_;

	if ($field !~ /^(?:add|remove)Children$/) {
		warn("Invalid field $field for VRML::JS::addChildren");
		return;
	}

	print "VRML::JS::addRemoveChildren: ", VRML::Debug::toString(\@_), "\n"
		if $VRML::verbose::js;

	if (ref $c eq "ARRAY") {
		return if (!@{$c});
		$this->{Browser}->api__sendEvent($node, $field, $c);
	} else {
		return if (!$c);
		$this->{Browser}->api__sendEvent($node, $field, [$c]);
	}
}

sub jspSFNodeGetProperty {
	my ($this, $prop, $handle) = @_;

	$node = VRML::Handles::get($handle);
	my $type = $node->{Type}{FieldTypes}{$prop};
	my $ftype = "VRML::Field::$type";
	my ($rs, $rval);

	my $value = $node->{RFields}{$prop};
	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "$handle"."_$prop=".$ftype->as_string($value, 1),
				   $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::jspSFNodeGetProperty");
	}
}

sub jspSFNodeSetProperty {
	my ($this, $prop, $handle) = @_;
	my ($node, $val, $vt, $actualField, $rval);
	my $scene = $this->{Browser}{Scene};

	$node = VRML::Handles::get($handle);

	## see VRML97, section 4.7 (field, eventIn, and eventOut semantics)
	if ($prop =~ /^set_($VRML::Error::Word+)/ and
		$node->{Type}{FieldKinds}{$prop} !~ /in$/i) {
		$actualField = $1;
	} elsif ($prop =~ /($VRML::Error::Word+)_changed$/ and
			 $node->{Type}{FieldKinds}{$prop} !~ /out$/i) {
		$actualField = $1;
	} else {
		$actualField = $prop;
	}

	$vt = $node->{Type}{FieldTypes}{$actualField};

	if (!defined $vt) {
		cleanupDie("Invalid property $prop");
	}
	$val = $this->getProperty($vt, "$handle"."_$prop");

	print "VRML::JS::jspSFNodeSetProperty: setting $actualField, ",
		VRML::Debug::toString($val), " for $prop of $handle\n"
				if $VRML::verbose::js;

	if ($actualField =~ /^(?:add|remove)Children$/) {
		$this->addRemoveChildren($node, $actualField, $val);
	} else {
		$node->{RFields}{$actualField} = $val;
	}
}

sub jspSFNodeAssign {
	my ($this, $id) = @_;
	my ($vt, $val);
	my $scene = $this->{Browser}{Scene};
	my $root = $scene->{RootNode};
	my $field = "addChildren";
	my @av;

	print "VRML::JS::jspSFNodeAssign: id $id\n"
		if $VRML::verbose::js;

	$vt = $this->{Node}{Type}{FieldTypes}{$id};
	if (!defined $vt) {
		cleanupDie("Invalid id $id");
	}
	$val = $this->getProperty($vt, $id);
	$this->{Node}{RFields}{$id} = $val;

	$this->addRemoveChildren($root, $field, $val);
}

sub jspSFNodeConstr {
	my ($this, $str) = @_;
	my $scene = $this->{Browser}{Scene};
	my $handle = $this->{Browser}->createVrmlFromString($str);

	$node = VRML::Handles::get($handle);

	print "VRML::JS::jspSFNodeConstr: handle $handle, string \"$str\"\n"
		if $VRML::verbose::js;

	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "$browserSFNodeHandle"."=\"$handle\"", $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::jspSFNodeConstr");
	}
}


sub jspBrowserGetName {
	my ($this) = @_;
	my ($rval, $rs);

	print "VRML::JS::jspBrowserGetName\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getName();
	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "$browserRetval"."=\"$n\"", $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::jspBrowserGetName");
	}
}

sub jspBrowserGetVersion {
	my ($this) = @_;
	my ($rval, $rs);

	print "VRML::JS::jspBrowserGetVersion\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getVersion();

	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "$browserRetval"."=\"$n\"", $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::jspBrowserGetVersion");
	}
}

sub jspBrowserGetCurrentSpeed {
	my ($this) = @_;
	my ($rval, $rs);

	print "VRML::JS::jspBrowserGetCurrentSpeed\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getCurrentSpeed();

	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "$browserRetval"."=$n", $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::jspBrowserGetCurrentSpeed");
	}
}

sub jspBrowserGetCurrentFrameRate {
	my ($this) = @_;
	my ($rval, $rs);

	print "VRML::JS::jspBrowserGetCurrentFrameRate\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getCurrentFrameRate();

	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "$browserRetval"."=$n", $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::jspBrowserGetCurrentFrameRate");
	}
}

sub jspBrowserGetWorldURL {
	my ($this) = @_;
	my ($rval, $rs);

	print "VRML::JS::jspBrowserGetWorldURL\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getWorldURL();

	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   "$browserRetval"."=\"$n\"", $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::jspBrowserGetWorldURL");
	}
}

sub jspBrowserReplaceWorld {
	my ($this, $handles) = @_;

	print "VRML::JS::jspBrowserReplaceWorld $handles\n"
		if $VRML::verbose::js;

	$this->{Browser}->replaceWorld($handles);
}

sub jspBrowserLoadURL {
	my ($this, $url, $parameter) = @_;

	print "VRML::JS::jspBrowserLoadURL $url, $parameter\n"
		if $VRML::verbose::js;

	## VRML::Browser::loadURL not implemented yet
	$this->{Browser}->loadURL($url, $handles);
}

sub jspBrowserSetDescription {
	my ($this, $desc) = @_;
	$desc = join(" ", split(" ", $desc));

	## see VRML97, section 4.12.10.8
	if (!$this->{Node}{Type}{Defaults}{"mustEvaluate"}) {
		warn "VRML::JS::jspBrowserSetDescription: mustEvaluate for ",
			$this->{Node}{TypeName},
				" (", VRML::NodeIntern::dump_name($this->{Node}),
					") must be set to TRUE to call setDescription($desc)";
		return;
	}

	print "VRML::JS::jspBrowserSetDescription: $desc\n"
		if $VRML::verbose::js;

	$this->{Browser}->setDescription($desc);
}

sub jspBrowserCreateVrmlFromString {
	my ($this, $str) = @_;
	my ($rval, $rs);
	my @handles;
	print "VRML::JS::jspBrowserCreateVrmlFromString: \"$str\"\n"
		if $VRML::verbose::js;

	my $h = $this->{Browser}->createVrmlFromString($str);
	push @handles, split(" ", $h);

	my @nodes = map(VRML::Handles::get($_), @handles);
	my $constr = $this->constrString(MFNode, \@nodes);
	my $script = "$browserRetval"."=$constr";

	if (!runScript($this->{JSContext}, $this->{JSGlobal},
				   $script, $rs, $rval)) {
		cleanupDie("runScript failed in VRML::JS::jspBrowserCreateVrmlFromString");
	}
}

sub jspBrowserCreateVrmlFromURL {
	my ($this, $url, $handle, $event) = @_;
	my $h;
	my @createdHandles;
	my @rootNodes;

	print "VRML::JS::jspBrowserCreateVrmlFromURL: ",
		VRML::Debug::toString(\@_), "\n"
				if $VRML::verbose::js;

	my $scene = $this->{Browser}{Scene};
	my $root = $scene->{RootNode};
	my $urls = VRML::Field::MFString->parse($scene, $url);
	my $node = VRML::Handles::get($handle);

	for (@{$urls}) {
		$h = $this->{Browser}->createVrmlFromURL($_);
		push @createdHandles, split(" ", $h);
	}
	my @createdNodes = map(VRML::Handles::get($_), @createdHandles);

	for(@createdNodes) {
		if (defined $_->{Scene}{RootNode}) {
			push @rootNodes, $_->{Scene}{RootNode};
		}
	}

	print "VRML::JS::jspBrowserCreateVrmlFromURL: createdNodes",
		VRML::Debug::toString(\@createdNodes),
				", root nodes ",
					VRML::Debug::toString(\@rootNodes),
							"\n" if $VRML::verbose::js;

	$this->{Browser}->api__sendEvent($node, $event, \@rootNodes);
}

## update routing???
sub jspBrowserAddRoute {
	my ($this, $str) = @_;

	print "VRML::JS::jspBrowserAddRoute: route \"$str\"\n"
			if $VRML::verbose::js;
	if ($str =~ /^([^ ]+) ([^ ]+) ([^ ]+) ([^ ]+)$/) {
		my ($fn, $ff, $tn, $tf) = ($1, $2, $3, $4);

		$this->{Browser}->addRoute($fn, $ff, $tn, $tf);
	} else {
		cleanupDie("Invalid route for addRoute.");
	}
}

sub jspBrowserDeleteRoute {
	my ($this, $str) = @_;

	print "VRML::JS::jspBrowserDeleteRoute: route \"$str\"\n"
			if $VRML::verbose::js;
	if ($str =~ /^([^ ]+) ([^ ]+) ([^ ]+) ([^ ]+)$/) {
		my ($fn, $ff, $tn, $tf) = ($1, $2, $3, $4);

		$this->{Browser}->deleteRoute($fn, $ff, $tn, $tf);
	} else {
		cleanupDie("Invalid route for deleteRoute.");
	}
}
