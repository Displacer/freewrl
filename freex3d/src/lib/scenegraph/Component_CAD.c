/*
=INSERT_TEMPLATE_HERE=

$Id: Component_CAD.c,v 1.3 2013/07/15 02:37:11 crc_canada Exp $

X3D Rendering Component

*/


/****************************************************************************
    This file is part of the FreeWRL/FreeX3D Distribution.

    Copyright 2013 John Alexander Stewart

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
#include "../opengl/Frustum.h"
#include "../opengl/Material.h"
#include "../opengl/OpenGL_Utils.h"
#include "Component_Shape.h"
#include "../scenegraph/RenderFuncs.h"
#include "../scenegraph/Polyrep.h"

void prep_CADAssembly (struct X3D_CADAssembly *node) {
}

void child_CADAssembly (struct X3D_CADAssembly *node) {
}

void compile_CADAssembly (struct X3D_CADAssembly *node) {
}

void prep_CADLayer (struct X3D_CADLayer *node) {
}

void child_CADLayer (struct X3D_CADLayer *node) {
}

void compile_CADLayer (struct X3D_CADLayer *node) {
}



void render_IndexedQuadSet (struct X3D_IndexedQuadSet *node) {
                COMPILE_POLY_IF_REQUIRED( node->coord, node->color, node->normal, node->texCoord)
		CULL_FACE(node->solid)
		render_polyrep(node);
}

void render_QuadSet (struct X3D_QuadSet *node) {
                COMPILE_POLY_IF_REQUIRED(node->coord, node->color, node->normal, node->texCoord)
		CULL_FACE(node->solid)
		render_polyrep(node);
}

