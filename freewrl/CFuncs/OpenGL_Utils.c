/*******************************************************************
 Copyright (C) 1998 Tuomas J. Lukka 2003 John Stewart, Ayla Khan CRC Canada
 Portions Copyright (C) 1998 John Breen
 DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 See the GNU Library General Public License (file COPYING in the distribution)
 for conditions of use and redistribution.
*********************************************************************/

/*
 * $Id: OpenGL_Utils.c,v 1.2 2003/06/13 21:45:37 ayla Exp $
 *
 */

#include "OpenGL_Utils.h"

static int render_frame = 5;	/* do we render, or do we sleep? */
static int now_mapped = 1;		/* are we on screen, or minimized? */


int
get_now_mapped()
{
	return now_mapped;
}


void
set_now_mapped(int val)
{
	now_mapped = val;
}


/* should we render? */
void
set_render_frame()
{
	/* render a couple of frames to let events propagate */
	render_frame = 5;
}


int
get_render_frame()
{
	return (render_frame && now_mapped);
}


void 
dec_render_frame()
{
	if (render_frame > 0) {
		render_frame--;
	}
}

void
glpOpenGLInitialize()
{
	GLclampf red = 0.0, green = 0.0, blue = 0.0, alpha = 1.0, ref = 0.1;

	/* Configure OpenGL for our uses. */

	glClearColor(red, green, blue, alpha);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	/*
     * JAS - ALPHA testing for textures - right now we just use 0/1 alpha
     * JAS   channel for textures - true alpha blending can come when we sort
     * JAS   nodes.
	 */

	/* glEnable(GL_BLEND); */
	/* glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); */
	glAlphaFunc (GL_GREATER, ref);
	glEnable (GL_ALPHA_TEST);

	/* end of ALPHA test */
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_CULL_FACE);

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelStorei(GL_PACK_ALIGNMENT,1);

	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, (float) 0.2 * 128);
}

void
BackEndClearBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void 
BackEndLightsOff()
{
	glDisable(GL_LIGHT1); /* Put them all off first (except headlight)*/
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glDisable(GL_LIGHT7);
}

void 
BackEndHeadlightOff()
{
	glDisable(GL_LIGHT0); /* headlight off (or other, if no headlight) */
}


void
BackEndHeadlightOn()
{
	float pos[] = { 0.0, 0.0, 1.0, 0.0 };
	float s[] = { 1.0, 1.0, 1.0, 1.0 };

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, s);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, s);
	glLightfv(GL_LIGHT0, GL_SPECULAR, s);
}
