/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 * 
 * $Id: jsVRMLClasses.h,v 1.1.2.1 2002/08/12 21:05:01 ayla Exp $
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

#define INIT_ARGC 3


#define AVECLEN(x) (sqrt((x)[0]*(x)[0]+(x)[1]*(x)[1]+(x)[2]*(x)[2]))
#define AVECPT(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define AVECSCALE(x,y) x[0] *= y;\
x[1] *= y;\
x[2] *= y;
#define AVECCP(x,y,z) (z)[0]=(x)[1]*(y)[2]-(x)[2]*(y)[1]; \
(z)[1]=(x)[2]*(y)[0]-(x)[0]*(y)[2]; \
(z)[2]=(x)[0]*(y)[1]-(x)[1]*(y)[0];



/* helper functions */


static JSBool
doMFAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);

static JSBool
doMFSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp);


/* class functions */

JSBool
LoadVRMLClasses(JSContext *context,
				JSObject *globalObj);

JSBool
SetTouchable(JSContext *cx,
			 JSObject *obj,
			 jsval id,
			 jsval *vp);

JSBool
AddAssignProperties(void *cx,
					void *globalObj,
					char *name,
					char *str);


JSBool
AddWatchProperties(void *cx,
				   void *globalObj,
				   char *name);


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
TJL_SFColorSet(void *p, void *sv);

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
SFColorGetPrivate(JSContext *cx,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);

JSBool
SFColorSetPrivate(JSContext *cx,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);

JSBool
SFColorSetProperty(void *cx,
				   void *globalObj,
				   char *name,
				   void *sv);


extern void *
TJL_SFVec3fNew(void);

extern void
TJL_SFVec3fDelete(void *p);

extern void
TJL_SFVec3fAssign(void *top, void *fromp);

extern void
TJL_SFVec3fSet(void *p, void *sv);

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
SFVec3fGetPrivate(JSContext *cx,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);

JSBool 
SFVec3fSetPrivate(JSContext *cx,
				  JSObject *obj,
				  jsval id,
				  jsval *vp);

JSBool
SFVec3fSetProperty(void *cx,
				   void *globalObj,
				   char *name,
				   void *sv);

extern void *
TJL_SFRotationNew(void);

extern void
TJL_SFRotationDelete(void *p);

extern void
TJL_SFRotationAssign(void *top, void *fromp);

extern void
TJL_SFRotationSet(void *p, void *sv);


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
SFRotationGetPrivate(JSContext *cx,
					 JSObject *obj,
					 jsval id,
					 jsval *vp);

JSBool 
SFRotationSetPrivate(JSContext *cx,
					 JSObject *obj,
					 jsval id,
					 jsval *vp);

JSBool
SFRotationSetProperty(void *cx,
					  void *globalObj,
					  char *name,
					  void *sv);


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

static JSFunctionSpec (SFNodeMethods)[] = {{0}};

static JSObject *proto_SFRotation;

/* typedef struct _SFRotationIntern { */
/* 	float r[4]; */
/* } SFRotationIntern; */

typedef struct _TJL_SFRotation {
	int touched; 
	struct SFRotation v;
} TJL_SFRotation;


static JSClass SFRotationClass = {
	"SFRotation",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFRotationGetPrivate,
	SFRotationSetPrivate,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSPropertySpec (SFRotationProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{"angle",3, JSPROP_ENUMERATE},
	{0}
};

static JSFunctionSpec (SFRotationMethods)[] = {
	{"assign", SFRotationAssign, 0},
	{"toString", SFRotationToString, 0},
	{"__touched", SFRotationTouched, 0},
	{"multVec", SFRotationMultVec, 0},
	{"inverse", SFRotationInverse, 0},
	{0}
};


static JSObject *proto_SFColor;

/* typedef struct _SFColorIntern { */
/* 	float c[3]; */
/* } SFColorIntern; */

typedef struct _TJL_SFColor {
	int touched; 
	struct SFColor v;
} TJL_SFColor;

static JSClass SFColorClass = {
	"SFColor",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFColorGetPrivate,
	SFColorSetPrivate,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSPropertySpec (SFColorProperties)[] = {
	{"r", 0, JSPROP_ENUMERATE},
	{"g", 1, JSPROP_ENUMERATE},
	{"b", 2, JSPROP_ENUMERATE},
	{0}
};

static JSFunctionSpec (SFColorMethods)[] = {
	{"assign", SFColorAssign, 0},
	{"toString", SFColorToString, 0},
	{"__touched", SFColorTouched, 0},
	{0}
};


static JSObject *proto_SFVec3f;

typedef struct _TJL_SFVec3f {
	int touched; 
	struct SFColor v;
} TJL_SFVec3f;

static JSClass SFVec3fClass = {
	"SFVec3f",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec3fGetPrivate,
	SFVec3fSetPrivate,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

static JSPropertySpec (SFVec3fProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{0}
};

static JSFunctionSpec (SFVec3fMethods)[] = {
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

static JSFunctionSpec (MFStringMethods)[] = {
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

static JSFunctionSpec (MFNodeMethods)[] = {
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

static JSFunctionSpec (MFRotationMethods)[] = {
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

static JSFunctionSpec (MFVec3fMethods)[] = {
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

static JSFunctionSpec (MFColorMethods)[] = {
	{"assign", MFColorAssign, 0},
	{0}
};

#endif /*  __jsVRMLClasses_h__ */
