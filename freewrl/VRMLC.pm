# $Id: VRMLC.pm,v 1.107 2003/07/31 17:07:30 crc_canada Exp $
#
# Copyright (C) 1998 Tuomas J. Lukka 1999 John Stewart CRC Canada
# Portions Copyright (C) 1998 Bernhard Reiter
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.

# The C routines to render various nodes quickly
#
# Field values by subs so that generalization possible..
#
# getf(Node,"fieldname",[index,...]) returns c string to get field name.
# getaf(Node,"fieldname",n) returns comma-separated list of all the field values.
# getfn(Node,"fieldname"
#
# Of these, VP is taken into account by Transform
#
# Why so elaborate code generation?
#  - makes it easy to change structs later
#  - makes it very easy to add fast implementations for new proto'ed 
#    node types
#
#
# TODO:
#  Test indexedlineset
#
# $Log: VRMLC.pm,v $
# Revision 1.107  2003/07/31 17:07:30  crc_canada
# delays (to restrict rendering speed) moved to C.
#
# Revision 1.106  2003/07/30 18:45:38  crc_canada
# set_viewer_type call modified.
#
# Revision 1.105  2003/07/22 16:04:48  ayla
#
# Fix of ROUTE implementation problems with PROTO and Script nodes, take 1.
#
# Revision 1.104  2003/07/18 17:15:43  crc_canada
# added a getProperty type of call for Javascript
#
# Revision 1.103  2003/07/17 13:42:48  crc_canada
# EAI work
#
# Revision 1.102  2003/07/15 18:51:48  crc_canada
# EAI in C work
#
# Revision 1.101  2003/07/15 14:04:08  crc_canada
# Viewer selection work
#
# Revision 1.100  2003/07/09 18:30:34  crc_canada
# removeChildren now possible for a normal ROUTE.
#
# Revision 1.99  2003/07/09 18:12:31  crc_canada
# added function to allow Javascript with DEF'd parameter to add/remove children through this param.
#
# Revision 1.98  2003/07/07 19:08:20  ayla
#
# Billboard calculation uses ModelView matrix again, which hopefully fixes
# Billboard node problems once and for all.
# Fixed function get_timestamp() in VRMLC.pm and removed time as argument to a
# number of Viewer functions since TickTime is a global.
#
# Revision 1.97  2003/07/03 20:01:20  ayla
# Added include for new header file.
#
# Revision 1.96  2003/06/20 07:12:24  ayla
# Code cleanup and attempts to silence some C compile warnings.
#
# Revision 1.95  2003/06/19 20:48:01  crc_canada
# FPS calculations now performed in C.
#
# Revision 1.94  2003/06/16 17:06:42  crc_canada
# save Browser URL and Version to C.
#
# Revision 1.93  2003/06/13 21:45:37  ayla
# Migrating more code from Perl to C.
# Removed UNUSED macro from jsUtils.h.
#
# Revision 1.92  2003/06/12 18:21:47  ayla
#
# Re-enabled '/' key function which is to print Viewer information.
#
# Revision 1.91  2003/06/06 20:22:49  ayla
#
# Migrating more OpenGL module code from XS to C.
#
# Revision 1.90  2003/06/02 18:21:16  crc_canada
# more work on CRoutes for Scripting
#
# Revision 1.89  2003/05/28 14:54:15  crc_canada
# Javascript interface moved mainly to C code in CFuncs dir.
#
# Revision 1.88  2003/05/17 05:54:30  ayla
#
# Changes needed to support the port of Viewer and Quaternion Perl code to C - pass 1.
# No doubt there will also be problems with this, and there are blocks of code that have been disabled for now.
#
# Revision 1.87  2003/05/14 18:03:32  crc_canada
# Collision now done in C
#
# Revision 1.86  2003/05/14 17:30:58  crc_canada
# ProximitySensor now in C
#
# Revision 1.85  2003/05/14 15:42:43  crc_canada
# Anchor code first pass
#
# Revision 1.84  2003/05/12 19:02:38  crc_canada
# Bindables in C, part II
#
# Revision 1.83  2003/05/08 17:30:02  crc_canada
# Missed declaration of saved hitpoint
#
# Revision 1.82  2003/05/01 18:49:19  ayla
#
# Use viewer position and orientation from Viewer.pm for calculations in
# the rendering loop.
#
# Revision 1.81  2003/04/29 19:56:45  crc_canada
# MovieTexture rendering code now in C
#
# Revision 1.80  2003/04/29 17:12:43  crc_canada
# TimeSensor ClockTick code now in C
#
# Revision 1.79  2003/04/28 19:40:25  crc_canada
# AudioClip ClockTick now in C
#
# Revision 1.78  2003/04/25 15:50:24  crc_canada
# Interpolators now in C
#
# Revision 1.77  2003/04/09 20:40:53  crc_canada
# font paths now determined entirely in freewrl.PL; the save_font_path
# function only serves to save it within the C side of FreeWRL.
#
# Revision 1.76  2003/04/09 16:30:57  sdumoulin
# Changed for Aqua build
#
# Revision 1.75  2003/04/04 15:24:24  crc_canada
# set_stereo_offset C function added - uses fieldofview variable now.
# (works reliably only with 45 degrees; algorithm will have to be tweaked
# for other angles)
#
# Revision 1.74  2003/04/03 20:04:37  crc_canada
# cleanup and bounds check fieldOfView
#
# Revision 1.73  2003/04/03 17:29:01  crc_canada
# setup_projection function to take advantage of animated Viewpoints
#
# Revision 1.72  2003/04/01 20:08:46  crc_canada
# zpl code removed - see freewrl.PL.
#
# Revision 1.71  2003/03/19 15:23:17  crc_canada
# ccw field for complex geometry working according to spec now.
#
# Revision 1.70  2003/03/17 16:16:13  crc_canada
# Code for locating DirectionalLights was broken - code removed, as all is
# now handled from VRMLRend.pm
#
# Revision 1.69  2003/02/06 20:27:54  crc_canada
# remove unused endlist macro
#
# Revision 1.68  2003/01/31 19:33:18  crc_canada
# More SoundEngine work
#
# Revision 1.67  2003/01/07 18:59:49  crc_canada
# more sound engine work
#
# Revision 1.66  2002/10/16 15:35:49  crc_canada
# More sound work...
#
# Revision 1.65  2002/08/14 17:43:34  ncoder
# Stepping code
#
# Revision 1.64  2002/08/08 17:03:05  ncoder
# Added Text collision
# Added ElevationGrid collision
# Improved multiple collision handling.
# Corrected polydisp that used to use a reversed normal.
#
# Revision 1.63  2002/08/02 15:08:53  ncoder
# Corrected proximitysensor changes.
# Added more fine-grained glError checking
#
# Revision 1.62  2002/07/31 20:56:53  ncoder
# Added:
#     Support for "collide" boolean field in collision nodes
#     Support for proxy nodes.
# 	Collide time envents launched
#
# Small improvement in ProximitySensors
#
# Revision 1.61  2002/07/30 14:03:35  crc_canada
# MovieTexture repeatS and repeatT flags passed correctly now
#
# Revision 1.60  2002/07/29 20:07:39  ncoder
# Removed lingering printmatrix debug code. (VRMLRend.pm)
#
# Other non significant code changes.
#
# Revision 1.59  2002/07/19 16:56:52  ncoder
# added collision detection for
# Boxes, cones, cylinders.
# Added a debug flag to print out collision information ("collision")
#
# a few modifications/additions to functions in LinearAlgebra
#
# Revision 1.58  2002/07/19 14:05:29  crc_canada
# seg fault caused in save_font_paths; changed way of verifying that
# files exist to fopen/fclosed from fstat. Appears to fix problem. Also
# changed it to return a success/fail flag; this allows freewrl to search
# a bunch of directories for fonts
#
# Revision 1.57  2002/07/15 15:37:48  crc_canada
# add CFunc to allow setting of max texture size from freewrl.PL
#
# Revision 1.56  2002/07/09 16:13:02  crc_canada
# Text nodes now in here; seperate module removed
#
# Revision 1.55  2002/07/03 17:04:09  crc_canada
# MovieTexture now has internal decoding
#
# Revision 1.54  2002/06/17 14:41:45  ncoder
# Added sphere collision detection (more to come)
# -This included adding rendering passes,
# -Changed the render_node parameter passing method (now using flags).
# -Other details I forgot.
#
# Added C file LinearAlgebra.h/.c
# -Contains matrix/vector calculations.
# -Some were moved from headers.h, others from random places. a few new ones.
#
# Bugs pending:
# -Temprorary unefficient state : to much redraws of the viewpoint and perspective.
# -Proximity sensors out of sync when collision active.
#
# Revision 1.53  2002/06/17 12:27:41  crc_canada
# Material node properties
#
# Revision 1.52  2002/06/05 18:57:16  crc_canada
# Function Prototypes moved to CFuncs/headers.h
# 
# Revision 1.51  2002/05/30 21:18:04  ncoder
# Fixed bug with the order of the multiplication of the transformations while rendering the viewpoint.
# Increased performance of viewpoint rendering code.
#
# Added some comments.
#
# Revision 1.50  2002/05/23 18:26:48  crc_canada
# move some "externs" out to CFuncs/headers.h
#
# Revision 1.49  2002/05/13 14:59:51  crc_canada
# Move some functions out to the CFuncs subdirectory
#
# Revision 1.48  2002/05/01 15:11:30  crc_canada
# Split out ElevationGrid and IndexedFaceSet, to help make it all more readable.
#
# Revision 1.47  2002/04/17 19:20:34  crc_canada
# ElevationGrid debug print statements left in by accident...
#
# Revision 1.46  2002/04/03 15:45:10  crc_canada
# Smooth normals for Indexed Face Sets... initial try
#
# Revision 1.45  2002/02/05 20:31:20  crc_canada
# Cleaned up way "smooth normals" were tagged in the code.
#
# Revision 1.44  2002/01/22 16:34:50  crc_canada
# When a display list becomes invalid (eg, node has changed), propagate
# the changed_flag to all parents.
#
# Revision 1.43  2001/12/14 17:54:28  crc_canada
# render_polyrep changed - IFS and ElevationGrids should have colour field
# superceded by textures, if there are any.
#
# Revision 1.42  2001/08/17 20:11:05  ayla
#
# Begin initial trunk-NetscapeIntegration merge.
#
# Revision 1.41  2001/08/16 16:55:19  crc_canada
# Viewpoint work.
#
# Revision 1.40  2001/08/03 16:44:03  kitfox
# Fixed colors with nancy face and elevation grids.
#
# Revision 1.39  2001/07/30 20:06:48  kitfox
# Added support for user-specified normals on elevation grids
#
# Revision 1.38  2001/07/24 13:22:06  crc_canada
# 1) reduce memory usage for textures.
# 2) more parameter checking on indexedfacesets
#
# Revision 1.37  2001/07/18 14:09:29  crc_canada
# IFS tecCoordIndex with selected coordinates from within a large Coordinate node
# verified to work.
#
# Revision 1.36  2001/07/17 19:09:17  crc_canada
# render_polyrep to generate default tex coords with IFS a little bit better.
#
# Revision 1.35  2001/07/16 18:55:18  crc_canada
# Scale textures to fit texture limits of OpenGL implementation. (do_texture changed)
#
# Revision 1.34  2001/07/12 15:50:23  kitfox
# Merging extrusion_normals branch to the main branch
#
# Revision 1.33  2001/07/11 20:43:04  ayla
#
#
# Fixed problem with Plugin/Source/npfreewrl.c, so all debugging info. is turned
# off.
#
# Committing merge between NetscapeIntegration and the trunk.
#
# Revision 1.32  2001/07/05 16:04:00  crc_canada
# Initial IFS default texture rewrite.
#
# Revision 1.31  2001/06/25 18:35:01  crc_canada
# ElevationGrid textures now ok.
#
# Revision 1.30  2001/06/18 17:24:50  crc_canada
# IRIX compile warnings removed
#
# Revision 1.29  2001/06/15 19:43:29  crc_canada
# Alains Smooth Shading for Extrusions added.
#
# Revision 1.28.2.4  2001/07/10 14:05:01  kitfox
# Code Cleanup after fixing the last bug
#
# Revision 1.28.2.3  2001/07/10 13:22:13  kitfox
# Fixed a bug with smooth shading for elevation grids.  The bug had to do
# with some indexing mix-ups.
#
# Revision 1.28.2.2  2001/07/04 16:49:24  uid53492
#
# Applied smooth normals to ElevationGrids using the extrusion code.
#
# Revision 1.28.2.1  2001/06/27 20:48:03  kitfox
#
# Added a significant chunk of code that calculates normals that allow
# smooth shading on extrusions.
#
# Revision 1.28  2001/05/16 16:00:47  crc_canada
# check for degenerate triangles in render_ray_polyrep, and skip it if one is found.
#
# Revision 1.27  2001/04/27 16:49:43  crc_canada
# Working display lists for Shape nodes
#
# Revision 1.26  2001/04/24 19:55:00  crc_canada
# Display list work
#
# Revision 1.25  2001/03/26 17:40:36  crc_canada
# Background fixes for some implementations of OpenGL.
# IndexedLineSet no longer disrupts other renderings.
#
# Revision 1.24  2001/03/23 16:01:45  crc_canada
# Unknown
#
# Revision 1.23  2001/03/13 16:18:59  crc_canada
# Pre 0.28 checkin
#
# Revision 1.22  2001/02/16 18:31:52  crc_canada
# Cleaned up pixeltextures in do_texture routine
#
# Revision 1.21  2000/12/20 17:28:14  crc_canada
# more IndexedFaceSet work - normals this time
#
# Revision 1.20  2000/12/18 21:16:56  crc_canada
# IndexedFaceSet colorIndex and colorPerVertex now working correctly.
#
# Revision 1.19  2000/12/13 14:40:21  crc_canada
# Bug with texcoords field and Extrusions. (not being zeroed)
#
# Revision 1.18  2000/12/07 19:11:18  crc_canada
# IndexedFaceSet texture mapping
#
# Revision 1.17  2000/11/16 18:45:05  crc_canada
# Bug in do_textures; was not setting GL_CLAMP correctly, boolean test added
#
# Revision 1.16  2000/11/15 18:25:28  crc_canada
# Changed texture quality to give us good quality when gluScaleTexture is called.
#
# Revision 1.15  2000/11/15 15:26:35  crc_canada
# Removed a printf "Render tex coord".
#
# Revision 1.14  2000/11/07 14:51:36  crc_canada
# Better glGenTexture initialization
#
# Revision 1.13  2000/11/03 21:51:36  crc_canada
# texture pbjects for increased speed
#
# Revision 1.12  2000/10/28 17:40:57  crc_canada
# EAI addchildren, etc, should be ok now.
#
# Revision 1.11  2000/09/15 22:48:40  rcoscali
# Add patch from bob kozdemba for concave polygon fix
#
# Revision 1.10  2000/09/03 20:17:50  rcoscali
# Made some test for blending
# Tests are displayed with 38 & 39.wrl�
#
# Revision 1.9  2000/09/02 23:54:39  rcoscali
# Fixed the core dump for 27.wrl and 28.wrl
#
# Revision 1.8  2000/08/31 22:49:23  rcoscali
# Add depth 2 support (2 channels/color components) which isMINANCE_ALPHAre (wi
#
# Revision 1.7  2000/08/31 07:59:52  rcoscali
# Terminate fix of image loading
# Add flip_image routine in JPEG.xs
# Handle it in VRMLNodes & take care of depth in VRMLC.pm
#
# Revision 1.6  2000/08/30 00:04:19  rcoscali
# Comment out a glDisable(GL_LIGHTING) (uncommented by mistake ??)
# Fixed a problem in cone rendering (order of the vertexes)i
#
# Revision 1.5  2000/08/13 14:27:55  rcoscali
# Fixed a trace
#
# Revision 1.4  2000/08/08 21:15:36  rcoscali
# Fixed Image Texture rendering problem (depth parameter on gluScaleImage)
#
# Revision 1.3  2000/08/07 02:41:01  rcoscali
# Removed the comment of John (we discuss it and I think not usefull anymore in pixel tex code). Was image relevant.
#
# Revision 1.2  2000/08/06 19:48:37  rcoscali
# Fixed Cylinder. Now attacking cone.
#
# Revision 1.1.1.1  2000/08/04 07:07:38  rcoscali
# initial import (creation) of sources repository
#
#

# To allow faster internal representations of nodes to be calculated,
# there is the field '_change' which can be compared between the node
# and the internal rep - if different, needs to be regenerated.
#
# the rep needs to be allocated if _intern == 0.
# XXX Freeing?!!?

require 'VRMLFields.pm';
require 'VRMLNodes.pm';
require 'VRMLRend.pm';

#######################################################################
#######################################################################
#######################################################################
#
# Constants
#  VRMLFunc constants, shared amongs the Perl and C code.
# 

%Constants = (
	      VF_Viewpoint     => 0x0001,
	      VF_Geom          => 0x0002,
	      VF_Lights        => 0x0004,
	      VF_Sensitive     => 0x0008,
	      VF_Blend         => 0x0010,
	      VF_Proximity     => 0x0020,
	      VF_Collision     => 0x0040,
	      
);



#######################################################################
#######################################################################
#######################################################################
#
# RendRay --
#  code for checking whether a ray (defined by mouse pointer)
#  intersects with the geometry of the primitive.
#
#

# Y axis rotation around an unit vector:
# alpha = angle between Y and vec, theta = rotation angle
#  1. in X plane ->
#   Y = Y - sin(alpha) * (1-cos(theta))
#   X = sin(alpha) * sin(theta)
#
#  
# How to find out the orientation from two vectors (we are allowed
# to assume no negative scales)
#  1. Y -> Y' -> around any vector on the plane midway between the 
#                two vectors
#     Z -> Z' -> around any vector ""
#
# -> intersection.
#
# The plane is the midway normal between the two vectors
# (if the two vectors are the same, it is the vector).

%RendRayC = (
Box => '
	float x = $f(size,0)/2;
	float y = $f(size,1)/2;
	float z = $f(size,2)/2;
	/* 1. x=const-plane faces? */
	if(!XEQ) {
		float xrat0 = XRAT(x);
		float xrat1 = XRAT(-x);
		if(verbose) printf("!XEQ: %f %f\\n",xrat0,xrat1);
		if(TRAT(xrat0)) {
			float cy = MRATY(xrat0);
			if(verbose) printf("TRok: %f\\n",cy);
			if(cy >= -y && cy < y) {
				float cz = MRATZ(xrat0);
				if(verbose) printf("cyok: %f\\n",cz);
				if(cz >= -z && cz < z) {
					if(verbose) printf("czok:\\n");
					rayhit(xrat0, x,cy,cz, 1,0,0, -1,-1, "cube x0");
				}
			}
		}
		if(TRAT(xrat1)) {
			float cy = MRATY(xrat1);
			if(cy >= -y && cy < y) {
				float cz = MRATZ(xrat1);
				if(cz >= -z && cz < z) {
					rayhit(xrat1, -x,cy,cz, -1,0,0, -1,-1, "cube x1");
				}
			}
		}
	}
	if(!YEQ) {
		float yrat0 = YRAT(y);
		float yrat1 = YRAT(-y);
		if(TRAT(yrat0)) {
			float cx = MRATX(yrat0);
			if(cx >= -x && cx < x) {
				float cz = MRATZ(yrat0);
				if(cz >= -z && cz < z) {
					rayhit(yrat0, cx,y,cz, 0,1,0, -1,-1, "cube y0");
				}
			}
		}
		if(TRAT(yrat1)) {
			float cx = MRATX(yrat1);
			if(cx >= -x && cx < x) {
				float cz = MRATZ(yrat1);
				if(cz >= -z && cz < z) {
					rayhit(yrat1, cx,-y,cz, 0,-1,0, -1,-1, "cube y1");
				}
			}
		}
	}
	if(!ZEQ) {
		float zrat0 = ZRAT(z);
		float zrat1 = ZRAT(-z);
		if(TRAT(zrat0)) {
			float cx = MRATX(zrat0);
			if(cx >= -x && cx < x) {
				float cy = MRATY(zrat0);
				if(cy >= -y && cy < y) {
					rayhit(zrat0, cx,cy,z, 0,0,1, -1,-1,"cube z0");
				}
			}
		}
		if(TRAT(zrat1)) {
			float cx = MRATX(zrat1);
			if(cx >= -x && cx < x) {
				float cy = MRATY(zrat1);
				if(cy >= -y && cy < y) {
					rayhit(zrat1, cx,cy,-z, 0,0,-1,  -1,-1,"cube z1");
				}
			}
		}
	}
',

# Distance to zero as function of ratio is
# sqrt(
#	((1-r)t_r1.x + r t_r2.x)**2 +
#	((1-r)t_r1.y + r t_r2.y)**2 +
#	((1-r)t_r1.z + r t_r2.z)**2
# ) == radius
# Therefore,
# radius ** 2 == ... ** 2 
# and 
# radius ** 2 = 
# 	(1-r)**2 * (t_r1.x**2 + t_r1.y**2 + t_r1.z**2) +
#       2*(r*(1-r)) * (t_r1.x*t_r2.x + t_r1.y*t_r2.y + t_r1.z*t_r2.z) +
#       r**2 (t_r2.x**2 ...)
# Let's name tr1sq, tr2sq, tr1tr2 and then we have
# radius ** 2 =  (1-r)**2 * tr1sq + 2 * r * (1-r) tr1tr2 + r**2 tr2sq
# = (tr1sq - 2*tr1tr2 + tr2sq) r**2 + 2 * r * (tr1tr2 - tr1sq) + tr1sq
# 
# I.e.
# 
# (tr1sq - 2*tr1tr2 + tr2sq) r**2 + 2 * r * (tr1tr2 - tr1sq) + 
#	(tr1sq - radius**2) == 0
#
# I.e. second degree eq. a r**2 + b r + c == 0 where
#  a = tr1sq - 2*tr1tr2 + tr2sq
#  b = 2*(tr1tr2 - tr1sq)
#  c = (tr1sq-radius**2)
# 
# 
Sphere => '
	float r = $f(radius);
	/* Center is at zero. t_r1 to t_r2 and t_r1 to zero are the vecs */
	float tr1sq = VECSQ(t_r1);
	float tr2sq = VECSQ(t_r2);
	float tr1tr2 = VECPT(t_r1,t_r2);
	struct pt dr2r1;
	float dlen;
	float a,b,c,disc;

	VECDIFF(t_r2,t_r1,dr2r1);
	dlen = VECSQ(dr2r1);

	a = dlen; /* tr1sq - 2*tr1tr2 + tr2sq; */
	b = 2*(VECPT(dr2r1, t_r1));
	c = tr1sq - r*r;

	disc = b*b - 4*a*c; /* The discriminant */
	
	if(disc > 0) { /* HITS */
		float q ;
		float sol1 ;
		float sol2 ;
		float cx,cy,cz;
		q = sqrt(disc);
		/* q = (-b+(b>0)?q:-q)/2; */
		sol1 = (-b+q)/(2*a);
		sol2 = (-b-q)/(2*a);
		/*
		printf("SPHSOL0: (%f %f %f) (%f %f %f)\\n",
			t_r1.x, t_r1.y, t_r1.z, t_r2.x, t_r2.y, t_r2.z);
		printf("SPHSOL: (%f %f %f) (%f) (%f %f) (%f) (%f %f)\\n",
			tr1sq, tr2sq, tr1tr2, a, b, c, und, sol1, sol2);
		*/
		cx = MRATX(sol1);
		cy = MRATY(sol1);
		cz = MRATZ(sol1);
		rayhit(sol1, cx,cy,cz, cx/r,cy/r,cz/r, -1,-1, "sphere 0");
		cx = MRATX(sol2);
		cy = MRATY(sol2);
		cz = MRATZ(sol2);
		rayhit(sol2, cx,cy,cz, cx/r,cy/r,cz/r, -1,-1, "sphere 1");
	}
',

# Cylinder: first test the caps, then against infinite cylinder.
Cylinder => '
	float h = $f(height)/2; /* pos and neg dir. */
	float r = $f(radius);
	float y = h;
	/* Caps */
	if(!YEQ) {
		float yrat0 = YRAT(y);
		float yrat1 = YRAT(-y);
		if(TRAT(yrat0)) {
			float cx = MRATX(yrat0);
			float cz = MRATZ(yrat0);
			if(r*r > cx*cx+cz*cz) {
				rayhit(yrat0, cx,y,cz, 0,1,0, -1,-1, "cylcap 0");
			}
		}
		if(TRAT(yrat1)) {
			float cx = MRATX(yrat1);
			float cz = MRATZ(yrat1);
			if(r*r > cx*cx+cz*cz) {
				rayhit(yrat1, cx,-y,cz, 0,-1,0, -1,-1, "cylcap 1");
			}
		}
	}
	/* Body -- do same as for sphere, except no y axis in distance */
	if((!XEQ) && (!ZEQ)) {
		float dx = t_r2.x-t_r1.x; float dz = t_r2.z-t_r1.z;
		float a = dx*dx + dz*dz;
		float b = 2*(dx * t_r1.x + dz * t_r1.z);
		float c = t_r1.x * t_r1.x + t_r1.z * t_r1.z - r*r;
		float und;
		b /= a; c /= a;
		und = b*b - 4*c;
		if(und > 0) { /* HITS the infinite cylinder */
			float sol1 = (-b+sqrt(und))/2;
			float sol2 = (-b-sqrt(und))/2;
			float cy,cx,cz;
			cy = MRATY(sol1);
			if(cy > -h && cy < h) {
				cx = MRATX(sol1);
				cz = MRATZ(sol1);
				rayhit(sol1, cx,cy,cz, cx/r,0,cz/r, -1,-1, "cylside 1");
			}
			cy = MRATY(sol2);
			if(cy > -h && cy < h) {
				cx = MRATX(sol2);
				cz = MRATZ(sol2);
				rayhit(sol2, cx,cy,cz, cx/r,0,cz/r, -1,-1, "cylside 2");
			}
		}
	}
',

# For cone, this is most difficult. We have
# sqrt(
#	((1-r)t_r1.x + r t_r2.x)**2 +
#	((1-r)t_r1.z + r t_r2.z)**2
# ) == radius*( -( (1-r)t_r1.y + r t_r2.y )/(2*h)+0.5)
# == radius * ( -( r*(t_r2.y - t_r1.y) + t_r1.y )/(2*h)+0.5)
# == radius * ( -r*(t_r2.y-t_r1.y)/(2*h) + 0.5 - t_r1.y/(2*h))

#
# Other side: r*r*(
Cone => '
	float h = $f(height)/2; /* pos and neg dir. */
	float y = h;
	float r = $f(bottomRadius);
	float dx = t_r2.x-t_r1.x; float dz = t_r2.z-t_r1.z;
	float dy = t_r2.y-t_r1.y;
	float a = dx*dx + dz*dz - (r*r*dy*dy/(2*h*2*h));
	float b = 2*(dx*t_r1.x + dz*t_r1.z) +
		2*r*r*dy/(2*h)*(0.5-t_r1.y/(2*h));
	float tmp = (0.5-t_r1.y/(2*h));
	float c = t_r1.x * t_r1.x + t_r1.z * t_r1.z 
		- r*r*tmp*tmp;
	float und;
	b /= a; c /= a;
	und = b*b - 4*c;
	/* 
	printf("CONSOL0: (%f %f %f) (%f %f %f)\\n",
		t_r1.x, t_r1.y, t_r1.z, t_r2.x, t_r2.y, t_r2.z);
	printf("CONSOL: (%f %f %f) (%f) (%f %f) (%f)\\n",
		dx, dy, dz, a, b, c, und);
	*/
	if(und > 0) { /* HITS the infinite cylinder */
		float sol1 = (-b+sqrt(und))/2;
		float sol2 = (-b-sqrt(und))/2;
		float cy,cx,cz;
		float cy0;
		cy = MRATY(sol1);
		if(cy > -h && cy < h) {
			cx = MRATX(sol1);
			cz = MRATZ(sol1);
			/* XXX Normal */
			rayhit(sol1, cx,cy,cz, cx/r,0,cz/r, -1,-1, "conside 1");
		}
		cy0 = cy;
		cy = MRATY(sol2);
		if(cy > -h && cy < h) {
			cx = MRATX(sol2);
			cz = MRATZ(sol2);
			rayhit(sol2, cx,cy,cz, cx/r,0,cz/r, -1,-1, "conside 2");
		}
		/*
		printf("CONSOLV: (%f %f) (%f %f)\\n", sol1, sol2,cy0,cy);
		*/
	}
	if(!YEQ) {
		float yrat0 = YRAT(-y);
		if(TRAT(yrat0)) {
			float cx = MRATX(yrat0);
			float cz = MRATZ(yrat0);
			if(r*r > cx*cx + cz*cz) {
				rayhit(yrat0, cx, -y, cz, 0, -1, 0, -1, -1, "conbot");
			}
		}
	}
',

ElevationGrid => ( '
		$mk_polyrep();
		render_ray_polyrep(this_, 
			0, NULL
		);
'),

Text => ( '
		$mk_polyrep();
		render_ray_polyrep(this_, 
			0, NULL
		);
'),

Extrusion => ( '
		$mk_polyrep();
		render_ray_polyrep(this_, 
			0, NULL
		);
'),

IndexedFaceSet => '
		struct SFColor *points; int npoints;
		$fv(coord, points, get3, &npoints);
		$mk_polyrep();
		render_ray_polyrep(this_, 
			npoints, points
		);
',

);

#####################################################################3
#####################################################################3
#####################################################################3
#
# GenPolyRep
#  code for generating internal polygonal representations
#  of some nodes (ElevationGrid, Text, Extrusion and IndexedFaceSet)
#
#

%GenPolyRepC = (
	ElevationGrid => (do "VRMLElevationGrid.pm"),
	Extrusion => (do "VRMLExtrusion.pm"),
	IndexedFaceSet => (do "VRMLIndexedFaceSet.pm"),
	Text => (do "VRMLText.pm"),
);

######################################################################
######################################################################
######################################################################
#
# Get3
#  get a coordinate / color / normal array from the node.
#

%Get3C = (
Coordinate => '
	/* ptr for invalidating the shape display list */
	this_->_myshape = last_visited_shape; 
	*n = $f_n(point); 
	return $f(point);
',
Color => '
	/* ptr for invalidating the shape display list */
	this_->_myshape = last_visited_shape;
	*n = $f_n(color); 
	return $f(color);
',
Normal => '
	/* ptr for invalidating the shape display list */
	this_->_myshape = last_visited_shape; 
	*n = $f_n(vector);
	return $f(vector);
'
);

%Get2C = (
TextureCoordinate => '
	/* ptr for invalidating the shape display list */
	this_->_myshape = last_visited_shape; 
	*n = $f_n(point);
	return $f(point);
',
);

######################################################################
######################################################################
######################################################################
#
# Generation
#  Functions for generating the code
#


{
	my %AllNodes = (%RendC, %RendRayC, %PrepC, %FinC, %ChildC, %Get3C, %Get2C, %LightC,
		%ChangedC, %ProximityC, %CollisionC);
	@NodeTypes = keys %AllNodes;
}

sub assgn_m {
	my($f, $l) = @_;
	return ((join '',map {"m[$_] = ".getf(Material, $f, $_).";"} 0..2).
		"m[3] = $l;");
}

# XXX Might we need separate fields for separate structs?
sub getf {
	my ($t, $f, @l) = @_;
	my $type = $VRML::Nodes{$t}{FieldTypes}{$f};
	if($type eq "") {
		die("Invalid type $t $f '$type'");
	}
	return "VRML::Field::$type"->cget("(this_->$f)",@l);
}

sub getfn {
	my($t, $f) = @_;
	my $type = $VRML::Nodes{$t}{FieldTypes}{$f};
	return "VRML::Field::$type"->cgetn("(this_->$f)");
}

# XXX Code copy :(
sub fvirt {
	my($t, $f, $ret, $v, @a) = @_;
	# Die if not exists
	my $type = $VRML::Nodes{$t}{FieldTypes}{$f};
	if($type ne "SFNode") {
		die("Fvirt must have SFNode");
	}
	if($ret) {$ret = "$ret = ";}
	return "if(this_->$f) {
		  if(!(*(struct VRML_Virt **)(this_->$f))->$v) {
		  	die(\"NULL METHOD $t $f $v\");
		  }
		  $ret ((*(struct VRML_Virt **)(this_->$f))->$v(this_->$f,
		    ".(join ',',@a).")) ;}
 	  else { (die(\"NULL FIELD $t $f $a\"));}";
}

sub fvirt_null {
	my($t, $f, $ret, $v, @a) = @_;
	# Die if not exists
	my $type = $VRML::Nodes{$t}{FieldTypes}{$f};
	if($type ne "SFNode") {
		die("Fvirt must have SFNode");
	}
	if($ret) {$ret = "$ret = ";}
	return "if(this_->$f) {
		  if(!(*(struct VRML_Virt **)(this_->$f))->$v) {
		  	die(\"NULL METHOD $t $f $v\");
		  }
		  $ret ((*(struct VRML_Virt **)(this_->$f))->$v(this_->$f,
		    ".(join ',',@a).")) ;
		}";
}


sub fgetfnvirt_n {
	my($n, $ret, $v, @a) = @_;
	if($ret) {$ret = "$ret = ";}
	return "if($n) {
	         if(!(*(struct VRML_Virt **)n)->$v) {
		  	die(\"NULL METHOD $n $ret $v\");
		 }
		 $ret ((*(struct VRML_Virt **)($n))->$v($n,
		    ".(join ',',@a).")) ;}
	"
}

sub rend_geom {
	return $_[0];
}

################################################################
#
# gen_struct - Generate a node structure, adding fields for
# internal use

sub gen_struct {
	my($name,$node) = @_;
	my @f = keys %{$node->{FieldTypes}};
	my $nf = scalar @f;
	# /* Store actual point etc. later */
       my $s = "struct VRML_$name {\n" .
               " /***/ struct VRML_Virt *v;\n"         	.
               " /*s*/ int _sens; \n"                  	.
               " /*t*/ int _hit; \n"                   	.
               " /*a*/ int _change; \n"                	.
               " /*n*/ int _dlchange; \n"              	.
               " /*d*/ GLuint _dlist; \n"              	.
	       "       void **_parents; \n"	  	.
	       "       int _nparents; \n"		.
	       "       int _nparalloc; \n"		.
	       "       int _ichange; \n"		.
	       " /*disp list JAS*/ struct VRML_Shape *_myshape; \n"		.
               " /*d*/ void *_intern; \n"              	.
               " /***/\n";
	
	my $o = "
void *
get_${name}_offsets(p)
	SV *p;
CODE:
	int *ptr_;
	SvGROW(p,($nf+1)*sizeof(int));
	SvCUR_set(p,($nf+1)*sizeof(int));
	ptr_ = (int *)SvPV(p,PL_na);
";
	my $p = " {
		my \$s = '';
		my \$v = get_${name}_offsets(\$s);
		\@{\$n->{$name}{Offs}}{".(join ',',map {"\"$_\""} @f,'_end_')."} =
			unpack(\"i*\",\$s);
		\$n->{$name}{Virt} = \$v;
 }
	";
	for(@f) {
		my $cty = "VRML::Field::$node->{FieldTypes}{$_}"->ctype($_);
		$s .= "\t$cty;\n";
		$o .= "\t*ptr_++ = offsetof(struct VRML_$name, $_);\n";
	}
	$o .= "\t*ptr_++ = sizeof(struct VRML_$name);\n";
	$o .= "RETVAL=&(virt_${name});
	if(verbose) printf(\"$name virtual: %d \\n \", RETVAL);
OUTPUT:
	RETVAL
";
	$s .= $ExtraMem{$name};
	$s .= "};\n";
	return ($s,$o,$p);
}

#########################################################
sub get_offsf {
	my($f) = @_;
	my ($ct) = ("VRML::Field::$_")->ctype("*ptr_");
	my ($ctp) = ("VRML::Field::$_")->ctype("*");
	my ($c) = ("VRML::Field::$_")->cfunc("(*ptr_)", "sv_");
	my ($ca) = ("VRML::Field::$_")->calloc("(*ptr_)");
	my ($cf) = ("VRML::Field::$_")->cfree("(*ptr_)");
	return "

void 
set_offs_$f(ptr,offs,sv_)
	void *ptr
	int offs
	SV *sv_
CODE:
	$ct = ($ctp)(((char *)ptr)+offs);
	update_node(ptr);
	$c


void 
alloc_offs_$f(ptr,offs)
	void *ptr
	int offs
CODE:
	$ct = ($ctp)(((char *)ptr)+offs);
	$ca

void
free_offs_$f(ptr,offs)
	void *ptr
	int offs
CODE:
	$ct = ($ctp)(((char *)ptr)+offs);
	$cf

"
}
#######################################################

sub get_rendfunc {
	my($n) = @_;
	print "RENDF $n ";
	# XXX
	my @f = qw/Prep Rend Child Fin RendRay GenPolyRep Light Get3 Get2
		Changed Proximity Collision/;
	my $f;
	my $v = "
static struct VRML_Virt virt_${n} = { ".
	(join ',',map {${$_."C"}{$n} ? "${n}_$_" : "NULL"} @f).
",\"$n\"};";
	for(@f) {
		my $c =${$_."C"}{$n};
		next if !defined $c;
		print "$_ (",length($c),") ";
		# Substitute field gets

		$c =~ s/\$f\(([^)]*)\)/getf($n,split ',',$1)/ge;
		$c =~ s/\$i\(([^)]*)\)/(this_->$1)/g;
		$c =~ s/\$f_n\(([^)]*)\)/getfn($n,split ',',$1)/ge;
		$c =~ s/\$fv\(([^)]*)\)/fvirt($n,split ',',$1)/ge;
		$c =~ s/\$fv_null\(([^)]*)\)/fvirt_null($n,split ',',$1)/ge;
		$c =~ s/\$mk_polyrep\(\)/if(!this_->_intern || 
			this_->_change != ((struct VRML_PolyRep *)this_->_intern)->_change)
				regen_polyrep(this_);/g;
		if($_ eq "Get3") {
			$f .= "\n\nstruct SFColor *${n}_$_(void *nod_,int *n)";
		} elsif($_ eq "Get2") {
			$f .= "\n\nstruct SFVec2f *${n}_$_(void *nod_,int *n)";
		} else {
			$f .= "\n\nvoid ${n}_$_(void *nod_)";
		}
		$f .= "{ /* GENERATED FROM HASH ${_}C, MEMBER $n */
			struct VRML_$n *this_ = (struct VRML_$n *)nod_;
			{$c}
			}";
	}
	print "\n";
	return ($f,$v);
}


######################################################################
######################################################################
######################################################################
#
# gen_constants_xxx - generate constants and values in perl and C.
#
#

sub gen_constants_pm_header {

    my @k = keys %Constants;

    return "\@EXPORT = qw( @k );\n";
    
}

sub gen_constants_pm_body {
    my $code = "";

    $code .= "\n# VRMLFunc constants. Generated by gen_constants_pm_body in VRMLC.pm \n";
    while(($var,$value) = each(%Constants)) {
	$code .= "sub $var () { $value } \n";
    }
    $code .= "# End of autogenerated constants. \n\n";

    return $code;
}

sub gen_constants_c {

    my $code = "";
    $code .= "\n/* VRMLFunc constants. Generated by gen_constants_c in VRMLC.pm */\n";
    while(($var,$value) = each(%Constants)) {
	$code .= "#define $var $value\n";
    }
    $code .= "/* End of autogenerated constants. */\n\n";

    return $code;
}

######################################################################
######################################################################
######################################################################
#
# gen - the main function. this contains much verbatim code
#
#

sub gen {
	for(@VRML::Fields) {
		push @str, ("VRML::Field::$_")->cstruct . "\n";
		push @xsfn, get_offsf($_);
	}
        push @str, "\n/* and now the structs for the nodetypes */ \n";
	for(@NodeTypes) {
		my $no = $VRML::Nodes{$_}; 
		my($str, $offs, $perl) = gen_struct($_, $no);
		push @str, $str;
		push @xsfn, $offs;
		push @poffsfn, $perl;
		my($f, $vstru) = get_rendfunc($_);
		push @func, $f;
		push @vstruc, $vstru;
	}

	# print out structures to a file
	open STRUCTS, ">CFuncs/Structs.h";
	print STRUCTS '
/* Structs.h generated by VRMLC.pm. DO NOT MODIFY, MODIFY VRMLC.pm INSTEAD */

/* Code here comes almost verbatim from VRMLC.pm */

#ifndef STRUCTSH
#define STRUCTSH

#include "EXTERN.h"
#include "perl.h"
#ifdef AQUA
#include <gl.h>
#else
#include "GL/gl.h"
#endif

/* for time tick calculations */
#include <sys/time.h>
#include <sys/poll.h>

struct pt {GLdouble x,y,z;};
struct orient {GLdouble x,y,z,a;};

struct VRML_Virt {
	void (*prep)(void *);
	void (*rend)(void *); 
	void (*children)(void *);
	void (*fin)(void *);
	void (*rendray)(void *);
	void (*mkpolyrep)(void *);
	void (*light)(void *);
	/* And get float coordinates : Coordinate, Color */
	/* XXX Relies on MFColor repr.. */
	struct SFColor *(*get3)(void *, int *); /* Number in int */
	struct SFVec2f *(*get2)(void *, int *); /* Number in int */
	void (*changed)(void *);
	void (*proximity)(void *);
	void (*collision)(void *);
	char *name;
};

/* Internal representation of IndexedFaceSet, Text, Extrusion & ElevationGrid:
 * set of triangles.
 * done so that we get rid of concave polygons etc.
 */
struct VRML_PolyRep { /* Currently a bit wasteful, because copying */
	int _change;
	int ccw;	/* ccw field for single faced structures */
	int ntri; /* number of triangles */
	int alloc_tri; /* number of allocated triangles */
	int *cindex;   /* triples (per triangle) */
	float *coord; /* triples (per point) */
	int *colindex;   /* triples (per triangle) */
	float *color; /* triples or null */
	int *norindex;
	float *normal; /* triples or null */
        int *tcindex; /* triples or null */
        float *tcoord;	/* triples (per triangle) of texture coords */
};

/* viewer dimentions (for collision detection) */
struct sNaviInfo {
        double width;
        double height;
        double step;
};

';



	# print out the generated structures
	print STRUCTS join '',@str;

	print STRUCTS '
#endif /* ifndef */
';
	

	open CST, ">CFuncs/constants.h";
	print CST  &gen_constants_c();
	close CST;


	open XS, ">VRMLFunc.xs";
	print XS '
/* VRMLFunc.c generated by VRMLC.pm. DO NOT MODIFY, MODIFY VRMLC.pm INSTEAD */

/* Code here comes almost verbatim from VRMLC.pm */

#include "CFuncs/headers.h"
#include "XSUB.h"

#include <math.h>

#ifndef AQUA
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#else
#include <gl.h>
#include <glu.h>
#include <glext.h>
#endif

#include "OpenGL/OpenGL.m"
#include "CFuncs/Viewer.h"
#include "CFuncs/OpenGL_Utils.h"
#include "CFuncs/Collision.h"
#include "CFuncs/Bindable.h"
#include "CFuncs/Textures.h"
#include "CFuncs/Polyrep.h"
#include "CFuncs/sounds.h"
#include "CFuncs/SensInterps.h"


D_OPENGL;

/* Rearrange to take advantage of headlight when off */
int curlight = 0;
int nlightcodes = 7;
int lightcode[7] = {
	GL_LIGHT1,
	GL_LIGHT2,
	GL_LIGHT3,
	GL_LIGHT4,
	GL_LIGHT5,
	GL_LIGHT6,
	GL_LIGHT7,
};
int nextlight() {
	if(curlight == nlightcodes) { return -1; }
	return lightcode[curlight++];
}


/* for display list regeneration, we need to keep track of this*/
struct VRML_Shape *last_visited_shape = 0;

/* material node usage depends on texture depth; if rgb (depth1) we blend color field
   and diffusecolor with texture, else, we dont bother with material colors */
int last_texture_depth = 0;

/* Sounds can come from AudioClip nodes, or from MovieTexture nodes. Different
   structures on these */
int sound_from_audioclip = 0;

/* and, we allow a maximum of so many pixels per texture */
GLint global_texSize = 64;

/* for printing warnings about Sound node problems - only print once per invocation */
int soundWarned = FALSE;

int verbose;
int verbose_collision; /*print out collision info*/

int render_vp; /*set up the inverse viewmatrix of the viewpoint.*/
int render_geom;
int render_light;
int render_sensitive;
int render_blend;
int render_proximity;
int render_collision;


int found_vp; /*true when viewpoint found*/

GLuint last_bound_texture;

int smooth_normals = -1; /* -1 means, uninitialized */

int cur_hits=0;

/* Collision detection results */
struct sCollisionInfo CollisionInfo = { {0,0,0} , 0, 0. };

/* Displacement of viewer , used for colision calculation  PROTYPE, CURRENTLY UNUSED*/
struct pt ViewerDelta = {0,0,0};

/* dimentions of viewer, and "up" vector (for collision detection) */
struct sNaviInfo naviinfo = {0.25, 1.6, 0.75};

/* for alignment of collision cylinder, and gravity (later). */
struct pt ViewerUpvector = {0,0,0};

VRML_Viewer Viewer;


/* These two points define a ray in window coordinates */

struct pt r1 = {0,0,-1},r2 = {0,0,0},r3 = {0,1,0};
struct pt t_r1,t_r2,t_r3; /* transformed ray */
void *hypersensitive = 0; int hyperhit = 0;
struct pt hyper_r1,hyper_r2; /* Transformed ray for the hypersensitive node */

GLint viewport[4] = {-1,-1,2,2};

/* These three points define 1. hitpoint 2., 3. two different tangents
 * of the surface at hitpoint (to get transformation correctly */

/* All in window coordinates */

struct pt hp, ht1, ht2;
double hpdist; /* distance in ray: 0 = r1, 1 = r2, 2 = 2*r2-r1... */

/* Viewpoint Field of View */
GLdouble fieldofview = 45;


/* current time and other time related stuff */
double TickTime;
double lastTime;
double BrowserStartTime; 	/* start of calculating FPS 	*/
double BrowserFPS = 0.0;	/* calculated FPS		*/
struct pollfd waittimer[1];	/* used to rate-limit freewrl	*/

/* used to save rayhit and hyperhit for later use by C functions */
struct SFColor hyp_save_posn, hyp_save_norm, ray_save_posn;

/* Any action for the Browser (perl code) to do? */
int BrowserAction = FALSE;
char * BrowserActionString = 0;

struct currayhit {
void *node; /* What node hit at that distance? */
GLdouble modelMatrix[16]; /* What the matrices were at that node */
GLdouble projMatrix[16];
} rh,rph,rhhyper;
 /* used to test new hits */

/* this is used to return the duration of an audioclip to the perl
side of things. SvPV et al. works, but need to figure out all
references, etc. to bypass this fudge JAS */
float AC_LastDuration[50]  = {-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0,
				-1.0,-1.0,-1.0,-1.0,-1.0} ;

/* is the sound engine started yet? */
int SoundEngineStarted = FALSE;

/* stored FreeWRL version, pointers to initialize data */
char *BrowserVersion = NULL;
char *BrowserURL = NULL;
char *BrowserName = "FreeWRL VRML/X3D Browser";

/*************************JAVASCRIPT*********************************/
#ifndef __jsUtils_h__
#include "jsUtils.h" /* misc helper C functions and globals */
#endif

#ifndef __jsVRMLBrowser_h__
#include "jsVRMLBrowser.h" /* VRML browser script interface implementation */
#endif

#include "jsVRMLClasses.h" /* VRML field type implementation */


#define MAX_RUNTIME_BYTES 0x100000L
#define STACK_CHUNK_SIZE 0x2000L

/*
 * See perldoc perlapi, perlcall, perlembed, perlguts for how this all
 * works.
 */
void
doPerlCallMethod(SV *sv, const char *methodName)
{
	int count = 0;
	SV *retSV;

 #define PERL_NO_GET_CONTEXT

	dSP; /* local copy of stack pointer (dont leave home without it) */
	ENTER;
	SAVETMPS;
	PUSHMARK(SP); /* keep track of the stack pointer */
	XPUSHs(sv); /* push package ref to the stack */
	PUTBACK;
	count = call_method(methodName, G_SCALAR);

	SPAGAIN; /* refresh local copy of the stack pointer */
	
	if (count > 1) {
		fprintf(stderr,
				"doPerlCallMethod: call_method returned in list context - shouldnt happen here!\n");
	}

	PUTBACK;
	FREETMPS;
	LEAVE;
}

void
doPerlCallMethodVA(SV *sv, const char *methodName, const char *format, ...)
{
	va_list ap; /* will point to each unnamed argument in turn */
	char *c;
	void *v;
	int count = 0;
	size_t len = 0;
	const char *p = format;

	dSP;
	ENTER;
	SAVETMPS;
	PUSHMARK(SP);
	XPUSHs(sv);

	//printf ("doPerlCallMethodVA, method %s format %s\n",methodName,format);
	va_start(ap, format); /* point to first element after format*/
	while(*p) {
		switch (*p++) {
		case 0x73: /* ascii letter "s" -xs screws this up if in quotes */
			c = va_arg(ap, char *);
			len = strlen(c);
			c[len] = 0;
			XPUSHs(sv_2mortal(newSVpv(c, len)));
			break;
		case 0x70: /* ascii letter "p" -xs screws this up if in quotes */
			v = va_arg(ap, void *);
			XPUSHs(sv_2mortal(newSViv((IV) v)));
			break;
		default:
			fprintf(stderr, "doPerlCallMethodVA: argument type not supported!\n");
			break;
		}
	}
	va_end(ap);

	PUTBACK;
	count = call_method(methodName, G_SCALAR);

	SPAGAIN;
	

if (count > 1) {
		fprintf(stderr,
				"doPerlCallMethodVA: call_method returned in list context - shouldnt happen here!\n");
	}

	PUTBACK;
	FREETMPS;
	LEAVE;
}

/*************************JAVASCRIPT*********************************/

/* Sub, rather than big macro... */
void rayhit(float rat, float cx,float cy,float cz, float nx,float ny,float nz, 
float tx,float ty, char *descr)  {
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	GLdouble wx, wy, wz;
	/* Real rat-testing */
	if(verbose) 
		printf("RAY HIT %s! %f (%f %f %f) (%f %f %f)\nR: (%f %f %f) (%f %f %f)\n",
		descr, rat,cx,cy,cz,nx,ny,nz,
		t_r1.x, t_r1.y, t_r1.z,
		t_r2.x, t_r2.y, t_r2.z
		);
	if(rat<0 || (rat>hpdist && hpdist >= 0)) {
		return;
	}
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	gluProject(cx,cy,cz, modelMatrix, projMatrix, viewport,
		&hp.x, &hp.y, &hp.z);
	hpdist = rat;
	rh=rph;
	rhhyper=rph;
}

/* Call this when modelview and projection modified */
void upd_ray() {
	GLdouble modelMatrix[16];
	GLdouble projMatrix[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	gluUnProject(r1.x,r1.y,r1.z,modelMatrix,projMatrix,viewport,
		&t_r1.x,&t_r1.y,&t_r1.z);
	gluUnProject(r2.x,r2.y,r2.z,modelMatrix,projMatrix,viewport,
		&t_r2.x,&t_r2.y,&t_r2.z);
	gluUnProject(r3.x,r3.y,r3.z,modelMatrix,projMatrix,viewport,
		&t_r3.x,&t_r3.y,&t_r3.z);
/*	printf("Upd_ray: (%f %f %f)->(%f %f %f) == (%f %f %f)->(%f %f %f)\n",
		r1.x,r1.y,r1.z,r2.x,r2.y,r2.z,
		t_r1.x,t_r1.y,t_r1.z,t_r2.x,t_r2.y,t_r2.z);
*/
}


void *what_vp;

/* if a node changes, void the display lists */
/* Courtesy of Jochen Hoenicke */

void update_node(void *ptr) {
	struct VRML_Box *p = ptr;
	int i;
	p->_change ++;
	for (i = 0; i < p->_nparents; i++) {
		update_node(p->_parents[i]);
	}
}

/*explicit declaration. Needed for Collision_Child*/
void Group_Child(void *nod_);

/*********************************************************************
 * Code here is generated from the hashes in VRMLC.pm and VRMLRend.pm
 */
	';

#######################################################j


	print XS join '',@func;
	print XS join '',@vstruc;
#######################################################
	print XS <<'ENDHERE'

/*********************************************************************
 * Code here again comes almost verbatim from VRMLC.pm 
 */

/*********************************************************************
 *********************************************************************
 *
 * render_node : call the correct virtual functions to render the node
 * depending on what we are doing right now.
 */

void render_node(void *node) {
	struct VRML_Virt *v;
	struct VRML_Box *p;
	int srg;
	int sch;
	struct currayhit srh;
	int glerror = GL_NONE;
	char* stage = "";

	if(verbose) printf("\nRender_node %u\n",(unsigned int) node);
	if(!node) {return;}
	v = *(struct VRML_Virt **)node;
	p = node;

	if(verbose)
	  {
	    printf("=========================================NODE RENDERED===================================================\n");
	    printf("Render_node_v %d (%s) PREP: %d REND: %d CH: %d FIN: %d RAY: %d HYP: %d\n",v,
		   v->name, 
		   v->prep, 
		   v->rend, 
		   v->children, 
		   v->fin, 
		   v->rendray,
		   hypersensitive);
	    printf("Render_state geom %d light %d sens %d\n",
		   render_geom, 
		   render_light, 
		   render_sensitive);
	    printf ("pchange %d pichange %d vchanged %d\n",p->_change, p->_ichange,v->changed);
	  }

        /* we found viewpoint on render_vp pass, stop exploring tree.. */
        if(render_vp && found_vp) return;

	if(p->_change != p->_ichange && v->changed) 
	  {
	    if (verbose) printf ("rs 1 pch %d pich %d vch %d\n",p->_change,p->_ichange,v->changed);
	    v->changed(node);
	    p->_ichange = p->_change;
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "change";
	  }

	if(v->prep) 
	  {
	    if (verbose) printf ("rs 2\n");
	    v->prep(node);
	    if(render_sensitive && !hypersensitive) 
	      {
		upd_ray();
	      }
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "prep";
	  }

	if(render_proximity && v->proximity) 
	{
	    if (verbose) printf ("rs 2a\n");
	    v->proximity(node);
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "render_proximity";
	}

	if(render_collision && v->collision) 
	{
	    if (verbose) printf ("rs 2b\n");
	    v->collision(node);
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "render_collision";
	}

	if(render_geom && !render_sensitive && v->rend) 
	  {
	    if (verbose) printf ("rs 3\n");
	    v->rend(node);
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "render_geom";
	  }
	if(render_light && v->light) 
	  {
	    if (verbose) printf ("rs 4\n");
	    v->light(node);
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "render_light";
	  }
	/* Future optimization: when doing VP/Lights, do only 
	 * that child... further in future: could just calculate
	 * transforms myself..
	 */
	if(render_sensitive && p->_sens) 
	  {
	    if (verbose) printf ("rs 5\n");
	    srg = render_geom;
	    render_geom = 1;
	    if(verbose) printf("CH1 %d: %d\n",node, cur_hits, p->_hit);
	    sch = cur_hits;
	    cur_hits = 0;
	    /* HP */
	      srh = rph;
	    rph.node = node;
	    glGetDoublev(GL_MODELVIEW_MATRIX, rph.modelMatrix);
	    glGetDoublev(GL_PROJECTION_MATRIX, rph.projMatrix);
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "render_sensitive";

	  }
	if(render_geom && render_sensitive && !hypersensitive && v->rendray) 
	  {
	    if (verbose) printf ("rs 6\n");
	    v->rendray(node);
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "rs 6";
	  }
        

        if(hypersensitive == node)  /* is this really common to all rendering passes? -ncoder */
        {
            if (verbose) printf ("rs 7\n");
            hyper_r1 = t_r1;
            hyper_r2 = t_r2;
            hyperhit = 1;
        }
        if(v->children) {
            if (verbose) printf ("rs 8\n");
            v->children(node);
	    if(glerror == GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "children";
        }

	if(render_sensitive && p->_sens) 
	  {
	    if (verbose) printf ("rs 9\n");
	    render_geom = srg;
	    cur_hits = sch;
	    if(verbose) printf("CH3: %d %d\n",cur_hits, p->_hit);
	    /* HP */
	      rph = srh;
	  }

	if(v->fin) 
	  {
	    if (verbose) printf ("rs A\n");
	    v->fin(node);
	    if(render_sensitive && v == &virt_Transform) 
	      { 
		upd_ray();
	      }
	    if(glerror != GL_NONE && ((glerror = glGetError()) != GL_NONE) ) stage = "fin";
	  }
	if (verbose) printf("(end render_node)\n");
	if(glerror != GL_NONE)
	  {
	    printf("============== GLERROR : %s in stage %s =============\n",gluErrorString(glerror),stage);
	    printf("Render_node_v %d (%s) PREP: %d REND: %d CH: %d FIN: %d RAY: %d HYP: %d\n",v,
		   v->name, 
		   v->prep, 
		   v->rend, 
		   v->children, 
		   v->fin, 
		   v->rendray,
		   hypersensitive);
	    printf("Render_state geom %d light %d sens %d\n",
		   render_geom, 
		   render_light, 
		   render_sensitive);
	    printf ("pchange %d pichange %d vchanged %d\n",p->_change, p->_ichange,v->changed);
	    printf("==============\n");
	  }
}

/*
 * The following code handles keeping track of the parents of a given
 * node. This enables us to traverse the scene on C level for optimizations.
 *
 * We use this array code because VRML nodes usually don't have
 * hundreds of children and don't usually shuffle them too much.
 */

#define NODE_ADD_PARENT(a) add_parent(a,ptr)

void add_parent(void *node_, void *parent_) {
	struct VRML_Box *node = node_;
	struct VRML_Box *parent = parent_;
	if(!node) return;
	node->_nparents ++;
	if(node->_nparents > node->_nparalloc) {
		node->_nparalloc += 10;
		node->_parents = 
		 (node->_parents ? 
			realloc(node->_parents, sizeof(node->_parents[0])*
							node->_nparalloc) 
							:
			malloc(sizeof(node->_parents[0])* node->_nparalloc) 
		 );
	}
	node->_parents[node->_nparents-1] = parent_;
}

#define NODE_REMOVE_PARENT(a) add_parent(a,ptr)

void remove_parent(void *node_, void *parent_) {
	struct VRML_Box *node = node_;
	struct VRML_Box *parent = parent_;
	int i;
	if(!node) return;
	node->_nparents --;
	for(i=0; i<node->_nparents; i++) {
		if(node->_parents[i] == parent) {
			break;
		}
	}
	for(; i<node->_nparents; i++) {
		node->_parents[i] = node->_parents[i+1];
	}
}


void
render_hier(void *p, int rwhat, void *wvp)
{
	struct pt upvec = {0,1,0};
	GLdouble modelMatrix[16];

	render_vp = rwhat & VF_Viewpoint;
	found_vp = 0;
	render_geom =  rwhat & VF_Geom;
	render_light = rwhat & VF_Lights;
	render_sensitive = rwhat & VF_Sensitive;
	render_blend = rwhat & VF_Blend;
	render_proximity = rwhat & VF_Proximity;
	render_collision = rwhat & VF_Collision;
	curlight = 0;
	what_vp = wvp;
	hpdist = -1;

	if (!p) {
		fprintf(stderr, "Render_hier: arg 1 is NULL\n");
		return;
	}

	/* verbose = 1; */
	if (verbose)
  		printf("Render_hier node=%d what=%d what_vp=%d\n", p, rwhat, wvp);

	if (render_sensitive) {
		upd_ray();
	}
	render_node(p);

	/* Get raycasting results */
	if(render_sensitive) {
		if(hpdist >= 0) {
			if(verbose) printf("RAY HIT!\n");
		}
	}

	/*get viewpoint result, only for upvector*/
	if (render_vp &&
		ViewerUpvector.x == 0 &&
		ViewerUpvector.y == 0 &&
		ViewerUpvector.z == 0) {

		/* store up vector for gravity and collision detection */
		/* naviinfo.reset_upvec is set to 1 after a viewpoint change */
		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		matinverse(modelMatrix,modelMatrix);
		transform3x3(&ViewerUpvector,&upvec,modelMatrix);

		if (verbose) printf("ViewerUpvector = (%f,%f,%f)\n", ViewerUpvector);
	}
}


void
get_collisionoffset(double *x, double *y, double *z)
{
	struct pt res = CollisionInfo.Offset;

	/* uses mean direction, with maximum distance */
	if (CollisionInfo.Count == 0) {
	    *x = *y = *z = 0;
	} else {
	    if (vecnormal(&res, &res) == 0.) {
			*x = *y = *z = 0;
	    } else {
			vecscale(&res, &res, sqrt(CollisionInfo.Maximum2));
			*x = res.x;
			*y = res.y;
			*z = res.z;
	    }
	}
}


MODULE = VRML::VRMLFunc PACKAGE = VRML::VRMLFunc
PROTOTYPES: ENABLE



#####################################################################
# 
# Set the texture rendering size
# 
# OpenGL implementations specify a maximum texture size; resize
# textures to this size, or allow specifying a smaller size
#
#####################################################################

void
set_maxtexture_size (maxsize)
	int maxsize;
CODE:
        int texSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &texSize);
	
	/* see which one we should use */
	if (maxsize < texSize) {
		global_texSize = maxsize;
	} else {
		global_texSize = texSize;
	}



#####################################################################
#
# get an MPEG file
#
####################################################################

int 
read_mpg_file(init_tex, fname,repeatS,repeatT)
	unsigned int init_tex
	char *fname
	int repeatS
	int repeatT
CODE:
	/* go directly to the CFuncs/MPEG_Utils, and run from there */
#ifndef AQUA
	RETVAL = mpg_main(init_tex, fname, repeatS, repeatT);
#endif
OUTPUT:
	RETVAL

####################################################################
#

####################################################################
#
# Save Font Paths for later use in C, if Text nodes exist
#
####################################################################

int
save_font_path(myfp)
	char *myfp
CODE:
		strncpy(sys_fp,myfp,fp_name_len-20);



void *
alloc_struct(siz,virt)
	int siz
	void *virt
CODE:
	void *ptr = malloc(siz);
	struct VRML_Box *p = ptr;
	/* printf("Alloc: %d %d -> %d\n", siz, virt, ptr);  */
	*(struct VRML_Virt **)ptr = (struct VRML_Virt *)virt;
	p->_sens = p->_hit = 0;
	p->_intern = 0;
	p->_change = 153;
	p->_dlchange = 0;
	p->_dlist = 0;
        p->_parents = 0;
        p->_nparents = 0;
        p->_nparalloc = 0;
	p->_ichange = 0;
	p->_myshape = last_visited_shape; /* This is for display list regeneration */
	RETVAL=ptr;
OUTPUT:
	RETVAL

void
release_struct(ptr)
	void *ptr
CODE:
	struct VRML_Box *p = ptr;
	if(p->_parents) free(p->_parents);
	if(p->_dlist) glDeleteLists(p->_dlist,1);
	printf ("release_struct, texture needs deletion \n");
	free(ptr); /* COULD BE MEMLEAK IF STUFF LEFT INSIDE */

void
set_sensitive(ptr,sens)
	void *ptr
	int sens
CODE:
	/* Choose box randomly */
	struct VRML_Box *p = ptr;
	p->_sens = sens;

void 
set_hypersensitive(ptr)
	void *ptr
CODE:	
	hypersensitive = ptr;
	hyperhit = 0;

# get the hyperhit - eg, a planesensor, and SAVE the results for later. 
# right now the results are returned to Perl, and also saved for later
# use by direct C code. JAS
int
get_hyperhit()
CODE:
	double x1,y1,z1,x2,y2,z2;
	GLdouble projMatrix[16];

	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	gluUnProject(r1.x, r1.y, r1.z, rhhyper.modelMatrix,
		projMatrix, viewport, &x1, &y1, &z1);
	gluUnProject(r2.x, r2.y, r2.z, rhhyper.modelMatrix,
		projMatrix, viewport, &x2, &y2, &z2);

	/* printf ("get_hyperhit in VRMLC %f %f %f, %f %f %f\n",
		x1,y1,z1,x2,y2,z2); */

	/* and save this globally */
	hyp_save_posn.c[0] = x1; hyp_save_posn.c[1] = y1; hyp_save_posn.c[2] = z1;
	hyp_save_norm.c[0] = x2; hyp_save_norm.c[1] = y2; hyp_save_norm.c[2] = z2;
	RETVAL=1;
OUTPUT:
	RETVAL


void
set_viewer_delta(x,y,z)
	double x
	double y
	double z
CODE:
	    /*prototype code, currently UNUSED */
	ViewerDelta.x = x;
	ViewerDelta.y = y;
	ViewerDelta.z = z;


# allow Perl code to set the fieldOfView
void
set_fieldofview(angle)
	double angle
CODE:
	fieldofview = angle;


# allow Perl code to get the fieldOfView
double
get_fieldofview()
CODE:
	RETVAL = fieldofview;
OUTPUT:
	RETVAL


# setup_projection
void
setup_projection(ratio)
	double ratio
CODE:

	/* bounds check */
	if ((fieldofview <= 0.0) || (fieldofview > 180.0)) fieldofview=45.0;

        gluPerspective(fieldofview, ratio, 0.1, 21000.0);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
        glMatrixMode(GL_MODELVIEW);

int
get_hits(ptr)
	void *ptr
CODE:
	struct VRML_Box *p = ptr;
	RETVAL = p->_hit;
	p->_hit = 0;
OUTPUT:
	RETVAL

void
zero_hits(ptr)
	void *ptr
CODE:
	struct VRML_Box *p = ptr;
	p->_hit = 0;

void
render_verbose(i)
	int i;
CODE:
	verbose=i;

void 
render_verbose_collision(i)
	int i;
CODE:
	verbose_collision=i;


void
get_collisionoffset(x,y,z)
	double *x
	double *y
	double *z


void
render_hier(p,rwhat,wvp)
	void *p
	int rwhat
	void *wvp


void
render_geom(p)
	void *p
CODE:
	struct VRML_Virt *v;
	if(!p) {
		die("Render_geom null!??");
	}
	v = *(struct VRML_Virt **)p;
	v->rend(p);


void
do_render_collisions(p)
	void *p
PREINIT:
	struct pt v;
CODE:
	CollisionInfo.Offset.x = 0;
	CollisionInfo.Offset.y = 0;
	CollisionInfo.Offset.z = 0;
	CollisionInfo.Count = 0;
	CollisionInfo.Maximum2 = 0.;

	render_hier(p, VF_Collision, 0);
	get_collisionoffset(&(v.x), &(v.y), &(v.z));
	increment_pos(&Viewer, &v);


# get the current rayhit. Save the rayhit for later use by Cfunctions, eg, PlaneSensor
void *
get_rayhit()
CODE:
	double x,y,z;

	if(hpdist >= 0) {
		gluUnProject(hp.x,hp.y,hp.z,rh.modelMatrix,rh.projMatrix,viewport,&x,&y,&z);

		/* and save this globally */
		ray_save_posn.c[0] = x; ray_save_posn.c[1] = y; ray_save_posn.c[2] = z;

		RETVAL = rh.node;
	} else {
		RETVAL=0;
	}
OUTPUT:
	RETVAL

#*****************************************************************************
#
# signal to the Perl browser that Perl action is required. 
#
int 
BrowserAction()
CODE:
	RETVAL = BrowserAction;
OUTPUT:
	RETVAL

#*****************************************************************************
#
# return the action to the Browser
#
void
getAnchorBrowserAction(x)
	char * x
CODE:
	BrowserAction = FALSE;
	x = BrowserActionString;
OUTPUT:
	x


#*****************************************************************************
# return a C pointer to a func for the interpolator functions. Used in CRoutes
# to enable event propagation to call the correct interpolator
#
unsigned int
InterpPointer(x)
	char *x
CODE:
	void *pt;
	void do_Oint4(void *x);

	/* XXX still to do; Fog, Background, Viewpoint, NavigationInfo, Collision */

	if (strncmp("OrientationInterpolator",x,strlen("OrientationInterpolator"))==0) {
		pt = do_Oint4;
		RETVAL = (unsigned int) pt;
	} else if (strncmp("ScalarInterpolator",x,strlen("ScalarInterpolator"))==0) {
		pt = do_OintScalar;
		RETVAL = (unsigned int) pt;
	} else if (strncmp("ColorInterpolator",x,strlen("ColorInterpolator"))==0) {
		pt = do_Oint3;
		RETVAL = (unsigned int) pt;
	} else if (strncmp("PositionInterpolator",x,strlen("PositionInterpolator"))==0) {
		pt = do_Oint3;
		RETVAL = (unsigned int) pt;
	} else if (strncmp("CoordinateInterpolator",x,strlen("CoordinateInterpolator"))==0) {
		pt = do_OintCoord;
		RETVAL = (unsigned int) pt;
	} else if (strncmp("NormalInterpolator",x,strlen("NormalInterpolator"))==0) {
		pt = do_OintCoord;
		RETVAL = (unsigned int) pt;
	} else {
		RETVAL = 0;
	}
OUTPUT:
	RETVAL


#*******************************************************************************
# return lengths if C types - used for Routing C to C structures. Platform
# dependent sizes...
# These have to match the clength subs in VRMLFields.pm
# the value of zero (0) is used in VRMLFields.pm to indicate a "don't know".
# some, eg MultiVec3f have a value of -1, and this is just passed back again.
# (this is handled by the routing code; it's more time consuming)
# others, eg ints, have a value from VRMLFields.pm of 1, and return the
# platform dependent size. 

int
getClen(x)
	int x
CODE:
	switch (x) {	
		case 1:	RETVAL = sizeof (int);
			break;
		case 2:	RETVAL = sizeof (float);
			break;
		case 3:	RETVAL = sizeof (double);
			break;
		case 4:	RETVAL = sizeof (struct SFRotation);
			break;
		case 5: RETVAL = sizeof (struct SFColor);
			break;
		case 6: RETVAL = sizeof (struct SFVec2f);
			break;
		case -11: RETVAL = sizeof (unsigned int);
			break;
		default:	RETVAL = x;
	}
OUTPUT:
	RETVAL


#********************************************************************************
#
# register a route that can go via C, rather than perl.

void
do_CRoutes_Register(from, fromoffset, to_count, tonode_str, len, intptr, scrpt, extra)
	void *from
	int fromoffset
	int to_count
	char *tonode_str
	int len
	void *intptr
	int scrpt
	int extra
CODE:
	CRoutes_Register(from, fromoffset, to_count, tonode_str, len, intptr, scrpt, extra);

void
CRoutes_js_new (num, cx, glob, brow)
	int num
	void *cx
	void *glob
	void *brow
CODE:
	do_CRoutes_js_new (num, cx, glob, brow);


#********************************************************************************
#
# Free memory allocated in CRoutes_Register

void
do_CRoutes_free()
CODE:
	CRoutes_free();


#********************************************************************************
# Viewer functions implemented in C replacing viewer Perl module

void
do_print_viewer()
CODE:
	print_viewer(&Viewer);


unsigned int
do_get_buffer()
CODE:
	RETVAL = get_buffer(&Viewer);
OUTPUT:
	RETVAL

void
do_set_buffer(buffer)
   unsigned int buffer
CODE:
	set_buffer(&Viewer, buffer);

int
do_get_headlight()
CODE:
	RETVAL = get_headlight(&Viewer);
OUTPUT:
	RETVAL

void
do_toggle_headlight()
CODE:
	toggle_headlight(&Viewer);


void
do_viewer_togl()
CODE:
	viewer_togl(&Viewer, fieldofview);

void
do_set_eyehalf(eyehalf, eyehalfangle)
	double eyehalf
	double eyehalfangle
CODE:
	set_eyehalf(&Viewer, eyehalf, eyehalfangle);

#JAS void
#JAS do_set_viewer_type(type)
#JAS 	int type
#JAS CODE:
#JAS 	printf ("calling do_set_viewer_type\n");
#JAS 	set_viewer_type(type);

void
do_handle_key(key)
	char key
CODE:
	handle_key(&Viewer, TickTime, key);


void
do_handle_keyrelease(key)
	char key
CODE:
	handle_keyrelease(&Viewer, TickTime, key);


void
do_handle_tick()
CODE:
	handle_tick(&Viewer, TickTime);


void
do_handle(mev, button, x, y)
	char *mev
	unsigned int button
	double x
	double y
CODE:
	handle(&Viewer, mev, button, x, y);


int
use_keys()

void
set_viewer_type(type)
	int type

#********************************************************************************

#********************************************************************************
# Send a bind/unbind to this node

void
do_bind_to (x,outptr,bindValue)
	char *x
	void *outptr
	int bindValue
CODE:
	send_bind_to (x,outptr,bindValue);

#********************************************************************************
# do the events, and events, and events, until no more events triggered

void
do_propagate_events()
CODE:
	propagate_events();

#********************************************************************************
#Mouse events at beginning of event loop - Sensors.
#
# params - x		- pointer to type string; 
#	 - pt		- pointer to data structure (CNode) for this invocation
#	 - typ		- mouse action, eg "DRAG" (string! yeeech!)
#	 - over 	- isOver
void
handle_mouse_sensitive(x,pt,typ,over)
	char *x
	void *pt
	char *typ
	int over
CODE:
	if (strncmp("TouchSensor",x,strlen("TouchSensor"))==0) {
		do_TouchSensor (pt,typ,over);
		
	} else if (strncmp("PlaneSensor",x,strlen("PlaneSensor"))==0) {
		do_PlaneSensor (pt,typ,over);

	} else if (strncmp("CylinderSensor",x,strlen("CylinderSensor"))==0) {
		do_CylinderSensor (pt,typ,over);

	} else if (strncmp("SphereSensor",x,strlen("SphereSensor"))==0) {
		do_SphereSensor (pt,typ,over);

	} else if (strncmp("Anchor",x,strlen("Anchor"))==0) {
		do_Anchor (pt,typ,over);

	} else { printf ("do_handle_events, unknown %s\n",x);}
	
	



#********************************************************************************

double
get_timestamp()
CODE:
	static int loop_count = 0;
	double waittime;

	struct timeval mytime;
	struct timezone tz; /* unused see man gettimeofday */
	gettimeofday (&mytime,&tz);
	TickTime = (double) mytime.tv_sec + (double)mytime.tv_usec/1000000.0;

	/* First time through */
	if (loop_count == 0) {
		BrowserStartTime = TickTime;
		lastTime = TickTime;
	} else {
		// rate limit ourselves to 120fps. 
		waittime = TickTime - lastTime - 0.0084; 
		lastTime = TickTime;
		if (waittime < 0.0) {
			waittime = 1000.0 * -waittime;
			//printf ("waiting %d\n",(int)waittime);
			poll (waittimer,0,(int)waittime); // sleep for so many milliseconds
		}
	}

	if (loop_count == 25) {
		BrowserFPS = 25.0 / (TickTime-BrowserStartTime);
		//printf ("Fps: %f\n",BrowserFPS);
		BrowserStartTime = TickTime; 
		loop_count = 1;
	} else {
		loop_count++;
	}
	RETVAL = TickTime;
OUTPUT:
	RETVAL



void
AudioClockTick(node)
	void *node
CODE:
	do_AudioTick(node);

void
MovieTextureClockTick(node)
	void *node
CODE:
	do_MovieTextureTick(node);

void 
ProximitySensorClockTick(node)
	void *node
CODE:
	do_ProximitySensorTick(node);

void 
TimeSensorClockTick(node)
	void *node
CODE:
	do_TimeSensorTick(node);

void
CollisionClockTick(node)
	void *node
CODE:
	struct VRML_Collision *cx = node;
	if (cx->__hit == 3) {
		/* printf ("COLLISION at %f\n",TickTime); */
		cx->collideTime = TickTime;
		mark_event ((unsigned int) node, offsetof(struct VRML_Collision, collideTime));
	}


# save the specific FreeWRL version number from the Config files.
void
SaveVersion(str)
	char *str
CODE:
	BrowserVersion = malloc (strlen(str)+1);
	strcpy (BrowserVersion,str);

# save the specific FreeWRL version number from the Config files.
void
SaveURL(str)
	char *str
CODE:
	if (BrowserURL != NULL) free (BrowserURL);
	BrowserURL = malloc (strlen(str)+1);
	strcpy (BrowserURL,str);


#****************JAVASCRIPT FUNCTIONS*********************************

void
setJSVerbose(v)
	int v;
CODE:
{
	JSVerbose = v;
}


## worry about garbage collection here ???
void
jsinit(num, sv_js)
	int num
	SV *sv_js
CODE:
	JSInit(num,sv_js);

void
jscleanup(num)
	int num
CODE:
	JScleanup(num);

int
jsrunScript(num, script, rstr, rnum)
	int num
	char *script
	SV *rstr
	SV *rnum
CODE:
	RETVAL = JSrunScript (num, script, rstr, rnum);
OUTPUT:
RETVAL
rstr
rnum


int
jsGetProperty(num, script, rstr)
	int num
	char *script
	SV *rstr
CODE:
	RETVAL = JSGetProperty (num, script, rstr);
OUTPUT:
RETVAL
rstr


int
jsSFColorSet(cx, obj, name, sv)
	void *cx
	void *obj
	char *name
	SV *sv
CODE:
{
	JSContext *_cx;
	JSObject *_obj, *_sfcolObj;
	jsval _val;
	void *_privPtr;

	_cx = cx;
	_obj = obj;
	if (JSVerbose) {
		printf("jsSFColorSet: obj %u, name %s\n", (unsigned int) _obj, name);
	}
	if (!JS_GetProperty(_cx, _obj, name, &_val)) {
		fprintf(stderr, "JS_GetProperty failed in jsSFColorSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	if (!JSVAL_IS_OBJECT(_val)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in jsSFColorSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	_sfcolObj = JSVAL_TO_OBJECT(_val);

	if ((_privPtr = JS_GetPrivate(_cx, _sfcolObj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in jsSFColorSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	SFColorNativeSet(_privPtr, sv);
	RETVAL = JS_TRUE;
}
OUTPUT:
RETVAL
cx
obj


int
jsSFImageSet(cx, obj, name, sv)
	void *cx
	void *obj
	char *name
	SV *sv
CODE:
{
	JSContext *_cx;
	JSObject *_obj, *_sfimObj;
	jsval _val;
	void *_privPtr;

	_cx = cx;
	_obj = obj;
	if (JSVerbose) {
		printf("jsSFImageSet: obj %u, name %s\n", (unsigned int) _obj, name);
	}
	if (!JS_GetProperty(_cx, _obj, name, &_val)) {
		fprintf(stderr, "JS_GetProperty failed in jsSFImageSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	if (!JSVAL_IS_OBJECT(_val)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in jsSFImageSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	_sfimObj = JSVAL_TO_OBJECT(_val);

	if ((_privPtr = JS_GetPrivate(_cx, _sfimObj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in jsSFColorSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	SFImageNativeSet(_privPtr, sv);
	RETVAL = JS_TRUE;
}
OUTPUT:
RETVAL
cx
obj


int
jsSFVec2fSet(cx, obj, name, sv)
	void *cx
	void *obj
	char *name
	SV *sv
CODE:
{
	JSContext *_cx;
	JSObject *_obj, *_sfvec2fObj;
	jsval _val;
	void *_privPtr;

	_cx = cx;
	_obj = obj;
	if (JSVerbose) {
		printf("jsSFVec2fSet: obj %u, name %s\n", (unsigned int) _obj, name);
	}
	if(!JS_GetProperty(_cx, _obj, name, &_val)) {
		fprintf(stderr, "JS_GetProperty failed in jsSFVec2fSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	if(!JSVAL_IS_OBJECT(_val)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in jsSFVec2fSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	_sfvec2fObj = JSVAL_TO_OBJECT(_val);

	if ((_privPtr = JS_GetPrivate(_cx, _sfvec2fObj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in jsSFVec2fSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	SFVec2fNativeSet(_privPtr, sv);
	RETVAL = JS_TRUE;
}
OUTPUT:
RETVAL
cx
obj


int
jsSFRotationSet(cx, obj, name, sv)
	void *cx
	void *obj
	char *name
	SV *sv
CODE:
{
	JSContext *_cx;
	JSObject *_obj, *_sfrotObj;
	jsval _val;
	void *_privPtr;

	_cx = cx;
	_obj = obj;
	if (JSVerbose) {
		printf("jsSFRotationSet: obj %u, name %s\n", (unsigned int) _obj, name);
	}
	if (!JS_GetProperty(_cx, _obj, name, &_val)) {
		fprintf(stderr, "JS_GetProperty failed in jsSFRotationSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	if (!JSVAL_IS_OBJECT(_val)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in jsSFRotationSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	_sfrotObj = JSVAL_TO_OBJECT(_val);

	if ((_privPtr = JS_GetPrivate(_cx, _sfrotObj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in jsSFRotationSet.\n");
		RETVAL = JS_FALSE;
		return;
	}
	SFRotationNativeSet(_privPtr, sv);
	RETVAL = JS_TRUE;
}
OUTPUT:
RETVAL
cx
obj


int
addSFNodeProperty(num, nodeName, name, str)
	int num
	char *nodeName
	char *name
	char *str
CODE:
	RETVAL = JSaddSFNodeProperty(num, nodeName, name, str);
OUTPUT:
RETVAL


int
addGlobalAssignProperty(num, name, str)
	int num
	char *name
	char *str
CODE:
	RETVAL = JSaddGlobalAssignProperty(num, name, str);
OUTPUT:
RETVAL


int
addGlobalECMANativeProperty(num, name)
	int num
	char *name
CODE:
	RETVAL = JSaddGlobalECMANativeProperty(num, name);
OUTPUT:
RETVAL

int
paramIndex(evin, evtype)
	char *evin
	char *evtype
CODE:
	RETVAL = JSparamIndex (evin,evtype);
OUTPUT:
RETVAL


# allow Javascript to add/remove children when the parent is a USE - see JS/JS.pm.
void
jsManipulateChild(par, fiel, child)
	int par
	char *fiel
	int child
CODE:
	char onechildline[100];

	sprintf (onechildline, "[ %d ]",child);
	getMFNodetype (onechildline, (struct Multi_Node *) par,
		!strncmp (fiel,"addChild",strlen ("addChild")));

# link into EAI.
void
do_create_EAI(eailine)
	char *eailine
	CODE:
	create_EAI(eailine);

void
do_handle_EAI ()
	CODE:
	handle_EAI();


#****************JAVASCRIPT FUNCTIONS*********************************

ENDHERE
;
	print XS join '',@xsfn;
	print XS '

BOOT:
	I_OPENGL;
	new_tessellation();
	
';

	open PM, ">VRMLFunc.pm";
	print PM "
# VRMLFunc.pm, generated by VRMLC.pm. DO NOT MODIFY, MODIFY VRMLC.pm INSTEAD
package VRML::VRMLFunc;
require DynaLoader;
require Exporter;
\@ISA=qw(Exporter DynaLoader);
";
        print PM &gen_constants_pm_header();
	print PM "
bootstrap VRML::VRMLFunc;
sub load_data {
	my \$n = \\\%VRML::CNodes;
";
	print PM join '',@poffsfn;
	print PM "
}
";
        print PM &gen_constants_pm_body();

}


gen();


