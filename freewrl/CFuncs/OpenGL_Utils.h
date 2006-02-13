/*******************************************************************
 Copyright (C) 2003 John Stewart, CRC Canada.
 DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 See the GNU Library General Public License (file COPYING in the distribution)
 for conditions of use and redistribution.
*********************************************************************/

#ifndef __OPENGL_UTILS_H_
#define __OPENGL_UTILS_H_

/*
 * $Id: OpenGL_Utils.h,v 1.14 2006/02/13 18:26:21 crc_canada Exp $
 *
 */

#ifdef AQUA

#include <gl.h>
#include <glu.h>
#include <glext.h>

#else

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#endif

#ifdef LINUX
#include <GL/glext.h>
#endif


#include "headers.h"

void start_textureTransform (void *textureNode, int ttnum);
void end_textureTransform (void *textureNode, int ttnum);

int
get_now_mapped(void);


void
set_now_mapped(int val);


void
glpOpenGLInitialize(void);


void
BackEndClearBuffer(void);

void
BackEndLightsOff(void);

void lightState (GLint light, int state);


#ifndef AQUA
extern Display *Xdpy;
extern Window Xwin;
extern GLXContext GLcx;
extern void resetGeometry();
#endif
extern void glpOpenGLInitialize(void);

#endif /* __OPENGL_UTILS_H_ */
