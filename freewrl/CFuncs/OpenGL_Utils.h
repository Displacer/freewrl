#ifndef __OPENGL_UTILS_H_
#define __OPENGL_UTILS_H_

/*
 * $Id: OpenGL_Utils.h,v 1.4 2003/07/31 17:08:01 crc_canada Exp $
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
#include <GL/glext.h>

#endif


#include "headers.h"


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

void 
BackEndHeadlightOff(void);

void
BackEndHeadlightOn(void);

#endif /* __OPENGL_UTILS_H_ */
