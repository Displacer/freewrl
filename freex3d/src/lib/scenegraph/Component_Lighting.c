/*
=INSERT_TEMPLATE_HERE=

$Id: Component_Lighting.c,v 1.19 2012/10/26 19:45:44 crc_canada Exp $

X3D Lighting Component

*/

/****************************************************************************
    This file is part of the FreeWRL/FreeX3D Distribution.

    Copyright 2009 CRC Canada. (http://www.crc.gc.ca)

    FreeWRL/FreeX3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FreeWRL/FreeX3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FreeWRL/FreeX3D.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/



#include <config.h>
#include <system.h>
#include <display.h>
#include <internal.h>

#include <libFreeWRL.h>

#include "../vrml_parser/Structs.h"
#include "../main/headers.h"
#include "../opengl/OpenGL_Utils.h"
#include "RenderFuncs.h"
//#include "../opengl/OpenGL_Utils.h"
#include "LinearAlgebra.h"

#define RETURN_IF_LIGHT_STATE_NOT_US \
		if (renderstate()->render_light== VF_globalLight) { \
			if (!node->global) return;\
			/* printf ("and this is a global light\n"); */\
		} else if (node->global) return; \
		/* else printf ("and this is a local light\n"); */

#define CHBOUNDS(aaa) \
    if (aaa.c[0]>1.0) aaa.c[0] = 1.0; \
    if (aaa.c[0]<0.0) aaa.c[0] = 0.0; \
    if (aaa.c[1]>1.0) aaa.c[1] = 1.0; \
    if (aaa.c[1]<0.0) aaa.c[1] = 0.0; \
    if (aaa.c[2]>1.0) aaa.c[2] = 1.0; \
    if (aaa.c[2]<0.0) aaa.c[3] = 0.0; 


void compile_DirectionalLight (struct X3D_DirectionalLight *node) {
    struct point_XYZ vec;

    vec.x = (double) -((node->direction).c[0]);
    vec.y = (double) -((node->direction).c[1]);
    vec.z = (double) -((node->direction).c[2]);
    normalize_vector(&vec);
    node->_dir.c[0] = (float) vec.x;
    node->_dir.c[1] = (float) vec.y;
    node->_dir.c[2] = (float) vec.z;
    node->_dir.c[3] = 0.0f;/* 0.0 = DirectionalLight */

    node->_col.c[0] = ((node->color).c[0]) * (node->intensity);
    node->_col.c[1] = ((node->color).c[1]) * (node->intensity);
    node->_col.c[2] = ((node->color).c[2]) * (node->intensity);
    node->_col.c[3] = 1;
    CHBOUNDS(node->_col);
    
    
    node->_amb.c[0] = ((node->color).c[0]) * (node->ambientIntensity);
    node->_amb.c[1] = ((node->color).c[1]) * (node->ambientIntensity);
    node->_amb.c[2] = ((node->color).c[2]) * (node->ambientIntensity);
    node->_amb.c[3] = 1;
    CHBOUNDS(node->_amb);
    MARK_NODE_COMPILED;
}


void render_DirectionalLight (struct X3D_DirectionalLight *node) {
	/* if we are doing global lighting, is this one for us? */
	RETURN_IF_LIGHT_STATE_NOT_US

    COMPILE_IF_REQUIRED;

	if(node->on) {
		int light = nextlight();
		if(light >= 0) {
			lightState(light,TRUE);
			FW_GL_LIGHTFV(light, GL_POSITION, (GLfloat* )node->_dir.c);
			FW_GL_LIGHTFV(light, GL_DIFFUSE, node->_col.c);
			FW_GL_LIGHTFV(light, GL_SPECULAR, node->_col.c);
			FW_GL_LIGHTFV(light, GL_AMBIENT, node->_amb.c);
		}
	}
}

/* global lights  are done before the rendering of geometry */
void prep_DirectionalLight (struct X3D_DirectionalLight *node) {
	if (!renderstate()->render_light) return;
	render_DirectionalLight(node);
}

void compile_PointLight (struct X3D_PointLight *node) {
    int i;
    
    for (i=0; i<3; i++) node->_loc.c[0] = node->location.c[0];
    node->_loc.c[3] = 0.0f;/* vec3 to vec4... */
    
    node->_col.c[0] = ((node->color).c[0]) * (node->intensity);
    node->_col.c[1] = ((node->color).c[1]) * (node->intensity);
    node->_col.c[2] = ((node->color).c[2]) * (node->intensity);
    node->_col.c[3] = 1;
    CHBOUNDS(node->_col);
    
    
    node->_amb.c[0] = ((node->color).c[0]) * (node->ambientIntensity);
    node->_amb.c[1] = ((node->color).c[1]) * (node->ambientIntensity);
    node->_amb.c[2] = ((node->color).c[2]) * (node->ambientIntensity);
    node->_amb.c[3] = 1;
    CHBOUNDS(node->_amb);
    MARK_NODE_COMPILED;
}


void render_PointLight (struct X3D_PointLight *node) {

	/* if we are doing global lighting, is this one for us? */
	RETURN_IF_LIGHT_STATE_NOT_US

    COMPILE_IF_REQUIRED;

	if(node->on) {
		int light = nextlight();
		if(light >= 0) {
			float vec[4] = {0.0f, 0.0f, -1.0f, 1.0f};
            
			lightState(light,TRUE);
			FW_GL_LIGHTFV(light, GL_SPOT_DIRECTION, vec);
			FW_GL_LIGHTFV(light, GL_POSITION, node->_loc.c);

			FW_GL_LIGHTF(light, GL_CONSTANT_ATTENUATION,
				((node->attenuation).c[0]));
			FW_GL_LIGHTF(light, GL_LINEAR_ATTENUATION,
				((node->attenuation).c[1]));
			FW_GL_LIGHTF(light, GL_QUADRATIC_ATTENUATION,
				((node->attenuation).c[2]));


			FW_GL_LIGHTFV(light, GL_DIFFUSE, node->_col.c);
			FW_GL_LIGHTFV(light, GL_SPECULAR, node->_col.c);
			FW_GL_LIGHTFV(light, GL_AMBIENT, node->_amb.c);

			/* XXX */
			FW_GL_LIGHTF(light, GL_SPOT_CUTOFF, 180);
		}
	}
}

/* pointLights are done before the rendering of geometry */
void prep_PointLight (struct X3D_PointLight *node) {

	if (!renderstate()->render_light) return;
	/* this will be a global light here... */
	render_PointLight(node);
}

void compile_SpotLight (struct X3D_SpotLight *node) {
    struct point_XYZ vec;
    int i;
    
    for (i=0; i<3; i++) node->_loc.c[0] = node->location.c[0];
    node->_loc.c[3] = 0.0f;/* vec3 to vec4... */

    vec.x = (double) -((node->direction).c[0]);
    vec.y = (double) -((node->direction).c[1]);
    vec.z = (double) -((node->direction).c[2]);
    normalize_vector(&vec);
    node->_dir.c[0] = (float) vec.x;
    node->_dir.c[1] = (float) vec.y;
    node->_dir.c[2] = (float) vec.z;
    node->_dir.c[3] = 1.0f;/* 1.0 = SpotLight */

    node->_col.c[0] = ((node->color).c[0]) * (node->intensity);
    node->_col.c[1] = ((node->color).c[1]) * (node->intensity);
    node->_col.c[2] = ((node->color).c[2]) * (node->intensity);
    node->_col.c[3] = 1;
    CHBOUNDS(node->_col);
    
    
    node->_amb.c[0] = ((node->color).c[0]) * (node->ambientIntensity);
    node->_amb.c[1] = ((node->color).c[1]) * (node->ambientIntensity);
    node->_amb.c[2] = ((node->color).c[2]) * (node->ambientIntensity);
    node->_amb.c[3] = 1;
    CHBOUNDS(node->_amb);

    MARK_NODE_COMPILED;
}


void render_SpotLight(struct X3D_SpotLight *node) {
	float ft;

	/* if we are doing global lighting, is this one for us? */
	RETURN_IF_LIGHT_STATE_NOT_US

    COMPILE_IF_REQUIRED;

	if(node->on) {
		int light = nextlight();
		if(light >= 0) {
			lightState(light,TRUE);
			FW_GL_LIGHTFV(light, GL_SPOT_DIRECTION, node->_dir.c);
			FW_GL_LIGHTFV(light, GL_POSITION, node->_loc.c);
	
			FW_GL_LIGHTF(light, GL_CONSTANT_ATTENUATION,
					((node->attenuation).c[0]));
			FW_GL_LIGHTF(light, GL_LINEAR_ATTENUATION,
					((node->attenuation).c[1]));
			FW_GL_LIGHTF(light, GL_QUADRATIC_ATTENUATION,
					((node->attenuation).c[2]));
	
            FW_GL_LIGHTFV(light, GL_DIFFUSE, node->_col.c);
			FW_GL_LIGHTFV(light, GL_SPECULAR, node->_col.c);
			FW_GL_LIGHTFV(light, GL_AMBIENT, node->_amb.c);
            
			ft = 0.5f/(node->beamWidth +0.1f);
			if (ft>128.0) ft=128.0f;
			if (ft<0.0) ft=0.0f;
			FW_GL_LIGHTF(light, GL_SPOT_EXPONENT,ft);

			ft = node->cutOffAngle /3.1415926536f*180.0f;
			if (ft>90.0) ft=90.0f;
			if (ft<0.0) ft=0.0f;
			FW_GL_LIGHTF(light, GL_SPOT_CUTOFF, ft);
		}
	}
}
/* SpotLights are done before the rendering of geometry */
void prep_SpotLight (struct X3D_SpotLight *node) {
	if (!renderstate()->render_light) return;
	render_SpotLight(node);
}
