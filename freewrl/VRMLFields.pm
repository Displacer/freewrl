#
# $Id: VRMLFields.pm,v 1.73 2006/10/23 18:28:11 crc_canada Exp $
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
# Revision 1.73  2006/10/23 18:28:11  crc_canada
# More changes and code cleanups.
#
# Revision 1.72  2006/10/19 18:28:46  crc_canada
# More changes for removing Perl from the runtime
#
# Revision 1.71  2006/10/18 20:22:43  crc_canada
# More removal of Perl code
#
# Revision 1.70  2006/08/18 17:47:37  crc_canada
# Javascript initialization
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

# The C type interface for the field type, encapsulated
# By encapsulating things well enough, we'll be able to completely
# change the interface later, e.g. to fit together with javascript etc.
sub ctype ($) {my ($type) = @_; die "VRML::Field::ctype - fori $type abstract function called"}
sub cstruct () {"/*cstruct*/"}

###########################################################
package VRML::Field::SFFloat;
@ISA=VRML::Field;
VRML::Error->import();

sub ctype {"float $_[1]"}
sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}


###########################################################
package VRML::Field::SFTime;
@ISA=VRML::Field::SFFloat;


sub ctype {"double $_[1]"}
sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}


###########################################################
package VRML::Field::FreeWRLPTR;
@ISA=VRML::Field;
VRML::Error->import;



sub ctype {return "void * $_[1]"}
sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}

###########################################################
###########################################################
package VRML::Field::SFInt32;
@ISA=VRML::Field;
VRML::Error->import;



sub ctype {return "int $_[1]"}
sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}


###########################################################
package VRML::Field::SFColor;
@ISA=VRML::Field;
VRML::Error->import;



sub cstruct {return "struct SFColor {
	float c[3]; };"}
sub ctype {return "struct SFColor $_[1]"}
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


sub cstruct {return "struct SFColorRGBA { float r[4]; };"}
sub ctype {return "struct SFColorRGBA $_[1]"}
sub cInitialize {
	print "SFCOLORRGBA INIT\n";
	return 0;
}

###########################################################
package VRML::Field::SFVec3f;
@ISA=VRML::Field::SFColor;
sub cstruct {return ""}
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

sub cstruct {return "struct SFVec2f {
	float c[2]; };"}
sub ctype {return "struct SFVec2f $_[1]"}
sub cInitialize {
	my ($this,$field,$val) = @_;
	return 	"$field.c[0] = @{$val}[0];".
		"$field.c[1] = @{$val}[1];";
}


###########################################################
package VRML::Field::SFRotation;
@ISA=VRML::Field;
VRML::Error->import();


sub cstruct {return "struct SFRotation {
 	float r[4]; };"}

sub ctype {return "struct SFRotation $_[1]"}
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


sub ctype {return "int $_[1]"}
sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}


###########################################################
package VRML::Field::SFString;
@ISA=VRML::Field;

sub ctype {return "struct Uni_String *$_[1]"}
sub cInitialize {
	my ($this,$field,$val) = @_;

	if ($field eq "tmp2->__parenturl") {
		return "$field = newASCIIString(getInputURL())";
	} else {
		return "$field = newASCIIString(\"$val\")";
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
			$retstr = $retstr .  "$field.p[$tmp] = newASCIIString(\"".@{$val}[$tmp]."\");";
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
			$retstr = $retstr .  "$field.p[$tmp] = @{$val}[tmp];";
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


sub ctype {
	my $r = (ref $_[0] or $_[0]);
	$r =~ s/VRML::Field::MF//;
	return "struct Multi_$r $_[1]";
}
sub cstruct {
	my $r = (ref $_[0] or $_[0]);
	my $t = $r;
	$r =~ s/VRML::Field::MF//;
	$t =~ s/::MF/::SF/;
	my $ct = $t->ctype;
	return "struct Multi_$r { int n; $ct *p; };"
}


###########################################################
package VRML::Field::SFNode;


sub ctype {"void *$_[1]"}      # XXX ???
sub cstruct {""}

sub cInitialize {
	my ($this,$field,$val) = @_;
	return "$field = $val";
}

###########################################################
package VRML::Field::SFImage;
@ISA=VRML::Field;
VRML::Error->import;


sub ctype {return "struct Uni_String *$_[1]"}

sub cInitialize {
        my ($this,$field,$val) = @_;
        return "$field = newASCIIString(\"$val\")";

}

1;

