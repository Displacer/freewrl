/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 * 
 * $Id: jsVRMLBrowser.c,v 1.1.2.1 2002/08/12 21:05:01 ayla Exp $
 * 
 */

#include "jsVRMLBrowser.h"

JSBool
VRMLBrowserInit(JSContext *context, JSObject *globalObj, BrowserIntern *brow)
{
	JSObject *obj;
/* 	BrowserIntern *brow = (BrowserIntern *) JS_malloc(context, sizeof(BrowserIntern)); */
	/* brow->jssv = newSVsv(jssv); */ /* new duplicate of jssv */
/* 	brow->magic = BROWMAGIC; */


	if (verbose) {
		printf("VRMLBrowserInit\n");
	}
	/* why not JS_InitClass ??? */
	obj = JS_DefineObject(context,
						  globalObj,
						  "__Browser_proto", 
						  &Browser,
						  NULL,
						  JSPROP_ENUMERATE | JSPROP_PERMANENT);
	if (!JS_DefineFunctions(context, obj, BrowserMethods)) {
		return JS_FALSE;
	}

	/* JS_SetContextPrivate(context, brow); ??? */

	/* keeps pointer or makes copy ??? */
	if (!JS_SetPrivate(context, obj, brow)) {
		return JS_FALSE;
	}
	return JS_TRUE;
}


JSBool
VRMLBrowserCreateVRMLFromString(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int count;
/* 	SV *sv; */
	jsval v;
	unsigned int i;
	BrowserIntern *brow;
	if ((brow = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in VRMLBrowserCreateVRMLFromString.\n");
		return JS_FALSE;
	}
	
	UNUSED(count);
/* 	UNUSED(sv); */

	if (brow->magic != BROWMAGIC) {
		fprintf(stderr, "Wrong browser magic!\n");
		return JS_FALSE;
	}
	if (argc != 1) {
		fprintf(stderr, "Invalid number of arguments for browser method!\n");
		return JS_FALSE;
	}

	for (i = 0; i < argc; i++) {
		char buffer[SMALLSTRING];
		sprintf(buffer, "__arg%d",  i);
		JS_SetProperty(context, obj, buffer, argv+i);
	}

	doPerlCallMethod(brow->jssv, "browserCreateVrmlFromString");
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_createVrmlFromString",  G_SCALAR); */
/* 		if(count) { */
/* 			if(verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */
	

	if (!JS_GetProperty(context, obj, "__bret",  &v)) {
		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserCreateVRMLFromString.\n");
		/* exit(1); */
	}
	*rval = v;
	return JS_TRUE;
}

#if FALSE
/* JSBool */
/* VRMLBrowserSetDescription(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval) */
/* { */
/* 	int count; */
/* 	SV *sv; */
/* 	jsval v; */
/* 	unsigned int i; */
/* 	BrowserIntern *brow = JS_GetPrivate(context, obj); */
/* 	UNUSED(count); */
/* 	UNUSED(sv); */

/* 	if (!brow) { */
/* 		return JS_FALSE; */
/* 	} */
	

/* 	if (brow->magic != BROWMAGIC) { */
/* 		fprintf(stderr, "Wrong browser magic!\n"); */
/* 	} */
/* 	if (argc != 1) { */
/* 		fprintf(stderr, "Invalid number of arguments for browser method!\n"); */
/* 	} */
/* 	for (i = 0; i < argc; i++) { */
/* 		char buffer[SMALLSTRING]; */
/* 		sprintf(buffer,"__arg%d", i); */
/* 		JS_SetProperty(context, obj, buffer, argv+i); */
/* 	} */
/* 	doPerlCallMethod(brow->jssv, "browserSetDescription"); */
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_setDescription",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */

/* 	if (!JS_GetProperty(context, obj, "__bret",  &v)) { */
/* 		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserSetDescription.\n"); */
		/* exit(1); */
/* 	} */
	
/* 	*rval = v; */
/* 	return JS_TRUE; */
/* } */
#endif /* FALSE */

		
JSBool
VRMLBrowserGetName(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int count;
/* 	SV *sv; */
	jsval v;
	unsigned int i;
	BrowserIntern *brow;
	if ((brow = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in VRMLBrowserGetName.\n");
		return JS_FALSE;
	}
	
	UNUSED(count);
/* 	UNUSED(sv); */

	if (brow->magic != BROWMAGIC) {
		fprintf(stderr, "Wrong browser magic!\n");
		return JS_FALSE;
	}

	if (argc != 0) {
		fprintf(stderr, "Invalid number of arguments for browser method!\n");
		return JS_FALSE;
	}
	for (i = 0; i < argc; i++) {
		char buffer[SMALLSTRING];
		sprintf(buffer,"__arg%d", i);
		JS_SetProperty(context, obj, buffer, argv+i);
	}

	doPerlCallMethod(brow->jssv, "browserGetName");
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_getName",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */

	if (!JS_GetProperty(context, obj, "__bret",  &v)) {
		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserGetName.\n");
		/* exit(1); */
	}
	
	*rval = v;
	return JS_TRUE;
}


#if FALSE		
/* JSBool */
/* VRMLBrowserDeleteRoute(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval) */
/* { */
/* 	int count; */
/* 	SV *sv; */
/* 	jsval v; */
/* 	unsigned int i; */
/* 	BrowserIntern *brow = JS_GetPrivate(context, obj); */
/* 	UNUSED(count); */
/* 	UNUSED(sv); */

/* 	if (!brow) { */
/* 		return JS_FALSE; */
/* 	} */
	
/* 	if (brow->magic != BROWMAGIC) { */
/* 		fprintf(stderr, "Wrong browser magic!\n"); */
/* 	} */
/* 	if (argc != 4) { */
/* 		fprintf(stderr, "Invalid number of arguments for browser method!\n"); */
/* 	} */
/* 	for (i = 0; i < argc; i++) { */
/* 		char buffer[SMALLSTRING]; */
/* 		sprintf(buffer,"__arg%d", i); */
/* 		JS_SetProperty(context, obj, buffer, argv+i); */
/* 	} */
/* 	doPerlCallMethod(brow->jssv, ""); */
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_deleteRoute",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */

/* 	if (!JS_GetProperty(context, obj, "__bret", &v)) { */
/* 		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserDeleteRoute.\n"); */
		/* exit(1); */
/* 	} */
	
/* 	*rval = v; */
/* 	return JS_TRUE; */
/* } */
#endif /* FALSE */

		
#if FALSE		
/* JSBool */
/* VRMLBrowserLoadURL(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval) */
/* { */
/* 	int count; */
/* 	SV *sv; */
/* 	jsval v; */
/* 	unsigned int i; */
/* 	BrowserIntern *brow = JS_GetPrivate(context, obj); */
/* 	UNUSED(count); */
/* 	UNUSED(sv); */

/* 	if (!brow) { */
/* 		return JS_FALSE; */
/* 	} */
	
/* 	if (brow->magic != BROWMAGIC) { */
/* 		fprintf(stderr, "Wrong browser magic!\n"); */
/* 	} */
/* 	if (argc != 2) { */
/* 		fprintf(stderr, "Invalid number of arguments for browser method!\n"); */
/* 	} */
/* 	for (i = 0; i < argc; i++) { */
/* 		char buffer[SMALLSTRING]; */
/* 		sprintf(buffer,"__arg%d", i); */
/* 		JS_SetProperty(context, obj, buffer, argv+i); */
/* 	} */
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_loadURL",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */

/* 	if (!JS_GetProperty(context, obj, "__bret", &v)) { */
/* 		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserLoadURL.\n"); */
		/* exit(1); */
/* 	} */
	
/* 	*rval = v; */
/* 	return JS_TRUE; */
/* } */
#endif /* FALSE */

		
#if FALSE		
/* JSBool */
/* VRMLBrowserReplaceWorld(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval) */
/* { */
/* 	int count; */
/* 	SV *sv; */
/* 	jsval v; */
/* 	unsigned int i; */
/* 	BrowserIntern *brow = JS_GetPrivate(context, obj); */
/* 	UNUSED(count); */
/* 	UNUSED(sv); */

/* 	if (!brow) { */
/* 		return JS_FALSE; */
/* 	} */
	
/* 	if (brow->magic != BROWMAGIC) { */
/* 		fprintf(stderr, "Wrong browser magic!\n"); */
/* 	} */
/* 	if (argc != 1) { */
/* 		fprintf(stderr, "Invalid number of arguments for browser method!\n"); */
/* 	} */
/* 	for (i = 0; i < argc; i++) { */
/* 		char buffer[SMALLSTRING]; */
/* 		sprintf(buffer,"__arg%d", i); */
/* 		JS_SetProperty(context, obj, buffer, argv+i); */
/* 	} */
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_replaceWorld",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */


/* 	if (!JS_GetProperty(context, obj, "__bret",  &v)) { */
/* 		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserReplaceWorld.\n"); */
		/* exit(1); */
/* 	} */
/* 	*rval = v; */
/* 	return JS_TRUE; */
/* } */
#endif /* FALSE */

	
#if FALSE		
/* JSBool */
/* VRMLBrowserGetCurrentSpeed(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval) */
/* { */
/* 	int count; */
/* 	SV *sv; */
/* 	jsval v; */
/* 	unsigned int i; */
/* 	BrowserIntern *brow = JS_GetPrivate(context, obj); */
/* 	UNUSED(count); */
/* 	UNUSED(sv); */

/* 	if (!brow) { */
/* 		return JS_FALSE; */
/* 	} */
	
/* 	if (brow->magic != BROWMAGIC) { */
/* 		fprintf(stderr, "Wrong browser magic!\n"); */
/* 	} */
/* 	if (argc != 0) { */
/* 		fprintf(stderr, "Invalid number of arguments for browser method!\n"); */
/* 	} */
/* 	for (i = 0; i < argc; i++) { */
/* 		char buffer[SMALLSTRING]; */
/* 		sprintf(buffer,"__arg%d", i); */
/* 		JS_SetProperty(context, obj, buffer, argv+i); */
/* 	} */
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_getCurrentSpeed",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */


/* 	if (!JS_GetProperty(context, obj, "__bret",  &v)) { */
/* 		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserGetCurrentSpeed.\n"); */
		/* exit(1); */
/* 	} */

/* 	*rval = v; */
/* 	return JS_TRUE; */
/* } */
#endif /* FALSE */

		
JSBool
VRMLBrowserGetVersion(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int count;
/* 	SV *sv; */
	jsval v;
	unsigned int i;
	BrowserIntern *brow;
	if ((brow = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in VRMLBrowserGetVersion.\n");
		return JS_FALSE;
	}
	
	UNUSED(count);
/* 	UNUSED(sv); */

	if (brow->magic != BROWMAGIC) {
		fprintf(stderr, "Wrong browser magic!\n");
		return JS_FALSE;
	}
	if (argc != 0) {
		fprintf(stderr, "Invalid number of arguments for browser method!\n");
		return JS_FALSE;
	}
	for (i = 0; i < argc; i++) {
		char buffer[SMALLSTRING];
		sprintf(buffer,"__arg%d", i);
		JS_SetProperty(context, obj, buffer, argv+i);
	}

	doPerlCallMethod(brow->jssv, "browserGetVersion");
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_getVersion",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */


	if (!JS_GetProperty(context, obj, "__bret",  &v)) {
		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserGetVersion.\n");
		/* exit(1); */
	}
	*rval = v;
	return JS_TRUE;
}

	
#if FALSE		
/* JSBool */
/* VRMLBrowserAddRoute(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval) */
/* { */
/* 	int count; */
/* 	SV *sv; */
/* 	jsval v; */
/* 	unsigned int i; */
/* 	BrowserIntern *brow = JS_GetPrivate(context, obj); */
/* 	UNUSED(count); */
/* 	UNUSED(sv); */

/* 	if (!brow) { */
/* 		return JS_FALSE; */
/* 	} */
	
/* 	if (brow->magic != BROWMAGIC) { */
/* 		fprintf(stderr, "Wrong browser magic!\n"); */
/* 	} */
/* 	if (argc != 4) { */
/* 		fprintf(stderr, "Invalid number of arguments for browser method!\n"); */
/* 	} */
/* 	for (i = 0; i < argc; i++) { */
/* 		char buffer[SMALLSTRING]; */
/* 		sprintf(buffer,"__arg%d", i); */
/* 		JS_SetProperty(context, obj, buffer, argv+i); */
/* 	} */
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */

/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_addRoute",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */

/* 	if (!JS_GetProperty(context, obj, "__bret",  &v)) { */
/* 		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserAddRoute.\n"); */
		/* exit(1); */
/* 	} */
/* 	*rval = v; */
/* 	return JS_TRUE; */
/* } */
#endif /* FALSE */

	
JSBool
VRMLBrowserGetCurrentFrameRate(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int count;
/* 	SV *sv; */
	jsval v;
	unsigned int i;
	BrowserIntern *brow;
	if ((brow = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in JS_RMLBrowserGetCurrentFrameRate.\n");
		return JS_FALSE;
	}
	
	UNUSED(count);
/* 	UNUSED(sv); */
	
	if (brow->magic != BROWMAGIC) {
		fprintf(stderr, "Wrong browser magic!\n");
		return JS_FALSE;
	}
	if (argc != 0) {
		fprintf(stderr, "Invalid number of arguments for browser method!\n");
		return JS_FALSE;
	}
	for (i = 0; i < argc; i++) {
		char buffer[SMALLSTRING];
		sprintf(buffer,"__arg%d", i);
		JS_SetProperty(context, obj, buffer, argv+i);
	}

	doPerlCallMethod(brow->jssv, "browserGetCurrentFrameRate");
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */


/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_getCurrentFrameRate",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */


	if (!JS_GetProperty(context, obj, "__bret",  &v)) {
		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserGetCurrentFrameRate.\n");
		/* exit(1); */
	}
	*rval = v;
	return JS_TRUE;
}

	
JSBool
VRMLBrowserGetWorldURL(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int count;
/* 	SV *sv; */
	jsval v;
	unsigned int i;
	BrowserIntern *brow;
	if ((brow = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in VRMLBrowserGetWorldURL.\n");
		return JS_FALSE;
	}
	
	UNUSED(count);
/* 	UNUSED(sv); */

	if (brow->magic != BROWMAGIC) {
		fprintf(stderr, "Wrong browser magic!\n");
		return JS_FALSE;
	}

	if (argc != 0) {
		fprintf(stderr, "Invalid number of arguments for browser method!\n");
		return JS_FALSE;
	}
	for (i = 0; i < argc; i++) {
		char buffer[SMALLSTRING];
		sprintf(buffer,"__arg%d", i);
		JS_SetProperty(context, obj, buffer, argv+i);
	}

	doPerlCallMethod(brow->jssv, "browserGetWorldURL");
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */


/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_getWorldURL",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */


	if (!JS_GetProperty(context, obj, "__bret",  &v)) {
		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserGetWorldURL.\n");
		/* exit(1); */
	}

	*rval = v;
	return JS_TRUE;
}

	
JSBool
VRMLBrowserCreateVRMLFromURL(JSContext *context, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int count;
/* 	SV *sv; */
	jsval v;
	unsigned int i;
	BrowserIntern *brow;
	if ((brow = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in VRMLBrowserCreateVRMLFromURL.\n");
		return JS_FALSE;
	}
	
	UNUSED(count);
/* 	UNUSED(sv); */

	if (brow->magic != BROWMAGIC) {
		fprintf(stderr, "Wrong browser magic!\n");
		return JS_FALSE;
	}

	if (argc != 3) {
		fprintf(stderr, "Invalid number of arguments for browser method!\n");
		return JS_FALSE;
	}
	for (i = 0; i < argc; i++) {
		char buffer[SMALLSTRING];
		sprintf(buffer,"__arg%d", i);
		JS_SetProperty(context, obj, buffer, argv+i);
	}

	doPerlCallMethod(brow->jssv, "browserCreateVrmlFromURL");
/* 	if (verbose) { */
/* 		printf("Calling method with sv %u (%s)\n", (unsigned int) brow->jssv, SvPV(brow->jssv, PL_na)); */
/* 	} */


/* 	{ */
/* 		dSP; */
/* 		ENTER; */
/* 		SAVETMPS; */
/* 		PUSHMARK(sp); */
/* 		XPUSHs(brow->jssv); */
/* 		PUTBACK; */
/* 		count = perl_call_method("brow_createVRMLFromURL",  G_SCALAR); */
/* 		if (count) { */
/* 			if (verbose) printf("Got return %f\n", POPn); */
/* 		} */
/* 		PUTBACK; */
/* 		FREETMPS; */
/* 		LEAVE; */
/* 	} */


	if (!JS_GetProperty(context, obj, "__bret",  &v)) {
		fprintf(stderr, "JS_GetProperty failed in VRMLBrowserCreateVRMLFromURL.\n");
		/* exit(1); */
	}

	*rval = v;
	return JS_TRUE;
}

