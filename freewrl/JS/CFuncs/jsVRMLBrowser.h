/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart, Ayla Khan CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 * 
 * $Id: jsVRMLBrowser.h,v 1.3 2002/11/21 16:33:44 ayla Exp $
 * 
 */


#ifndef __jsVRMLBrowser_h__
#define __jsVRMLBrowser_h__

#ifndef __jsUtils_h__
#include "jsUtils.h"
#endif /* __jsUtils_h__ */


/*
 * for now, could try setting magic to pid since we only need one browser
 * per process -- but is it really needed ???
 */
typedef struct _BrowserNative {
	int magic; /* does this really do anything ??? */
	SV *sv_js;
} BrowserNative;


static JSBool
doVRMLRoute(JSContext *context,
			JSObject *obj,
			uintN argc,
			jsval *argv,
			const char *callingFunc,
			const char *perlBrowserFunc,
			const char *browserFunc);

JSBool
VrmlBrowserInit(JSContext *context,
			JSObject *globalObj, 
			BrowserNative *brow);


JSBool
VrmlBrowserGetVersion(JSContext *cx,
				  JSObject *obj,
				  uintN argc,
				  jsval *argv,
				  jsval *rval);


JSBool
VrmlBrowserCreateVrmlFromString(JSContext *cx,
							JSObject *obj,
							uintN argc,
							jsval *argv,
							jsval *rval);


#if FALSE
/* JSBool */
/* VrmlBrowserSetDescription(JSContext *cx, */
/* 					  JSObject *obj, */
/* 					  uintN argc, */
/* 					  jsval *argv, */
/* 					  jsval *rval); */
#endif

		
JSBool
VrmlBrowserGetName(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);

	
JSBool
VrmlBrowserAddRoute(JSContext *cx,
				JSObject *obj,
				uintN argc,
				jsval *argv,
				jsval *rval);


JSBool
VrmlBrowserDeleteRoute(JSContext *cx,
				   JSObject *obj,
				   uintN argc,
				   jsval *argv,
				   jsval *rval);

	
JSBool
VrmlBrowserCreateVrmlFromURL(JSContext *cx,
						 JSObject *obj,
						 uintN argc,
						 jsval *argv,
						 jsval *rval);


JSBool
VrmlBrowserLoadURL(JSContext *cx,
			   JSObject *obj,
			   uintN argc,
			   jsval *argv,
			   jsval *rval);


JSBool
VrmlBrowserGetWorldURL(JSContext *cx,
				   JSObject *obj,
				   uintN argc,
				   jsval *argv,
				   jsval *rval);


#if FALSE		
/* JSBool */
/* VrmlBrowserReplaceWorld(JSContext *cx, */
/* 					JSObject *obj, */
/* 					uintN argc, */
/* 					jsval *argv, */
/* 					jsval *rval); */
#endif /* FALSE */


#if FALSE		
/* JSBool */
/* VrmlBrowserGetCurrentSpeed(JSContext *cx, */
/* 					   JSObject *obj, */
/* 					   uintN argc, */
/* 					   jsval *argv, */
/* 					   jsval *rval); */
#endif /* FALSE */


JSBool
VrmlBrowserGetCurrentFrameRate(JSContext *cx,
						   JSObject *obj,
						   uintN argc,
						   jsval *argv,
						   jsval *rval);


#if FALSE		
/* JSBool */
/* VrmlBrowserSetDescription(JSContext *cx, */
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
	{"createVrmlFromString", VrmlBrowserCreateVrmlFromString, 0},
	{"createVrmlFromURL", VrmlBrowserCreateVrmlFromURL, 0},
	{"getName", VrmlBrowserGetName, 0},
	{"getVersion", VrmlBrowserGetVersion, 0},
	{"getCurrentFrameRate", VrmlBrowserGetCurrentFrameRate, 0},
	{"getWorldURL", VrmlBrowserGetWorldURL, 0},
	{"addRoute", VrmlBrowserAddRoute, 0},
	{"deleteRoute", VrmlBrowserDeleteRoute, 0},
/* 	{"setDescription", VrmlBrowserSetDescription, 0}, */
/* 	{"loadURL", VrmlBrowserLoadURL, 0}, */
/* 	{"replaceWorld", VrmlBrowserReplaceWorld, 0}, */
/* 	{"getCurrentSpeed", VrmlBrowserGetCurrentSpeed, 0}, */
	{0}
};


#endif /* __jsVRMLBrowser_h__ */
