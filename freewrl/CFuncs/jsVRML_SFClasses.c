/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart, Ayla Khan CRC Canada
 * 2007 John Stewart CRC Canada.
 *
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 *
 * $Id: jsVRML_SFClasses.c,v 1.1 2007/05/11 13:09:31 crc_canada Exp $
 *
 */
#include "headers.h"
#include "jsVRMLClasses.h"


/********************************************************/
/*							*/
/* Second part - SF classes				*/
/*							*/
/********************************************************/

/* from http://www.cs.rit.edu/~ncs/color/t_convert.html */
double MIN(double a, double b, double c) {
	double min;
	if((a<b)&&(a<c))min=a; else if((b<a)&&(b<c))min=b; else min=c; return min;
}

double MAX(double a, double b, double c) {
	double max;
	if((a>b)&&(a>c))max=a; else if((b>a)&&(b>c))max=b; else max=c; return max;
}

void convertRGBtoHSV(double r, double g, double b, double *h, double *s, double *v) {
	double min, max, delta;

	min = MIN( r, g, b );
	max = MAX( r, g, b );
	*v = max;				/* v */
	delta = max - min;
	if( max != 0 )
		*s = delta / max;		/* s */
	else {
		/* r = g = b = 0 */		/* s = 0, v is undefined */
		*s = 0;
		*h = -1;
		return;
	}
	if( r == max )
		*h = ( g - b ) / delta;		/* between yellow & magenta */
	else if( g == max )
		*h = 2 + ( b - r ) / delta;	/* between cyan & yellow */
	else
		*h = 4 + ( r - g ) / delta;	/* between magenta & cyan */
	*h *= 60;				/* degrees */
	if( *h < 0 )
		*h += 360;
}
void convertHSVtoRGB( double h, double s, double v ,double *r, double *g, double *b)
{
	int i;
	double f, p, q, t;
	if( s == 0 ) {
		/* achromatic (grey) */
		*r = *g = *b = v;
		return;
	}
	h /= 60;			/* sector 0 to 5 */
	i = floor( h );
	f = h - i;			/* factorial part of h */
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );
	switch( i ) {
		case 0: *r = v; *g = t; *b = p; break;
		case 1: *r = q; *g = v; *b = p; break;
		case 2: *r = p; *g = v; *b = t; break;
		case 3: *r = p; *g = q; *b = v; break;
		case 4: *r = t; *g = p; *b = v; break;
		default: *r = v; *g = p; *b = q; break;
	}
}

JSBool
SFColorGetHSV(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *result;
	jsdouble *dp;
	double xp[3];
	jsval _h; jsval _s; jsval _v;
	SFColorNative *ptr;
	int i;

	UNUSED(argv);
	if (argc != 0) {
		printf ("SFColorGetHSV; arguments found but not expected\n");
		return JS_FALSE;
	}
	
	/* get the RGB values */
        if ((ptr = (SFColorNative *)JS_GetPrivate(cx, obj)) == NULL) {
                printf( "JS_GetPrivate failed in SFColorToString.\n");
                return JS_FALSE;
        }

	/* convert rgb to hsv */
	convertRGBtoHSV((ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2],&xp[0],&xp[1],&xp[2]);

	#ifdef JSVRMLCLASSESVERBOSE
        printf("hsv code, orig rgb is %.9g %.9g %.9g\n", (ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	printf ("hsv conversion is %lf %lf %lf\n",xp[0],xp[1],xp[2]);
	#endif

	result = JS_NewArrayObject(cx, 3, NULL); 
        JS_AddRoot(cx, &result); 
        for(i=0; i<3; i++) { 
		if ((dp = JS_NewDouble(cx, xp[i])) == NULL) {
			printf( "JS_NewDouble failed for %f in SFColorGetHSV.\n", xp[i]);
			return JS_FALSE;
		}
		_v = DOUBLE_TO_JSVAL(dp);
        	JS_SetElement(cx, result, (jsint)i, &_v); 
        } 

        JS_RemoveRoot(cx, &result); 
        *rval = OBJECT_TO_JSVAL(result); 
	return JS_TRUE;
}

JSBool
SFColorSetHSV(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    SFColorNative *ptr;
	double hue, saturation, value;
	double red,green,blue;

	if ((ptr = (SFColorNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorToString.\n");
		return JS_FALSE;
	}
	if (!JS_ConvertArguments(cx, argc, argv, "d d d", &hue, &saturation, &value)) {
		printf( "JS_ConvertArguments failed in SFColorSetHSV.\n");
		return JS_FALSE;
	}

	/* do conversion here!!! */
	#ifdef JSCLASSESVERBOSE
        printf("hsv code, orig rgb is %.9g %.9g %.9g\n", (ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	printf ("SFColorSetHSV, have %lf %lf %lf\n",hue,saturation,value);
	#endif

	convertHSVtoRGB(hue,saturation,value, &red, &green, &blue);
	ptr->v.c[0] = red;
	ptr->v.c[1] = green;
	ptr->v.c[2] = blue;
	ptr->touched++;
	#ifdef JSCLASSESVERBOSE
        printf("hsv code, now rgb is %.9g %.9g %.9g\n", (ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	#endif

	*rval = OBJECT_TO_JSVAL(obj);

    return JS_TRUE;
}

JSBool
SFColorToString(JSContext *cx, JSObject *obj,
				uintN argc, jsval *argv, jsval *rval)
{
    SFColorNative *ptr;
    JSString *_str;
	char _buff[STRING];

	UNUSED(argc);
	UNUSED(argv);
	if ((ptr = (SFColorNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorToString.\n");
		return JS_FALSE;
	}

	memset(_buff, 0, STRING);
	sprintf(_buff, "%.9g %.9g %.9g",
			(ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	_str = JS_NewStringCopyZ(cx, _buff);
    *rval = STRING_TO_JSVAL(_str);

    return JS_TRUE;
}

JSBool
SFColorAssign(JSContext *cx, JSObject *obj,
			   uintN argc, jsval *argv, jsval *rval)
{
    JSObject *_from_obj;
    SFColorNative *ptr, *fptr;
    char *_id_str;

	if ((ptr = (SFColorNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFColorAssign.\n");
        return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, obj, &SFColorClass, argv)) {
		printf( "JS_InstanceOf failed for obj in SFColorAssign.\n");
        return JS_FALSE;
	}
	if (!JS_ConvertArguments(cx, argc, argv, "o s", &_from_obj, &_id_str)) {
		printf( "JS_ConvertArguments failed in SFColorAssign.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, _from_obj, &SFColorClass, argv)) {
		printf( "JS_InstanceOf failed for _from_obj in SFColorAssign.\n");
        return JS_FALSE;
    }
	if ((fptr = (SFColorNative *)JS_GetPrivate(cx, _from_obj)) == NULL) {
		printf( "JS_GetPrivate failed for _from_obj in SFColorAssign.\n");
        return JS_FALSE;
	}
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorAssign: obj = %u, id = \"%s\", from = %u\n",
			   VERBOSE_OBJ obj, _id_str, VERBOSE_OBJ _from_obj);
	#endif

    SFColorNativeAssign(ptr, fptr);
    *rval = OBJECT_TO_JSVAL(obj);

    return JS_TRUE;
}

JSBool
SFColorTouched(JSContext *cx, JSObject *obj,
			   uintN argc, jsval *argv, jsval *rval)
{
    SFColorNative *ptr;
    int t;

	UNUSED(argc);
	UNUSED(argv);
	if ((ptr = (SFColorNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFSFColorTouched.\n");
		return JS_FALSE;
	}

    t = ptr->touched;
	ptr->touched = 0;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorTouched: obj = %u, touched = %d\n", VERBOSE_OBJ obj, t);
	#endif
	
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}

JSBool
SFColorConstr(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval)
{
	SFColorNative *ptr;
	jsdouble pars[3];

	if ((ptr = (SFColorNative *) SFColorNativeNew()) == NULL) {
		printf( "SFColorNativeNew failed in SFColorConstr.\n");
		return JS_FALSE;
	}

	if (!JS_DefineProperties(cx, obj, SFColorProperties)) {
		printf( "JS_DefineProperties failed in SFColorConstr.\n");
		return JS_FALSE;
	}

	if (!JS_SetPrivate(cx, obj, ptr)) {
		printf( "JS_SetPrivate failed in SFColorConstr.\n");
		return JS_FALSE;
	}

	if (argc == 0) {
		(ptr->v).c[0] = 0.0;
		(ptr->v).c[1] = 0.0;
		(ptr->v).c[2] = 0.0;
	} else if (JS_ConvertArguments(cx, argc, argv, "d d d", &(pars[0]), &(pars[1]), &(pars[2]))) {
		(ptr->v).c[0] = pars[0];
		(ptr->v).c[1] = pars[1];
		(ptr->v).c[2] = pars[2];
	} else {
		printf( "Invalid arguments for SFColorConstr.\n");
		return JS_FALSE;
	}
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorConstr: obj = %u, %u args, %f %f %f\n",
			   VERBOSE_OBJ obj, argc,
			   (ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	#endif
	*rval = OBJECT_TO_JSVAL(obj);

	return JS_TRUE;
}

void
SFColorFinalize(JSContext *cx, JSObject *obj)
{
	SFColorNative *ptr;

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorFinalize: obj = %u\n", VERBOSE_OBJ obj);
	#endif
	if ((ptr = (SFColorNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorFinalize.\n");
		return;
	}
	SFColorNativeDelete(ptr);
}

JSBool
SFColorGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFColorNative *ptr;
	jsdouble d, *dp;

	if ((ptr = (SFColorNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorGetProperty.\n");
		return JS_FALSE;
	}
	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			d = (ptr->v).c[0];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFColorGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 1:
			d = (ptr->v).c[1];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFColorGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 2:
			d = (ptr->v).c[2];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFColorGetProperty.\n",
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
SFColorSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFColorNative *ptr;
	jsval _val;

	if ((ptr = (SFColorNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorSetProperty.\n");
		return JS_FALSE;
	}
	ptr->touched++;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorSetProperty: obj = %u, id = %d, touched = %d\n",
			   VERBOSE_OBJ obj, JSVAL_TO_INT(id), ptr->touched);
	#endif

	if (!JS_ConvertValue(cx, *vp, JSTYPE_NUMBER, &_val)) {
		printf( "JS_ConvertValue failed in SFColorSetProperty.\n");
		return JS_FALSE;
	}

	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			(ptr->v).c[0] = *JSVAL_TO_DOUBLE(_val);
			break;
		case 1:
			(ptr->v).c[1] = *JSVAL_TO_DOUBLE(_val);
			break;
		case 2:
			(ptr->v).c[2] = *JSVAL_TO_DOUBLE(_val);
			break;

		}
	}
	return JS_TRUE;
}

/* copy code from SFColorGetHSV if the spec ever decides to implement this. */
JSBool
SFColorRGBAGetHSV(JSContext *cx, JSObject *obj,
				uintN argc, jsval *argv, jsval *rval)
{
	JSObject *_arrayObj;
	jsdouble hue = 0, saturation = 0, value = 0;
	jsval _v;

	UNUSED(obj);
	UNUSED(argc);
	UNUSED(argv);
	/* do conversion here!!! */

	if ((_arrayObj = JS_NewArrayObject(cx, 0, NULL)) == NULL) {
		printf( "JS_NewArrayObject failed in SFColorRGBAGetHSV.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(_arrayObj);

	/* construct new double before conversion? */
	_v = DOUBLE_TO_JSVAL(&hue);
	if (!JS_SetElement(cx, _arrayObj, 0, &_v)) {
		printf( "JS_SetElement failed for hue in SFColorRGBAGetHSV.\n");
		return JS_FALSE;
	}
	_v = DOUBLE_TO_JSVAL(&saturation);
	if (!JS_SetElement(cx, _arrayObj, 1, &_v)) {
		printf( "JS_SetElement failed for saturation in SFColorRGBAGetHSV.\n");
		return JS_FALSE;
	}

	_v = DOUBLE_TO_JSVAL(&value);
	if (!JS_SetElement(cx, _arrayObj, 2, &_v)) {
		printf( "JS_SetElement failed for value in SFColorRGBAGetHSV.\n");
		return JS_FALSE;
	}

    return JS_TRUE;
}

/* implement later?? Copy most of code from SFColorSetHSV if we require this */
JSBool
SFColorRGBASetHSV(JSContext *cx, JSObject *obj,
				uintN argc, jsval *argv, jsval *rval)
{
    SFColorRGBANative *ptr;
	jsdouble hue, saturation, value;

	if ((ptr = (SFColorRGBANative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorRGBAToString.\n");
		return JS_FALSE;
	}
	if (!JS_ConvertArguments(cx, argc, argv, "d d d",
							 &hue, &saturation, &value)) {
		printf( "JS_ConvertArguments failed in SFColorRGBASetHSV.\n");
		return JS_FALSE;
	}

	/* do conversion here!!! */

	*rval = OBJECT_TO_JSVAL(obj);

    return JS_TRUE;
}

JSBool
SFColorRGBAToString(JSContext *cx, JSObject *obj,
				uintN argc, jsval *argv, jsval *rval)
{
    SFColorRGBANative *ptr;
    JSString *_str;
	char _buff[STRING];

	UNUSED(argc);
	UNUSED(argv);
	if ((ptr = (SFColorRGBANative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorRGBAToString.\n");
		return JS_FALSE;
	}

	memset(_buff, 0, STRING);
	sprintf(_buff, "%.9g %.9g %.9g %.9g",
			(ptr->v).r[0], (ptr->v).r[1], (ptr->v).r[2],(ptr->v).r[3]);
	_str = JS_NewStringCopyZ(cx, _buff);
    *rval = STRING_TO_JSVAL(_str);

    return JS_TRUE;
}

JSBool
SFColorRGBAAssign(JSContext *cx, JSObject *obj,
			   uintN argc, jsval *argv, jsval *rval)
{
    JSObject *_from_obj;
    SFColorRGBANative *ptr, *fptr;
    char *_id_str;

	if ((ptr = (SFColorRGBANative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFColorRGBAAssign.\n");
        return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, obj, &SFColorRGBAClass, argv)) {
		printf( "JS_InstanceOf failed for obj in SFColorRGBAAssign.\n");
        return JS_FALSE;
	}
	if (!JS_ConvertArguments(cx, argc, argv, "o s", &_from_obj, &_id_str)) {
		printf( "JS_ConvertArguments failed in SFColorRGBAAssign.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, _from_obj, &SFColorRGBAClass, argv)) {
		printf( "JS_InstanceOf failed for _from_obj in SFColorRGBAAssign.\n");
        return JS_FALSE;
    }
	if ((fptr = (SFColorRGBANative *)JS_GetPrivate(cx, _from_obj)) == NULL) {
		printf( "JS_GetPrivate failed for _from_obj in SFColorRGBAAssign.\n");
        return JS_FALSE;
	}
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorRGBAAssign: obj = %u, id = \"%s\", from = %u\n",
			   VERBOSE_OBJ obj, _id_str, VERBOSE_OBJ _from_obj);
	#endif

    SFColorRGBANativeAssign(ptr, fptr);
    *rval = OBJECT_TO_JSVAL(obj);

    return JS_TRUE;
}

JSBool
SFColorRGBATouched(JSContext *cx, JSObject *obj,
			   uintN argc, jsval *argv, jsval *rval)
{
    SFColorRGBANative *ptr;
    int t;

	UNUSED(argc);
	UNUSED(argv);
	if ((ptr = (SFColorRGBANative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFSFColorRGBATouched.\n");
		return JS_FALSE;
	}

    t = ptr->touched;
	ptr->touched = 0;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorRGBATouched: obj = %u, touched = %d\n", VERBOSE_OBJ obj, t);
	#endif
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}

JSBool
SFColorRGBAConstr(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval)
{
	SFColorRGBANative *ptr;
	jsdouble pars[3];

	if ((ptr = (SFColorRGBANative *) SFColorNativeNew()) == NULL) {
		printf( "SFColorRGBANativeNew failed in SFColorConstr.\n");
		return JS_FALSE;
	}

	if (!JS_DefineProperties(cx, obj, SFColorRGBAProperties)) {
		printf( "JS_DefineProperties failed in SFColorRGBAConstr.\n");
		return JS_FALSE;
	}

	if (!JS_SetPrivate(cx, obj, ptr)) {
		printf( "JS_SetPrivate failed in SFColorRGBAConstr.\n");
		return JS_FALSE;
	}

	if (argc == 0) {
		(ptr->v).r[0] = 0.0;
		(ptr->v).r[1] = 0.0;
		(ptr->v).r[2] = 0.0;
		(ptr->v).r[3] = 0.0;
	} else if (JS_ConvertArguments(cx, argc, argv, "d d d d",
					&(pars[0]), &(pars[1]), &(pars[2]), &(pars[3]))) {
		(ptr->v).r[0] = pars[0];
		(ptr->v).r[1] = pars[1];
		(ptr->v).r[2] = pars[2];
		(ptr->v).r[3] = pars[3];
	} else {
		printf( "Invalid arguments for SFColorRGBAConstr.\n");
		return JS_FALSE;
	}
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorRGBAConstr: obj = %u, %u args, %f %f %fi %f\n",
			   VERBOSE_OBJ obj, argc,
			   (ptr->v).r[0], (ptr->v).r[1], (ptr->v).r[2],(ptr->v).r[3]);
	#endif
	*rval = OBJECT_TO_JSVAL(obj);

	return JS_TRUE;
}

void
SFColorRGBAFinalize(JSContext *cx, JSObject *obj)
{
	SFColorRGBANative *ptr;

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorRGBAFinalize: obj = %u\n", VERBOSE_OBJ obj);
	#endif
	if ((ptr = (SFColorRGBANative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorRGBAFinalize.\n");
		return;
	}
	SFColorRGBANativeDelete(ptr);
}

JSBool
SFColorRGBAGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFColorRGBANative *ptr;
	jsdouble d, *dp;

	if ((ptr = (SFColorRGBANative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorRGBAGetProperty.\n");
		return JS_FALSE;
	}
	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			d = (ptr->v).r[0];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFColorRGBAGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 1:
			d = (ptr->v).r[1];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFColorRGBAGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 2:
			d = (ptr->v).r[2];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFColorRGBAGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 3:
			d = (ptr->v).r[3];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFColorRGBAGetProperty.\n",
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
SFColorRGBASetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFColorRGBANative *ptr;
	jsval _val;

	if ((ptr = (SFColorRGBANative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFColorRGBASetProperty.\n");
		return JS_FALSE;
	}
	ptr->touched++;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorRGBASetProperty: obj = %u, id = %d, touched = %d\n",
			   VERBOSE_OBJ obj, JSVAL_TO_INT(id), ptr->touched);
	#endif

	if (!JS_ConvertValue(cx, *vp, JSTYPE_NUMBER, &_val)) {
		printf( "JS_ConvertValue failed in SFColorRGBASetProperty.\n");
		return JS_FALSE;
	}

	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			(ptr->v).r[0] = *JSVAL_TO_DOUBLE(_val);
			break;
		case 1:
			(ptr->v).r[1] = *JSVAL_TO_DOUBLE(_val);
			break;
		case 2:
			(ptr->v).r[2] = *JSVAL_TO_DOUBLE(_val);
			break;
		case 3:
			(ptr->v).r[3] = *JSVAL_TO_DOUBLE(_val);
			break;

		}
	}
	return JS_TRUE;
}

JSBool
SFImageTouched(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    SFImageNative *ptr;
    int t;

	UNUSED(argc);
	UNUSED(argv);
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFImageTouched: obj = %u, %u args\n", VERBOSE_OBJ obj, argc);
	#endif

	if ((ptr = (SFImageNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFSFImageTouched.\n");
		return JS_FALSE;
	}

    t = ptr->touched;
	ptr->touched = 0;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFImageTouched: obj = %u, touched = %d\n", VERBOSE_OBJ obj, t);
	#endif
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}

JSBool
SFImageToString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFImageToString: obj = %u, %u args\n", VERBOSE_OBJ obj, argc);
	#endif

	UNUSED(argc);
	UNUSED(argv);
	return doMFToString(cx, obj, "SFImage", rval);
}

JSBool
SFImageAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFImageAssign: obj = %u, %u args\n", VERBOSE_OBJ obj, argc);
	#endif

	return _standardMFAssign (cx, obj, argc, argv, rval, &SFImageClass,"SFImageAssign");
}

JSBool
SFImageConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSString *_str;
	unsigned int i;
	jsval mv;
	int param[3];
	int expectedSize;

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFImageConstr: obj = %u, %u args\n", VERBOSE_OBJ obj, argc);
	#endif

	/* make this so that one can get the ".x", ".y", ".comp" and ".array" */
	if (!JS_DefineProperties(cx, obj, SFImageProperties)) {
		printf( "JS_DefineProperties failed in SFImageConstr.\n");
		return JS_FALSE;
	}

	/* null image. Make this [0, 0, 0] NOTE - there are only 3 elements now! */
	if (!argc) { 
		/* expect arguments to be number, number, number, mfint32 */
		mv = INT_TO_JSVAL(0);
		for (i=0; i<4; i++) {
			if (i==3) {
				MFInt32Constr(cx, obj, 0, NULL, &mv);
			}
			if (!JS_DefineElement(cx, obj, (jsint) i, mv,
			  JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE)) {
				printf( "JS_DefineElement failed for arg %d in SFImageConstr.\n", i);
				return JS_FALSE;
			}
		}
		mv = INT_TO_JSVAL(4);
		if (!JS_DefineProperty(cx, obj, "length", mv, JS_PropertyStub, JS_PropertyStub, JSPROP_PERMANENT)) {
			printf( "JS_DefineProperty failed for \"length\" in SFImageConstr.\n");
			return JS_FALSE;
		}

		mv = INT_TO_JSVAL(0);
		if (!JS_DefineProperty(cx, obj, "__touched_flag", mv, JS_PropertyStub, JS_PropertyStub, JSPROP_PERMANENT)) {
			printf( "JS_DefineProperty failed for \"__touched_flag\" in SFImageConstr.\n");
			return JS_FALSE;
		}


		return JS_TRUE; 
	}
	
	/* ok, ok. There are some parameters here. There had better be 4, or else... */
	if (argc != 4) {
		printf ("SFImageConstr, expect 4 parameters, got %d\n",argc);
		return JS_FALSE;
	}

	mv = INT_TO_JSVAL(4);
	if (!JS_DefineProperty(cx, obj, "length", mv, JS_PropertyStub, JS_PropertyStub, JSPROP_PERMANENT)) {
		printf( "JS_DefineProperty failed for \"length\" in SFImageConstr.\n");
		return JS_FALSE;
	}

	mv = INT_TO_JSVAL(0);
	if (!JS_DefineProperty(cx, obj, "__touched_flag", mv, JS_PropertyStub, JS_PropertyStub, JSPROP_PERMANENT)) {
		printf( "JS_DefineProperty failed for \"__touched_flag\" in SFImageConstr.\n");
		return JS_FALSE;
	}

	/* expect arguments to be number, number, number, mfint32 */
	for (i=0; i<3; i++) {
		/* printf ("looking at parameter %d\n",i); */
        	if (JSVAL_IS_INT(argv[i])) { 
			/* printf ("parameter is a number\n"); */
                	param[i] =  JSVAL_TO_INT(argv[i]);
			/* printf ("param is %d\n",param[i]); */
        	} else {        
                	printf ("SFImageConstr: parameter %d is not a number\n",i);
                	return JS_FALSE;
		}
	}
	/* now look at the MFInt32 array, and tack it on here */
	expectedSize = param[0] * param[1];

	
	/* the third number should be in the range of 0-4 inclusive (number of components in image) */
	if ((param[2]<0) || (param[2]>4)) {
			
		printf ("SFImageConstr: with size > 0, comp must be between 1 and 4 inclusive, got %d\n",param[2]);
		return JS_FALSE;
	}

	/* case 1 of null initializer */
	if ((expectedSize == 0) && (param[2] != 0)) {
		printf ("SFImageConstr: with x and y equal to zero, comp must be zero\n");
		return JS_FALSE;
	}
	/* case 2 of null initializer */
	if ((expectedSize != 0) && (param[2] == 0)) {
		printf ("SFImageConstr: with x and y not zero, comp must be non-zero\n");
		return JS_FALSE;
	}

	#ifdef JSVRMLCLASSESVERBOSE
	printJSNodeType(cx,argv[3]);
	#endif

	/* worry about the MFInt32 array. Note that we copy the object pointer here. Should
	   we copy ALL of the elements, or just the object itself?? */

	if (!JS_InstanceOf(cx, argv[3], &MFInt32Class, NULL)) {
		printf ("SFImageConstr: expected array element to be an MFInt32 array\n");
		return JS_FALSE;
	} else {
		if (!JS_GetProperty(cx, argv[3], "length", &mv)) {
			printf( "JS_GetProperty failed for MFInt32 length in SFNodeConstr\n");
        		return JS_FALSE;
		}
        	if (expectedSize != JSVAL_TO_INT(mv)) {
			printf ("SFImageConstr: expected %d elements in image data, got %d\n",expectedSize, JSVAL_TO_INT(mv));
			return JS_FALSE;
		}


	}

	/* parameters are ok - just save them now in the new object. */
	for (i=0; i<4; i++) {
		if (!JS_DefineElement(cx, obj, (jsint) i, argv[i],
			  JS_PropertyStub, JS_PropertyStub, JSPROP_ENUMERATE)) {
			printf( "JS_DefineElement failed for arg %d in SFImageConstr.\n", i);
			return JS_FALSE;
		}
	}
	
	/* if we are here, we must have had some success... */
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

JSBool
SFImageAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return doMFAddProperty(cx, obj, id, vp,"SFImageAddProperty");
}

JSBool
SFImageGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return _standardMFGetProperty(cx, obj, id, vp, "_FreeWRL_Internal = 0", "SFImage");
}

JSBool
SFImageSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
	return doMFSetProperty(cx, obj, id, vp,"SFImageSetProperty");
}


/**********************************************************************************/

/* returns a string rep of the pointer to the node in memory */
JSBool
SFNodeToString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSString *_str;
	SFNodeNative *ptr;

	UNUSED(argc);
	UNUSED(argv);
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("SFNODETOSTRING\n");
	#endif
	if ((ptr = (SFNodeNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFNodeToString.\n");
		return JS_FALSE;
	}

	/* get the string from creation, and return it. */
#ifdef WANTVRMLSTRING
	_str = JS_NewStringCopyZ(cx, ptr->X3DString);
	*rval = STRING_TO_JSVAL(_str);
#else
	*rval = INT_TO_JSVAL(ptr->handle);
#endif

	return JS_TRUE;
}

JSBool
SFNodeAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *_from_obj;
	SFNodeNative *fptr, *ptr;
	char *_id_str;

	/* unsigned int toptr; */
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFNodeAssign argc %d\n",argc);
	#endif

	/* are we saving to a SFNode? */
	if (!JS_InstanceOf(cx, obj, &SFNodeClass, argv)) {
		printf( "JS_InstanceOf failed for obj in SFNodeAssign.\n");
                        printJSNodeType(cx,obj);
	    return JS_FALSE;
	}

	/* get the pointer to the internal stuff */
	if ((ptr = (SFNodeNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFNodeAssign.\n");
	    return JS_FALSE;
	}

	/* get the from, and the string */
	printf ("SFNodeAssign, we have %d and %d\n",argv[0], argv[1]);
	if (!JS_ConvertArguments(cx, argc, argv, "o s", &_from_obj, &_id_str)) {
		printf( "JS_ConvertArguments failed in SFNodeAssign.\n");
		return JS_FALSE;
	}
	if (_from_obj != NULL) {
		if (!JS_InstanceOf(cx, _from_obj, &SFNodeClass, argv)) {
			printf( "JS_InstanceOf failed for _from_obj in SFNodeAssign.\n");
			printJSNodeType(cx,_from_obj); printf ("\n");
		    return JS_FALSE;
		}
		if ((fptr = (SFNodeNative *)JS_GetPrivate(cx, _from_obj)) == NULL) {
			printf( "JS_GetPrivate failed for _from_obj in SFNodeAssign.\n");
		    return JS_FALSE;
		}
		#ifdef JSVRMLCLASSESVERBOSE
			printf("SFNodeAssign: obj = %u, id = \"%s\", from = %u\n",
				   VERBOSE_OBJ obj, _id_str, VERBOSE_OBJ _from_obj);
		#endif
	} else { fptr = NULL; }

	/* assign this internally */
	if (!SFNodeNativeAssign(ptr, fptr)) {
		printf( "SFNodeNativeAssign failed in SFNodeAssign.\n");
	    return JS_FALSE;
	}

	*rval = OBJECT_TO_JSVAL(obj);

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("end of SFNodeAssign\n");
	#endif
	return JS_TRUE;
}

JSBool
SFNodeTouched(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval)
{
    SFNodeNative *ptr;
    int t;

	UNUSED(argc);
	UNUSED(argv);

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFNodeTouched\n");
	#endif
	if ((ptr = (SFNodeNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFNodeTouched.\n");
		return JS_FALSE;
	}

	t = ptr->touched;
	ptr->touched = 0;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFNodeTouched: obj = %u, touched = %d\n",
			   VERBOSE_OBJ obj, t);
	#endif
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}

JSBool SFNodeConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
	SFNodeNative *newPtr;
	SFNodeNative *oldPtr;

	uintptr_t *newHandle;
	JSString *myStr;
	char *cString;
	int tmp;

	struct X3D_Group *myGroup;

	newHandle = 0;
	cString = NULL;

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("Start of SFNodeConstr argc %d\n",argc);
	#endif

	/* verify the argc */
	if (argc == 0) {
		newHandle = 0;
		cString = strdup("SFNodeConstr from argc eq 0");
	} else if (argc == 1) {
		/* is this a string, or a number indicating a node? */
		myStr = JS_ValueToString(cx, argv[0]);
		cString = JS_GetStringBytes(myStr);
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("SFNodeConstr, argc =1l string %s\n",cString);
		#endif

		/* this is either a memory pointer, or it is actual X3D text, or it is junk */
		if (JSVAL_IS_OBJECT(argv[0])) {
			#ifdef JSVRMLCLASSESVERBOSE
			printf ("SFNodeConstr, cstring was an object\n");
			#endif

        		if ((oldPtr = (SFNodeNative *)JS_GetPrivate(cx, (JSObject *)argv[0])) == NULL) {
                		printf( "JS_GetPrivate failed in SFNodeConstr.\n");
                		return JS_FALSE;
        		}

			newHandle = oldPtr->handle;
			cString = strdup(oldPtr->X3DString);

		} else {
			#ifdef JSVRMLCLASSESVERBOSE
			printf ("SFNodeConstr, cstring was NOT an object\n");
			#endif

			/* is this just an integer, eg, "0" - happens on initialization for SFNodes */
			if (JSVAL_IS_INT(argv[0])) {
				sscanf (cString,"%ld",&newHandle);
				cString = strdup("node created in SFNodeConstr");
			} else {

				/* try compiling this X3D code... */
				myGroup = (struct X3D_Group *) createNewX3DNode(NODE_Group);
	                        inputParse(FROMSTRING, cString, FALSE, FALSE, myGroup, offsetof(struct X3D_Group, children), 
					&tmp, FALSE);
	
				#ifdef JSVRMLCLASSESVERBOSE
				printf ("SFNodeConstr we have created %d nodes\n",myGroup->children.n);
				#endif
	
				/* we MUST create 1 node here; if not, there is an error */
				if ((myGroup->children.n) != 1) {
					printf ("SFNativeNew - created %d nodes, expected 1 only\n",myGroup->children.n);
					return JS_FALSE;
				}
				newHandle = (uintptr_t *) myGroup->children.p[0];
			}
		}	

	} else if (argc == 2) {
		/* eg, createVrmlFromString will send a bunch of SFNodes to a MFNode with text */

		#ifdef JSVRMLCLASSESVERBOSE
		printf ("SFNodeConstr - have 2 arguments\n");
		#endif

		if ((JSVAL_IS_STRING(argv[0])) && (JSVAL_IS_STRING(argv[1]))) {
        		JSString *_idStr;
        		char *_id_c;

			_idStr = JS_ValueToString(cx, argv[0]);
			_id_c = JS_GetStringBytes(_idStr);
			cString = strdup(_id_c);

			_idStr = JS_ValueToString(cx, argv[1]);
			_id_c = JS_GetStringBytes(_idStr);

			if (sscanf (_id_c,"%d",&newHandle) != 1) {
				printf ("SFNodeConstr - can not get handle from %s\n",_id_c);
				return JS_FALSE;
			}
			#ifdef JSVRMLCLASSESVERBOSE
			printf ("string is :%s: new handle is %d\n",cString,newHandle);
			#endif

		} else {
			printf ("SFNodeConstr - 2 args, expected 2 strings\n");
			return JS_FALSE;
		}


	} else {
		printf( "SFNodeConstr requires at least 1 string arg.\n");
		return JS_FALSE;
	}


	/* ok, so far so good... */
	if ((newPtr = (SFNodeNative *) SFNodeNativeNew()) == NULL) {
		printf( "SFNodeNativeNew failed in SFNodeConstr.\n");
		return JS_FALSE;
	}

	if (!JS_DefineProperties(cx, obj, SFNodeProperties)) {
		printf( "JS_DefineProperties failed in SFNodeConstr.\n");
		return JS_FALSE;
	}

	if (!JS_SetPrivate(cx, obj, newPtr)) {
		printf( "JS_SetPrivate failed in SFNodeConstr.\n");
		return JS_FALSE;
	}

	newPtr->handle = newHandle;
	newPtr->X3DString = (char *)strdup(cString);

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFNodeConstr: created obj = %u, argc: %u mem ptr: %d text string: %s\n",
			   VERBOSE_OBJ obj, argc, newHandle, cString);
	#endif
	*rval = OBJECT_TO_JSVAL(obj);

	return JS_TRUE;
}

void
SFNodeFinalize(JSContext *cx, JSObject *obj)
{
	SFNodeNative *ptr;

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFNodeFinalize: obj = %u\n", VERBOSE_OBJ obj);
	#endif
	if ((ptr = (SFNodeNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFNodeFinalize.\n");
		return;
	}
	SFNodeNativeDelete(ptr);
}

JSBool
SFNodeGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{

	/* We can't really get a property of a SFNode. There are no sub-indexes, etc...
	   so we don't do anything. Check out SFVec3fGetProperty to see how it handles
	   properties, should we need to have properties in the future. */

	UNUSED (cx);
	UNUSED (obj);
	UNUSED (id);
	UNUSED (vp);
	return JS_TRUE;
}

JSBool
SFNodeSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	JSString *_idStr, *_valStr;
	char *_id_c, *_val_c;
	SFNodeNative *ptr;
	int val_len;
	int retint;
	uintptr_t ra;


	_idStr = JS_ValueToString(cx, id);
	_id_c = JS_GetStringBytes(_idStr);

	_valStr = JS_ValueToString(cx, *vp);
	_val_c = JS_GetStringBytes(_valStr);

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFNodeSetProperty: obj = %u, id = %s, vp = %s\n",
			   VERBOSE_OBJ obj, _id_c, _val_c);
	#endif


	if ((ptr = (SFNodeNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFNodeSetProperty.\n");
		return JS_FALSE;
	}

	if (JSVAL_IS_INT(id)) {
		ptr->touched++;
		val_len = strlen(_val_c) + 1;

		#ifdef JSVRMLCLASSESVERBOSE
		printf ("switching on %d\n",JSVAL_TO_INT(id));
		#endif

		switch (JSVAL_TO_INT(id)) {
		case 0:
			if ((strlen(ptr->X3DString) + 1) > val_len) {
				ptr->X3DString =
					(char *) REALLOC (ptr->X3DString, val_len * sizeof(char));
			}
			memset(ptr->X3DString, 0, val_len);
			memmove(ptr->X3DString, _val_c, val_len);
			break;
		case 1:
			if ((strlen(ptr->handle) + 1) > val_len) {
				ptr->handle =
					(char *) REALLOC (ptr->handle, val_len * sizeof(char));
			}
			memset(ptr->handle, 0, val_len);
			memmove(ptr->handle, _val_c, val_len);
			break;
		}

	} else {
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("JS_IS_INT false\n");

		printf ("SFNodeSetProperty, setting node %d field %s to value %s\n", ptr->handle,_id_c,_val_c);

		{
			struct X3D_Node* ptx;
			ptx = (struct X3D_Node*) ptr->handle;
			printf ("node is of type %s\n",stringNodeType(ptx->_nodeType));
		}
		#endif
		setField_fromJavascript ((void *)ptr->handle, _id_c, _val_c);
	}

	return JS_TRUE;
}

/********************************************************************/

JSBool
SFRotationGetAxis(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *_retObj;
	SFRotationNative *_rot;
	SFVec3fNative *_retNative;

	UNUSED(argc);
	UNUSED(argv);
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationGetAxis\n");
	#endif

	if ((_retObj = JS_ConstructObject(cx, &SFVec3fClass, NULL, NULL)) == NULL) {
		printf( "JS_ConstructObject failed in SFRotationGetAxis.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(_retObj);

	if ((_rot = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFRotationGetAxis.\n");
		return JS_FALSE;
	}

	if ((_retNative = (SFVec3fNative *)JS_GetPrivate(cx, _retObj)) == NULL) {
		printf( "JS_GetPrivate failed for _retObj in SFRotationGetAxis.\n");
		return JS_FALSE;
	}

	(_retNative->v).c[0] = (_rot->v).r[0];
	(_retNative->v).c[1] = (_rot->v).r[1];
	(_retNative->v).c[2] = (_rot->v).r[2];

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFRotationGetAxis: obj = %u, result = [%.9g, %.9g, %.9g]\n",
			   VERBOSE_OBJ obj,
			   (_retNative->v).c[0],
			   (_retNative->v).c[1],
			   (_retNative->v).c[2]);
	#endif

	return JS_TRUE;
}

/* implement later */
JSBool
SFRotationInverse(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *_retObj, *_proto;
	SFRotationNative *_rot, *_retNative;

	UNUSED(argc);
	UNUSED(argv);
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationInverse\n");
	#endif

	if ((_proto = JS_GetPrototype(cx, obj)) == NULL) {
		printf( "JS_GetPrototype failed in SFRotationInverse.\n");
		return JS_FALSE;
	}
	if ((_retObj = JS_ConstructObject(cx, &SFRotationClass, _proto, NULL)) == NULL) {
		printf( "JS_ConstructObject failed in SFRotationInverse.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(_retObj);

	if ((_rot = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFRotationInverse.\n");
		return JS_FALSE;
	}

	if ((_retNative = (SFRotationNative *)JS_GetPrivate(cx, _retObj)) == NULL) {
		printf( "JS_GetPrivate failed for _retObj in SFRotationInverse.\n");
		return JS_FALSE;
	}

	/* calculation correct? */
/* 	_retNative->v.r[0] = (_rot->v).r[0]; */
/* 	_retNative->v.r[1] = (_rot->v).r[1]; */
/* 	_retNative->v.r[2] = (_rot->v).r[2]; */
/* 	_retNative->v.r[3] = -(_rot->v).r[3]; */
	printf( "SFRotation's inverse function does nothing!\n");

	return JS_TRUE;
}

/* implement later */
JSBool
SFRotationMultiply(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *_multObj, *_proto, *_retObj;
	SFRotationNative *_rot1, *_rot2, *_retNative;
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationMultiply\n");
	#endif

	if (!JS_ConvertArguments(cx, argc, argv, "o", &_multObj)) {
		printf( "JS_ConvertArguments failed in SFRotationMultiply.\n");
		return JS_FALSE;
	}
	if (!JS_InstanceOf(cx, _multObj, &SFRotationClass, argv)) {
		printf( "JS_InstanceOf failed in SFRotationMultiply.\n");
		return JS_FALSE;
	}
	if ((_proto = JS_GetPrototype(cx, _multObj)) == NULL) {
		printf( "JS_GetPrototype failed in SFRotationMultiply.\n");
		return JS_FALSE;
	}

	if ((_retObj = JS_ConstructObject(cx, &SFRotationClass, _proto, NULL)) == NULL) {
		printf( "JS_ConstructObject failed in SFRotationMultiply.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(_retObj);

	if ((_rot1 = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFRotationMultiply.\n");
		return JS_FALSE;
	}

	if ((_rot2 = (SFRotationNative *)JS_GetPrivate(cx, _multObj)) == NULL) {
		printf( "JS_GetPrivate failed for _multObj in SFRotationMultiply.\n");
		return JS_FALSE;
	}

	if ((_retNative = (SFRotationNative *)JS_GetPrivate(cx, _retObj)) == NULL) {
		printf( "JS_GetPrivate failed for _retObj in SFRotationMultiply.\n");
		return JS_FALSE;
	}

	printf( "SFRotation's multiply function does nothing!\n");

	return JS_TRUE;
}

JSBool
SFRotationMultVec(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *_multObj, *_retObj, *_proto;
	SFRotationNative *_rot;
	SFVec3fNative *_vec, *_retNative;
	float rl, vl, rlpt, s, c, angle;
	struct pt r, v, c1, c2;

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationMultiVec\n");
	#endif

	if (!JS_ConvertArguments(cx, argc, argv, "o", &_multObj)) {
		printf( "JS_ConvertArguments failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if (!JS_InstanceOf(cx, _multObj, &SFVec3fClass, argv)) {
		printf( "JS_InstanceOf failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	if ((_proto = JS_GetPrototype(cx, _multObj)) == NULL) {
		printf( "JS_GetPrototype failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}

	if ((_retObj = JS_ConstructObject(cx, &SFVec3fClass, _proto, NULL)) == NULL) {
		printf( "JS_ConstructObject failed in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	*rval = OBJECT_TO_JSVAL(_retObj);

	if ((_rot = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFRotationMultVec.\n");
		return JS_FALSE;
	}
	r.x = _rot->v.r[0];
	r.y = _rot->v.r[1];
	r.z = _rot->v.r[2];
	angle = _rot->v.r[3];

	if ((_vec = (SFVec3fNative *)JS_GetPrivate(cx, _multObj)) == NULL) {
		printf( "JS_GetPrivate failed for_multObjin SFRotationMultVec.\n");
		return JS_FALSE;
	}
	v.x = _vec->v.c[0];
	v.y = _vec->v.c[1];
	v.z = _vec->v.c[2];
	if ((_retNative = (SFVec3fNative *)JS_GetPrivate(cx, _retObj)) == NULL) {
		printf( "JS_GetPrivate failed for _retObj in SFRotationMultVec.\n");
		return JS_FALSE;
	}

	rl = veclength(r);
	vl = veclength(v);
	rlpt = VECPT(r, v) / rl / vl;
	s = sin(angle);
	c = cos(angle);
	VECCP(r, v, c1);
	VECSCALE(c1, 1.0 / rl);
	VECCP(r, c1, c2);
	VECSCALE(c2, 1.0 / rl) ;
	_retNative->v.c[0] = v.x + s * c1.x + (1-c) * c2.x;
	_retNative->v.c[1] = v.y + s * c1.y + (1-c) * c2.y;
	_retNative->v.c[2] = v.z + s * c1.z + (1-c) * c2.z;

	return JS_TRUE;
}

JSBool
SFRotationSetAxis(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *_setAxisObj;
	SFRotationNative *_rot;
	SFVec3fNative *_vec;

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationSetAxis\n");
	#endif

	if (!JS_ConvertArguments(cx, argc, argv, "o", &_setAxisObj)) {
		printf( "JS_ConvertArguments failed in SFRotationSetAxis.\n");
		return JS_FALSE;
	}
	if (!JS_InstanceOf(cx, _setAxisObj, &SFVec3fClass, argv)) {
		printf( "JS_InstanceOf failed in SFRotationSetAxis.\n");
		return JS_FALSE;
	}

	if ((_rot = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFRotationSetAxis.\n");
		return JS_FALSE;
	}

	if ((_vec = (SFVec3fNative *)JS_GetPrivate(cx, _setAxisObj)) == NULL) {
		printf( "JS_GetPrivate failed for _retObj in SFRotationSetAxis.\n");
		return JS_FALSE;
	}

	(_rot->v).r[0] = (_vec->v).c[0];
	(_rot->v).r[1] = (_vec->v).c[1];
	(_rot->v).r[2] = (_vec->v).c[2];

	*rval = OBJECT_TO_JSVAL(obj);

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFRotationSetAxis: obj = %u, result = [%.9g, %.9g, %.9g, %.9g]\n",
			   VERBOSE_OBJ obj,
			   (_rot->v).r[0],
			   (_rot->v).r[1],
			   (_rot->v).r[2],
			   (_rot->v).r[3]);
	#endif

	return JS_TRUE;
}

/* implement later */
JSBool
SFRotationSlerp(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSObject *_destObj, *_retObj, *_proto;
	SFRotationNative *_rot, *_dest, *_ret;
	Quaternion _quat, _quat_dest, _quat_ret;
	jsdouble t;

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationSlerp\n");
	#endif
	if (!JS_ConvertArguments(cx, argc, argv, "o d", &_destObj, &t)) {
		printf( "JS_ConvertArguments failed in SFRotationSlerp.\n");
		return JS_FALSE;
	}
	if (!JS_InstanceOf(cx, _destObj, &SFRotationClass, argv)) {
		printf( "JS_InstanceOf failed in SFRotationSlerp.\n");
		return JS_FALSE;
	}

	/*
	 * From Annex C, C.6.7.4:
	 *
	 * For t = 0, return object's rotation.
	 * For t = 1, return 1st argument.
	 * For 0 < t < 1, compute slerp.
	 */
	if (APPROX(t, 0)) {
		*rval = OBJECT_TO_JSVAL(obj);
	} else if (APPROX(t, 1)) {
		*rval = OBJECT_TO_JSVAL(_destObj);
	} else {
		if ((_proto = JS_GetPrototype(cx, _destObj)) == NULL) {
			printf( "JS_GetPrototype failed in SFRotationSlerp.\n");
			return JS_FALSE;
		}

		if ((_retObj = JS_ConstructObject(cx, &SFRotationClass, _proto, NULL)) == NULL) {
			printf( "JS_ConstructObject failed in SFRotationSlerp.\n");
			return JS_FALSE;
		}
		/* root the object */
		*rval = OBJECT_TO_JSVAL(_retObj);

		if ((_rot = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
			printf( "JS_GetPrivate failed for obj in SFRotationSlerp.\n");
			return JS_FALSE;
		}

		if ((_dest = (SFRotationNative *)JS_GetPrivate(cx, _destObj)) == NULL) {
			printf( "JS_GetPrivate failed for _destObj in SFRotationSlerp.\n");
			return JS_FALSE;
		}

		if ((_ret = (SFRotationNative *)JS_GetPrivate(cx, _retObj)) == NULL) {
			printf( "JS_GetPrivate failed for _retObj in SFRotationSlerp.\n");
			return JS_FALSE;
		}

		vrmlrot_to_quaternion(&_quat,
							  (_rot->v).r[0],
							  (_rot->v).r[1],
							  (_rot->v).r[2],
							  (_rot->v).r[3]);

		vrmlrot_to_quaternion(&_quat_dest,
							  (_dest->v).r[0],
							  (_dest->v).r[1],
							  (_dest->v).r[2],
							  (_dest->v).r[3]);

		slerp(&_quat_ret, &_quat, &_quat_dest, t);
		quaternion_to_vrmlrot(&_quat_ret,
							  (double *) &(_ret->v).r[0],
							  (double *) &(_ret->v).r[1],
							  (double *) &(_ret->v).r[2],
							  (double *) &(_ret->v).r[3]);
	}

	return JS_TRUE;
}

JSBool
SFRotationToString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    SFRotationNative *ptr;
    JSString *_str;
	char buff[STRING];

	UNUSED(argc);
	UNUSED(argv);
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationToString\n");
	#endif

	if ((ptr = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFRotationToString.\n");
		return JS_FALSE;
	}

	memset(buff, 0, STRING);
	sprintf(buff, "%.9g %.9g %.9g %.9g",
			(ptr->v).r[0], (ptr->v).r[1], (ptr->v).r[2], (ptr->v).r[3]);
	_str = JS_NewStringCopyZ(cx, buff);
    *rval = STRING_TO_JSVAL(_str);

    return JS_TRUE;
}

JSBool
SFRotationAssign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    JSObject *_from_obj;
    SFRotationNative *fptr, *ptr;
    char *_id_str;

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationAssign\n");
	#endif

	if ((ptr = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFRotationAssign.\n");
        return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, obj, &SFRotationClass, argv)) {
		printf( "JS_InstanceOf failed for obj in SFRotationAssign.\n");
        return JS_FALSE;
	}
	if (!JS_ConvertArguments(cx, argc, argv, "o s", &_from_obj, &_id_str)) {
		printf( "JS_ConvertArguments failed in SFRotationAssign.\n");
		return JS_FALSE;
	}

	/* is this an assignment of NULL? */
	if (_from_obj == NULL) {
		printf ("we have an assignment to null in SFRotationAssign\n");
		*rval = NULL;
	} else {

	    if (!JS_InstanceOf(cx, _from_obj, &SFRotationClass, argv)) {
			printf( "JS_InstanceOf failed for _from_obj in SFRotationAssign.\n");
	        return JS_FALSE;
	    }
		if ((fptr = (SFRotationNative *)JS_GetPrivate(cx, _from_obj)) == NULL) {
			printf( "JS_GetPrivate failed for _from_obj in SFRotationAssign.\n");
        	return JS_FALSE;
		}
		#ifdef JSVRMLCLASSESVERBOSE
			printf("SFRotationAssign: obj = %u, id = \"%s\", from = %u\n",
				   VERBOSE_OBJ obj, _id_str, VERBOSE_OBJ _from_obj);
		#endif

	    SFRotationNativeAssign(ptr, fptr);
		*rval = OBJECT_TO_JSVAL(obj);
	}
	return JS_TRUE;
}

JSBool
SFRotationTouched(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    SFRotationNative *ptr;
    int t;

	UNUSED(argc);
	UNUSED(argv);
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationTouched\n");
	#endif

	if ((ptr = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFRotationTouched.\n");
		return JS_FALSE;
	}

    t = ptr->touched;
	ptr->touched = 0;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFRotationTouched: obj = %u, touched = %d\n", VERBOSE_OBJ obj, t);
	#endif
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}

JSBool
SFRotationConstr(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	SFVec3fNative *_vec;
	SFVec3fNative *_vec2;
	SFRotationNative *ptr;
	JSObject *_ob1, *_ob2;
	jsdouble pars[4];
	jsdouble doub;
	float v1len, v2len;
	double v12dp;
	struct pt v1, v2;
	int v3fv3f;

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationConstr\n");
	#endif
	if ((ptr = (SFRotationNative *)SFRotationNativeNew()) == NULL) {
		printf( "SFRotationNativeNew failed in SFRotationConstr.\n");
		return JS_FALSE;
	}

	if (!JS_DefineProperties(cx, obj, SFRotationProperties)) {
		printf( "JS_DefineProperties failed in SFRotationConstr.\n");
		return JS_FALSE;
	}
	if (!JS_SetPrivate(cx, obj, ptr)) {
		printf( "JS_SetPrivate failed in SFRotationConstr.\n");
		return JS_FALSE;
	}

	if (argc == 0) {
		(ptr->v).r[0] = 0.0; (ptr->v).r[1] = 0.0; (ptr->v).r[2] = 1.0; (ptr->v).r[3] = 0.0;

	} else if (argc == 2) {
		/* two possibilities - SFVec3f/numeric, or SFVec3f/SFVec3f */
		if (JSVAL_IS_OBJECT(argv[0])) {
			_ob1 = argv[0];
			if (!JS_InstanceOf(cx, _ob1, &SFVec3fClass, argv)) {
				printf( "JS_InstanceOf failed for 2 arg format in SFRotationConstr.\n");
				return JS_FALSE;
			}
			if ((_vec = (SFVec3fNative *)JS_GetPrivate(cx, _ob1)) == NULL) {
				printf( "JS_GetPrivate failed for arg format \"o d\" in SFRotationConstr.\n");
				return JS_FALSE;
			}
		}
		if (JSVAL_IS_OBJECT(argv[1])) {
			_ob2 = argv[1];

			v3fv3f = TRUE;
			if (!JS_InstanceOf(cx, _ob2, &SFVec3fClass, argv)) {
				printf( "JS_InstanceOf failed for 2 arg (vec3f/vec3f) format in SFRotationConstr.\n");
				return JS_FALSE;
			}
			if ((_vec2 = (SFVec3fNative *)JS_GetPrivate(cx, _ob2)) == NULL) {
				printf( "JS_GetPrivate failed for _ob1 in SFRotationConstr.\n");
				return JS_FALSE;
			}
		} else {
			v3fv3f = FALSE;
			if (!JSVAL_IS_NUMBER(argv[1])) {
				printf ("SFRotationConstr param error - number expected\n");
				return JS_FALSE;
			}
			if (!JS_ValueToNumber(cx, argv[1], &doub)) {
				printf("JS_ValueToNumber failed in SFRotationConstr.\n");
				return JS_FALSE;
			}
		}


		if (!v3fv3f) {
			(ptr->v).r[0] = _vec->v.c[0];
			(ptr->v).r[1] = _vec->v.c[1];
			(ptr->v).r[2] = _vec->v.c[2];
			(ptr->v).r[3] = doub;
		} else {
			v1.x = _vec->v.c[0];
			v1.y = _vec->v.c[1];
			v1.z = _vec->v.c[2];
			v2.x = _vec2->v.c[0];
			v2.y = _vec2->v.c[1];
			v2.z = _vec2->v.c[2];
	
			v1len = veclength(v1);
			v2len = veclength(v2);
			v12dp = vecdot(&v1, &v2);
			(ptr->v).r[0] = v1.y * v2.z - v2.y * v1.z;
			(ptr->v).r[1] = v1.z * v2.x - v2.z * v1.x;
			(ptr->v).r[2] = v1.x * v2.y - v2.x * v1.y;
			v12dp /= v1len * v2len;
			(ptr->v).r[3] = atan2(sqrt(1 - v12dp * v12dp), v12dp);
		}
	} else if (argc == 4 && JS_ConvertArguments(cx, argc, argv, "d d d d",
			&(pars[0]), &(pars[1]), &(pars[2]), &(pars[3]))) {
		(ptr->v).r[0] = pars[0];
		(ptr->v).r[1] = pars[1];
		(ptr->v).r[2] = pars[2];
		(ptr->v).r[3] = pars[3];
	} else {
		printf( "Invalid arguments for SFRotationConstr.\n");
		return JS_FALSE;
	}

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFRotationConstr: obj = %u, %u args, %f %f %f %f\n",
			   VERBOSE_OBJ obj, argc,
			   (ptr->v).r[0], (ptr->v).r[1], (ptr->v).r[2], (ptr->v).r[3]);
	#endif
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

void
SFRotationFinalize(JSContext *cx, JSObject *obj)
{
	SFRotationNative *ptr;

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFRotationFinalize: obj = %u\n", VERBOSE_OBJ obj);
	#endif

	if ((ptr = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFRotationFinalize.\n");
		return;
	}
	SFRotationNativeDelete(ptr);
}

JSBool
SFRotationGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFRotationNative *ptr;
	jsdouble d, *dp;

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationGetProperty\n");
	#endif

	if ((ptr = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFRotationGetProperty.\n");
		return JS_FALSE;
	}

	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			d = (ptr->v).r[0];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFRotationGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 1:
			d = (ptr->v).r[1];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFRotationGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 2:
			d = (ptr->v).r[2];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFRotationGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 3:
			d = (ptr->v).r[3];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFRotationGetProperty.\n",
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
SFRotationSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFRotationNative *ptr;
	jsval myv;

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("start of SFRotationSetProperty\n");
	#endif

	if ((ptr = (SFRotationNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFRotationSetProperty.\n");
		return JS_FALSE;
	}
	ptr->touched++;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFRotationSetProperty: obj = %u, id = %d, touched = %d\n",
			   VERBOSE_OBJ obj, JSVAL_TO_INT(id), ptr->touched);
	#endif

	if (!JS_ConvertValue(cx, *vp, JSTYPE_NUMBER, &myv)) {
		printf( "JS_ConvertValue failed in SFRotationSetProperty.\n");
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

/********************************************************************/

/* Generic SFVec2f routines that return a SFVec2f */
#define __2FADD		1
#define __2FDIVIDE 	2
#define __2FMULT	3
#define __2FSUBT	4
#define __2FDOT		5
#define __2FLENGTH	6
#define __2FNORMALIZE	8
JSBool SFVec2fGeneric( JSContext *cx, JSObject *obj,
		   uintN argc, jsval *argv, jsval *rval, int op) {

	JSObject *_paramObj, *_proto, *_retObj;
	SFVec2fNative *_vec1, *_vec2, *_retNative;
	jsdouble d=0.0;
	jsdouble d0=0.0;
	jsdouble d1=0.0;
	jsdouble *dp;
	struct pt v1, v2;


	/* parameters */
	int SFParam = FALSE;
	int numParam = FALSE;

	/* return values */
	int retSFVec2f = FALSE;
	int retNumeric = FALSE;

	/* is the "argv" parameter a string? */
	int param_isString;
	char *charString;
	jsdouble pars[3];
	JSString *_str;

	/* determine what kind of parameter to get */
	if ((op==__2FADD)||(op==__2FDOT)||(op==__2FSUBT))SFParam=TRUE;
	if ((op==__2FDIVIDE)||(op==__2FMULT))numParam=TRUE;

	/* determine the return value, if it is NOT a SFVec2f */
	if ((op==__2FDOT)||(op==__2FLENGTH)) retNumeric = TRUE;
	retSFVec2f = (!retNumeric);

	/* is the parameter a string, possibly gotten from the VRML/X3d
	 * side of things? */
	param_isString = JSVAL_IS_STRING (*argv);

	/* get the parameter */
	if ((SFParam) || (numParam)) {
		if (numParam) {
			if (!JSVAL_IS_NUMBER(argv[0])) {
				printf ("SFVec2f param error - number expected\n");
				return JS_FALSE;
			}
			if (!JS_ValueToNumber(cx, argv[0], &d)) {
				printf("JS_ValueToNumber failed in SFVec2f.\n");
				return JS_FALSE;
			}
		} else {
			/* did this come in from VRML as a string, or did
			 * it get created in javascript? */
			if (param_isString) {
				_str = JS_ValueToString(cx, *argv);
				charString = JS_GetStringBytes(_str);

				if (sscanf(charString, "%lf %lf",
							&(pars[0]), &(pars[1])) != 2) {
					printf ("conversion problem in SFVec2fGeneric\n");
					return JS_FALSE;
				}
				/* printf ("past scan, %f %f %f\n",pars[0], pars[1]);*/
			} else {
				if (!JS_ConvertArguments(cx, argc, argv, "o", &_paramObj)) {
					printf( "JS_ConvertArguments failed in SFVec2f.\n");
					return JS_FALSE;
				}
				if (!JS_InstanceOf(cx, _paramObj, &SFVec2fClass, argv)) {
					printf( "SFVec2f - expected a SFVec2f parameter.\n");
					printJSNodeType (cx,_paramObj);
					return JS_FALSE;
				}

				if ((_vec2 = (SFVec2fNative*)JS_GetPrivate(cx, _paramObj)) == NULL) {
					printf( "JS_GetPrivate failed for _paramObj in SFVec2f.\n");
					return JS_FALSE;
				}
				pars[0]= (_vec2->v).c[0];
				pars[1] = (_vec2->v).c[1];
			}
		}
	}

	/* get our values */
	if ((_vec1 = (SFVec2fNative*)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFVec2fAdd.\n");
		return JS_FALSE;
	}

	/* do the operation */
	switch (op) {
		/* returning a SFVec2f */
		case __2FADD:
			d0 = (_vec1->v).c[0] + (_vec2->v).c[0];
			d1 = (_vec1->v).c[1] + (_vec2->v).c[1];
			break;
		case __2FDIVIDE:
			d0 = (_vec1->v).c[0] / d;
			d1 = (_vec1->v).c[1] / d;
			break;
		case __2FMULT:
			d0 = (_vec1->v).c[0] * d;
			d1 = (_vec1->v).c[1] * d;
			break;
		case __2FSUBT:
			d0 = (_vec1->v).c[0] - (_vec2->v).c[0];
			d1 = (_vec1->v).c[1] - (_vec2->v).c[1];
			break;
		case __2FDOT:
			v1.x = (_vec1->v).c[0]; v1.y=(_vec1->v).c[1];v1.z=0.0;
			v2.x = (_vec2->v).c[0]; v2.y=(_vec2->v).c[1];v2.z=0.0;
			d = vecdot (&v1, &v2);
			break;
		case __2FLENGTH:
			v1.x = (_vec1->v).c[0]; v1.y=(_vec1->v).c[1];v1.z=0.0;
			d = veclength(v1);
			break;
		case __2FNORMALIZE:
			v1.x = (_vec1->v).c[0]; v1.y=(_vec1->v).c[1];v1.z=0.0;
			vecnormal(&v1, &v1);
			d0 = v1.x; d1 = v1.y;
			break;
		default:
		return JS_FALSE;
	}

	/* set the return object */
	if (retSFVec2f) {
		if ((_proto = JS_GetPrototype(cx, obj)) == NULL) {
			printf( "JS_GetPrototype failed in SFVec2f.\n");
			return JS_FALSE;
		}
		if ((_retObj =
			JS_ConstructObject(cx, &SFVec2fClass, _proto, NULL)) == NULL) {
			printf( "JS_ConstructObject failed in SFVec2f.\n");
			return JS_FALSE;
		}
		*rval = OBJECT_TO_JSVAL(_retObj);
		if ((_retNative = (SFVec2fNative*)JS_GetPrivate(cx, _retObj)) == NULL) {
			printf( "JS_GetPrivate failed for _retObj in SFVec2f.\n");
			return JS_FALSE;
		}
		(_retNative->v).c[0] = d0;
		(_retNative->v).c[1] = d1;
	} else if (retNumeric) {
		if ((dp = JS_NewDouble(cx,d)) == NULL) {
			printf( "JS_NewDouble failed for %f in SFVec2f.\n",d);
			return JS_FALSE;
		}
		*rval = DOUBLE_TO_JSVAL(dp);
	}

	#ifdef JSVRMLCLASSESVERBOSE
	if (retSFVec2f){
		printf("SFVec2fgeneric: obj = %u, result = [%.9g, %.9g]\n",
			   VERBOSE_OBJ obj,
			   (_retNative->v).c[0], (_retNative->v).c[1]);
	}
	if (retNumeric){
		printf("SFVec2fgeneric: obj = %u, result = %.9g\n",
			   VERBOSE_OBJ obj, d);
	}
	#endif

	return JS_TRUE;
}

JSBool
SFVec2fAdd(JSContext *cx, JSObject *obj,
		   uintN argc, jsval *argv, jsval *rval) {
	return SFVec2fGeneric(cx, obj, argc, argv, rval, __2FADD);
}

JSBool
SFVec2fDivide(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval) {
	return SFVec2fGeneric(cx, obj, argc, argv, rval, __2FDIVIDE);
}

JSBool
SFVec2fMultiply(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval) {
	return SFVec2fGeneric(cx, obj, argc, argv, rval, __2FMULT);
}

JSBool
SFVec2fSubtract(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval) {
	return SFVec2fGeneric(cx, obj, argc, argv, rval, __2FSUBT);
}

JSBool
SFVec2fDot(JSContext *cx, JSObject *obj,
		   uintN argc, jsval *argv, jsval *rval) {
	return SFVec2fGeneric(cx, obj, argc, argv, rval, __2FDOT);
}

JSBool
SFVec2fLength(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval) {
	return SFVec2fGeneric(cx, obj, argc, argv, rval, __2FLENGTH);
}

JSBool
SFVec2fNormalize(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval) {
	return SFVec2fGeneric(cx, obj, argc, argv, rval, __2FNORMALIZE);
}

JSBool
SFVec2fToString(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval)
{
    SFVec2fNative *ptr;
    JSString *_str;
	char buff[STRING];

	UNUSED(argc);
	UNUSED(argv);
	if ((ptr = (SFVec2fNative*)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec2fToString.\n");
		return JS_FALSE;
	}

	memset(buff, 0, STRING);
	sprintf(buff, "%.9g %.9g",
			(ptr->v).c[0], (ptr->v).c[1]);
	_str = JS_NewStringCopyZ(cx, buff);
    *rval = STRING_TO_JSVAL(_str);

    return JS_TRUE;
}

JSBool
SFVec2fAssign(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval)
{
    JSObject *_from_obj;
    SFVec2fNative *fptr, *ptr;
    char *_id_str;

	if ((ptr = (SFVec2fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFVec2fAssign.\n");
        return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, obj, &SFVec2fClass, argv)) {
		printf( "JS_InstanceOf failed for obj in SFVec2fAssign.\n");
        return JS_FALSE;
	}
	if (!JS_ConvertArguments(cx, argc, argv, "o s", &_from_obj, &_id_str)) {
		printf( "JS_ConvertArguments failed in SFVec2fAssign.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, _from_obj, &SFVec2fClass, argv)) {
		printf( "JS_InstanceOf failed for _from_obj in SFVec2fAssign.\n");
        return JS_FALSE;
    }
	if ((fptr = (SFVec2fNative *)JS_GetPrivate(cx, _from_obj)) == NULL) {
		printf( "JS_GetPrivate failed for _from_obj in SFVec2fAssign.\n");
        return JS_FALSE;
	}
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec2fAssign: obj = %u, id = \"%s\", from = %u\n",
			   VERBOSE_OBJ obj, _id_str, VERBOSE_OBJ _from_obj);
	#endif

    SFVec2fNativeAssign(ptr, fptr);
    *rval = OBJECT_TO_JSVAL(obj);

    return JS_TRUE;
}

JSBool
SFVec2fTouched(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval)
{
    SFVec2fNative *ptr;
    int t;

	UNUSED(argc);
	UNUSED(argv);
	if ((ptr = (SFVec2fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec2fTouched.\n");
		return JS_FALSE;
	}

    t = ptr->touched;
	ptr->touched = 0;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec2fTouched: obj = %u, touched = %d\n", VERBOSE_OBJ obj, t);
	#endif
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}

JSBool
SFVec2fConstr(JSContext *cx, JSObject *obj,
			  uintN argc, jsval *argv, jsval *rval)
{
	SFVec2fNative *ptr;
	jsdouble pars[2];

	if ((ptr = (SFVec2fNative *) SFVec2fNativeNew()) == NULL) {
		printf( "SFVec2fNativeNew failed in SFVec2fConstr.\n");
		return JS_FALSE;
	}

	if (!JS_DefineProperties(cx, obj, SFVec2fProperties)) {
		printf( "JS_DefineProperties failed in SFVec2fConstr.\n");
		return JS_FALSE;
	}
	if (!JS_SetPrivate(cx, obj, ptr)) {
		printf( "JS_SetPrivate failed in SFVec2fConstr.\n");
		return JS_FALSE;
	}

	if (argc == 0) {
		(ptr->v).c[0] = 0.0;
		(ptr->v).c[1] = 0.0;
	} else {
		if (!JS_ConvertArguments(cx, argc, argv, "d d", &(pars[0]), &(pars[1]))) {
			printf( "JS_ConvertArguments failed in SFVec2fConstr.\n");
			return JS_FALSE;
		}
		(ptr->v).c[0] = pars[0];
		(ptr->v).c[1] = pars[1];
	}
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec2fConstr: obj = %u, %u args, %f %f\n",
			   VERBOSE_OBJ obj, argc,
			   (ptr->v).c[0], (ptr->v).c[1]);
	#endif
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}


void
SFVec2fFinalize(JSContext *cx, JSObject *obj)
{
	SFVec2fNative *ptr;

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFColorFinalize: obj = %u\n", VERBOSE_OBJ obj);
	#endif

	if ((ptr = (SFVec2fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec2fFinalize.\n");
		return;
	}
	SFVec2fNativeDelete(ptr);
}

JSBool
SFVec2fGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFVec2fNative *ptr;
	jsdouble d, *dp;

	if ((ptr = (SFVec2fNative *)JS_GetPrivate(cx,obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec2fGetProperty.\n");
		return JS_FALSE;
	}

	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			d = (ptr->v).c[0];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFVec2fGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 1:
			d = (ptr->v).c[1];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFVec2fGetProperty.\n",
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
SFVec2fSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFVec2fNative *ptr;
	jsval myv;

	if ((ptr = (SFVec2fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec2fSetProperty.\n");
		return JS_FALSE;
	}
	ptr->touched++;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec2fSetProperty: obj = %u, id = %d, touched = %d\n",
			   VERBOSE_OBJ obj, JSVAL_TO_INT(id), ptr->touched);
	#endif

	if (!JS_ConvertValue(cx, *vp, JSTYPE_NUMBER, &myv)) {
		printf( "JS_ConvertValue failed in SFVec2fSetProperty.\n");
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


/********************************************************************/

/* Generic SFVec3f routines that return a SFVec3f */
#define __3FADD		1
#define __3FDIVIDE 	2
#define __3FMULT	3
#define __3FSUBT	4
#define __3FDOT		5
#define __3FLENGTH	6
#define __3FNORMALIZE	8
#define __3FNEGATE	7
#define __3FCROSS	9

JSBool SFVec3fGeneric( JSContext *cx, JSObject *obj,
		   uintN argc, jsval *argv, jsval *rval, int op) {
	JSObject *_paramObj, *_proto, *_retObj;
	SFVec3fNative *_vec1, *_vec2, *_retNative;
	jsdouble d=0.0;
	jsdouble d0=0.0;
	jsdouble d1=0.0;
	jsdouble d2=0.0;
	jsdouble *dp;
	struct pt v1, v2, ret;


	/* parameters */
	int SFParam = FALSE;
	int numParam = FALSE;

	/* return values */
	int retSFVec3f = FALSE;
	int retNumeric = FALSE;

	/* is the "argv" parameter a string? */
	int param_isString;
	char *charString;
	jsdouble pars[3];
	JSString *_str;

	/* determine what kind of parameter to get */
	if ((op==__3FADD)||(op==__3FDOT)||(op==__3FCROSS)||(op==__3FSUBT))SFParam=TRUE;
	if ((op==__3FDIVIDE)||(op==__3FMULT))numParam=TRUE;

	/* determine the return value, if it is NOT a SFVec3f */
	if ((op==__3FDOT)||(op==__3FLENGTH)) retNumeric = TRUE;
	retSFVec3f = (!retNumeric);

	/* is the parameter a string, possibly gotten from the VRML/X3d
	 * side of things? */
	param_isString = JSVAL_IS_STRING (*argv);

	/* get the parameter */
	if ((SFParam) || (numParam)) {
		if (numParam) {
			if (!JSVAL_IS_NUMBER(argv[0])) {
				printf ("SFVec3f param error - number expected\n");
				return JS_FALSE;
			}
			if (!JS_ValueToNumber(cx, argv[0], &d)) {
				printf("JS_ValueToNumber failed in SFVec3f.\n");
				return JS_FALSE;
			}
		} else {
			/* did this come in from VRML as a string, or did
			 * it get created in javascript? */
			if (param_isString) {
				_str = JS_ValueToString(cx, *argv);
				charString = JS_GetStringBytes(_str);

				if (sscanf(charString, "%lf %lf %lf",
							&(pars[0]), &(pars[1]), &(pars[2])) != 3) {
					printf ("conversion problem in SFVec3fGeneric\n");
					return JS_FALSE;
				}
				/* printf ("past scan, %f %f %f\n",pars[0], pars[1],pars[2]);*/
			} else {
				if (!JS_ConvertArguments(cx, argc, argv, "o", &_paramObj)) {
					printf( "JS_ConvertArguments failed in SFVec3f.\n");
					return JS_FALSE;
				}
				if (!JS_InstanceOf(cx, _paramObj, &SFVec3fClass, argv)) {
					printf( "SFVec3f - expected a SFVec3f parameter.\n");
					printJSNodeType (cx,_paramObj);

					return JS_FALSE;
				}

				/* get the second object's data */
				if ((_vec2 = (SFVec3fNative*)JS_GetPrivate(cx, _paramObj)) == NULL) {
					printf( "JS_GetPrivate failed for _paramObj in SFVec3f.\n");
					return JS_FALSE;
				}
				pars[0]= (_vec2->v).c[0];
				pars[1] = (_vec2->v).c[1];
				pars[2] = (_vec2->v).c[2];
			}
		}
	}

	/* get our values */
	if ((_vec1 = (SFVec3fNative*)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFVec3fAdd.\n");
		return JS_FALSE;
	}

	/* do the operation */
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("SFVec3f generic, vec2 %f %f %f\n",pars[0],pars[1],pars[2]);
	#endif

	switch (op) {
		/* returning a SFVec3f */
		case __3FADD:
			d0 = (_vec1->v).c[0] + pars[0];
			d1 = (_vec1->v).c[1] + pars[1];
			d2 = (_vec1->v).c[2] + pars[2];
			break;
		case __3FDIVIDE:
			d0 = (_vec1->v).c[0] / d;
			d1 = (_vec1->v).c[1] / d;
			d2 = (_vec1->v).c[2] / d;
			break;
		case __3FMULT:
			d0 = (_vec1->v).c[0] * d;
			d1 = (_vec1->v).c[1] * d;
			d2 = (_vec1->v).c[2] * d;
			break;
		case __3FSUBT:
			d0 = (_vec1->v).c[0] - pars[0];
			d1 = (_vec1->v).c[1] - pars[1];
			d2 = (_vec1->v).c[2] - pars[2];
			break;
		case __3FDOT:
			v1.x = (_vec1->v).c[0]; v1.y=(_vec1->v).c[1];v1.z=(_vec1->v).c[2];
			v2.x = pars[0]; v2.y=pars[1];v2.z=pars[2];
			d = vecdot (&v1, &v2);
			break;
		case __3FCROSS:
			v1.x = (_vec1->v).c[0]; v1.y=(_vec1->v).c[1];v1.z=(_vec1->v).c[2];
			v2.x = pars[0]; v2.y=pars[1];v2.z=pars[2];
			veccross(&ret, v1, v2);
			d0 = ret.x;d1 = ret.y, d2 = ret.z;
			break;
		case __3FLENGTH:
			v1.x = (_vec1->v).c[0]; v1.y=(_vec1->v).c[1];v1.z=(_vec1->v).c[2];
			d = veclength(v1);
			break;
		case __3FNORMALIZE:
			v1.x = (_vec1->v).c[0]; v1.y=(_vec1->v).c[1];v1.z=(_vec1->v).c[2];
			vecnormal(&v1, &v1);
			d0 = v1.x; d1 = v1.y; d2 = v1.z;
			break;
		case __3FNEGATE:
			d0 = -(_vec1->v).c[0];
			d1 = -(_vec1->v).c[1];
			d2 = -(_vec1->v).c[2];
			break;
		default:
			printf ("woops... %d\n",op);
		return JS_FALSE;
	}

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("past calcs\n");
	#endif

	/* set the return object */
	if (retSFVec3f) {
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("returning SFVec3f\n");
		#endif
		if ((_proto = JS_GetPrototype(cx, obj)) == NULL) {
			printf( "JS_GetPrototype failed in SFVec3f.\n");
			return JS_FALSE;
		}
		if ((_retObj =
			JS_ConstructObject(cx, &SFVec3fClass, _proto, NULL)) == NULL) {
			printf( "JS_ConstructObject failed in SFVec3f.\n");
			return JS_FALSE;
		}
		*rval = OBJECT_TO_JSVAL(_retObj);
		if ((_retNative = (SFVec3fNative*)JS_GetPrivate(cx, _retObj)) == NULL) {
			printf( "JS_GetPrivate failed for _retObj in SFVec3f.\n");
			return JS_FALSE;
		}
		(_retNative->v).c[0] = d0;
		(_retNative->v).c[1] = d1;
		(_retNative->v).c[2] = d2;
	} else if (retNumeric) {
		if ((dp = JS_NewDouble(cx,d)) == NULL) {
			printf( "JS_NewDouble failed for %f in SFVec3f.\n",d);
			return JS_FALSE;
		}
		*rval = DOUBLE_TO_JSVAL(dp);
	}
	#ifdef JSVRMLCLASSESVERBOSE
	if (retSFVec3f){
		printf("SFVec3fgeneric: obj = %u, result = [%.9g, %.9g, %.9g]\n",
			   VERBOSE_OBJ obj,
			   (_retNative->v).c[0], (_retNative->v).c[1],
			   (_retNative->v).c[2]);
	}
	if (retNumeric){
		printf("SFVec2fgeneric: obj = %u, result = %.9g\n",
			   VERBOSE_OBJ obj, d);
	}
	#endif
return JS_TRUE;
}

JSBool
SFVec3fAdd(JSContext *cx, JSObject *obj,
		   uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FADD);
}

JSBool
SFVec3fCross(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FCROSS);
}

JSBool
SFVec3fDivide(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FDIVIDE);
}

JSBool
SFVec3fDot(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FDOT);
}

JSBool
SFVec3fLength(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FLENGTH);
}


JSBool
SFVec3fMultiply(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FMULT);
}


JSBool
SFVec3fNegate(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FNEGATE);
}

JSBool
SFVec3fNormalize(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FNORMALIZE);
}

JSBool
SFVec3fSubtract(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval) {
	return SFVec3fGeneric(cx, obj, argc, argv, rval, __3FSUBT);
}

JSBool
SFVec3fToString(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval)
{
    SFVec3fNative *ptr;
    JSString *_str;
	char buff[STRING];

	UNUSED(argc);
	UNUSED(argv);
	if ((ptr = (SFVec3fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec3fToString.\n");
		return JS_FALSE;
	}

	memset(buff, 0, STRING);
	sprintf(buff, "%.9g %.9g %.9g",
			(ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	_str = JS_NewStringCopyZ(cx, buff);
    *rval = STRING_TO_JSVAL(_str);

	#ifdef JSVRMLCLASSESVERBOSE
		printf ("SFVec3fToString, string is :%s:\n",buff);
	#endif

    return JS_TRUE;
}

JSBool
SFVec3fAssign(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval)
{
    JSObject *_from_obj;
    SFVec3fNative *fptr, *ptr;
    char *_id_str;

	#ifdef JSVRMLCLASSESVERBOSE
		printf ("start of SFVec3fAssign\n");
	#endif

	if ((ptr = (SFVec3fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed for obj in SFVec3fAssign.\n");
        return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, obj, &SFVec3fClass, argv)) {
		printf( "JS_InstanceOf failed for obj in SFVec3fAssign.\n");
        return JS_FALSE;
	}
	if (!JS_ConvertArguments(cx, argc, argv, "o s", &_from_obj, &_id_str)) {
		printf( "JS_ConvertArguments failed in SFVec3fAssign.\n");
		return JS_FALSE;
	}
    if (!JS_InstanceOf(cx, _from_obj, &SFVec3fClass, argv)) {
		printf( "JS_InstanceOf failed for _from_obj in SFVec3fAssign.\n");
        return JS_FALSE;
    }
	if ((fptr = (SFVec3fNative *)JS_GetPrivate(cx, _from_obj)) == NULL) {
		printf( "JS_GetPrivate failed for _from_obj in SFVec3fAssign.\n");
        return JS_FALSE;
	}
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec3fAssign: obj = %u, id = \"%s\", from = %u\n",
			   VERBOSE_OBJ obj, _id_str, VERBOSE_OBJ  _from_obj);
	#endif

    SFVec3fNativeAssign(ptr, fptr);
    *rval = OBJECT_TO_JSVAL(obj);

	#ifdef JSVRMLCLASSESVERBOSE
		printf ("end of SFVec3fAssign\n");
	#endif

    return JS_TRUE;
}

JSBool
SFVec3fTouched(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval)
{
    SFVec3fNative *ptr;
    int t;

	UNUSED(argc);
	UNUSED(argv);

	#ifdef JSVRMLCLASSESVERBOSE
		printf ("start of SFVec3fTouched\n");
	#endif

	if ((ptr = (SFVec3fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec3fTouched.\n");
		return JS_FALSE;
	}

    t = ptr->touched;
	ptr->touched = 0;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec3fTouched: obj = %u, touched = %d\n", VERBOSE_OBJ obj, t);
	#endif
    *rval = INT_TO_JSVAL(t);
    return JS_TRUE;
}

JSBool
SFVec3fConstr(JSContext *cx, JSObject *obj,
			 uintN argc, jsval *argv, jsval *rval)
{
	SFVec3fNative *ptr;
	jsdouble pars[3];
	
	#ifdef JSVRMLCLASSESVERBOSE
		printf ("start of SFVec3fConstr\n");
	#endif

	if ((ptr = (SFVec3fNative *) SFVec3fNativeNew()) == NULL) {
		printf( "SFVec3fNativeNew failed in SFVec3fConstr.\n");
		return JS_FALSE;
	}

	if (!JS_DefineProperties(cx, obj, SFVec3fProperties)) {
		printf( "JS_DefineProperties failed in SFVec3fConstr.\n");
		return JS_FALSE;
	}
	if (!JS_SetPrivate(cx, obj, ptr)) {
		printf( "JS_SetPrivate failed in SFVec3fConstr.\n");
		return JS_FALSE;
	}

	if (argc == 0) {
		(ptr->v).c[0] = 0.0;
		(ptr->v).c[1] = 0.0;
		(ptr->v).c[2] = 0.0;
	} else {
		if (!JS_ConvertArguments(cx, argc, argv, "d d d",
				 &(pars[0]), &(pars[1]), &(pars[2]))) {
			printf( "JS_ConvertArguments failed in SFVec3fConstr.\n");
			return JS_FALSE;
		}
		(ptr->v).c[0] = pars[0];
		(ptr->v).c[1] = pars[1];
		(ptr->v).c[2] = pars[2];
	}
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec3fConstr: obj = %u, %u args, %f %f %f\n",
			   VERBOSE_OBJ obj, argc,
			   (ptr->v).c[0], (ptr->v).c[1], (ptr->v).c[2]);
	#endif
	*rval = OBJECT_TO_JSVAL(obj);
	return JS_TRUE;
}

void
SFVec3fFinalize(JSContext *cx, JSObject *obj)
{
	SFVec3fNative *ptr;

	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec3fFinalize: obj = %u\n", VERBOSE_OBJ obj);
	#endif

	if ((ptr = (SFVec3fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec3fFinalize.\n");
		return;
	}
	SFVec3fNativeDelete(ptr);
}

JSBool
SFVec3fGetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFVec3fNative *ptr;
	jsdouble d, *dp;

	#ifdef JSVRMLCLASSESVERBOSE

	JSString *_idStr;
	char *_id_c;

	_idStr = JS_ValueToString(cx, id);
	_id_c = JS_GetStringBytes(_idStr);
printf ("start of SFVec3fGetProperty... id is %s\n",_id_c);
	_idStr = JS_ValueToString(cx, *vp);
	_id_c = JS_GetStringBytes(_idStr);
printf ("start of SFVec3fGetProperty... vp is %s\n",_id_c);

	#endif

	if ((ptr = (SFVec3fNative *)JS_GetPrivate(cx,obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec3fGetProperty.\n");
		return JS_FALSE;
	}

#ifdef OLDCODE
    if (!JS_InstanceOf(cx, obj, &SFVec3fClass, id)) {
                printf( "JS_InstanceOf failed for obj in SFVec3fGetProperty.\n");
        return JS_FALSE;
        }
#endif


	if (JSVAL_IS_INT(id)) {
		switch (JSVAL_TO_INT(id)) {
		case 0:
			d = (ptr->v).c[0];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFVec3fGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 1:
			d = (ptr->v).c[1];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFVec3fGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		case 2:
			d = (ptr->v).c[2];
			if ((dp = JS_NewDouble(cx, d)) == NULL) {
				printf(
						"JS_NewDouble failed for %f in SFVec3fGetProperty.\n",
						d);
				return JS_FALSE;
			}
			*vp = DOUBLE_TO_JSVAL(dp);
			break;
		}
	} else {
		#ifdef JSVRMLCLASSESVERBOSE
			printf ("SFVec3fGetProperty, id is NOT an int...\n");
		#endif
	}

	return JS_TRUE;
}

JSBool
SFVec3fSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	SFVec3fNative *ptr;
	jsval myv;

	if ((ptr = (SFVec3fNative *)JS_GetPrivate(cx, obj)) == NULL) {
		printf( "JS_GetPrivate failed in SFVec3fSetProperty.\n");
		return JS_FALSE;
	}
	ptr->touched++;
	#ifdef JSVRMLCLASSESVERBOSE
		printf("SFVec3fSetProperty: obj = %u, id = %d, touched = %d\n",
			   VERBOSE_OBJ obj, JSVAL_TO_INT(id), ptr->touched);
	#endif

	if (!JS_ConvertValue(cx, *vp, JSTYPE_NUMBER, &myv)) {
		printf( "JS_ConvertValue failed in SFVec3fSetProperty.\n");
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