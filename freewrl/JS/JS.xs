/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 * 
 * $Id: JS.xs,v 1.3.2.2 2002/08/12 21:03:21 ayla Exp $
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
/* static BrowserIntern *brow; */
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
	JS_ResolveStub,
	JS_ConvertStub,
	JS_FinalizeStub
};

void
doPerlCallMethod(void *jssv, const char *methodName)
{
	int count = 0;

	dSP;
	ENTER;
	SAVETMPS;
	PUSHMARK(sp);
	XPUSHs((SV *) jssv);
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
JSBool
runScript(void *cx, void *obj, char *script, SV *rstr, SV *rnum)
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
		return JS_FALSE;
	}
	strval = JS_ValueToString(context, rval);
	strp = JS_GetStringBytes(strval);
	sv_setpv(rstr, strp);

	if (!JS_ValueToNumber(context, rval, &dval)) {
		fprintf(stderr, "JS_ValueToNumber failed.\n");
		return JS_FALSE;
	}
	sv_setnv(rnum, dval);

	return JS_TRUE;
}


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
	to->touched ++;
	(to->v) = (from->v);
}

void
TJL_SFColorSet(void *p, void *sv)
{
	AV *a;
	SV **b;
	int i;
	TJL_SFColor *ptr = p;
	ptr->touched = 0;

	if (!SvROK((SV *) sv)) {
		(ptr->v).c[0] = 0;
		(ptr->v).c[1] = 0;
		(ptr->v).c[2] = 0;
	} else {
		if (SvTYPE(SvRV((SV *) sv)) != SVt_PVAV) {
			fprintf(stderr, "SFColor without being arrayref in TJL_SFColorSet.\n");
			return;
		}
		a = (AV *) SvRV((SV *) sv);
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
	to->touched ++;
	(to->v) = (from->v);
}

void
TJL_SFRotationSet(void *p, void *sv)
{
	AV *a;
	SV **b;
	int i;
	TJL_SFRotation *ptr = p;
	ptr->touched = 0;

	if (!SvROK((SV *) sv)) {
		(ptr->v).r[0] = 0;
		(ptr->v).r[1] = 1;
		(ptr->v).r[2] = 0;
		(ptr->v).r[3] = 0;
	} else {
		if (SvTYPE(SvRV((SV *) sv)) != SVt_PVAV) {
			fprintf(stderr, "SFRotation without being arrayref in TJL_SFRotationSet.\n");
			return;
		}
		a = (AV *) SvRV((SV *) sv);
		for (i=0; i<4; i++) {
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
	to->touched ++;
	(to->v) = (from->v);
}

void
TJL_SFVec3fSet(void *p, void *sv)
{
	AV *a;
	SV **b;
	int i;
	TJL_SFVec3f *ptr = p;
	ptr->touched = 0;

	if (!SvROK((SV *) sv)) {
		(ptr->v).c[0] = 0;
		(ptr->v).c[1] = 0;
		(ptr->v).c[2] = 0;
	} else {
		if (SvTYPE(SvRV((SV *) sv)) != SVt_PVAV) {
			fprintf(stderr, "SFVec3f without being arrayref in TJL_SFVec3fSet.\n");
			return;
		}
		a = (AV *) SvRV((SV *) sv);
		for (i=0; i<3; i++) {
			b = av_fetch(a, i, 1); /* LVal for easiness */
			if (!b) {
				fprintf(stderr, "SFVec3f b == 0 in TJL_SFVec3f.\n");
				return;
			}
			(ptr->v).c[i] = SvNV(*b);
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
initContext(context, global, brow, jssv)
	void *context
	void *global
	void *brow
	SV *jssv
CODE:
{
    JSContext *cx;
	BrowserIntern *br;
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

	br = (BrowserIntern *) JS_malloc(context, sizeof(BrowserIntern));
	br->jssv = (void *) newSVsv(jssv); /* new duplicate of jssv */
	br->magic = BROWMAGIC;
	brow = br;
	
	if (!LoadVRMLClasses(context, global)) {
		die("JS_LoadVRMLClasses failed");
	}
	if (verbose) {
		printf("\tVRML classes loaded,\n");
	}

	/* browser init ??? */
	if (!VRMLBrowserInit(context, global, brow)) {
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

void
cleanupJS(cx, br)
	void *cx
	void *br
CODE:
{
	BrowserIntern *brow = br;
	JSContext *context = cx;

	printf("cleanupJS:\n");
	if (brow) {
		JS_free(context, brow);
	}
	printf("\tbrowser internals freed!!!\n");

	/* JS_DestroyContext(context); */
	/* printf("\tJS context destroyed!!!\n"); */

	JS_DestroyRuntime(runtime);
	printf("\tJS runtime destroyed!!!\n");

    JS_ShutDown();
	printf("\tJS shutdown!!!\n");
}


JSBool
runScript(cx, globalObj, script, rstr, rnum)
	void *cx
	void *globalObj
	char *script
	SV *rstr
	SV *rnum


JSBool
AddAssignProperties(cx, globalObj, name, str)
	void *cx
	void *globalObj
	char *name
	char *str


JSBool
AddWatchProperties(cx, globalObj, name)
	void *cx
	void *globalObj
	char *name


JSBool
SFColorSetProperty(cx, globalObj, name, sv)
	void *cx
	void *globalObj
	char *name
	void *sv


JSBool
SFVec3fSetProperty(cx, globalObj, name, sv)
	void *cx
	void *globalObj
	char *name
	void *sv


JSBool
SFRotationSetProperty(cx, globalObj, name, sv)
	void *cx
	void *globalObj
	char *name
	void *sv
