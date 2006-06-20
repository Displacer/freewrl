#
# $Id: VRMLFields.pm,v 1.68 2006/06/20 18:48:54 crc_canada Exp $
#
# Copyright (C) 1998 Tuomas J. Lukka 1999 John Stewart CRC Canada
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.
#
# Field types, parsing and printing, Perl, C and Java.
#
# SFNode is in Parse.pm
#
# $Log: VRMLFields.pm,v $
# Revision 1.68  2006/06/20 18:48:54  crc_canada
# keep track of current working url.
#
# Revision 1.67  2006/06/20 17:58:57  crc_canada
# adds BrowserFullPath to __parenturl fields.
#
# Revision 1.66  2006/06/19 16:40:46  crc_canada
# PixelTextures using new parser.
#
# Revision 1.65  2006/06/09 17:16:43  crc_canada
# More SAI code changes.
#
# Revision 1.64  2006/06/02 17:19:15  crc_canada
# more C code changes, reduces the amount of perl interface calls dramatically.
#
# Revision 1.63  2006/05/24 19:29:03  crc_canada
# More VRML C Parser code
#
# Revision 1.62  2006/05/24 16:27:15  crc_canada
# more changes for VRML C parser.
#
# Revision 1.61  2006/03/13 15:08:24  crc_canada
# Event Utilities should be complete.
#
# Revision 1.60  2006/03/09 20:43:33  crc_canada
# Initial Event Utilities Component work.
#
# Revision 1.59  2006/03/01 18:02:33  crc_canada
# more warnings removed.
#
# Revision 1.58  2006/01/24 15:04:44  crc_canada
# Some warnings removed.
#
# Revision 1.57  2005/12/21 18:16:40  crc_canada
# Rework Generation code.
#
# Revision 1.56  2005/11/18 18:24:24  crc_canada
# TextureBackground work.
#
# Revision 1.55  2005/10/19 19:38:58  crc_canada
# MultiTexture, META, PROFILE, COMPONENT node support.
#
# Revision 1.54  2005/08/05 18:54:39  crc_canada
# ElevationGrid to new structure. works ok; still some minor errors.
#
# Revision 1.53  2005/08/03 18:41:40  crc_canada
# Working on Polyrep structure.
#
# Revision 1.52  2005/06/30 14:20:05  crc_canada
# 64 bit compile changes.
#
# Revision 1.51  2005/06/29 17:00:12  crc_canada
# EAI and X3D Triangle code added.
#
# Revision 1.50  2005/06/24 12:35:10  crc_canada
# Changes to help with 64 bit compiles.
#
# Revision 1.49  2005/06/17 21:15:09  crc_canada
# Javascript: MFTime, MF* routing, and script-to-script routing.
#
# Revision 1.48  2005/06/09 14:52:49  crc_canada
# ColorRGBA nodes supported.
#
# Revision 1.47  2005/03/21 13:39:04  crc_canada
# change permissions, remove whitespace on file names, etc.
#
# Revision 1.46  2005/02/10 14:50:25  crc_canada
# LineSet implemented.
#
# Revision 1.45  2005/02/07 20:25:43  crc_canada
# SFImage parse - parse an SFImage, new parsing terminal symbols include
# those found when parsing a PROTO decl. Eg, "field" is now left alone.
#
# Revision 1.44  2005/01/28 14:55:34  crc_canada
# Javascript SFImage works; Texture parsing changed to speed it up; and Cylinder side texcoords fixed.
#
# Revision 1.43  2005/01/18 20:52:35  crc_canada
# Make a ConsoleMessage that displays xmessage for running in a plugin.
#
# Revision 1.42  2005/01/16 20:55:08  crc_canada
# Various compile warnings removed; some code from Matt Ward for Alldev;
# some perl changes for generated code to get rid of warnings.
#
# Revision 1.41  2004/12/07 15:05:26  crc_canada
# Various changes; eat comma before SFColors for Rasmol; Anchor work,
# and general configuration changes.
#
# Revision 1.40  2004/08/25 14:56:18  crc_canada
# handle ISO extended characters within SFString
#
# Revision 1.39  2004/07/16 13:17:50  crc_canada
# SFString as a Java .class script field entry.
#
# Revision 1.38  2004/05/27 15:22:04  crc_canada
# javascripting problems fixed.
#
# Revision 1.37  2004/05/20 19:01:34  crc_canada
# pre4 files.
#
# Revision 1.36  2003/12/04 18:33:57  crc_canada
# Basic threading ok
#
# Revision 1.35  2003/09/16 14:57:24  crc_canada
# EAI in C updates for Sept 15 2003
#
# Revision 1.34  2003/06/12 19:08:56  crc_canada
# more work on getting javascript routing into C
#
# Revision 1.33  2003/06/02 18:21:40  crc_canada
# more work on CRoutes for Scripting
#
# Revision 1.32  2003/05/28 14:18:35  crc_canada
# Scripts moved to CRoute structure
#
# Revision 1.31  2003/05/08 16:01:33  crc_canada
# Moving code to C
#
# Revision 1.30  2003/04/25 19:43:13  crc_canada
# changed SFTime to double from float - float was not enough precision
# to hold time since epoch values.
#
# Revision 1.29  2003/03/20 18:37:22  ayla
#
# Added init() functions to be used by VRML::NodeIntern::do_defaults() to
# supply eventOuts with default values (used by Script, PROTO definitions).
#
# Revision 1.28  2003/01/28 18:23:59  ayla
# Problem with checking for undefined values in VRML::Field::Multi::as_string fixed.
#
# Revision 1.27  2002/11/29 17:07:10  ayla
#
# Parser was choking on escaped double quotes inside strings.  This should
# make it better.
#
# Revision 1.26  2002/11/28 20:15:41  crc_canada
# For 0.37, PixelTextures are handled in the same fashion as other static images
#
# Revision 1.25  2002/11/25 16:55:27  ayla
#
# Tweaked float/double formatting to strings and made changes to property
# setting for SFNodes.
#
# Revision 1.24  2002/11/22 22:09:14  ayla
# Tweaking SFFloat and SFTime formatting in as_string.
#
# Revision 1.23  2002/11/22 16:28:57  ayla
#
# Format floating point numbers for string conversion.
#
# Revision 1.22  2002/11/20 21:33:55  ayla
#
# Modified as_string functions as needed, commented unused js functions.
#
# Revision 1.21  2002/09/19 19:39:35  crc_canada
# some changes brought about by much EAI work.
#
# Revision 1.20  2002/07/11 16:21:42  crc_canada
# ImageTexture fields changed to read in textures by C rather than Perl
#
# Revision 1.19  2002/06/21 19:51:54  crc_canada
# Comma handling in number strings is much better now; previous code would
# not handle commas when preceeded by a space.
#
# Revision 1.18  2002/06/17 16:39:19  ayla
#
# Fixed VRML::DEF copy problem in package VRML::Field.
#
# Revision 1.17  2002/05/22 21:47:52  ayla
#
# Broke Scene.pm into files containing constituent packages for sanity's sake.
#
# Revision 1.16  2002/05/01 15:12:24  crc_canada
# add __texture field to store OpenGLs bound texture number
#
# Revision 1.15  2002/01/12 15:44:18  hoenicke
# Removed the Java SAI stuff from here.  It moved to VRMLJava.pm
# and java/classes/vrml/genfields.pl
#
# Revision 1.14  2001/11/28 08:34:30  ayla
#
# Tweaked regexp processing for VRML::Field::SFString::parse due to a submitted
# bug report.  It appeared that perl crashed with a segmentation fault while
# attempting to process the regexp for an SFString.  This process has
# been optimized.  Hopefully it won't break anything!
#
# Revision 1.13  2001/08/16 16:55:54  crc_canada
# Viewpoint work
#
# Revision 1.12  2001/07/11 20:43:05  ayla
#
#
# Fixed problem with Plugin/Source/npfreewrl.c, so all debugging info. is turned
# off.
#
# Committing merge between NetscapeIntegration and the trunk.
#
# Revision 1.11  2001/06/18 17:25:11  crc_canada
# IRIX compile warnings removed.
#
#
#
# Merging recent changes from the freewrl trunk to this branch.
#
# Revision 1.11  2001/06/18 17:25:11  crc_canada
# IRIX compile warnings removed.
#
# Revision 1.10  2001/05/30 19:07:44  ayla
#
# Ref. bug id #426972: sfvec3f values don't take commas, submitted by J. Stewart (crc_canada).
# Altered the code used to read in SFColor, SFVec2f and SFRotation floating point numbers.
#
# Revision 1.9  2001/05/25 21:34:00  ayla
#
#
# The regexp used to identify and capture strings was altered to follow the VRML97
# specification more closely.
# The function parse in package VRML::Field::Multi was altered to handle the occurrence
# of multiple commas in VRML fields (as in the NIST test file Appearance/FontStyle/default.wrl).
#
# Revision 1.8  2001/03/23 16:02:11  crc_canada
# unknown, unrecorded changes.
#
# Revision 1.7  2000/12/13 14:41:57  crc_canada
# Bug hunting.
#
# Revision 1.6  2000/12/01 02:01:39  crc_canada
# More SAI work
#
# Revision 1.5  2000/11/29 18:45:19  crc_canada
# Format changes, and trying to get more SAI node types working.
#
# Revision 1.4  2000/11/15 13:51:25  crc_canada
# First attempt to work again on SAI
#
# Revision 1.3  2000/08/13 14:26:46  rcoscali
# Add a js constructor for SFImage (need some work to do)
# Fixed default SFImage JS statement
#
# Revision 1.2  2000/08/07 08:28:35  rcoscali
# Removed reference to Image::Xpm (not usefull anymore for PixelTexture)
#
#

# XXX Decide what's the forward assertion..

@VRML::Fields = qw/
	SFFloat
	MFFloat
	SFRotation
	MFRotation
	SFVec3f
	MFVec3f
	SFBool
	MFBool
	SFInt32
	MFInt32
	SFNode
	MFNode
	SFColor
	MFColor
	SFColorRGBA
	MFColorRGBA
	SFTime
	MFTime
	SFString
	MFString
	SFVec2f
	MFVec2f
	SFImage
	FreeWRLPTR
/;

###########################################################
package VRML::Field;
VRML::Error->import();

sub es {
	$p = (pos $_[1]) - 20;
	return substr $_[1],$p,40;

}

# The C type interface for the field type, encapsulated
# By encapsulating things well enough, we'll be able to completely
# change the interface later, e.g. to fit together with javascript etc.
sub ctype ($) {die "VRML::Field::ctype - abstract function called"}
sub clength ($) {0} #for C routes. Keep in sync with getClen in VRMLC.pm.
sub cstruct () {"/*cstruct*/"}

sub VRMLFieldcopy {
	my($type, $value) = @_;

	if (!ref $value) {
		return $value
	} elsif (ref $value eq "VRML::USE") {
		#$value = $value->real_node(); return $value;
		my $ret = $value->copy();
		return $ret;
	} elsif (ref $value eq "ARRAY") {
		return [map {VRMLFieldcopy("",$_)} @$value]
	} elsif (ref $value eq "VRML::NodeIntern") {
		return $value;
	} elsif (ref $value eq "VRML::DEF") {
		##my $ret = [map {copy("",$_)} @$value];
		##bless $ret, "VRML::DEF";
		##return $ret;
		my $ret = bless { map {VRMLFieldcopy("",$_)} %$value }, "VRML::DEF";
		return $ret;
	} else {
		print "going to die;\n";
		print "copy ", ref $value, "\n";
		die("Can't copy this");
	}
}

sub as_string {"VRML::Field - can't print this!"}


###########################################################
package VRML::Field::SFFloat;
@ISA=VRML::Field;
VRML::Error->import();

sub VRMLFieldInit { return 0.0; }

sub parse {
	my($type,$p,$s,$n) = @_;
	$_[2] =~ /\G\s*($Float)/ogcs or
		parsefail($_[2], "didn't match SFFloat");
	return $1;
}

sub as_string { return sprintf("%.4g", $_[1]); }

sub print {print $_[1]}

sub ctype {"float $_[1]"}
sub clength {2} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}


###########################################################
package VRML::Field::SFTime;
@ISA=VRML::Field::SFFloat;

sub VRMLFieldInit {return 0;}

sub as_string { return sprintf("%f", $_[1]); }
sub ctype {"double $_[1]"}
sub clength {3} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}


###########################################################
package VRML::Field::FreeWRLPTR;
@ISA=VRML::Field;
VRML::Error->import;

sub VRMLFieldInit {return 0;}

sub parse {
	my($type,$p,$s,$n) = @_;
	$_[2] =~ /\G\s*($Integer)\b/ogsc
		or parsefail($_[2],"not proper FreeWRLPTR");
	return $1;
}

sub print {print " $_[1] "}
sub as_string {$_[1]}

sub ctype {return "void * $_[1]"}
sub clength {8} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}

###########################################################
###########################################################
package VRML::Field::SFInt32;
@ISA=VRML::Field;
VRML::Error->import;

sub VRMLFieldInit {return 0;}

sub parse {
	my($type,$p,$s,$n) = @_;
	$_[2] =~ /\G\s*($Integer)\b/ogsc
		or parsefail($_[2],"not proper SFInt32");
	return $1;
}

sub print {print " $_[1] "}
sub as_string {$_[1]}

sub ctype {return "int $_[1]"}
sub clength {1} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}


###########################################################
package VRML::Field::SFColor;
@ISA=VRML::Field;
VRML::Error->import;

sub VRMLFieldInit { return [0, 0, 0]; }

sub parse {
	my($type,$p) = @_;

	# eat comma if one is there (thanks, rasmol, for invalid VRML...)
	$_[2] =~ /\G\s*,\s*/gsc;

	$_[2] =~ /\G\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)/ogsc
	    or parsefail($_[2],"Didn't match SFColor");

	return [$1,$2,$3];
}

sub print {print join ' ',@{$_[1]}}
sub as_string {join ' ',@{$_[1]}}

sub cstruct {return "struct SFColor {
	float c[3]; };"}
sub ctype {return "struct SFColor $_[1]"}
sub clength {5} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	my ($this,$field,$val) = @_;
	return 	"$field.c[0] = @{$val}[0];".
		"$field.c[1] = @{$val}[1];".
		"$field.c[2] = @{$val}[2];";
}

###########################################################
package VRML::Field::SFColorRGBA;
@ISA=VRML::Field;
VRML::Error->import();
sub VRMLFieldInit { return [0, 0, 0, 1]; }
sub parse {
	my($type,$p) = @_;

	# first, look for 4 floats, can have commas
	$_[2] =~ /\G\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)/ogsc
		or VRML::Error::parsefail($_[2],"not proper rotation");
	return [$1,$2,$3,$4];
}

sub print {print join ' ',@{$_[1]}}
sub as_string {join ' ',@{$_[1]}}

sub cstruct {return "struct SFColorRGBA {
 	float r[4]; };"}

sub ctype {return "struct SFColorRGBA $_[1]"}
sub clength {7} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	print "SFCOLORRGBA INIT\n";
	return 0;
}

###########################################################
package VRML::Field::SFVec3f;
@ISA=VRML::Field::SFColor;
sub VRMLFieldInit {return [0, 0, 0];}
sub cstruct {return ""}

sub vec_add { join '',map {"$_[3].c[$_] = $_[1].c[$_] + $_[2].c[$_];"} 0..2; }
sub vec_subtract { join '',map {"$_[3].c[$_] = $_[1].c[$_] - $_[2].c[$_];"} 0..2; }
sub vec_negate { join '',map {"$_[2].c[$_] = -$_[1].c[$_];"} 0..2; }
sub vec_length { "$_[2] = sqrt(".(join '+',map {"$_[1].c[$_]*$_[1].c[$_]"} 0..2)
	.");"; }
sub vec_normalize { "{double xx = sqrt(".(join '+',map {"$_[1].c[$_]*$_[1].c[$_]"} 0..2)
	.");
	". (join '', map {"$_[2].c[$_] = $_[1].c[$_]/xx;"} 0..2)."}" }

sub vec_cross {
	"
		$_[3].c[0] =
			$_[1].c[1] * $_[2].c[2] -
			$_[2].c[1] * $_[1].c[2];
		$_[3].c[1] =
			$_[1].c[2] * $_[2].c[0] -
			$_[2].c[2] * $_[1].c[0];
		$_[3].c[2] =
			$_[1].c[0] * $_[2].c[1] -
			$_[2].c[0] * $_[1].c[1];
	"
}


sub cInitialize {
	my ($this,$field,$val) = @_;
	return 	"$field.c[0] = @{$val}[0];".
		"$field.c[1] = @{$val}[1];".
		"$field.c[2] = @{$val}[2];";
}

###########################################################
package VRML::Field::SFVec2f;
@ISA=VRML::Field;
VRML::Error->import();
sub VRMLFieldInit { return [0, 0]; }

sub parse {
	my($type,$p) = @_;

	$_[2] =~ /\G\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)/ogsc
	    or parsefail($_[2],"didn't match SFVec2f");
	return [$1,$2];
}

sub print {print join ' ',@{$_[1]}}
sub as_string {join ' ',@{$_[1]}}

sub cstruct {return "struct SFVec2f {
	float c[2]; };"}
sub ctype {return "struct SFVec2f $_[1]"}
sub clength {6} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	my ($this,$field,$val) = @_;
	return 	"$field.c[0] = @{$val}[0];".
		"$field.c[1] = @{$val}[1];";
}


###########################################################
package VRML::Field::SFRotation;
@ISA=VRML::Field;
VRML::Error->import();

sub VRMLFieldInit { return [0, 0, 1, 0]; }

sub parse {
	my($type,$p) = @_;

	# first, look for 4 floats, can have commas
	$_[2] =~ /\G\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)\s{0,},{0,1}\s{0,}($Float)/ogsc
		or VRML::Error::parsefail($_[2],"not proper rotation");
	return [$1,$2,$3,$4];
}

sub print {print join ' ',@{$_[1]}}
sub as_string {join ' ',@{$_[1]}}

sub cstruct {return "struct SFRotation {
 	float r[4]; };"}

sub ctype {return "struct SFRotation $_[1]"}
sub clength {4} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	my ($this,$field,$val) = @_;
	return 	"$field.r[0] = @{$val}[0];".
		"$field.r[1] = @{$val}[1];".
		"$field.r[2] = @{$val}[2];".
		"$field.r[3] = @{$val}[3];";
}


###########################################################
package VRML::Field::SFBool;
@ISA=VRML::Field;

sub VRMLFieldInit { return 0; }

sub parse {
	my($type,$p,$s,$n) = @_;
	$_[2] =~ /\G\s*(TRUE|FALSE)\b/gs
		or die "Invalid value for SFBool\n";
	if ($1 eq "TRUE") {return 1};
	return 0;
}

sub ctype {return "int $_[1]"}
sub clength {1} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub print {print ($_[1] ? TRUE : FALSE)}
sub as_string {
	my ($this, $bool, $as_ecmascript) = @_;
	if ($as_ecmascript) {
		return ($bool ? true : false);
	} else {
		return ($bool ? TRUE : FALSE);
	}
}

sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}


###########################################################
package VRML::Field::SFString;
@ISA=VRML::Field;

# Ayla... This is closer to the VRML97 specification,
# which allows anything but unescaped " or \ in SFStrings.

#$Chars = qr/(?:[\x00-\x21\x23-\x5b\x5d-\x7f]*|\x5c\x22|\x5c{2}[^\x22])*/o;
$Chars = qr/(?:[\x00-\x21\x23-\x5b\x5d-\xffff]*|\x5c\x22|\x5c{2}[^\x22])*/o;

sub VRMLFieldInit { return ""; }

# XXX Handle backslashes in string properly
sub parse {
	$_[2] =~ /\G\s*\x22($Chars)\x22\s*/g
		or VRML::Error::parsefail($_[2], "improper SFString");
	my $str = $1;
	## cleanup ms-dos/windows end of line chars
	$str =~ s///g;
	$str =~ s/\\(.)/$1/g;
	#print "sfstring str is $str\n";
	return $str;
}

sub ctype {return "SV *$_[1]"}

sub print {print "\"$_[1]\""}
sub as_string{"\"$_[1]\""}

sub clength {1} #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
	my ($this,$field,$val) = @_;

	if ($field eq "tmp2->__parenturl") {
		return "$field = EAI_newSVpv(getInputURL())";
	} else {
		return "$field = EAI_newSVpv(\"$val\")";
	}
}

###########################################################
package VRML::Field::MFString;
@ISA=VRML::Field::Multi;


sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	my $retstr;
	my $tmp;

	#print "MFSTRING field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		#print "MALLOC MFSTRING field $field val @{$val} has $count INIT\n";
		$retstr = $restsr . "$field.p = malloc (sizeof(float)*$count);";
		for ($tmp=0; $tmp<$count; $tmp++) {
			$retstr = $retstr .  "$field.p[$tmp] = EAI_newSVpv(\"".@{$val}[$tmp]."\");";
		}
		$retstr = $retstr . "$field.n=$count; ";
		
	} else {
		return "$field.n=0; $field.p=0";
	}
	return $retstr;
}

# XXX Should be optimized heavily! Other MFs are ok.

###########################################################
package VRML::Field::MFFloat;
@ISA=VRML::Field::Multi;

sub parse {
	my($type,$p) = @_;
	if($_[2] =~ /\G\s*\[\s*/gsc) {
		$_[2] =~ /\G([^\]]*)\]/gsc or
		 VRML::Error::parsefail($_[2],"unterminated MFFloat");
		my $a = $1;
		$a =~ s/^\s*//;
		$a =~ s/\s*$//;
		# XXX Errors ???
		my @a = split /\s*,\s*|\s+/,$a;
		pop @a if $a[-1] =~ /^\s+$/;
		# while($_[2] !~ /\G\s*,?\s*\]\s*/gsc) {
		# 	$_[2] =~ /\G\s*,\s*/gsc; # Eat comma if it is there...
		# 	my $v =  $stype->parse($p,$_[2],$_[3]);
		# 	push @a, $v if defined $v;
		# }
		return \@a;
	} else {
		my $res = [VRML::Field::SFFloat->parse($p,$_[2],$_[3])];
		# Eat comma if it is there
		$_[2] =~ /\G\s*,\s*/gsc;
		return $res;
	}
}

sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	my $retstr;
	my $tmp;

	#print "MFFLOAT field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		#print "MALLOC MFFLOAT field $field val @{$val} has $count INIT\n";
		$retstr = $restsr . "$field.p = malloc (sizeof(float)*$count);\n";
		for ($tmp=0; $tmp<$count; $tmp++) {
			$retstr = $retstr .  "$field.p[1] = @{$val}[tmp];";
		}
		$retstr = $retstr . "$field.n=$count; /*CHECKTHIS*/";
		
	} else {
		return "$field.n=0; $field.p=0";
	}
	return $retstr;
}


###########################################################
package VRML::Field::MFTime;
@ISA=VRML::Field::MFFloat;

sub cInitialize {
	print "MFTIME INIT\n";
}

###########################################################
package VRML::Field::MFVec3f;
@ISA=VRML::Field::Multi;

sub parse {
	my($type,$p) = @_;
	my $pos = pos $_[2];
	if($_[2] =~ /\G\s*\[\s*/gsc) {
		(pos $_[2]) = $pos;
		my $nums = VRML::Field::MFFloat->parse($p,$_[2]);
		my @arr;
		for(0..($#$nums-2)/3) {
			push @arr,[$nums->[$_*3], $nums->[$_*3+1], $nums->[$_*3+2]];
		}
		return \@arr;
	} else {
		my $res = [VRML::Field::SFVec3f->parse($p,$_[2],$_[3])];
		# Eat comma if it is there
		$_[2] =~ /\G\s*,\s*/gsc;
		return $res;
	}
}


sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	my $retstr;
	my $tmp;
	my $whichVal;

	#print "MFVEC3F field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		#print "MALLOC MFVEC3F field $field val @{$val} has $count INIT\n";

		$retstr = $restsr . "$field.p = malloc (sizeof(float)*3*$count);\n";
		for ($tmp=0; $tmp<$count; $tmp++) {
			my $arline = @{$val}[$tmp];
			for ($whichVal = 0; $whichVal < 3; $whichVal++) {
				$retstr = $retstr. "\n\t\t\t$field.p[$tmp*3+$whichVal] = ".@{$arline}[$whichVal]."; ";
			}
			$retstr = $retstr."$field.n=$count;";
		}



	} else {
		return "$field.n=0; $field.p=0";
	}
}

###########################################################
package VRML::Field::MFNode;
@ISA=VRML::Field::Multi;

sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	#print "MFNODE field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		print "MFNODE HAVE TO MALLOC HERE\n";
	} else {
		return "$field.n=0; $field.p=0";
	}
}


###########################################################
package VRML::Field::MFColor;
@ISA=VRML::Field::Multi;

sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	my $retstr;
	my $tmp;
	my $whichVal;

	#print "MFCOLOR field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		#print "MALLOC MFCOLOR field $field val @{$val} has $count INIT\n";

		$retstr = $restsr . "$field.p = malloc (sizeof(float)*3*$count);\n";
		for ($tmp=0; $tmp<$count; $tmp++) {
			my $arline = @{$val}[$tmp];
			for ($whichVal = 0; $whichVal < 3; $whichVal++) {
				$retstr = $retstr. "\n\t\t\t$field.p[$tmp*3+$whichVal] = ".@{$arline}[$whichVal]."; ";
			}
			$retstr = $retstr."$field.n=$count;";
		}



	} else {
		return "$field.n=0; $field.p=0";
	}
}

###########################################################
package VRML::Field::MFColorRGBA;
@ISA=VRML::Field::Multi;


sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	#print "MFCOLORRGBA field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		print "MALLOC MFCOLORRGBA field $field val @{$val} has $count INIT\n";
	} else {
		return "$field.n=0; $field.p=0";
	}
}

###########################################################
package VRML::Field::MFVec2f;
@ISA=VRML::Field::Multi;


sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	my $retstr;
	my $tmp;
	my $whichVal;

	#print "MFVec2F field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		#print "MALLOC MFVec2F field $field val @{$val} has $count INIT\n";

		$retstr = $restsr . "$field.p = malloc (sizeof(float)*2*$count);\n";
		for ($tmp=0; $tmp<$count; $tmp++) {
			my $arline = @{$val}[$tmp];
			for ($whichVal = 0; $whichVal < 2; $whichVal++) {
				$retstr = $retstr. "\n\t\t\t$field.p[$tmp*2+$whichVal] = ".@{$arline}[$whichVal]."; ";
			}
			$retstr = $retstr."$field.n=$count;";
		}



	} else {
		return "$field.n=0; $field.p=0";
	}
}

###########################################################
package VRML::Field::MFInt32;
@ISA=VRML::Field::Multi;

sub parse {
	my($type,$p) = @_;
	if($_[2] =~ /\G\s*\[\s*/gsc) {
		$_[2] =~ /\G([^\]]*)\]/gsc or
		 VRML::Error::parsefail($_[2],"unterminated MFFInt32");
		my $a = $1;
		$a =~ s/^\s*//s;
		$a =~ s/\s*$//s;
		# XXX Errors ???
		my @a = split /\s*,\s*|\s+/,$a;
		pop @a if $a[-1] =~ /^\s+$/;
		return \@a;
	} else {
		my $res = [VRML::Field::SFInt32->parse($p,$_[2],$_[3])];
		# Eat comma if it is there
		$_[2] =~ /\G\s*,\s*/gsc;
		return $res;
	}
}

sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	#print "MFINT32 field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		print "HAVE TO MALLOC HERE\n";
	} else {
		return "$field.n=0; $field.p=0";
	}
}

###########################################################
package VRML::Field::MFBool;
@ISA=VRML::Field::Multi;

sub parse {
	my($type,$p) = @_;
	if($_[2] =~ /\G\s*\[\s*/gsc) {
		$_[2] =~ /\G([^\]]*)\]/gsc or
		 VRML::Error::parsefail($_[2],"unterminated MFFBool");
		my $a = $1;
		$a =~ s/^\s*//s;
		$a =~ s/\s*$//s;
		# XXX Errors ???
		my @a = split /\s*,\s*|\s+/,$a;
		pop @a if $a[-1] =~ /^\s+$/;
		return \@a;
	} else {
		my $res = [VRML::Field::SFBool->parse($p,$_[2],$_[3])];
		# Eat comma if it is there
		$_[2] =~ /\G\s*,\s*/gsc;
		return $res;
	}
}



sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	my $retstr;
	my $tmp;

	#print "MFBOOL field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		#print "MALLOC MFBOOL field $field val @{$val} has $count INIT\n";

		$retstr = $restsr . "$field.p = malloc (sizeof(int)*$count);\n";
		for ($tmp=0; $tmp<$count; $tmp++) {
			my $arline = @{$val}[$tmp];
			$retstr = $retstr. "\n\t\t\t$field.p[$tmp] = $arline; ";
		}
		$retstr = $retstr."$field.n=$count;";



	} else {
		return "$field.n=0; $field.p=0";
	}
}

###########################################################
package VRML::Field::MFRotation;
@ISA=VRML::Field::Multi;


sub cInitialize {
	my ($this,$field,$val) = @_;
	my $count = @{$val};
	my $retstr;
	my $tmp;
	my $whichVal;

	#print "MFROTATION field $field val @{$val} has $count INIT\n";
	if ($count > 0) {
		#print "MALLOC MFROTATION field $field val @{$val} has $count INIT\n";

		$retstr = $restsr . "$field.p = malloc (sizeof(float)*4*$count);\n";
		for ($tmp=0; $tmp<$count; $tmp++) {
			my $arline = @{$val}[$tmp];
			for ($whichVal = 0; $whichVal < 4; $whichVal++) {
				$retstr = $retstr. "\n\t\t\t$field.p[$tmp*4+$whichVal] = ".@{$arline}[$whichVal]."; ";
			}
			$retstr = $retstr."$field.n=$count;";
		}



	} else {
		return "$field.n=0; $field.p=0";
	}
}

###########################################################
package VRML::Field::Multi;
@ISA=VRML::Field;

sub VRMLFieldInit { return []; }

sub ctype {
	my $r = (ref $_[0] or $_[0]);
	$r =~ s/VRML::Field::MF//;
	return "struct Multi_$r $_[1]";
}
sub clength {
	#for C routes. Keep in sync with getClen in VRMLC.pm.
	#clengths that are negative signal that something more than just a straight
	#copy for javascripting is required. Other than the fact that numbers are
	#negative, there is no symbolism placed on their values.

	my $r = (ref $_[0] or $_[0]);
	$r =~ s/VRML::Field::MF//;
	# these negative ones are returned in other places...
	# -11 = SFNode
	#JAS - lets try SFImages as SFStrings for now... # -12 = SFImage

	if ($r eq "String") {return -13;}	# signal that a clength -13 is a Multi_String for CRoutes
	if ($r eq "Float") {return -14;}	# signal that a clength -14 is a Multi_Float for CRoutes
	if ($r eq "Rotation") {return -15;}	# signal that a clength -15 is a Multi_Rotation for CRoutes
	if ($r eq "Int32") {return -16;}	# signal that a clength -16 is a Multi_Int32 for CRoutes
	if ($r eq "Color") {return -17;}	# signal that a clength -1  is a Multi_Color for CRoutes
	if ($r eq "Vec2f") {return -18;}	# signal that a clength -18 is a Multi_Vec2f for CRoutes
	if ($r eq "Vec3f") {return -19;} 	# signal that a clength -19  is a Multi_Vec3f for CRoutes
	if ($r eq "Node") {return -10;} 	# signal that a clength -10 is a Multi_Node for CRoutes

	print "clength struct not handled Multi_$r $_[1]\n";
	return 0;
}
sub cstruct {
	my $r = (ref $_[0] or $_[0]);
	my $t = $r;
	$r =~ s/VRML::Field::MF//;
	$t =~ s/::MF/::SF/;
	my $ct = $t->ctype;
	return "struct Multi_$r { int n; $ct *p; };"
}

sub parse {
	my($type,$p) = @_;
	my $stype = $type;
	$stype =~ s/::MF/::SF/;
	if($_[2] =~ /\G\s*\[\s*/gsc) {
		my @a;
		while($_[2] !~ /\G\s*,?\s*\]\s*/gsc) {
			# print "POS0: ",(pos $_[2]),"\n";
			# removing $r = causes this to be evaluated
			# in array context -> fail.
			# eat commas if they are there...
			my $r = ($_[2] =~ /\G\s*,+\s*/gsc);
			# my $wa = wantarray;
			# print "R: '$r' (WA: $wa)\n";
			# print "POS1: ",(pos $_[2]),"\n";
			my $v =  $stype->parse($p,$_[2],$_[3]);
			# print "POS2: ",(pos $_[2]),"\n";
			push @a, $v if defined $v;
		}
		return \@a;
	} else {
		my $res = [$stype->parse($p,$_[2],$_[3])];
		# eat commas if they are there
		my $r = $_[2] =~ /\G\s*,+\s*/gsc;
		return $res;
	}
}

sub print {
	my($type) = @_;
	print " [ ";
	my $r = $type;
	$r =~ s/::MF/::SF/;
	for(@{$_[1]}) {
		$r->print($_);
	}
	print " ]\n";
}

sub as_string {
	my ($type, $value, $as_ecmascript) = @_;
	$type =~ s/::MF/::SF/;

	if (!$value) { return; }

	if("ARRAY" eq ref $value) {
		return "[]" if (!@{$value});
		return "[ ".(join ' ', map {$type->as_string($_)} @{$value})." ]";
	} else {
		return $value->as_string();
	}
}



###########################################################
package VRML::Field::SFNode;

sub VRMLFieldInit { return "NULL"; }

sub VRMLFieldcopy { return $_[1] }

sub ctype {"void *$_[1]"}      # XXX ???
sub cstruct {""}

sub as_string {
	$_[1]->as_string();
}
sub clength {-11} #for C routes. Keep in sync with getClen in VRMLC.pm.



use vars qw/$Word/;
VRML::Error->import;


sub parse {
	my($type, $scene, $txt, $parentField) = @_;
	$_[2] =~ /\G\s*/gsc;

	if ($VRML::verbose::parse) {
		my ($package, $filename, $line) = caller;
		print "VRML::Field::SFNode::parse: ",
			(pos $_[2]), " ",
				length $_[2], " from $package, $line\n";
	}

	$_[2] =~ /\G\s*($Word)/ogsc or parsefail($_[2],"didn't match for sfnode fword");

	my $nt = $1;
	if($nt eq "NULL") {
		return "NULL";
	}
	my $vrmlname;
	my $is_name;
	my $p;
	my $rep_field;
	my $field_counter = 1;


	if($nt eq "DEF") {
		$_[2] =~ /\G\s*($Word)/ogsc or parsefail($_[2],
			"DEF must be followed by a defname");

		# store this as a sequence number, because multiple DEFS of the same name
		# must be unique. (see the spec)
		$vrmlname = $1;
		VRML::Handles::def_reserve($vrmlname);
		my $defname = VRML::Handles::return_def_name($vrmlname);
		print "Parser.pm: DEF $vrmlname as $defname\n"
			if $VRML::verbose::parse;

		my $node = VRML::Field::SFNode->parse($scene, $_[2],$parentField);
		print "DEF - node $defname is $node \n" if  $VRML::verbose::parse;

		return $scene->new_def($defname, $node, $vrmlname);

	}
	if($nt eq "USE") {
		$_[2] =~ /\G\s*($Word)/ogsc or parsefail($_[2],
			"USE must be followed by a defname");

		$vrmlname = $1;
		# is is already DEF'd???
		my $dn = VRML::Handles::return_def_name($vrmlname);
        	if(!defined $dn) {
			VRML::VRMLFunc::ConsoleMessage( "USE name $vrmlname not DEFined yet\n");
			goto PARSE_EXIT;
		}

		print "USE $dn\n"
			if $VRML::verbose::parse;
		return $scene->new_use($dn);
	}
	if($nt eq "Script") {
		print "SCRIPT!\n"
			if $VRML::verbose::parse;
		return VRML::Parser::parse_script($scene,$_[2]);
	}

	# verify that this node is ok for the parent field type.
	# my ($package, $filename, $line) = caller; print "--- sub parse, nt $nt parentfield $parentField from $package, $line\n";

	if ($parentField eq "") {
		my ($package, $filename, $line) = caller;
		print "VRML::Field::SFNode::parse: null parentField  ",
			(pos $_[2]), " ",
				length $_[2], " from $package, $line\n";
	} else {
		# is this a switch choice?
		if ($parentField eq "choice") {$parentField = "children";}

		# is this a LOD level?
		if ($parentField eq "level") {$parentField = "children";}

		# is this a PROTO? All valid children fields should work for proto top nodes.
		# see below... JAS #if ($parentField eq "protoTop") {$parentField = "children";}

		if ($VRML::Nodes::{$parentField}{$nt}) {
		        #print "node $nt is ok for a parentField of $parentField\n";
		} else {


##### THIS IS BROKEN FOR EXTERNPROTOS - JUST ASSUME ALL PROTOS ARE OK FOR NOW
			### my $okPROTO = 0;
			my $okPROTO = 1;

			# is this a proto?
			my $prot = $scene->get_proto($nt);
			if (defined $prot) {
				my $nodeszero = $prot->{Nodes}[0];
				my $firstchild=$nodeszero->{Fields}{children}[0];

				# we have the first child; resolve the def if required.
				if (ref $firstchild eq "VRML::DEF") {
					$firstchild = $firstchild->real_node();
				}

				my $childtype = $firstchild->{Type}{Name};

				#print "and, first chid of nodeszero is $firstchild\n";
				#print "and, first chid of nodeszero TYPE is ",$childtype,"\n";

				if ($VRML::Nodes::{$parentField}{$childtype}) {
					#print "child type $childtype IS OK! for $parentField\n";
					$okPROTO=1;
				}
			}

			# is this a proto top field? If so, just accept it, as when the
			# proto is used, the first node will get type checked.
			if ($parentField eq "protoTop") {
				$okPROTO = 1;
			}

			# is this a sub-texture of TextureBackground or some other texture/texture mapping
			my $pind = index($parentField,"Texture");
			my $cind = index ($nt,"Texture");
			if (($pind > 0) && ($cind>0)) { $okPROTO = 1;}

			# nope, it failed even the PROTO tests.
			if ($okPROTO == 0) {
				my ($package, $filename, $line) = caller;
        			VRML::VRMLFunc::ConsoleMessage("WARNING -- node $nt may not be ok as a ".
					"field of type $parentField\n(...called from $package, $line)");
			}
		}
	}


	my $proto;
	$p = pos $_[2];

	my $no = $VRML::Nodes{$nt};
	## look in PROTOs that have already been processed
	if (!defined $no) {
		$no = $scene->get_proto($nt);
		print "PROTO? '$no'\n"
			if $VRML::verbose::parse;
	}

	## next, let's try looking for EXTERNPROTOs in the file
	if (!defined $no) {
		## return to the beginning
		pos $_[2] = 0;
		VRML::Parser::parse_externproto($scene, $_[2], $nt);

		## reset position and try looking for PROTO nodes again
		pos $_[2] = $p;
		$no = $scene->get_proto($nt);
		print "PROTO? '$no'\n"
			if $VRML::verbose::parse;
	}

	if (!defined $no) {
		parsefail($_[2],"Invalid node '$nt'");
	}

	$proto=1;
	print "Match: '$nt'\n"
		if $VRML::verbose::parse;

	$_[2] =~ /\G\s*{\s*/gsc or parsefail($_[2],"didn't match brace!\n");
	my $isscript = ($nt eq "Script");
	my %f;
	while(1) {
		while(VRML::Parser::parse_statement($scene,$_[2],1,$parentField)
			!= -1) {}; # check for PROTO & co
		last if ($_[2] =~ /\G\s*}\s*/gsc);
		print "Pos: ",(pos $_[2]),"\n"
			if $VRML::verbose::parse;
		# Apparently, some people use it :(
		$_[2] =~ /\G\s*,\s*/gsc and parsewarnstd($_[2], "Comma not really right");

		$_[2] =~ /\G\s*($Word)\s*/gsc or parsefail($_[2],"Node body","field name not found");
		print "FIELD: '$1'\n"
			if $VRML::verbose::parse;

		my $f = VRML::Parser::parse_exposedField($1, $no);

		# X3DV - some changes to the spec - make things ok here...
		# some nodes have same fields as VRML97, but different name.
		if (($nt eq "LOD") && ($f eq "children")) {
			$f = level; # VRML97, children
		}
		if (($nt eq "Switch") && ($f eq "children")) {
			$f = choice; # VRML97, children
		}


		my $ft = $no->{FieldTypes}{$f};
		print "FT: $ft\n"
			if $VRML::verbose::parse;
		if(!defined $ft) {
			my $em = "Invalid field '$f' for node '$nt'\n";
			$em = $em. "Possible fields are: ";
			foreach (keys % {$no->{FieldTypes}}) {
				if (index($_,"_") !=0) {$em= $em. "$_ ";}
			}
			$em = $em. "\n";

			VRML::VRMLFunc::ConsoleMessage ($em);
			goto PARSE_EXIT;
		}

		# JAS print "checking if this $nt field $f is a ComposedGeom node\n";
		if (defined $VRML::Nodes::X3DFaceGeometry{$nt}) {
			# JAS print "it IS a composedGeom node\n";

			my $ok = 1;

			# check field exists matrix. Must be a better way to code this.
			if ($nt eq "IndexedFaceSet") {
			    if (!defined $VRML::Nodes::X3DCG_IndexedFaceSet{$f}) {
				$ok = 0;
			    }
			} elsif ($nt eq "ElevationGrid") {
			    if (!defined $VRML::Nodes::X3DCG_ElevationGrid{$f}) {
				$ok = 0;
			    }
			} elsif ($nt eq "IndexedTriangleFanSet") {
			    if (!defined $VRML::Nodes::X3DCG_IndexedTriangleFanSet{$f}) {
				$ok = 0;
			    }
			} elsif ($nt eq "IndexedTriangleSet") {
			    if (!defined $VRML::Nodes::X3DCG_IndexedTriangleSet{$f}) {
				$ok = 0;
			    }
			} elsif ($nt eq "IndexedTriangleStripSet") {
			    if (!defined $VRML::Nodes::X3DCG_IndexedTriangleStripSet{$f}) {
				$ok = 0;
			    }
			} elsif ($nt eq "TriangleFanSet") {
			    if (!defined $VRML::Nodes::X3DCG_TriangleFanSet{$f}) {
				$ok = 0;
			    }
			} elsif ($nt eq "TriangleStripSet") {
			    if (!defined $VRML::Nodes::X3DCG_TriangleStripSet{$f}) {
				$ok = 0;
			    }
			} elsif ($nt eq "TriangleSet") {
			    if (!defined $VRML::Nodes::X3DCG_TriangleSet{$f}) {
				$ok = 0;
			    }
			}
			if ($ok ==0) {
				VRML::VRMLFunc::ConsoleMessage ("Invalid field $f for node $nt");
				goto PARSE_EXIT;
			}
		}	

		# the following lines return something like:
		# 	Storing values... SFInt32 for node VNetInfo, f port
		#       storing type 2, port, (8888)

		print "Storing values... $ft for node $nt, f $f\n"
			 if $VRML::verbose::parse;

		if($_[2] =~ /\G\s*IS\s+($Word)/gsc) {
			$is_name = $1;

			# Allow multiple IS statements for a single field in a node in
			# a prototype definition.
			# Prepending digits to the field name should be safe, since legal
			# VRML names may not begin with numerical characters.
			#
			# See NIST test Misc, PROTO, #19 (30eventouts.wrl) as example.
			if (exists $f{$f}) {
				$rep_field = ++$field_counter.$f;
				print "VRML::Field::SFNode::parse: an IS for $ft $f exists, try $rep_field.\n"
					if $VRML::verbose::parse;
				$no->{FieldTypes}{$rep_field} = $no->{FieldTypes}{$f};
				$no->{FieldKinds}{$rep_field} = $no->{FieldKinds}{$f};
				$no->{Defaults}{$rep_field} = $no->{Defaults}{$f};

				if (exists $no->{EventIns}{$f}) {
					$no->{EventIns}{$rep_field} = $rep_field;
				}

				if (exists $no->{EventOuts}{$f}) {
					$no->{EventOuts}{$rep_field} = $rep_field;
				}

				$f{$rep_field} = $scene->new_is($is_name, $rep_field);
			} else {
				$f{$f} = $scene->new_is($is_name, $f);
			}
			print "storing type 1, $f, (name ",
				VRML::Debug::toString($f{$f}), ")\n"
						if $VRML::verbose::parse;
		} else {
			$f{$f} = "VRML::Field::$ft"->parse($scene,$_[2],$f);
				print "storing type 2, $f, (",
					VRML::NodeIntern::dump_name($f{$f}), ")\n"
						if $VRML::verbose::parse;
		}
	}
	print "END\n"
		if $VRML::verbose::parse;
	return $scene->new_node($nt, \%f);
}


sub print {
	my($typ, $this) = @_;
	if($this->{Type}{Name} eq "DEF") {
		print "DEF $this->{Fields}{id} ";
		$this->{Type}{Fields}{node}->print($this->{Fields}{node});
		return;
	}
	if($this->{Type}{Name} eq "USE") {
		print "USE $this->{Fields}{id} ";
		return;
	}
	print "$this->{Type}{Name} {";
	for(keys %{$this->{Fields}}) {
		print "$_ ";
		$this->{Type}{Fields}{$_}->print($this->{Fields}{$_});
		print "\n";
	}
	print "}\n";
}

sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}

###########################################################
package VRML::Field::SFImage;
@ISA=VRML::Field;
VRML::Error->import;

sub VRMLFieldInit { return [0, 0, 0];}

# parse all possible numbers up to a }, watching that one does not parse
# in the first couple of characters of "field" or "eventIn" as might happen
# in a PROTO def in VRML.

sub parse {
	my($type,$p) = @_;

	$SFHEXmageChars = qr~[a-fA-FxX\d]+~;
	$SFImageChars = qr~[,+\- \n\t\s\d]+~;
	my $retstr = "";

	my $keepmatching = 1;

	while ($keepmatching == 1){
		# match numbers, spaces, newlines, etc.

		# Match. If we did not get anything, just return what we have.
		$_[2] =~ /\G\s*($SFImageChars)\s*/gc
			or return $retstr;

		# we did get something, append it to what we already have.
		$retstr = $retstr.$1;

		# now, is this in the middle of a hex char?
		$_[2] =~ /\G\s*([xX])\s*/gc;

		# could we possbly be at a hex number? If so, match hex digits.
		if (($1 eq "x")|($1 eq "X")) {
			$retstr = $retstr.$1;
			$_[2] =~ /\G\s*($SFHEXmageChars)\s*/gc;
			#print " in X, just matched $1\n";
			$retstr = $retstr.$1." ";
			#print "retstr now $retstr\n";
		} else {
			# most likely we got to a "field" or something else that looks hexidecimalish
			#print "did not match an x\n";
			$keepmatching = 3;
		}
	}
  return $retstr;
}

sub ctype {return "SV *$_[1]"}

sub print {print "\"$_[1]\""}
sub as_string{"\"$_[1]\""}

sub clength {-12}; # signal that a -12 is a SFImage for CRoutes #for C routes. Keep in sync with getClen in VRMLC.pm.

sub cInitialize {
        my ($this,$field,$val) = @_;
        return "$field = EAI_newSVpv(\"$val\")";

# SFImages should return a pointer to a Multi_Int32, not SV, if using Daniel Kraft's parser
#return "if (useExperimentalParser) { \n" .
#		"int initf[] = {0,0,0}; \n".
#		"$field = malloc (sizeof (struct Multi_Int32*));\n" .
#		"((struct Multi_Int32*)tmp2)->n = 3; \n".
#		"((struct Multi_Int32*)tmp2)->p = malloc (sizeof(int) *3);\n".
#		"memcpy(((struct Multi_Int32*)tmp2)->p,initf,sizeof(int)*3);\n".
#	" } else $field = EAI_newSVpv(\"$val\")\n"; 
}

1;

