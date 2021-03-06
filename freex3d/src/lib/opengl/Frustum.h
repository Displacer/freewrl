/*
=INSERT_TEMPLATE_HERE=

$Id: Frustum.h,v 1.21 2014/01/16 15:47:49 dug9 Exp $

Global includes.

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


#ifndef __FREEWRL_FRUSTUM_H__
#define __FREEWRL_FRUSTUM_H__

/* for Extents and BoundingBoxen */
#define EXTENT_MAX_X _extent[0]
#define EXTENT_MIN_X _extent[1]
#define EXTENT_MAX_Y _extent[2]
#define EXTENT_MIN_Y _extent[3]
#define EXTENT_MAX_Z _extent[4]
#define EXTENT_MIN_Z _extent[5]

#define RECORD_DISTANCE \
	{\
	ttrenderstate rs = renderstate();\
	if (rs->render_geom && (!rs->render_blend)) {record_ZBufferDistance (X3D_NODE(node)); }\
	}

/* no occlusion queries right now - need to work on the shader implementation 
    of occlusion culling */ 

#if defined (AQUA)
    #define OCCLUSION
    #define VISIBILITYOCCLUSION
    #define SHAPEOCCLUSION
#else
    #undef OCCLUSION
    #undef VISIBILITYOCCLUSION
    #undef SHAPEOCCLUSION
#endif




int newOcclude(void);


#ifdef OCCLUSION
#define OCCLUSIONTEST \
	/* a value of ZERO means that it HAS visible children - helps with initialization */ \
        if ((renderstate()->render_geom!=0) | (renderstate()->render_sensitive!=0)) { \
		/* printf ("OCCLUSIONTEST node %d fl %x\n",node, node->_renderFlags & VF_hasVisibleChildren); */ \
                if ((node->_renderFlags & VF_hasVisibleChildren) == 0) { \
                        /* printf ("WOW - we do NOT need to do this transform but doing it %x!\n",(node->_renderFlags)); \
 printf (" vp %d geom %d light %d sens %d blend %d prox %d col %d\n", \
         render_vp,render_geom,render_light,render_sensitive,render_blend,render_proximity,render_collision); */ \
                        return; \
                } \
        } 
#else
#define OCCLUSIONTEST
#endif


void beginOcclusionQuery(struct X3D_VisibilitySensor* node, int render_geometry);
void endOcclusionQuery(struct X3D_VisibilitySensor* node, int render_geometry);

/*
#define BEGINOCCLUSIONQUERY \
	if (render_geom) { \
		if (potentialOccluderCount < OccQuerySize) { \
 printf ("beginOcclusionQuery, potoc %d occQ %d\n",potentialOccluderCount, OccQuerySize, node->__occludeCheckCount);  \
			if (node->__occludeCheckCount < 0) { \
				 printf ("beginOcclusionQuery, query %u, node %s\n",potentialOccluderCount, stringNodeType(node->_nodeType));  \
				FW_GL_BEGIN_QUERY(GL_SAMPLES_PASSED, OccQueries[potentialOccluderCount]); \
				occluderNodePointer[potentialOccluderCount] = (void *)node; \
			} \
		} \
	} 

#define ENDOCCLUSIONQUERY \
	if (render_geom) { \
		if (potentialOccluderCount < OccQuerySize) { \
			if (node->__occludeCheckCount < 0) { \
				 printf ("glEndQuery node %u\n",node);  \
				FW_GL_END_QUERY(GL_SAMPLES_PASSED); \
				potentialOccluderCount++; \
			} \
		} \
	} 
*/

void moveAndRotateThisPoint(struct point_XYZ *mypt, double x, double y, double z, double *MM);
void setExtent(float maxx, float minx, float maxy, float miny, float maxz, float minz, struct X3D_Node *me);
void printmatrix(GLDOUBLE* mat);
void propagateExtent(struct X3D_Node *me);
void record_ZBufferDistance(struct X3D_Node *node);
void OcclusionStartofRenderSceneUpdateScene(void);
void OcclusionCulling (void);
void zeroOcclusion(void);

int is_Switchchild_inrange(struct X3D_Switch *node, struct X3D_Node *me);
int is_GeoLODchild_inrange (struct X3D_GeoLOD* gpnode, struct X3D_Node *me);
int is_CADLayerchild_inrange(struct X3D_CADLayer *node, struct X3D_Node *me);


#endif /* __FREEWRL_FRUSTUM_H__ */
