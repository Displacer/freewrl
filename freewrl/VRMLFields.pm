#
# $Id: VRMLFields.pm,v 1.16 2002/05/01 15:12:24 crc_canada Exp $
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
	SFInt32
	MFInt32
	SFNode
	MFNode
	SFColor
	MFColor
	SFTime
	SFString
	MFString
	SFVec2f
	MFVec2f
	SFImage
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
sub calloc ($$) {""}
sub cassign ($$) {"$_[1] = $_[2];"}
sub cfree ($) {if($_[0]->calloc) {return "free($_[1]);"} return ""}
sub cget {if(!defined $_[2]) {return "$_[1]"}
	else {die "If CGet with indices, abstract must be overridden"} }
sub cstruct () {""}
sub cfunc {die("Must overload cfunc")}
sub jsimpleget {return {}}

sub copy {
	my($type, $value) = @_;
	if(!ref $value) {return $value}
	if(ref $value eq "ARRAY") {
		return [map {copy("",$_)} @$value]
	}
	if(ref $value eq "VRML::Node") {
		return $value;
	}
	if(ref $value eq "VRML::DEF") {
		my $ret = [map {copy("",$_)} @$value];
		bless $ret, "VRML::DEF";
		return $ret;
	}
	die("Can't copy this");
}

sub as_string {"VRML::Field - can't print this!"}


###########################################################
package VRML::Field::SFFloat;
@ISA=VRML::Field;
VRML::Error->import();

sub parse {
	my($type,$p,$s,$n) = @_;
	$_[2] =~ /\G\s*($Float)/ogcs or 
		parsefail($_[2], "didn't match SFFloat");
	return $1;
}

sub as_string {$_[1]}

sub print {print $_[1]}

sub ctype {"float $_[1]"}
sub cfunc {"$_[1] = SvNV($_[2]);\n"}


###########################################################
package VRML::Field::SFTime;
@ISA=VRML::Field::SFFloat;


###########################################################
package VRML::Field::SFInt32;
@ISA=VRML::Field;
VRML::Error->import;

sub parse {
	my($type,$p,$s,$n) = @_;
	$_[2] =~ /\G\s*($Integer)\b/ogsc 
		or parsefail($_[2],"not proper SFInt32");
	return $1;
}

sub print {print " $_[1] "}
sub as_string {$_[1]}

sub ctype {return "int $_[1]"}
sub cfunc {return "$_[1] = SvIV($_[2]);\n"}


###########################################################
package VRML::Field::SFColor;
@ISA=VRML::Field;
VRML::Error->import;

sub parse {
	my($type,$p) = @_;

	$_[2] =~ /\G\s*($Float)\s+($Float)\s+($Float)/ogsc
	    or $_[2] =~ /\G\s*($Float),\s+($Float),\s+($Float)/ogsc 
	    or parsefail($_[2],"Didn't match SFColor");

	return [$1,$2,$3];
}

sub print {print join ' ',@{$_[1]}}
sub as_string {join ' ',@{$_[1]}}

sub cstruct {return "struct SFColor {
	float c[3]; };"}
sub ctype {return "struct SFColor $_[1]"}
sub cget {return "($_[1].c[$_[2]])"}

sub cfunc {
#	return ("a,b,c","float a;\nfloat b;\nfloat c;\n",
#		"$_[1].c[0] = a; $_[1].c[1] = b; $_[1].c[2] = c;");
	return "{
		AV *a;
		SV **b;
		int i;
		if(!SvROK($_[2])) {
			$_[1].c[0] = 0;
			$_[1].c[1] = 0;
			$_[1].c[2] = 0;
			/* die(\"Help! SFColor without being ref\"); */
		} else {
			if(SvTYPE(SvRV($_[2])) != SVt_PVAV) {
				die(\"Help! SFColor without being arrayref\");
			}
			a = (AV *) SvRV($_[2]);
			for(i=0; i<3; i++) {
				b = av_fetch(a, i, 1); /* LVal for easiness */
				if(!b) {
					die(\"Help: SFColor b == 0\");
				}
				$_[1].c[i] = SvNV(*b);
			}
		}
	}
	"
}


# javascript

sub jsprop {
	return '{"r", 0, JSPROP_ENUMERATE},{"g", 1, JSPROP_ENUMERATE},
		{"b", 2, JSPROP_ENUMERATE}'
}
sub jsnumprop {
	return { map {($_ => "$_[1].c[$_]")} 0..2 }
}
sub jstostr {
	return "
		{static char buff[250];
		 sprintf(buff,\"\%f \%f \%f\", $_[1].c[0], $_[1].c[1], $_[1].c[2]);
		 \$RET(buff);
		}
	"
}
sub jscons {
	return [
		"jsdouble pars[3];",
		"d d d",
		"&(pars[0]),&(pars[1]),&(pars[2])",
		"$_[1].c[0] = pars[0]; $_[1].c[1] = pars[1]; $_[1].c[2] = pars[2];",
		# Last: argless
		"$_[1].c[0] = 0; $_[1].c[1] = 0; $_[1].c[2] = 0;",
	];
}

sub js_default {
	return "new SFColor(0,0,0)"
}


###########################################################
package VRML::Field::SFVec3f;
@ISA=VRML::Field::SFColor;
sub cstruct {return ""}

sub jsprop {
	return '{"x", 0, JSPROP_ENUMERATE},{"y", 1, JSPROP_ENUMERATE},
		{"z", 2, JSPROP_ENUMERATE}'
}
sub js_default {
	return "new SFVec3f(0,0,0)"
}

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


###########################################################
package VRML::Field::SFVec2f;
@ISA=VRML::Field;
VRML::Error->import();

sub parse {
	my($type,$p) = @_;

	$_[2] =~ /\G\s*($Float)\s+($Float)/ogsc
	    or $_[2] =~ /\G\s*($Float),\s+($Float)/ogsc
	    or parsefail($_[2],"didn't match SFVec2f");
	return [$1,$2];
}

sub print {print join ' ',@{$_[1]}}
sub as_string {join ' ',@{$_[1]}}

sub cstruct {return "struct SFVec2f {
	float c[2]; };"}
sub ctype {return "struct SFVec2f $_[1]"}
sub cget {return "($_[1].c[$_[2]])"}

sub cfunc {
	return "{
		AV *a;
		SV **b;
		int i;
		if(!SvROK($_[2])) {
			$_[1].c[0] = 0;
			$_[1].c[1] = 0;
			/* die(\"Help! SFVec2f without being ref\"); */
		} else {
			if(SvTYPE(SvRV($_[2])) != SVt_PVAV) {
				die(\"Help! SFVec2f without being arrayref\");
			}
			a = (AV *) SvRV($_[2]);
			for(i=0; i<2; i++) {
				b = av_fetch(a, i, 1); /* LVal for easiness */
				if(!b) {
					die(\"Help: SFColor b == 0\");
				}
				$_[1].c[i] = SvNV(*b);
			}
		}
	}
	"
}


###########################################################
package VRML::Field::SFRotation;
@ISA=VRML::Field;
VRML::Error->import();

sub parse {
	my($type,$p) = @_;

	# first, look for 4 floats, NO COMMAS.
	my $count=($_[2] =~ /\G\s*($Float)\s+($Float)\s+($Float)\s+($Float)/ogsc );

	# XXX - Verify second, did not parse, maybe the user put commas in?
	if (!$count) {
	    $_[2] =~ /\G\s*($Float),\s+($Float),\s+($Float),\s+($Float)/ogsc 
		or VRML::Error::parsefail($_[2],"not proper rotation");
	}
	return [$1,$2,$3,$4];
}

sub print {print join ' ',@{$_[1]}}
sub as_string {join ' ',@{$_[1]}}

sub cstruct {return "struct SFRotation {
 	float r[4]; };"}

sub rot_invert {
	"
	 $_[2].r[0] = $_[1].r[0];
	 $_[2].r[1] = $_[1].r[1];
	 $_[2].r[2] = $_[1].r[2];
	 $_[2].r[3] = -$_[1].r[3];
	"
}

$VRML::Field::avecmacros = "
#define AVECLEN(x) (sqrt((x)[0]*(x)[0]+(x)[1]*(x)[1]+(x)[2]*(x)[2]))
#define AVECPT(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define AVECCP(x,y,z)   (z)[0]=(x)[1]*(y)[2]-(x)[2]*(y)[1]; \\
			(z)[1]=(x)[2]*(y)[0]-(x)[0]*(y)[2]; \\
			(z)[2]=(x)[0]*(y)[1]-(x)[1]*(y)[0];
#define AVECSCALE(x,y) x[0] *= y; x[1] *= y; x[2] *= y;
";

sub rot_multvec {
	qq~
		double rl = AVECLEN($_[1].r);
		double vl = AVECLEN($_[2].c);
		/* Unused JAS double rlpt = AVECPT($_[1].r, $_[2].c) / rl / vl; */
		float c1[3];
		float c2[3];
		double s = sin($_[1].r[3]), c = cos($_[1].r[3]);
		AVECCP($_[1].r,$_[2].c,c1); AVECSCALE(c1, 1.0 / rl );
		AVECCP($_[1].r,c1,c2); AVECSCALE(c2, 1.0 / rl) ;
		$_[3].c[0] = $_[2].c[0] + s * c1[0] + (1-c)*c2[0];
		$_[3].c[1] = $_[2].c[1] + s * c1[1] + (1-c)*c2[1];
		$_[3].c[2] = $_[2].c[2] + s * c1[2] + (1-c)*c2[2];
		/*
		printf("ROT MULTVEC (%f %f %f : %f) (%f %f %f) -> (%f %f %f)\\n",
			$_[1].r[0], $_[1].r[1], $_[1].r[2], $_[1].r[3],
			$_[2].c[0], $_[2].c[1], $_[2].c[2],
			$_[3].c[0], $_[3].c[1], $_[3].c[2]);
		*/
	~
}

sub ctype {return "struct SFRotation $_[1]"}
sub cget {return "($_[1].r[$_[2]])"}

sub cfunc {
#	return ("a,b,c,d","float a;\nfloat b;\nfloat c;\nfloat d;\n",
#		"$_[1].r[0] = a; $_[1].r[1] = b; $_[1].r[2] = c; $_[1].r[3] = d;");
	return "{
		AV *a;
		SV **b;
		int i;
		if(!SvROK($_[2])) {
			$_[1].r[0] = 0;
			$_[1].r[1] = 1;
			$_[1].r[2] = 0;
			$_[1].r[3] = 0;
			/* die(\"Help! SFRotation without being ref\"); */
		} else {
			if(SvTYPE(SvRV($_[2])) != SVt_PVAV) {
				die(\"Help! SFRotation without being arrayref\");
			}
			a = (AV *) SvRV($_[2]);
			for(i=0; i<4; i++) {
				b = av_fetch(a, i, 1); /* LVal for easiness */
				if(!b) {
					die(\"Help: SFColor b == 0\");
				}
				$_[1].r[i] = SvNV(*b);
			}
		}
	}
	"
}

sub jsprop {
	return '{"x", 0, JSPROP_ENUMERATE},{"y", 1, JSPROP_ENUMERATE},
		{"z", 2, JSPROP_ENUMERATE},{"angle",3, JSPROP_ENUMERATE}'
}
sub jsnumprop {
	return { map {($_ => "$_[1].r[$_]")} 0..3 }
}
sub jstostr {
	return "
		{static char buff[250];
		 sprintf(buff,\"\%f \%f \%f \%f\", $_[1].r[0], $_[1].r[1], $_[1].r[2], $_[1].r[3]);
		 \$RET(buff);
		}
	"
}
sub jscons {
return ["",qq~
	jsdouble pars[4];
	JSObject *ob1;
	JSObject *ob2;
	if(JS_ConvertArguments(cx,argc,argv,"d d d d",
		&(pars[0]),&(pars[1]),&(pars[2]),&(pars[3])) == JS_TRUE) {
		$_[1].r[0] = pars[0]; 
		$_[1].r[1] = pars[1]; 
		$_[1].r[2] = pars[2]; 
		$_[1].r[3] = pars[3];
	} else if(JS_ConvertArguments(cx,argc,argv,"o o",
		&ob1,&ob2) == JS_TRUE) {
		TJL_SFVec3f *vec1;
		TJL_SFVec3f *vec2;
		double v1len, v2len, v12dp;
		    if (!JS_InstanceOf(cx, ob1, &cls_SFVec3f, argv)) {
			die("sfrot obj: has to be SFVec3f ");
			return JS_FALSE;
		    }
		    if (!JS_InstanceOf(cx, ob2, &cls_SFVec3f, argv)) {
			die("sfrot obj: has to be SFVec3f ");
			return JS_FALSE;
		    }
		vec1 = JS_GetPrivate(cx,ob1);
		vec2 = JS_GetPrivate(cx,ob2);
		v1len = sqrt( vec1->v.c[0] * vec1->v.c[0] + 
			vec1->v.c[1] * vec1->v.c[1] + 
			vec1->v.c[2] * vec1->v.c[2] );
		v2len = sqrt( vec2->v.c[0] * vec2->v.c[0] + 
			vec2->v.c[1] * vec2->v.c[1] + 
			vec2->v.c[2] * vec2->v.c[2] );
		v12dp = vec1->v.c[0] * vec2->v.c[0] + 
			vec1->v.c[1] * vec2->v.c[1] + 
			vec1->v.c[2] * vec2->v.c[2] ;
		$_[1].r[0] = 
			vec1->v.c[1] * vec2->v.c[2] - 
			vec2->v.c[1] * vec1->v.c[2];
		$_[1].r[1] = 
			vec1->v.c[2] * vec2->v.c[0] - 
			vec2->v.c[2] * vec1->v.c[0];
		$_[1].r[2] = 
			vec1->v.c[0] * vec2->v.c[1] - 
			vec2->v.c[0] * vec1->v.c[1];
		v12dp /= v1len * v2len;
		$_[1].r[3] = 
			atan2(sqrt(1-v12dp*v12dp),v12dp);
		/* 
		printf("V12cons: (%f %f %f) (%f %f %f) %f %f %f (%f %f %f : %f)\n",
			vec1->v.c[0], vec1->v.c[1], vec1->v.c[2],
			vec2->v.c[0], vec2->v.c[1], vec2->v.c[2],
			v1len, v2len, v12dp, 
			$_[1].r[0], $_[1].r[1], $_[1].r[2], $_[1].r[3]);
		*/
	} else if(JS_ConvertArguments(cx,argc,argv,"o d",
		&ob1,&(pars[0])) == JS_TRUE) {
		TJL_SFVec3f *vec;
		    if (!JS_InstanceOf(cx, ob1, &cls_SFVec3f, argv)) {
			die("multVec: has to be SFVec3f ");
			return JS_FALSE;
		    }
		vec = JS_GetPrivate(cx,ob1);
		$_[1].r[0] = vec->v.c[0]; 
		$_[1].r[1] = vec->v.c[1]; 
		$_[1].r[2] = vec->v.c[2]; 
		$_[1].r[3] = pars[0];
		
	} else if(argc == 0) {
		$_[1].r[0] = 0;
		$_[1].r[0] = 0;
		$_[1].r[0] = 1;
		$_[1].r[0] = 0;
	} else {
		die("Invalid constructor for SFRotation");
	}

~];
}

sub js_default {
	return "new SFRotation(0,0,1,0)"
}


###########################################################
package VRML::Field::SFBool;
@ISA=VRML::Field;

sub parse {
	my($type,$p,$s,$n) = @_;
	$_[2] =~ /\G\s*(TRUE|FALSE)\b/gs or die "Invalid value for BOOL\n";
	return ($1 eq "TRUE");
}

sub ctype {return "int $_[1]"}
sub cget {return "($_[1])"}
sub cfunc {return "$_[1] = SvIV($_[2]);\n"}

sub print {print ($_[1] ? TRUE : FALSE)}
sub as_string {($_[1] ? TRUE : FALSE)}


###########################################################
package VRML::Field::SFString;
@ISA=VRML::Field;

# Ayla... This is closer to the VRML97 specification,
# which allows anything but unescaped " or \ in SFStrings.
#
# The qr// operator enables more regex processing at
# compile time.
# The Perl logical operators are less greedy than
# the regex alternation operator, so this is more
# efficient.
$SFStringChars = qr/(?ixs:[^\"\\])*/||qr/(?ixs:\\[\"\\])*/;

# XXX Handle backslashes in string properly
sub parse {
###my($type,$p,$s,$n) = @_; -- NOT USED
        # Magic regexp which hopefully exactly quotes backslashes and quotes
        # Remi... $_[2] =~ /\G\s*"((?:[^"\\]|\\.)*)"\s*/gsc
        # $_[2] =~ /\G\s*\"((?:[^\"\\]|\\.)*)\"\s*/gsc
	$_[2] =~ /\G\s*\"($SFStringChars)\"\s*/gc
                or VRML::Error::parsefail($_[2], "improper SFString");
	## cleanup ms-dos/windows end of line chars
        my $str = $1;
        $str =~ s/\\(.)/$1/g;
        return $str;
}

sub ctype {return "SV *$_[1]"}
sub calloc {"$_[1] = newSVpv(\"\",0);"}
sub cassign {"sv_setsv($_[1],$_[2]);"}
sub cfree {"SvREFCNT_dec($_[1]);"}
sub cfunc {"sv_setsv($_[1],$_[2]);"}

sub print {print "\"$_[1]\""}
sub as_string{"\"$_[1]\""}


###########################################################
package VRML::Field::MFString;
@ISA=VRML::Field::Multi;



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


###########################################################
package VRML::Field::MFNode;
@ISA=VRML::Field::Multi;


###########################################################
package VRML::Field::MFColor;
@ISA=VRML::Field::Multi;


###########################################################
package VRML::Field::MFVec2f;
@ISA=VRML::Field::Multi;


###########################################################
package VRML::Field::MFInt32;
@ISA=VRML::Field::Multi;

sub parse {
	my($type,$p) = @_;
	if($_[2] =~ /\G\s*\[\s*/gsc) {
		$_[2] =~ /\G([^\]]*)\]/gsc or
		 VRML::Error::parsefail($_[2],"unterminated MFFloat");
		my $a = $1;
		$a =~ s/^\s*//s;
		$a =~ s/\s*$//s;
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
		my $res = [VRML::Field::SFInt32->parse($p,$_[2],$_[3])];
		# Eat comma if it is there
		$_[2] =~ /\G\s*,\s*/gsc;
		return $res;
	}
}


###########################################################
package VRML::Field::MFRotation;
@ISA=VRML::Field::Multi;


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
sub calloc {
	return "$_[1].n = 0; $_[1].p = 0;";
}
sub cassign {
	my $t = (ref $_[0] or $_[0]);
	$t =~ s/::MF/::SF/;
	my $cm = $t->calloc("$_[1].n");
	my $ca = $t->cassign("$_[1].p[__i]", "$_[2].p[__i]");
	"if($_[1].p) {free($_[1].p)};
	 $_[1].n = $_[2].n; $_[1].p = malloc(sizeof(*($_[1].p))*$_[1].n);
	 {int __i;
	  for(__i=0; __i<$_[1].n; __i++) {
	  	$cm
		$ca
	  }
	 }
	"
}
sub cfree {
	"if($_[1].p) {free($_[1].p);$_[1].p=0;} $_[1].n = 0;"
}
sub cgetn { "($_[1].n)" }
sub cget { if($#_ == 1) {"($_[1].p)"} else {
	my $r = (ref $_[0] or $_[0]);
	$r =~ s/::MF/::SF/;
	if($#_ == 2) {
		return "($_[1].p[$_[2]])";
	}
	return $r->cget("($_[1].p[$_[2]])", @$_[3..$#_])
	} }

sub cfunc {
	my $r = (ref $_[0] or $_[0]);
	$r =~ s/::MF/::SF/;
	my $cm = $r->calloc("$_[1].p[iM]");
	my $su = $r->cfunc("($_[1].p[iM])","(*bM)");
	return "{
		AV *aM;
		SV **bM;
		int iM;
		int lM;
		if(!SvROK($_[2])) {
			$_[1].n = 0;
			$_[1].p = 0;
			/* die(\"Help! Multi without being ref\"); */
		} else {
			if(SvTYPE(SvRV($_[2])) != SVt_PVAV) {
				die(\"Help! Multi without being arrayref\");
			}
			aM = (AV *) SvRV($_[2]);
			lM = av_len(aM)+1;
			/* XXX Free previous p */
			$_[1].n = lM;
			$_[1].p = malloc(lM * sizeof(*($_[1].p)));
			/* XXX ALLOC */
			for(iM=0; iM<lM; iM++) {
				bM = av_fetch(aM, iM, 1); /* LVal for easiness */
				if(!bM) {
					die(\"Help: Multi $r bM == 0\");
				}
				$cm
				$su
			}
		}
	}
	"
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
	my $r = $_[0];
	$r =~ s/::MF/::SF/;
	if("ARRAY" eq ref $_[1]) {
		"(Multi print, $_[0], $_[1])", @{$_[1]},"  [ ".(join ' ',map {$r->as_string($_)} @{$_[1]})." ] ";
	} else {
		$_[1]->as_string();
	}
}


sub js_default {
	my($type) = @_;
	# $type =~ s/::MF/::SF/;
	$type =~ s/VRML::Field:://;
	return "new $type()";
}


###########################################################
package VRML::Field::SFNode;

sub copy { return $_[1] }

sub ctype {"void *$_[1]"}      # XXX ???
sub calloc {"$_[1] = 0;"}
sub cfree {"NODE_REMOVE_PARENT($_[1]); $_[1] = 0;"}
sub cstruct {""}
sub cfunc {
	"$_[1] = (void *)SvIV($_[2]); NODE_ADD_PARENT($_[1]);"
}
sub cget {if(!defined $_[2]) {return "$_[1]"}
	else {die "SFNode index!??!"} }

sub as_string {
	$_[1]->as_string();
}

sub js_default { 'new SFNode("","NULL")' }

# javascript implemented in place because of special nature.

##
## RCS: Implement SFImage
## Remi Cohen-Scali
##


###########################################################
package VRML::Field::SFImage;
@ISA=VRML::Field;
VRML::Error->import;

sub parse {
  my($type,$p) = @_;

  # JAS $VRML::verbose::parse=1;

  $_[2] =~ /\G\s*([0-9]+)\s+([0-9]+)\s+([0-9]+)/ogcs
    or parsefail($_[2], "didn't match width/height/depth of SFImage");
  my ($sfimage_width, $sfimage_height, $sfimage_depth) = ($1, $2, $3);
  parsefail($_[2], "pixel texture depth is invalid: consider values between 1 and 4 inclusive !")
    if (! (1 <= $sfimage_depth && $sfimage_depth <= 4));
  print "VRML::Field::SFImage::parse -  width = $sfimage_width     height = $sfimage_height   depth = $sfimage_depth \n"
    if $VRML::verbose::parse;
  my $npixgrp = ($sfimage_width * $sfimage_height) -1;
  my @pixrows; # A table of sfimage_height rows of (sfimage_width * sfimage_depth) columns
  my $x = 0, $y = 0;
 PIXGRP:
  while (1) 
    {
      $_[2] =~ /\G\s+((0x)[0-9A-Fa-f]+|[0-9]+)/ogcs
  	or parsefail($_[2], "didn't match a pixel (still $npixgrp to go)");
      my $pixel = $1;
      my $isx = (defined $2);
      print "VRML::Field::SFImage::parse -  pixel is $1 (".($2 ? "hexa" : "decimal").")\n" 
	if $VRML::verbose::parse;
      if ($isx) 
	{
 	DEPTHSWITCHX:
	  for ($sfimage_depth) {
	    m/1/ && do { 
  	      parsefail($pixel, "pixel $pixel didn't match a one-component pixel", "do not have correct number of digits") if ($pixel !~ m/0x([0-9A-Fa-f]{2})/);
  	      $pixrows[$y][$x++] = sprintf("%02s", $1); 
	      last DEPTHSWITCHX; 
	    };
	    m/2/ && do { 
   	      parsefail($pixel, "pixel $pixel didn't match a two-component pixel", "do not have correct number of digits") if ($pixel !~ m/0x([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})/);
  	      $pixrows[$y][$x++] = sprintf("%02s", $1);
  	      $pixrows[$y][$x++] = sprintf("%02s", $2);
	      last DEPTHSWITCHX; 
	    };
	    m/3/ && do {
   	      parsefail($pixel, "pixel $pixel didn't match a three-component pixel", "do not have correct number of digits") if ($pixel !~ m/0x([0-9A-Fa-f]{2}){3}/);
  	      $pixel =~ m/0x([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})/;
  	      $pixrows[$y][$x++] = sprintf("%02s", $1);
  	      $pixrows[$y][$x++] = sprintf("%02s", $2);
  	      $pixrows[$y][$x++] = sprintf("%02s", $3);
	      last DEPTHSWITCHX; 
	    };
	    m/4/ && do {
   	      parsefail($pixel, "pixel $pixel didn't match a three-component pixel", "do not have correct number of digits") if ($pixel !~ m/0x([0-9A-Fa-f]{2}){4}/);
  	      $pixel =~ m/0x([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})/;
  	      $pixrows[$y][$x++] = sprintf("%02s", $1);
  	      $pixrows[$y][$x++] = sprintf("%02s", $2);
  	      $pixrows[$y][$x++] = sprintf("%02s", $3);
  	      $pixrows[$y][$x++] = sprintf("%02s", $4);
	      last DEPTHSWITCHX;
	    };
	  }
	} 
      else
	{
 	DEPTHSWITCH:
	  for ($sfimage_depth) {
	    m/1/ && do { 
	      parsefail($pixel, "pixel $pixel didn't match a one-component pixel", "greater than 0xFF") if ($pixel >= 0x100);
  	      $pixrows[$y][$x++] = sprintf("%x", $pixel); 
	      last DEPTHSWITCH; 
	    };
	    m/2/ && do { 
 	      parsefail($pixel, "pixel $pixel didn't match a two-component pixel", "greater than 0xFFFF") if ($pixel >= 0x10000);
  	      $pixrows[$y][$x++] = sprintf("%02x", ((int($pixel) >> 8) & 0x00ff));	      
  	      $pixrows[$y][$x++] = sprintf("%02x", (int($pixel) & 0x00ff));
	      last DEPTHSWITCH; 
	    };
	    m/3/ && do {
 	      parsefail($pixel, "pixel $pixel didn't match a three-component pixel", "greater than 0xFFFFFF") if ($pixel >= 0x1000000);
  	      sprintf( "%06x", $pixel ) =~ m/([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})/;
  	      $pixrows[$y][$x++] = $1;
  	      $pixrows[$y][$x++] = $2;
  	      $pixrows[$y][$x++] = $3;
	      last DEPTHSWITCH; 
	    };
	    m/4/ && do {
 	      parsefail($pixel, "pixel $pixel didn't match a three-component pixel", "greater than 0xFFFFFFFF") if ($pixel > 0xFFFFFFFF);
  	      sprintf( "%06x", (($pixel >> 8) & 0x00ffffff)) =~ m/([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})/;
  	      $pixrows[$y][$x++] = $1;
  	      $pixrows[$y][$x++] = $2;
  	      $pixrows[$y][$x++] = $3;
  	      $pixrows[$y][$x++] = sprintf( "%02x", ($pixel & 0x000000ff));
	      last DEPTHSWITCH;
	    };
	  }
	}
      do {$x = 0; $y++} if ($x == ($sfimage_width * $sfimage_depth));
      next PIXGRP if ($npixgrp--);
      last PIXGRP;
    }
  my $dat = '';
  my $len = 0;
  for $y (0 .. ($sfimage_height -1))
    {
      for $x (0 .. (($sfimage_width * $sfimage_depth) -1))
	{
  	  $dat = $dat.eval( 'chr(0x'.$pixrows[$y][$x].' )');
	}
    }
  return [$sfimage_width, $sfimage_height, $sfimage_depth, $dat];
}

sub print {print join ' ',@{$_[1]}}
sub as_string {join ' ',@{$_[1]}}

sub ctype {"struct SFImage $_[1]"}
sub cstruct {return <<EOF
struct SFImage {
  int __x;
  int __y;
  int __depth;
  unsigned char *__data;
  int __texture;
};
EOF
}

sub cget {return "($_[1].$_[2])"}

sub cfunc {
	return <<EOF
            {
		AV *a;
		SV **__data, **__x, **__y, **__depth;
		STRLEN pl_na;
		int i, x, y;
		if(!SvROK($_[2])) {
			$_[1].__data = NULL;
			$_[1].__x = 0;
			$_[1].__y = 0;
			$_[1].__depth = 0;
			/* die(\"Help! SFImage without being ref\"); */
		} else {
			if(SvTYPE(SvRV($_[2])) != SVt_PVAV) {
				die(\"Help! SFImage without being arrayref\");
			}
			a = (AV *) SvRV($_[2]);

			/* __x */
			__x = av_fetch(a, 0, 1); /* LVal for easiness */
			if(!__x) die(\"Help: SFImage __x == NULL\");
			$_[1].__x = SvNV(*__x);
			/* fprintf(stdout, \"SFImage __x = %d\\n\", $_[1].__x); */

			/* __y */
			__y = av_fetch(a, 1, 1); /* LVal for easiness */
			if(!__y) die(\"Help: SFImage __y == NULL\");
			$_[1].__y = SvNV(*__y);
			/* fprintf(stdout, \"SFImage __y = %d\\n\", $_[1].__y); */

			/* __depth */
			__depth = av_fetch(a, 2, 1); /* LVal for easiness */
			if(!__depth) die(\"Help: SFImage __depth == NULL\");
			$_[1].__depth = SvNV(*__depth);
			/* fprintf(stdout, \"SFImage __depth = %d\\n\", $_[1].__depth); */

			/* Handle image data */
			__data = av_fetch(a, 4, 1); /* LVal for easiness */
			if(!__data) die(\"Help: SFImage __data == 0\");
			$_[1].__data = SvPV(*__data, pl_na);
			/* fprintf(stdout, \"SFImage __data = %s  (len = %d) \\n\", $_[1].__data, pl_na); */
		}
	}
EOF
}

sub js_ctor {
  return <<EOF;
    function SFImage (x, y, d, data) {
      this.x = x
      this.y = y
      this.d = d
      this.data = data
    }
EOF
}

sub js_default {
  return "new SFImage(0,0,0,\"\")";
}

1;

