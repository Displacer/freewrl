/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart, Ayla Khan CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 * 
 * $Id: jsVRMLClasses.h,v 1.1.2.3 2002/08/30 04:56:18 ayla Exp $
 * 
 * Complex VRML nodes as Javascript classes.
 *
 */

#ifndef __jsVRMLClasses_h__
#define  __jsVRMLClasses_h__

#include <math.h>

#ifndef __jsUtils_h__
#include "jsUtils.h"
#endif /* __jsUtils_h__ */

#include "jsVRMLBrowser.h"
#include "Structs.h"
#include "LinearAlgebra.h" /* do the math */

#define INIT_ARGC_IMG 3
#define INIT_ARGC 3
#define INIT_ARGC_ROT 2

#if 0
/* #define AVECLEN(x) (sqrt((x)[0]*(x)[0]+(x)[1]*(x)[1]+(x)[2]*(x)[2])) */
/* #define AVECPT(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2]) */
/* #define AVECSCALE(x,y) x[0] *= y;\ */
/* x[1] *= y;\ */
/* x[2] *= y; */
/* #define AVECCP(x,y,z) (z)[0]=(x)[1]*(y)[2]-(x)[2]*(y)[1]; \ */
/* (z)[1]=(x)[2]*(y)[0]-(x)[0]*(y)[2]; \ */
/* (z)[2]=(x)[0]*(y)[1]-(x)[1]*(y)[0]; */
#endif

/*
 * The simple VRML field types are supported in perl (see JS.pm):
 * * SFBool, SFFloat, SFTime, SFInt32, SFString
 *
 * VRML field types that are implemented here as Javascript classes
 * are:
 *
 * * SFColor, MFColor
 * * SFVec2f, MFVec2f -- untested!!!
 * * SFVec3f, MFVec3f
 * * SFRotation, MFRotation
 * * SFNode (special case - also implemented in perl (see JS.pm), MFNode
 * * SFImage -- untested!!!
 *
 * These fields have struct types defined elsewhere (see Structs.h)
 * that are stored by Javascript classes as private data. 
 *
 * Some of the computations for SFVec3f, SFRotation are now defined
 * elsewhere (see LinearAlgebra.h) to avoid duplication.
 */



/* helper functions */

static JSBool
doMFAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

static JSBool
doMFSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);


/* class functions */

JSBool
globalResolve(JSContext *cx, JSObject *obj, jsval id);

JSBool
loadVRMLClasses(JSContext *context,
				JSObject *globalObj);

JSBool
setTouchable(JSContext *cx,
			 JSObject *obj,
			 jsval id,
			 jsval *vp);

JSBool
addAssignProperty(void *cx,
				  void *glob,
				  char *name,
				  char *str,
				  void *robj);

/*
 * Adds additional (touchable) property to instance of an existing
 * class.
 */
extern JSBool
addTouchableProperty(void *cx,
					 void *glob,
					 char *name);

extern JSBool
getAssignProperty(JSContext *context,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);

JSBool
setAssignProperty(JSContext *context,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);

JSBool
SFNodeConstr(JSContext *cx,
			 JSObject *obj,
			 uintN argc,
			 jsval *argv,
			 jsval *rval);

JSBool
SFNodeGetProperty(JSContext *cx,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);

JSBool
SFNodeSetProperty(JSContext *cx,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);

extern void *
TJL_SFColorNew(void);

extern void
TJL_SFColorDelete(void *p);

extern void
TJL_SFColorAssign(void *top, void *fromp);

extern void
TJL_SFColorSet(void *p, SV *sv);


JSBool
SFColorToString(JSContext *cx,
				JSObject *obj,
				uintN argc,
				jsval *argv,
				jsval *rval);

JSBool
SFColorAssign(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFColorTouched(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);

JSBool 
SFColorConstr(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFColorGetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool
SFColorSetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool
SFColorFinalize(JSContext *cx,
				JSObject *obj);

JSBool
SFColorSetInternal(void *cx,
				   void *globalObj,
				   char *name,
				   SV *sv);


extern void *
TJL_SFImageNew(void);

extern void
TJL_SFImageDelete(void *p);

extern void
TJL_SFImageAssign(void *top, void *fromp);

extern void
TJL_SFImageSet(void *p, SV *sv);


JSBool
SFImageToString(JSContext *cx,
				JSObject *obj,
				uintN argc,
				jsval *argv,
				jsval *rval);

JSBool
SFImageAssign(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFImageTouched(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);

JSBool 
SFImageConstr(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool 
SFImageFinalize(JSContext *cx,
				JSObject *obj);

JSBool
SFImageGetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool
SFImageSetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool
SFImageSetInternal(void *cx,
				   void *globalObj,
				   char *name,
				   SV *sv);


extern void *
TJL_SFVec2fNew(void);

extern void
TJL_SFVec2fDelete(void *p);

extern void
TJL_SFVec2fAssign(void *top, void *fromp);

extern void
TJL_SFVec2fSet(void *p, SV *sv);


JSBool
SFVec2fToString(JSContext *cx,
				JSObject *obj,
				uintN argc,
				jsval *argv,
				jsval *rval);

JSBool
SFVec2fAssign(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFVec2fTouched(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);

JSBool
SFVec2fSubtract(JSContext *cx,
				JSObject *obj,
				uintN argc,
				jsval *argv,
				jsval *rval);

JSBool
SFVec2fNormalize(JSContext *cx,
				 JSObject *obj,
				 uintN argc,
				 jsval *argv,
				 jsval *rval);

JSBool
SFVec2fAdd(JSContext *cx,
		   JSObject *obj,
		   uintN argc,
		   jsval *argv,
		   jsval *rval);

JSBool
SFVec2fLength(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFVec2fNegate(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFVec2fConstr(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFVec2fFinalize(JSContext *cx,
				JSObject *obj);

JSBool 
SFVec2fGetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool 
SFVec2fSetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool
SFVec2fSetInternal(void *cx,
				   void *globalObj,
				   char *name,
				   SV *sv);


extern void *
TJL_SFVec3fNew(void);

extern void
TJL_SFVec3fDelete(void *p);

extern void
TJL_SFVec3fAssign(void *top, void *fromp);

extern void
TJL_SFVec3fSet(void *p, SV *sv);


JSBool
SFVec3fToString(JSContext *cx,
				JSObject *obj,
				uintN argc,
				jsval *argv,
				jsval *rval);

JSBool
SFVec3fAssign(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFVec3fTouched(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);

JSBool
SFVec3fSubtract(JSContext *cx,
				JSObject *obj,
				uintN argc,
				jsval *argv,
				jsval *rval);

JSBool
SFVec3fNormalize(JSContext *cx,
				 JSObject *obj,
				 uintN argc,
				 jsval *argv,
				 jsval *rval);

JSBool
SFVec3fAdd(JSContext *cx,
		   JSObject *obj,
		   uintN argc,
		   jsval *argv,
		   jsval *rval);

JSBool
SFVec3fLength(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFVec3fCross(JSContext *cx,
			 JSObject *obj,
			 uintN argc,
			 jsval *argv,
			 jsval *rval);

JSBool
SFVec3fNegate(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFVec3fConstr(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
SFVec3fFinalize(JSContext *cx,
				JSObject *obj);

JSBool 
SFVec3fGetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool 
SFVec3fSetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool
SFVec3fSetInternal(void *cx,
				   void *globalObj,
				   char *name,
				   SV *sv);

extern void *
TJL_SFRotationNew(void);

extern void
TJL_SFRotationDelete(void *p);

extern void
TJL_SFRotationAssign(void *top, void *fromp);

extern void
TJL_SFRotationSet(void *p, SV *sv);


JSBool
SFRotationToString(JSContext *cx,
				   JSObject *obj,
				   uintN argc,
				   jsval *argv,
				   jsval *rval);

JSBool
SFRotationAssign(JSContext *cx,
				 JSObject *obj,
				 uintN argc,
				 jsval *argv,
				 jsval *rval);

JSBool
SFRotationTouched(JSContext *cx,
				  JSObject *obj,
				  uintN argc,
				  jsval *argv,
				  jsval *rval);

JSBool
SFRotationMultVec(JSContext *cx,
				  JSObject *obj,
				  uintN argc,
				  jsval *argv,
				  jsval *rval);

JSBool
SFRotationInverse(JSContext *cx,
				  JSObject *obj,
				  uintN argc,
				  jsval *argv,
				  jsval *rval);

JSBool 
SFRotationConstr(JSContext *cx,
				 JSObject *obj,
				 uintN argc,
				 jsval *argv,
				 jsval *rval);

JSBool 
SFRotationFinalize(JSContext *cx,
				   JSObject *obj);

JSBool 
SFRotationGetProperty(JSContext *cx,
					  JSObject *obj,
					  jsval id,
					  jsval *vp);

JSBool 
SFRotationSetProperty(JSContext *cx,
					  JSObject *obj,
					  jsval id,
					  jsval *vp);

JSBool
SFRotationSetInternal(void *cx,
					  void *globalObj,
					  char *name,
					  SV *sv);


JSBool
MFColorConstr(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
MFColorAssign(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool 
MFColorSetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool
MFColorAddProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);


JSBool
MFVec3fConstr(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool
MFVec3fAssign(JSContext *cx,
			  JSObject *obj,
			  uintN argc,
			  jsval *argv,
			  jsval *rval);

JSBool 
MFVec3fSetProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);

JSBool
MFVec3fAddProperty(JSContext *cx,
				   JSObject *obj,
				   jsval id,
				   jsval *vp);


JSBool
MFRotationConstr(JSContext *cx,
				 JSObject *obj,
				 uintN argc,
				 jsval *argv,
				 jsval *rval);

JSBool
MFRotationAssign(JSContext *cx,
				 JSObject *obj,
				 uintN argc,
				 jsval *argv,
				 jsval *rval);

JSBool 
MFRotationSetProperty(JSContext *cx,
					  JSObject *obj,
					  jsval id,
					  jsval *vp);

JSBool
MFRotationAddProperty(JSContext *cx,
					  JSObject *obj,
					  jsval id,
					  jsval *vp);


JSBool
MFNodeConstr(JSContext *cx,
			 JSObject *obj,
			 uintN argc,
			 jsval *argv,
			 jsval *rval);

JSBool
MFNodeAssign(JSContext *cx,
			 JSObject *obj,
			 uintN argc,
			 jsval *argv,
			 jsval *rval);

JSBool 
MFNodeSetProperty(JSContext *cx,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);


JSBool
MFNodeAddProperty(JSContext *cx,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);


JSBool
MFStringConstr(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);

JSBool
MFStringAssign(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);

JSBool 
MFStringSetProperty(JSContext *cx,
					JSObject *obj,
					jsval id,
					jsval *vp);


JSBool
MFStringAddProperty(JSContext *cx,
					JSObject *obj,
					jsval id,
					jsval *vp);



/*
 * VRML Node types as JS classes:
 */

static JSObject *proto_SFNode;

static JSClass SFNodeClass = {
	"SFNode",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFNodeGetProperty,
	SFNodeSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSFunctionSpec (SFNodeFunctions)[] = {{0}};

JSObject *proto_SFRotation;


typedef struct _TJL_SFRotation {
	int touched; 
	struct SFRotation v;
} TJL_SFRotation;


static JSClass SFRotationClass = {
	"SFRotation",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFRotationGetProperty,
	SFRotationSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	SFRotationFinalize
};

static JSPropertySpec (SFRotationProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{"angle",3, JSPROP_ENUMERATE},
	{0}
};

static JSFunctionSpec (SFRotationFunctions)[] = {
	{"assign", SFRotationAssign, 0},
	{"toString", SFRotationToString, 0},
	{"__touched", SFRotationTouched, 0},
	{"multVec", SFRotationMultVec, 0},
	{"inverse", SFRotationInverse, 0},
	{0}
};


static JSObject *proto_SFColor;

typedef struct _TJL_SFColor {
	int touched; 
	struct SFColor v;
} TJL_SFColor;

static JSClass SFColorClass = {
	"SFColor",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFColorGetProperty,
	SFColorSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	SFColorFinalize
};

static JSPropertySpec (SFColorProperties)[] = {
	{"r", 0, JSPROP_ENUMERATE},
	{"g", 1, JSPROP_ENUMERATE},
	{"b", 2, JSPROP_ENUMERATE},
	{0}
};

static JSFunctionSpec (SFColorFunctions)[] = {
	{"assign", SFColorAssign, 0},
	{"toString", SFColorToString, 0},
	{"__touched", SFColorTouched, 0},
	{0}
};

static JSObject *proto_SFImage;

typedef struct _TJL_SFImage {
	int touched; 
	struct SFImage v;
} TJL_SFImage;


static JSClass SFImageClass = {
	"SFImage",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub, /* getter */
	JS_PropertyStub, /* setter */
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	SFImageFinalize
};

static JSPropertySpec (SFImageProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"depth", 2, JSPROP_ENUMERATE},
	{"data", 3, JSPROP_ENUMERATE},
	{"texture", 4, JSPROP_ENUMERATE},
	{0}
};

static JSFunctionSpec (SFImageFunctions)[] = {
	{"assign", SFImageAssign, 0},
	{"toString", SFImageToString, 0},
	{"__touched", SFImageTouched, 0},
	{0}
};


JSObject *proto_SFVec3f;

typedef struct _TJL_SFVec3f {
	int touched; 
	struct SFColor v;
} TJL_SFVec3f;

static JSClass SFVec3fClass = {
	"SFVec3f",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec3fGetProperty,
	SFVec3fSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	SFVec3fFinalize
};

static JSPropertySpec (SFVec3fProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{0}
};

static JSFunctionSpec (SFVec3fFunctions)[] = {
	{"assign", SFVec3fAssign, 0},
	{"toString", SFVec3fToString, 0},
	{"__touched", SFVec3fTouched, 0},
	{"subtract", SFVec3fSubtract, 0},
	{"normalize", SFVec3fNormalize, 0},
	{"add", SFVec3fAdd, 0},
	{"length", SFVec3fLength, 0},
	{"cross", SFVec3fCross, 0},
	{"negate", SFVec3fNegate, 0},
	{0}
};

JSObject *proto_SFVec2f;

typedef struct _TJL_SFVec2f {
	int touched; 
	struct SFVec2f v;
} TJL_SFVec2f;

static JSClass SFVec2fClass = {
	"SFVec2f",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec2fGetProperty,
	SFVec2fSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	SFVec2fFinalize
};

static JSPropertySpec (SFVec2fProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{0}
};

static JSFunctionSpec (SFVec2fFunctions)[] = {
	{"assign", SFVec2fAssign, 0},
	{"toString", SFVec2fToString, 0},
	{"__touched", SFVec2fTouched, 0},
	{"subtract", SFVec2fSubtract, 0},
	{"normalize", SFVec2fNormalize, 0},
	{"add", SFVec2fAdd, 0},
	{"length", SFVec2fLength, 0},
	{"negate", SFVec2fNegate, 0},
	{0}
};


static JSObject *proto_MFString;

static JSClass MFStringClass = {
	"MFString",
	JSCLASS_HAS_PRIVATE,
	MFStringAddProperty,
	JS_PropertyStub,
	JS_PropertyStub,
	MFStringSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSFunctionSpec (MFStringFunctions)[] = {
	{"assign", MFStringAssign, 0},
	{0}
};


static JSObject *proto_MFNode;

static JSClass MFNodeClass = {
	"MFNode",
	JSCLASS_HAS_PRIVATE,
	MFNodeAddProperty,
	JS_PropertyStub,
	JS_PropertyStub,
	MFNodeSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSFunctionSpec (MFNodeFunctions)[] = {
	{"assign", MFNodeAssign, 0},
	{0}
};


static JSObject *proto_MFRotation;

static JSClass MFRotationClass = {
	"MFRotation",
	JSCLASS_HAS_PRIVATE,
	MFRotationAddProperty,
	JS_PropertyStub,
	JS_PropertyStub,
	MFRotationSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSFunctionSpec (MFRotationFunctions)[] = {
	{"assign", MFRotationAssign, 0},
	{0}
};


static JSObject *proto_MFVec3f;

static JSClass MFVec3fClass = {
	"MFVec3f",
	JSCLASS_HAS_PRIVATE,
	MFVec3fAddProperty,
	JS_PropertyStub,
	JS_PropertyStub,
	MFVec3fSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSFunctionSpec (MFVec3fFunctions)[] = {
	{"assign", MFVec3fAssign, 0},
	{0}
};


static JSObject *proto_MFColor;

static JSClass MFColorClass = {
	"MFColor",
	JSCLASS_HAS_PRIVATE,
	MFColorAddProperty,
	JS_PropertyStub,
	JS_PropertyStub,
	MFColorSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSFunctionSpec (MFColorFunctions)[] = {
	{"assign", MFColorAssign, 0},
	{0}
};

#endif /*  __jsVRMLClasses_h__ */
