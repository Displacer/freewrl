/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 * 
 * $Id: jsVRMLBrowser.h,v 1.1.2.2 2002/08/20 21:35:23 ayla Exp $
 * 
 */


#ifndef __jsVRMLBrowser_h__
#define __jsVRMLBrowser_h__

#ifndef __jsUtils_h__
#include "jsUtils.h"
#endif /* __jsUtils_h__ */



/*
 * for now, set magic to pid since we only need one browser per
 * process -- but is it really needed ???
 */
typedef struct _BrowserInternal {
	int magic; /* does this really do anything ??? */
	SV *jssv;
} BrowserInternal;



/* JSBool */
/* browserInit(JSContext *context, */
/* 			JSObject *globalObj,  */
/* 			SV *jssv); */



JSBool
VRMLBrowserInit(JSContext *context,
			JSObject *globalObj, 
			BrowserInternal *brow);


JSBool
VRMLBrowserGetVersion(JSContext *cx,
				  JSObject *obj,
				  uintN argc,
				  jsval *argv,
				  jsval *rval);


JSBool
VRMLBrowserCreateVRMLFromString(JSContext *cx,
							JSObject *obj,
							uintN argc,
							jsval *argv,
							jsval *rval);


#if FALSE
/* JSBool */
/* VRMLBrowserSetDescription(JSContext *cx, */
/* 					  JSObject *obj, */
/* 					  uintN argc, */
/* 					  jsval *argv, */
/* 					  jsval *rval); */
#endif

		
JSBool
VRMLBrowserGetName(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);


	
#if FALSE		
/* JSBool */
/* VRMLBrowserAddRoute(JSContext *cx, */
/* 				JSObject *obj, */
/* 				uintN argc, */
/* 				jsval *argv, */
/* 				jsval *rval); */
#endif /* FALSE */


#if FALSE		
/* JSBool */
/* VRMLBrowserDeleteRoute(JSContext *cx, */
/* 				   JSObject *obj, */
/* 				   uintN argc, */
/* 				   jsval *argv, */
/* 				   jsval *rval); */
#endif /* FALSE */

	
JSBool
VRMLBrowserCreateVRMLFromURL(JSContext *cx,
						 JSObject *obj,
						 uintN argc,
						 jsval *argv,
						 jsval *rval);


JSBool
VRMLBrowserLoadURL(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);


JSBool
VRMLBrowserGetWorldURL(JSContext *cx,
				   JSObject *obj,
				   uintN argc,
				   jsval *argv,
				   jsval *rval);


#if FALSE		
/* JSBool */
/* VRMLBrowserReplaceWorld(JSContext *cx, */
/* 					JSObject *obj, */
/* 					uintN argc, */
/* 					jsval *argv, */
/* 					jsval *rval); */
#endif /* FALSE */


#if FALSE		
/* JSBool */
/* VRMLBrowserGetCurrentSpeed(JSContext *cx, */
/* 					   JSObject *obj, */
/* 					   uintN argc, */
/* 					   jsval *argv, */
/* 					   jsval *rval); */
#endif /* FALSE */


JSBool
VRMLBrowserGetCurrentFrameRate(JSContext *cx,
						   JSObject *obj,
						   uintN argc,
						   jsval *argv,
						   jsval *rval);


#if FALSE		
/* JSBool */
/* VRMLBrowserSetDescription(JSContext *cx, */
/* 					  JSObject *obj, */
/* 					  uintN argc, */
/* 					  jsval *argv, */
/* 					  jsval *rval); */
#endif /* FALSE */



static JSClass Browser = {
	"Browser",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};


static JSFunctionSpec (BrowserFunctions)[] = {
	{"createVRMLFromString", VRMLBrowserCreateVRMLFromString, 0},
	{"getName", VRMLBrowserGetName, 0},
	{"getVersion", VRMLBrowserGetVersion, 0},
	{"getCurrentFrameRate", VRMLBrowserGetCurrentFrameRate, 0},
	{"getWorldURL", VRMLBrowserGetWorldURL, 0},
	{"createVRMLFromURL", VRMLBrowserCreateVRMLFromURL, 0},
/* 	{"setDescription", VRMLBrowserSetDescription, 0}, */
/* 	{"deleteRoute", VRMLBrowserDeleteRoute, 0}, */
/* 	{"loadURL", VRMLBrowserLoadURL, 0}, */
/* 	{"replaceWorld", VRMLBrowserReplaceWorld, 0}, */
/* 	{"getCurrentSpeed", VRMLBrowserGetCurrentSpeed, 0}, */
/* 	{"addRoute", VRMLBrowserAddRoute, 0}, */
	{0}
};


#endif /* __jsVRMLBrowser_h__ */
