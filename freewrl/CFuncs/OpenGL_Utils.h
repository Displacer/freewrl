#ifndef __OPENGL_UTILS_H_
#define __OPENGL_UTILS_H_

/*
 * $Id: OpenGL_Utils.h,v 1.2 2003/06/06 20:22:49 ayla Exp $
 *
 */


#include "headers.h"


int
get_now_mapped(void);


void
set_now_mapped(int val);


void
set_render_frame(void);


int
get_render_frame(void);


void
dec_render_frame(void);


#endif /* __OPENGL_UTILS_H_ */
