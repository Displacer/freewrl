#
# $Id: VRMLNodes.pm,v 1.212 2007/03/12 20:54:00 crc_canada Exp $
#
# Copyright (C) 1998 Tuomas J. Lukka 1999 John Stewart CRC Canada.
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.

package VRML::NodeType;

########################################################################

{
    # XXX When this changes, change Scene.pm: VRML::Scene::newp too --
    # the members must correspond.
    sub new {
		my($type, $name, $fields, $X3DNodeType) = @_;

		if ($X3DNodeType eq "") {
			print "NodeType, X3DNodeType blank for $name\n";
			$X3DNodeType = "unknown";
		}

		my $this = bless {
						  Name => $name,
						  Defaults => {},
						  EventOuts => {},
						  EventIns => {},
						  Fields => {},
						  FieldKinds => {},
						  FieldTypes => {},
						  X3DNodeType => $X3DNodeType
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

%VRML::Nodes = (
	###################################################################################

	#		Time Component

	###################################################################################

	TimeSensor => new VRML::NodeType("TimeSensor", {
						cycleInterval => [SFTime, 1, exposedField],
						enabled => [SFBool, 1, exposedField],
						loop => [SFBool, 0, exposedField],
						startTime => [SFTime, 0, exposedField],
						stopTime => [SFTime, 0, exposedField],
						cycleTime => [SFTime, -1, eventOut],
						fraction_changed => [SFFloat, 0.0, eventOut],
						isActive => [SFBool, 0, eventOut],
						time => [SFTime, -1, eventOut],
						resumeTime => [SFTime,0,exposedField],
						pauseTime => [SFTime,0,exposedField],
						isPaused => [SFTime,0,eventOut],
						 # time that we were initialized at
						 __inittime => [SFTime, 0, field],
						# cycleTimer flag.
						__ctflag =>[SFTime, 10, exposedField]
					   },"X3DSensorNode"),


	###################################################################################

	#		Networking Component

	###################################################################################

	Anchor => new VRML::NodeType("Anchor", {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						description => [SFString, "", exposedField],
						parameter => [MFString, [], exposedField],
						url => [MFString, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						__parenturl =>[SFString,"",field],
					   },"X3DGroupingNode"),


	Inline => new VRML::NodeType("Inline", {
						url => [MFString, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						load => [SFBool,0,field],
                                                __children => [MFNode, [], exposedField],
						__loadstatus =>[SFInt32,0,field],
						__parenturl =>[SFString,"",field],
					   },"X3DNetworkSensorNode"),

	LoadSensor => new VRML::NodeType("LoadSensor", {
						enabled => [SFBool,1,exposedField],
						timeOut  => [SFTime,0,exposedField],
						watchList => [MFNode, [], exposedField],
						isActive  => [SFBool,0,eventOut],
						isLoaded  => [SFBool,0,eventOut],
						loadTime  => [SFTime,0,eventOut],
						progress  => [SFFloat,0,eventOut],
						__loading => [SFBool,0,field],		# current internal status
						__finishedloading => [SFBool,0,field],	# current internal status
						__StartLoadTime => [SFTime,0,eventOut], # time we started loading...
					},"X3DNetworkSensorNode"),

	###################################################################################

	#		Grouping Component

	###################################################################################

	Group => new VRML::NodeType("Group", {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						 __protoDef => [FreeWRLPTR, 0, field], # tell renderer that this is a proto...
					   },"X3DGroupingNode"),

	StaticGroup => new VRML::NodeType("StaticGroup", {
						children => [MFNode, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						 __transparency => [SFInt32, -1, field], # display list for transparencies
						 __solid => [SFInt32, -1, field],	 # display list for solid geoms.
						 __OccludeNumber =>[SFInt32,-1,field], # for Occlusion tests.
					   },"X3DGroupingNode"),

	Switch => new VRML::NodeType("Switch", {
					 	addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						choice => [MFNode, [], exposedField],
						whichChoice => [SFInt32, -1, exposedField],
						 bboxCenter => [SFVec3f, [0, 0, 0], field],
						 bboxSize => [SFVec3f, [-1, -1, -1], field],
					   },"X3DGroupingNode"),

	Transform => new VRML::NodeType ("Transform", {
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
						 __OccludeNumber =>[SFInt32,-1,field], # for Occlusion tests.

						 # fields for reducing redundant calls
						 __do_center => [SFInt32, 0, field],
						 __do_trans => [SFInt32, 0, field],
						 __do_rotation => [SFInt32, 0, field],
						 __do_scaleO => [SFInt32, 0, field],
						 __do_scale => [SFInt32, 0, field],
						},"X3DGroupingNode"),

	WorldInfo => new VRML::NodeType("WorldInfo", {
						info => [MFString, [], field],
						title => [SFString, "", field]
					   },"X3DChildNode"),

	###################################################################################

	#		Rendering Component

	###################################################################################

	Color => new VRML::NodeType("Color", { color => [MFColor, [], exposedField], },"X3DColorNode"),

	ColorRGBA => new VRML::NodeType("ColorRGBA", { color => [MFColorRGBA, [], exposedField], },"X3DColorNode"),

	Coordinate => new VRML::NodeType("Coordinate", { point => [MFVec3f, [], exposedField] },"X3DCoordinateNode"),

	IndexedLineSet => new VRML::NodeType("IndexedLineSet", {
						set_colorIndex => [MFInt32, undef, eventIn],
						set_coordIndex => [MFInt32, undef, eventIn],
						color => [SFNode, NULL, exposedField],
						coord => [SFNode, NULL, exposedField],
						colorIndex => [MFInt32, [], field],
						colorPerVertex => [SFBool, 1, field],
						coordIndex => [MFInt32, [], field],
						__vertArr  =>[FreeWRLPTR,0,field],
						__vertIndx  =>[FreeWRLPTR,0,field],
						__colours  =>[FreeWRLPTR,0,field],
						__vertices  =>[FreeWRLPTR,0,field],
						__vertexCount =>[FreeWRLPTR,0,field],
						__segCount =>[SFInt32,0,field],
					   },"X3DGeometryNode"),

	IndexedTriangleFanSet => new VRML::NodeType("IndexedTriangleFanSet", {
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
						texCoordIndex => [MFInt32, [], field],
						index => [MFInt32, [], field],
						fanCount => [MFInt32, [], field],
						stripCount => [MFInt32, [], field],

						set_height => [MFFloat, undef, eventIn],
						height => [MFFloat, [], field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field],
						__PolyStreamed => [SFBool, 0, field],
						},"X3DGeometryNode"),

	IndexedTriangleSet => new VRML::NodeType("IndexedTriangleSet", {
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
						texCoordIndex => [MFInt32, [], field],
						index => [MFInt32, [], field],
						fanCount => [MFInt32, [], field],
						stripCount => [MFInt32, [], field],

						set_height => [MFFloat, undef, eventIn],
						height => [MFFloat, [], field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field],
						__PolyStreamed => [SFBool, 0, field],
						},"X3DGeometryNode"),

	IndexedTriangleStripSet => new VRML::NodeType("IndexedTriangleStripSet", {
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
						texCoordIndex => [MFInt32, [], field],
						index => [MFInt32, [], field],
						fanCount => [MFInt32, [], field],
						stripCount => [MFInt32, [], field],

						set_height => [MFFloat, undef, eventIn],
						height => [MFFloat, [], field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field],
						__PolyStreamed => [SFBool, 0, field],
						},"X3DGeometryNode"),

	LineSet => new VRML::NodeType("LineSet", {
						color => [SFNode, NULL, exposedField],
						coord => [SFNode, NULL, exposedField],
						vertexCount => [MFInt32,[],exposedField],
						__vertArr  =>[FreeWRLPTR,0,field],
						__vertIndx  =>[FreeWRLPTR,0,field],
						__segCount =>[SFInt32,0,field],
					   },"X3DGeometryNode"),

	Normal => new VRML::NodeType("Normal", { vector => [MFVec3f, [], exposedField] },"X3DNormalNode"),

	PointSet => new VRML::NodeType("PointSet", {
						color => [SFNode, NULL, exposedField],
						coord => [SFNode, NULL, exposedField]
					   },"X3DGeometryNode"),

	TriangleFanSet => new VRML::NodeType("TriangleFanSet", {
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
						texCoordIndex => [MFInt32, [], field],
						index => [MFInt32, [], field],
						fanCount => [MFInt32, [], field],
						stripCount => [MFInt32, [], field],

						set_height => [MFFloat, undef, eventIn],
						height => [MFFloat, [], field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field],
						__PolyStreamed => [SFBool, 0, field],
						},"X3DGeometryNode"),

	TriangleStripSet => new VRML::NodeType("TriangleStripSet", {
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
						texCoordIndex => [MFInt32, [], field],
						index => [MFInt32, [], field],
						fanCount => [MFInt32, [], field],
						stripCount => [MFInt32, [], field],

						set_height => [MFFloat, undef, eventIn],
						height => [MFFloat, [], field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field],
						__PolyStreamed => [SFBool, 0, field],
						},"X3DGeometryNode"),

	TriangleSet => new VRML::NodeType("TriangleSet", {
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
						texCoordIndex => [MFInt32, [], field],
						index => [MFInt32, [], field],
						fanCount => [MFInt32, [], field],
						stripCount => [MFInt32, [], field],

						set_height => [MFFloat, undef, eventIn],
						height => [MFFloat, [], field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field],
						__PolyStreamed => [SFBool, 0, field],
						},"X3DGeometryNode"),


	###################################################################################

#			Shape Component

	###################################################################################

	Appearance => new VRML::NodeType ("Appearance", {
						 material => [SFNode, NULL, exposedField],
						 texture => [SFNode, NULL, exposedField],
						 textureTransform => [SFNode, NULL, exposedField],
						 lineProperties => [SFNode, NULL, exposedField],
						 fillProperties => [SFNode, NULL, exposedField],
						},"X3DAppearanceNode"),

	FillProperties => new VRML::NodeType ("FillProperties", {
						filled => [SFBool, 1, exposedField],
						hatchColor => [SFColor, [1,1,1], exposedField],
						hatched => [SFBool, 1, exposedField],
						hatchStyle => [SFInt32, 1, exposedField],
						},"X3DAppearanceChildNode"),

	LineProperties => new VRML::NodeType ("LineProperties", {
						applied => [SFBool, 1, exposedField],
						linetype => [SFInt32, 1, exposedField],
						linewidthScaleFactor => [SFFloat, 0, exposedField],
						},"X3DAppearanceChildNode"),

	Material => new VRML::NodeType ("Material", {
						 ambientIntensity => [SFFloat, 0.2, exposedField],
						 diffuseColor => [SFColor, [0.8, 0.8, 0.8], exposedField],
						 emissiveColor => [SFColor, [0, 0, 0], exposedField],
						 shininess => [SFFloat, 0.2, exposedField],
						 specularColor => [SFColor, [0, 0, 0], exposedField],
						 transparency => [SFFloat, 0, exposedField]
						},"X3DMaterialNode"),

	Shape => new VRML::NodeType ("Shape", {
						 appearance => [SFNode, NULL, exposedField],
						 geometry => [SFNode, NULL, exposedField],
						 bboxCenter => [SFVec3f, [0, 0, 0], field],
						 bboxSize => [SFVec3f, [-1, -1, -1], field],
						 __OccludeNumber =>[SFInt32,-1,field], # for Occlusion tests.
						},"X3DBoundedObject"),


	###################################################################################

	#		Geometry3D Component

	###################################################################################

	Box => new VRML::NodeType("Box", { 	size => [SFVec3f, [2, 2, 2], field],
						solid => [SFBool, 1, field],
						__points  =>[FreeWRLPTR,0,field],
					   },"X3DGeometryNode"),

	Cone => new VRML::NodeType ("Cone", {
						 bottomRadius => [SFFloat, 1.0, field],
						 height => [SFFloat, 2.0, field],
						 side => [SFBool, 1, field],
						 bottom => [SFBool, 1, field],
						solid => [SFBool, 1, field],
						 __sidepoints =>[FreeWRLPTR,0,field],
						 __botpoints =>[FreeWRLPTR,0,field],
						 __normals =>[FreeWRLPTR,0,field],
						},"X3DGeometryNode"),

	Cylinder => new VRML::NodeType ("Cylinder", {
						 bottom => [SFBool, 1, field],
						 height => [SFFloat, 2.0, field],
						 radius => [SFFloat, 1.0, field],
						 side => [SFBool, 1, field],
						 top => [SFBool, 1, field],
						solid => [SFBool, 1, field],
						 __points =>[FreeWRLPTR,0,field],
						 __normals =>[FreeWRLPTR,0,field],
						},"X3DGeometryNode"),

	ElevationGrid => new VRML::NodeType("ElevationGrid", {
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
						texCoordIndex => [MFInt32, [], field],
						index => [MFInt32, [], field],
						fanCount => [MFInt32, [], field],
						stripCount => [MFInt32, [], field],

						set_height => [MFFloat, undef, eventIn],
						height => [MFFloat, [], field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field],
						__PolyStreamed => [SFBool, 0, field],
					   },"X3DGeometryNode"),

	Extrusion => new VRML::NodeType("Extrusion", {
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
					   },"X3DGeometryNode"),

	IndexedFaceSet => new VRML::NodeType("IndexedFaceSet", {
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
						texCoordIndex => [MFInt32, [], field],
						index => [MFInt32, [], field],
						fanCount => [MFInt32, [], field],
						stripCount => [MFInt32, [], field],

						set_height => [MFFloat, undef, eventIn],
						height => [MFFloat, [], field],
						xDimension => [SFInt32, 0, field],
						xSpacing => [SFFloat, 1.0, field],
						zDimension => [SFInt32, 0, field],
						zSpacing => [SFFloat, 1.0, field],
						__PolyStreamed => [SFBool, 0, field],
					   },"X3DGeometryNode"),

	Sphere => new VRML::NodeType("Sphere",
					   { 	radius => [SFFloat, 1.0, field],
						solid => [SFBool, 1, field],
						 __points =>[FreeWRLPTR,0,field],
 					   },"X3DGeometryNode"),


	###################################################################################

	#		Geometry 2D Component

	###################################################################################

	Arc2D => new VRML::NodeType("Arc2D", {
					    	endAngle => [SFFloat, 1.5707, field],
					    	radius => [SFFloat, 1.0, field],
					    	startAngle => [SFFloat, 0.0, field],
						__points  =>[FreeWRLPTR,0,field],
						__numPoints =>[SFInt32,0,field],
 					   },"X3DGeometryNode"),

	ArcClose2D => new VRML::NodeType("ArcClose2D", {
						closureType => [SFString,"PIE",field],
					    	endAngle => [SFFloat, 1.5707, field],
					    	radius => [SFFloat, 1.0, field],
						solid => [SFBool, 0, field],
					    	startAngle => [SFFloat, 0.0, field],
						__points  =>[FreeWRLPTR,0,field],
						__numPoints =>[SFInt32,0,field],
 					   },"X3DGeometryNode"),


	Circle2D => new VRML::NodeType("Circle2D", {
					    	radius => [SFFloat, 1.0, field],
						__points  =>[FreeWRLPTR,0,field],
						__numPoints =>[SFInt32,0,field],
 					   },"X3DGeometryNode"),

	Disk2D => new VRML::NodeType("Disk2D", {
					    	innerRadius => [SFFloat, 0.0, field],
					    	outerRadius => [SFFloat, 1.0, field],
						solid => [SFBool, 0, field],
						__points  =>[FreeWRLPTR,0,field],
						__texCoords  =>[FreeWRLPTR,0,field],
						__numPoints =>[SFInt32,0,field],
						__simpleDisk => [SFBool,0,field],
 					   },"X3DGeometryNode"),

	Polyline2D => new VRML::NodeType("Polyline2D", {
					    	lineSegments => [MFVec2f, [], field],
 					   },"X3DGeometryNode"),

	Polypoint2D => new VRML::NodeType("Polypoint2D", {
					    	point => [MFVec2f, [], exposedField],
 					   },"X3DGeometryNode"),

	Rectangle2D => new VRML::NodeType("Rectangle2D", {
					    	size => [SFVec2f, [2.0, 2.0], field],
						solid => [SFBool, 0, field],
						__points  =>[FreeWRLPTR,0,field],
						__numPoints =>[SFInt32,0,field],
 					   },"X3DGeometryNode"),


	TriangleSet2D => new VRML::NodeType("TriangleSet2D", {
					    	vertices => [MFVec2f, [], exposedField],
						solid => [SFBool, 0, field],
						__texCoords  =>[FreeWRLPTR,0,field],
 					   },"X3DGeometryNode"),

	###################################################################################

	#		Text Component

	###################################################################################

	Text => new VRML::NodeType ("Text", {
						 string => [MFString, [], exposedField],
						 fontStyle => [SFNode, NULL, exposedField],
						 length => [MFFloat, [], exposedField],
						solid => [SFBool, 1, field],
						 maxExtent => [SFFloat, 0, exposedField],
						 __rendersub => [SFInt32, 0, exposedField] # Function ptr hack
						},"X3DTextNode"),

	FontStyle => new VRML::NodeType("FontStyle", {
						family => [MFString, ["SERIF"], field],
						horizontal => [SFBool, 1, field],
						justify => [MFString, ["BEGIN"], field],
						language => [SFString, "", field],
						leftToRight => [SFBool, 1, field],
						size => [SFFloat, 1.0, field],
						spacing => [SFFloat, 1.0, field],
						style => [SFString, "PLAIN", field],
						topToBottom => [SFBool, 1, field]
					   },"X3DFontStyleNode"), 

	###################################################################################

	#		Sound Component

	###################################################################################

	AudioClip => new VRML::NodeType("AudioClip", {
						description => [SFString, "", exposedField],
						loop =>	[SFBool, 0, exposedField],
						pitch => [SFFloat, 1.0, exposedField],
						startTime => [SFTime, 0, exposedField],
						stopTime => [SFTime, 0, exposedField],
						url => [MFString, [], exposedField],
						duration_changed => [SFTime, -1, eventOut],
						isActive => [SFBool, 0, eventOut],

						pauseTime => [SFTime,0,exposedField],
						resumeTime => [SFTime,0,exposedField],
						elapsedTime => [SFTime,0,eventOut],
						isPaused => [SFBool,0,eventOut],

						# parent url, gets replaced at node build time
						__parenturl =>[SFString,"",field],

						# internal sequence number
						__sourceNumber => [SFInt32, -1, field],
						# local name, as received on system
						__localFileName => [FreeWRLPTR, 0,field],
						# time that we were initialized at
						__inittime => [SFTime, 0, field],
					   },"X3DSoundSourceNode"),

	Sound => new VRML::NodeType("Sound", {
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
					   },"X3DSoundSourceNode"),

	###################################################################################

	#		Lighting Component

	###################################################################################

	DirectionalLight => new VRML::NodeType("DirectionalLight", {
						ambientIntensity => [SFFloat, 0, exposedField],
						color => [SFColor, [1, 1, 1], exposedField],
						direction => [SFVec3f, [0, 0, -1], exposedField],
						intensity => [SFFloat, 1.0, exposedField],
						on => [SFBool, 1, exposedField]
					   },"X3DLightNode"),

	PointLight => new VRML::NodeType("PointLight", {
						ambientIntensity => [SFFloat, 0, exposedField],
						attenuation => [SFVec3f, [1, 0, 0], exposedField],
						color => [SFColor, [1, 1, 1], exposedField],
						intensity => [SFFloat, 1.0, exposedField],
						location => [SFVec3f, [0, 0, 0], exposedField],
						on => [SFBool, 1, exposedField],
						radius => [SFFloat, 100.0, exposedField],
						##not in the spec
						direction => [SFVec3f, [0, 0, -1.0], exposedField]
					   },"X3DLightNode"),

	SpotLight => new VRML::NodeType("SpotLight", {
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
					   },"X3DLightNode"),

	###################################################################################

	#		Texturing Component

	###################################################################################

	ImageTexture => new VRML::NodeType("ImageTexture", {
						url => [MFString, [], exposedField],
						repeatS => [SFBool, 1, field],
						repeatT => [SFBool, 1, field],
						__textureTableIndex => [SFInt32, 0, field],
						__parenturl =>[SFString,"",field],
					   },"X3DTextureNode"),

	MovieTexture => new VRML::NodeType ("MovieTexture", {
						 loop => [SFBool, 0, exposedField],
						 speed => [SFFloat, 1.0, exposedField],
						 startTime => [SFTime, 0, exposedField],
						 stopTime => [SFTime, 0, exposedField],
						 url => [MFString, [""], exposedField],
						 repeatS => [SFBool, 1, field],
						 repeatT => [SFBool, 1, field],
						 duration_changed => [SFTime, -1, eventOut],
						 isActive => [SFBool, 0, eventOut],
						resumeTime => [SFTime,0,exposedField],
						pauseTime => [SFTime,0,exposedField],
						elapsedTime => [SFTime,0,eventOut],
						isPaused => [SFTime,0,eventOut],
						__textureTableIndex => [SFInt32, 0, field],
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
						},"X3DTextureNode"),


	MultiTexture => new VRML::NodeType("MultiTexture", {
						alpha =>[SFFloat, 1, exposedField],
						color =>[SFColor,[1,1,1],exposedField],
						function =>[MFString,[],exposedField],
						mode =>[MFString,[],exposedField],
						source =>[MFString,[],exposedField],
						texture=>[MFNode,undef,exposedField],
						__params => [FreeWRLPTR, 0, field],
					   },"X3DTextureNode"),

	MultiTextureCoordinate => new VRML::NodeType("MultiTextureCoordinate", {
						texCoord =>[MFNode,undef,exposedField],
					   },"X3DTextureCoordinateNode"),

	MultiTextureTransform => new VRML::NodeType("MultiTextureTransform", {
						textureTransform=>[MFNode,undef,exposedField],
					   },"X3DTextureTransformNode"),

	PixelTexture => new VRML::NodeType("PixelTexture", {
						image => [SFImage, "0, 0, 0", exposedField],
						repeatS => [SFBool, 1, field],
						repeatT => [SFBool, 1, field],
						__parenturl =>[SFString,"",field],
						__textureTableIndex => [SFInt32, 0, field],
					   },"X3DTextureNode"),

	TextureCoordinate => new VRML::NodeType("TextureCoordinate", { 
						point => [MFVec2f, [], exposedField],
						__compiledpoint => [MFVec2f, [], field],
					 },"X3DTextureCoordinateNode"),

	TextureCoordinateGenerator => new VRML::NodeType("TextureCoordinateGenerator", { 
						parameter => [MFFloat, [], exposedField],
						mode => [SFString,"SPHERE",exposedField],
						__compiledmode => [SFInt32,0,field],
					 },"X3DTextureCoordinateNode"),

	TextureTransform => new VRML::NodeType ("TextureTransform", {
						 center => [SFVec2f, [0, 0], exposedField],
						 rotation => [SFFloat, 0, exposedField],
						 scale => [SFVec2f, [1, 1], exposedField],
						 translation => [SFVec2f, [0, 0], exposedField]
						},"X3DTextureTransformNode"),


	###################################################################################

	#		Interpolation Component

	###################################################################################

	ColorInterpolator => new VRML::NodeType("ColorInterpolator", {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFColor, [], exposedField],
			value_changed => [SFColor, [0, 0, 0], eventOut],
		   },"X3DInterpolatorNode"),

	############################################################################33
	# CoordinateInterpolators and NormalInterpolators use almost the same thing;
	# look at the _type field.

	CoordinateInterpolator => new VRML::NodeType("CoordinateInterpolator", {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFVec3f, [], exposedField],
			value_changed => [MFVec3f, [], eventOut],
			_type => [SFInt32, 0, exposedField], #1 means dont normalize
		},"X3DInterpolatorNode"),

	NormalInterpolator => new VRML::NodeType("NormalInterpolator", {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFVec3f, [], exposedField],
			value_changed => [MFVec3f, [], eventOut],
			_type => [SFInt32, 1, exposedField], #1 means normalize
		},"X3DInterpolatorNode"),

	OrientationInterpolator => new VRML::NodeType("OrientationInterpolator", {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFRotation, [], exposedField],
			value_changed => [SFRotation, [0, 0, 1, 0], eventOut]
		   },"X3DInterpolatorNode"),

	PositionInterpolator => new VRML::NodeType("PositionInterpolator", {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFVec3f, [], exposedField],
			value_changed => [SFVec3f, [0, 0, 0], eventOut],
		   },"X3DInterpolatorNode"),


	ScalarInterpolator => new VRML::NodeType("ScalarInterpolator", {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFFloat, [], exposedField],
			value_changed => [SFFloat, 0.0, eventOut]
		   },"X3DInterpolatorNode"),

	CoordinateInterpolator2D => new VRML::NodeType("CoordinateInterpolator2D", {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFVec2f, [], exposedField],
			value_changed => [MFVec2f, [[0,0]], eventOut],
		},"X3DInterpolatorNode"),

	PositionInterpolator2D => new VRML::NodeType("PositionInterpolator2D", {
			set_fraction => [SFFloat, undef, eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFVec2f, [], exposedField],
			value_changed => [SFVec2f, [0, 0, 0], eventOut],
		},"X3DInterpolatorNode"),


	###################################################################################

	#		Pointing Device Component

	###################################################################################

	TouchSensor => new VRML::NodeType("TouchSensor", {
						enabled => [SFBool, 1, exposedField],
						hitNormal_changed => [SFVec3f, [0, 0, 0], eventOut],
						hitPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						hitTexCoord_changed => [SFVec2f, [0, 0], eventOut],
						isActive => [SFBool, 0, eventOut],
						isOver => [SFBool, 0, eventOut],
						description => [SFString, "", field],
						touchTime => [SFTime, -1, eventOut]
					   },"X3DPointingDeviceSensorNode"),

	PlaneSensor => new VRML::NodeType("PlaneSensor", {
						autoOffset => [SFBool, 1, exposedField],
						enabled => [SFBool, 1, exposedField],
						maxPosition => [SFVec2f, [-1, -1], exposedField],
						minPosition => [SFVec2f, [0, 0], exposedField],
						offset => [SFVec3f, [0, 0, 0], exposedField],
						isActive => [SFBool, 0, eventOut],
						isOver => [SFBool, 0, eventOut],
						description => [SFString, "", field],
						trackPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						translation_changed => [SFVec3f, [0, 0, 0], eventOut],
						# where we are at a press...
						_origPoint => [SFVec3f, [0, 0, 0], field],
					   },"X3DPointingDeviceSensorNode"),

	SphereSensor => new VRML::NodeType("SphereSensor", {
						autoOffset => [SFBool, 1, exposedField],
						enabled => [SFBool, 1, exposedField],
						offset => [SFRotation, [0, 1, 0, 0], exposedField],
						isActive => [SFBool, 0, eventOut],
						rotation_changed => [SFRotation, [0, 0, 1, 0], eventOut],
						trackPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						isOver => [SFBool, 0, eventOut],
						description => [SFString, "", field],
						# where we are at a press...
						_origPoint => [SFVec3f, [0, 0, 0], field],
						_radius => [SFFloat, 0, field],
					   },"X3DPointingDeviceSensorNode"),

	CylinderSensor => new VRML::NodeType("CylinderSensor", {
						autoOffset => [SFBool, 1, exposedField],
						diskAngle => [SFFloat, 0.262, exposedField],
						enabled => [SFBool, 1, exposedField],
						maxAngle => [SFFloat, -1.0, exposedField],
						minAngle => [SFFloat, 0, exposedField],
						offset => [SFFloat, 0, exposedField],
						isActive => [SFBool, 0, eventOut],
						isOver => [SFBool, 0, eventOut],
						description => [SFString, "", field],
						rotation_changed => [SFRotation, [0, 0, 1, 0], eventOut],
						trackPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						# where we are at a press...
						_origPoint => [SFVec3f, [0, 0, 0], field],
						_radius => [SFFloat, 0, field],
					   },"X3DPointingDeviceSensorNode"),


	###################################################################################

	#		Key Device Component

	###################################################################################

	# KeySensor
	# StringSensor

	###################################################################################

	#		Environmental Sensor Component

	###################################################################################


	ProximitySensor => new VRML::NodeType("ProximitySensor", {
						center => [SFVec3f, [0, 0, 0], exposedField],
						size => [SFVec3f, [0, 0, 0], exposedField],
						enabled => [SFBool, 1, exposedField],
						isActive => [SFBool, 0, eventOut],
						position_changed => [SFVec3f, [0, 0, 0], eventOut],
						orientation_changed => [SFRotation, [0, 0, 1, 0], eventOut],
						enterTime => [SFTime, -1, eventOut],
						exitTime => [SFTime, -1, eventOut],
						centerOfRotation_changed =>[SFVec3f, [0,0,0], eventOut],

						# These fields are used for the info.
						__hit => [SFInt32, 0, exposedField],
						__t1 => [SFVec3f, [10000000, 0, 0], exposedField],
						__t2 => [SFRotation, [0, 1, 0, 0], exposedField]
					   },"X3DEnvironmentalSensorNode"),

	VisibilitySensor => new VRML::NodeType("VisibilitySensor", {
						center => [SFVec3f, [0, 0, 0], exposedField],
						enabled => [SFBool, 1, exposedField],
						size => [SFVec3f, [0, 0, 0], exposedField],
						enterTime => [SFTime, -1, eventOut],
						exitTime => [SFTime, -1, eventOut],
						isActive => [SFBool, 0, eventOut],
						 __OccludeNumber =>[SFInt32,-1,field], 	# for Occlusion tests.
						__points  =>[FreeWRLPTR,0,field],	# for Occlude Box.
						__Samples =>[SFInt32,0,field],		# Occlude samples from last pass
					   },"X3DEnvironmentalSensorNode"),



	###################################################################################

	#		Navigation Component

	###################################################################################

	LOD => new VRML::NodeType("LOD", {
						 addChildren => [MFNode, undef, eventIn],
						 removeChildren => [MFNode, undef, eventIn],
						level => [MFNode, [], exposedField],
						center => [SFVec3f, [0, 0, 0],  field],
						range => [MFFloat, [], field],
						_selected =>[SFInt32,0,field],
					   },"X3DGroupingNode"),

	Billboard => new VRML::NodeType("Billboard", {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						axisOfRotation => [SFVec3f, [0, 1, 0], exposedField],
						children => [MFNode, [], exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field]
					   },"X3DGroupingNode"),

	Collision => new VRML::NodeType("Collision", {
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
					   },"X3DEnvironmentalSensorNode"),

	Viewpoint => new VRML::NodeType("Viewpoint", {
						set_bind => [SFBool, undef, eventIn],
						fieldOfView => [SFFloat, 0.785398, exposedField],
						jump => [SFBool, 1, exposedField],
						orientation => [SFRotation, [0, 0, 1, 0], exposedField],
						position => [SFVec3f,[0, 0, 10], exposedField],
						description => [SFString, "", field],
						bindTime => [SFTime, -1, eventOut],
						isBound => [SFBool, 0, eventOut],
						centerOfRotation =>[SFVec3f, [0,0,0], exposedField],
						__BGNumber => [SFInt32,-1,field], # for ordering backgrounds for binding
					   },"X3DBindableNode"),

	NavigationInfo => new VRML::NodeType("NavigationInfo", {
						set_bind => [SFBool, undef, eventIn],
						avatarSize => [MFFloat, [0.25, 1.6, 0.75], exposedField],
						headlight => [SFBool, 1, exposedField],
						speed => [SFFloat, 1.0, exposedField],
						type => [MFString, ["WALK", "ANY"], exposedField],
						visibilityLimit => [SFFloat, 0, exposedField],
						isBound => [SFBool, 0, eventOut],
						transitionType => [MFString, [],exposedField],
						bindTime => [SFTime, -1, eventOut],
						__BGNumber => [SFInt32,-1,field], # for ordering backgrounds for binding
					   },"X3DBindableNode"),

	###################################################################################

	#		Environmental Effects Component

	###################################################################################

	Fog => new VRML::NodeType("Fog", {
						set_bind => [SFBool, undef, eventIn],
						color => [SFColor, [1, 1, 1], exposedField],
						fogType => [SFString, "LINEAR", exposedField],
						visibilityRange => [SFFloat, 0, exposedField],
						isBound => [SFBool, 0, eventOut],
						__BGNumber => [SFInt32,-1,field], # for ordering backgrounds for binding
					   },"X3DBindableNode"),

	TextureBackground => new VRML::NodeType("TextureBackground", {
						set_bind => [SFBool, undef, eventIn],
						groundAngle => [MFFloat, [], exposedField],
						groundColor => [MFColor, [], exposedField],
						skyAngle => [MFFloat, [], exposedField],
						skyColor => [MFColor, [[0,0,0]], exposedField],
						bindTime => [SFTime,0,eventOut],
						isBound => [SFBool, 0, eventOut],
						__parenturl =>[SFString,"",field],
						__points =>[FreeWRLPTR,0,field],
						__colours =>[FreeWRLPTR,0,field],
						__quadcount => [SFInt32,0,field],
						__BGNumber => [SFInt32,-1,field], # for ordering backgrounds for binding

						frontTexture=>[SFNode,NULL,exposedField],
						backTexture=>[SFNode,NULL,exposedField],
						topTexture=>[SFNode,NULL,exposedField],
						bottomTexture=>[SFNode,NULL,exposedField],
						leftTexture=>[SFNode,NULL,exposedField],
						rightTexture=>[SFNode,NULL,exposedField],
						transparency=> [MFFloat,[0],exposedField],
					   },"X3DBackgroundNode"),

	Background => new VRML::NodeType("Background", {
						set_bind => [SFBool, undef, eventIn],
						groundAngle => [MFFloat, [], exposedField],
						groundColor => [MFColor, [], exposedField],
						skyAngle => [MFFloat, [], exposedField],
						skyColor => [MFColor, [[0, 0, 0]], exposedField],
						bindTime => [SFTime,0,eventOut],
						isBound => [SFBool, 0, eventOut],
						__parenturl =>[SFString,"",field],
						__points =>[FreeWRLPTR,0,field],
						__colours =>[FreeWRLPTR,0,field],
						__quadcount => [SFInt32,0,field],
						__BGNumber => [SFInt32,-1,field], # for ordering backgrounds for binding

						(map {(
							   $_.Url => [MFString, [], exposedField],
							   # OpenGL texture number
							   __texture.$_ => [SFInt32, 0, exposedField],
							  )} qw/back front top bottom left right/),
					   },"X3DBackgroundNode"),

	###################################################################################

	#		Geospatial Component

	###################################################################################


	GeoCoordinate => new VRML::NodeType("GeoCoordinate", {
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						point => [MFString,[],field],
					},"X3DCoordinateNode"),

	GeoElevationGrid => new VRML::NodeType("GeoElevationGrid", {
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
					},"X3DGeometryNode"),

	GeoLOD => new VRML::NodeType("GeoLOD", {
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
					},"X3DGroupingNode"),


	GeoMetadata=> new VRML::NodeType("GeoMetadata", {
						data => [MFNode,[],exposedField],
						summary => [MFString,[],exposedField],
						url => [MFString,[],exposedField],
					},"X3DChildNode"),

	GeoPositionInterpolator=> new VRML::NodeType("GeoPositionInterpolator", {
						set_fraction => [SFFloat,undef,eventIn],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						key => [MFFloat,[],field],
						keyValue => [MFString,[],field],
						geovalue_changed => [SFString,"",eventOut],
						value_changed => [SFVec3f,[0,0,0],eventOut],
					},"X3DInterpolatorNode"),


	GeoTouchSensor=> new VRML::NodeType("GeoTouchSensor", {
						enabled => [SFBool,1,exposedField],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						hitNormal_changed => [SFVec3f, [0, 0, 0], eventOut],
						hitPoint_changed => [SFVec3f, [0, 0, 0], eventOut],
						hitTexCoord_changed => [SFVec2f, [0, 0], eventOut],
						hitGeoCoord_changed => [SFString,"",eventOut],
						isActive => [SFBool, 0, eventOut],
						isOver => [SFBool, 0, eventOut],
						description => [SFString, "", field],
						touchTime => [SFTime, -1, eventOut]
					},"X3DPointingDeviceSensorNode"),

	GeoViewpoint => new VRML::NodeType("GeoViewpoint", {
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
						__BGNumber => [SFInt32,-1,field], # for ordering backgrounds for binding

						# "compiled" versions of strings above
						__position => [SFVec3f,[0, 0, 100000], field],
						__geoSystem => [SFInt32,0,field],
					   },"X3DBindableNode"),

	GeoOrigin => new VRML::NodeType("GeoOrigin", {
						geoSystem => [MFString,["GD","WE"],exposedField],
						geoCoords => [SFString,"",exposedField],
						rotateYUp => [SFBool,0,field],

						# these are now static in CFuncs/GeoVRML.c
						# "compiled" versions of strings above
						#__geoCoords => [SFVec3f,[0, 0, 0], field],
						#__geoSystem => [SFInt32,0,field],
					},"X3DChildNode"),

	GeoLocation => new VRML::NodeType("GeoLocation", {
						geoCoords => [SFString,"",exposedField],
						children => [MFNode, [], field],
						geoOrigin => [SFNode, NULL, field],
						geoSystem => [MFString,["GD","WE"],field],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],

						# "compiled" versions of strings above
						__geoCoords => [SFVec3f,[0, 0, 0], field],
						__geoSystem => [SFInt32,0,field],

					},"X3DGroupingNode"),


	###################################################################################

	#		H-Anim Component

	###################################################################################

	HAnimDisplacer => new VRML::NodeType("HAnimDisplacer", {
						coordIndex => [MFInt32, [], exposedField],
						displacements => [MFVec3f, [], exposedField],
						name => [SFString, "", exposedField],
						weight => [SFFloat, 0.0, exposedField],
					},"X3DGeometricPropertyNode"),

	HAnimHumanoid => new VRML::NodeType("HAnimHumanoid", {
						center => [SFVec3f, [0, 0, 0], exposedField],
						info => [MFString, [],exposedField],
						joints => [MFNode,[],exposedField],
						name => [SFString, "", exposedField],
						rotation => [SFRotation,[0,0,1,0], exposedField],
						scale => [SFVec3f,[1,1,1],exposedField],
						scaleOrientation => [SFRotation, [0, 0, 1, 0], exposedField],
						segments => [MFNode,[],exposedField],
						sites => [MFNode,[],exposedField],
						skeleton => [MFNode,[],exposedField],
						skin => [MFNode,[],exposedField],
						skinCoord => [SFNode, NULL, exposedField],
						skinNormal => [SFNode, NULL, exposedField],
						translation => [SFVec3f, [0, 0, 0], exposedField],
						version => [SFString,"",exposedField],
						viewpoints => [MFNode,[],exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
					},"X3DChildNode"),

	HAnimJoint => new VRML::NodeType("HAnimJoint", {

						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						center => [SFVec3f, [0, 0, 0], exposedField],
						children => [MFNode, [], exposedField],
						rotation => [SFRotation, [0, 0, 1, 0], exposedField],
						scale => [SFVec3f, [1, 1, 1], exposedField],
						scaleOrientation => [SFRotation, [0, 0, 1, 0], exposedField],
						translation => [SFVec3f, [0, 0, 0], exposedField],
						displacers => [MFNode, [], exposedField],
						limitOrientation => [SFRotation, [0, 0, 1, 0], exposedField],
						llimit => [MFFloat,[],exposedField],
						name => [SFString, "", exposedField],
						skinCoordIndex => [MFInt32,[],exposedField],
						skinCoordWeight => [MFFloat,[],exposedField],
						stiffness => [MFFloat,[0,0,0],exposedField],
						ulimit => [MFFloat,[],exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],

						 # fields for reducing redundant calls
						 __do_center => [SFInt32, 0, field],
						 __do_trans => [SFInt32, 0, field],
						 __do_rotation => [SFInt32, 0, field],
						 __do_scaleO => [SFInt32, 0, field],
						 __do_scale => [SFInt32, 0, field],
					},"X3DChildNode"),

	HAnimSegment => new VRML::NodeType("HAnimSegment", {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						name => [SFString, "", exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						centerOfMass => [SFVec3f, [0, 0, 0], exposedField],
						coord => [SFNode, NULL, exposedField],
						displacers => [MFNode,[],exposedField],
						mass => [SFFloat, 0, exposedField],
						momentsOfInertia =>[MFFloat, [0, 0, 0, 0, 0, 0, 0, 0, 0],exposedField],
					},"X3DChildNode"),



	HAnimSite => new VRML::NodeType("HAnimSite", {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
						name => [SFString, "", exposedField],
						bboxCenter => [SFVec3f, [0, 0, 0], field],
						bboxSize => [SFVec3f, [-1, -1, -1], field],
						center => [SFVec3f, [0, 0, 0], exposedField],
						children => [MFNode, [], exposedField],
						rotation => [SFRotation, [0, 0, 1, 0], exposedField],
						scale => [SFVec3f, [1, 1, 1], exposedField],
						scaleOrientation => [SFRotation, [0, 0, 1, 0], exposedField],
						translation => [SFVec3f, [0, 0, 0], exposedField],

						 # fields for reducing redundant calls
						 __do_center => [SFInt32, 0, field],
						 __do_trans => [SFInt32, 0, field],
						 __do_rotation => [SFInt32, 0, field],
						 __do_scaleO => [SFInt32, 0, field],
						 __do_scale => [SFInt32, 0, field],
					},"X3DGroupingNode"),


	###################################################################################

	#		NURBS Component

	###################################################################################

	Contour2D => new VRML::NodeType("Contour2D", {
						addChildren => [MFNode, undef, eventIn],
						removeChildren => [MFNode, undef, eventIn],
						children => [MFNode, [], exposedField],
					},"X3DParametricGeometryNode"),


	ContourPolyLine2D =>
	new VRML::NodeType("ContourPolyline2D",
					{
					},"X3DParametricGeometryNode"
					),

	NurbsCurve =>
	new VRML::NodeType("NurbsCurve",
					{
						controlPoint =>[MFVec3f,[],exposedField],
						weight => [MFFloat,[],exposedField],
						tessellation => [SFInt32,0,exposedField],
						knot => [MFFloat,[],field],
						order => [SFInt32,3,field],
					},"X3DParametricGeometryNode"
					),

	NurbsCurve2D =>
	new VRML::NodeType("NurbsCurve2D",
					{
						controlPoint =>[MFVec2f,[],exposedField],
						weight => [MFFloat,[],exposedField],
						tessellation => [SFInt32,0,exposedField],
						knot => [MFFloat,[],field],
						order => [SFInt32,3,field],
					},"X3DParametricGeometryNode"
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
					},"X3DGroupingNode"
					),

	NurbsPositionInterpolator =>
	new VRML::NodeType("NurbsPositionInterpolator",
					{

						set_fraction => [SFFloat,undef,eventIn],
						dimension => [SFInt32,0,exposedField],
						keyValue => [MFVec3f,[],exposedField],
						keyWeight => [MFFloat,[],exposedField],
						knot => [MFFloat,[],exposedField],
						order => [SFInt32,0,exposedField],
						value_changed => [SFVec3f,[0,0,0],eventOut],
					},"X3DInterpolatorNode"
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
					},"X3DParametricGeometryNode"
					),
	NurbsTextureSurface =>
	new VRML::NodeType("NurbsTextureSurface",
					{
					},"X3DParametricGeometryNode"
					),

	NurbsTrimmedSurface =>
	new VRML::NodeType("NurbsTrimmedSurface",
					{
					},"X3DParametricGeometryNode"
					),


	###################################################################################

	#		Scripting Component

	###################################################################################
	Script =>
	new VRML::NodeType("Script",
					   {
						url => [MFString, [], exposedField],
						directOutput => [SFBool, 0, field],
						mustEvaluate => [SFBool, 0, field],
						 __scriptObj => [FreeWRLPTR, 0, field],
					   },"X3DScriptNode"
					  ),

	###################################################################################

	#		EventUtilities Component

	###################################################################################

	BooleanFilter => 
	new VRML::NodeType("BooleanFilter", {
			set_boolean =>[SFBool,undef,eventIn],
			inputFalse => [SFBool, 0, eventOut],
			inputNegate => [SFBool, 0, eventOut],
			inputTrue => [SFBool, 1, eventOut],
	},"X3DChildNode"),


	BooleanSequencer => 
	new VRML::NodeType("BooleanSequencer", {
			next =>[SFBool,undef,eventIn],
			previous =>[SFBool,undef,eventIn],
			set_fraction =>[SFFloat,undef,eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFBool, [], exposedField],
			value_changed => [SFBool, 0, eventOut],
	},"X3DSequencerNode"),


	BooleanToggle => 
	new VRML::NodeType("BooleanToggle", {
			set_boolean =>[SFBool,undef,eventIn],
			toggle => [SFBool, 0, eventOut],
	},"X3DChildNode"),


	BooleanTrigger => 
	new VRML::NodeType("BooleanTrigger", {
			set_triggerTime => [SFTime,undef ,eventIn],
			triggerTrue => [SFBool, 0, eventOut],
	},"X3DTriggerNode"),


	IntegerSequencer => 
	new VRML::NodeType("IntegerSequencer", {
			next =>[SFBool,undef,eventIn],
			previous =>[SFBool,undef,eventIn],
			set_fraction =>[SFFloat,undef,eventIn],
			key => [MFFloat, [], exposedField],
			keyValue => [MFInt32, [], exposedField],
			value_changed => [SFInt32, 0, eventOut],
	},"X3DSequencerNode"),

	IntegerTrigger => 
	new VRML::NodeType("IntegerTrigger", {
			set_triggerTime => [SFTime,undef ,eventIn],
			integerKey => [SFInt32, 0, exposedField],
			triggerValue => [SFInt32, 0, eventOut],
	},"X3DTriggerNode"),

	TimeTrigger => 
	new VRML::NodeType("TimeTrigger", {
			set_boolean =>[SFBool,undef,eventIn],
			triggerTime => [SFTime, 0, eventOut],
	},"X3DTriggerNode"),



	###################################################################################

	#		Old VRML nodes.

	###################################################################################
	# Internal structures, to store def and use in the right way

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
					} ,"X3DChildNode"
					),

	###################################################################################

	# testing...

	###################################################################################

	MidiKey =>
	new VRML::NodeType("MidiKey",
					{
						deviceName => [SFString,"",exposedField],	# "Subtractor 1"
						_deviceNameIndex => [SFInt32, -99, field],	#  name in name table index


						# encoded bus,device,channel
						_bus => [SFInt32,0,field],			# internal for efficiency
						_channel => [SFInt32,0,field],			# internal for efficiency

						keyValue => [SFInt32, 0, exposedField],		#  
						keyVelocity => [SFInt32, 100, exposedField],	# 
						press => [SFBool,undef,eventIn],			# press event
						isPressed =>[SFBool,FALSE,eventOut],		# press event

						devicePresent => [SFBool, FALSE, exposedField],	# TRUE when ReWire is working
					}, "X3DNetworkSensorNode"
					),

	MidiControl =>
	new VRML::NodeType("MidiControl",
					{
						deviceName => [SFString,"",exposedField],	# "Subtractor 1"
						channel => [SFString,"",exposedField],		# "Osc1 Wave"
						_deviceNameIndex => [SFInt32, -99, field],	#  name in name table index
						_channelIndex => [SFInt32, -99, field],		#  name in name table index


						# encoded bus,device,channel
						_bus => [SFInt32,0,field],			# internal for efficiency
						_channel => [SFInt32,0,field],			# internal for efficiency
						_controller => [SFInt32,0,field],		# internal for efficiency

						deviceMinVal => [SFInt32, 0, field],		# what the device sets
						deviceMaxVal => [SFInt32, 0, field],		# what the device sets

						minVal => [SFInt32, 0, exposedField],		# used to scale floats, and 
						maxVal => [SFInt32, 10000, exposedField],		# bounds check ints. The resulting
												# value will be <= maxVal <= deviceMaxVal
												# and >=minVal >= deviceMinVal

						intValue => [SFInt32, 0, exposedField],		# integer value for i/o
						_oldintValue => [SFInt32, -99, field],		# old integer value for i/o
						floatValue => [SFFloat, 0, exposedField],	# float value for i/o
						useIntValue => [SFBool, TRUE, exposedField],	# which value to use for input

						continuousEvents => [SFBool, TRUE, exposedField],# do we send an event per event loop, or
												# only when the intValue changes?

						highResolution => [SFBool, TRUE, exposedField],	# high resolution controller
						controllerType => [SFString, "", exposedField],	# "Continuous" "Step" "Bipolar" "Unknown"
						intControllerType => [SFInt32,0, exposedField], # use ReWire definitions
						controllerPresent => [SFBool, FALSE, exposedField],	# TRUE when ReWire is working
					}, "X3DNetworkSensorNode"
					),
); 


1;

