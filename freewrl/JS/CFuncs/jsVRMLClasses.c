/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 * 
 * $Id: jsVRMLClasses.c,v 1.1.2.1 2002/08/12 21:05:01 ayla Exp $
 * 
 */

#include "jsVRMLClasses.h"


JSBool
doMFAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsval v;
	jsval myv;
	char *p, *pp;
	JSString *str, *sstr;
	int len = 0, ind = JSVAL_TO_INT(id);

	if (verbose) {
		printf("\tdoMFAddProperty\n");
	}
	str = JS_ValueToString(cx, id);
	p = JS_GetStringBytes(str);

	if (!strcmp(p, "length") || !strcmp(p, "constructor") ||
		!strcmp(p, "assign") || !strcmp(p, "__touched_flag")) {
		if (verbose) {
			printf("\tproperty \"%s\" is one of \"length\", \"constructor\", \"assign\", \"__touched_flag\". Do nothing.\n", p);
		}
		return JS_TRUE;
	}

	sstr = JS_ValueToString(cx, *vp);
	pp = JS_GetStringBytes(sstr);

	if (verbose) {
		printf("\tadding property %s for %u, %s\n", pp, (unsigned int) obj, p);
	}
	if (!JSVAL_IS_INT(id)){ 
		fprintf(stderr, "JSVAL_IS_INT failed for id in doMFAddProperty.\n");
		return JS_FALSE;
	}
	if (!JS_GetProperty(cx, obj, "length", &v)) {
		fprintf(stderr,
				"JS_GetProperty failed for \"length\" in doMFAddProperty.\n");
		return JS_FALSE;
	}
	
	len = JSVAL_TO_INT(v);
	if (verbose) {
		printf("\tadding property %d, %d\n", ind, len);
	}
	if (ind >= len) {
		len = ind + 1;
		v = INT_TO_JSVAL(len);
		if (!JS_SetProperty(cx, obj, "length", &v)) {
			fprintf(stderr,
					"JS_SetProperty failed for \"length\" in doMFAddProperty.\n");
			return JS_FALSE;
		}
	}
	myv = INT_TO_JSVAL(1);
	if (!JS_SetProperty(cx, obj, "__touched_flag", &myv)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"__touched_flag\" in doMFAddProperty.\n");
		return JS_FALSE;
	}
	return JS_TRUE;
}


JSBool
doMFSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsval myv;
	JSString *str, *sstr;
	char *p, *pp;

	str = JS_ValueToString(cx, id);
	p = JS_GetStringBytes(str);

	sstr = JS_ValueToString(cx, *vp);
	pp = JS_GetStringBytes(str);

	if (verbose) {
		printf("\tsetting property %s for %u, %s\n", pp, (unsigned int) obj, p);
	}
	if (JSVAL_IS_INT(id)) {
		myv = INT_TO_JSVAL(1);
		if (!JS_SetProperty(cx, obj, "__touched_flag", &myv)) {
			fprintf(stderr,
					"JS_SetProperty failed for \"__touched_flag\" in doMFSetProperty.\n");
			return JS_FALSE;
		}
	}
	return JS_TRUE;
}


JSBool
LoadVRMLClasses(JSContext *context, JSObject *globalObj)
{
	jsval v = 0;

	if ((proto_SFColor = JS_InitClass(context, globalObj, NULL, &SFColorClass,
									  SFColorConstr, INIT_ARGC, NULL,
									  SFColorMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for SFColorClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_SFColor);
	if (!JS_SetProperty(context, globalObj, "__SFColor_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for SFColorClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;
	
	if ((proto_SFVec3f = JS_InitClass(context, globalObj, NULL, &SFVec3fClass,
									  SFVec3fConstr, INIT_ARGC, NULL,
									  SFVec3fMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for SFVec3fClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_SFVec3f);
	if (!JS_SetProperty(context, globalObj, "__SFVec3f_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for SFVec3fClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;

	if ((proto_SFRotation = JS_InitClass(context, globalObj, NULL,
										 &SFRotationClass, SFRotationConstr, INIT_ARGC,
										 NULL, SFRotationMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for SFRotationClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_SFRotation);
	if (!JS_SetProperty(context, globalObj, "__SFRotation_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for SFRotationClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;

	if ((proto_MFColor = JS_InitClass(context, globalObj, NULL,
									  &MFColorClass, MFColorConstr, INIT_ARGC,
									  NULL, MFColorMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for SFColorClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_MFColor);
	if (!JS_SetProperty(context, globalObj, "__MFColor_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for MFColorClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;

	if ((proto_MFVec3f = JS_InitClass(context, globalObj, NULL,
									  &MFVec3fClass, MFVec3fConstr, INIT_ARGC,
									  NULL, MFVec3fMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for MFVec3fClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_MFVec3f);
	if (!JS_SetProperty(context, globalObj, "__MFVec3f_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for MFVec3fClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;

	if ((proto_MFRotation = JS_InitClass(context, globalObj, NULL,
										 &MFRotationClass, MFRotationConstr, INIT_ARGC,
										 NULL, MFRotationMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for MFRotationClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_MFRotation);
	if (!JS_SetProperty(context, globalObj, "__MFRotation_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for MFRotationClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;
	
	if ((proto_MFNode = JS_InitClass(context, globalObj, NULL,
									 &MFNodeClass, MFNodeConstr, INIT_ARGC,
									 NULL, MFNodeMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for MFNodeClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_MFNode);
	if (!JS_SetProperty(context, globalObj, "__MFNode_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for MFNodeClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;

	if ((proto_MFString = JS_InitClass(context, globalObj, NULL,
									   &MFStringClass, MFStringConstr, INIT_ARGC,
									   NULL, MFStringMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for SFString failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_MFString);
	if (!JS_SetProperty(context, globalObj, "__MFString_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for MFStringClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;

	if ((proto_SFNode = JS_InitClass(context, globalObj, NULL,
									 &SFNodeClass, SFNodeConstr, INIT_ARGC,
									 NULL, SFNodeMethods, NULL, NULL)) == NULL) {
		fprintf(stderr,
				"JS_InitClass for SFNodeClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}

	v = OBJECT_TO_JSVAL(proto_SFNode);
	if (!JS_SetProperty(context, globalObj, "__SFNode_proto", &v)) {
		fprintf(stderr,
				"JS_SetProperty for SFNodeClass failed in JS_LoadVRMLClasses.\n");
		return JS_FALSE;
	}
	v = 0;

	return JS_TRUE;
}


JSBool
SetTouchable(JSContext *context, JSObject *obj, jsval id, jsval *vp)
{
	/* jsval v; */
	char buffer[STRING];
	char *n = JS_GetStringBytes(JSVAL_TO_STRING(id));
	
	if (verbose) {
		printf("JS_SetTouchable %s\n", n);
	}

	memset(buffer, 0, STRING);
	sprintf(buffer, "_%s_touched", n);
	/* v = INT_TO_JSVAL(1); */
	*vp = INT_TO_JSVAL(1);
	/* JS_SetProperty(context, obj, buffer, &v); */
	if (!JS_SetProperty(context, obj, buffer, vp)) {
		fprintf(stderr,
				"JS_SetProperty failed in JS_SetTouchable.\n");
		return JS_FALSE;
	}
	return JS_TRUE;
}


JSBool
AddAssignProperties(void *cx, void *glob, char *name, char *str)
{
	JSContext *context;
	JSObject *globalObj;

	jsval rval = INT_TO_JSVAL(0);
	context = cx;
	globalObj = glob;

	if (verbose) {
		printf("JS_AddAssignProperties: evaluate script = \"%s\"\n", str);
	}
	if (!JS_EvaluateScript(context, globalObj, str, strlen(str), FNAME_STUB, LINENO_STUB, &rval)) {
		fprintf(stderr,
				"JS_EvaluateScript failed for \"%s\" in JS_AddAssignProperties.\n",
				str);
		return JS_FALSE;
	}
	if (!JS_DefineProperty(context, globalObj, name, rval, NULL, NULL,
						   0 | /* JSPROP_ASSIGNHACK | */ JSPROP_PERMANENT)) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"%s\" in JS_AddAssignProperties.\n",
				name);
		return JS_FALSE;
	}
	return JS_TRUE;
}


JSBool
AddWatchProperties(void *cx, void *glob, char *name)
{
	char buffer[SMALLSTRING];
	JSContext *context;
	JSObject *globalObj;

	jsval v, rval = INT_TO_JSVAL(0);
	context = cx;
	globalObj = glob;


	if (!JS_DefineProperty(context, globalObj, name, rval, NULL, SetTouchable,
						   0 | JSPROP_PERMANENT)) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"%s\" in JS_AddWatchProperties.\n",
				name);
		return JS_FALSE;
	}
	if (verbose) {
		printf("JS_AddWatchProperties for \"%s\"\n", name);
	}

	memset(buffer, 0, SMALLSTRING);
	sprintf(buffer, "_%s_touched", name);
	v = INT_TO_JSVAL(1);
	if (!JS_SetProperty(context, globalObj, buffer, &v)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"%s\" in JS_AddWatchProperties.\n",
				buffer);
		return JS_FALSE;
	}
	return JS_TRUE;
}


JSBool
SFColorGetPrivate(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsdouble d, *dp;
	TJL_SFColor *ptr;
	if ((ptr = JS_GetPrivate(cx,obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFColorGetPrivate.\n");
		return JS_FALSE;
	}

	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			d = (ptr->v).c[0];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFColorGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 1:
			d = (ptr->v).c[1];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFColorGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 2: d = (ptr->v).c[2];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFColorGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		}
	}
	return JS_TRUE;
}

JSBool
SFColorSetPrivate(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsval myv;
	TJL_SFColor *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFColorSetPrivate.\n");
		return JS_FALSE;
	}
	ptr->touched++;

	if (!JS_ConvertValue(cx, *vp, JSTYPE_NUMBER, &myv)) {
		fprintf(stderr, "JS_ConvertValue failed in SFColorSetPrivate.\n");
		return JS_FALSE;
	}

	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			(ptr->v).c[0] = *JSVAL_TO_DOUBLE(myv);
			break;
		case 1:
			(ptr->v).c[1] = *JSVAL_TO_DOUBLE(myv);
			break;
		case 2:
			(ptr->v).c[2] = *JSVAL_TO_DOUBLE(myv);
			break;

		}
	}
	return JS_TRUE;
}


JSBool
SFColorToString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSString *_str;
	char buff[STRING];
    TJL_SFColor *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFColorToString.\n");
		return JS_FALSE;
	}
	UNUSED(argc);

    if (!JS_InstanceOf(cx, obj, &SFColorClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFColorToString.\n");
        return JS_FALSE;
	}
    
	memset(buff, 0, STRING);
	sprintf(buff, "%f %f %f", (ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	_str = JS_NewStringCopyZ(cx, buff);
	    
    *rval = STRING_TO_JSVAL(_str);
    return JS_TRUE;
}

JSBool
SFColorAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSObject *o;
    TJL_SFColor *ptr, *fptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFColorAssign.\n");
        return JS_FALSE;
	}

    if (!JS_InstanceOf(cx, obj, &SFColorClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for obj in SFColorAssign.\n");
        return JS_FALSE;
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o", &o, &o)) {
		fprintf(stderr, "JS_ConvertArguments failed in SFColorAssign.\n");
		return JS_FALSE;
	}

    if (!JS_InstanceOf(cx, o, &SFColorClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for o in SFColorAssign.\n");
        return JS_FALSE;
    }

	if ((fptr = JS_GetPrivate(cx, o)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for o in SFColorAssign.\n");
        return JS_FALSE;
	}

    TJL_SFColorAssign(ptr, fptr);
    *rval = OBJECT_TO_JSVAL(obj); 

    return JS_TRUE;
}

JSBool
SFColorTouched(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int t;
    TJL_SFColor *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFSFColorTouched.\n");
		return JS_FALSE;
	}
	UNUSED(argc);

    if (!JS_InstanceOf(cx, obj, &SFColorClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFColorTouched.\n");
        return JS_FALSE;
	}
    t = ptr->touched;
	ptr->touched = 0;

    if (verbose) {
		printf("SFColorTouched: touched = %d\n", t);
	}
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}


JSBool
SFColorConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	jsdouble pars[3];
	TJL_SFColor *ptr;
	/* void *p; */
	/* if ((p = TJL_SFColorNew()) == NULL) { */
	if ((ptr = (TJL_SFColor *) TJL_SFColorNew()) == NULL) {
		fprintf(stderr, "TJL_SFColorNew failed in SFColorConstr.\n");
		return JS_FALSE;
	}
	/* ptr = p; */
	UNUSED(rval);

	if (!JS_DefineProperties(cx, obj, SFColorProperties)) {
		fprintf(stderr, "JS_DefineProperties failed in SFColorConstr.\n");
		return JS_FALSE;
	}

	if (!JS_SetPrivate(cx, obj, /* (void *) */ ptr)) {
		fprintf(stderr, "JS_SetPrivate failed in SFColorConstr.\n");
		return JS_FALSE;
	}
     	
	if (argc == 0) {
		(ptr->v).c[0] = 0;
		(ptr->v).c[1] = 0;
		(ptr->v).c[2] = 0;
		if (verbose) {
			printf("\tSFColorConstr: %d args, 0 0 0\n", argc);
		}
		return JS_TRUE;
	}
	
	if (!JS_ConvertArguments(cx, argc, argv, "d d d", &(pars[0]), &(pars[1]), &(pars[2]))) {
		fprintf(stderr, "JS_ConvertArguments failed in SFColorConstr.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("\tSFColorConstr: %d args, %f %f %f\n",argc, pars[0], pars[1], pars[2]);
	}

	(ptr->v).c[0] = pars[0];
	(ptr->v).c[1] = pars[1];
	(ptr->v).c[2] = pars[2];
		
	return JS_TRUE;
}


/* void */
/* set_property_SFColor(void *cp, void *p, char *name, SV *sv) */
JSBool
SFColorSetProperty(void *cx, void *glob, char *name, void *sv)
{
	JSObject *obj;
	jsval v;
	void *privateData;
	JSContext *context;
	JSObject *globalObj;

	context = cx;
	globalObj = glob;


	if (!JS_GetProperty(context, globalObj, name, &v)) {
		fprintf(stderr, "JS_GetProperty failed in SFColorSetProperty.\n");
		return JS_FALSE;
	}
	if (!JSVAL_IS_OBJECT(v)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in SFColorSetProperty.\n");
		return JS_FALSE;
	}
	obj = JSVAL_TO_OBJECT(v);
/*
	  if (!JS_InstanceOf(cx, obj, &cls_SFColor, argv)) {
		  die("Property %s was not of type SFColor",name);
	  }
*/
	/* Trust it... ARGH */
	/* set_SFColor(JS_GetPrivate(cx,obj), sv); */
	if ((privateData = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFColorSetProperty.\n");
		return JS_FALSE;
	}
	TJL_SFColorSet(privateData, sv);
	return JS_TRUE;
}



JSBool 
SFVec3fGetPrivate(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsdouble d, *dp;
	TJL_SFVec3f *ptr;
	if ((ptr = JS_GetPrivate(cx,obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFVec3fGetPrivate.\n");
		return JS_FALSE;
	}
	
	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			d = (ptr->v).c[0];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFVec3fGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break; 
		case 1:
			d = (ptr->v).c[1];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFVec3fGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break; 
		case 2:
			d = (ptr->v).c[2];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFVec3fGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break; 
		}
	}
	return JS_TRUE;
}

JSBool 
SFVec3fSetPrivate(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsval myv;
	TJL_SFVec3f *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFVec3fSetPrivate.\n");
		return JS_FALSE;
	}
	ptr->touched ++;

	if (!JS_ConvertValue(cx, *vp, JSTYPE_NUMBER, &myv)) {
		fprintf(stderr, "JS_ConvertValue failed in SFVec3fSetPrivate.\n");
		return JS_FALSE;
	}

	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			(ptr->v).c[0] = *JSVAL_TO_DOUBLE(myv);
			break;
		case 1:
			(ptr->v).c[1] = *JSVAL_TO_DOUBLE(myv);
			break;
		case 2:
			(ptr->v).c[2] = *JSVAL_TO_DOUBLE(myv);
			break;
		}
	}
	return JS_TRUE;
}


JSBool
SFVec3fToString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char buff[STRING];
    JSString *_str;
    TJL_SFVec3f *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFVec3fToString.\n");
		return JS_FALSE;
	}
	UNUSED(argc);

    if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fToString.\n");
        return JS_FALSE;
	}
    
	memset(buff, 0, STRING);
	sprintf(buff, "%f %f %f", (ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	_str = JS_NewStringCopyZ(cx, buff);
	    
    *rval = STRING_TO_JSVAL(_str);
    return JS_TRUE;
}

JSBool
SFVec3fAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSObject *o;
    TJL_SFVec3f *fptr, *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFVec3fAssign.\n");
        return JS_FALSE;
	}

    if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for obj in SFVec3fAssign.\n");
        return JS_FALSE;
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o", &o, &o)) {
		fprintf(stderr, "JS_ConvertArguments failed in SFVec3fAssign.\n");
		return JS_FALSE;
	}

    if (!JS_InstanceOf(cx, o, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for o in SFVec3fAssign.\n");
        return JS_FALSE;
    }

	if ((fptr = JS_GetPrivate(cx, o)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for o in SFVec3fAssign.\n");
        return JS_FALSE;
	}

    TJL_SFVec3fAssign(ptr, fptr);
    *rval = OBJECT_TO_JSVAL(obj); 

    return JS_TRUE;
}

JSBool
SFVec3fTouched(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int t;
    TJL_SFVec3f *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFVec3fTouched.\n");
		return JS_FALSE;
	}
	UNUSED(argc);

    if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fTouched.\n");
        return JS_FALSE;
	}

    t = ptr->touched;
	ptr->touched = 0;

    if (verbose) {
		printf("SFVec3fTouched: touched = %d\n", t);
	}
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}


JSBool
SFVec3fSubtract(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *o, *proto, *ro;
	TJL_SFVec3f *vec1, *vec2, *res;
		
	if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for obj in SFVec3fSubtract.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFVec3fSubtract\n");
	}
			
	if (!JS_ConvertArguments(cx, argc, argv, "o", &o)) {
		fprintf(stderr, "JS_ConvertArguments failed in SFVec3fSubtract.\n");
		return JS_FALSE;
	}
	
	if (!JS_InstanceOf(cx, o, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for o in SFVec3fSubtract.\n");
		return JS_FALSE;
	}

	if ((proto = JS_GetPrototype(cx, o)) == NULL) {
		fprintf(stderr, "JS_GetPrototype failed in SFVec3fSubtract.\n");
		return JS_FALSE;
	}
	if ((ro = JS_ConstructObject(cx, &SFVec3fClass, proto, NULL)) == NULL) {
		fprintf(stderr, "JS_ConstructObject failed in SFVec3fSubtract.\n");
		return JS_FALSE;
	}
	if ((vec1 = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate for obj failed in SFVec3fSubtract.\n");
		return JS_FALSE;
	}
	if ((vec2 = JS_GetPrivate(cx, o)) == NULL) {
		fprintf(stderr, "JS_GetPrivate for o failed in SFVec3fSubtract.\n");
		return JS_FALSE;
	}
	if ((res = JS_GetPrivate(cx, ro)) == NULL) {
		fprintf(stderr, "JS_GetPrivate for ro failed in SFVec3fSubtract.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(ro);
	
	(*res).v.c[0] = (*vec1).v.c[0] - (*vec2).v.c[0];
	(*res).v.c[1] = (*vec1).v.c[1] - (*vec2).v.c[1];
	(*res).v.c[2] = (*vec1).v.c[2] - (*vec2).v.c[2];
	return JS_TRUE;
}


JSBool
SFVec3fNormalize(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *ro, *proto;
	TJL_SFVec3f *vec1, *res;
	double xx;

	if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fNormalize.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFVec3fNormalize\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "")) {
		fprintf(stderr, "JS_ConvertArguments failed in SFVec3fNormalize.\n");
		return JS_FALSE;
	}
	if ((proto = JS_GetPrototype(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrototype failed in SFVec3fNormalize.\n");
		return JS_FALSE;
	}
	if ((ro = JS_ConstructObject(cx, &SFVec3fClass, proto, NULL)) == NULL) {
		fprintf(stderr, "JS_ConstructObject failed in SFVec3fNormalize.\n");
		return JS_FALSE;
	}
	if ((vec1 = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFVec3fNormalize.\n");
		return JS_FALSE;
	}
	if ((res = JS_GetPrivate(cx, ro)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for ro in SFVec3fNormalize.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(ro);

	xx = sqrt((*vec1).v.c[0] * (*vec1).v.c[0] +
			  (*vec1).v.c[1] * (*vec1).v.c[1] +
			  (*vec1).v.c[2] * (*vec1).v.c[2]);
	(*res).v.c[0] = (*vec1).v.c[0] / xx;
	(*res).v.c[1] = (*vec1).v.c[1] / xx;
	(*res).v.c[2] = (*vec1).v.c[2] / xx;

	return JS_TRUE;
}
		

JSBool
SFVec3fAdd(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *v2, *proto, *ro;
	TJL_SFVec3f *vec1, *vec2, *res;

	if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fAdd.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFVec3fAdd\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o", &v2)) {
		fprintf(stderr, "JS_ConvertArguments failed in SFVec3fAdd.\n");
		return JS_FALSE;
	}
	if (!JS_InstanceOf(cx, v2, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fAdd.\n");
		return JS_FALSE;
	}
	if ((proto = JS_GetPrototype(cx, v2)) == NULL) {
		fprintf(stderr, "JS_GetPrototype failed in SFVec3fAdd.\n");
		return JS_FALSE;
	}
	if ((ro = JS_ConstructObject(cx, &SFVec3fClass, proto, NULL)) == NULL) {
		fprintf(stderr, "JS_ConstructObject failed in SFVec3fAdd.\n");
		return JS_FALSE;
	}
	if ((vec1 = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFVec3fAdd.\n");
		return JS_FALSE;
	}
	if ((vec2 = JS_GetPrivate(cx, v2)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for v2 in SFVec3fAdd.\n");
		return JS_FALSE;
	}
	if ((res = JS_GetPrivate(cx, ro)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for ro in SFVec3fAdd.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(ro);
	(*res).v.c[0] = (*vec1).v.c[0] + (*vec2).v.c[0];
	(*res).v.c[1] = (*vec1).v.c[1] + (*vec2).v.c[1];
	(*res).v.c[2] = (*vec1).v.c[2] + (*vec2).v.c[2];

	return JS_TRUE;
}
		

JSBool
SFVec3fLength(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	jsdouble result, *dp;
	JSObject *proto;
	TJL_SFVec3f *vec1;


	if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fLength.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFVec3fLength\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "")) {
		fprintf(stderr, "JS_ConvertArguments failed in SFVec3fLength.\n");
		return JS_FALSE;
	}
	if ((proto = JS_GetPrototype(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrototype failed in SFVec3fLength.\n");
		return JS_FALSE;
	}
	if ((vec1 = JS_GetPrivate(cx,obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFVec3fLength.\n");
		return JS_FALSE;
	}
	result = sqrt((*vec1).v.c[0] * (*vec1).v.c[0] +
				  (*vec1).v.c[1] * (*vec1).v.c[1] +
				  (*vec1).v.c[2] * (*vec1).v.c[2]); 
	if ((dp = JS_NewDouble(cx, result)) == NULL) {
		fprintf(stderr, "JS_NewDouble failed for %f in SFVec3fLength.\n", result);
		return JS_FALSE;
	}
	*rval = DOUBLE_TO_JSVAL(dp); 

	return JS_TRUE;
}
		

JSBool
SFVec3fCross(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *v2, *proto, *ro;
	TJL_SFVec3f *vec1, *vec2, *res;

	if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fCross.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFVec3fCross\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o", &v2)) {
		fprintf(stderr, "JS_ConvertArguments failed in SFVec3fCross.\n");
		return JS_FALSE;
	}
	if (!JS_InstanceOf(cx, v2, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fCross.\n");
		return JS_FALSE;
	}
	if ((proto = JS_GetPrototype(cx, v2)) == NULL) {
		fprintf(stderr, "JS_GetPrototype failed in SFVec3fCross.\n");
		return JS_FALSE;
	}
	if ((ro = JS_ConstructObject(cx, &SFVec3fClass, proto, NULL)) == NULL) {
		fprintf(stderr, "JS_ConstructObject failed in SFVec3fCross.\n");
		return JS_FALSE;
	}
	if ((vec1 = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFVec3fCross.\n");
		return JS_FALSE;
	}
	if ((vec2 = JS_GetPrivate(cx,v2)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for v2 in SFVec3fCross.\n");
		return JS_FALSE;
	}
	if ((res = JS_GetPrivate(cx,ro)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for ro in SFVec3fCross.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(ro);
	   
	(*res).v.c[0] = 
		(*vec1).v.c[1] * (*vec2).v.c[2] - 
		(*vec2).v.c[1] * (*vec1).v.c[2];
	(*res).v.c[1] = 
		(*vec1).v.c[2] * (*vec2).v.c[0] - 
		(*vec2).v.c[2] * (*vec1).v.c[0];
	(*res).v.c[2] = 
		(*vec1).v.c[0] * (*vec2).v.c[1] - 
		(*vec2).v.c[0] * (*vec1).v.c[1];

	return JS_TRUE;
}
		

JSBool
SFVec3fNegate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *ro, *proto;
	TJL_SFVec3f *vec1, *res;

	if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFVec3fNegate.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFVec3fCross\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "")) {
		fprintf(stderr, "JS_ConvertArguments failed in SFVec3fNegate.\n");
		return JS_FALSE;
	}
	if ((proto = JS_GetPrototype(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrototype failed in SFVec3fNegate.\n");
		return JS_FALSE;
	}
	if ((ro = JS_ConstructObject(cx, &SFVec3fClass, proto, NULL)) == NULL) {
		fprintf(stderr, "JS_ConstructObject failed in SFVec3fNegate.\n");
		return JS_FALSE;
	}
	if ((vec1 = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFVec3fNegate.\n");
		return JS_FALSE;
	}
	if ((res = JS_GetPrivate(cx, ro)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for ro in SFVec3fNegate.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(ro);
	(*res).v.c[0] = -(*vec1).v.c[0];
	(*res).v.c[1] = -(*vec1).v.c[1];
	(*res).v.c[2] = -(*vec1).v.c[2];

	return JS_TRUE;
}


JSBool
SFVec3fConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	jsdouble pars[3];
	TJL_SFVec3f *ptr;
	/* void *p; */
	/* ptr = p; */
	if ((ptr = (TJL_SFVec3f *) TJL_SFVec3fNew()) == NULL) {
		fprintf(stderr, "TJL_SFVec3fNew failed in SFVec3fConstr.\n");
		return JS_FALSE;
	}
	UNUSED(rval);

	if (!JS_DefineProperties(cx, obj, SFVec3fProperties)) {
		fprintf(stderr, "JS_DefineProperties failed in SFVec3fConstr.\n");
		return JS_FALSE;
	}
	if (!JS_SetPrivate(cx, obj, ptr)) {
		fprintf(stderr, "JS_SetPrivate failed in SFVec3fConstr.\n");
		return JS_FALSE;
	}
     	
	if (argc == 0) {
		(ptr->v).c[0] = 0;
		(ptr->v).c[1] = 0;
		(ptr->v).c[2] = 0;
		if (verbose) {
			printf("\tSFVec3fConstr: %d args, 0 0 0\n", argc);
		}
		return JS_TRUE;
	}
	
	if (!JS_ConvertArguments(cx, argc, argv, "d d d", &(pars[0]), &(pars[1]), &(pars[2]))) {
		fprintf(stderr, "JS_ConvertArguments failed in SFVec3fConstr.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("\tSFVec3fConstr: %d args, %f %f %f\n", argc, pars[0], pars[1], pars[2]);
	}

	(ptr->v).c[0] = pars[0];
	(ptr->v).c[1] = pars[1];
	(ptr->v).c[2] = pars[2];
		
	return JS_TRUE;
}

/* void */
/* set_property_SFVec3f(void *cp, void *p, char *name, SV *sv) */
JSBool
SFVec3fSetProperty(void *cx, void *glob, char *name, void *sv)
{
	JSObject *obj;
	jsval v;
	void *privateData;
	JSContext *context;
	JSObject *globalObj;

	context = cx;
	globalObj = glob;


	if(!JS_GetProperty(context, globalObj, name, &v)) {
		fprintf(stderr, "JS_GetProperty failed in SFVec3fSetProperty.\n");
		return JS_FALSE;
	}
	if(!JSVAL_IS_OBJECT(v)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in SFVec3fSetProperty.\n");
		return JS_FALSE;
	}
	obj = JSVAL_TO_OBJECT(v);
/*
	  if (!JS_InstanceOf(cx, obj, &cls_SFVec3f, argv)) {
		  die("Property %s was not of type SFVec3f",name);
	  }
*/
	/* Trust it... ARGH */
	/* set_SFVec3f(JS_GetPrivate(cx, obj), sv); */
	if ((privateData = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFVec3fSetProperty.\n");
		return JS_FALSE;
	}
	TJL_SFVec3fSet(privateData, sv);
	return JS_TRUE;
}


JSBool 
SFRotationGetPrivate(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsdouble d;
	jsdouble *dp;
	TJL_SFRotation *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFRotationGetPrivate.\n");
		return JS_FALSE;
	}
	
	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			d = (ptr->v).r[0];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFRotationGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break; 
		case 1:
			d = (ptr->v).r[1];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFRotationGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break; 
		case 2:
			d = (ptr->v).r[2];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFRotationGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break; 
		case 3:
			d = (ptr->v).r[3];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				fprintf(stderr,
						"JS_NewDouble failed for %f in SFRotationGetPrivate.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		}
	}
	return JS_TRUE;
}

JSBool 
SFRotationSetPrivate(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	jsval myv;
	TJL_SFRotation *ptr = JS_GetPrivate(cx, obj);
	if (ptr == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFRotationSetPrivate.\n");
		return JS_FALSE;
	}
	ptr->touched ++;

	if (!JS_ConvertValue(cx, *vp, JSTYPE_NUMBER, &myv)) {
		fprintf(stderr, "JS_ConvertValue failed in SFRotationSetPrivate.\n");
		return JS_FALSE;
	}
	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			(ptr->v).r[0] = *JSVAL_TO_DOUBLE(myv);
			break;
		case 1:
			(ptr->v).r[1] = *JSVAL_TO_DOUBLE(myv);
			break;
		case 2:
			(ptr->v).r[2] = *JSVAL_TO_DOUBLE(myv);
			break;
		case 3:
			(ptr->v).r[3] = *JSVAL_TO_DOUBLE(myv);
			break;
		}
	}
	return JS_TRUE;
}


JSBool
SFRotationToString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char buff[STRING];
    JSString *_str;
    TJL_SFRotation *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFRotationToString.\n");
		return JS_FALSE;
	}
	UNUSED(argc);

    if (!JS_InstanceOf(cx, obj, &SFRotationClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFRotationToString.\n");
        return JS_FALSE;
	}
    
	memset(buff, 0, STRING);
	sprintf(buff, "%f %f %f %f", (ptr->v).r[0], (ptr->v).r[1], (ptr->v).r[2], (ptr->v).r[3]);
	_str = JS_NewStringCopyZ(cx, buff);
	    
    *rval = STRING_TO_JSVAL(_str);
    return JS_TRUE;
}

JSBool
SFRotationAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSObject *o;
    TJL_SFRotation *fptr;
    TJL_SFRotation *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFRotationAssign.\n");
        return JS_FALSE;
	}

    if (!JS_InstanceOf(cx, obj, &SFRotationClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for obj in SFRotationAssign.\n");
        return JS_FALSE;
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o", &o, &o)) {
		fprintf(stderr, "JS_ConvertArguments failed in SFRotationAssign.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, o, &SFRotationClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for o in SFRotationAssign.\n");
        return JS_FALSE;
    }
	if ((fptr = JS_GetPrivate(cx, o)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for o in SFRotationAssign.\n");
        return JS_FALSE;
	}

    TJL_SFRotationAssign(ptr, fptr);
    *rval = OBJECT_TO_JSVAL(obj); 
    return JS_TRUE;
}

JSBool
SFRotationTouched(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    int t;
    TJL_SFRotation *ptr;
	if ((ptr = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFRotationTouched.\n");
		return JS_FALSE;
	}
	UNUSED(argc);

    if (!JS_InstanceOf(cx, obj, &SFRotationClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFRotationTouched.\n");
        return JS_FALSE;
	}
    t = ptr->touched;
	ptr->touched = 0;

    if (verbose) {
		printf("SFRotationTouched: touched = %d\n", t);
	}
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}


JSBool
SFRotationMultVec(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *o, *ro, *proto;
	TJL_SFRotation *rfrom;
	TJL_SFVec3f *vfrom, *vto;
	double rl, vl, s, c;
	float c1[3], c2[3];

	if (!JS_InstanceOf(cx, obj, &SFRotationClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFRotationMultVec\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o", &o)) {
		fprintf(stderr, "JS_ConvertArguments failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if (!JS_InstanceOf(cx, o, &SFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if ((proto = JS_GetPrototype(cx, o)) == NULL) {
		fprintf(stderr, "JS_GetPrototype failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if ((ro = JS_ConstructObject(cx, &SFVec3fClass, proto, NULL)) == NULL) {
		fprintf(stderr, "JS_ConstructObject failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if ((rfrom = JS_GetPrivate(cx,obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if ((vfrom = JS_GetPrivate(cx, o)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for o in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if ((vto = JS_GetPrivate(cx, ro)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for ro in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	
	rl = AVECLEN(rfrom->v.r);
	vl = AVECLEN(vfrom->v.c);
	s = sin(rfrom->v.r[3]);
	c = cos(rfrom->v.r[3]);
	AVECCP(rfrom->v.r, vfrom->v.c,c1);
	AVECSCALE(c1, 1.0 / rl );
	AVECCP(rfrom->v.r, c1, c2);
	AVECSCALE(c2, 1.0 / rl) ;
	vto->v.c[0] = vfrom->v.c[0] + s * c1[0] + (1-c) * c2[0];
	vto->v.c[1] = vfrom->v.c[1] + s * c1[1] + (1-c) * c2[1];
	vto->v.c[2] = vfrom->v.c[2] + s * c1[2] + (1-c) * c2[2];
	
	*rval = OBJECT_TO_JSVAL(ro);
	return JS_TRUE;
}
		

JSBool
SFRotationInverse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *o, *proto;
	TJL_SFRotation *rfrom, *rto;
	UNUSED(argc);

	if (!JS_InstanceOf(cx, obj, &SFRotationClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in SFRotationInverse.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFRotationInverse\n");
	}

	if ((proto = JS_GetPrototype(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrototype failed in SFRotationInverse.\n");
		return JS_FALSE;
	}
	if ((o = JS_ConstructObject(cx, &SFRotationClass, proto, NULL)) == NULL) {
		fprintf(stderr, "JS_ConstructObject failed in SFRotationInverse.\n");
		return JS_FALSE;
	}
	if ((rfrom = JS_GetPrivate(cx, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for obj in SFRotationInverse.\n");
		return JS_FALSE;
	}
	if ((rto = JS_GetPrivate(cx, o)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed for o in SFRotationInverse.\n");
		return JS_FALSE;
	}

	rto->v.r[0] = rfrom->v.r[0];
	rto->v.r[1] = rfrom->v.r[1];
	rto->v.r[2] = rfrom->v.r[2];
	rto->v.r[3] = -rfrom->v.r[3];

	*rval = OBJECT_TO_JSVAL(o);
	return JS_TRUE;
}
		

JSBool 
SFRotationConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	jsdouble pars[4];
	JSObject *ob1, *ob2, *ob3;
	TJL_SFVec3f *vec1, *vec2, *vec3;
	double v1len, v2len, v12dp;
	TJL_SFRotation *ptr;
	/* void *p; */
	/* ptr = p; */
	if ((ptr = TJL_SFRotationNew()) == NULL) {
		fprintf(stderr, "TJL_SFRotationNew failed in SFRotationConstr.\n");
		return JS_FALSE;
	}
	UNUSED(rval);

	if (!JS_DefineProperties(cx, obj, SFRotationProperties)) {
		fprintf(stderr, "JS_DefineProperties failed in SFRotationConstr.\n");
		return JS_FALSE;
	}
	if (!JS_SetPrivate(cx, obj, ptr)) {
		fprintf(stderr, "JS_SetPrivate failed in SFRotationConstr.\n");
		return JS_FALSE;
	}

	if (JS_ConvertArguments(cx, argc, argv, "d d d d",
							&(pars[0]), &(pars[1]), &(pars[2]), &(pars[3]))) {
		(ptr->v).r[0] = pars[0];
		(ptr->v).r[1] = pars[1];
		(ptr->v).r[2] = pars[2];
		(ptr->v).r[3] = pars[3];
	} else if (JS_ConvertArguments(cx, argc, argv, "o o", &ob1, &ob2)) {
		if (!JS_InstanceOf(cx, ob1, &SFVec3fClass, argv)) {
			fprintf(stderr, "JS_InstanceOf failed for ob1 in SFRotationConstr.\n");
			return JS_FALSE;
		}

		if (!JS_InstanceOf(cx, ob2, &SFVec3fClass, argv)) {
			fprintf(stderr, "JS_InstanceOf failed for ob2 in SFRotationConstr.\n");
			return JS_FALSE;
		}
		vec1 = JS_GetPrivate(cx, ob1);
		if (vec1 == NULL) {
			fprintf(stderr, "JS_GetPrivate failed for ob1 in SFRotationConstr.\n");
			return JS_FALSE;
		}
		vec2 = JS_GetPrivate(cx, ob2);
		if (vec2 == NULL) {
			fprintf(stderr, "JS_GetPrivate failed for ob2 in SFRotationConstr.\n");
			return JS_FALSE;
		}
		v1len = sqrt(vec1->v.c[0] * vec1->v.c[0] +
					 vec1->v.c[1] * vec1->v.c[1] +
					 vec1->v.c[2] * vec1->v.c[2]);
		v2len = sqrt(vec2->v.c[0] * vec2->v.c[0] +
					 vec2->v.c[1] * vec2->v.c[1] +
					 vec2->v.c[2] * vec2->v.c[2]);
		v12dp =
			vec1->v.c[0] * vec2->v.c[0] +
			vec1->v.c[1] * vec2->v.c[1] +
			vec1->v.c[2] * vec2->v.c[2];
		(ptr->v).r[0] =
			vec1->v.c[1] * vec2->v.c[2] - 
			vec2->v.c[1] * vec1->v.c[2];
		(ptr->v).r[1] =
			vec1->v.c[2] * vec2->v.c[0] - 
			vec2->v.c[2] * vec1->v.c[0];
		(ptr->v).r[2] =
			vec1->v.c[0] * vec2->v.c[1] - 
			vec2->v.c[0] * vec1->v.c[1];
		v12dp /= v1len * v2len;
		(ptr->v).r[3] = atan2(sqrt(1 - v12dp * v12dp), v12dp);
	} else if (JS_ConvertArguments(cx, argc, argv, "o d", &ob3, &(pars[0]))) {
		if (!JS_InstanceOf(cx, ob3, &SFVec3fClass, argv)) {
			fprintf(stderr, "JS_InstanceOf failed for ob3 in SFRotationConstr.\n");
			return JS_FALSE;
		}
		vec3 = JS_GetPrivate(cx, ob3);
		if (vec3 == NULL) {
			fprintf(stderr, "JS_GetPrivate failed for ob3 in SFRotationConstr.\n");
			return JS_FALSE;
		}
		(ptr->v).r[0] = vec3->v.c[0]; 
		(ptr->v).r[1] = vec3->v.c[1]; 
		(ptr->v).r[2] = vec3->v.c[2]; 
		(ptr->v).r[3] = pars[0];
		
	} else if (argc == 0) {
		(ptr->v).r[0] = 0;
		(ptr->v).r[0] = 0;
		(ptr->v).r[0] = 1;
		(ptr->v).r[0] = 0;
	} else {
		fprintf(stderr, "Invalid arguments for SFRotationConstr.\n");
		return JS_FALSE;
	}
	return JS_TRUE;
}

/* void */
/* set_property_SFRotation(void *cp, void *p, char *name, SV *sv) */
JSBool
SFRotationSetProperty(void *cx, void *glob, char *name, void *sv)
{
	JSObject *obj;
	jsval v;
	void *privateData;
	JSContext *context;
	JSObject *globalObj;

	context = cx;
	globalObj = glob;

	if (!JS_GetProperty(context, globalObj, name, &v)) {
		fprintf(stderr, "JS_GetProperty failed in SFRotationSetProperty.\n");
		return JS_FALSE;
	}
	if (!JSVAL_IS_OBJECT(v)) {
		fprintf(stderr, "JSVAL_IS_OBJECT failed in SFRotationSetProperty.\n");
		return JS_FALSE;
	}
	obj = JSVAL_TO_OBJECT(v);
/*
	  if (!JS_InstanceOf(cx, obj, &cls_SFRotation, argv)) {
		  die("Property %s was not of type SFRotation",name);
	  }
*/
	/* Trust it... ARGH */
	/* set_SFRotation(JS_GetPrivate(cx,obj), sv); */
	if ((privateData = JS_GetPrivate(context, obj)) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFRotationSetProperty.\n");
		return JS_FALSE;
	}
	TJL_SFRotationSet(privateData, sv);
	return JS_TRUE;
}


JSBool
MFColorAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFColorAddProperty:\n");
	}
	return doMFAddProperty(cx, obj, id, vp);
}

JSBool 
MFColorSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFColorSetProperty:\n");
	}
	return doMFSetProperty(cx, obj, id, vp);
}


JSBool
MFColorConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	unsigned int i;
	char buf[SMALLSTRING];
	jsval v = INT_TO_JSVAL(argc);
	UNUSED(rval);

	if (!JS_DefineProperty(cx, obj, "length", v, NULL, NULL,
						   JSPROP_PERMANENT )) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"length\" in MFColorConstr.\n");
		return JS_FALSE;
	}
	v = INT_TO_JSVAL(0);
	if (!JS_DefineProperty(cx, obj, "__touched_flag", v, NULL, NULL,
						   JSPROP_PERMANENT)) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"__touched_flag\" in MFColorConstr.\n");
		return JS_FALSE;
	}
	if (!argv) {
		return JS_TRUE;
	}

	for (i = 0; i < argc; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
		/* XXX Check type */
		if (!JS_DefineProperty(cx, obj, buf, argv[i], JS_PropertyStub, JS_PropertyStub,
							   JSPROP_ENUMERATE)) {
			fprintf(stderr,
					"JS_DefineProperty failed for \"%s\" in MFColorConstr.\n",
					buf);
			return JS_FALSE;
		}
	}
	return JS_TRUE;
}

JSBool
MFColorAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval val, myv;
    int len, i;
    JSObject *o;
	char buf[SMALLSTRING];

	if (!JS_InstanceOf(cx, obj, &MFColorClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in MFColorAssign.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("MFColorAssign\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o", &o)) {
		fprintf(stderr, "JS_ConvertArguments failed in MFColorAssign.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, o, &MFColorClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in MFColorAssign.\n");
        return JS_FALSE;
    }
	/* Now, we assign length properties from o to obj (XXX HERE) */
	myv = INT_TO_JSVAL(1);
    if (!JS_SetProperty(cx, obj, "__touched_flag", &myv)) {
		fprintf(stderr, "JS_SetProperty failed for \"__touched_flag\" in MFColorAssign.\n");
        return JS_FALSE;
	}
    if (!JS_GetProperty(cx, o, "length", &val)) {
		fprintf(stderr, "JS_GetProperty failed for \"length\" in MFColorAssign.\n");
        return JS_FALSE;
	}
    if (!JS_SetProperty(cx, obj, "length", &val)) {
		fprintf(stderr, "JS_SetProperty failed for \"length\" in MFColorAssign.\n");
        return JS_FALSE;
	}
    len = JSVAL_TO_INT(val); /* XXX Assume int */

    for (i = 0; i < len; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf,"%d",i);
		if (!JS_GetProperty(cx, o, buf, &val)) {
			fprintf(stderr, "JS_GetProperty failed for \"%s\" in MFColorAssign.\n", buf);
			return JS_FALSE;
		}
		if (!JS_SetProperty(cx, obj, buf, &val)) {
			fprintf(stderr, "JS_SetProperty failed for \"%s\" in MFColorAssign.\n", buf);
			return JS_FALSE;
		}
    }
    *rval = OBJECT_TO_JSVAL(obj); 
    return JS_TRUE;
}


JSBool
MFVec3fAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFVec3fAddProperty:\n");
	}
	return doMFAddProperty(cx, obj, id, vp);
}

JSBool 
MFVec3fSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFVec3fSetProperty:\n");
	}
	return doMFSetProperty(cx, obj, id, vp);
}


JSBool
MFVec3fConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	unsigned int i;
	char buf[SMALLSTRING];
	jsval v = INT_TO_JSVAL(argc);
	UNUSED(rval);

	if (!JS_DefineProperty(cx, obj, "length", v, NULL, NULL,
						   JSPROP_PERMANENT )) {
		fprintf(stderr, "JS_DefineProperty failed for \"length\" in MFVec3fConstr.\n");
        return JS_FALSE;
	}
	v = INT_TO_JSVAL(0);
	if (!JS_DefineProperty(cx, obj, "__touched_flag", v, NULL, NULL,
						   JSPROP_PERMANENT)) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"__touched_flag\" in MFVec3fConstr.\n");
        return JS_FALSE;
	}
	if (!argv) {
		return JS_TRUE;
	}

	for (i = 0; i < argc; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
		/* XXX Check type */
		if (!JS_DefineProperty(cx, obj, buf, argv[i], JS_PropertyStub, JS_PropertyStub,
							   JSPROP_ENUMERATE)) {
			fprintf(stderr,
					"JS_DefineProperty failed for \"%s\" in MFVec3fConstr.\n",
					buf);
			return JS_FALSE;
		}
	}
	return JS_TRUE;
}

JSBool
MFVec3fAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval val, myv;
    int len, i;
    JSObject *o;
	char buf[SMALLSTRING];

	if (!JS_InstanceOf(cx, obj, &MFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in MFVec3fAssign.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("MFVec3fAssign\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o", &o)) {
		fprintf(stderr, "JS_ConvertArguments failed in MFVec3fAssign.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, o, &MFVec3fClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for o in MFVec3fAssign.\n");
        return JS_FALSE;
    }

	/* Now, we assign length properties from o to obj */
	/* XXX HERE */
	myv = INT_TO_JSVAL(1);
    if (!JS_SetProperty(cx, obj, "__touched_flag", &myv)) {
		fprintf(stderr, "JS_SetProperty failed for \"__touched_flag\" in MFVec3fAssign.\n");
        return JS_FALSE;
	}
    if (!JS_GetProperty(cx, o, "length", &val)) {
		fprintf(stderr, "JS_GetProperty failed for \"length\" in MFVec3fAssign.\n");
        return JS_FALSE;
	}
    if (!JS_SetProperty(cx, obj, "length", &val)) {
		fprintf(stderr, "JS_SetProperty failed for \"length\" in MFVec3fAssign.\n");
        return JS_FALSE;
	}
    len = JSVAL_TO_INT(val); /* XXX Assume int */

    for (i = 0; i < len; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
	    if (!JS_GetProperty(cx, o, buf, &val)) {
			fprintf(stderr,
					"JS_GetProperty failed for \"%s\" in MFVec3fAssign.\n",
					buf);
			return JS_FALSE;
		}
	    if (!JS_SetProperty(cx, obj, buf, &val)) {
			fprintf(stderr,
					"JS_SetProperty failed for \"%s\" in MFVec3fAssign.\n",
					buf);
			return JS_FALSE;
		}
    }
    *rval = OBJECT_TO_JSVAL(obj); 
    return JS_TRUE;
}


JSBool
MFRotationAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFRotationAddProperty:\n");
	}
	return doMFAddProperty(cx, obj, id, vp);
}

JSBool 
MFRotationSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFRotationSetProperty:\n");
	}
	return doMFSetProperty(cx, obj, id, vp);
}


JSBool
MFRotationConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	unsigned int i;
	char buf[SMALLSTRING];
	jsval v = INT_TO_JSVAL(argc);
	UNUSED(rval);

	if (!JS_DefineProperty(cx, obj, "length", v,
						   NULL, NULL, JSPROP_PERMANENT )) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"length\" in MFRotationConstr.\n");
		return JS_FALSE;
	}

	v = INT_TO_JSVAL(0);
	if (!JS_DefineProperty(cx, obj, "__touched_flag", v,
						   NULL, NULL, JSPROP_PERMANENT)) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"__touched_flag\" in MFRotationConstr.\n");
		return JS_FALSE;
	}

	if (!argv) return JS_TRUE;

	for (i = 0; i < argc; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
		/* XXX Check type */
		if (!JS_DefineProperty(cx,obj,buf,argv[i],
							   JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE)) {
			fprintf(stderr,
					"JS_DefineProperty failed for \"%s\" in MFRotationConstr.\n",
					buf);
			return JS_FALSE;
		}
	}
	return JS_TRUE;
}

JSBool
MFRotationAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval val;
    jsval myv;
    int len;
    int i;
    JSObject *o;
	char buf[SMALLSTRING];

    if (!JS_InstanceOf(cx, obj, &MFRotationClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for obj in MFRotationAssign.\n");
        return JS_FALSE;
	}
    if (verbose) {
		printf("MFRotationAssign\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o",&o)) {
		fprintf(stderr, "JS_ConvertArguments failed in MFRotationAssign.\n");
		return JS_FALSE;
	}
	
    if (!JS_InstanceOf(cx, o, &MFRotationClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed for o in MFRotationAssign.\n");
        return JS_FALSE;
    }
	/* Now, we assign length properties from o to obj */
	/* XXX HERE */
	myv = INT_TO_JSVAL(1);

    if (!JS_SetProperty(cx, obj, "__touched_flag", &myv)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"__touched_flag\" in MFRotationAssign.\n");
        return JS_FALSE;
	}

    if (!JS_GetProperty(cx, o, "length", &val)) {
		fprintf(stderr,
				"JS_GetProperty failed for \"length\" in MFRotationAssign.\n");
        return JS_FALSE;
	}

    if (!JS_SetProperty(cx, obj, "length", &val)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"length\" in MFRotationAssign.\n");
        return JS_FALSE;
	}

    len = JSVAL_TO_INT(val); /* XXX Assume int */
    for (i = 0; i < len; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
	    if (!JS_GetProperty(cx, o, buf, &val)) {
			fprintf(stderr,
					"JS_GetProperty failed for \"%s\" in MFRotationAssign.\n",
					buf);
			return JS_FALSE;
		}

	    if (!JS_SetProperty(cx, obj, buf, &val)) {
			fprintf(stderr,
					"JS_SetProperty failed for \"%s\" in MFRotationAssign.\n",
					buf);
			return JS_FALSE;
		}
    }
    *rval = OBJECT_TO_JSVAL(obj); 
    return JS_TRUE;
}



JSBool
MFNodeAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFNodeAddProperty:\n");
	}
	return doMFAddProperty(cx, obj, id, vp);
}

JSBool
MFNodeSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFNodeSetProperty:\n");
	}
	return doMFSetProperty(cx, obj, id, vp);
}


JSBool
MFNodeConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	unsigned int i;
	char buf[SMALLSTRING];
	jsval v = INT_TO_JSVAL(argc);
	UNUSED(rval);

	if (!JS_DefineProperty(cx, obj, "length", v, NULL, NULL,
						   JSPROP_PERMANENT)) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"length\" in MFNodeConstr.\n");
		return JS_FALSE;
	}
	v = INT_TO_JSVAL(0);
	if (!JS_DefineProperty(cx, obj, "__touched_flag", v, NULL, NULL,
						   JSPROP_PERMANENT)) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"__touched_flag\" in MFNodeConstr.\n");
		return JS_FALSE;
	}
	if (!argv) {
		return JS_TRUE;
	}

	for (i = 0; i < argc; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
		/* XXX Check type */
		if (!JS_DefineProperty(cx, obj, buf, argv[i], JS_PropertyStub, JS_PropertyStub,
							   JSPROP_ENUMERATE)) {
			fprintf(stderr,
					"JS_DefineProperty failed for \"%s\" in MFNodeConstr.\n",
					buf);
			return JS_FALSE;
		}
	}
	return JS_TRUE;
}

JSBool
MFNodeAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval val, myv;
    int len, i;
    JSObject *o;
	char buf[SMALLSTRING];

    if (!JS_InstanceOf(cx, obj, &MFNodeClass, argv)) {
		fprintf(stderr, "JS_InstanceOf for obj failed in MFNodeAssign.\n");
        return JS_FALSE;
	}
    if (verbose) {
		printf("MFNodeAssign\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o",&o)) {
		fprintf(stderr, "JS_ConvertArguments failed in MFNodeAssign.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, o, &MFNodeClass, argv)) {
		fprintf(stderr, "JS_InstanceOf for o failed in MFNodeAssign.\n");
        return JS_FALSE;
    }
	/* Now, we assign length properties from o to obj */
	/* XXX HERE */
	myv = INT_TO_JSVAL(1);
    if (!JS_SetProperty(cx, obj, "__touched_flag", &myv)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"__touched_flag\" in MFNodeAssign.\n");
        return JS_FALSE;
	}
    if (!JS_GetProperty(cx, o, "length", &val)) {
		fprintf(stderr,
				"JS_GetProperty failed for \"length\" in MFNodeAssign.\n");
        return JS_FALSE;
	}
    if (!JS_SetProperty(cx, obj, "length", &val)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"length\" in MFNodeAssign.\n");
        return JS_FALSE;
	}
    len = JSVAL_TO_INT(val); /* XXX Assume int */

	for (i = 0; i < len; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
		if (!JS_GetProperty(cx, o, buf, &val)) {
			fprintf(stderr,
					"JS_SetProperty failed for \"%s\" in MFNodeAssign.\n",
					buf);
			return JS_FALSE;
		}
		if (!JS_SetProperty(cx, obj, buf, &val)) {
			fprintf(stderr,
					"JS_SetProperty failed for \"%s\" in MFNodeAssign.\n",
					buf);
			return JS_FALSE;
		}
	}
    *rval = OBJECT_TO_JSVAL(obj); 
    return JS_TRUE;
}


JSBool
MFStringAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFStringAddProperty:\n");
	}
	return doMFAddProperty(cx, obj, id, vp);
}


JSBool 
MFStringSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (verbose) {
		printf("MFStringSetProperty:\n");
	}
	return doMFSetProperty(cx, obj, id, vp);
}


JSBool
MFStringConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	unsigned int i;
	char buf[SMALLSTRING];
	jsval v = INT_TO_JSVAL(argc);
	UNUSED(rval);

	if (!JS_DefineProperty(cx, obj, "length", v, NULL, NULL,
						   JSPROP_PERMANENT)) {
		fprintf(stderr, "JS_DefineProperty failed for \"length\" in MFStringConstr.\n");
		return JS_FALSE;
	}
	v = INT_TO_JSVAL(0);
	if (!JS_DefineProperty(cx, obj, "__touched_flag", v, NULL, NULL,
						   JSPROP_PERMANENT)) {
		fprintf(stderr,
				"JS_DefineProperty failed for \"__touched_flag\" in MFStringConstr.\n");
		return JS_FALSE;
	}
	if (!argv) {
		return JS_TRUE;
	}

	for (i = 0; i < argc; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
		/* XXX Check type */
		if (!JS_DefineProperty(cx, obj, buf, argv[i], JS_PropertyStub, JS_PropertyStub,
							   JSPROP_ENUMERATE)) {
			fprintf(stderr,
					"JS_DefineProperty failed for \"%s\" in MFStringConstr.\n",
					buf);
			return JS_FALSE;
		}
	}
	return JS_TRUE;
}

JSBool
MFStringAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    jsval val, myv;
    int len, i;
    JSObject *o;
	char buf[SMALLSTRING];

	if (!JS_InstanceOf(cx, obj, &MFStringClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in MFStringAssign.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("MFStringAssign\n");
	}

	if (!JS_ConvertArguments(cx, argc, argv, "o",&o)) {
		fprintf(stderr, "JS_ConvertArguments failed in MFStringClass.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, o, &MFStringClass, argv)) {
		fprintf(stderr, "JS_InstanceOf failed in MFStringAssign.\n");
        return JS_FALSE;
    }
	/* Now, we assign length properties from o to obj */
	/* XXX HERE */
	myv = INT_TO_JSVAL(1);
    if (!JS_SetProperty(cx, obj, "__touched_flag", &myv)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"__touched_flag\" in MFStringAssign.\n");
        return JS_FALSE;
	}
    if (!JS_GetProperty(cx, o, "length", &val)) {
		fprintf(stderr,
				"JS_GetProperty failed for \"length\" in MFStringAssign.\n");
        return JS_FALSE;
	}
    if (!JS_SetProperty(cx, obj, "length", &val)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"length\" in MFStringAssign.\n");
        return JS_FALSE;
	}
    len = JSVAL_TO_INT(val); /* XXX Assume int */

	for (i = 0; i < len; i++) {
		memset(buf, 0, SMALLSTRING);
		sprintf(buf, "%d", i);
		if (!JS_GetProperty(cx, o, buf, &val)) {
			fprintf(stderr,
					"JS_GetProperty failed for \"%s\" in MFStringAssign.\n",
					buf);
			return JS_FALSE;
		}
		if (!JS_SetProperty(cx, obj, buf, &val)) {
			fprintf(stderr,
					"JS_SetProperty failed for \"%s\" in MFStringAssign.\n",
					buf);
			return JS_FALSE;
		}
	}
    *rval = OBJECT_TO_JSVAL(obj); 
    return JS_TRUE;
}


JSBool
SFNodeConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSString *str;
	char *p;
	UNUSED(rval);

	if (argc == 0) {
		fprintf(stderr, "SFNodeConstr failed: argc must be >= 1\n");
		return JS_FALSE;
	} else if (argc == 1) {
		fprintf(stderr,
				"SFNodeConstr failed: can't construct SFNodeClass from VRML(XXX FIXME).\n");
		return JS_FALSE;
	} else if (argc == 2) {
		str = JS_ValueToString(cx, argv[1]);
		p = JS_GetStringBytes(str);

		/* Hidden two-arg constructor: we construct it using an id... */
		if (verbose) {
			printf("SFNodeConstr: '%s'\n", p);
		}
		if (!JS_DefineProperty(cx, obj,"__id", argv[1], NULL, NULL, JSPROP_PERMANENT)) {
			fprintf(stderr, "JS_DefineProperty failed for \"__id\" in SFNodeConstr.\n");
			return JS_FALSE;
		}
		return JS_TRUE;
	} else {
		fprintf(stderr, "SFNodeConstr failed: argc must be < 3\n");
	}
	return JS_FALSE;
}


JSBool
SFNodeSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	JSObject *globalObj;
	BrowserIntern *brow;
	jsval pv, v = OBJECT_TO_JSVAL(obj);

	globalObj = JS_GetGlobalObject(cx);
	if (globalObj == NULL) {
		fprintf(stderr, "JS_GetGlobalObject failed in SFNodeSetProperty.\n");
		return JS_FALSE;
	}
	if (!JS_GetProperty(cx, globalObj, "Browser", &pv)) {
		fprintf(stderr, "JS_GetProperty failed for \"Browser\" in SFNodeSetProperty.\n");
		return JS_FALSE;
	}
	if (!JSVAL_IS_OBJECT(pv)) {
		fprintf(stderr, "jsval pv is not an object in SFNodeSetProperty.\n");
		return JS_FALSE;
	}
	
	if ((brow = JS_GetPrivate(cx, JSVAL_TO_OBJECT(pv))) == NULL) {
		fprintf(stderr, "JS_GetPrivate failed in SFNodeSetProperty.\n");
		return JS_FALSE;
	}

	if (!JS_SetProperty(cx, globalObj, "__node", &v)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"__node\" in SFNodeSetProperty.\n");
		return JS_FALSE;
	}
	if (!JS_SetProperty(cx, globalObj, "__prop", &id)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"__prop\" in SFNodeSetProperty.\n");
		return JS_FALSE;
	}

	if (!JS_SetProperty(cx, globalObj, "__val", vp)) {
		fprintf(stderr,
				"JS_SetProperty failed for \"__val\" in SFNodeSetProperty.\n");
		return JS_FALSE;
	}
	if (verbose) {
		printf("SFNodeSetProperty\n");
	}
	doPerlCallMethod(brow->jssv, "nodeSetProperty");
/* 	dSP; */
/* 	ENTER; */
/* 	SAVETMPS; */
/* 	PUSHMARK(sp); */
/* 	XPUSHs(brow->js_sv); */
/* 	PUTBACK; */
/* 	count = perl_call_method("node_setprop", G_SCALAR); */
/* 	if (count) { */
/* 		if (verbose) printf("Got return %f\n",POPn); */
/* 	} */
/* 	PUTBACK; */
/* 	FREETMPS; */
/* 	LEAVE; */
	return JS_TRUE;
}

JSBool
SFNodeGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	UNUSED(cx);
	UNUSED(obj);
	UNUSED(id);
	UNUSED(vp);
	if (verbose) {
		printf("SFNodeGetProperty\n");
	}
	return JS_TRUE;
}

