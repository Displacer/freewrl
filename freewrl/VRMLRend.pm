# Copyright (C) 1998 Tuomas J. Lukka 1999 John Stewart CRC Canada
# Portions Copyright (C) 1998 Bernhard Reiter
# DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
# See the GNU Library General Public License (file COPYING in the distribution)
# for conditions of use and redistribution.
#
# $Id: VRMLRend.pm,v 1.72 2002/08/02 15:08:53 ncoder Exp $
#
# Name:        VRMLRend.c
# Description: 
#              Fills Hash Variables with "C" Code. They are used by VRMLC.pm
#              to write the C functions-source to render different nodes.
#              
#              Certain Abbreviation are used, some are substituted in the
#              writing process in get_rendfunc() [VRMLC.pm]. 
#              Others are "C-#defines".
#              e.g. for #define glTexCoord2f(a,b) glTexCoord2f(a,b) see gen() [VRMLC.pm] 
#  
#              Hashes filled in this file:
#                      %RendC, %PrepC, %FinC, %ChildC, %LightC
#
# $Log: VRMLRend.pm,v $
# Revision 1.72  2002/08/02 15:08:53  ncoder
# Corrected proximitysensor changes.
# Added more fine-grained glError checking
#
# Revision 1.71  2002/07/31 20:56:53  ncoder
# Added:
#     Support for "collide" boolean field in collision nodes
#     Support for proxy nodes.
# 	Collide time envents launched
#
# Small improvement in ProximitySensors
#
# Revision 1.70  2002/07/30 18:12:14  ncoder
# Added collision support for extrusions
# Polyrep disp. still flakey, i'll need to do corrections.
#
# Revision 1.69  2002/07/29 20:07:39  ncoder
# Removed lingering printmatrix debug code. (VRMLRend.pm)
#
# Other non significant code changes.
#
# Revision 1.68  2002/07/29 19:36:25  crc_canada
# Background Textures now working with new style of texture reading.
#
# Revision 1.67  2002/07/26 18:15:51  ncoder
# Added support (incomplete) of index face set collisions.
# Bugs corrected in Collision detection
# Further cleanup in rendering (GLBackEnd.pm)
# removed useless free in Polyrep.c
#
# Revision 1.66  2002/07/19 16:56:52  ncoder
# added collision detection for
# Boxes, cones, cylinders.
# Added a debug flag to print out collision information ("collision")
#
# a few modifications/additions to functions in LinearAlgebra
#
# Revision 1.65  2002/07/15 15:38:41  crc_canada
# enable sharing of textures, if they have the same url
#
# Revision 1.63  2002/07/09 16:13:59  crc_canada
# Text nodes now in main module, seperate module removed
#
# Revision 1.62  2002/07/03 17:02:59  crc_canada
# MovieTexture now has internal decoding
#
# Revision 1.61  2002/06/21 20:15:43  crc_canada
# compile warning removed in Appearance
#
# Revision 1.60  2002/06/21 19:58:58  crc_canada
# Material now called for RGB textures, but diffuseColor set to white.
#
# Revision 1.59  2002/06/17 14:41:45  ncoder
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
# Revision 1.58  2002/06/17 12:30:28  crc_canada
# Material properties for greyscale textures and RGB textures should be ok now
#
# Revision 1.57  2002/05/30 21:18:04  ncoder
# Fixed bug with the order of the multiplication of the transformations while rendering the viewpoint.
# Increased performance of viewpoint rendering code.
#
# Added some comments.
#
# Revision 1.56  2002/05/08 18:15:07  crc_canada
# Text nodes without a FontStyle were not rendered correctly. Fixed.
#
# Revision 1.55  2002/05/06 16:44:20  crc_canada
# initial MovieTexture work
#
# Revision 1.53  2002/05/01 15:13:11  crc_canada
# Fog support
#
# better texture binding
#
# Revision 1.52  2002/04/17 19:21:15  crc_canada
# glErrors are no longer checked for and printed. Code is in there so at some
# point in time, we can have a look at the problem again.
#
# Revision 1.51  2002/02/05 20:32:53  crc_canada
# Aubery Jaffers lighting changes.
#
# Added support code for nodes not currently being supported; gets
# around parsing problems.
#
# Revision 1.50  2002/01/23 21:07:56  crc_canada
# textureTransform problems fixed (they were not being zeroed), but efficiency
# may be better handled.
#
# some material/texture problems - material should show through textures, now
# works.
#
# Revision 1.49  2002/01/22 17:42:27  crc_canada
# I made a mistake in comment types; comment changed to C style from Perl style
#
# Revision 1.48  2002/01/20 15:05:25  crc_canada
# if display lists change when in creation, don't do the glEnd for them
#
# Revision 1.47  2002/01/09 16:14:45  crc_canada
# removal of some debugging statements, and it no longer dies on some proximity
# sensor errors.
#
# Revision 1.46  2002/01/08 18:55:59  crc_canada
# standard text and fontstyle now works - not quite up to standard, though.
#
# Revision 1.45  2002/01/04 20:19:01  crc_canada
# FontStyle and Text node work
#
# Revision 1.44  2001/12/14 17:52:34  crc_canada
# ElevationGrid and IndexedFaceSet texture over colour problem resolved.
#
# Revision 1.43  2001/12/14 16:35:34  crc_canada
# TextureTransform bug fixed - texture transforms were not within the
# associated Display List for the Shape. Moved creating display list to
# before texture code in Shape node.
#
# Revision 1.42  2001/12/14 14:46:06  crc_canada
# Transparency issues fixed, this time with debug statements removed!
#
# Revision 1.41  2001/12/14 14:29:20  crc_canada
# Material transparency now handled by the GL_STIPPLE method. This appears
# to be how the browser in the NIST test renders transparency.
#
# Revision 1.40  2001/12/12 19:17:16  crc_canada
# LODS now spec compliant
#
# Revision 1.39  2001/08/16 16:56:25  crc_canada
# Viewpoint work
#
# Revision 1.38  2001/07/31 16:20:33  crc_canada
# more Background node work
#
# Revision 1.37  2001/07/05 16:04:33  crc_canada
# Initial IFS texture default code re-write
#
# Revision 1.36  2001/06/25 18:34:42  crc_canada
# ElevationGrid default textures now correct.
#
# Revision 1.35  2001/06/18 19:11:50  crc_canada
# Background ground angle bug if > 3 angles fixed.
#
# Revision 1.34  2001/06/18 17:24:32  crc_canada
# IRIX compile warnings removed.
#
# Revision 1.33  2001/06/15 19:32:18  crc_canada
# lighting disabled as per spec if no Material and/or no Appearance
#
# Revision 1.32  2001/06/01 15:37:36  crc_canada
# ProximitySensor now has correct axis for rotations when rotating about
# the X axis. This affects many things, particularly the HUD test (24.wrl)
#
# Revision 1.31  2001/05/16 18:04:17  crc_canada
# changes to allow compiling on Irix without errors (still warnings, though)
#
# Revision 1.30  2001/05/16 16:00:06  crc_canada
# Check for glError at the end of Shape node.
#
# Revision 1.29  2001/05/11 15:00:11  crc_canada
# Cone Normals now correct
#
# Revision 1.28  2001/05/04 19:51:32  crc_canada
# some extraneous debugging code removed.
#
# Revision 1.27  2001/05/03 20:24:08  crc_canada
# Proper use of Display lists and Textures for Shape nodes and below.
#
# Revision 1.26  2001/04/27 16:50:03  crc_canada
# Working display lists for Shape nodes
#
# Revision 1.25  2001/04/24 19:54:16  crc_canada
# Display list work
#
# Revision 1.24  2001/03/26 17:41:50  crc_canada
# Background rendering for some OpenGL implementations fixed.
# IndexedLineSet no longer disrupts other renderings.
#
# Revision 1.23  2001/03/23 16:00:02  crc_canada
# IndexedLineSet culling disabled - it was affecting other shapes in the scene graph.
#
# Revision 1.22  2001/03/13 16:19:57  crc_canada
# Pre 0.28 checkin
#
# Revision 1.20  2000/12/20 17:27:52  crc_canada
# more IndexedFaceSet work - normals this time.
#
# Revision 1.19  2000/12/18 21:19:59  crc_canada
# IndexedFaceSet colorPerVertex and colorIndex now working correctly
#
# Revision 1.18  2000/12/08 13:10:39  crc_canada
# Two fixes to Background nodes:
# 	- possible core dump if only one sky angle and no ground angles.
# 	- make background sphere a lot larger.
#
# Revision 1.17  2000/12/07 19:12:25  crc_canada
# code cleanup, and IndexedFaceSet texture mapping
#
# Revision 1.16  2000/11/16 18:46:02  crc_canada
# Nothing much - just comments and verbose code.
#
# Revision 1.15  2000/11/15 16:37:01  crc_canada
# Removed some extra opengl calls from box.
#
# Revision 1.14  2000/11/03 21:52:16  crc_canada
# Speed updates - texture objects added
#
# Revision 1.13  2000/10/28 17:42:51  crc_canada
# EAI addchildren, etc, should be ok now.
#
# Revision 1.12  2000/09/03 20:17:50  rcoscali
# Made some test for blending
# Tests are displayed with 38 & 39.wrl�
#
# Revision 1.11  2000/08/31 23:00:00  rcoscali
# Add depth 2 support (2 channels/color components) which isMINANCE_ALPHAre (wi
#
# Revision 1.10  2000/08/30 00:04:18  rcoscali
# Comment out a glDisable(GL_LIGHTING) (uncommented by mistake ??)
# Fixed a problem in cone rendering (order of the vertexes)i
#
# Revision 1.9  2000/08/07 01:24:45  rcoscali
# Removed a glShadeModel(mooth) forgotten after a test
#
# Revision 1.8  2000/08/07 01:03:09  rcoscali
# Fixed another little problem on texture of cone. It was mapped in the wrong way.
#
# Revision 1.7  2000/08/07 00:28:44  rcoscali
# Removed debug macros and traces for sphere
#
# Revision 1.6  2000/08/06 22:55:17  rcoscali
# Ok ! All is like it should be for 0.26
# I tag the release as A_0_26
#
# Revision 1.5  2000/08/06 20:44:46  rcoscali
# Cone OK ... Now doing sphere ...
#
# Revision 1.4  2000/08/06 20:14:41  rcoscali
# Box Ok too. Now doing cone ...
#
# Revision 1.3  2000/08/06 19:48:37  rcoscali
# Fixed Cylinder. Now attacking cone.
#
# Revision 1.2  2000/08/04 22:54:41  rcoscali
# Add a cvs header, mainly for test
#
#



#######################################################################
#######################################################################
#######################################################################
#
# Rend --
#  actually render the node
#
#

# Rend = real rendering
%RendC = (
Box => '
	 float x = $f(size,0)/2;
	 float y = $f(size,1)/2;
	 float z = $f(size,2)/2;

	/* for shape display list redrawing */
	this_->_myshape = last_visited_shape; 

	 glPushAttrib(GL_LIGHTING);
	 glShadeModel(GL_FLAT);
	 glBegin(GL_QUADS);

		/* front side */
		glNormal3f(0,0,1);
		glTexCoord2f(1,1);
		glVertex3f(x,y,z);
		glTexCoord2f(0,1);
		glVertex3f(-x,y,z);
		glTexCoord2f(0,0);
		glVertex3f(-x,-y,z);
		glTexCoord2f(1,0);
		glVertex3f(x,-y,z);

		/* back side */
		glNormal3f(0,0,-1);
		glTexCoord2f(0,0);
		glVertex3f(x,-y,-z);
		glTexCoord2f(1,0);
		glVertex3f(-x,-y,-z);
		glTexCoord2f(1,1);
		glVertex3f(-x,y,-z);
		glTexCoord2f(0,1);
		glVertex3f(x,y,-z);

		/* top side */
		glNormal3f(0,1,0);
		glTexCoord2f(0,0);
		glVertex3f(-x,y,z);
		glTexCoord2f(1,0);
		glVertex3f(x,y,z);
		glTexCoord2f(1,1);
		glVertex3f(x,y,-z);
		glTexCoord2f(0,1);
		glVertex3f(-x,y,-z);

		/* down side */
		glNormal3f(0,-1,0);
		glTexCoord2f(0,0);
		glVertex3f(-x,-y,-z);
		glTexCoord2f(1,0);
		glVertex3f(x,-y,-z);
		glTexCoord2f(1,1);
		glVertex3f(x,-y,z);
		glTexCoord2f(0,1);
		glVertex3f(-x,-y,z);

		/* right side */
		glNormal3f(1,0,0);
		glTexCoord2f(0,0);
		glVertex3f(x,-y,z);
		glTexCoord2f(1,0);
		glVertex3f(x,-y,-z);
		glTexCoord2f(1,1);
		glVertex3f(x,y,-z);
		glTexCoord2f(0,1);
		glVertex3f(x,y,z);

		/* left side */
		glNormal3f(-1,0,0);
		glTexCoord2f(1,0);
		glVertex3f(-x,-y,z);
		glTexCoord2f(1,1);
		glVertex3f(-x,y,z);
		glTexCoord2f(0,1);
		glVertex3f(-x,y,-z);
		glTexCoord2f(0,0);
		glVertex3f(-x,-y,-z);
		glEnd();
		glDepthMask(GL_TRUE);
	glPopAttrib();
	
',


Cylinder => '
		int div = horiz_div;
		float df = div;
		float h = $f(height)/2;
		float r = $f(radius);
		float a,a1,a2;
		DECL_TRIG1
		int i = 0;
		INIT_TRIG1(div)
        
		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape;

		if($f(top)) {
			            /*	printf ("Cylinder : top\n"); */
			glBegin(GL_POLYGON);
			glNormal3f(0,1,0);
			START_TRIG1
			for(i=0; i<div; i++) {
				glTexCoord2f( 0.5 - 0.5*SIN1, 0.5 - 0.5*COS1);
				glVertex3f( -r*SIN1, (float)h, r*COS1 );
				UP_TRIG1
			}
			glEnd();
		} else {
			/* printf ("Cylinder : NO top\n"); */
		}

		if($f(bottom)) {
            		/* printf ("Cylinder : bottom\n"); */
			glBegin(GL_POLYGON);
			glNormal3f(0,-1,0);
			START_TRIG1
			for(i=0; i<div; i++) {
				glTexCoord2f(0.5+0.5*SIN1,0.5+0.5*COS1);
				glVertex3f(r*SIN1,(float)-h,r*COS1);
				UP_TRIG1
			}
			glEnd();
		} else {
			/* printf ("Cylinder : NO bottom\n"); */
		} 

		if($f(side)) {
				/* if(!nomode) {
				glPushAttrib(GL_LIGHTING);
				} */
			glBegin(GL_QUADS);
			START_TRIG1
			for(i=0; i<div; i++) {
				float lsin = SIN1;
				float lcos = COS1;
				UP_TRIG1;

				glNormal3f(lsin, 0.0, lcos);
 				glTexCoord2f(1.0-((float)i/df), 1.0);
				 glVertex3f((float)r*lsin, (float)h, (float)r*lcos);

				glNormal3f(SIN1, 0.0, COS1);
 				glTexCoord2f(1.0-(((float)i+1.0)/df), 1.0);
				glVertex3f(r*SIN1,  (float)h, r*COS1);

				/* glNormal3f(SIN1, 0.0, COS1); (same) */
				glTexCoord2f(1.0-(((float)i+1.0)/df), 0.0);
				glVertex3f(r*SIN1, (float)-h, r*COS1);

				glNormal3f(lsin, 0.0, lcos);
				glTexCoord2f(1.0-((float)i/df), 0.0);
				glVertex3f((float)r*lsin, (float)-h, (float)r*lcos);


			}
			glEnd();
				/*
				if(!nomode) {
				glPopAttrib();
				}
				*/
		}
',


Cone => '
	/*==================================================================
	May 10th 2001, Alain Gagnon.
	This block of code was fixed from the original because of normals.  
	 y axis
	 ^
	 |                                           
	 |       /|\             /|                     
	 |      / | \          /  |                   
	 |     /  |  \ml     /hyp |                  
	 |    / hreal \    /      |htwo
	 |   /    |   T\ /G        |
	 |   -----+----- ---------+    
	 |
	 |  |__r__|         rtwo
	 +--------------------------->x axis
	 |
	To understand these variables is to understand this code.
		r     : the radius of the cone bottom 
		hreal : the actual height of the cone
		h     : hreal/2
	        ml    : the hypothenuse on the cone side
	 
		hyp   : a unit vector (length = 1) which is perpendicular to
			ml.  Is is a normal for a given triangle making up 
			the cone side.
		htwo  : the height of the hyp vector
		rtwo  : a radius tha yields the start point of each normal
			vector.
		div   : the number of triangles that make up the cone surface
		d_div : div * 2

		theta : the angle located at T. 
		gamma : the angle located at G. 
	===================================================================*/

		int div = horiz_div;
		int d_div = div * 2;
		float df = div;
		float h = $f(height)/2;
		float r = $f(bottomRadius); 
		float a,a1;
		int i;
		DECL_TRIG1


		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape;

		if(h <= 0 && r <= 0) {return;}

		/* The angular distance of each step in the rotations that create  */
		/* the bottom and the sides of the cone is halved.                 */
		/* This permits the correct direction of the normals at the top of */
		/* the cone.  Otherwise, the normals would be slighlty off.        */
		INIT_TRIG1(d_div)

		if($f(bottom)) {
            		/* printf ("Cone : bottom\n"); */
			glBegin(GL_POLYGON);
			glNormal3f(0,-1,0);
			START_TRIG1
			for(i=0; i<d_div; i++) {
				if (!(i % 2))
				{
					glTexCoord2f(0.5+0.5*SIN1,0.5+0.5*COS1);
					glVertex3f(r*SIN1,(float)-h,r*COS1);
				}
				UP_TRIG1
			}
			glEnd();
		} else {
			/* printf ("Cone : NO bottom\n"); */
		} 

		if($f(side)) {
			double hreal = $f(height);
			double ml = sqrt(hreal*hreal + r * r);
			double mlh = h / ml;
			double mlr = r / ml;
			
			/*This code is a bug fix for the normals*/

			/* use atan(hreal/r) to get the angle for the bottom corner */
			double theta = atan(hreal / r);    

			/* calculate the vertical angle for the normal vector*/
                        double gamma = ( PI/2 ) - theta;  

			/* find the dimensions */
			double htwo  =  sin(gamma);
			double rtwo  =  cos(gamma);
                        
			glBegin(GL_TRIANGLES);
			START_TRIG1

			for(i=0; i<div; i++) {
				float lsin = SIN1;
				float lcos = COS1;

				/* perform an angular displacement */	
				UP_TRIG1;  

				/* place the top point and normal */
				glNormal3f(rtwo*SIN1, htwo, rtwo*COS1);
				glTexCoord2f(1.0-((i+0.5)/df), 1.0);
				glVertex3f(0.0, (float)h, 0.0);

				/* perform another angular displacement */
				UP_TRIG1;

				/* place the bottom points and normals */
				glNormal3f(rtwo*SIN1, htwo, rtwo*COS1);
				glTexCoord2f(1.0-((i+1.0)/df), 0.0);
				glVertex3f(r*SIN1, (float)-h, r*COS1);

				glNormal3f(rtwo*lsin, htwo, rtwo*lcos);
				glTexCoord2f(1.0-((float)i/df), 0.0);
				glVertex3f(r*lsin, (float)-h, r*lcos);

			}
			glEnd();
		}
',
Sphere => 'int vdiv = vert_div;
		int hdiv = horiz_div;
	   	float vf = vert_div;
	   	float hf = horiz_div;
		int v; int h;
		float va1,va2,van,ha1,ha2,han;
		DECL_TRIG1
		DECL_TRIG2

		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape; 

		INIT_TRIG1(vdiv) 
		INIT_TRIG2(hdiv)
		glPushMatrix();
		glScalef($f(radius), $f(radius), $f(radius));
		glBegin(GL_QUAD_STRIP);
		START_TRIG1
		for(v=0; v<vdiv; v++) {
			float vsin1 = SIN1;
			float vcos1 = COS1, vsin2,vcos2;
			UP_TRIG1
			vsin2 = SIN1;
			vcos2 = COS1;
			START_TRIG2
			for(h=0; h<=hdiv; h++) {
				float hsin1 = SIN2;
				float hcos1 = COS2;
				UP_TRIG2
/* 
 * Round a tex coord just barely greater than 1 to 1 : Since we are 
 * counting modulo 1, we do not want 1 to become zero spuriously.
 */
/* We really want something more rigorous here */
#define MY_EPS 0.000001
#define MOD_1(x) ( (x)<=1 ? (x) : ((x)<=(1.0+MY_EPS))? 1.0 : (x)-1.0 )

				glNormal3f(vsin2 * hcos1, vcos2, vsin2 * hsin1);
				glTexCoord2f(MOD_1(h / hf), 2.0 * ((v + 1.0) / vf));
				glVertex3f(vsin2 * hcos1, vcos2, vsin2 * hsin1);

				glNormal3f(vsin1 * hcos1, vcos1, vsin1 * hsin1); 
				glTexCoord2f(MOD_1(h / hf), 2.0 * (v/vf));
				glVertex3f(vsin1 * hcos1, vcos1, vsin1 * hsin1); 
#undef MOD_1
#undef MY_EPS
			}
		}
		glEnd();
		glPopMatrix();
					/* if(!$nomode) {
						glPopAttrib();
					} */
',

IndexedFaceSet => '
		struct SFColor *points; int npoints;
		struct SFColor *colors; int ncolors=0;
		struct SFColor *normals; int nnormals=0;
		struct SFVec2f *texcoords; int ntexcoords=0;

		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape; 

		/* get "coord", "color", "normal", "texCoord", "colorIndex" */
		$fv(coord, points, get3, &npoints);
		$fv_null(color, colors, get3, &ncolors);
		$fv_null(normal, normals, get3, &nnormals);
		$fv_null(texCoord, texcoords, get2, &ntexcoords);

		$mk_polyrep();
		if(!$f(solid)) {
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_CULL_FACE);

		}
		render_polyrep(this_, 
			npoints, points,
			ncolors, colors,
			nnormals, normals,
			ntexcoords, texcoords);
		if(!$f(solid)) {
			glPopAttrib();
		}
',

IndexedLineSet => '
		int i;
		int cin = $f_n(coordIndex);
		int colin = $f_n(colorIndex);
		int cpv = $f(colorPerVertex);
		int plno = 0;
		int ind1,ind2;
		int ind;
		int c;
		struct SFColor *points; int npoints;
		struct SFColor *colors; int ncolors=0;


		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape; 

		if(verbose) printf("Line: cin %d colin %d cpv %d\n",cin,colin,cpv);
		$fv(coord, points, get3, &npoints);
		$fv_null(color, colors, get3, &ncolors);
		glEnable(GL_COLOR_MATERIAL); 
                glPushAttrib(GL_ENABLE_BIT);
                glDisable(GL_CULL_FACE);




		if(ncolors && !cpv) {
			if (verbose) printf("glColor3f(%f,%f,%f);\n",
				  colors[plno].c[0],
				  colors[plno].c[1],
				  colors[plno].c[2]);
			glColor3f(colors[plno].c[0],
				  colors[plno].c[1],
				  colors[plno].c[2]);
		}

		glBegin(GL_LINE_STRIP);
		for(i=0; i<cin; i++) {
			ind = $f(coordIndex,i);
			if(verbose) printf("Line: %d %d\n",i,ind); 
			if(ind==-1) {
				glEnd();
				plno++;
				if(ncolors && !cpv) {
					c = plno;
					if((!colin && plno < ncolors) ||
					   (colin && plno < colin)) {
						if(colin) {
							c = $f(colorIndex,c);
						}
					      glColor3f(colors[c].c[0],
					        colors[c].c[1],
					   	colors[c].c[2]);
					}
				}
				glBegin(GL_LINE_STRIP);
			} else {
				if(ncolors && cpv) {
					c = i;
					if(colin) {
						c = $f(colorIndex,c);
					}
					glColor3f(colors[c].c[0],
						  colors[c].c[1],
						  colors[c].c[2]);
				}
				glVertex3f(
					points[ind].c[0],
					points[ind].c[1],
					points[ind].c[2]
				);
			}
		}
		glEnd();
		glDisable(GL_COLOR_MATERIAL); 
                glPopAttrib();
',


PointSet => '
	int i; 
	struct SFColor *points; int npoints=0;
	struct SFColor *colors; int ncolors=0;

	/* for shape display list redrawing */
	this_->_myshape = last_visited_shape; 

	$fv(coord, points, get3, &npoints);
	$fv_null(color, colors, get3, &ncolors);
	if(ncolors && ncolors < npoints) {
		printf ("PointSet has less colors than points - removing color\n");
		ncolors = 0;
	}
	glDisable(GL_LIGHTING);
	glBegin(GL_POINTS);
	if(verbose) printf("PointSet: %d %d\n", npoints, ncolors);
	for(i=0; i<npoints; i++) {
		if(ncolors) {
			if(verbose) printf("Color: %f %f %f\n",
				  colors[i].c[0],
				  colors[i].c[1],
				  colors[i].c[2]);
			glColor3f(colors[i].c[0],
				  colors[i].c[1],
				  colors[i].c[2]);
		}
		glVertex3f(
			points[i].c[0],
			points[i].c[1],
			points[i].c[2]
		);
	}
	glEnd();
	glEnable(GL_LIGHTING);
',

ElevationGrid =>  '
		struct SFColor *colors; int ncolors=0;
                struct SFVec2f *texcoords; int ntexcoords=0;
		struct SFColor *normals; int nnormals=0;

		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape; 

		$fv_null(color, colors, get3, &ncolors);
		$fv_null(normal, normals, get3, &nnormals);
		$fv_null(texCoord, texcoords, get2, &ntexcoords);

		$mk_polyrep();
		if(!$f(solid)) {
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_CULL_FACE);
		}
		render_polyrep(this_, 
			0, NULL,
			ncolors, colors,
			nnormals, normals,
			ntexcoords, texcoords
		);
		if(!$f(solid)) {
			glPopAttrib();
		}
',

Extrusion => '

		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape; 

		$mk_polyrep();
		if(!$f(solid)) {
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_CULL_FACE);
		}
		render_polyrep(this_,0,NULL,0,NULL,0,NULL,0,NULL);
		if(!$f(solid)) {
			glPopAttrib();
		}
',

# FontStyle params handled in Text.
FontStyle => '',

# Text is a polyrep, as of freewrl 0.34
Text => '
		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape; 

		$mk_polyrep();

		/* always Text is visible from both sides */
                glPushAttrib(GL_ENABLE_BIT);
                glDisable(GL_CULL_FACE);

		render_polyrep(this_,0,NULL,0,NULL,0,NULL,0,NULL);

		glPopAttrib();
',

Material =>  '
		float m[4]; int i; 
		float dcol[4];
		float ecol[4];
		float scol[4];
		float shin;
		float amb;
		float trans;

		GLubyte quartertone[] = {
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22,
		      0x88, 0x88, 0x88, 0x88, 0x22, 0x22, 0x22, 0x22
		   };
		GLubyte halftone[] = {
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
		      0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55
		   };
		GLubyte threequartertone[] = {
		      0xF7, 0xF7, 0xF7, 0xF7, 0xBF, 0xBF, 0xBF, 0xBF,
		      0xFB, 0xFB, 0xFB, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF,
		      0xEF, 0xEF, 0xEF, 0xEF, 0xFE, 0xFE, 0xFE, 0xFE,
		      0xFD, 0xFD, 0xFD, 0xFD, 0xBF, 0xBF, 0xBF, 0xBF,
		      0xF7, 0xF7, 0xF7, 0xF7, 0xBF, 0xBF, 0xBF, 0xBF,
		      0xFB, 0xFB, 0xFB, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF,
		      0xEF, 0xEF, 0xEF, 0xEF, 0xFE, 0xFE, 0xFE, 0xFE,
		      0xFD, 0xFD, 0xFD, 0xFD, 0xBF, 0xBF, 0xBF, 0xBF,
		      0xF7, 0xF7, 0xF7, 0xF7, 0xBF, 0xBF, 0xBF, 0xBF,
		      0xFB, 0xFB, 0xFB, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF,
		      0xEF, 0xEF, 0xEF, 0xEF, 0xFE, 0xFE, 0xFE, 0xFE,
		      0xFD, 0xFD, 0xFD, 0xFD, 0xBF, 0xBF, 0xBF, 0xBF,
		      0xF7, 0xF7, 0xF7, 0xF7, 0xBF, 0xBF, 0xBF, 0xBF,
		      0xFB, 0xFB, 0xFB, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF,
		      0xEF, 0xEF, 0xEF, 0xEF, 0xFE, 0xFE, 0xFE, 0xFE,
		      0xFD, 0xFD, 0xFD, 0xFD, 0xBF, 0xBF, 0xBF, 0xBF
		   };

		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape; 

		/* We have to keep track of whether to reset diffuseColor if using
		   textures; no texture or greyscale, we use the diffuseColor, if
		   RGB we set diffuseColor to be grey */
		if (last_texture_depth >1) {
			dcol[0]=0.8, dcol[1]=0.8, dcol[2]=0.8;
		} else {
			for (i=0; i<3;i++){ dcol[i] = $f(diffuseColor,i); }
		}
		dcol[3] = 1.0;

		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dcol);

		amb = $f(ambientIntensity);

		for(i=0; i<3; i++) {
			dcol[i] *= amb;
		}
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, dcol);

		for (i=0; i<3;i++){ scol[i] = $f(specularColor,i); } scol[3] = 1.0;
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, scol);

		for (i=0; i<3;i++){ ecol[i] = $f(emissiveColor,i); } ecol[3] = 1.0;
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, ecol);

		glColor3f(ecol[0],ecol[1],ecol[2]);

		if (fabs($f(transparency)) > 0.01) {
			glEnable(GL_POLYGON_STIPPLE);
			if (fabs($f(transparency)) < 0.26) {
				glPolygonStipple (threequartertone);
			} else if (fabs($f(transparency)) < 0.51) {
				glPolygonStipple (halftone);
			} else { glPolygonStipple (quartertone);}
		}

		if(fabs($f(shininess) - 0.2) > 0.001) {
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 
				128 * $f(shininess));
		}
',

TextureTransform => '


		/* for shape display list redrawing */
	this_->_myshape = last_visited_shape; 

       	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();
	glTranslatef($f(translation,0), $f(translation,1), 0);
	glTranslatef(-$f(center,0),-$f(center,1), 0);
	glRotatef($f(rotation)/3.1415926536*180,0,0,1);
	glScalef($f(scale,0),$f(scale,1),1);
	glTranslatef($f(center,0),$f(center,1), 0);
	glMatrixMode(GL_MODELVIEW);
',

ImageTexture => '
	unsigned char *filename = SvPV((this_->__data),PL_na);
	
	/* for shape display list redrawing */
	this_->_myshape = last_visited_shape; 

	/* save the reference globally */
	last_bound_texture = this_->__texture;
	
	/* and, bind to the texture */
	bind_image (filename,this_->__texture,this_->repeatS,this_->repeatT,this_->__istemporary);
',

PixelTexture => '

	unsigned char *ptr = SvPV((this_->__data),PL_na);

		/* for shape display list redrawing */
	this_->_myshape = last_visited_shape; 

	/* save the reference globally */
	last_bound_texture = this_->__texture;

	glBindTexture (GL_TEXTURE_2D, this_->__texture);
	do_texture ((this_->__depth), (this_->__x), (this_->__y), ptr,
		((this_->repeatS)) ? GL_REPEAT : GL_CLAMP, 
		((this_->repeatT)) ? GL_REPEAT : GL_CLAMP,
		GL_NEAREST);
',


MovieTexture => '
	unsigned char *ptr;

	int temp;

	/* really simple, the texture number is calculated, then simply sent here.
	   The last_bound_texture field is sent, and, made current */

	last_bound_texture = this_->__ctex;
	this_->_myshape = last_visited_shape; 

',


Fog => '
	/* Fog node... */

	GLdouble mod[16];
	GLdouble proj[16];
	GLdouble unit[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	GLdouble x,y,z;
	GLdouble x1,y1,z1;
	GLdouble sx, sy, sz;
	int frtlen;
	GLfloat fog_colour [4];

	if(!((this_->isBound))) {return;}
	if ($f(visibilityRange) <= 0.00001) return;

	fog_colour[0] = $f(color,0);
	fog_colour[1] = $f(color,1);
	fog_colour[2] = $f(color,2);
	fog_colour[3] = 1.0;

	glPushMatrix();
	glGetDoublev(GL_MODELVIEW_MATRIX, mod);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	/* Get origin */
	gluUnProject(0,0,0,mod,proj,viewport,&x,&y,&z);
	glTranslatef(x,y,z);

	gluUnProject(0,0,0,mod,unit,viewport,&x,&y,&z);
	/* Get scale */
	gluProject(x+1,y,z,mod,unit,viewport,&x1,&y1,&z1);
	sx = 1/sqrt( x1*x1 + y1*y1 + z1*z1*4 );
	gluProject(x,y+1,z,mod,unit,viewport,&x1,&y1,&z1);
	sy = 1/sqrt( x1*x1 + y1*y1 + z1*z1*4 );
	gluProject(x,y,z+1,mod,unit,viewport,&x1,&y1,&z1);
	sz = 1/sqrt( x1*x1 + y1*y1 + z1*z1*4 );
	/* Undo the translation and scale effects */
	glScalef(sx,sy,sz);


	/* now do the foggy stuff */
	glFogfv(GL_FOG_COLOR,fog_colour);
	glFogf(GL_FOG_END,$f(visibilityRange));
	if (strcmp("LINEAR",SvPV((this_->fogType),frtlen))) {
		glFogi(GL_FOG_MODE, GL_EXP);
	} else {
		glFogi(GL_FOG_MODE, GL_LINEAR);
	}
	glEnable (GL_FOG);

	glPopMatrix();
 ',


# Sound ... Nothing here
Sound => ' ',

# AudioClip .... Nothing here
AudioClip => ' ',

# CylinderSensor .... Nothing here
CylinderCensor => ' ',


# GLBackend is using 200000 as distance - we use 100000 for background
# XXX Should just make depth test always fail.
Background => '
	GLdouble mod[16];
	GLdouble proj[16];
	GLdouble unit[16] = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
	struct pt vec[4]; struct pt vec2[4]; struct pt vec3[4];
	int i,j; int ind=0;
	GLdouble x,y,z;
	GLdouble x1,y1,z1;
	GLdouble sx, sy, sz;
	struct SFColor *c1,*c2;
	int hdiv = horiz_div;
	int h,v;
	double va1, va2, ha1, ha2;	/* JS - vert and horiz angles 	*/
	double vatemp;		
	GLuint mask;
	GLfloat bk_emis[4];		/* background emissive colour	*/
	float	sc;


	/* Background Texture Objects.... */
	static int bcklen,frtlen,rtlen,lftlen,toplen,botlen;
	unsigned char *bckptr,*frtptr,*rtptr,*lftptr,*topptr,*botptr;

	static unsigned int displayed_node = 0;
	static int background_display_list = -1;
	static GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
	static GLfloat light_diffuse[] = {0.0, 0.0, 0.0, 1.0};
	static GLfloat light_specular[] = {0.0, 0.0, 0.0, 1.0};
	static GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

	if(!((this_->isBound))) {return;}
	// if (glIsEnabled(GL_FOG)) {return;} //no need to do backgrounds then


	bk_emis[3]=0.0; /* always zero for backgrounds */

	/* Cannot start_list() because of moving center, so we do our own list later */

	glPushAttrib(GL_LIGHTING_BIT|GL_ENABLE_BIT|GL_TEXTURE_BIT);
	glShadeModel(GL_SMOOTH);
	glPushMatrix();
	glGetDoublev(GL_MODELVIEW_MATRIX, mod);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	/* Get origin */
	gluUnProject(0,0,0,mod,proj,viewport,&x,&y,&z);
	glTranslatef(x,y,z);

	glDisable (GL_LIGHTING);

	gluUnProject(0,0,0,mod,unit,viewport,&x,&y,&z);
	/* Get scale */
	gluProject(x+1,y,z,mod,unit,viewport,&x1,&y1,&z1);
	sx = 1/sqrt( x1*x1 + y1*y1 + z1*z1*4 );
	gluProject(x,y+1,z,mod,unit,viewport,&x1,&y1,&z1);
	sy = 1/sqrt( x1*x1 + y1*y1 + z1*z1*4 );
	gluProject(x,y,z+1,mod,unit,viewport,&x1,&y1,&z1);
	sz = 1/sqrt( x1*x1 + y1*y1 + z1*z1*4 );

	/* Undo the translation and scale effects */
	glScalef(sx,sy,sz);


	/* now, is this the same background as before??? */
	if(displayed_node==(unsigned int) nod_) {
		glCallList(background_display_list);
		glPopMatrix();
		glPopAttrib();
		return;
	}

	/* if not, then generate a display list */
	displayed_node = (unsigned int) nod_;
	if (background_display_list != -1) {
		/* delete old list, to make room for new one */
		/* printf ("deleting old background display list %d\n",background_display_list); */
		glDeleteLists(background_display_list,1);
	}
	background_display_list = glGenLists(1);

	/* do we have any background textures?  */
	frtptr = SvPV((this_->__datafront),frtlen); 
	bckptr = SvPV((this_->__databack),bcklen);
	topptr = SvPV((this_->__datatop),toplen);
	botptr = SvPV((this_->__databottom),botlen);
	lftptr = SvPV((this_->__dataleft),lftlen);
	rtptr = SvPV((this_->__dataright),rtlen);

	/*printf ("background textures; lengths %d %d %d %d %d %d\n",
		frtlen,bcklen,toplen,botlen,lftlen,rtlen);
	printf ("backgrouns textures; names %s %s %s %s %s %s\n",
		frtptr,bckptr,topptr,botptr,lftptr,rtptr); */


	/* printf ("new background display list is %d\n",background_display_list); */
	glNewList(background_display_list,GL_COMPILE_AND_EXECUTE);


	if(verbose)  printf("TS: %f %f %f,      %f %f %f\n",x,y,z,sx,sy,sz);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);


	sc = 2000.0; /* where to put the sky quads */
	glBegin(GL_QUADS);
	if(((this_->skyColor).n) == 1) {
		c1 = &(((this_->skyColor).p[0]));
		va1 = 0;
		va2 = PI/2; 
		bk_emis[0]=c1->c[0]; bk_emis[1]=c1->c[1]; bk_emis[2]=c1->c[2];
		glMaterialfv(GL_FRONT,GL_EMISSION, bk_emis);
		glColor3f(c1->c[0], c1->c[1], c1->c[2]);

		for(v=0; v<2; v++) {
			for(h=0; h<hdiv; h++) {
				ha1 = h * PI*2 / hdiv;
				ha2 = (h+1) * PI*2 / hdiv;

				glVertex3f(sin(va2)*sc * cos(ha1), cos(va2)*sc, sin(va2) * sin(ha1)*sc);
				glVertex3f(sin(va2)*sc * cos(ha2), cos(va2)*sc, sin(va2) * sin(ha2)*sc);
				glVertex3f(sin(va1)*sc * cos(ha2), cos(va1)*sc, sin(va1) * sin(ha2)*sc);
				glVertex3f(sin(va1)*sc * cos(ha1), cos(va1)*sc, sin(va1) * sin(ha1)*sc);
			}
			va1 = va2;
			va2 = PI;
		}
	} else {
		va1 = 0;
		for(v=0; v<((this_->skyColor).n)-1; v++) {
			c1 = &(((this_->skyColor).p[v]));
			c2 = &(((this_->skyColor).p[v+1]));
			va2 = ((this_->skyAngle).p[v]);
			
			for(h=0; h<hdiv; h++) {
				ha1 = h * PI*2 / hdiv;
				ha2 = (h+1) * PI*2 / hdiv;

				bk_emis[0]=c2->c[0]; bk_emis[1]=c2->c[1]; bk_emis[2]=c2->c[2];
				glMaterialfv(GL_FRONT,GL_EMISSION, bk_emis);
				glColor3f(c2->c[0], c2->c[1], c2->c[2]);
				glVertex3f(sin(va2) * cos(ha1)*sc, cos(va2)*sc, sin(va2) * sin(ha1)*sc);
				glVertex3f(sin(va2) * cos(ha2)*sc, cos(va2)*sc, sin(va2) * sin(ha2)*sc);
				bk_emis[0]=c1->c[0]; bk_emis[1]=c1->c[1]; bk_emis[2]=c1->c[2];
				glMaterialfv(GL_FRONT,GL_EMISSION, bk_emis);
				glColor3f(c1->c[0], c1->c[1], c1->c[2]);
				glVertex3f(sin(va1) * cos(ha2)*sc, cos(va1)*sc, sin(va1) * sin(ha2)*sc);
				glVertex3f(sin(va1) * cos(ha1)*sc, cos(va1)*sc, sin(va1) * sin(ha1)*sc);
			}
			va1 = va2;
		}

		/* now, the spec states: "If the last skyAngle is less than pi, then the
		  colour band between the last skyAngle and the nadir is clamped to the last skyColor." */
		if (va2 < (PI-0.01)) {
			bk_emis[0]=c2->c[0]; bk_emis[1]=c2->c[1]; bk_emis[2]=c2->c[2];
			glMaterialfv(GL_FRONT,GL_EMISSION, bk_emis);
			glColor3f(c2->c[0], c2->c[1], c2->c[2]);
			for(h=0; h<hdiv; h++) {
				ha1 = h * PI*2 / hdiv;
				ha2 = (h+1) * PI*2 / hdiv;
	
				glVertex3f(sin(PI) * cos(ha1)*sc, cos(PI)*sc, sin(PI) * sin(ha1)*sc);
				glVertex3f(sin(PI) * cos(ha2)*sc, cos(PI)*sc, sin(PI) * sin(ha2)*sc);
				glVertex3f(sin(va2) * cos(ha2)*sc, cos(va2)*sc, sin(va2) * sin(ha2)*sc);
				glVertex3f(sin(va2) * cos(ha1)*sc, cos(va2)*sc, sin(va2) * sin(ha1)*sc);
			}
		}
	}
	glEnd();


	/* Do the ground, if there is anything  to do. */
	if ((this_->groundColor).n>0) {
		sc = 1250.0; /* where to put the ground quads */
		glBegin(GL_QUADS);
		if(((this_->groundColor).n) == 1) {
			c1 = &(((this_->groundColor).p[0]));
			bk_emis[0]=c1->c[0]; bk_emis[1]=c1->c[1]; bk_emis[2]=c1->c[2];
			glMaterialfv(GL_FRONT,GL_EMISSION, bk_emis);
			glColor3f(c1->c[0], c1->c[1], c1->c[2]);
			for(h=0; h<hdiv; h++) {
				ha1 = h * PI*2 / hdiv;
				ha2 = (h+1) * PI*2 / hdiv;

				glVertex3f(sin(PI) * cos(ha1)*sc, cos(PI)*sc, sin(PI) * sin(ha1)*sc);
				glVertex3f(sin(PI) * cos(ha2)*sc, cos(PI)*sc, sin(PI) * sin(ha2)*sc);
				glVertex3f(sin(PI/2) * cos(ha2)*sc, cos(PI/2)*sc, sin(PI/2) * sin(ha2)*sc);
				glVertex3f(sin(PI/2) * cos(ha1)*sc, cos(PI/2)*sc, sin(PI/2) * sin(ha1)*sc);
			}
		} else {
			va1 = PI;
			for(v=0; v<((this_->groundColor).n)-1; v++) {
				c1 = &(((this_->groundColor).p[v]));
				c2 = &(((this_->groundColor).p[v+1]));
				va2 = PI - ((this_->groundAngle).p[v]);
		
				for(h=0; h<hdiv; h++) {
					ha1 = h * PI*2 / hdiv;
					ha2 = (h+1) * PI*2 / hdiv;

					bk_emis[0]=c1->c[0]; bk_emis[1]=c1->c[1]; bk_emis[2]=c1->c[2];
					glMaterialfv(GL_FRONT,GL_EMISSION, bk_emis);
					glColor3f(c1->c[0], c1->c[1], c1->c[2]);
					glVertex3f(sin(va1) * cos(ha1)*sc, cos(va1)*sc, sin(va1) * sin(ha1)*sc);
					glVertex3f(sin(va1) * cos(ha2)*sc, cos(va1)*sc, sin(va1) * sin(ha2)*sc);

					bk_emis[0]=c2->c[0]; bk_emis[1]=c2->c[1]; bk_emis[2]=c2->c[2];
					glMaterialfv(GL_FRONT,GL_EMISSION, bk_emis);
					glColor3f(c2->c[0], c2->c[1], c2->c[2]);
					glVertex3f(sin(va2) * cos(ha2)*sc, cos(va2)*sc, sin(va2) * sin(ha2)*sc);
					glVertex3f(sin(va2) * cos(ha1)*sc, cos(va2)*sc, sin(va2) * sin(ha1)*sc);
				}
				va1 = va2;
			}
		}
		glEnd();
	}




	/* now, for the textures, if they exist */

	if ((this_->__textureback>0) || 
			(this_->__texturefront>0) || 
			(this_->__textureleft>0) || 
			(this_->__textureright>0) || 
			(this_->__texturetop>0) || 
			(this_->__texturebottom>0)) {
        	GLfloat mat_emission[] = {1.0,1.0,1.0,1.0};
       	 	GLfloat col_amb[] = {1.0, 1.0, 1.0, 1.0};
       	 	GLfloat col_dif[] = {1.0, 1.0, 1.0, 1.0};

        	glEnable (GL_LIGHTING);
        	glEnable(GL_TEXTURE_2D);
        	glColor3f(1,1,1);

		sc = 500.0; /* where to put the tex vertexes */

        	glMaterialfv(GL_FRONT,GL_EMISSION, mat_emission);
        	glLightfv (GL_LIGHT0, GL_AMBIENT, col_amb);
        	glLightfv (GL_LIGHT0, GL_DIFFUSE, col_dif);

		/* go through each of the 6 possible sides */

		if(this_->__textureback>0) {
			bind_image (bckptr,this_->__textureback, 0,0,this_->__istemporaryback);
			glBegin(GL_QUADS);
			glNormal3f(0,0,1); 
			glTexCoord2f(1, 0); glVertex3f(-sc, -sc, sc);
			glTexCoord2f(1, 1); glVertex3f(-sc, sc, sc);
			glTexCoord2f(0, 1); glVertex3f(sc, sc, sc);
			glTexCoord2f(0, 0); glVertex3f(sc, -sc, sc);
			glEnd();
		};

		if(this_->__texturefront>0) {
			bind_image (frtptr,this_->__texturefront, 0,0,this_->__istemporaryfront);
			glBegin(GL_QUADS);
			glNormal3f(0,0,-1);
			glTexCoord2f(1,1); glVertex3f(sc,sc,-sc);
			glTexCoord2f(0,1); glVertex3f(-sc,sc,-sc);
			glTexCoord2f(0,0); glVertex3f(-sc,-sc,-sc);
			glTexCoord2f(1,0); glVertex3f(sc,-sc,-sc); 
			glEnd();
		};

		if(this_->__texturetop>0) {
			bind_image (topptr,this_->__texturetop, 0,0,this_->__istemporarytop);
			glBegin(GL_QUADS);
			glNormal3f(0,1,0);
			glTexCoord2f(1,1); glVertex3f(sc,sc,sc);
			glTexCoord2f(0,1); glVertex3f(-sc,sc,sc);
			glTexCoord2f(0,0); glVertex3f(-sc,sc,-sc);
			glTexCoord2f(1,0); glVertex3f(sc,sc,-sc);
			glEnd();
		};

		if(this_->__texturebottom>0) {
			bind_image (botptr,this_->__texturebottom, 0,0,this_->__istemporarybottom);
			glBegin(GL_QUADS);
			glNormal3f(0,-(1),0);
			glTexCoord2f(1,1); glVertex3f(sc,-sc,-sc);
			glTexCoord2f(0,1); glVertex3f(-sc,-sc,-sc);
			glTexCoord2f(0,0); glVertex3f(-sc,-sc,sc);
			glTexCoord2f(1,0); glVertex3f(sc,-sc,sc);
			glEnd();
		};

		if(this_->__textureright>0) {
			bind_image (rtptr,this_->__textureright, 0,0,this_->__istemporaryright);
			glBegin(GL_QUADS);
			glNormal3f(1,0,0);
			glTexCoord2f(1,1); glVertex3f(sc,sc,sc);
			glTexCoord2f(0,1); glVertex3f(sc,sc,-sc);
			glTexCoord2f(0,0); glVertex3f(sc,-sc,-sc);
			glTexCoord2f(1,0); glVertex3f(sc,-sc,sc);
			glEnd();
		};

		if(this_->__textureleft>0) {
			bind_image (lftptr,this_->__textureleft, 0,0,this_->__istemporaryleft);
			glBegin(GL_QUADS);
			glNormal3f(-1,0,0);
			glTexCoord2f(1,1); glVertex3f(-sc,sc, -sc);
			glTexCoord2f(0,1); glVertex3f(-sc,sc,  sc); 
			glTexCoord2f(0,0); glVertex3f(-sc,-sc, sc);
			glTexCoord2f(1,0); glVertex3f(-sc,-sc,-sc);
			glEnd();
		 };
	}

	/* end of textures... */
	glEndList();
	glPopMatrix();
	glPopAttrib();
',


DirectionalLight => '
	/* NOTE: This is called by the Group Children code
	 * at the correct point (in the beginning of the rendering
	 * of the children. We just turn the light on right now.
	 */
	if($f(on)) {
		int light = nextlight();
		if(light >= 0) {
			float vec[4];
			glEnable(light);
			vec[0] = -$f(direction,0);
			vec[1] = -$f(direction,1);
			vec[2] = -$f(direction,2);
			vec[3] = 0;
			glLightfv(light, GL_POSITION, vec);
			vec[0] = $f(color,0) * $f(intensity);
			vec[1] = $f(color,1) * $f(intensity);
			vec[2] = $f(color,2) * $f(intensity);
			vec[3] = 1;
			glLightfv(light, GL_DIFFUSE, vec);
			glLightfv(light, GL_SPECULAR, vec);

			/* Aubrey Jaffer */
			vec[0] = $f(color,0) * $f(ambientIntensity);
			vec[1] = $f(color,1) * $f(ambientIntensity);
			vec[2] = $f(color,2) * $f(ambientIntensity);

			glLightfv(light, GL_AMBIENT, vec);
		}
	}
',

);

#######################################################################
#######################################################################
#######################################################################
#
# Prep --
#  Prepare for rendering a node - e.g. for transforms, do the transform
#  but not the children.
#
#

%PrepC = (
Transform => (join '','
	
        /* rendering the viewpoint means doing the inverse transformations in reverse order (while poping stack),
         * so we do nothing here in that case -ncoder */
	if(!render_vp) {
                glPushMatrix();
    
		glTranslatef(',(join ',',map {getf(Transform,translation,$_)} 0..2),'
		);
		glTranslatef(',(join ',',map {getf(Transform,center,$_)} 0..2),'
		);
		glRotatef(',getf(Transform,rotation,3),'/3.1415926536*180,',
			(join ',',map {getf(Transform,rotation,$_)} 0..2),'
		);
		glRotatef(',getf(Transform,scaleOrientation,3),'/3.1415926536*180,',
			(join ',',map {getf(Transform,scaleOrientation,$_)} 0..2),'
		);
		glScalef(',(join ',',map {getf(Transform,scale,$_)} 0..2),'
		);
		glRotatef(-(',getf(Transform,scaleOrientation,3),'/3.1415926536*180),',
			(join ',',map {getf(Transform,scaleOrientation,$_)} 0..2),'
		);
		glTranslatef(',(join ',',map {"-(".getf(Transform,center,$_).")"} 0..2),'
		);
        } 
	/*
	$endlist();
	*/
'),

##JAS Collision=> ('
##JAS printf ("Start of Collision - dont think this is needed...JAS \n");
##JAS 
##JAS 	glPushMatrix();
##JAS 	$start_list();
##JAS 	$end_list();
##JAS '),

# Simplistic...
Billboard => '
	GLdouble mod[16];
	GLdouble proj[16];
	struct pt vec, ax, cp, z = {0,0,1}, cp2,cp3, arcp;
	int align;
	double len; double len2;
	double angle;
	int sign;
	ax.x = $f(axisOfRotation,0);
	ax.y = $f(axisOfRotation,1);
	ax.z = $f(axisOfRotation,2);
	align = (APPROX(VECSQ(ax),0));
	glPushMatrix();

	glGetDoublev(GL_MODELVIEW_MATRIX, mod);
	glGetDoublev(GL_PROJECTION_MATRIX, proj);
	gluUnProject(0,0,0,mod,proj,viewport,
		&vec.x,&vec.y,&vec.z);
	len = VECSQ(vec); if(APPROX(len,0)) {return;}
	VECSCALE(vec,1/sqrt(len));
	/* printf("Billboard: (%f %f %f) (%f %f %f)\n",vec.x,vec.y,vec.z,	
		ax.x, ax.y, ax.z); */
	if(!align) {
		VECCP(ax,z,arcp);
		VECCP(ax,arcp,cp3);
		len = VECSQ(ax); VECSCALE(ax,1/sqrt(len));
		VECCP(vec,ax,cp); /* cp is now 90deg to both vector and axis */
		len = sqrt(VECSQ(cp)); 
		if(APPROX(len,0)) {return;} /* Cant do a thing */
		VECSCALE(cp, 1/len)
		/* printf("Billboard: (%f %f %f) (%f %f %f)\n",cp.x,cp.y,cp.z,	
			cp3.x, cp3.y, cp3.z); */
		/* Now, find out angle between this and z axis */
		VECCP(cp,z,cp2);
		len2 = VECPT(cp,z); /* cos(angle) */
		len = sqrt(VECSQ(cp2)); /* this is abs(sin(angle)) */
		/* Now we need to find the sign first */
		if(VECPT(cp, arcp)>0) sign=-1; else sign=1;
		angle = atan2(len2,sign*len);
		/* printf("Billboard: sin angle = %f, cos angle = %f\n, sign: %d,
			atan2: %f\n", len, len2,sign,angle); */
		glRotatef(angle/3.1415926536*180, ax.x,ax.y,ax.z);
	} else {
		/* cp is the axis of the first rotation... */
		VECCP(z,vec,cp); len = sqrt(VECSQ(cp)); 
		VECSCALE(cp,1/len);
		VECCP(z,cp,cp2); 
		angle = asin(VECPT(cp2,vec));
		glRotatef(angle/3.1415926536*180, ax.x,ax.y,ax.z);
		
		/* XXXX */
		/* die("Cant do 0 0 0 billboard"); */

	}
',


Viewpoint => (join '','
	if(render_vp) {
		GLint vp[10];
		double a1;
		double angle;
		if(verbose) printf("Viewpoint: %d IB: %d..\n", 
			this_,$f(isBound));
		if(!$f(isBound)) {return;}

		/* stop rendering when we hit A viewpoint or THE viewpoint???
		   shouldnt we check for what_vp???? 
                   maybe only one viewpoint is in the tree at a time? -  ncoder*/

		found_vp = 1; /* We found the viewpoint */

		/* These have to be in this order because the viewpoint
		 * rotates in its place */
		glRotatef(-(',getf(Viewpoint,orientation,3),')/3.1415926536*180,',
			(join ',',map {getf(Viewpoint,orientation,$_)} 0..2),'
		);
		glTranslatef(',(join ',',map {"-(".getf(Viewpoint,position,$_).")"} 
			0..2),'
		);

		if (verbose) { 
		printf ("Rotation %f %f %f %f\n",
		-(',getf(Viewpoint,orientation,3),')/3.1415926536*180,',
			(join ',',map {getf(Viewpoint,orientation,$_)} 0..2),'
		);

		printf ("Translation %f %f %f\n",
		',(join ',',map {"-(".getf(Viewpoint,position,$_).")"} 
			0..2),'
		);
		}



		if(verbose) {
			glGetIntegerv(GL_VIEWPORT, vp);
			if(vp[2] > vp[3]) {
				a1=0;
				angle = $f(fieldOfView)/3.1415926536*180;
			} else {
				a1 = $f(fieldOfView);
				a1 = atan2(sin(a1),vp[2]/((float)vp[3]) * cos(a1));
				angle = a1/3.1415926536*180;
			}
			printf("Vp: %d %d %d %d %f %f\n", vp[0], vp[1], vp[2], vp[3],
				a1, angle);
		}

	}
'),

NavigationInfo => '
        if(verbose) printf("NavigationInfo: %d IB: %d..\n",
                this_,$f(isBound));
        if(!$f(isBound)) {return;}
        /* I have no idea what else should go in here -- John Breen */
',

);

#######################################################################
#######################################################################
#######################################################################
#
# Fin --
#  Finish the rendering i.e. restore matrices and whatever to the
#  original state.
#
#

# Finish rendering
%FinC = (
##JAS Collision => (join '','
##JAS 	glPopMatrix();
##JAS '),
Transform => (join '','
        
	if(!render_vp) {
            glPopMatrix();
	} else {
           /*Rendering the viewpoint only means finding it, and calculating the reverse WorldView matrix.*/
            if(found_vp) {
		glTranslatef(',(join ',',map {getf(Transform,center,$_)} 0..2),'
		);
		glRotatef(',getf(Transform,scaleOrientation,3),'/3.1415926536*180,',
			(join ',',map {getf(Transform,scaleOrientation,$_)} 0..2),'
		);
		glScalef(',(join ',',map {"1.0/(".getf(Transform,scale,$_).")"} 0..2),'
		);
		glRotatef(-(',getf(Transform,scaleOrientation,3),'/3.1415926536*180),',
			(join ',',map {getf(Transform,scaleOrientation,$_)} 0..2),'
		);
		glRotatef(-(',getf(Transform,rotation,3),')/3.1415926536*180,',
			(join ',',map {getf(Transform,rotation,$_)} 0..2),'
		);
		glTranslatef(',(join ',',map {"-(".getf(Transform,center,$_).")"} 0..2),'
		);
		glTranslatef(',(join ',',map {"-(".getf(Transform,translation,$_).")"} 
			0..2),'
		);
            }
        }

'),
Billboard => (join '','
	glPopMatrix();
'),
);

#######################################################################
#######################################################################
#######################################################################
#
# Child --
#  Render the actual children of the node.
#
#

# Render children (real child nodes, not e.g. appearance/geometry)
%ChildC = (
	Group => '
		int nc = $f_n(children); 
		int i;
		int savedlight = curlight;
		struct VRML_Virt virt_DirectionalLight;

		if(verbose) {printf("RENDER GROUP START %d (%d)\n",this_, nc);}
		if($i(has_light)) {
			glPushAttrib(GL_LIGHTING_BIT|GL_ENABLE_BIT);
			for(i=0; i<nc; i++) {
				void *p = $f(children,i);
				if($ntyptest(p,DirectionalLight)) {
					render_node(p);
				}
			}
		}
		for(i=0; i<nc; i++) {
			void *p = $f(children,i);
			if(verbose) {printf("RENDER GROUP %d CHILD %d\n",this_, p);}
			/* Hmm - how much time does this consume? */
			/* Not that much. */
			if(!$i(has_light) || !$ntyptest(p,DirectionalLight)) {
				render_node(p);
			}
		}
		if($i(has_light)) {
			glPopAttrib();
		}
		if(verbose) {printf("RENDER GROUP END %d\n",this_);}

		curlight = savedlight;
	',
	Switch => '
		int wc = $f(whichChoice);
		if(wc >= 0 && wc < $f_n(choice)) {
			void *p = $f(choice,wc);
			render_node(p);
		}
	',
	LOD => '
		GLdouble mod[16];
		GLdouble proj[16];
		struct pt vec;
		double dist;
		int nran = $f_n(range);
		int nnod = $f_n(level);
		int i;
		void *p;

		if(!nran) {
			void *p = $f(level, 0);
			render_node(p);
			return;
		}

		glGetDoublev(GL_MODELVIEW_MATRIX, mod);
		glGetDoublev(GL_PROJECTION_MATRIX, proj);
		gluUnProject(0,0,0,mod,proj,viewport,
			&vec.x,&vec.y,&vec.z);
		vec.x -= $f(center,0);
		vec.y -= $f(center,1);
		vec.z -= $f(center,2);
		dist = sqrt(VECSQ(vec));
		i = 0;

		while (i<nran) {
			if(dist < $f(range,i)) {
				break;
			}
			i++;
		}
		if(i >= nnod) {i = nnod-1;}

		p = $f(level,i);
		render_node(p);

	',
	Appearance => '
		/* for shape display list redrawing */
		this_->_myshape = last_visited_shape; 


		if($f(texture)) {

			/* is there a TextureTransform? if no texture, forget about it */
		    	if($f(textureTransform))   {
				render_node($f(textureTransform));
			    } else {
				glMatrixMode(GL_TEXTURE);
				glLoadIdentity();
				glTranslatef(0, 0, 0);
				glRotatef(0,0,0,1);
				glScalef(1,1,1);
				glMatrixMode(GL_MODELVIEW);
		    	}
			render_node($f(texture));
		} else {
			last_texture_depth = 0;
		}


		/* if we have a material, do it. last_texture_depth is used to select diffuseColor */
		if($f(material)) {
			render_node($f(material));
		} else {
			/* no material, so just colour the following shape */
                       	/* Spec says to disable lighting and set coloUr to 1,1,1 */
                       	glDisable (GL_LIGHTING);
			glColor3f(1.0,1.0,1.0);
		} 
	',
	Shape => '
		GLenum glError;


		if(!(this_->geometry)) { return; }

		if(render_collision) {
			/* only need to forward the call to the child */
			render_node((this_->geometry));
			return; 
		}

		/* Display lists used here. The name of the game is to use the
		display list for everything, except for sensitive nodes. */

		/* Appearance, Material, Shape, will make a new list, via this pointer */
		last_visited_shape = this_;

		/* a texture flag... */
		last_bound_texture = 0;
	

		glPushAttrib(GL_LIGHTING_BIT|GL_ENABLE_BIT|GL_TEXTURE_BIT);
		/* if we are rendering the geometry, see if we have a disp. list */
		if ((render_geom) && (!render_sensitive)) { 

			if(this_->_dlist) {
				if(this_->_dlchange == this_->_change) {
					glCallList(this_->_dlist); 
					glPopAttrib();
					return;
				} else {
					glDeleteLists(this_->_dlist,1);
				}
			}
			this_->_dlist = glGenLists(1);
			this_->_dlchange = this_->_change;
			glNewList(this_->_dlist,GL_COMPILE_AND_EXECUTE);

			/* is there an associated appearance node? */	
        	        if($f(appearance)) {
	                        render_node($f(appearance));
        	        } else {
				if (render_geom) {
	                            /* no material, so just colour the following shape */
                        	    /* Spec says to disable lighting and set coloUr to 1,1,1 */
                        	    glDisable (GL_LIGHTING);
        	                    glColor3f(1.0,1.0,1.0);
				}
	                }
			if (last_bound_texture != 0) {
				/* we had a texture */
				glEnable (GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D,last_bound_texture);
			}
		}

		/* Now, do the geometry */
		render_node((this_->geometry));

		
		if ((render_geom) && (!render_sensitive)) {
			if (this_->_dlchange == this_->_change) {
				/* premature ending of dlists */
				glEndList();
			}

			/* JAS
			glError = glGetError();
			while (glError != GL_NO_ERROR) {
				printf ("VRMLRend.pm::Shape:glError: %s\n",gluErrorString(glError));
				glError = glGetError();
			}
			*/
		}
		last_visited_shape = 0;
		glPopAttrib();
	',
	Collision => '
		int nc = $f_n(children); 
		int i;
		if(render_collision) {
			if($f(collide) && !$f(proxy)) {
				struct pt OldCollisionOffset = CollisionOffset;
				for(i=0; i<nc; i++) {
					void *p = $f(children,i);
					if(verbose) {printf("RENDER GROUP %d CHILD %d\n",this_, p);}
					render_node(p);
				}
				if(CollisionOffset.x != OldCollisionOffset.x ||
				   CollisionOffset.y != OldCollisionOffset.y ||
				   CollisionOffset.z != OldCollisionOffset.z) {
					/*collision occured
					 * bit 0 gives collision, bit 1 gives change */
					this_->__hit = (this_->__hit & 1) ? 1 : 3;
				} else
					this_->__hit = (this_->__hit & 1) ? 2 : 0;

			}
        	        if($f(proxy)) 
	                        render_node($f(proxy));

		} else { /*standard group behaviour*/
			int savedlight = curlight;
			struct VRML_Virt virt_DirectionalLight;

			if(verbose) {printf("RENDER GROUP START %d (%d)\n",this_, nc);}
			if($i(has_light)) {
				glPushAttrib(GL_LIGHTING_BIT|GL_ENABLE_BIT);
				for(i=0; i<nc; i++) {
					void *p = $f(children,i);
					if($ntyptest(p,DirectionalLight)) {
						render_node(p);
					}
				}
			}
			for(i=0; i<nc; i++) {
				void *p = $f(children,i);
				if(verbose) {printf("RENDER GROUP %d CHILD %d\n",this_, p);}
				/* Hmm - how much time does this consume? */
				/* Not that much. */
				if(!$i(has_light) || !$ntyptest(p,DirectionalLight)) {
					render_node(p);
				}
			}
			if($i(has_light)) {
				glPopAttrib();
			}
			if(verbose) {printf("RENDER GROUP END %d\n",this_);}
	
			curlight = savedlight;
		}
	',
);

$ChildC{Transform} = $ChildC{Group};
$ChildC{Billboard} = $ChildC{Group};
$ChildC{Anchor} = $ChildC{Group};
#$ChildC{Collision} = $ChildC{Group};  ## JAS 

#######################################################################
#######################################################################
#######################################################################
#
# Light --
#  Render a light. XXX This needs work to be like the spec :(
#
#

# NO startlist -- nextlight() may change :(
%LightC = (
	PointLight => '
		if($f(on)) {
			int light = nextlight();
			if(light >= 0) {
				float vec[4];
				glEnable(light);
				vec[0] = $f(direction,0);
				vec[1] = $f(direction,1);
				vec[2] = $f(direction,2);
				vec[3] = 1;
				glLightfv(light, GL_SPOT_DIRECTION, vec);
				vec[0] = $f(location,0);
				vec[1] = $f(location,1);
				vec[2] = $f(location,2);
				vec[3] = 1;
				glLightfv(light, GL_POSITION, vec);

				glLightf(light, GL_CONSTANT_ATTENUATION, 
					$f(attenuation,0));
				glLightf(light, GL_LINEAR_ATTENUATION, 
					$f(attenuation,1));
				glLightf(light, GL_QUADRATIC_ATTENUATION, 
					$f(attenuation,2));


				vec[0] = $f(color,0) * $f(intensity);
				vec[1] = $f(color,1) * $f(intensity);
				vec[2] = $f(color,2) * $f(intensity);
				vec[3] = 1;
				glLightfv(light, GL_DIFFUSE, vec);
				glLightfv(light, GL_SPECULAR, vec);

				/* Aubrey Jaffer */
				vec[0] = $f(color,0) * $f(ambientIntensity);
				vec[1] = $f(color,1) * $f(ambientIntensity);
				vec[2] = $f(color,2) * $f(ambientIntensity);

				glLightfv(light, GL_AMBIENT, vec);

				/* XXX */
				glLightf(light, GL_SPOT_CUTOFF, 180);
			}
		}
	',
	SpotLight => '
		if($f(on)) {
			int light = nextlight();
			if(light >= 0) {
				float vec[4];
				glEnable(light);
				vec[0] = $f(direction,0);
				vec[1] = $f(direction,1);
				vec[2] = $f(direction,2);
				vec[3] = 1;
				glLightfv(light, GL_SPOT_DIRECTION, vec);
				vec[0] = $f(location,0);
				vec[1] = $f(location,1);
				vec[2] = $f(location,2);
				vec[3] = 1;
				glLightfv(light, GL_POSITION, vec);

				glLightf(light, GL_CONSTANT_ATTENUATION, 
					$f(attenuation,0));
				glLightf(light, GL_LINEAR_ATTENUATION, 
					$f(attenuation,1));
				glLightf(light, GL_QUADRATIC_ATTENUATION, 
					$f(attenuation,2));


				vec[0] = $f(color,0) * $f(intensity);
				vec[1] = $f(color,1) * $f(intensity);
				vec[2] = $f(color,2) * $f(intensity);
				vec[3] = 1;
				glLightfv(light, GL_DIFFUSE, vec);
				glLightfv(light, GL_SPECULAR, vec);

				/* Aubrey Jaffer */
				vec[0] = $f(color,0) * $f(ambientIntensity);
				vec[1] = $f(color,1) * $f(ambientIntensity);
				vec[2] = $f(color,2) * $f(ambientIntensity);

				glLightfv(light, GL_AMBIENT, vec);

				/* XXX */
				glLightf(light, GL_SPOT_EXPONENT,
					0.5/($f(beamWidth)+0.1));
				glLightf(light, GL_SPOT_CUTOFF,
					$f(cutOffAngle)/3.1415926536*180);
			}
		}
	',
);

#######################################################################
#
# ExtraMem - extra members for the structures to hold
# 	cached info

%ExtraMem = (
	Group => 'int has_light; ',
);

$ExtraMem{Transform} = $ExtraMem{Group};
$ExtraMem{Billboard} = $ExtraMem{Group};
$ExtraMem{Anchor} = $ExtraMem{Group};
$ExtraMem{Collision} = $ExtraMem{Group};


#######################################################################
#
# ChangedC - when the fields change, the following code is run before
# rendering for caching the data.
# 	

%ChangedC = (
	Group => '
		int i;
		int nc = $f_n(children); 
		struct VRML_Virt virt_DirectionalLight;
		$i(has_light) = 0;
		for(i=0; i<nc; i++) {
			void *p = $f(children,i);
			if($ntyptest(p,DirectionalLight)) {
				$i(has_light) ++;
			}
		}
	'
);


$ChangedC{Transform} = $ChangedC{Group};
$ChangedC{Billboard} = $ChangedC{Group};
$ChangedC{Anchor} = $ChangedC{Group};
$ChangedC{Collision} = $ChangedC{Group};


#######################################################################
#
# ProximityC = following code is run to let proximity sensors send their 
# events. This is done in the rendering pass, because the position of 
# of the object relative to the viewer is available via the 
# modelview transformation matrix.
#

%ProximityC = (
ProximitySensor => q~
	/* Viewer pos = t_r2 */
	double cx,cy,cz;
	double len;
	struct pt dr1r2;
	struct pt dr2r3;
	struct pt vec;
	struct pt nor1,nor2;
	struct pt ins;
	static const struct pt yvec = {0,0.05,0};
	static const struct pt zvec = {0,0,-0.05};
	static const struct pt zpvec = {0,0,0.05};
	static const struct pt orig = {0,0,0};
	struct pt t_zvec, t_yvec, t_orig;
	GLdouble modelMatrix[16]; 
	GLdouble projMatrix[16];

	/* transforms viewers coordinate space into sensors coordinate space. 
	 * this gives the orientation of the viewer relative to the sensor.   
	 */
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projMatrix);
	gluUnProject(orig.x,orig.y,orig.z,modelMatrix,projMatrix,viewport,
		&t_orig.x,&t_orig.y,&t_orig.z);
	gluUnProject(zvec.x,zvec.y,zvec.z,modelMatrix,projMatrix,viewport,
		&t_zvec.x,&t_zvec.y,&t_zvec.z);
	gluUnProject(yvec.x,yvec.y,yvec.z,modelMatrix,projMatrix,viewport,
		&t_yvec.x,&t_yvec.y,&t_yvec.z);

	cx = t_orig.x - $f(center,0);
	cy = t_orig.y - $f(center,1);
	cz = t_orig.z - $f(center,2);

	if(!$f(enabled)) return;
	if($f(size,0) == 0 || $f(size,1) == 0 || $f(size,2) == 0) return;

	if(fabs(cx) > $f(size,0)/2 ||
	   fabs(cy) > $f(size,1)/2 ||
	   fabs(cz) > $f(size,2)/2) return;

	/* Ok, we now have to compute... */
	$f(__hit) = 1;

	/* Position */
	$f(__t1,0) = t_orig.x;
	$f(__t1,1) = t_orig.y;
	$f(__t1,2) = t_orig.z;

	VECDIFF(t_zvec,t_orig,dr1r2);  /* Z axis */
	VECDIFF(t_yvec,t_orig,dr2r3);  /* Y axis */

	len = sqrt(VECSQ(dr1r2)); VECSCALE(dr1r2,1/len);
	len = sqrt(VECSQ(dr2r3)); VECSCALE(dr2r3,1/len);

	if(verbose) printf("PROX_INT: (%f %f %f) (%f %f %f) (%f %f %f)\n (%f %f %f) (%f %f %f)\n",
		t_orig.x, t_orig.y, t_orig.z, 
		t_zvec.x, t_zvec.y, t_zvec.z, 
		t_yvec.x, t_yvec.y, t_yvec.z,
		dr1r2.x, dr1r2.y, dr1r2.z, 
		dr2r3.x, dr2r3.y, dr2r3.z
		);
	
	if(fabs(VECPT(dr1r2, dr2r3)) > 0.001) {
		printf ("Sorry, can't handle unevenly scaled ProximitySensors yet :("
		  "dp: %f v: (%f %f %f) (%f %f %f)\n", VECPT(dr1r2, dr2r3),
		  	dr1r2.x,dr1r2.y,dr1r2.z,
		  	dr2r3.x,dr2r3.y,dr2r3.z
			);
		return;
	}


	if(APPROX(dr1r2.z,1.0)) {
		/* rotation */
		$f(__t2,0) = 0;
		$f(__t2,1) = 0;
		$f(__t2,2) = 1;
		$f(__t2,3) = atan2(-dr2r3.x,dr2r3.y);
	} else if(APPROX(dr2r3.y,1.0)) {
		/* rotation */
		$f(__t2,0) = 0;
		$f(__t2,1) = 1;
		$f(__t2,2) = 0;
		$f(__t2,3) = atan2(dr1r2.x,dr1r2.z);
	} else {
		/* Get the normal vectors of the possible rotation planes */
		nor1 = dr1r2;
		nor1.z -= 1.0;
		nor2 = dr2r3;
		nor2.y -= 1.0;

		/* Now, the intersection of the planes, obviously cp */
		VECCP(nor1,nor2,ins);
/* don t know why this is here JAS

		if(APPROX(VECSQ(ins),0)) {
			printf ("Should die here: Proximitysensor problem!\n");
		}
*/

		len = sqrt(VECSQ(ins)); VECSCALE(ins,1/len);

		/* the angle */
		VECCP(dr1r2,ins, nor1);
		VECCP(zpvec, ins, nor2);
		len = sqrt(VECSQ(nor1)); VECSCALE(nor1,1/len);
		len = sqrt(VECSQ(nor2)); VECSCALE(nor2,1/len);
		VECCP(nor1,nor2,ins);

		$f(__t2,3) = -atan2(sqrt(VECSQ(ins)), VECPT(nor1,nor2));

		/* rotation  - should normalize sometime... */
		$f(__t2,0) = ins.x;
		$f(__t2,1) = ins.y;
		$f(__t2,2) = ins.z;
	}
	if(verbose) printf("NORS: (%f %f %f) (%f %f %f) (%f %f %f)\n",
		nor1.x, nor1.y, nor1.z,
		nor2.x, nor2.y, nor2.z,
		ins.x, ins.y, ins.z
	);
~,


);

#######################################################################
#
# ProximityC = following code is run to do collision detection 
#
# In collision nodes:
#    if enabled:
#       if no proxy:
#           passes rendering to its children
#       else (proxy)
#           passes rendering to its proxy
#    else
#       does nothing.
#
# In normal nodes:
#    uses gl modelview matrix to determine distance from viewer and 
# angle from viewer. ...
#
#
#	       /* the shape of the avatar is a cylinder */
#	       /*                                           */
#	       /*           |                               */
#	       /*           |                               */
#	       /*           |--|                            */
#	       /*           | width                         */
#	       /*        ---|---       -                    */
#	       /*        |     |       |                    */
#	       /*    ----|() ()| - --- | ---- y=0           */
#	       /*        |  \  | |     |                    */
#	       /*     -  | \ / | |head | height             */
#	       /*    step|     | |     |                    */
#	       /*     -  |--|--| -     -                    */
#	       /*           |                               */
#	       /*           |                               */
#	       /*           x,z=0                           */
	       

%CollisionC = (
Sphere => q~
	       struct pt t_orig; /*transformed origin*/
	       struct pt p_orig; /*projected transformed origin */ 
	       struct pt n_orig; /*normal(unit length) transformed origin */
	       GLdouble modelMatrix[16]; 
	       GLdouble dist2;
	       struct pt tmppt;
	       struct pt delta = {0,0,0};
	       GLdouble radius;
	       GLdouble tmp;

	       /*easy access, naviinfo.step unused for sphere collisions */
	       GLdouble awidth = naviinfo.width; /*avatar width*/
	       GLdouble atop = naviinfo.height * 1./3; /*top of avatar (relative to eyepoint)*/
	       GLdouble abottom = naviinfo.height * -2./3.; /*bottom of avatar (relative to eyepoint)*/

	       struct pt dir;

	       /* get the transformed position of the Sphere, and the scale-corrected radius. */
	       glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	       t_orig.x = modelMatrix[12];
	       t_orig.y = modelMatrix[13];
	       t_orig.z = modelMatrix[14];
	       radius = pow(det3x3(modelMatrix),1./3.) * $f(radius);

	       /* squared distance to center of sphere (on the y plane)*/
	       dist2 = t_orig.x * t_orig.x + t_orig.z * t_orig.z;

	       /* easy tests. clip as if sphere was a box */
	       /*clip with cylinder */
	       if(dist2 - (radius + awidth) * (radius +awidth) > 0) {
		   return;
	       } 
	       /*clip with bottom plane */
	       if(t_orig.y + radius < abottom) {
		   return;
	       }
	       /*clip with top plane */
	       if(t_orig.y-radius > atop) {
		   return;
	       } 
	       
	       /* project onto (y x t_orig) plane */
	       p_orig.x = sqrt(dist2);
	       p_orig.y = t_orig.y;
	       p_orig.z = 0;
	       /* we need this to unproject rapidly */
	       /* n_orig is t_orig.y projected on the y plane, then normalized. */
	       n_orig.x = t_orig.x;
	       n_orig.y = 0.0;
	       n_orig.z = t_orig.z;
	       VECSCALE(n_orig,1.0/p_orig.x); /*equivalent to vecnormal(n_orig);, but faster */

	       /* 5 cases : sphere is over, over side, side, under and side, under (relative to y axis) */
	       /* these 5 cases correspond to the 5 vornoi regions of the cylinder */
	       if(p_orig.y > atop) { 
		   
		   if(p_orig.x < awidth) { 
		       if(verbose) printf(" /* over, we push down. */ \n");
		       delta.y = (p_orig.y - radius) - (atop);
		   } else { 
		       struct pt d2s;
		       GLdouble ratio;
		       if(verbose) printf(" /* over side */ \n");

		       /* distance vector from corner to center of sphere*/
		       d2s.x = p_orig.x - awidth;
		       d2s.y = p_orig.y - (atop);
		       d2s.z = 0;
		       
		       ratio = 1- radius/sqrt(d2s.x * d2s.x + d2s.y * d2s.y);
		       
		       if(ratio >= 0) {
			   /* no collision */
			   return;
		       }
	       
		       /* distance vector from corner to surface of sphere, (do the math) */
		       VECSCALE(d2s, ratio );

		       /* unproject, this is the fastest way */
		       delta.y = d2s.y;
		       delta.x = d2s.x* n_orig.x;
		       delta.z = d2s.x* n_orig.z;
		   }
	       } else if(p_orig.y < abottom) {
		   if(p_orig.x < awidth) { 
		       if(verbose) printf(" /* under, we push up. */ \n");
		       delta.y = (p_orig.y + radius) -abottom;
		   } else { 
		       struct pt d2s;
		       GLdouble ratio;
		       if(verbose) printf(" /* under side */ \n");

		       /* distance vector from corner to center of sphere*/
		       d2s.x = p_orig.x - awidth;
		       d2s.y = p_orig.y - abottom;
		       d2s.z = 0;
		       
		       ratio = 1- radius/sqrt(d2s.x * d2s.x + d2s.y * d2s.y);
		       
		       if(ratio >= 0) {
			   /* no collision */
			   return;
		       }
	       
		       /* distance vector from corner to surface of sphere, (do the math) */
		       VECSCALE(d2s, ratio );

		       /* unproject, this is the fastest way */
		       delta.y = d2s.y;
		       delta.x = d2s.x* n_orig.x;
		       delta.z = d2s.x* n_orig.z;
		   }

	       } else {
		   if(verbose) printf(" /* side */ \n");
		   
		   /* push to side */
		   delta.x = ((p_orig.x - radius)- awidth) * n_orig.x;
		   delta.z = ((p_orig.x - radius)- awidth) * n_orig.z;
	       }


	       VECADD(CollisionOffset,delta);

	       if(verbose_collision && (delta.x != 0. || delta.y != 0. || delta.z != 0.)) 
	           printf("COLLISION_SPH: (%f %f %f) (%f %f %f) (px=%f nx=%f nz=%f)\n",
			  t_orig.x, t_orig.y, t_orig.z,
			  delta.x, delta.y, delta.z,
			  p_orig.x, n_orig.x, n_orig.z
			  );
	       

	       ~,
Box => q~

	       /*easy access, naviinfo.step unused for sphere collisions */
	       GLdouble awidth = naviinfo.width; /*avatar width*/
	       GLdouble atop = naviinfo.height * 1./3; /*top of avatar (relative to eyepoint)*/
	       GLdouble abottom = naviinfo.height * -2./3.; /*bottom of avatar (relative to eyepoint)*/

	       GLdouble modelMatrix[16]; 
	       struct pt iv = {$f(size,0),0,0};
	       struct pt jv = {0,$f(size,1),0};
	       struct pt kv = {0,0,$f(size,2)};
	       struct pt ov = {-$f(size,0)/2,-$f(size,1)/2,-$f(size,2)/2};
	       struct pt t_orig = {0,0,0};
	       GLdouble scale; /* FIXME: won''t work for non-uniform scales. */

	       struct pt delta;
	       
	       /* get the transformed position of the Sphere, and the scale-corrected radius. */
	       glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);

	       /* values for rapid test */
	       t_orig.x = modelMatrix[12];
	       t_orig.y = modelMatrix[13];
	       t_orig.z = modelMatrix[14];
	       scale = pow(det3x3(modelMatrix),1./3.);
	       if(!fast_ycylinder_box_intersect(abottom,atop,awidth,t_orig,scale*$f(size,0),scale*$f(size,1),scale*$f(size,2))) return;
	            
	       

	       /* get transformed box edges and position */
	       transform(&ov,&ov,modelMatrix);
	       transform3x3(&iv,&iv,modelMatrix);
	       transform3x3(&jv,&jv,modelMatrix);
	       transform3x3(&kv,&kv,modelMatrix);

	       

	       delta = box_disp(abottom,atop,awidth,ov,iv,jv,kv);
	       
	       vecscale(&delta,&delta,-1);
	       
	       VECADD(CollisionOffset,delta);

	       if(verbose_collision && (fabs(delta.x) != 0. || fabs(delta.y) != 0. || fabs(delta.z) != 0.)) 
	           printf("COLLISION_BOX: (%f %f %f) (%f %f %f)\n",
			  ov.x, ov.y, ov.z,
			  delta.x, delta.y, delta.z
			  );
	       if(verbose_collision && (fabs(delta.x != 0.) || fabs(delta.y != 0.) || fabs(delta.z) != 0.)) 
	           printf("iv=(%f %f %f) jv=(%f %f %f) kv=(%f %f %f)\n",
			  iv.x, iv.y, iv.z,
			  jv.x, jv.y, jv.z,
			  kv.x, kv.y, kv.z
			  );
	       
	       
	       ~,

Cone => q~

	       /*easy access, naviinfo.step unused for sphere collisions */
	       GLdouble awidth = naviinfo.width; /*avatar width*/
	       GLdouble atop = naviinfo.height * 1./3; /*top of avatar (relative to eyepoint)*/
	       GLdouble abottom = naviinfo.height * -2./3.; /*bottom of avatar (relative to eyepoint)*/

		float h = $f(height)/2;
		float r = $f(bottomRadius); 

	       GLdouble modelMatrix[16]; 
	       struct pt iv = {0,h,0};
	       struct pt jv = {0,-h,0};
	       GLdouble scale; /* FIXME: won''t work for non-uniform scales. */
	       struct pt t_orig = {0,0,0};

	       struct pt delta;
	       
	       /* get the transformed position of the Sphere, and the scale-corrected radius. */
	       glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);

	       /* values for rapid test */
	       t_orig.x = modelMatrix[12];
	       t_orig.y = modelMatrix[13];
	       t_orig.z = modelMatrix[14];
	       scale = pow(det3x3(modelMatrix),1./3.);
	       if(!fast_ycylinder_cone_intersect(abottom,atop,awidth,t_orig,scale*h,scale*r)) return;
	            
	       

	       /* get transformed box edges and position */
	       transform(&iv,&iv,modelMatrix);
	       transform(&jv,&jv,modelMatrix);

	       delta = cone_disp(abottom,atop,awidth,jv,iv,scale*r);
	       
	       vecscale(&delta,&delta,-1);
	       
	       VECADD(CollisionOffset,delta);

	       if(verbose_collision && (fabs(delta.x) != 0. || fabs(delta.y) != 0. || fabs(delta.z) != 0.)) 
	           printf("COLLISION_CON: (%f %f %f) (%f %f %f)\n",
			  iv.x, iv.y, iv.z,
			  delta.x, delta.y, delta.z
			  );
	       if(verbose_collision && (fabs(delta.x != 0.) || fabs(delta.y != 0.) || fabs(delta.z) != 0.)) 
	           printf("iv=(%f %f %f) jv=(%f %f %f) bR=%f\n",
			  iv.x, iv.y, iv.z,
			  jv.x, jv.y, jv.z,
			  scale*r							       
			  );
	       
	       
	       ~,

Cylinder => q~

	       /*easy access, naviinfo.step unused for sphere collisions */
	       GLdouble awidth = naviinfo.width; /*avatar width*/
	       GLdouble atop = naviinfo.height * 1./3; /*top of avatar (relative to eyepoint)*/
	       GLdouble abottom = naviinfo.height * -2./3.; /*bottom of avatar (relative to eyepoint)*/

		float h = $f(height)/2;
		float r = $f(radius); 

	       GLdouble modelMatrix[16]; 
	       struct pt iv = {0,h,0};
	       struct pt jv = {0,-h,0};
	       GLdouble scale; /* FIXME: won''t work for non-uniform scales. */
	       struct pt t_orig = {0,0,0};

	       struct pt delta;
	       
	       /* get the transformed position of the Sphere, and the scale-corrected radius. */
	       glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);

	       /* values for rapid test */
	       t_orig.x = modelMatrix[12];
	       t_orig.y = modelMatrix[13];
	       t_orig.z = modelMatrix[14];
	       scale = pow(det3x3(modelMatrix),1./3.);
	       if(!fast_ycylinder_cone_intersect(abottom,atop,awidth,t_orig,scale*h,scale*r)) return;
	            
	       

	       /* get transformed box edges and position */
	       transform(&iv,&iv,modelMatrix);
	       transform(&jv,&jv,modelMatrix);


	       delta = cylinder_disp(abottom,atop,awidth,jv,iv,scale*r);
	       
	       vecscale(&delta,&delta,-1);
	       
	       VECADD(CollisionOffset,delta);

	       if(verbose_collision && (fabs(delta.x) != 0. || fabs(delta.y) != 0. || fabs(delta.z) != 0.)) 
	           printf("COLLISION_CYL: (%f %f %f) (%f %f %f)\n",
			  iv.x, iv.y, iv.z,
			  delta.x, delta.y, delta.z
			  );
	       if(verbose_collision && (fabs(delta.x != 0.) || fabs(delta.y != 0.) || fabs(delta.z) != 0.)) 
	           printf("iv=(%f %f %f) jv=(%f %f %f) bR=%f\n",
			  iv.x, iv.y, iv.z,
			  jv.x, jv.y, jv.z,
			  scale*r							       
			  );
	       
	       
	       ~,

IndexedFaceSet => q~
	       GLdouble awidth = naviinfo.width; /*avatar width*/
	       GLdouble atop = naviinfo.height * 1./3; /*top of avatar (relative to eyepoint)*/
	       GLdouble abottom = naviinfo.height * -2./3.; /*bottom of avatar (relative to eyepoint)*/
	       GLdouble modelMatrix[16]; 
	       struct SFColor *points; int npoints;
	       int i;

	       GLdouble scale; /* FIXME: won''t work for non-uniform scales. */
	       struct pt t_orig = {0,0,0};
	       static int refnum = 0;

	       struct pt delta = {0,0,0};

	       struct VRML_PolyRep pr;
	       $mk_polyrep();
	       /* get "coord", why isn''t this already in the polyrep??? */
	       $fv(coord, points, get3, &npoints);
	       pr = *((struct VRML_PolyRep*)this_->_intern);
	       
	       glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);

	       /* values for rapid test */
	       t_orig.x = modelMatrix[12];
	       t_orig.y = modelMatrix[13];
	       t_orig.z = modelMatrix[14];
	       scale = pow(det3x3(modelMatrix),1./3.);
/*	       if(!fast_ycylinder_cone_intersect(abottom,atop,awidth,t_orig,scale*h,scale*r)) return;*/
	            
	
/*	       printf("npoints=%d\n",npoints);
	       for(i = 0; i < npoints; i++) {
		   printf("points[%d]=(%f,%f,%f)\n",i,points[i].c[0], points[i].c[1], points[i].c[2]);
	       }*/
	       pr.coord = (void*)points;
	       delta = polyrep_disp(abottom,atop,awidth,pr,modelMatrix,0);
	       
	       vecscale(&delta,&delta,-1);
	       
	       VECADD(CollisionOffset,delta);

	       if(verbose_collision && (fabs(delta.x) != 0. || fabs(delta.y) != 0. || fabs(delta.z) != 0.))  {
		   fprintf(stderr,"COLLISION_IFS: ref%d (%f %f %f) (%f %f %f)\n",refnum++,
			  t_orig.x, t_orig.y, t_orig.z,
			  delta.x, delta.y, delta.z
			  );
		   
	       }
	       
~,

Extrusion => q~


	       GLdouble awidth = naviinfo.width; /*avatar width*/
	       GLdouble atop = naviinfo.height * 1./3; /*top of avatar (relative to eyepoint)*/
	       GLdouble abottom = naviinfo.height * -2./3.; /*bottom of avatar (relative to eyepoint)*/
	       GLdouble modelMatrix[16]; 
	       struct SFColor *points; int npoints;
	       int i;

	       GLdouble scale; /* FIXME: won''t work for non-uniform scales. */
	       struct pt t_orig = {0,0,0};
	       static int refnum = 0;

	       struct pt delta = {0,0,0};

	       struct VRML_PolyRep pr;
	       prflags flags = 0;
	       $mk_polyrep();
	       if(!$f(solid)) {
		   flags = flags | PR_DOUBLESIDED;
	       }
/*	       printf("_PolyRep = %d\n",this_->_intern);*/
	       pr = *((struct VRML_PolyRep*)this_->_intern);
	       glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);

	       /* values for rapid test */
	       t_orig.x = modelMatrix[12];
	       t_orig.y = modelMatrix[13];
	       t_orig.z = modelMatrix[14];
	       scale = pow(det3x3(modelMatrix),1./3.);
/*	       if(!fast_ycylinder_cone_intersect(abottom,atop,awidth,t_orig,scale*h,scale*r)) return;*/
	            
/*	       printf("ntri=%d\n",pr.ntri);
	       for(i = 0; i < pr.ntri; i++) {
		   printf("cindex[%d]=%d\n",i,pr.cindex[i]);
	       }*/
	       delta = polyrep_disp(abottom,atop,awidth,pr,modelMatrix,flags);
	       
	       vecscale(&delta,&delta,-1);
	       
	       VECADD(CollisionOffset,delta);

	       if(verbose_collision && (fabs(delta.x) != 0. || fabs(delta.y) != 0. || fabs(delta.z) != 0.))  {
		   fprintf(stderr,"COLLISION_EXT: ref%d (%f %f %f) (%f %f %f)\n",refnum++,
			  t_orig.x, t_orig.y, t_orig.z,
			  delta.x, delta.y, delta.z
			  );
		   
	       }
	       
~,

#Extrusion => '',
Shape => '

/*		printf("Shape\n");*/
	',



);



1;
