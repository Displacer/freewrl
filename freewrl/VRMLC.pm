# $Id: VRMLC.pm,v 1.71 2003/03/19 15:23:17 crc_canada Exp $
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
#include "GL/gl.h"

struct pt {GLdouble x,y,z;};

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

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include "OpenGL/OpenGL.m"
#include "CFuncs/Structs.h"
#include "CFuncs/headers.h"
#include "CFuncs/constants.h"

#include "CFuncs/LinearAlgebra.h"
#include "CFuncs/Collision.h"

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

int horiz_div = 20; int vert_div = 20;
int vp_dist = 200000;


int smooth_normals = -1; /* -1 means, uninitialized */

int cur_hits=0;

/* Collision detection results */
struct sCollisionInfo CollisionInfo = { {0,0,0} , 0, 0. };

/* Displacement of viewer , used for colision calculation  PROTYPE, CURRENTLY UNUSED*/
struct pt ViewerDelta = {0,0,0}; 

/*dimentions of viewer, and "up" vector (for collision detection) */
struct sNaviInfo naviinfo = {0.25, 1.6, 0.75};

/*for alignment of collision cylinder, and gravity (later). */
struct pt ViewerUpvector = {0,0,0};

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
static int SoundEngineStarted = FALSE;


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

	if(verbose) printf("\nRender_node %d\n",node);
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
# for AudioClips - return the duration of the last registered clip
#
# This should not be necessary, but I had difficulty setting this
# from within C in the AudioClip rend function in VRMLRend.pm
#
#####################################################################
float
return_Duration (indx)
	int indx;
CODE:
	//printf ("return_Duration indx %d val %f\n",indx,AC_LastDuration[indx]);
	if (indx < 0) RETVAL = -1.0;
	else if (indx > 50) RETVAL = -1.0;
	else RETVAL = AC_LastDuration[indx];
OUTPUT:
	RETVAL

#####################################################################
#
# SetAudioActive - tell the SoundEngine whether a source is active or not.
#
#####################################################################
void
SetAudioActive (indx,status)
	int indx;
	int status;
CODE:
	if (!SoundEngineStarted) {
		//printf ("SetAudioActive: initializing SoundEngine\n");
		SoundEngineStarted = TRUE;
		SoundEngineInit();
	}

	SetAudioActive (indx,status);		


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
	RETVAL = mpg_main(init_tex, fname, repeatS, repeatT);
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
	FILE *fp;
	char fname [1024];
	strncpy(fname,myfp,fp_name_len-20);
	
	strcat(fname,"/Amrigoi.ttf");

	//printf ("save_font_path, trying %s\n",fname);
	if ((fp=fopen(fname,"r"))!=NULL) {
		// this path is ok
		strncpy(sys_fp,myfp,fp_name_len-20);
		fclose(fp);
		RETVAL=1;
	} else {
		RETVAL=0;
	}
OUTPUT:
	RETVAL



void *
alloc_struct(siz,virt)
	int siz
	void *virt
CODE:
	void *ptr = malloc(siz);
	struct VRML_Box *p = ptr;
	if(verbose) printf("Alloc: %d %d -> %d\n", siz, virt, ptr); 
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

int
get_hyperhit(x1,y1,z1,x2,y2,z2)
	double x1
	double y1
	double z1
	double x2
	double y2
	double z2
CODE:
	GLdouble projMatrix[16];
	/*
	if(hyperhit) {
		x1 = hyper_r1.x;
		y1 = hyper_r1.y;
		z1 = hyper_r1.z;
		x2 = hyper_r2.x;
		y2 = hyper_r2.y;
		z2 = hyper_r2.z;
		RETVAL=1;
	} else RETVAL = 0;
	*/
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	gluUnProject(r1.x, r1.y, r1.z, rhhyper.modelMatrix,
		projMatrix, viewport, &x1, &y1, &z1);
	gluUnProject(r2.x, r2.y, r2.z, rhhyper.modelMatrix,
		projMatrix, viewport, &x2, &y2, &z2);
	RETVAL=1;
OUTPUT:
	RETVAL
	x1
	y1
	z1
	x2
	y2
	z2
	
void
get_collisionoffset(x,y,z)
	double x
	double y
	double z
CODE:
	    /*uses mean direction, with maximum distance */
	if(CollisionInfo.Count == 0) {
	    x = y = z = 0;
	} else {
	    struct pt res = CollisionInfo.Offset;
	    if(vecnormal(&res,&res) == 0.) {
		x = y = z = 0;
	    } else {
		vecscale(&res,&res,sqrt(CollisionInfo.Maximum2));
		x = res.x;
		y = res.y;
		z = res.z;
	    }
	}
OUTPUT:
	x
	y
	z

void
reset_collisionoffset()
CODE:
	CollisionInfo.Offset.x = 0;
	CollisionInfo.Offset.y = 0;
	CollisionInfo.Offset.z = 0;
	CollisionInfo.Count = 0;
	CollisionInfo.Maximum2 = 0.;

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


void
set_naviinfo(width,height,step)
	double width
	double height
	double step
CODE:
	naviinfo.width = width;
	naviinfo.height = height;
	naviinfo.step = step;

void
reset_upvector()
CODE:
    ViewerUpvector.x = 0;
    ViewerUpvector.y = 0;
    ViewerUpvector.z = 0;

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
render_hier(p,rwhat,wvp)
	void *p
	int rwhat
	void *wvp
CODE:

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
	if(!p) {
		die("Render_hier null!??");
	}
	//verbose = 1;
	if(verbose)
  		printf("Render_hier node=%d what=%d what_vp=%d\n", p, rwhat, wvp);

	if(render_sensitive) 
		upd_ray();
	render_node(p);

	/* Get raycasting results */
	if(render_sensitive) {
		if(hpdist >= 0) {
			if(verbose) printf("RAY HIT!\n");
		}
	}

	/*get viewpoint result, only for upvector*/
	if(render_vp && ViewerUpvector.x == 0 && ViewerUpvector.y == 0 && ViewerUpvector.z == 0) {
 		struct pt upvec = {0,1,0};
		GLdouble modelMatrix[16];
		/*store up vector for gravity and collision detection */
		/*naviinfo.reset_upvec is set to 1 after a viewpoint change*/
		glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
		matinverse(modelMatrix,modelMatrix);
		transform3x3(&ViewerUpvector,&upvec,modelMatrix);
		if(verbose) printf("ViewerUpvector = (%f,%f,%f)\n",ViewerUpvector);
	}


void *
get_rayhit(x,y,z,nx,ny,nz,tx,ty)
	double x
	double y
	double z
	double nx
	double ny
	double nz
	double tx
	double ty
CODE:
	if(hpdist >= 0) {
		gluUnProject(hp.x,hp.y,hp.z,rh.modelMatrix,rh.projMatrix,viewport,&x,&y,&z);
		RETVAL = rh.node;
	} else {
		RETVAL=0;
	}
OUTPUT:
	RETVAL
	x
	y
	z
	nx
	ny
	nz
	tx
	ty

void 
get_proximitysensor_vecs(node,hit,x1,y1,z1,x2,y2,z2,q2)
	void *node
	int hit
	double x1
	double y1
	double z1
	double x2
	double y2
	double z2
	double q2
CODE:
	struct VRML_ProximitySensor *px = node;
	hit = px->__hit;
	px->__hit = 0;
	x1 = px->__t1.c[0];
	y1 = px->__t1.c[1];
	z1 = px->__t1.c[2];
	x2 = px->__t2.r[0];
	y2 = px->__t2.r[1];
	z2 = px->__t2.r[2];
	q2 = px->__t2.r[3];
	/* printf ("get_proximitysensor_vecs, x %f y %f z %f, x2 %f y2 %f z2 %f q2 %f\n", 
			x1,y1,z1,x2,y2,z2,q2);  */
OUTPUT:
	hit
	x1
	y1
	z1
	x2
	y2
	z2
	q2

void 
get_collision_info(node,hit)
	void *node
	int hit
CODE:
	struct VRML_Collision *cx = node;
	hit = cx->__hit;
OUTPUT:
	hit


void
set_vpdist(dist)
int dist
CODE:
	vp_dist = dist;

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


