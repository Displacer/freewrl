/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 * 
 * $Id: JS.xs,v 1.3.2.3 2002/08/20 21:35:23 ayla Exp $
 * 
 * A substantial amount of code has been adapted from the embedding
 * tutorials from the SpiderMonkey web pages
 * (http://www.mozilla.org/js/spidermonkey/)
 * and from js/src/js.c, which is the sample application included with
 * the javascript engine.
 *
 */

#include <EXTERN.h>
#include <perl.h>
#include "XSUB.h"

#ifndef __jsUtils_h__
#include "jsUtils.h" /* misc helper C functions and globals */
#endif

#ifndef __jsVRMLBrowser_h__
#include "jsVRMLBrowser.h" /* */
#endif

#include "jsVRMLClasses.h" /* */


/* #define MAX_RUNTIME_BYTES 0x100000UL */
#define MAX_RUNTIME_BYTES 0xF4240L
#define STACK_CHUNK_SIZE 0x2000L



/*
 * Global JS variables (from Brendan Eich's short embedding tutorial):
 *
 * JSRuntime       - 1 runtime per process
 * JSContext       - 1 context per thread
 * global JSObject - 1 global object per context
 *
 * struct JSClass {
 *     char *name;
 *     uint32 flags;
 * Mandatory non-null function pointer members:
 *     JSPropertyOp addProperty;
 *     JSPropertyOp delProperty;
 *     JSPropertyOp getProperty;
 *     JSPropertyOp setProperty;
 *     JSEnumerateOp enumerate;
 *     JSResolveOp resolve;
 *     JSConvertOp convert;
 *     JSFinalizeOp finalize;
 * Optionally non-null members start here:
 *     JSGetObjectOps getObjectOps;
 *     JSCheckAccessOp checkAccess;
 *     JSNative call;
 *     JSNative construct;
 *     JSXDRObjectOp xdrObject;
 *     JSHasInstanceOp hasInstance;
 *     prword spare[2];
 * };
 * 
 * global JSClass  - populated by stubs
 * 
 */

JSBool verbose = 0;

static JSRuntime *runtime;
/* static BrowserInternal *brow; */
/* static JSContext *context; */
/* static JSObject *global; */
static JSClass globalClass = {
	"global",
	0,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_PropertyStub,
	JS_EnumerateStub,
	globalResolve,
	JS_ConvertStub,
	JS_FinalizeStub
};

void
doPerlCallMethod(SV *jssv, const char *methodName)
{
	int count = 0;

	dSP;
	ENTER;
	SAVETMPS;
	PUSHMARK(sp);
	XPUSHs(jssv);
	PUTBACK;
	count = perl_call_method(methodName, G_SCALAR);
	if (count && verbose) {
		printf("perl_call_method returned %f in doPerlCallMethod.\n", POPn);
	}
	PUTBACK;
	FREETMPS;
	LEAVE;
}

/* double runscript(void *cxp, void *glo, char *script, SV*r) */
/* JSBool */
/* runScript(void *cx, void *obj, char *script, SV *rstr, SV *rnum) */
/* { */
/* 	jsval rval; */
/* 	JSString *strval; */
/* 	jsdouble dval = -1.0; */
/* 	char *strp; */
/* 	size_t len; */
/* 	JSObject *globalObj = obj; */
/* 	JSContext *context = cx; */

/* 	if (verbose) { */
/* 		printf("runScript: \"%s\"\n", script); */
/* 	} */
/* 	len = strlen(script); */
/* 	if (!JS_EvaluateScript(context, globalObj, script, len, */
/* 						   FNAME_STUB, LINENO_STUB, &rval)) { */
/* 		fprintf(stderr, "JS_EvaluateScript failed for \"%s\".\n", script); */
/* 		return JS_FALSE; */
/* 	} */
/* 	strval = JS_ValueToString(context, rval); */
/* 	strp = JS_GetStringBytes(strval); */
/* 	sv_setpv(rstr, strp); */
/* 	printf("runscript: strp = %s\n", strp); */

/* 	if (!JS_ValueToNumber(context, rval, &dval)) { */
/* 		fprintf(stderr, "JS_ValueToNumber failed.\n"); */
/* 		return JS_FALSE; */
/* 	} */
/* 	printf("runscript: dval = %f\n", dval); */
/* 	sv_setnv(rnum, dval); */

/* 	return JS_TRUE; */
/* } */


void *
TJL_SFColorNew()
{
	TJL_SFColor *ptr;
	ptr = malloc(sizeof(*ptr));
	if (ptr == NULL) {
		return NULL;
	}
	ptr->touched = 0;
	return ptr;
}

void
TJL_SFColorDelete(void *p)
{
	TJL_SFColor *ptr;
	if (p != NULL) {
		ptr = p;
		free(ptr);
	}
}

void
TJL_SFColorAssign(void *top, void *fromp)
{
	TJL_SFColor *to = top;
	TJL_SFColor *from = fromp;
	to->touched++;
	(to->v) = (from->v);
}

void
TJL_SFColorSet(void *p, SV *sv)
{
	AV *a;
	SV **b;
	int i;
	TJL_SFColor *ptr = p;
	ptr->touched = 0;

	if (verbose) {
		printf("TJL_SFColorSet:\n");
	}

	if (!SvROK(sv)) {
		(ptr->v).c[0] = 0;
		(ptr->v).c[1] = 0;
		(ptr->v).c[2] = 0;
		if (verbose) {
			printf("\thardcoded color values\n");
		}

	} else {
		if (SvTYPE(SvRV(sv)) != SVt_PVAV) {
			fprintf(stderr, "SFColor without being arrayref in TJL_SFColorSet.\n");
			return;
		}
		a = (AV *) SvRV(sv);
		for (i = 0; i < 3; i++) {
			b = av_fetch(a, i, 1); /* LVal for easiness */
			if (!b) {
				fprintf(stderr, "SFColor b == 0 in TJL_SFColorSet.\n");
				return;
			}
			(ptr->v).c[i] = SvNV(*b);
		}
	}

}


void *
TJL_SFRotationNew()
{
	TJL_SFRotation *ptr;
	ptr = malloc(sizeof(*ptr));
	if (ptr == NULL) {
		return NULL;
	}
	ptr->touched = 0;
	return ptr;
}

void
TJL_SFRotationDelete(void *p)
{
	TJL_SFRotation *ptr;
	if (p != NULL) {
		ptr = p;
		free(ptr);
	}
}

void
TJL_SFRotationAssign(void *top, void *fromp)
{
	TJL_SFRotation *to = top;
	TJL_SFRotation *from = fromp;
	to->touched++;
	(to->v) = (from->v);
}

void
TJL_SFRotationSet(void *p, SV *sv)
{
	AV *a;
	SV **b;
	int i;
	TJL_SFRotation *ptr = p;
	ptr->touched = 0;

	if (verbose) {
		printf("TJL_SFRotationSet:\n");
	}

	if (!SvROK(sv)) {
		if (verbose) {
			printf("\thardcoded rotation values\n");
		}

		(ptr->v).r[0] = 0;
		(ptr->v).r[1] = 1;
		(ptr->v).r[2] = 0;
		(ptr->v).r[3] = 0;
	} else {
		if (SvTYPE(SvRV(sv)) != SVt_PVAV) {
			fprintf(stderr, "SFRotation without being arrayref in TJL_SFRotationSet.\n");
			return;
		}
		a = (AV *) SvRV(sv);
		for (i = 0; i < 4; i++) {
			b = av_fetch(a, i, 1); /* LVal for easiness */
			if (!b) {
				fprintf(stderr, "SFRotation b == 0 in TJL_SFRotationSet.\n");
				return;
			}
			(ptr->v).r[i] = SvNV(*b);
		}
	}
}

void *
TJL_SFVec3fNew()
{
	TJL_SFVec3f *ptr;
	ptr = malloc(sizeof(*ptr));
	if (ptr == NULL) {
		return NULL;
	}
	ptr->touched = 0;
	return ptr;
}

void
TJL_SFVec3fDelete(void *p)
{
	TJL_SFVec3f *ptr;
	if (p != NULL) {
		ptr = p;
		free(ptr);
	}
}

void
TJL_SFVec3fAssign(void *top, void *fromp)
{
	TJL_SFVec3f *to = top;
	TJL_SFVec3f *from = fromp;
	to->touched++;
	(to->v) = (from->v);
}

void
TJL_SFVec3fSet(void *p, SV *sv)
{
	AV *a;
	SV **b;
	int i;
	TJL_SFVec3f *ptr = p;
	ptr->touched = 0;

	if (verbose) {
		printf("TJL_SFVec3fSet:\n");
	}

	if (!SvROK(sv)) {
		(ptr->v).c[0] = 0;
		(ptr->v).c[1] = 0;
		(ptr->v).c[2] = 0;
		if (verbose) {
			printf("\thardcoded vec3f values\n");
		}
	} else if (SvTYPE(SvRV(sv)) != SVt_PVAV) {
			fprintf(stderr, "SFVec3f without being arrayref in TJL_SFVec3fSet.\n");
			return;
	} else {
		a = (AV *) SvRV(sv);
		for (i = 0; i < 3; i++) {
			b = av_fetch(a, i, 1); /* LVal for easiness */
			if (!b) {
				fprintf(stderr, "SFVec3f b == 0 in TJL_SFVec3f.\n");
				return;
			}
			(ptr->v).c[i] = SvNV(*b);
		}
		if (verbose) {
			printf("\tvec3f values: (%f, %f, %f)\n",
				   (ptr->v).c[0],
				   (ptr->v).c[1],
				   (ptr->v).c[2]);
		}
	}
}



MODULE = VRML::JS	PACKAGE = VRML::JS
PROTOTYPES: ENABLE


void
setVerbose(v)
	JSBool v;
CODE:
{
	verbose = v;
	printf("verbose set!\n");
}


void
init(context, global, brow, jssv)
	void *context
	void *global
	void *brow
	SV *jssv
CODE:
{
    JSContext *cx;
	BrowserInternal *br;
	JSObject *glob;

	if (verbose) {
		printf("init:\n");
	}

	runtime = JS_NewRuntime(MAX_RUNTIME_BYTES);
	if (!runtime) {
		die("JS_NewRuntime failed");
	}
	if (verbose) {
		printf("\tJS runtime created,\n");
	}
	
	cx = JS_NewContext(runtime, STACK_CHUNK_SIZE);
	if (!cx) {
		die("JS_NewContext failed");
	}
	context = cx;
	if (verbose) {
		printf("\tJS context created,\n");
	}
	
	glob = JS_NewObject(context, &globalClass, NULL, NULL);
	if (!glob) {
		die("JS_NewObject failed");
	}
	global = glob;
	if (verbose) {
		printf("\tJS global object created,\n");
	}

	/* gets JS standard classes */
	if (!JS_InitStandardClasses(context, global)) {
		die("JS_InitStandardClasses failed");
	}
	if (verbose) {
		printf("\tJS standard classes initialized,\n");
	}

	if (verbose) {
		reportWarningsOn();
	} else {
		reportWarningsOff();
	}
	
	JS_SetErrorReporter(context, errorReporter);
	if (verbose) {
		printf("\tJS errror reporter set,\n");
	}

	br = (BrowserInternal *) JS_malloc(context, sizeof(BrowserInternal));
	br->jssv = newSVsv(jssv); /* new duplicate of jssv */
	br->magic = BROWMAGIC;
	brow = br;
	
	if (!LoadVRMLClasses(cx, glob)) {
	/* if (!LoadVRMLClasses(context, global)) { */
		die("JS_LoadVRMLClasses failed");
	}
	if (verbose) {
		printf("\tVRML classes loaded,\n");
	}

	if (!VRMLBrowserInit(cx, glob, br)) {
	/* if (!VRMLBrowserInit(context, global, brow)) { */
		die("JS_VRMLBrowserInit failed");
	}
	if (verbose) {
		printf("\tVRML browser initialized\n");
	}
}
OUTPUT:
context
global
brow
jssv

void
cleanupJS(cx, br)
	void *cx
	void *br
CODE:
{
	BrowserInternal *brow = br;
	JSContext *context = cx;

	printf("cleanupJS:\n");
	if (brow) {
		printf("\tfree browser internals!!!\n");
		JS_free(context, brow);
		printf("\tbrowser internals freed!!!\n");
	}

	/* JS_DestroyContext(context); */
	/* printf("\tJS context destroyed!!!\n"); */

	JS_DestroyRuntime(runtime);
	printf("\tJS runtime destroyed!!!\n");

    JS_ShutDown();
	printf("\tJS shutdown!!!\n");
}


JSBool
runScript(cx, obj, script, rstr, rnum)
	void *cx
	void *obj
	char *script
	SV *rstr
	SV *rnum
CODE:
{
	jsval rval;
	JSString *strval;
	jsdouble dval = -1.0;
	char *strp;
	size_t len;
	JSObject *globalObj = obj;
	JSContext *context = cx;

	if (verbose) {
		printf("runScript: \"%s\"\n", script);
	}
	len = strlen(script);
	if (!JS_EvaluateScript(context, globalObj, script, len,
						   FNAME_STUB, LINENO_STUB, &rval)) {
		fprintf(stderr, "JS_EvaluateScript failed for \"%s\".\n", script);
		RETVAL = JS_FALSE;
		return;
	}
	strval = JS_ValueToString(context, rval);
	strp = JS_GetStringBytes(strval);
	sv_setpv(rstr, strp);
	printf("runscript: strp = %s\n", strp);

	if (!JS_ValueToNumber(context, rval, &dval)) {
		fprintf(stderr, "JS_ValueToNumber failed.\n");
		RETVAL = JS_FALSE;
		return;
	}
	printf("runscript: dval = %f\n", dval);
	sv_setnv(rnum, dval);

	RETVAL = JS_TRUE;
}
OUTPUT:
RETVAL
rstr
rnum


## Experiment:
JSBool
SFRotationAddFunction()
CODE:
{
}

JSBool
SFColorSetInternal(cx, glob, name, sv)
	void *cx
	void *glob
	char *name
	SV *sv
CODE:
{
	JSObject *obj;
	jsval v;
	void *privateData;
	JSContext *context;
	JSObject *globalObj;

	context = cx;
	globalObj = glob;


	if (!JS_GetProperty(context, globalObj, name, &v)) {
		fprintf(stderr, "JS_GetProperty failed in SFColorSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	if (!JSVAL_IS_OBJECT(v)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in SFColorSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	obj = JSVAL_TO_OBJECT(v);

	/* set_SFColor(JS_GetPrivate(cx,obj), sv); */
	if ((privateData = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFColorSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	TJL_SFColorSet(privateData, sv);
	RETVAL = JS_TRUE;
}
OUTPUT:
RETVAL
sv


JSBool
SFVec3fSetInternal(cx, glob, name, sv)
	void *cx
	void *glob
	char *name
	SV *sv
CODE:
{
	JSObject *obj;
	jsval v;
	void *privateData;
	JSContext *context;
	JSObject *globalObj;

	context = cx;
	globalObj = glob;


	if(!JS_GetProperty(context, globalObj, name, &v)) {
		fprintf(stderr, "JS_GetProperty failed in SFVec3fSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	if(!JSVAL_IS_OBJECT(v)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in SFVec3fSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	obj = JSVAL_TO_OBJECT(v);

	/* set_SFVec3f(JS_GetPrivate(cx, obj), sv); */
	if ((privateData = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFVec3fSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	TJL_SFVec3fSet(privateData, sv);
	RETVAL = JS_TRUE;
}
OUTPUT:
RETVAL
sv


JSBool
SFRotationSetInternal(cx, glob, name, sv)
	void *cx
	void *glob
	char *name
	SV *sv
CODE:
{
	JSObject *obj;
	jsval v;
	void *privateData;
	JSContext *context;
	JSObject *globalObj;

	context = cx;
	globalObj = glob;

	if (!JS_GetProperty(context, globalObj, name, &v)) {
		fprintf(stderr, "JS_GetProperty failed in SFRotationSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	if (!JSVAL_IS_OBJECT(v)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in SFRotationSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	obj = JSVAL_TO_OBJECT(v);

	/* set_SFRotation(JS_GetPrivate(cx,obj), sv); */
	if ((privateData = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFRotationSetInternal.\n");
		RETVAL = JS_FALSE;
		return;
	}
	TJL_SFRotationSet(privateData, sv);
	RETVAL = JS_TRUE;
}
OUTPUT:
RETVAL
sv

###
JSBool
AddAssignProperty(cx, globalObj, name, str)
	void *cx
	void *globalObj
	char *name
	char *str


JSBool
AddWatchProperty(cx, globalObj, name)
	void *cx
	void *globalObj
	char *name
###

