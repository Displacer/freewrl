# Copyright (C) 1998 Tuomas J. Lukka 1999 John Stewart CRC Canada
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.
#
# $Id: FieldHash.pm,v 1.6 2005/03/21 13:38:55 crc_canada Exp $
#
# The FieldHash
#
# This is the object behind the "RFields" hash member of
# the object VRML::NodeIntern. It allows you to send an event by
# simply saying "$node->{RFields}{xyz} = [3,4,5]" for which
# calls the STORE method here which then queues the event.
#
# XXX This needs to be separated into eventins and eventouts --
# assigning has different meanings.

use strict vars;

package VRML::FieldHash;
@VRML::FieldHash::ISA=Tie::StdHash;

sub TIEHASH {
	my ($type, $node) = @_;
	bless \$node, $type;
}

{
my %REALN = map {($_=>1)} qw/VRML::DEF VRML::USE/;
sub FETCH {
	my ($this, $k) = @_;
	my $node = $$this;
	my $v = $node->{Fields}{$k};

	#my ($package, $filename, $line) = caller;
	#print "FETCH $this ", VRML::NodeIntern::dump_name($node), " $k ",
	#VRML::Debug::toString($v), " from $package, $line\n";


	if ($VRML::verbose::tief) {
		print "TIEH: FETCH $k $v\n" ;
		if ("ARRAY" eq ref $v) {
			print "TIEHARRVAL: @$v\n";
		}
	}

	if ($REALN{ref $v}) {
		#AK - #$v = $v->real_node();
		$v = $v->node();
		print "TIEH: MOVED TO REAL NODE: $v\n"
			if $VRML::verbose::tief;
	} elsif (ref $v eq "VRML::IS") {
		$v = $v->get_ref();
		# really shouldn't happen...
		if (ref $v eq "VRML::IS") {
			die("IS statement $v->{Name} should have been dereferenced by now");
		}
		return ${$v};
	}

	return $v;
}

sub STORE {
	my ($this, $k, $value) = @_;
	if ($VRML::verbose::tief) {
		print "TIEH: STORE Node $this K $k value $value\n" ;
		if ("ARRAY" eq ref $value) {
			print "TIEHARRVAL: @$value\n";
		}
	}

	my $node = $$this;
	my $v = \$node->{Fields}{$k};

	my ($package, $filename, $line) = caller;
	print "STORE $this ", VRML::NodeIntern::dump_name($node),
		" $k, value ", VRML::Debug::toString($value),
		", field was ", VRML::Debug::toString($$v),
		" from $package, $line\n" if $VRML::verbose::events;

	$$v = $value;

	if (defined $node->{BackNode}) {
		print "\tBackNode defined $node->{BackNode}\n"  if $VRML::verbose::events;
		$node->set_backend_fields($k);
	}
}
}

sub FIRSTKEY {
	return undef
}

1;
