/*
=INSERT_TEMPLATE_HERE=

$Id: Frustum.c,v 1.65 2013/10/30 19:26:58 crc_canada Exp $

???

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
#include "../scenegraph/quaternion.h"
#include "../scenegraph/Viewer.h"
#include "Frustum.h"
#include "../opengl/OpenGL_Utils.h"
#include "../scenegraph/LinearAlgebra.h"


#include "Textures.h"
#include <float.h>

//#define FRUSTUMVERBOSE

static void quaternion_multi_rotation(struct point_XYZ *ret, const Quaternion *quat, const struct point_XYZ * v, int count);
static void add_translation (struct point_XYZ *arr,  float x, float y, float z, int count);
static void multiply_in_scale(struct point_XYZ *arr, float x, float y, float z, int count);



/*********************************************************************
 * OLD - NOW USE Occlusion tests
 * Frustum calculations. Definitive work (at least IMHO) is thanks to
 * Steven Baker - look at http://sjbaker.org/steve/omniv/frustcull.html/
 *
 * Thanks Steve!
 *
 */


#undef  OCCLUSIONVERBOSE

#ifdef OCCLUSION

	/* if we have a visible Shape node, how long should we wait until we try to determine
	   if it is still visible? */
	#define OCCWAIT 		20 

	/* we have a visibility sensor, we want to really see when it becomes invis. */
	#define OCCCHECKNEXTLOOP	1

	/* we are invisible - don't let it go too long before we try to see visibility */
	#define OCCCHECKSOON		4

	/* how many samples of a Shape are needed before it becomes visible? If it is too
	   small, don't worry about displaying it If this number is too large, "flashing" 
	   will occur, as the shape is dropped, while still displaying (the number) of pixels
	   on the screen */
	#define OCCSHAPESAMPLESIZE	1	
#endif //OCCLUSION

typedef struct pFrustum{
	/* Occlusion VisibilitySensor code */
	GLuint *OccQueries;// = NULL;

	/* newer occluder code */
	GLuint potentialOccluderCount;// = 0;
	void ** occluderNodePointer;// = NULL;

	/* older occluder code */
	#ifdef OCCLUSION 
	int maxOccludersFound;// = 0;
	int QueryCount;// = 0;
	int OccInitialized;// = FALSE;
	#endif

	GLuint OccQuerySize;//=0;

	//	#ifdef OCCLUSIONVERBOSE
	//		GLint queryCounterBits;
	//	#endif

	GLint OccResultsAvailable;// = FALSE;

}* ppFrustum;
void *Frustum_constructor(){
	void *v = malloc(sizeof(struct pFrustum));
	memset(v,0,sizeof(struct pFrustum));
	return v;
}
void Frustum_init(struct tFrustum *t){
	//public
	t->OccFailed = FALSE;
	//private
	t->prv = Frustum_constructor();
	{
		ppFrustum p = (ppFrustum)t->prv;
		/* Occlusion VisibilitySensor code */
		p->OccQueries = NULL;

		/* newer occluder code */
		p->potentialOccluderCount = 0;
		p->occluderNodePointer = NULL;

		/* older occluder code */
		#ifdef OCCLUSION 
		p->maxOccludersFound = 0;
		p->QueryCount = 0;
		p->OccInitialized = FALSE;
		#endif

		p->OccQuerySize=0;

		p->OccResultsAvailable = FALSE;
	}
}

void beginOcclusionQuery(struct X3D_VisibilitySensor* node, int render_geometry)
{
	ppFrustum p = (ppFrustum)gglobal()->Frustum.prv;
	if (render_geometry) { 
		if (p->potentialOccluderCount < p->OccQuerySize) { 
            TRACE_MSG ("beginOcclusionQuery, potoc %d occQ %d\n",p->potentialOccluderCount, p->OccQuerySize); 
			if (node->__occludeCheckCount < 0) { 
				 TRACE_MSG ("beginOcclusionQuery, query %u, node %s\n",p->potentialOccluderCount, stringNodeType(node->_nodeType)); 
#if !defined(GL_ES_VERSION_2_0)
//void glBeginQuery(GLenum, GLuint);

				FW_GL_BEGIN_QUERY(GL_SAMPLES_PASSED, p->OccQueries[p->potentialOccluderCount]); 
#endif
				p->occluderNodePointer[p->potentialOccluderCount] = (void *)node; 
			} 
		}
	} 
}

void endOcclusionQuery(struct X3D_VisibilitySensor* node, int render_geometry)
{
	ppFrustum p = (ppFrustum)gglobal()->Frustum.prv;
	if (render_geometry) { 
		if (p->potentialOccluderCount < p->OccQuerySize) { 
			if (node->__occludeCheckCount < 0) { 
				TRACE_MSG ("glEndQuery node %p\n",node); 
#if !defined( GL_ES_VERSION_2_0 )
				FW_GL_END_QUERY(GL_SAMPLES_PASSED); 
#endif
				p->potentialOccluderCount++; 
			} 
		} 
	} 
}

#define PROP_EXTENT_CHECK \
		if (maxx > geomParent->EXTENT_MAX_X) {geomParent->EXTENT_MAX_X = maxx; touched = TRUE;} \
		if (minx < geomParent->EXTENT_MIN_X) {geomParent->EXTENT_MIN_X = minx; touched = TRUE;} \
		if (maxy > geomParent->EXTENT_MAX_Y) {geomParent->EXTENT_MAX_Y = maxy; touched = TRUE;} \
		if (miny < geomParent->EXTENT_MIN_Y) {geomParent->EXTENT_MIN_Y = miny; touched = TRUE;} \
		if (maxz > geomParent->EXTENT_MAX_Z) {geomParent->EXTENT_MAX_Z = maxz; touched = TRUE;} \
		if (minz < geomParent->EXTENT_MIN_Z) {geomParent->EXTENT_MIN_Z = minz; touched = TRUE;} 

#define FRUSTUM_TRANS(myNodeType)  \
	if (me->_nodeType == NODE_##myNodeType) { \
		/* have we actually done a propagateExtent on this one? Because we look at ALL points in a boundingBox, \
		   because of rotations, we HAVE to ensure that the default values are not there, otherwise we will \
		   take the "inside out" boundingBox as being correct! */ \
 \
			/* has this node actually been extented away from the default? */ \
 \
		if (!APPROX(me->EXTENT_MAX_X,-10000.0)) { \
			struct X3D_##myNodeType *node; \
			Quaternion rq; \
			struct point_XYZ inxyz[8]; struct point_XYZ outxyz[8]; \
			node = (struct X3D_##myNodeType *)me; \
	 \
			/* make up a "cube" with vertexes being our bounding box */ \
			BBV(0,MAX_X,MAX_Y,MAX_Z); \
			BBV(1,MAX_X,MAX_Y,MIN_Z); \
			BBV(2,MAX_X,MIN_Y,MAX_Z); \
			BBV(3,MAX_X,MIN_Y,MIN_Z); \
			BBV(4,MIN_X,MAX_Y,MAX_Z); \
			BBV(5,MIN_X,MAX_Y,MIN_Z); \
			BBV(6,MIN_X,MIN_Y,MAX_Z); \
			BBV(7,MIN_X,MIN_Y,MIN_Z); \
	 \
	                /* 1: REVERSE CENTER */ \
	                if (node->__do_center) { \
				add_translation(inxyz,-node->center.c[0],-node->center.c[1],-node->center.c[2],8); \
			} \
	 \
	                /* 2: REVERSE SCALE ORIENTATION */ \
	                if (node->__do_scaleO) { \
				vrmlrot_to_quaternion(&rq,node->scaleOrientation.c[0], node->scaleOrientation.c[1], node->scaleOrientation.c[2], -node->scaleOrientation.c[3]); \
				quaternion_multi_rotation(outxyz,&rq,inxyz,8); \
	 \
				/* copy these points back out */ \
				memcpy (inxyz,outxyz,8*sizeof(struct point_XYZ));  \
			} \
	 \
	                /* 3: SCALE */ \
	                if (node->__do_scale) { \
	                        /* FW_GL_SCALE_F(node->scale.c[0],node->scale.c[1],node->scale.c[2]); */ \
				multiply_in_scale(inxyz,node->scale.c[0],node->scale.c[1],node->scale.c[2],8); \
			} \
	 \
	                /* 4: SCALEORIENTATION */ \
	                if (node->__do_scaleO) { \
				vrmlrot_to_quaternion(&rq,node->scaleOrientation.c[0], node->scaleOrientation.c[1], node->scaleOrientation.c[2], -node->scaleOrientation.c[3]); \
				quaternion_multi_rotation(outxyz,&rq,inxyz,8); \
	 \
				/* copy these points back out */ \
				memcpy (inxyz,outxyz,8*sizeof(struct point_XYZ)); \
	                } \
	 \
	                /* 5: ROTATION */ \
	                if (node->__do_rotation) { \
	                        /* FW_GL_ROTATE_F(my_rotation, node->rotation.c[0],node->rotation.c[1],node->rotation.c[2]); */ \
				vrmlrot_to_quaternion(&rq,node->rotation.c[0], node->rotation.c[1], node->rotation.c[2], node->rotation.c[3]); \
				quaternion_multi_rotation(outxyz,&rq,inxyz,8); \
	 \
				/* copy these points back out */ \
				memcpy (inxyz,outxyz,8*sizeof(struct point_XYZ)); \
	                } \
	 \
	                /* 6: CENTER */ \
	                if (node->__do_center) { \
	                        /* FW_GL_TRANSLATE_F(node->center.c[0],node->center.c[1],node->center.c[2]); */ \
				add_translation(inxyz,node->center.c[0],node->center.c[1],node->center.c[2],8); \
			} \
\
	                /* 7: TRANSLATION */ \
	                if (node->__do_trans) { \
	                        /* FW_GL_TRANSLATE_F(node->translation.c[0],node->translation.c[1],node->translation.c[2]); */ \
				add_translation(inxyz,node->translation.c[0],node->translation.c[1],node->translation.c[2],8); \
			} \
	 \
	 \
			/* work changes into extent */ \
            /* because we have materially moved this Transform, the WHOLE Bounding Box has moved, too, \
                thus we do not bother to test against OLD max/min values */ \
            maxx = -FLT_MAX; maxy = -FLT_MAX; maxz = -FLT_MAX; \
            minx = FLT_MAX; miny = FLT_MAX; minz = FLT_MAX; \
			for (i=0; i<8; i++) { \
				if (inxyz[i].x > maxx) maxx =  (float)inxyz[i].x; \
				if (inxyz[i].y > maxy) maxy =  (float)inxyz[i].y; \
				if (inxyz[i].z > maxz) maxz =  (float)inxyz[i].z; \
				if (inxyz[i].x < minx) minx =  (float)inxyz[i].x; \
				if (inxyz[i].y < miny) miny =  (float)inxyz[i].y; \
				if (inxyz[i].z < minz) minz =  (float)inxyz[i].z; \
			} \
		} \
	} 


#define FRUSTUM_GEOLOCATION  \
	if (me->_nodeType == NODE_GeoLocation) { \
		/* have we actually done a propagateExtent on this one? Because we look at ALL points in a boundingBox, \
		   because of rotations, we HAVE to ensure that the default values are not there, otherwise we will \
		   take the "inside out" boundingBox as being correct! */ \
 \
			/* has this node actually been extented away from the default? */ \
 \
		if (!APPROX(me->EXTENT_MAX_X,-10000.0)) { \
		struct X3D_GeoLocation *node; \
		Quaternion rq; \
		struct point_XYZ inxyz[8]; struct point_XYZ outxyz[8]; \
		node = (struct X3D_GeoLocation *)me; \
	 \
		/* make up a "cube" with vertexes being our bounding box */ \
		BBV(0,MAX_X,MAX_Y,MAX_Z); \
		BBV(1,MAX_X,MAX_Y,MIN_Z); \
		BBV(2,MAX_X,MIN_Y,MAX_Z); \
		BBV(3,MAX_X,MIN_Y,MIN_Z); \
		BBV(4,MIN_X,MAX_Y,MAX_Z); \
		BBV(5,MIN_X,MAX_Y,MIN_Z); \
		BBV(6,MIN_X,MIN_Y,MAX_Z); \
		BBV(7,MIN_X,MIN_Y,MIN_Z); \
	 \
\
	        /* 5: ROTATION */ \
		vrmlrot_to_quaternion(&rq,node->__localOrient.c[0], node->__localOrient.c[1], node->__localOrient.c[2], node->__localOrient.c[3]); \
		quaternion_multi_rotation(outxyz,&rq,inxyz,8); \
	 \
		/* copy these points back out */ \
		memcpy (inxyz,outxyz,8*sizeof(struct point_XYZ)); \
	 \
                /* 7: TRANSLATION */ \
                /* FW_GL_TRANSLATE_F(node->translation.c[0],node->translation.c[1],node->translation.c[2]); */ \
		/*printf ("doing translation %f %f %f\n", node->__movedCoords.c[0],node->__movedCoords.c[1],node->__movedCoords.c[2]); */ \
		add_translation(inxyz,(float) node->__movedCoords.c[0],(float) node->__movedCoords.c[1],(float) node->__movedCoords.c[2],8); \
 \
	 \
	 \
		/* work changes into extent */ \
            /* because we have materially moved this Transform, the WHOLE Bounding Box has moved, too, \
                thus we do not bother to test against OLD max/min values */ \
            maxx = -FLT_MAX; maxy = -FLT_MAX; maxz = -FLT_MAX; \
            minx = FLT_MAX; miny = FLT_MAX; minz = FLT_MAX; \
		for (i=0; i<8; i++) { \
			if (inxyz[i].x > maxx) maxx = (float) inxyz[i].x; \
			if (inxyz[i].y > maxy) maxy = (float) inxyz[i].y; \
			if (inxyz[i].z > maxz) maxz = (float) inxyz[i].z; \
			if (inxyz[i].x < minx) minx = (float) inxyz[i].x; \
			if (inxyz[i].y < miny) miny = (float) inxyz[i].y; \
			if (inxyz[i].z < minz) minz = (float) inxyz[i].z; \
		} \
		} \
	} 

#define FRUSTUM_GEOTRANS  \
	if (me->_nodeType == NODE_GeoTransform) { \
		/* have we actually done a propagateExtent on this one? Because we look at ALL points in a boundingBox, \
		   because of rotations, we HAVE to ensure that the default values are not there, otherwise we will \
		   take the "inside out" boundingBox as being correct! */ \
 \
			/* has this node actually been extented away from the default? */ \
 \
		if (!APPROX(me->EXTENT_MAX_X,-10000.0)) { \
			struct X3D_GeoTransform *node; \
			Quaternion rq; \
			struct point_XYZ inxyz[8]; struct point_XYZ outxyz[8]; \
			node = (struct X3D_GeoTransform *)me; \
	 \
			/* make up a "cube" with vertexes being our bounding box */ \
			BBV(0,MAX_X,MAX_Y,MAX_Z); \
			BBV(1,MAX_X,MAX_Y,MIN_Z); \
			BBV(2,MAX_X,MIN_Y,MAX_Z); \
			BBV(3,MAX_X,MIN_Y,MIN_Z); \
			BBV(4,MIN_X,MAX_Y,MAX_Z); \
			BBV(5,MIN_X,MAX_Y,MIN_Z); \
			BBV(6,MIN_X,MIN_Y,MAX_Z); \
			BBV(7,MIN_X,MIN_Y,MIN_Z); \
	 \
	                /* 1: REVERSE CENTER */ \
	                if (node->__do_center) { \
				add_translation(inxyz,(float) (-node->geoCenter.c[0]), (float) (-node->geoCenter.c[1]),(float)(-node->geoCenter.c[2]),8); \
			} \
	 \
	                /* 2: REVERSE SCALE ORIENTATION */ \
	                if (node->__do_scaleO) { \
				vrmlrot_to_quaternion(&rq,node->scaleOrientation.c[0], node->scaleOrientation.c[1], node->scaleOrientation.c[2], -node->scaleOrientation.c[3]); \
				quaternion_multi_rotation(outxyz,&rq,inxyz,8); \
				memcpy (inxyz,outxyz,8*sizeof(struct point_XYZ)); \
			} \
	 \
	                /* 3: SCALE */ \
	                if (node->__do_scale) { \
				multiply_in_scale(inxyz,node->scale.c[0],node->scale.c[1],node->scale.c[2],8); \
			} \
	 \
	                /* 4: SCALEORIENTATION */ \
	                if (node->__do_scaleO) { \
				vrmlrot_to_quaternion(&rq,node->scaleOrientation.c[0], node->scaleOrientation.c[1], node->scaleOrientation.c[2], -node->scaleOrientation.c[3]); \
				quaternion_multi_rotation(outxyz,&rq,inxyz,8); \
				memcpy (inxyz,outxyz,8*sizeof(struct point_XYZ)); \
	                } \
	 \
	                /* 5: ROTATION */ \
	                if (node->__do_rotation) { \
				vrmlrot_to_quaternion(&rq,node->rotation.c[0], node->rotation.c[1], node->rotation.c[2], node->rotation.c[3]); \
				quaternion_multi_rotation(outxyz,&rq,inxyz,8); \
				memcpy (inxyz,outxyz,8*sizeof(struct point_XYZ)); \
	                } \
	 \
	                /* 6: CENTER */ \
	                if (node->__do_center) { \
				add_translation(inxyz,(float)node->geoCenter.c[0],(float)node->geoCenter.c[1],(float)node->geoCenter.c[2],8); \
			} \
			add_translation (inxyz,(float) X3D_GEOTRANSFORM(node)->__movedCoords.c[0], (float) X3D_GEOTRANSFORM(node)->__movedCoords.c[1], (float) X3D_GEOTRANSFORM(node)->__movedCoords.c[2],8); \
\
			vrmlrot_to_quaternion(&rq,X3D_GEOTRANSFORM(node)->__localOrient.c[0], X3D_GEOTRANSFORM(node)->__localOrient.c[1], X3D_GEOTRANSFORM(node)->__localOrient.c[2], X3D_GEOTRANSFORM(node)->__localOrient.c[3]); \
			quaternion_multi_rotation(outxyz,&rq,inxyz,8); \
			memcpy (inxyz,outxyz,8*sizeof(struct point_XYZ)); \
	                /* 7: TRANSLATION */ \
	                if (node->__do_trans) { \
				add_translation(inxyz,node->translation.c[0],node->translation.c[1],node->translation.c[2],8); \
			} \
	 \
	 \
			/* work changes into extent */ \
            		/* because we have materially moved this Transform, the WHOLE Bounding Box has moved, too, \
            		    thus we do not bother to test against OLD max/min values */ \
            		maxx = -FLT_MAX; maxy = -FLT_MAX; maxz = -FLT_MAX; \
            		minx = FLT_MAX; miny = FLT_MAX; minz = FLT_MAX; \
			for (i=0; i<8; i++) { \
				if (inxyz[i].x > maxx) maxx = (float) inxyz[i].x; \
				if (inxyz[i].y > maxy) maxy = (float) inxyz[i].y; \
				if (inxyz[i].z > maxz) maxz = (float) inxyz[i].z; \
				if (inxyz[i].x < minx) minx = (float) inxyz[i].x; \
				if (inxyz[i].y < miny) miny = (float) inxyz[i].y; \
				if (inxyz[i].z < minz) minz = (float) inxyz[i].z; \
			} \
		} \
	} 




/* does this current node actually fit in the Switch rendering scheme? */
int is_Switchchild_inrange(struct X3D_Switch *node, struct X3D_Node *me) {
        int wc = node->whichChoice;

        /* is this VRML, or X3D?? */
        if (node->__isX3D == 0) {
                if(wc >= 0 && wc < ((node->choice).n)) {
                        void *p = ((node->choice).p[wc]);
                        return (X3D_NODE(p)==me);
                }
        } else {
                if(wc >= 0 && wc < ((node->children).n)) {
                        void *p = ((node->children).p[wc]);
                        return (X3D_NODE(p)==me);
                }
        }
	return FALSE;
}


/* does this current node actually display, according to the CADLayer scheme? */
int is_CADLayerchild_inrange(struct X3D_CADLayer *node, struct X3D_Node *me) {
    int i;
    for (i=0; i<node->children.n; i++) {
        
        /* if we have more children than we have indexes into visible field, just return TRUE */
        if ((i >= node->visible.n) && (node->children.p[i] == me)) return TRUE;
        
        /* if not, if it is in the visible field, return true */
        else if ((node->visible.p[i]) && (node->children.p[i] == me)) return TRUE;
        }
    /* not visible, so return false */
    return FALSE;
}

/* does this current node actually fit in the GeoLOD rendering scheme? */
int is_GeoLODchild_inrange (struct X3D_GeoLOD* gpnode, struct X3D_Node *me) {
	/* is this node part of the active path for rendering? */
	int x,y;
	y = FALSE;

	for (x=0; x<gpnode->rootNode.n; x++) {
		/* printf ("comparing %u:%u %d of %d, types me %s rootNodeField: %s\n", 
			me, X3D_NODE(gpnode->rootNode.p[x]),
			x, gpnode->rootNode.n,
			stringNodeType (me->_nodeType),
			stringNodeType( X3D_NODE(gpnode->rootNode.p[x])->_nodeType)
			);
		*/

		if (me == X3D_NODE(gpnode->rootNode.p[x])) {
			y=TRUE;
			break;
		}
	}

/*
	if (y) printf ("GeoLOD, found child in rootNode "); else printf ("GeoLOD, child NOT part of ROOT ");
	if (X3D_GEOLOD(geomParent)->__inRange) printf ("INRANGE "); else printf ("NOT inrange ");
*/
	/* is this one actually being rendered? */
	return (y ^ gpnode->__inRange);
}


/* take the measurements of a geometry (eg, box), and save it. Note
 * that what is given is a Shape, the values get pushed up to the
 * Geometries grouping node parent. */


/* this is used for collision in transformChildren - don't bother going through
   children of a transform if there is nothing close... */

void setExtent(float maxx, float minx, float maxy, float miny, float maxz, float minz, struct X3D_Node *me) {
	int c,d;
	struct X3D_Node *shapeParent;
	struct X3D_Node *geomParent;
	int touched;

	UNUSED(touched); //compiler warning mitigation
    
	#ifdef FRUSTUMVERBOSE
	printf ("setExtent maxx %f minx %f maxy %f miny %f maxz %f minz %f me %p nt %s\n",
			maxx, minx, maxy, miny, maxz, minz, me, stringNodeType(me->_nodeType));
	#endif

	/* record this for ME for sorting purposes for sorting children fields */
	me->EXTENT_MAX_X = maxx; me->EXTENT_MIN_X = minx;
	me->EXTENT_MAX_Y = maxy; me->EXTENT_MIN_Y = miny;
	me->EXTENT_MAX_Z = maxz; me->EXTENT_MIN_Z = minz;

	if (me->_parentVector == NULL) {
		#ifdef FRUSTUMVERBOSE
		printf ("setExtent, parentVector NULL for node %p type %s\n",
			me,stringNodeType(me->_nodeType));
		#endif
		return;
	}

	for (c=0; c<vectorSize(me->_parentVector); c++) {
		shapeParent = vector_get(struct X3D_Node *, me->_parentVector,c);
	
		/* record this for ME for sorting purposes for sorting children fields */
		shapeParent->EXTENT_MAX_X = maxx; shapeParent->EXTENT_MIN_X = minx;
		shapeParent->EXTENT_MAX_Y = maxy; shapeParent->EXTENT_MIN_Y = miny;
		shapeParent->EXTENT_MAX_Z = maxz; shapeParent->EXTENT_MIN_Z = minz;
	
		#ifdef FRUSTUMVERBOSE
		if (shapeParent == NULL)
		printf ("parent %u of %u is %p, is null\n",c,vectorSize(me->_parentVector),shapeParent); 
		else
		printf ("parent %u of %u is %p, type %s\n",c,vectorSize(me->_parentVector),shapeParent,stringNodeType(shapeParent->_nodeType)); 
		#endif

		for (d=0; d<vectorSize(shapeParent->_parentVector); d++) {
			geomParent = vector_get(struct X3D_Node *, shapeParent->_parentVector,d);

			#ifdef FRUSTUMVERBOSE
			printf ("setExtent in loop, parent %u of shape %s is %s\n",c,stringNodeType(shapeParent->_nodeType),
				stringNodeType(geomParent->_nodeType)); 
	
			/* is there a problem with this geomParent? */
			if (!checkNode(geomParent, __FILE__, __LINE__)) printf ("problem here with checkNode\n");
			#endif
	
	
			/* note, maxz is positive, minz is negative, distance should be negative, so we take a negative distance,
				and subtract the "positive" z value to get the closest point, then take the negative distance,
				and subtract the "negative" z value to get the far distance */
	
			PROP_EXTENT_CHECK;
	
			#ifdef FRUSTUMVERBOSE
			printf ("setExtent - now I am %p (%s) has extent maxx %f minx %f maxy %f miny %f maxz %f minz %f\n",
					me, stringNodeType(me->_nodeType),
					me->EXTENT_MAX_X ,
					me->EXTENT_MIN_X ,
					me->EXTENT_MAX_Y ,
					me->EXTENT_MIN_Y ,
					me->EXTENT_MAX_Z ,
					me->EXTENT_MIN_Z);
			printf ("setExtent - now parent %p (%s) has extent maxx %f minx %f maxy %f miny %f maxz %f minz %f\n",
					geomParent, stringNodeType(geomParent->_nodeType),
					geomParent->EXTENT_MAX_X ,
					geomParent->EXTENT_MIN_X ,
					geomParent->EXTENT_MAX_Y ,
					geomParent->EXTENT_MIN_Y ,
					geomParent->EXTENT_MAX_Z ,
					geomParent->EXTENT_MIN_Z);
			#endif 

		}
	}
}

static void quaternion_multi_rotation(struct point_XYZ *ret, const Quaternion *quat, const struct point_XYZ * v, int count){
	int i;
	for (i=0; i<count; i++) {
		quaternion_rotation(ret, quat, v);
		ret++; v++;
	}
}


#define BBV(num,XX,YY,ZZ) \
			inxyz[num].x= (double) (me->EXTENT_##XX); \
			inxyz[num].y= (double) (me->EXTENT_##YY); \
			inxyz[num].z= (double) (me->EXTENT_##ZZ);

static void add_translation (struct point_XYZ *arr,  float x, float y, float z, int count) {
	int i;
	for (i=0; i<count; i++) {
		arr->x += (double)x;
		arr->y += (double)y;
		arr->z += (double)z;
		arr++;
	}
}

static void multiply_in_scale(struct point_XYZ *arr, float x, float y, float z, int count) {
	int i;
	for (i=0; i<count; i++) {
		arr->x *= (double)x;
		arr->y *= (double)y;
		arr->z *= (double)z;
		arr++;
	}
}


void printmatrix(GLDOUBLE* mat) {
    int i;
    for(i = 0; i< 16; i++) {
	printf("mat[%d] = %4.3f%s",i,mat[i],i==3 ? "\n" : i==7? "\n" : i==11? "\n" : "");
    }
	printf ("\n");

}


/* for children nodes; set the parent grouping nodes extent - we expect the center
 * of the group to be passed in in the floats x,y,z */

void propagateExtent(struct X3D_Node *me) {
	float minx, miny, minz, maxx, maxy, maxz;
	int i;
	struct X3D_Node *geomParent;
	int touched;

	if (me==NULL) return;


	#ifdef FRUSTUMVERBOSE
	printf ("propextent Iam %s, myExtent (%4.2f %4.2f) (%4.2f %4.2f) (%4.2f %4.2f) me %p parents %d\n",
			stringNodeType(me->_nodeType),
			me->EXTENT_MAX_X, me->EXTENT_MIN_X,
			me->EXTENT_MAX_Y, me->EXTENT_MIN_Y,
			me->EXTENT_MAX_Z, me->EXTENT_MIN_Z,
			me, vectorSize(me->_parentVector));
	#endif


	if (me->_parentVector == NULL) {
		ConsoleMessage ("propagateExtent, parentVector NULL, me %p %s\n",
			me,stringNodeType(me->_nodeType));
		return;
	}

	/* calculate the maximum of the current position, and add the previous extent */
	/* these MIGHT be overwritten if we have a Transform node here */
	/* these values are used in PROP_EXTENT_CHECK */
	maxx = me->EXTENT_MAX_X; minx = me->EXTENT_MIN_X;
	maxy = me->EXTENT_MAX_Y; miny = me->EXTENT_MIN_Y;
	maxz = me->EXTENT_MAX_Z; minz = me->EXTENT_MIN_Z;

	/* is this a transform? Should we add in the translated position?? */
	FRUSTUM_TRANS(Transform);
	FRUSTUM_GEOTRANS;
	FRUSTUM_GEOLOCATION;
	FRUSTUM_TRANS(HAnimSite);
	FRUSTUM_TRANS(HAnimJoint);


	for (i=0; i<vectorSize(me->_parentVector); i++) {
		geomParent = vector_get(struct X3D_Node *, me->_parentVector, i);

		/* do we propagate for this parent? */
		touched = FALSE;

		/* switch nodes - only propagate extent back up if the node is "active" */
		switch (geomParent->_nodeType) {
			case NODE_GeoLOD: 
				if (is_GeoLODchild_inrange(X3D_GEOLOD(geomParent), me)) {
			                PROP_EXTENT_CHECK;
        			}
				break;
			case NODE_LOD:
				/* works for both X3D and VRML syntax; compare with the "_selected" field */
				if (me == X3D_LODNODE(geomParent)->_selected) {
			                PROP_EXTENT_CHECK;
        			}
				break;
        
			case NODE_Switch: 
				if (is_Switchchild_inrange(X3D_SWITCH(geomParent), me)) {
			                PROP_EXTENT_CHECK;
        			}
                break;
            case NODE_CADLayer: 
                if (is_CADLayerchild_inrange(X3D_CADLAYER(geomParent),me)) {
                    PROP_EXTENT_CHECK;
				}
				break;
			default: {
				PROP_EXTENT_CHECK;
			}
		}
            
                
		#ifdef FRUSTUMVERBOSE
		printf ("after transform calcs me (%p %s) my parent %d is (%p %s) ext %4.2f %4.2f %4.2f %4.2f %4.2f %4.2f\n",
			me, stringNodeType(me->_nodeType),i,geomParent, stringNodeType(geomParent->_nodeType),
			geomParent->EXTENT_MAX_X, geomParent->EXTENT_MIN_X,
			geomParent->EXTENT_MAX_Y, geomParent->EXTENT_MIN_Y,
			geomParent->EXTENT_MAX_Z, geomParent->EXTENT_MIN_Z);
		#endif

		/* now, send these up the line, assuming this child makes the extent larger */
		if (touched) propagateExtent(geomParent); 
        }
}

/* perform all the viewpoint rotations for a point */
/* send in a pointer for the result, the current bounding box point to rotate, and the current ModelView matrix */

void moveAndRotateThisPoint(struct point_XYZ *mypt, double x, double y, double z, double *MM) {
		float outF[3];
		float inF[3];
		inF[0] = (float) x; inF[1] = (float) y; inF[2] = (float) z;

		/* transform this vertex via the modelview matrix */
		transformf (outF,inF,MM);

		#ifdef VERBOSE
		printf ("transformed %4.2f %4.2f %4.2f, to %4.2f %4.2f %4.2f\n",inF[0], inF[1], inF[2],
			outF[0], outF[1], outF[2]);
		#endif
		mypt->x = outF[0]; mypt->y=outF[1],mypt->z = outF[2];
}


/**************************************************************************************/

/* get the center of the bounding box, rotate it, and find out how far it is Z distance from us.
*/


void record_ZBufferDistance(struct X3D_Node *node) {
	GLDOUBLE modelMatrix[16];
	double ex;
	double ey;
	double ez;
	struct point_XYZ movedPt;
	double minMovedDist;

	minMovedDist = -1000000000;

	#ifdef FRUSTUMVERBOSE
	printf ("\nrecordDistance for node %p nodeType %s size %4.2f %4.2f %4.2f ",node, stringNodeType (node->_nodeType),
	node->EXTENT_MAX_X - node->EXTENT_MIN_X,
	node->EXTENT_MAX_Y - node->EXTENT_MIN_Y,
	node->EXTENT_MAX_Z - node->EXTENT_MIN_Z
	); 

	if (APPROX(node->EXTENT_MAX_X,-10000.0)) printf ("EXTENT NOT INIT");

	printf ("\n");
	printf ("recordDistance, max,min %f:%f, %f:%f, %f:%f\n",
		node->EXTENT_MAX_X , node->EXTENT_MIN_X,
		node->EXTENT_MAX_Y , node->EXTENT_MIN_Y,
		node->EXTENT_MAX_Z , node->EXTENT_MIN_Z);
	#endif

	/* get the current pos in modelMatrix land */
	FW_GL_GETDOUBLEV(GL_MODELVIEW_MATRIX, modelMatrix);

#ifdef TRY_ONLY_ONE_POINT
#ifdef TRY_RADIUS
	/* get radius of bounding box around its origin */
	ex = (node->EXTENT_MAX_X - node->EXTENT_MIN_X) / 2.0;
	ey = (node->EXTENT_MAX_Y - node->EXTENT_MIN_Y) / 2.0;
	ez = (node->EXTENT_MAX_Z - node->EXTENT_MIN_Z) / 2.0;
	printf ("	ex %lf ey %lf ez %lf\n",ex,ey,ez);
#else
	/* get the center of the bounding box */
	ex = node->EXTENT_MAX_X + node->EXTENT_MIN_X;
	ey = node->EXTENT_MAX_Y + node->EXTENT_MIN_Y;
	ez = node->EXTENT_MAX_Z + node->EXTENT_MIN_Z;
#endif

	
	/* rotate the center of this point */
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	printf ("%lf %lf %lf centre is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z);
	minMovedDist = movedPt.z;

#else
	
	/* printf ("moving all 8 points of this bounding box\n"); */
	ex = node->EXTENT_MIN_X;
	ey = node->EXTENT_MIN_Y;
	ez = node->EXTENT_MIN_Z;
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	if (movedPt.z > minMovedDist) minMovedDist = movedPt.z;
	/* printf ("%lf %lf %lf moved is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z); */

	ex = node->EXTENT_MIN_X;
	ey = node->EXTENT_MIN_Y;
	ez = node->EXTENT_MAX_Z;
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	if (movedPt.z > minMovedDist) minMovedDist = movedPt.z;
	/* printf ("%lf %lf %lf moved is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z); */

	ex = node->EXTENT_MIN_X;
	ey = node->EXTENT_MAX_Y;
	ez = node->EXTENT_MIN_Z;
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	if (movedPt.z > minMovedDist) minMovedDist = movedPt.z;
	/* printf ("%lf %lf %lf moved is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z); */

	ex = node->EXTENT_MIN_X;
	ey = node->EXTENT_MAX_Y;
	ez = node->EXTENT_MAX_Z;
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	if (movedPt.z > minMovedDist) minMovedDist = movedPt.z;
	/* printf ("%lf %lf %lf moved is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z); */

	ex = node->EXTENT_MAX_X;
	ey = node->EXTENT_MIN_Y;
	ez = node->EXTENT_MIN_Z;
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	if (movedPt.z > minMovedDist) minMovedDist = movedPt.z;
	/* printf ("%lf %lf %lf moved is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z); */

	ex = node->EXTENT_MAX_X;
	ey = node->EXTENT_MIN_Y;
	ez = node->EXTENT_MAX_Z;
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	if (movedPt.z > minMovedDist) minMovedDist = movedPt.z;
	/* printf ("%lf %lf %lf moved is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z); */

	ex = node->EXTENT_MAX_X;
	ey = node->EXTENT_MAX_Y;
	ez = node->EXTENT_MIN_Z;
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	if (movedPt.z > minMovedDist) minMovedDist = movedPt.z;
	/* printf ("%lf %lf %lf moved is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z); */

	ex = node->EXTENT_MAX_X;
	ey = node->EXTENT_MAX_Y;
	ez = node->EXTENT_MAX_Z;
	moveAndRotateThisPoint (&movedPt, ex,ey,ez,modelMatrix);
	if (movedPt.z > minMovedDist) minMovedDist = movedPt.z;
	/* printf ("%lf %lf %lf moved is %lf %lf %lf\n",ex,ey,ez,movedPt.x, movedPt.y, movedPt.z); */
#endif

	node->_dist = minMovedDist;

#ifdef FRUSTUMVERBOSE
	printf ("I am at %lf %lf %lf\n",Viewer()->currentPosInModel.x, Viewer()->currentPosInModel.y, Viewer()->currentPosInModel.z);
	printf ("and distance to the nearest corner of the BB for this node is %lf\n", node->_dist);
#endif


 
#undef FRUSTUMVERBOSE

}

/***************************************************************************/

void OcclusionStartofRenderSceneUpdateScene() {

#ifdef OCCLUSION /* do we have hardware for occlusion culling? */
	int i;
	ppFrustum p;
	ttglobal tg = gglobal();
	p = (ppFrustum)gglobal()->Frustum.prv;
	/* each time through the event loop, we count the occluders. Note, that if, say, a 
	   shape was USED 100 times, that would be 100 occlude queries, BUT ONE SHAPE, thus
	   there is not an implicit 1:1 mapping between shapes and occlude queries */

	p->potentialOccluderCount = 0;

	/* did we have a failure here ? */
	if (tg->Frustum.OccFailed) return;

	/* have we been through this yet? */
	if (p->OccInitialized == FALSE) {
		#ifdef OCCLUSIONVERBOSE
		printf ("initializing OcclusionCulling...\n");
		#endif
		/* do we have an environment variable for this? */
		if (gglobal()->internalc.global_occlusion_disable) {
			tg->Frustum.OccFailed = TRUE;
		} else {
	        	if (gglobal()->display.rdr_caps.av_occlusion_q) {
		
				#ifdef OCCLUSIONVERBOSE
	        	        printf ("OcclusionStartofRenderSceneUpdateScene: have OcclusionQuery\n"); 
				#endif
	
				/* we make the OccQuerySize larger than the maximum number of occluders,
				   so we don't have to realloc too much */
				p->OccQuerySize = p->maxOccludersFound + 1000;

				p->occluderNodePointer = MALLOC (void **, sizeof (void *) * p->OccQuerySize);
				p->OccQueries = MALLOC (GLuint *, sizeof(GLuint) * p->OccQuerySize);
                glGenQueries(p->OccQuerySize,p->OccQueries);
                //ConsoleMessage ("generated %d queries, pointer %p",p->OccQuerySize,p->OccQueries);
				p->OccInitialized = TRUE;
				for (i=0; i<p->OccQuerySize; i++) {
					p->occluderNodePointer[i] = 0;
				}
				p->QueryCount = p->maxOccludersFound; /* for queries - we can do this number */
				#ifdef OCCLUSIONVERBOSE
				printf ("QueryCount now %d\n",p->QueryCount);
				#endif

        		} else {
				#ifdef OCCLUSIONVERBOSE
        	       		 printf ("OcclusionStartofRenderSceneUpdateScene: DO NOT have OcclusionQuery\n"); 
				#endif

				/* we dont seem to have this extension here at runtime! */
				/* this happened, eg, on my Core4 AMD64 box with Mesa	*/
				tg->Frustum.OccFailed = TRUE;
				return;
			}
		}

	}


	/* did we find more shapes than before? */
	if (p->maxOccludersFound > p->QueryCount) {
        	if (p->maxOccludersFound > p->OccQuerySize) {
        	        /* printf ("have to regen queries\n"); */
			p->QueryCount = 0;

			/* possibly previous had zero occluders, lets just not bother deleting for zero */
			if (p->OccQuerySize > 0) {
				glDeleteQueries (p->OccQuerySize, p->OccQueries);
				FW_GL_FLUSH();
			}

			p->OccQuerySize = p->maxOccludersFound + 1000;
			p->occluderNodePointer = REALLOC (p->occluderNodePointer,sizeof (void *) * p->OccQuerySize);
			p->OccQueries = REALLOC (p->OccQueries,sizeof (int) * p->OccQuerySize);
            glGenQueries(p->OccQuerySize,p->OccQueries);
                ConsoleMessage ("reinitialized queries... now %p",p->OccQueries);
			for (i=0; i<p->OccQuerySize; i++) {
				p->occluderNodePointer[i] = 0;
			}
		}
		p->QueryCount = p->maxOccludersFound; /* for queries - we can do this number */
		#ifdef OCCLUSIONVERBOSE
		printf ("QueryCount here is %d\n",p->QueryCount);
		#endif

       }


//	#ifdef OCCLUSIONVERBOSE
//        glGetQueryiv(GL_SAMPLES_PASSED, GL_QUERY_COUNTER_BITS, &p->queryCounterBits);
//        printf ("queryCounterBits %d\n",p->queryCounterBits);
//        #endif
#endif /* OCCLUSION */

}

void OcclusionCulling ()  {
#ifdef OCCLUSION /* do we have hardware for occlusion culling? */
	int i;
	struct X3D_Shape *shapePtr;
	struct X3D_VisibilitySensor *visSenPtr;
	int checkCount;
	GLint samples;
	ppFrustum p;
	ttglobal tg = gglobal();
	p = (ppFrustum)tg->Frustum.prv;

//#ifdef OCCLUSIONVERBOSE
//	{
//	GLint query;
//	glGetQueryiv(GL_SAMPLES_PASSED, GL_CURRENT_QUERY, &query);
//	printf ("currentQuery is %d\n",query);
//	}
//#endif

	visSenPtr = NULL;
	shapePtr = NULL;

	/* Step 0. go through list of assigned nodes, and either:
		- if we have OcclusionQueries: REMOVE the VF_hasVisibleChildren flag;
		- else, set every node to VF_hasVisibleChildren */
	zeroVisibilityFlag();

	/* Step 1. did we have some problem with Occlusion ? */
	if (tg->Frustum.OccFailed) return;
	 
	/* Step 2. go through the list of "OccludeCount" nodes, and determine if they are visible. 
	   If they are not, then, we have to, at some point, make them visible, so that we can test again. */
	/* note that the potentialOccluderCount is only incremented if the __occludeCheckCount tells us
	   that it should be checked again - see the interplay between the eventLoop stuff in OpenGLUtils.c
 	   and the OCCLUSION* defines in headers.h - we DO NOT generate a query every time through the loop */
 
	//#ifdef OCCLUSIONVERBOSE
	//printf ("OcclusionCulling - potentialOccluderCount %d\n",p->potentialOccluderCount);
	//#endif

	for (i=0; i<p->potentialOccluderCount; i++) {
		#ifdef OCCLUSIONVERBOSE
		printf ("checking node %d of %d\n",i, p->potentialOccluderCount);
		#endif

		checkCount = 0;

		/* get the check count field for this node - see if we did a check of this */
		shapePtr = X3D_SHAPE(p->occluderNodePointer[i]);
		if (shapePtr != NULL) {
			if (shapePtr->_nodeType == NODE_Shape) {
				visSenPtr = NULL;
				checkCount = shapePtr->__occludeCheckCount;
			} else if (shapePtr->_nodeType == NODE_VisibilitySensor) {
				visSenPtr = X3D_VISIBILITYSENSOR(shapePtr);
				shapePtr = NULL;
				checkCount = visSenPtr->__occludeCheckCount;
			} else {
				printf ("OcclusionCulling on node type %s not allowed\n",stringNodeType(shapePtr->_nodeType));
				return;
			}
		}

		#ifdef OCCLUSIONVERBOSE
		if (shapePtr) printf ("OcclusionCulling, for a %s (index %d ptr %p) checkCount %d\n",stringNodeType(shapePtr->_nodeType),i,shapePtr,checkCount);
		else printf ("OcclusionCulling, for a %s (index %d) checkCount %d\n",stringNodeType(visSenPtr->_nodeType),i,checkCount);
		#endif

		/* an Occlusion test will have been run on this one */

		FW_GL_GETQUERYOBJECTIV(p->OccQueries[i],GL_QUERY_RESULT_AVAILABLE,&p->OccResultsAvailable);
		PRINT_GL_ERROR_IF_ANY("FW_GL_GETQUERYOBJECTIV::QUERY_RESULTS_AVAIL");

		#define SLEEP_FOR_QUERY_RESULTS
		#ifdef SLEEP_FOR_QUERY_RESULTS
		/* for now, lets loop to see when we get results */
		while (p->OccResultsAvailable == GL_FALSE) {
			usleep(100);
			FW_GL_GETQUERYOBJECTIV(p->OccQueries[i],GL_QUERY_RESULT_AVAILABLE,&p->OccResultsAvailable);
			PRINT_GL_ERROR_IF_ANY("FW_GL_GETQUERYOBJECTIV::QUERY_RESULTS_AVAIL");
		}
		#endif


		#ifdef OCCLUSIONVERBOSE
		if (p->OccResultsAvailable == GL_FALSE) printf ("results not ready for %d\n",i);
		#endif


		/* if we are NOT ready; we keep the count going, but we do NOT change the results of VisibilitySensors */
		if (p->OccResultsAvailable == GL_FALSE) samples = 10000;  
			
	        FW_GL_GETQUERYOBJECTIV (p->OccQueries[i], GL_QUERY_RESULT, &samples);
		PRINT_GL_ERROR_IF_ANY("FW_GL_GETQUERYOBJECTIV::QUERY");
				
		#ifdef OCCLUSIONVERBOSE
		printf ("i %d checkc %d samples %d\n",i,checkCount,samples);
		#endif
	
		if (p->occluderNodePointer[i] != 0) {
		
			/* if this is a VisibilitySensor, record the samples */
			if (visSenPtr != NULL) {

				#ifdef OCCLUSIONVERBOSE
				printf ("OcclusionCulling, found VisibilitySensor at %d, fragments %d active %d\n",i,samples,checkCount);
				#endif

                
				/* if this is a DEF/USE, we might already have done this one, as we have same
				   node pointer used in other places. */
				if (checkCount != OCCCHECKNEXTLOOP) {
	
					if (samples > 0) {
						visSenPtr->__visible  = TRUE;
						visSenPtr->__occludeCheckCount = OCCCHECKNEXTLOOP; /* look for this EVERY time through */
						visSenPtr->__Samples = samples;
					} else {
						visSenPtr->__occludeCheckCount = OCCCHECKSOON; /* check again soon */
						visSenPtr->__visible =FALSE;
						visSenPtr->__Samples = 0;
					}

				 /* } else {
					printf ("shape, already have checkCount == OCCCHECKNEXTLOOP, not changing visibility params\n");
				
			          */	
				}
			}
		
		
			/* is this is Shape? */
			else if (shapePtr != NULL) {
				#ifdef OCCLUSIONVERBOSE
				printf ("OcclusionCulling, found Shape %d, fragments %d active %d\n",i,samples,checkCount);
				#endif
                //if (samples == 0) ConsoleMessage ("invisible shape %d, fragments %d",i,samples);
                
				/* if this is a DEF/USE, we might already have done this one, as we have same
				   node pointer used in other places. */
				if (checkCount != OCCWAIT) {
	
					/* is this node visible? If so, tell the parents! */
					if (samples > OCCSHAPESAMPLESIZE) {
						TRACE_MSG ("Shape %p is VISIBLE\n",shapePtr);
						shapePtr->__visible = TRUE;
						shapePtr->__occludeCheckCount= OCCWAIT; /* wait a little while before checking again */
						shapePtr->__Samples = samples;
					} else {
						TRACE_MSG ("Shape %p is NOT VISIBLE\n",shapePtr);
						shapePtr->__visible=FALSE;
						shapePtr->__occludeCheckCount = OCCCHECKSOON; /* check again soon */
						shapePtr->__Samples = 0; 
					}
				/* } else {
					printf ("shape, already have checkCount == OCCWAIT, not changing visibility params\n");
				*/	
				}
			}
		}
	}
#endif /* OCCLUSION */
}

/* shut down the occlusion stuff */
void zeroOcclusion(void) {

#ifdef OCCLUSION /* do we have hardware for occlusion culling? */

	int i;
	ppFrustum p;
	ttglobal tg = gglobal();
	p= (ppFrustum)tg->Frustum.prv;

	if (tg->Frustum.OccFailed) return;

        #ifdef OCCLUSIONVERBOSE
        printf ("zeroOcclusion - potentialOccluderCount %d\n",p->potentialOccluderCount);
        #endif

        for (i=0; i<p->potentialOccluderCount; i++) {
#ifdef OCCLUSIONVERBOSE
                printf ("checking node %d of %d\n",i, p->potentialOccluderCount);
#endif

                FW_GL_GETQUERYOBJECTIV(p->OccQueries[i],GL_QUERY_RESULT_AVAILABLE,&p->OccResultsAvailable);
                PRINT_GL_ERROR_IF_ANY("FW_GL_GETQUERYOBJECTIV::QUERY_RESULTS_AVAIL");

                /* for now, lets loop to see when we get results */
                while (p->OccResultsAvailable == GL_FALSE) {
#ifdef OCCLUSIONVERBOSE
                        printf ("zero - waiting and looping for results\n"); 
#endif
                        usleep(1000);
                        FW_GL_GETQUERYOBJECTIV(p->OccQueries[i],GL_QUERY_RESULT_AVAILABLE,&p->OccResultsAvailable);
                        PRINT_GL_ERROR_IF_ANY("FW_GL_GETQUERYOBJECTIV::QUERY_RESULTS_AVAIL");
                }
	}
#ifdef OCCLUSIONVERBOSE
	printf ("zeroOcclusion - done waiting\n");
#endif

	p->QueryCount = 0;
    
    // debugging
    //if (p->OccQueries) {
      //  ConsoleMessage ("p->OccQueries exists, p->OccQuerySize %p, p->OccQueries %p",p->OccQuerySize, p->OccQueries);
    //}
	//if(p->OccQueries)
		//glDeleteQueries (p->OccQuerySize, p->OccQueries);
	//FW_GL_FLUSH();
	
	p->OccQuerySize=0;
	p->maxOccludersFound = 0;
	p->OccInitialized = FALSE;
	p->potentialOccluderCount = 0;
    //ConsoleMessage ("freeing OccQueries %p",p->OccQueries);
	FREE_IF_NZ(p->OccQueries);
	FREE_IF_NZ(p->occluderNodePointer);
#endif /* OCCLUSION */
}
