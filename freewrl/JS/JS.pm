# Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart CRC Canada
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.
#
# $Id: JS.pm,v 1.1.1.1.8.2 2002/08/12 21:03:21 ayla Exp $
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
				 cleanup
				};

bootstrap VRML::JS $VERSION;

## Debug:
$VRML::verbose::js = 1;

if ($VRML::verbose::js) {
	setVerbose(1);
}

sub numeric { $_[0]+0 }

our %Types = (
			  SFBool => sub { $_[0] ? "true" : "false" },
			  SFFloat => \&numeric,
			  SFTime => \&numeric,
			  SFInt32 => \&numeric,
			  SFString => sub { '"'.$_[0].'"' }, # XXX
			  SFNode => sub { 'new SFNode("","'.(VRML::Handles::reserve($_[0])).'")'}
			 );
## Try later:
## SFNode => sub { q/new SFNode("","'.(VRML::Handles::reserve($_[0])).'")/}


## See VRML97, section 4.12 (Scripting)
my $DefaultScriptMethods = "function initialize() {}; function shutdown() {}; function eventsProcessed() {}; TRUE=true; FALSE=false;";



sub new {
	my ($type, $text, $node, $browser) = @_;
	my $this = bless {
					  Node => $node,
					  Browser => $browser,
					  JSContext => undef,
					  JSGlobal => undef,
					  BrowserIntern => undef,
					 }, $type;
	print "VRML::JS::new\n$text\n" if $VRML::verbose::js;
	${$this->{Browser}}{JSCleanup} = \&{cleanup};

	initContext($this->{JSContext}, $this->{JSGlobal}, $this->{BrowserIntern}, $this);

	my $rs;
	my $rval;

	if (!runScript($this->{JSContext},
				   $this->{JSGlobal},
				   $DefaultScriptMethods,
				   $rs,
				   $rval)) {
		cleanup();
		die("runScript failed in VRML::JS");
	}
print "runScript returned: $rs, $rval\n";
	if (!runScript($this->{JSContext}, $this->{JSGlobal}, $text, $rs, $rval)) {
		cleanup();
		die("runScript failed in VRML::JS");
	}

	# Initialize fields.
	my $nt = $node->{Type};
	my @k = keys %{$nt->{Defaults}};
	my $type;
	my $ftype;
	my $rval;

	print "\tNode type = ", %{$nt}, "\n" if $VRML::verbose::js;
	print "\tFields: \n" if $VRML::verbose::js;
	for (@k) {
		next if $_ eq "url" or $_ eq "mustEvaluate" or $_ eq "directOutput";

		#my $type = $nt->{FieldTypes}{$_};
		#my $ftype = "VRML::Field::$type";
		$type = $nt->{FieldTypes}{$_};
		$ftype = "VRML::Field::$type";
		print "\t\tinitialize field $_ of type $ftype\n" if $VRML::verbose::js;

		if ($nt->{FieldKinds}{$_} eq "field" or $nt->{FieldKinds}{$_} eq "eventOut") {

			print "\t\t\tJS field $_\n" if $VRML::verbose::js;
			if ($Types{$type}) {
				# addwatchprop($this->{JSContext},$this->{JSGlobal}, $_);
print "try AddWatchProperties for field or eventOut!\n";
				AddWatchProperties($this->{JSContext}, $this->{JSGlobal}, $_);
			} else {
				# addasgnprop($this->{JSContext},$this->{JSGlobal}, $_, $ftype->js_default);
print "try AddAssignProperties for field or eventOut!\n";
				AddAssignProperties($this->{JSContext}, $this->{JSGlobal}, $_, $ftype->js_default);
			}

			if ($nt->{FieldKinds}{$_} eq "field") {
				my $value = $node->{RFields}{$_};
				print "\t\t\tJS field property $_\n" if $VRML::verbose::js;
				if ($Types{$type}) {
					print "\t\t\t\tset type $_ '$value'\n" if $VRML::verbose::js;
					# my $v = runScript("$_=".$Types{$type}->($value), $rs);
					if (!runScript($this->{JSContext}, $this->{JSGlobal}, "$_=".$Types{$type}->($value), $rs, $rval)) {
						$this->cleanup();
						die("runScript failed in VRML::JS");
					}
				} else {
					$this->setProperty($_, $value, $_);
				}
			}
		} elsif ($nt->{FieldKinds}{$_} eq "eventIn") {
			if (!$Types{$type}) {
				# addasgnprop($this->{JSContext},$this->{JSGlobal}, "__tmp_arg_$_", $ftype->js_default);
print "try AddAssignProperties for eventIn!\n";
				AddAssignProperties($this->{JSContext}, $this->{JSGlobal}, "__tmp_arg_$_", $ftype->js_default);
			}
		} else {
			warn("Invalid field type '$_' for $node->{TypeName}");
		}
	}
	# Ignore all events we may have sent while building
	$this->gatherSentEvents(1);
	return $this;
}

sub initialize {
	my ($this) = @_;
	my $rs;
	my $rval;

	runScript($this->{JSContext}, $this->{JSGlobal}, "initialize()", $rs, $rval);
	$this->gatherSentEvents(0);
}

sub cleanup {
	my ($this) = @_;
	cleanupJS($this->{JSContext}, $this->{BrowserIntern});
}

sub sendeventsproc {
	my($this) = @_;
	my ($rs, $rval);
	runScript($this->{JSContext}, $this->{JSGlobal}, "eventsProcessed()", $rs, $rval);
	$this->gatherSentEvents();
}

sub gatherSentEvents {
	my ($this, $ignore) = @_;
	my $node = $this->{Node};
	my $t = $node->{Type};
	my @k = keys %{$t->{Defaults}};
	my @a;
	my $rs;
	for(@k) {
		next if $_ eq "url";
		my $type = $t->{FieldTypes}{$_};
		my $ftyp = $type;
		if($t->{FieldKinds}{$_} eq "eventOut") {
			print "VRML::JS eventOut $_\n"
				if $VRML::verbose::js;
			my ($v, $rval);
			if($type =~ /^MF/) {
				runScript($this->{JSContext}, $this->{JSGlobal}, "$_.__touched_flag", $rs, $v);
				runScript($this->{JSContext}, $this->{JSGlobal}, "$_.__touched_flag = 0", $rs, $rval);
			} elsif($Types{$ftyp}) {
				runScript($this->{JSContext}, $this->{JSGlobal}, "_${_}_touched", $rs, $v);
				runScript($this->{JSContext}, $this->{JSGlobal}, "_${_}_touched = 0", $rs, $rval);
				# print "SIMP_TOUCH $v\n";
			} else {
				runScript($this->{JSContext}, $this->{JSGlobal}, "$_.__touched()", $rs, $v);
			}
			print "VRML::JS::runScript returned: $v, $rs, $_\n"
				if $VRML::verbose::js;
			if($v && !$ignore) {
				push @a, [$node, $_,
					# $this->get_prop($type,$_)];
					$this->getProperty($type,$_)];
			}
		}
		# $this->{O}->print("$t->{FieldKinds}{$_}\n
	}
	return @a;
}

sub sendevent {
	my ($this, $node, $event, $value, $timestamp) = @_;
	my $rs;
	my $typ = $node->{Type}{FieldTypes}{$event};
	my $aname = "__tmp_arg_$event";
	my $rval;

	print "VRML::JS::sendevent $node $event $value $timestamp ($typ)\n"
		if $VRML::verbose::js;
	$this->setProperty($event,$value,$aname);
	runScript($this->{JSContext}, $this->{JSGlobal}, "$event($aname,$timestamp)", $rs, $rval);
	return $this->gatherSentEvents();

	unless($Types{$typ}) {
		&{"$nodeSetProperty->{Type}{FieldTypes}{$event}"}(
			$this->{JSContext}, $this->{JSGlobal}, "__evin", $value);
		# runScript($this->{JSContext}, $this->{JSGlobal}, "$event(__evin,$timestamp)", $rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$event(__evin,$timestamp)", $rs, $rval);
	} else {
		print "VRML::JS::sendevent $event $timestamp\n".
			"$event(".$Types{$typ}->($value).",$timestamp)\n"
				if $VRML::verbose::js;
		# my $v = runScript($this->{JSContext}, $this->{JSGlobal}, "$event(".$Types{$typ}->($value).",$timestamp)", $rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$event(".$Types{$typ}->($value).",$timestamp)", $rs, $rval);
		print "VRML::JS::runScript returned: $rval, $rs\n"
			if $VRML::verbose::js;
	}
	$this->gatherSentEvents(0);
}


sub setProperty { # Assigns a value to a property.
	my ($this,$field,$value,$prop) = @_;
	my $typ = $this->{Node}{Type};
	my $ftype;
	if ($field =~ s/^____//) { # recurse hack
		$ftype = $field;
	} else {
		$ftype = $typ->{FieldTypes}{$field};
	}
	my $rs;
	my $i;
	my $rval;

	if ($ftype =~ /^MF/) {
		my $styp = $ftype; $styp =~ s/^MF/SF/;
		for ($i = 0; $i < $#{$value}; $i++) {
			$this->setProperty("____$styp", $value->[$i], "____tmp");
#JS -added the rs parameter; lets see if this works....

			# runScript($this->{JSContext}, $this->{JSGlobal}, "$prop"."[$i] = ____tmp", $rs);
			runScript($this->{JSContext}, $this->{JSGlobal}, "$prop"."[$i] = ____tmp", $rs, $rval);
		}
		# runScript($this->{JSContext},$this->{JSGlobal}, "$prop.__touched_flag = 0", $rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$prop.__touched_flag = 0", $rs, $rval);
	} elsif ($Types{$ftype}) {
		# runScript($this->{JSContext},$this->{JSGlobal}, "$prop = ".(&{$Types{$ftype}}($value)), $rs);
		# runScript($this->{JSContext},$this->{JSGlobal},"_${prop}__touched=0", $rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$prop = ".(&{$Types{$ftype}}($value)), $rs, $rval);
		runScript($this->{JSContext}, $this->{JSGlobal}, "_${prop}__touched=0", $rs, $rval);
	} else {
		print "VRML::JS::setProperty: $ftype $prop %{$value}\n"
			if $VRML::verbose::js;
		&{"$ftype"."SetProperty"}($this->{JSContext}, $this->{JSGlobal}, $prop, $value);
		# runScript($this->{JSContext},$this->{JSGlobal},"$prop.__touched()", $rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$prop.__touched()", $rs, $rval);
	}
}

sub getProperty {
	my ($this,$type,$prop) = @_;
	my $rs;
	my $rval;
	my $l;

	print "RS2: $rs\n"
		if $VRML::verbose::js;
	if ($type =~ /^SFNode$/) {
		# runScript($this->{JSContext},$this->{JSGlobal}, "$prop.__id",$rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$prop.__id", $rs, $rval);
		return VRML::Handles::get($rs);
	} elsif ($type =~ /^MFNode$/) {
		# my $l = runScript($this->{JSContext},$this->{JSGlobal}, "$prop.length",$rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$prop.length", $rs, $l);
		print "LENGTH: $l, '$rs'\n"
			if $VRML::verbose::js;
		my $fn = $prop;
		my @res = map {
		     # runScript($this->{JSContext},$this->{JSGlobal}, "$fn",$rs);
		     runScript($this->{JSContext}, $this->{JSGlobal}, "$fn", $rs, $rval);
		     print "Just mfnode: '$rs'\n"
		     	if $VRML::verbose::js;
		     # runScript($this->{JSContext},$this->{JSGlobal}, "$fn"."[$_]",$rs);
		     runScript($this->{JSContext}, $this->{JSGlobal}, "$fn"."[$_]", $rs, $rval);
		     print "Just node: '$rs'\n"
		     	if $VRML::verbose::js;
		     # runScript($this->{JSContext},$this->{JSGlobal}, "$fn"."[$_][0]",$rs);
		     runScript($this->{JSContext}, $this->{JSGlobal}, "$fn"."[$_][0]", $rs, $rval);
		     print "Just node[0]: '$rs'\n"
		     	if $VRML::verbose::js;
		     # runScript($this->{JSContext},$this->{JSGlobal}, "$fn"."[$_].__id",$rs);
		     runScript($this->{JSContext}, $this->{JSGlobal}, "$fn"."[$_].__id", rs, $rval);
		     print "MFN: Got '$rs'\n"
		     	if $VRML::verbose::js;
		     VRML::Handles::get($rs);
		} (0..$l-1);
		return \@res;
	} elsif ($type =~ /^MFString$/) {
		# runScript($this->{JSContext}, $this->{JSGlobal}, "$prop.length", $rs, \$l);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$prop.length", $rs, $l);
		my $fn = $prop;
		my @res = map {
		     # runScript($this->{JSContext},$this->{JSGlobal}, "$fn"."[$_]",$rs);
		     # runScript($this->{JSContext},$this->{JSGlobal}, "$fn"."[$_]",$rs);
		     runScript($this->{JSContext}, $this->{JSGlobal}, "$fn"."[$_]", $rs, $rval);
		     runScript($this->{JSContext}, $this->{JSGlobal}, "$fn"."[$_]", $rs, $rval);
		     $rs
		} (0..$l-1);
		return \@res;
	} elsif ($type =~ /^MF/) {
		# my $l = runScript($this->{JSContext},$this->{JSGlobal}, "$prop.length",$rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$prop.length", $rs, $l);
		print "VRML::JS::runScript returned: rval, '$rs'\n"
			if $VRML::verbose::js;
		my $fn = $prop;
		my $st = $type;
		$st =~ s/MF/SF/;
		my @res = map {
		     # runScript($this->{JSContext},$this->{JSGlobal}, "$fn"."[$_]",$rs);
		     runScript($this->{JSContext}, $this->{JSGlobal}, "$fn"."[$_]", $rs, $rval);
		     print "VRML::JS::runScript returned: '$rs', $rval\n" if $VRML::verbose::js;
		     (pos $rs) = 0;
		     "VRML::Field::$st"-> parse(undef, $rs);
		} (0..$l-1);
		print "RESVAL:\n"
			if $VRML::verbose::js;
		for (@res) {
			if ("ARRAY" eq ref $_) {
				print "@$_\n"
					if $VRML::verbose::js;
			}
		}
		my $r = \@res;
		print "REF: $r\n"
			if $VRML::verbose::js;
		return $r;
	} elsif ($Types{$type}) {
		# my $v = runScript($this->{JSContext},$this->{JSGlobal}, "_${_}_touched=0; $prop",$rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "_${_}_touched=0; $prop", $rs, $rval);
		print "VRML::JS::runScript returned: $rval '$rs'\n" if $VRML::verbose::js;
		return $v;
	} else {
		# runScript($this->{JSContext},$this->{JSGlobal}, "$prop",$rs);
		runScript($this->{JSContext}, $this->{JSGlobal}, "$prop", $rs, $rval);
		# print "VAL: $rs\n";
		(pos $rs) = 0;
		return "VRML::Field::$type"->parse(undef,$rs);
	}
}

sub nodeSetProperty {
	my ($this) = @_;
	my ($node, $prop, $val);
	my $rval;

	print "VRML::JS::nodeSetProperty\n" if $VRML::verbose::js;

	# runScript($this->{JSContext},$this->{JSGlobal},"__node.__id",$node);
	# runScript($this->{JSContext},$this->{JSGlobal},"__prop",$prop);
	runScript($this->{JSContext}, $this->{JSGlobal}, "__node.__id", $node, $rval);
	runScript($this->{JSContext}, $this->{JSGlobal}, "__prop", $prop, $rval);

	print "\tsetting Property: '$node' '$prop' \n" if $VRML::verbose::js;
	$node = VRML::Handles::get($node);

	my $vt = $node->{Type}{FieldTypes}{$prop};
	if (!defined $vt) {
		die("Javascript tried to assign to invalid property!\n");
	}
	my $val = $this->getProperty($vt, "__val");

	if (0) {
		#	if ($vt =~ /Node/) {die("Can't handle yet");}
		#	if ($Types{$vt}) {
		#		runScript($this->{JSContext},$this->{JSGlobal},"__val",$val);
		#		print "GOT '$val'\n";
		#		$val = "VRML::Field::$vt"->parse(undef, $val);
		#	} else {
		#		runScript($this->{JSContext},$this->{JSGlobal},"__val.toString()",$val);
		#		print "GOT '$val'\n";
		#		$val = "VRML::Fields::$vt"->parse(undef, $val);
		#	}
	}

	print "\tsetting property to '$val'\n" if $VRML::verbose::js;
	$node->{RFields}{$prop} = $val;
}

sub browserGetName {
	my ($this) = @_;
	my $rval;

	print "browserGetName ($this) !\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getName();
	my $rs;
	# runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = \"$n\"", $rs);
	runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = \"$n\"", $rs, $rval);
}

sub browserGetVersion {
	my ($this) = @_;
	my $rval;

	print "browserGetVersion ($this) !\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getVersion();
	my $rs;
	# runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = \"$n\"", $rs);
	runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = \"$n\"", $rs, $rval);
}

## ????
#sub browserSetDescription {
#	my ($this, $desc) = @_;
#	print "browserSetDescription ($this) !\n"
#		if $VRML::verbose::js;
#	my $n = $this->{Browser}->setDescription($desc);
#	my $rs;
#	runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = \"$n\"", $rs);
#}

sub browserGetCurrentFrameRate {
	my ($this) = @_;
	my $rval;

	print "browserGetCurrentFrameRate ($this) !\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getCurrentFrameRate();
	my $rs;
	# runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = $n", $rs);
	runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = $n", $rs, $rval);
}

sub browserGetWorldURL {
	my ($this) = @_;
	my $rval;

	print "browserGetWorldURL ($this) !\n"
		if $VRML::verbose::js;
	my $n = $this->{Browser}->getWorldURL();
	my $rs;
	# runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = $n", $rs);
	runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__bret = $n", $rs, $rval);
}

sub browserCreateVrmlFromString {
	my ($this) = @_;
	my $rs;
	my $rval;

	# runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__arg0", $rs);
	runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__arg0", $rs, $rval);

	print "browserCreateVrmlFromString '$rs'\n"
		if $VRML::verbose::js;
	my $mfn = $this->{Browser}->createVrmlFromString($rs);
	my @hs = map {VRML::Handles::reserve($_)} @$mfn;
	my $sc = "Browser.__bret = new MFNode(".
		(join ',',map {qq'new SFNode("","$_")'} @hs).")";
	# runScript($this->{JSContext}, $this->{JSGlobal}, $sc, $rs);
	runScript($this->{JSContext}, $this->{JSGlobal}, $sc, $rs, $rval);
}

sub browserCreateVrmlFromURL {
	my ($this) = @_;
	my $rs;
	my $rval;

	# runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__arg0", $rs);
	runScript($this->{JSContext}, $this->{JSGlobal}, "Browser.__arg0", $rs, $rval);

	print "browserCreateVrmlFromURL '$rs'\n"
		if $VRML::verbose::js;

	my $mfn = $this->{Browser}->createVrmlFromURL($rs);
	my @hs = map {VRML::Handles::reserve($_)} @$mfn;
	my $sc = "Browser.__bret = new MFNode(".
		(join ',',map {qq'new SFNode("","$_")'} @hs).")";

	# runScript($this->{JSContext}, $this->{JSGlobal}, $sc, $rs);
	runScript($this->{JSContext}, $this->{JSGlobal}, $sc, $rs, $rval);
}
