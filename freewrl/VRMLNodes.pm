#
# $Id: VRMLNodes.pm,v 1.126 2005/01/28 14:55:34 crc_canada Exp $
#
# Copyright (C) 1998 Tuomas J. Lukka 1999 John Stewart CRC Canada.
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.

package VRML::NodeType; 

#########################################################
# The routines below implement the browser object interface.

# EG Die unless perl scripts are enabled
sub check_perl_script {
 	if(!$VRML::DO_PERL) {
 		die <<EOF ;

Perl scripts are currently unsafe as they are trusted and run in the
main interpreter. If you are sure of your code (i.e. you have written
it or know the writer), use freewrl's "-ps" (perl scripts) option.

FreeWRL should soon be modified to run untrusted Perl code in Safe
compartments. Until that, Perl scripts are disabled by default.

EOF
 	}
 }

## EG
## perl_script_output ( $on )
##
## If a script output file has been specified with the "-psout" option, and
##   If $on is true,  then
##     the filehandle corresponding to the script output file is selected.
##   If $on is false, then
##     the previously selected filehandle is selected.
##
## The following VRML:: variables are used
BEGIN { 
								# Script output file name (set with -psout)
  $VRML::script_out_file = "" unless
	defined $VRML::script_out_file ;
  $VRML::script_out_open = 0;	# True if output file is open
								# True if script output file is
								# currently selected
  $VRML::script_out_selected = 0 ;
  $VRML::LAST_HANDLE = 0 ;		# Filehandle in use before script call
}

sub perl_script_output {

  my $on = shift ;
  my $v = 0 ;					# Local verbose option

  if ($VRML::script_out_file) {	# If output is re-routed

								# I've already opened the file ....
    if ($VRML::script_out_open) {
								# ... but has someone deleted it?
	  if (! -f $VRML::script_out_file) {
		print "Script output file disappeared! (no problem)\n" if $v;
		close VRML_SCRIPT_OUT;
		$VRML::script_out_open = 0;
	  }
	}
								# Eventually open the file
    if (!$VRML::script_out_open) {
      open VRML_SCRIPT_OUT, ">$VRML::script_out_file"
		or die "Can't open script output file '$VRML::script_out_file'\n";
      $VRML::script_out_open = 1;
      print "Opened script output file '$VRML::script_out_file'\n" if $v;
    }

    if ($on) {					# select script output filehandle
      if (!$VRML::script_out_selected){
		$VRML::LAST_HANDLE = select VRML_SCRIPT_OUT;
		$| = 1;
		$VRML::script_out_selected = 1;
		print $VRML::LAST_HANDLE "VRML_SCRIPT_OUT selected\n" if $v;
      } else {
		print $VRML::LAST_HANDLE "VRML_SCRIPT_OUT already selected\n" if $v;
      }
    } else {					# select previous output filehandle
      select $VRML::LAST_HANDLE;
      $VRML::script_out_selected = 0;
      print "Selected previous filehandle '$VRML::LAST_HANDLE'\n" if $v;
    }
  } else {
    print "No script output file specified\n" if $v;
  }
}								# End perl_script_output

## EG : Just a little debugging function
sub show_stack {
  my $n = @_ ? shift : 1 ;
  for $i (2..$n+1) {
	my @a = caller ($i);
	if (@a) {
	  $a[1] =~ s{^.*/}{};
	  print "package $a[0], file $a[1]:$a[2], sub $a[3], wantarray $a[5]\n" ;
	}
  }
}
use Data::Dumper ;

# EG : returns the list of names of fields that are visible in the
# script
sub script_variables {
  ## my $fields = shift ;
  ## my @res = grep { ! /mustEvaluate|directOutput/ } sort keys %$fields ;
  ## map {print "$_ has value ", Dumper ($fields->{$_}) } @res ;
  ## @res;
  grep { ! /mustEvaluate|directOutput/ } sort keys %{shift()};
}

#JASpackage MTS ;			# My tied Scalar : Will be used to tie scalars
#JAS				# to fields and eventIn for perl scripting.
#JAS
#JASuse Exporter ;
#JAS@ISA = qw(Exporter);
#JAS@EXPORT = qw( with ) ;
#JAS
#JASrequire Tie::Scalar;
#JAS
#JASsub TIESCALAR {
#JAS  ### my $class = shift;
#JAS  ## my $self = shift;
#JAS  ## bless $self;
#JAS  ### bless shift ;
#JAS  bless $_[1];
#JAS}
#JAS
#JASsub FETCH {
#JAS  ## my $self = shift;
#JAS  ## $$self ;
#JAS  ### ${shift()};
#JAS  ${$_[0]};
#JAS}
#JAS
#JASsub STORE {
#JAS  ## my $self = shift;
#JAS  ## $$self = shift ;
#JAS  ${$_[0]} = $_[1];
#JAS}
#JAS
#JAS
package VRML::NodeType;

########################################################################

my $protono;

{
    # XXX When this changes, change Scene.pm: VRML::Scene::newp too --
    # the members must correspond.
    sub new {
		my($type, $name, $fields, $eventsubs) = @_;
		my $this = bless {
						  Name => $name,
						  Actions => $eventsubs,
						  Defaults => {},
						  EventOuts => {},
						  EventIns => {},
						  Fields => {},
						  FieldKinds => {},
						  FieldTypes => {}
						 },$type;

		my $t;
		for (keys %$fields) {
			if (ref $fields->{$_}[1] eq "ARRAY") {
				push @{$this->{Defaults}{$_}}, @{$fields->{$_}[1]};
			} else {
				$this->{Defaults}{$_} = $fields->{$_}[1];
			}
			$this->{FieldTypes}{$_} = $fields->{$_}[0];
			$t = $fields->{$_}[2];
			if (!defined $t) {
				die("Missing field or event type for $_ in $name");
			}

			if ($t =~ /in$/i) {
				$this->{EventIns}{$_} = $_;
			} elsif ($t =~ /out$/i) {
				$this->{EventOuts}{$_} = $_;
			} elsif ($t =~ /^exposed/) {
				## in case the 'set_' prefix or '_changed' suffix isn't
				## used for exposedFields in routes
				$this->{EventIns}{$_} = $_;
				$this->{EventOuts}{$_} = $_;
			}
			$this->{FieldKinds}{$_} = $t;
		}
		return $this;
    }
}


%VRML::Nodes::bindable = map {($_,1)} qw/
 Viewpoint
 Background
 NavigationInfo
 Fog
 GeoViewpoint
/;

# initevents are used in event propagation as the "First" events to run.
# check out add_first subroutine, and event propagation to see what happens.
# all of these require the "ClockTick" method.

%VRML::Nodes::initevents = map {($_,1)} qw/
 TimeSensor
 ProximitySensor
 Collision
 MovieTexture
 AudioClip
/;

# What are the transformation-hierarchy child nodes?
%VRML::Nodes::children = qw(
 Transform	children
 Group		children
 Billboard	children
 Anchor		children
 Collision	children
 GeoLocation	children
 InlineLoadControl children

);

%VRML::Nodes::siblingsensitive = map {($_, 1)} qw/
 TouchSensor
 GeoTouchSensor
 PlaneSensor
 CylinderSensor
 SphereSensor
/;

%VRML::Nodes::sensitive = map {($_, 1)} qw/
 ProximitySensor
 Anchor
/;

%VRML::Nodes = (
	# Internal structures, to store def and use in the right way
	DEF =>
	new VRML::NodeType("DEF",
					   { node => [SFNode, NULL, exposedField] },
					   id => [SFString, ""]
					  ),
	USE =>
	new VRML::NodeType("USE",
					   { node => [SFNode, NULL, exposedField] },
					   id => [SFString, ""]
					  ),
	Shape =>
	new VRML::NodeType ("Shape",
						{
						 appearance => [SFNode, NULL, exposedField],
						 geometry => [SFNode, NULL, exposedField]
						}
					   ),
	Appearance =>
	new VRML::NodeType ("Appearance",
						{
						 material => [SFNode, NULL, exposedField],
						 texture => [SFNode, NULL, exposedField],
						 textureTransform => [SFNode, NULL, exposedField]
						}
					   ),
	Material =>
	new VRML::NodeType ("Material",
						{
						 ambientIntensity => [SFFloat, 0.2, exposedField],
						 diffuseColor => [SFColor, [0.8, 0.8, 0.8], exposedField],
						 emissiveColor => [SFColor, [0, 0, 0], exposedField],
						 shininess => [SFFloat, 0.2, exposedField],
						 specularColor => [SFColor, [0, 0, 0], exposedField],
						 transparency => [SFFloat, 0, exposedField]
						}
					   ),
	ImageTexture =>
	new VRML::NodeType("ImageTexture",
					   {
						url => [MFString, [], exposedField],
						repeatS => [SFBool, 1, field],
						repeatT => [SFBool, 1, field],
						__texture => [SFInt32, 0, field],
						__parenturl =>[SFString,"",field],
					   },
					  ),
	PixelTexture =>
	new VRML::NodeType("PixelTexture",
					   {
						#JAS image => [SFImage, [0, 0, 0], exposedField],
						image => [SFImage, "0, 0, 0", exposedField],
						repeatS => [SFBool, 1, field],
						repeatT => [SFBool, 1, field],
						__texture => [SFInt32, 0, field],
						__parenturl =>[SFString,"",field],
					   },
					  ),
	MovieTexture =>
	new VRML::NodeType ("MovieTexture",
						{
						 loop => [SFBool, 0, exposedField],
						 speed => [SFFloat, 1.0, exposedField],
						 startTime => [SFTime, 0, exposedField],
						 stopTime => [SFTime, 0, exposedField],
						 url => [MFString, [""], exposedField],
						 repeatS => [SFBool, 1, field],
						 repeatT => [SFBool, 1, field],
						 duration_changed => [SFTime, -1, eventOut],
						 isActive => [SFBool, 0, eventOut],
						 
						 # has the URL changed???
						 __oldurl => [MFString, [""], field],
						 # initial texture number
						 __texture0_ => [SFInt32, 0, field],
						 # last texture number
						 __texture1_ => [SFInt32, 0, field],
						 # which texture number is used
						 __ctex => [SFInt32, 0, field],
						 # time that we were initialized at
						 __inittime => [SFTime, 0, field],
						 # internal sequence number
						 __sourceNumber => [SFInt32, -1, field],
						# parent url, gets replaced at node build time
						__parenturl =>[SFString,"",field],
						}
					   ),
	Box =>
	new VRML::NodeType("Box",
					   { 	size => [SFVec3f, [2, 2, 2], field],
						solid => [SFBool, 1, field],
						__points  =>[SFInt32,0,field],
					   }
					  ),
	Cylinder =>
	new VRML::NodeType ("Cylinder",
						{
						 bottom => [SFBool, 1, field],
						 height => [SFFloat, 2.0, field],
						 radius => [SFFloat, 1.0, field],
						 side => [SFBool, 1, field],
						 top => [SFBool, 1, field],
						solid => [SFBool, 1, field],
						 __points =>[SFInt32,0,field],
						 __normals =>[SFInt32,0,field],
						},
					   ),
	Cone =>
	new VRML::NodeType ("Cone",
						{
						 bottomRadius => [SFFloat, 1.0, field],
						 height => [SFFloat, 2.0, field],
						 side => [SFBool, 1, field],
						 bottom => [SFBool, 1, field],
						solid => [SFBool, 1, field],
						 __sidepoints =>[SFInt32,0,field],
						 __botpoints =>[SFInt32,0,field],
						 __normals =>[SFInt32,0,field],
						},
					   ),
	Coordinate =>
	new VRML::NodeType("Coordinate",
					   { point => [MFVec3f, [], exposedField] }
					  ),
	Color =>
	new VRML::NodeType("Color",
					   { color => [MFColor, [], exposedField] }
					  ),
	Normal =>
	new VRML::NodeType("Normal",
					   { vector => [MFVec3f, [], exposedField] }
					  ),
	TextureCoordinate =>
	new VRML::NodeType("TextureCoordinate",
					   { point => [MFVec2f, [], exposedField] }
					  ),
	ElevationGrid =>
	new VRML::NodeType("ElevationGrid",
					   {
						set_height => [MFFloat, undef, eventIn],
						color => [SFNode, NULL, exposedField],
						normal => [SFNode, NULL, exposedField],
						texCoord => [SFNode, NULL, exposedField],
						height => [MFFloat, [], field],
						ccw => [SFBool, 1, field],
						colorPerVertex => [SFBool, 1, field],
						creaseAngle => [SFFloat, 0, field],
						normalPerVertex => [SFBool, 1, field],
						solid => [SFBool, 1, field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field]
					   }
					  ),
	Extrusion =>
	new VRML::NodeType("Extrusion",
					   {
						set_crossSection => [MFVec2f, undef, eventIn],
						set_orientation => [MFRotation, undef, eventIn],
						set_scale => [MFVec2f, undef, eventIn],
						set_spine => [MFVec3f, undef, eventIn],
						beginCap => [SFBool, 1, field],
						ccw => [SFBool, 1, field],
						convex => [SFBool, 1, field],
						creaseAngle => [SFFloat, 0, field],
						crossSection => [MFVec2f, [[1, 1],[1, -1],[-1, -1],
												   [-1, 1],[1, 1]], field],
						endCap => [SFBool, 1, field],
						orientation => [MFRotation, [[0, 0, 1, 0]],field],

						scale => [MFVec2f, [[1, 1]], field],
						solid => [SFBool, 1, field],
						spine => [MFVec3f, [[0, 0, 0],[0, 1, 0]], field]
					   }
					  ),
	Sphere =>
	new VRML::NodeType("Sphere",
					   { 	radius => [SFFloat, 1.0, field],
						solid => [SFBool, 1, field],
						 __points =>[SFInt32,0,field],
 					   }
					  ),
	IndexedFaceSet =>
	new VRML::NodeType("IndexedFaceSet",
					   {
						set_colorIndex => [MFInt32, undef, eventIn],
						set_coordIndex => [MFInt32, undef, eventIn],
						set_normalIndex => [MFInt32, undef, eventIn],
						set_texCoordIndex => [MFInt32, undef, eventIn],
						color => [SFNode, NULL, exposedField],
						coord => [SFNode, NULL, exposedField],
						normal => [SFNode, NULL, exposedField],
						texCoord => [SFNode, NULL, exposedField],
						ccw => [SFBool, 1, field],
						colorIndex => [MFInt32, [], field],
						colorPerVertex => [SFBool, 1, field],
						convex => [SFBool, 1, field],
						coordIndex => [MFInt32, [], field],
						creaseAngle => [SFFloat, 0, field],
						normalIndex => [MFInt32, [], field],
						normalPerVertex => [SFBool, 1, field],
						solid => [SFBool, 1, field],
						texCoordIndex => [MFInt32, [], field]
					   }
					  ),
	IndexedLineSet =>
	new VRML::NodeType("IndexedLineSet",
					   {
						set_colorIndex => [MFInt32, undef, eventIn],
						set_coordIndex => [MFInt32, undef, eventIn],
						color => [SFNode, NULL, exposedField],
						coord => [SFNode, NULL, exposedField],
						colorIndex => [MFInt32, [], field],
						colorPerVertex => [SFBool, 1, field],
						coordIndex => [MFInt32, [], field]
					   }
					  ),
	PointSet =>
	new VRML::NodeType("PointSet",
					   {
						color => [SFNode, NULL, exposedField],
						coord => [SFNode, NULL, exposedField]
					   }
					  ),
	Text =>
	new VRML::NodeType ("Text",
						{
						 string => [MFString, [], exposedField],
						 fontStyle => [SFNode, NULL, exposedField],
						 length => [MFFloat, [], exposedField],
						 maxExtent => [SFFloat, 0, exposedField],
						 __rendersub => [SFInt32, 0, exposedField] # Function ptr hack
						}
					   ),
	FontStyle =>
	new VRML::NodeType("FontStyle",
					   {
						family => [MFString, ["SERIF"], field],
						horizontal => [SFBool, 1, field],
						justify => [MFString, ["BEGIN"], field],
						language => [SFString, "", field],
						leftToRight => [SFBool, 1, field],
						size => [SFFloat, 1.0, field],
						spacing => [SFFloat, 1.0, field],
						style => [SFString, "PLAIN", field],
						topToBottom => [SFBool, 1, field]
					   }
					  ),
	AudioClip =>
	new VRML::NodeType("AudioClip",
					   {
						description => [SFString, "", exposedField],
						loop =>	[SFBool, 0, exposedField],
						pitch => [SFFloat, 1.0, exposedField],
						startTime => [SFTime, 0, exposedField],
						stopTime => [SFTime, 0, exposedField],
						url => [MFString, [], exposedField],
						duration_changed => [SFTime, -1, eventOut],
						isActive => [SFBool, 0, eventOut],
						# parent url, gets replaced at node build time
						__parenturl =>[SFString,"",field],

						# internal sequence number
						__sourceNumber => [SFInt32, -1, field],
						# local name, as received on system
						__localFileName => [SFInt32, 0,field],
						# time that we were initialized at
						__inittime => [SFTime, 0, field],
					   }
					  ),
	Sound =>
	new VRML::NodeType("Sound",
					   {
						direction => [SFVec3f, [0, 0, 1], exposedField],
						intensity => [SFFloat, 1.0, exposedField],
						location => [SFVec3f, [0, 0, 0], exposedField],
						maxBack => [SFFloat, 10.0, exposedField],
						maxFront => [SFFloat, 10.0, exposedField],
						minBack => [SFFloat, 1.0, exposedField],
						minFront => [SFFloat, 1.0, exposedField],
						priority => [SFFloat, 0, exposedField],
						source => [SFNode, NULL, exposedField],
						spatialize => [SFBool,1, field]
					   },
					  ),
	Switch =>
	new VRML::NodeType("Switch",
					   {
						choice => [MFNode, [], exposedField],
						whichChoice => [SFInt32, -1, exposedField]
					   }
					  ),
	LOD =>
	new VRML::NodeType("LOD",
					   {
						level => [MFNode, [], exposedField],
						center => [SFVec3f, [0, 0, 0],  field],
						range => [MFFloat, [], field],
						_selected =>[SFInt32,0,field]
					   }
					  ),
	Transform =>
	new VRML::NodeType ("Transform",
						{
						 addChildren => [MFNode, undef, eventIn],
						 removeChildren => [MFNode, undef, eventIn],
						 center => [SFVec3f, [0, 0, 0], exposedField],
						 children => [MFNode, [], exposedField],
						 rotation => [SFRotation, [0, 0, 1, 0], exposedField],
						 scale => [SFVec3f, [1, 1, 1], exposedField],
						 scaleOrientation => [SFRotation, [0, 0, 1, 0], exposedField],
						 translation => [SFVec3f, [0, 0, 0], exposedField],
						 bboxCenter => [SFVec3f, [0, 0, 0], field],
						 bboxSize => [SFVec3f, [-1, -1, -1], field],

						 # fields for reducing redundant calls
						 __do_center => [SFInt32, 0, field],
						 __do_trans => [SFInt32, 0, field],
						 __do_rotation => [SFInt32, 0, field],
						 __do_scaleO => [SFInt32, 0, field],
						 __do_scale => [SFInt32, 0, field],
						},
					   ),
	TextureTransform =>
	new VRML::NodeType ("TextureTransform",
						{
						 center => [SFVec2f, [0, 0], exposedField],
						 rotation => [SFFloat, 0, exposedField],
						 scale => [SFVec2f, [1, 1], exposedField],
						 translation => [SFVec2f, [0, 0], exposedField]
						}
					   ),
	Group =>
	new VRML::NodeType("Group",
					   {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field]
					   },
					  ),
	Anchor =>
	new VRML::NodeType("Anchor",
					   {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						description => [SFString, "", exposedField],
						parameter => [MFString, [], exposedField],
						url => [MFString, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						__parenturl =>[SFString,"",field],
					   },
					  ),
	Billboard =>
	new VRML::NodeType("Billboard",
					   {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						axisOfRotation => [SFVec3f, [0, 1, 0], exposedField],
						children => [MFNode, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field]
					   },
					  ),
	WorldInfo =>
	new VRML::NodeType("WorldInfo",
					   {
						info => [MFString, [], field],
						title => [SFString, "", field]
					   }
					  ),

	#############################################################################################
	# interpolators
	#

	ScalarInterpolator =>
	new VRML::NodeType("ScalarInterpolator",
		   {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFFloat, [], exposedField],
			value_changed => [SFFloat, 0.0, eventOut]
		   },
		  ),

	OrientationInterpolator =>
	new VRML::NodeType("OrientationInterpolator",
		   {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFRotation, [], exposedField],
			value_changed => [SFRotation, [0, 0, 1, 0], eventOut]
		   },
		  ),

	###################################################################################
	# PositionInterpolator,ColorInterpolator use same code
	# MAKE SURE FIELD DEFS ARE SAME AND IN SAME ORDER

	ColorInterpolator =>
	new VRML::NodeType("ColorInterpolator",
		   {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFColor, [], exposedField],
			value_changed => [SFColor, [0, 0, 0], eventOut],
		   },
		  ),

	PositionInterpolator =>
	new VRML::NodeType("PositionInterpolator",
		   {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFVec3f, [], exposedField],
			value_changed => [SFVec3f, [0, 0, 0], eventOut],
		   },
		  ),

	############################################################################33
	# CoordinateInterpolators and NormalInterpolators use almost the same thing;
	# look at the _type field.

	CoordinateInterpolator =>
	new VRML::NodeType("CoordinateInterpolator",
		{
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFVec3f, [], exposedField],
			value_changed => [MFVec3f, [], eventOut],
			_type => [SFInt32, 0, exposedField], #1 means dont normalize
		},
	  ),



	NormalInterpolator =>
	new VRML::NodeType("NormalInterpolator",
		{
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFVec3f, [], exposedField],
			value_changed => [MFVec3f, [], eventOut],
			_type => [SFInt32, 1, exposedField], #1 means normalize
		},
	  ),

	###################################################################################


	TimeSensor =>
	new VRML::NodeType("TimeSensor",
					   {
						cycleInterval => [SFTime, 1, exposedField],
						enabled => [SFBool, 1, exposedField],
						loop => [SFBool, 0, exposedField],
						startTime => [SFTime, 0, exposedField],
						stopTime => [SFTime, 0, exposedField],
						cycleTime => [SFTime, -1, eventOut],
						fraction_changed => [SFFloat, 0.0, eventOut],
						isActive => [SFBool, 0, eventOut],
						time => [SFTime, -1, eventOut],
						 # time that we were initialized at
						 __inittime => [SFTime, 0, field],
						# cycleTimer flag.
						__ctflag =>[SFTime, 10, exposedField]
					   },
					  ),


	TouchSensor =>
	new VRML::NodeType("TouchSensor",
					   {
						enabled => [SFBool, 1, exposedField],
						hitNormal_changed => [SFVec3f, [0, 0, 0], eventOut],
						hitPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						hitTexCoord_changed => [SFVec2f, [0, 0], eventOut],
						isActive => [SFBool, 0, eventOut],
						isOver => [SFBool, 0, eventOut],
						touchTime => [SFTime, -1, eventOut]
					   },
					  ),


	PlaneSensor =>
	new VRML::NodeType("PlaneSensor",
					   {
						autoOffset => [SFBool, 1, exposedField],
						enabled => [SFBool, 1, exposedField],
						maxPosition => [SFVec2f, [-1, -1], exposedField],
						minPosition => [SFVec2f, [0, 0], exposedField],
						offset => [SFVec3f, [0, 0, 0], exposedField],
						isActive => [SFBool, 0, eventOut],
						trackPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						translation_changed => [SFVec3f, [0, 0, 0], eventOut],
						# where we are at a press...
						_origPoint => [SFVec3f, [0, 0, 0], field],
					   },
					  ),

	SphereSensor =>
	new VRML::NodeType("SphereSensor",
					   {
						autoOffset => [SFBool, 1, exposedField],
						enabled => [SFBool, 1, exposedField],
						offset => [SFRotation, [0, 1, 0, 0], exposedField],
						isActive => [SFBool, 0, eventOut],
						rotation_changed => [SFRotation, [0, 0, 1, 0], eventOut],
						trackPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						# where we are at a press...
						_origPoint => [SFVec3f, [0, 0, 0], field],
						_radius => [SFFloat, 0, field],
					   },
					  ),

	CylinderSensor =>
	new VRML::NodeType("CylinderSensor",
					   {
						autoOffset => [SFBool, 1, exposedField],
						diskAngle => [SFFloat, 0.262, exposedField],
						enabled => [SFBool, 1, exposedField],
						maxAngle => [SFFloat, -1.0, exposedField],
						minAngle => [SFFloat, 0, exposedField],
						offset => [SFFloat, 0, exposedField],
						isActive => [SFBool, 0, eventOut],
						rotation_changed => [SFRotation, [0, 0, 1, 0], eventOut],	
						trackPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						# where we are at a press...
						_origPoint => [SFVec3f, [0, 0, 0], field],
						_radius => [SFFloat, 0, field],
					   },
					  ),


	ProximitySensor =>
	new VRML::NodeType("ProximitySensor",
					   {
						center => [SFVec3f, [0, 0, 0], exposedField],
						size => [SFVec3f, [0, 0, 0], exposedField],
						enabled => [SFBool, 1, exposedField],
						isActive => [SFBool, 0, eventOut],
						position_changed => [SFVec3f, [0, 0, 0], eventOut],
						orientation_changed => [SFRotation, [0, 0, 1, 0], eventOut],
						enterTime => [SFTime, -1, eventOut],
						exitTime => [SFTime, -1, eventOut],
						# These fields are used for the info.
						__hit => [SFInt32, 0, exposedField],
						__t1 => [SFVec3f, [10000000, 0, 0], exposedField],
						__t2 => [SFRotation, [0, 1, 0, 0], exposedField]
					   },
					  ),


	DirectionalLight =>
	new VRML::NodeType("DirectionalLight",
					   {
						ambientIntensity => [SFFloat, 0, exposedField],
						color => [SFColor, [1, 1, 1], exposedField],
						direction => [SFVec3f, [0, 0, -1], exposedField],
						intensity => [SFFloat, 1.0, exposedField],
						on => [SFBool, 1, exposedField]
					   }
					  ),

	PointLight =>
	new VRML::NodeType("PointLight",
					   {
						ambientIntensity => [SFFloat, 0, exposedField],
						attenuation => [SFVec3f, [1, 0, 0], exposedField],
						color => [SFColor, [1, 1, 1], exposedField],
						intensity => [SFFloat, 1.0, exposedField],
						location => [SFVec3f, [0, 0, 0], exposedField],
						on => [SFBool, 1, exposedField],
						radius => [SFFloat, 100.0, exposedField],
						##not in the spec
						direction => [SFVec3f, [0, 0, -1.0], exposedField]
					   }
					  ),

	SpotLight =>
	new VRML::NodeType("SpotLight",
					   {
						ambientIntensity => [SFFloat, 0, exposedField],
						attenuation => [SFVec3f, [1, 0, 0], exposedField],
						beamWidth => [SFFloat, 1.570796, exposedField],
						color => [SFColor, [1, 1, 1], exposedField],
						cutOffAngle => [SFFloat, 0.785398, exposedField],
						direction => [SFVec3f, [0, 0, -1], exposedField],
						intensity => [SFFloat, 1.0, exposedField],
						location => [SFVec3f, [0, 0, 0], exposedField],
						on => [SFBool, 1, exposedField],
						radius => [SFFloat, 100.0, exposedField]
					   }
					  ),

	Fog =>
	new VRML::NodeType("Fog",
					   {
						set_bind => [SFBool, undef, eventIn],
						color => [SFColor, [1, 1, 1], exposedField],
						fogType => [SFString, "LINEAR", exposedField],
						visibilityRange => [SFFloat, 0, exposedField],
						isBound => [SFBool, 0, eventOut]
					   }
					  ),

	Background =>
	new VRML::NodeType("Background",
					   {
						set_bind => [SFBool, undef, eventIn],
						groundAngle => [MFFloat, [], exposedField],
						groundColor => [MFColor, [], exposedField],
						skyAngle => [MFFloat, [], exposedField],
						skyColor => [MFColor, [[0, 0, 0]], exposedField],
						isBound => [SFBool, 0, eventOut],
						__parenturl =>[SFString,"",field],
						__points =>[SFInt32,0,field],
						__colours =>[SFInt32,0,field],
						__quadcount => [SFInt32,0,field],
						
						(map {(
							   $_.Url => [MFString, [], exposedField],
							   # OpenGL texture number
							   __texture.$_ => [SFInt32, 0, exposedField],
							  )} qw/back front top bottom left right/),
					   },
					  ),

	Viewpoint =>
	new VRML::NodeType("Viewpoint",
					   {
						set_bind => [SFBool, undef, eventIn],
						fieldOfView => [SFFloat, 0.785398, exposedField],
						jump => [SFBool, 1, exposedField],
						orientation => [SFRotation, [0, 0, 1, 0], exposedField],
						position => [SFVec3f,[0, 0, 10], exposedField],
						description => [SFString, "", field],
						bindTime => [SFTime, -1, eventOut],
						isBound => [SFBool, 0, eventOut],
					   },
					  ),


	NavigationInfo =>
	new VRML::NodeType("NavigationInfo",
					   {
						set_bind => [SFBool, undef, eventIn],
						avatarSize => [MFFloat, [0.25, 1.6, 0.75], exposedField],
						headlight => [SFBool, 1, exposedField],
						speed => [SFFloat, 1.0, exposedField],
						type => [MFString, ["WALK", "ANY"], exposedField],
						visibilityLimit => [SFFloat, 0, exposedField],
						isBound => [SFBool, 0, eventOut]
					   },
					  ),

	# Complete
	# fields, eventins and outs parsed in Parse.pm by special switch :(
	# JAS took out perl script, because it is not standard.


	## directOutput && mustEvaluate work???

	Script =>
	new VRML::NodeType("Script",
					   {
						url => [MFString, [], exposedField],
						directOutput => [SFBool, 0, field],
						mustEvaluate => [SFBool, 0, field],
					   },
					   {
						url => sub {
							print "ScriptURL $_[0] $_[1]!!\n"
								if $VRML::verbose::script;
							die "URL setting not enabled";
						},
						__any__ => sub {
							my($t,$f,$v,$time,$ev) = @_;
							print "ScriptANY ",VRML::NodeIntern::dump_name($t),
								" $_[1] $_[2] $_[3] $_[4]\n"

									if $VRML::verbose::script;
							my $s;


							if (($s = $t->{ScriptScript}{$ev})) {
								print "CALL $s\n"
									if $VRML::verbose::script;
								##EG show_stack (5);
								##EG return &{$s}();
								perl_script_output (1);
								my @res = &{$s}();
								perl_script_output (0);
								return @res ;
							} elsif ($t->{J}) {

								#EG			if($t->{J}) {
								return $t->{J}->sendevent($t, $ev, $v, $time);
							}
							return ();
						},
					   }
					  ),

	Collision =>
	new VRML::NodeType("Collision",
					   {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						collide => [SFBool, 1, exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						proxy => [SFNode, NULL, field],
						collideTime => [SFTime, -1, eventOut],
						# return info for collisions
						# bit 0 : collision or not
						# bit 1: changed from previous of not
						__hit => [SFInt32, 0, exposedField]
					   },
					  ),

	Inline =>
	new VRML::NodeType("Inline",
					   {
						url => [MFString, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
                                                __children => [MFNode, [], exposedField],
						__loadstatus =>[SFInt32,0,field],
						__parenturl =>[SFString,"",field],
					   },
					  ),
	# GeoVRML Nodes
#XXX
	GeoCoordinate =>
	new VRML::NodeType("GeoCoordinate",
					{
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						point => [MFString,[],field],
					}
				),
#XXX
	GeoElevationGrid =>
	new VRML::NodeType("GeoElevationGrid",
					{
						set_height => [MFFloat, undef, eventIn],
						set_YScale => [MFFloat, undef, eventIn],
						color => [SFNode, NULL, exposedField],
						normal => [SFNode, NULL, exposedField],
						texCoord => [SFNode, NULL, exposedField],
						ccw => [SFBool,1,field],
						colorPerVertex => [SFBool, 1, field],
						creaseAngle => [SFFloat, 0, field],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						geoGridOrigin => [SFString,"0 0 0",field],
						height => [MFFloat, [], field],
						normalPerVertex => [SFBool, 1, field],
						solid => [SFBool, 1, field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFString, "1.0", field],
						yScale => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFString, "1.0", field]
					}
				),

	GeoLOD =>
	new VRML::NodeType("GeoLOD",
					{
						center => [SFString,"",field],
						child1Url =>[MFString,[],field],
						child2Url =>[MFString,[],field],
						child3Url =>[MFString,[],field],
						child4Url =>[MFString,[],field],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						range => [SFFloat,10.0,field],
						rootUrl => [MFString,[],field],
						rootNode => [MFNode,[],field],
						children => [MFNode,[],eventOut],
					}
				),


	GeoMetadata=>
	new VRML::NodeType("GeoMetadata",
					{
						data => [MFNode,[],exposedField],
						summary => [MFString,[],exposedField],
						url => [MFString,[],exposedField],
					}
				),

	GeoPositionInterpolator=>
	new VRML::NodeType("GeoPositionInterpolator",
					{
						set_fraction => [SFFloat,undef,eventIn],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						key => [MFFloat,[],field],
						keyValue => [MFString,[],field],
						geovalue_changed => [SFString,"",eventOut],
						value_changed => [SFVec3f,undef,eventOut],
					}
				),


	GeoTouchSensor=>
	new VRML::NodeType("GeoTouchSensor",
					{
						enabled => [SFBool,1,exposedField],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						hitNormal_changed => [SFVec3f, [0, 0, 0], eventOut],
						hitPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						hitTexCoord_changed => [SFVec2f, [0, 0], eventOut],
						hitGeoCoord_changed => [SFString,"",eventOut],
						isActive => [SFBool, 0, eventOut],
						isOver => [SFBool, 0, eventOut],
						touchTime => [SFTime, -1, eventOut]
					}
				),

	GeoViewpoint =>
	new VRML::NodeType("GeoViewpoint",
					   {
						set_bind => [SFBool, undef, eventIn],
						set_orientation => [SFString, undef, eventIn],
						set_position => [SFString, undef, eventIn],
						fieldOfView => [SFFloat, 0.785398, exposedField],
						headlight => [SFBool, 1, exposedField],
						jump => [SFBool, 1, exposedField],
						navType => [MFString, ["EXAMINE","ANY"],exposedField],
						description => [SFString, "", field],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						orientation => [SFRotation, [0, 0, 1, 0], field],
						position => [SFString,"0, 0, 100000", field],
						speedFactor => [SFFloat,1.0,field],
						bindTime => [SFTime, -1, eventOut],
						isBound => [SFBool, 0, eventOut],

						# "compiled" versions of strings above
						__position => [SFVec3f,[0, 0, 100000], field],
						__geoSystem => [SFInt32,0,field],
					   },
					  ),
	GeoOrigin =>
	new VRML::NodeType("GeoOrigin",
					{
						geoSystem => [MFString,["GD","WE"],exposedField],
						geoCoords => [SFString,"",exposedField],
						rotateYUp => [SFBool,0,field],

						# these are now static in CFuncs/GeoVRML.c
						# "compiled" versions of strings above
						#__geoCoords => [SFVec3f,[0, 0, 0], field],
						#__geoSystem => [SFInt32,0,field],
					}
					),

	GeoLocation =>
	new VRML::NodeType("GeoLocation",
					{
						geoCoords => [SFString,"",exposedField],
						children => [MFNode, [], field],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						__bboxCenter => [SFVec3f, [0, 0, 0], field],
						__bboxSize => [SFVec3f, [-1, -1, -1], field],

						# "compiled" versions of strings above
						__geoCoords => [SFVec3f,[0, 0, 0], field],
						__geoSystem => [SFInt32,0,field],
						
					}
					),



	# Unimplemented as yet...


	VisibilitySensor =>
	new VRML::NodeType("VisibilitySensor",
					   {
						center => [SFVec3f, [0, 0, 0], exposedField],
						enabled => [SFBool, 1, exposedField],
						size => [SFVec3f, [0, 0, 0], exposedField],
						enterTime => [SFTime, -1, eventOut],
						exitTime => [SFTime, -1, eventOut],
						isActive => [SFBool, 0, eventOut]
					   }
					  ),


	Contour2D =>
	new VRML::NodeType("Contour2D",
					{
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
					}
					),

	CoordinateDeformer =>
	new VRML::NodeType("CoordinateDeformer",
					{
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						controlPoint =>[MFVec3f,[],exposedField],
						inputCoord => [MFNode,[],exposedField],
						inputTransform => [MFNode,[],exposedField],
						outputCoord => [MFNode,[],exposedField],
						weight => [MFFloat,[],exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						uDimension => [SFInt32,0,field],
						uKnot => [MFFloat,[],field],
						uOrder => [SFInt32,2,field],
						vDimension => [SFInt32,0,field],
						vOrder => [SFInt32,2,field],
						wDimension => [SFInt32,0,field],
						wKnot => [MFFloat,[],field],
						wOrder =>[SFInt32,2,field],
					}
					),


	InlineLoadControl =>
	new VRML::NodeType("InlineLoadControl",
					{
						load =>[SFBool,TRUE,exposedField],
						url => [MFString,[],exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						children => [MFNode, [], eventOut],
						__loadstatus =>[SFInt32,0,field],
						__parenturl =>[SFString,"",field],
					}
					),

	NurbsCurve =>
	new VRML::NodeType("NurbsCurve",
					{
						controlPoint =>[MFVec3f,[],exposedField],
						weight => [MFFloat,[],exposedField],
						tessellation => [SFInt32,0,exposedField],
						knot => [MFFloat,[],field],
						order => [SFInt32,3,field],
					}
					),

	NurbsCurve2D =>
	new VRML::NodeType("NurbsCurve2D",
					{
						controlPoint =>[MFVec2f,[],exposedField],
						weight => [MFFloat,[],exposedField],
						tessellation => [SFInt32,0,exposedField],
						knot => [MFFloat,[],field],
						order => [SFInt32,3,field],
					}
					),

	NurbsGroup =>
	new VRML::NodeType("NurbsGroup",
					{
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						tessellationScale => [SFFloat,1.0,exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
					}
					),

	NurbsPositionInterpolator =>
	new VRML::NodeType("NurbsPositionInterpolator",
					{
						
						set_fraction => [SFFloat,undef,eventIn],
						dimension => [SFInt32,0,exposedField],
						keyValue => [MFVec3f,[],exposedField],
						keyWeight => [MFFloat,[],exposedField],
						knot => [MFFloat,[],exposedField],
						order => [SFInt32,[],exposedField],
						value_changed => [SFVec3f,undef,eventOut],
					}
					),

	NurbsSurface =>
	new VRML::NodeType("NurbsSurface",
					{
						controlPoint =>[MFVec3f,[],exposedField],
						texCoord => [SFNode, NULL, exposedField],
						uTessellation => [SFInt32,0,exposedField],
						vTessellation => [SFInt32,0,exposedField],
						weight => [MFFloat,[],exposedField],
						ccw => [SFBool,1,field],

						knot => [MFFloat,[],field],
						order => [SFInt32,3,field],
					}
					),
	NurbsTextureSurface =>
	new VRML::NodeType("NurbsTextureSurface",
					{
					}
					),

	PolyLine2D =>
	new VRML::NodeType("Polyline2D",
					{
					}
					),
	TrimmedSurface =>
	new VRML::NodeType("TrimmedSurface",
					{
					}
					),




); ##%VRML::Nodes


1;

