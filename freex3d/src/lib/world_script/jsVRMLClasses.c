/*
=INSERT_TEMPLATE_HERE=

$Id: jsVRMLClasses.c,v 1.37 2013/10/29 16:59:43 crc_canada Exp $

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
#include <system_threads.h>
#include <display.h>
#include <internal.h>

#include <libFreeWRL.h>


#include "../vrml_parser/Structs.h"
#include "../main/headers.h"
#include "../vrml_parser/CParseGeneral.h"
#include "../main/Snapshot.h"
#include "../scenegraph/Collision.h"
#include "../scenegraph/quaternion.h"
#include "../scenegraph/Viewer.h"
#include "../input/SensInterps.h"
#include "../x3d_parser/Bindable.h"

#include "JScript.h"
#include "CScripts.h"
#include "jsUtils.h"
#include "jsNative.h"
#include "jsVRMLClasses.h"

#ifdef HAVE_JAVASCRIPT

/********************************************************/
/*							*/
/* first part - standard helper functions		*/
/*							*/
/********************************************************/

void _get4f(double *ret, double *mat, int row);
void _set4f(double len, double *mat, int row);

/* for keeping track of the ECMA values */
#define ECMAValueTableSize 100
//struct ECMAValueStruct ECMAValues[ECMAValueTableSize];
//int maxECMAVal = 0;

typedef struct pjsVRMLClasses{
	struct ECMAValueStruct ECMAValues[ECMAValueTableSize];
	int maxECMAVal;// = 0;
}* ppjsVRMLClasses;
void *jsVRMLClasses_constructor(){
	void *v = malloc(sizeof(struct pjsVRMLClasses));
	memset(v,0,sizeof(struct pjsVRMLClasses));
	return v;
}
void jsVRMLClasses_init(struct tjsVRMLClasses *t){
	//public
	//private
	t->prv = jsVRMLClasses_constructor();
	{
		ppjsVRMLClasses p = (ppjsVRMLClasses)t->prv;
		//p->ECMAValues[ECMAValueTableSize];
		p->maxECMAVal = 0;
	}
}
//	ppjsVRMLClasses p = (ppjsVRMLClasses)gglobal()->jsVRMLClasses.prv;
/*
 * VRML Node types as JS classes:
 */



JSClass SFColorClass = {
	"SFColor",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFColorGetProperty,
	SFColorSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFColorProperties)[] = {
	{"r", 0, JSPROP_ENUMERATE},
	{"g", 1, JSPROP_ENUMERATE},
	{"b", 2, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFColorFunctions)[] = {
	{"getHSV", SFColorGetHSV, 0},
	{"setHSV", SFColorSetHSV, 0},
	{"toString", SFColorToString, 0},
	{"assign", SFColorAssign, 0},
	{0}
};



JSClass SFColorRGBAClass = {
	"SFColorRGBA",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFColorRGBAGetProperty,
	SFColorRGBASetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFColorRGBAProperties)[] = {
	{"r", 0, JSPROP_ENUMERATE},
	{"g", 1, JSPROP_ENUMERATE},
	{"b", 2, JSPROP_ENUMERATE},
	{"a", 3, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFColorRGBAFunctions)[] = {
	{"getHSV", SFColorRGBAGetHSV, 0},
	{"setHSV", SFColorRGBASetHSV, 0},
	{"toString", SFColorRGBAToString, 0},
	{"assign", SFColorRGBAAssign, 0},
	{0}
};



JSClass SFImageClass = {
	"SFImage",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFImageGetProperty,
	SFImageSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFImageProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"comp", 2, JSPROP_ENUMERATE},
	{"array", 3, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFImageFunctions)[] = {
	{"toString", SFImageToString, 0},
	{"assign", SFImageAssign, 0},
	{0}
};



JSClass SFNodeClass = {
	"SFNode",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFNodeGetProperty,
	SFNodeSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	SFNodeFinalize	/* note, this is different, as it contains a string to get rid of */
};

JSPropertySpec (SFNodeProperties)[] = {
	{0}
};

JSFunctionSpec (SFNodeFunctions)[] = {
#ifdef NEWCLASSES
	{"getNodeName", SFNodeGetNodeName, 0},
	{"getNodeType", SFNodeGetNodeType, 0},
	{"getFieldDefinitions",SFNodeGetFieldDefs, 0},
	{"toVRMLString",SFNodeToVRMLString, 0},
	{"toXMLString", SFNodeToXMLString, 0},
#endif
	{"toString", SFNodeToString, 0}, /* depreciated JAS */
	{"assign", SFNodeAssign, 0},
	{0}
};



JSClass SFRotationClass = {
	"SFRotation",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFRotationGetProperty,
	SFRotationSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFRotationProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{"angle",3, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFRotationFunctions)[] = {
	{"getAxis", SFRotationGetAxis, 0},
	{"inverse", SFRotationInverse, 0},
	{"multiply", SFRotationMultiply, 0},
	{"multVec", SFRotationMultVec, 0},
	{"setAxis", SFRotationSetAxis, 0},
	{"slerp", SFRotationSlerp, 0},
	{"toString", SFRotationToString, 0},
	{"assign", SFRotationAssign, 0},
	{0}
};



JSClass SFVec2fClass = {
	"SFVec2f",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec2fGetProperty,
	SFVec2fSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFVec2fProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFVec2fFunctions)[] = {
	{"add", SFVec2fAdd, 0},
	{"divide", SFVec2fDivide, 0},
	{"dot", SFVec2fDot, 0},
	{"length", SFVec2fLength, 0},
	{"multiply", SFVec2fMultiply, 0},
	{"normalize", SFVec2fNormalize, 0},
	{"subtract", SFVec2fSubtract, 0},
	{"toString", SFVec2fToString, 0},
	{"assign", SFVec2fAssign, 0},
	{0}
};

#ifdef NEWCLASSES
JSClass SFVec2dClass = {
	"SFVec2d",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec2dGetProperty,
	SFVec2dSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFVec2dProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFVec2dFunctions)[] = {
	{"add", SFVec2dAdd, 0},
	{"divide", SFVec2dDivide, 0},
	{"dot", SFVec2dDot, 0},
	{"length", SFVec2dLength, 0},
	{"multiply", SFVec2dMultiply, 0},
	{"normalize", SFVec2dNormalize, 0},
	{"subtract", SFVec2dSubtract, 0},
	{"toString", SFVec2dToString, 0},
	{"assign", SFVec2dAssign, 0},
	{0}
};

#endif /* NEWCLASSES */

JSClass SFVec4fClass = {
	"SFVec4f",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec4fGetProperty,
	SFVec4fSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFVec4fProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{"w", 3, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFVec4fFunctions)[] = {
	/* do not know what functions to use here */
	{"toString", SFVec4fToString, 0},
	{"assign", SFVec4fAssign, 0},
	{0}
};


JSClass SFVec4dClass = {
	"SFVec4d",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec4dGetProperty,
	SFVec4dSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFVec4dProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{"w", 3, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFVec4dFunctions)[] = {
	/* do not know what functions to use here */
	{"toString", SFVec4dToString, 0},
	{"assign", SFVec4dAssign, 0},
	{0}
};




JSClass SFVec3fClass = {
	"SFVec3f",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec3fGetProperty,
	SFVec3fSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFVec3fProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFVec3fFunctions)[] = {
	{"add", SFVec3fAdd, 0},
	{"cross", SFVec3fCross, 0},
	{"divide", SFVec3fDivide, 0},
	{"dot", SFVec3fDot, 0},
	{"length", SFVec3fLength, 0},
	{"multiply", SFVec3fMultiply, 0},
	{"negate", SFVec3fNegate, 0},
	{"normalize", SFVec3fNormalize, 0},
	{"subtract", SFVec3fSubtract, 0},
	{"toString", SFVec3fToString, 0},
	{"assign", SFVec3fAssign, 0},
	{0}
};


JSClass SFVec3dClass = {
	"SFVec3d",
	JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_PropertyStub,
	SFVec3dGetProperty,
	SFVec3dSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (SFVec3dProperties)[] = {
	{"x", 0, JSPROP_ENUMERATE},
	{"y", 1, JSPROP_ENUMERATE},
	{"z", 2, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec (SFVec3dFunctions)[] = {
	{"add", SFVec3dAdd, 0},
	{"cross", SFVec3dCross, 0},
	{"divide", SFVec3dDivide, 0},
	{"dot", SFVec3dDot, 0},
	{"length", SFVec3dLength, 0},
	{"multiply", SFVec3dMultiply, 0},
	{"negate", SFVec3dNegate, 0},
	{"normalize", SFVec3dNormalize, 0},
	{"subtract", SFVec3dSubtract, 0},
	{"toString", SFVec3dToString, 0},
	{"assign", SFVec3dAssign, 0},
	{0}
};

JSClass MFColorClass = {
	"MFColor",
	JSCLASS_HAS_PRIVATE,
	MFColorAddProperty,
	JS_PropertyStub,
	MFColorGetProperty,
	MFColorSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFColorFunctions)[] = {
	{"toString", MFColorToString, 0},
	{"assign", MFColorAssign, 0},
	{0}
};



JSClass MFFloatClass = {
	"MFFloat",
	JSCLASS_HAS_PRIVATE,
	MFFloatAddProperty,
	JS_PropertyStub,
	MFFloatGetProperty,
	MFFloatSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFFloatFunctions)[] = {
	{"toString", MFFloatToString, 0},
	{"assign", MFFloatAssign, 0},
	{0}
};



JSClass MFInt32Class = {
	"MFInt32",
	JSCLASS_HAS_PRIVATE,
	MFInt32AddProperty,
	JS_PropertyStub,
	MFInt32GetProperty,
	MFInt32SetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFInt32Functions)[] = {
	{"toString", MFInt32ToString, 0},
	{"assign", MFInt32Assign, 0},
	{0}
};

#ifdef NEWCLASSES
JSClass MFBoolClass = {
	"MFBool",
	JSCLASS_HAS_PRIVATE,
	MFBoolAddProperty,
	JS_PropertyStub,
	MFBoolGetProperty,
	MFBoolSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFBoolFunctions)[] = {
	{"toString", MFBoolToString, 0},
	{"assign", MFBoolAssign, 0},
	{0}
};


JSClass MFDoubleClass = {
	"MFDouble",
	JSCLASS_HAS_PRIVATE,
	MFDoubleAddProperty,
	JS_PropertyStub,
	MFDoubleGetProperty,
	MFDoubleSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFDoubleFunctions)[] = {
	{"toString", MFDoubleToString, 0},
	{"assign", MFDoubleAssign, 0},
	{0}
};



JSClass MFImageClass = {
	"MFImage",
	JSCLASS_HAS_PRIVATE,
	MFImageAddProperty,
	JS_PropertyStub,
	MFImageGetProperty,
	MFImageSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFImageFunctions)[] = {
	{"toString", MFImageToString, 0},
	{"assign", MFImageAssign, 0},
	{0}
};


JSClass MFVec2dClass = {
	"MFVec2d",
	JSCLASS_HAS_PRIVATE,
	MFVec2dAddProperty,
	JS_PropertyStub,
	MFVec2dGetProperty,
	MFVec2dSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFVec2dFunctions)[] = {
	{"toString", MFVec2dToString, 0},
	{"assign", MFVec2dAssign, 0},
	{0}
};

JSClass MFVec3dClass = {
	"MFVec3d",
	JSCLASS_HAS_PRIVATE,
	MFVec3dAddProperty,
	JS_PropertyStub,
	MFVec3dGetProperty,
	MFVec3dSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFVec3dFunctions)[] = {
	{"toString", MFVec3dToString, 0},
	{"assign", MFVec3dAssign, 0},
	{0}
};

#endif /* NEWCLASSES */

JSClass MFNodeClass = {
	"MFNode",
	JSCLASS_HAS_PRIVATE,
	MFNodeAddProperty,
	JS_PropertyStub,
	MFNodeGetProperty,
	MFNodeSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFNodeFunctions)[] = {
	{"toString", MFNodeToString, 0},
	{"assign", MFNodeAssign, 0},
	{0}
};



JSClass MFRotationClass = {
	"MFRotation",
	JSCLASS_HAS_PRIVATE,
	MFRotationAddProperty,
	JS_PropertyStub,
	MFRotationGetProperty,
	MFRotationSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFRotationFunctions)[] = {
	{"toString", MFRotationToString, 0},
	{"assign", MFRotationAssign, 0},
	{0}
};



/* this one is not static. */
JSClass MFStringClass = {
	"MFString",
	JSCLASS_HAS_PRIVATE,
	MFStringAddProperty,
	MFStringDeleteProperty, /* JS_PropertyStub, */
	MFStringGetProperty,
	MFStringSetProperty,
	MFStringEnumerateProperty, /* JS_EnumerateStub, */
	MFStringResolveProperty, /* JS_ResolveStub, */
	MFStringConvertProperty, /* JS_ConvertStub, */
	JS_MY_Finalize
};

JSFunctionSpec (MFStringFunctions)[] = {
	{"toString", MFStringToString, 0},
	{"assign", MFStringAssign, 0},
	{0}
};



JSClass MFTimeClass = {
	"MFTime",
	JSCLASS_HAS_PRIVATE,
	MFTimeAddProperty,
	JS_PropertyStub,
	MFTimeGetProperty,
	MFTimeSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSPropertySpec (MFTimeProperties)[] = { 
 	{0} 
};

JSFunctionSpec (MFTimeFunctions)[] = {
	{"toString", MFTimeToString, 0},
	{"assign", MFTimeAssign, 0},
	{0}
};



JSClass MFVec2fClass = {
	"MFVec2f",
	JSCLASS_HAS_PRIVATE,
	MFVec2fAddProperty,
	JS_PropertyStub,
	MFVec2fGetProperty,
	MFVec2fSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFVec2fFunctions)[] = {
	{"toString", MFVec2fToString, 0},
	{"assign", MFVec2fAssign, 0},
	{0}
};



JSClass MFVec3fClass = {
	"MFVec3f",
	JSCLASS_HAS_PRIVATE,
	MFVec3fAddProperty,
	JS_PropertyStub,
	MFVec3fGetProperty,
	MFVec3fSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (MFVec3fFunctions)[] = {
	{"toString", MFVec3fToString, 0},
	{"assign", MFVec3fAssign, 0},
	{0}
};

JSObject *proto_VrmlMatrix;

JSClass VrmlMatrixClass = {
	"VrmlMatrix",
	JSCLASS_HAS_PRIVATE,
	VrmlMatrixAddProperty,
	JS_PropertyStub,
	VrmlMatrixGetProperty,
	VrmlMatrixSetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (VrmlMatrixFunctions)[] = {
	{"setTransform", VrmlMatrixsetTransform, 0},
	{"getTransform", VrmlMatrixgetTransform, 0},
	{"inverse", VrmlMatrixinverse, 0},
	{"transpose", VrmlMatrixtranspose, 0},
	{"multLeft", VrmlMatrixmultLeft, 0},
	{"multRight", VrmlMatrixmultRight, 0},
	{"multVecMatrix", VrmlMatrixmultVecMatrix, 0},
	{"multMatrixVec", VrmlMatrixmultMatrixVec, 0},
	{"toString", VrmlMatrixToString, 0},
	{"assign", VrmlMatrixAssign, 0},
	{0}
};


#ifdef NEWCLASSES

JSObject *proto_X3DMatrix3;

JSClass X3DMatrix3Class = {
	"X3DMatrix3",
	JSCLASS_HAS_PRIVATE,
	X3DMatrix3AddProperty,
	JS_PropertyStub,
	X3DMatrix3GetProperty,
	X3DMatrix3SetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (X3DMatrix3Functions)[] = {
	{"getTransform", X3DMatrix3getTransform, 0},
	{"setTransform", X3DMatrix3setTransform, 0},
	{"inverse", X3DMatrix3inverse, 0},
	{"transpose", X3DMatrix3transpose, 0},
	{"multLeft", X3DMatrix3multLeft, 0},
	{"multRight", X3DMatrix3multRight, 0},
	{"multVecMatrix", X3DMatrix3multVecMatrix, 0},
	{"multMatrixVec", X3DMatrix3multMatrixVec, 0},
	{"toString", X3DMatrix3ToString, 0},
	{"assign", X3DMatrix3Assign, 0},
	{0}
};


JSObject *proto_X3DMatrix4;

JSClass X3DMatrix4Class = {
	"X3DMatrix4",
	JSCLASS_HAS_PRIVATE,
	X3DMatrix4AddProperty,
	JS_PropertyStub,
	X3DMatrix4GetProperty,
	X3DMatrix4SetProperty,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JS_MY_Finalize
};

JSFunctionSpec (X3DMatrix4Functions)[] = {
	{"toString", X3DMatrix4ToString, 0},
	{"assign", X3DMatrix4Assign, 0},
	{"getTransform", X3DMatrix4getTransform, 0},
	{"setTransform", X3DMatrix4setTransform, 0},
	{"inverse", X3DMatrix4inverse, 0},
	{"transpose", X3DMatrix4transpose, 0},
	{"multLeft", X3DMatrix4multLeft, 0},
	{"multRight", X3DMatrix4multRight, 0},
	{"multVecMatrix", X3DMatrix4multVecMatrix, 0},
	{"multMatrixVec", X3DMatrix4multMatrixVec, 0},
	{0}
};

#endif /* NEWCLASSES */

//#define MFFloatProperties NULL
//#define MFInt32Properties NULL
//#define MFColorProperties NULL
//#define MFVec2fProperties NULL
//#define MFVec3fProperties NULL
//#define MFRotationProperties NULL
//#define MFNodeProperties NULL
//#define MFStringProperties NULL
//#define VrmlMatrixProperties NULL


struct JSLoadPropElement {
	JSClass *class;
	void *constr;
	void *Functions;
	void *Properties;
	char *id;
};


struct JSLoadPropElement (JSLoadProps) [] = {
#ifdef NEWCLASSES
        { &MFVec2dClass, MFVec2dConstr, &MFVec2dFunctions, &MFVec2dProperties, "MFVec2dClass"},
        { &MFVec3dClass, MFVec3dConstr, &MFVec3dFunctions, &MFVec3dProperties, "MFVec3dClass"},
        { &SFVec2dClass, SFVec2dConstr, &SFVec2dFunctions, &SFVec2dProperties, "SFVec2dClass"},
        { &MFBoolClass, MFBoolConstr, &MFBoolFunctions, &MFBoolProperties, "MFBoolClass"},
        { &MFDoubleClass, MFDoubleConstr, &MFDoubleFunctions, &MFDoubleProperties, "MFDoubleClass"},
        { &MFImageClass, MFImageConstr, &MFImageFunctions, &MFImageProperties, "MFImageClass"},
        { &X3DMatrix3Class, X3DMatrix3Constr, &X3DMatrix3Functions, &X3DMatrix3Properties, "X3DMatrix3Class"},
        { &X4DMatrix4Class, X4DMatrix4Constr, &X4DMatrix4Functions, &X4DMatrix4Properties, "X4DMatrix4Class"},
#endif /* NEWCLASSES */

        { &SFColorClass, SFColorConstr, &SFColorFunctions, &SFColorProperties, "SFColorClass"},
        { &SFVec2fClass, SFVec2fConstr, &SFVec2fFunctions, &SFVec2fProperties, "SFVec2fClass"},
        { &SFColorRGBAClass, SFColorRGBAConstr, &SFColorRGBAFunctions, &SFColorRGBAProperties, "SFColorRGBAClass"},
        { &SFVec3fClass, SFVec3fConstr, &SFVec3fFunctions, &SFVec3fProperties, "SFVec3fClass"},
        { &SFVec3dClass, SFVec3dConstr, &SFVec3dFunctions, &SFVec3dProperties, "SFVec3dClass"},
        { &SFRotationClass, SFRotationConstr, &SFRotationFunctions, &SFRotationProperties, "SFRotationClass"},
        { &SFNodeClass, SFNodeConstr, &SFNodeFunctions, &SFNodeProperties, "SFNodeClass"},
        { &MFFloatClass, MFFloatConstr, &MFFloatFunctions, NULL, "MFFloatClass"},
        { &MFTimeClass, MFTimeConstr, &MFTimeFunctions, &MFTimeProperties, "MFTimeClass"},
        { &MFInt32Class, MFInt32Constr, &MFInt32Functions, NULL, "MFInt32Class"},
        { &MFColorClass, MFColorConstr, &MFColorFunctions, NULL, "MFColorClass"},
        { &MFVec2fClass, MFVec2fConstr, &MFVec2fFunctions, NULL, "MFVec2fClass"},

        { &MFVec3fClass, MFVec3fConstr, &MFVec3fFunctions, NULL, "MFVec3fClass"},

        { &SFVec4fClass, SFVec4fConstr, &SFVec4fFunctions, &SFVec4fProperties, "SFVec4fClass"},
        { &SFVec4dClass, SFVec4dConstr, &SFVec4dFunctions, &SFVec4dProperties, "SFVec4dClass"},

        { &MFRotationClass, MFRotationConstr, &MFRotationFunctions, NULL, "MFRotationClass"},
        { &MFNodeClass, MFNodeConstr, &MFNodeFunctions, NULL, "MFNodeClass"},
        { &SFImageClass, SFImageConstr, &SFImageFunctions, &SFImageProperties, "SFImageClass"},
/*        { &MFColorRGBAClass, MFColorRGBAConstr, &MFColorRGBAFunctions, &MFColorRGBAProperties, "MFColorRGBAClass"},*/
        { &MFStringClass, MFStringConstr, &MFStringFunctions, NULL, "MFStringClass"},
        { &VrmlMatrixClass, VrmlMatrixConstr, &VrmlMatrixFunctions, NULL, "VrmlMatrixClass"},
        {0}
};



/* return the string representation of this class */
const char *classToString(JSClass *myClass) {

        int i;
	i = 0;

        /* printf ("class to string starting\n"); */

        /* ok - this is an object, lets find out what class it is */
        while (JSLoadProps[i].class != NULL) {
		if (JSLoadProps[i].class == myClass) {
			/* printf ("found it! it is a %s\n",JSLoadProps[i].id); */
                        return JSLoadProps[i].id;
                }
                i++;
        }
	return "class unknown";
}

/* try and print what an element is in case of error */
void printJSNodeType (JSContext *context, JSObject *myobj) {
	int i;
	i=0;
	
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("printJSNodeType, obj pointer is %p\n",myobj);
	#endif

	/* ok - this is an object, lets find out what class it is */
	while (JSLoadProps[i].class != NULL) {
		if (JS_InstanceOf(context, myobj, JSLoadProps[i].class, NULL)) {
			printf ("'%s'\n",JSLoadProps[i].id);
			return;
		}
		i++;
	}
	printf ("'unknown class'\n");
}

/* do a simple copy; from, to, and count */
JSBool _simplecopyElements (JSContext *cx,
		JSObject *fromObj,
		JSObject *toObj,
		int count,
		int type) {
	int i;
	jsval val;
	double dd;
	int ii;


	#ifdef JSVRMLCLASSESVERBOSE
	printf ("simpleCopyElements, count %d\n",count);
	#endif

	for (i = 0; i < count; i++) {
		if (!JS_GetElement(cx, fromObj, (jsint) i, &val)) {
			printf( "failed in get %d index %d.\n",type, i);
			return JS_FALSE;
		}
		/* ensure that the types are ok */
		if ((type == FIELDTYPE_SFFloat) || (type == FIELDTYPE_SFTime)) {
			/* if we expect a double, and we have an INT... */
			if (JSVAL_IS_INT(val)) {
				ii = JSVAL_TO_INT(val);
				dd = (double) ii;
				/* printf ("integer is %d doulbe %lf\n",ii,dd); */
		                if (JS_NewNumberValue(cx,dd,&val) == JS_FALSE) {
                		        printf( "JS_NewNumberValue failed for %f in simplecopyelements.\n",dd);
                        		return JS_FALSE;
                		}
			}
		}

		/*
		if (JSVAL_IS_OBJECT(val)) printf ("sc, element %d is an OBJECT\n",i);
		if (JSVAL_IS_STRING(val)) printf ("sc, element %d is an STRING\n",i);
		if (JSVAL_IS_NUMBER(val)) printf ("sc, element %d is an NUMBER\n",i);
		if (JSVAL_IS_DOUBLE(val)) printf ("sc, element %d is an DOUBLE\n",i);
		if (JSVAL_IS_INT(val)) printf ("sc, element %d is an INT\n",i);
		*/

		if (!JS_SetElement(cx, toObj, (jsint) i, &val)) {
			printf( "failed in set %d index %d.\n", type, i);
			return JS_FALSE;
		}
	}
	return JS_TRUE;
}


/* make a standard assignment for MF variables */
JSBool _standardMFAssign(JSContext *cx,
	JSObject *obj,
	uintN argc,
	jsval *argv,
	jsval *rval,
	JSClass *myClass,
	int type) {

	JSObject *_from_obj;
	jsval val;
	int32 len;
        SFImageNative *ptr;
#if JS_VERSION < 185
	char *_id_str;
#else
	JSString *_id_jsstr;
#endif

	if (!JS_InstanceOf(cx, obj, myClass, argv)) {
		printf("JS_InstanceOf failed for fieldType %s.\n",stringFieldtypeType(type));
		return JS_FALSE;
	}

#if JS_VERSION < 185
	if (!JS_ConvertArguments(cx, argc, argv, "o s", &_from_obj, &_id_str)) {
#else
	if (!JS_ConvertArguments(cx, argc, argv, "oS", &_from_obj, &_id_jsstr)) {
#endif
		printf("JS_ConvertArguments failed in %s.\n",stringFieldtypeType(type));
		return JS_FALSE;
	}


	if (!JS_InstanceOf(cx, _from_obj, myClass, argv)) {
		printf("JS_InstanceOf failed for fieldType %s.\n",stringFieldtypeType(type));
		return JS_FALSE;
	}

	if (!JS_GetProperty(cx, _from_obj, MF_LENGTH_FIELD, &val)) {
		printf("JS_GetProperty failed for \"%s\" in %s.\n",MF_LENGTH_FIELD,stringFieldtypeType(type));
		return JS_FALSE;
	}

	if (!JS_SetProperty(cx, obj, MF_LENGTH_FIELD, &val)) {
		printf("JS_SetProperty failed for \"%s\" in %s\n",MF_LENGTH_FIELD,stringFieldtypeType(type));
		return JS_FALSE;
	}

	len = JSVAL_TO_INT(val);

	#ifdef JSVRMLCLASSESVERBOSE
#if JS_VERSION >= 185
		_id_str = JS_EncodeString(cx,_id_jsstr);
#endif
		printf("StandardMFAssign %s: obj = %p, id = \"%s\", from = %p, len = %d\n",stringFieldtypeType(type),
		obj, _id_str, _from_obj, len);
#if JS_VERSION >= 185
		JS_free(cx,_id_str);
#endif
	#endif

	/* copyElements */
	*rval = OBJECT_TO_JSVAL(obj);

	/* SF* values that use this routine - check if we need to set valueChanged in private area */

	if (type == FIELDTYPE_SFImage) {
        	if ((ptr = (SFImageNative *)JS_GetPrivate(cx, obj)) == NULL) {
        	        printf( "JS_GetPrivate failed in standard MF assign.\n");
        	        return JS_FALSE;
        	}
		ptr->valueChanged = 1;
	}

	return _simplecopyElements(cx, _from_obj, obj, len,type);
}

/* standardized GetProperty for MF's */
JSBool
_standardMFGetProperty(JSContext *cx,
		JSObject *obj,
#if JS_VERSION < 185
		jsval id,
#else
		jsid iid,
#endif
		jsval *vp,
		char *makeNewElement,
		int type) {

	int32 _length, _index;
	jsval _length_val;

	/*  in case we need to run makeNewElement*/
	int newElemenLen;
	jsval newEle;
#if JS_VERSION >= 185
	jsval id;
	if (!JS_IdToValue(cx,iid,&id)) {
		printf( "JS_IdToValue failed\n");
		return JS_FALSE;
	}
#endif

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("_standardMFGetProperty starting for type %d\n",type);
	printJSNodeType (cx,obj);
	#endif

	if (!JS_GetProperty(cx, obj, MF_LENGTH_FIELD, &_length_val)) {
		printf( "JS_GetProperty failed for \"%s\" in %d.\n",MF_LENGTH_FIELD,type);
		return JS_FALSE;
	}

	_length = JSVAL_TO_INT(_length_val);
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("standarg get property, len %d\n",_length);
	#endif

	if (JSVAL_IS_INT(id)) {
		_index = JSVAL_TO_INT(id);
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("standard get property, index requested %d length is %d\n",_index,_length);
		#endif

		if (_index >= _length) {
			#ifdef JSVRMLCLASSESVERBOSE
			printf ("\n\nconstructing new object\n");
			#endif
			/*  we were making this with C calls, but it would fail with a*/
			/*  segfault; so, now, we run a script to do it.*/


			newElemenLen = (int)strlen(makeNewElement);

			if (!JS_EvaluateScript(cx, obj, makeNewElement, newElemenLen,
				FNAME_STUB, LINENO_STUB, &newEle)) {
				ConsoleMessage ("standardMFGetProperty: JS_EvaluateScript failed for %s", makeNewElement);
				return JS_FALSE;
			}

/* error? newEle is already a jsval
			*vp = OBJECT_TO_JSVAL(newEle); */
			*vp = newEle;

			#ifdef JSVRMLCLASSESVERBOSE
			printf ("defining element %d now... is %d %#x\n",_index,(int)*vp,(unsigned int)*vp);
			#endif

			if (!JS_DefineElement(cx, obj, (jsint) _index, *vp,
				JS_GET_PROPERTY_STUB, JS_SET_PROPERTY_STUB8,
				JSPROP_ENUMERATE)) {
				printf( "JS_DefineElement failed in %d.\n",type);
				return JS_FALSE;
			}

			if (!doMFSetProperty(cx,obj,
#if JS_VERSION < 185
			                     id,
#else
			                     iid,
#endif
			                     vp,type)) {
				printf ("wow, cant assign property\n");
			}
		}
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("object might already have this index\n");
		#endif
		if (!JS_LookupElement(cx, obj, _index, vp)) {
			printf( "JS_LookupElement failed in %d.\n",type);
			return JS_FALSE;
		}
		if (JSVAL_IS_NULL(*vp)) {
			printf( "warning: %d: obj = %p, jsval = %d does not exist!\n",type,
				obj, (int) _index);
			return JS_TRUE;
		}
	} else if (JSVAL_IS_STRING(id)) {
		#ifdef JSVRMLCLASSESVERBOSE
		JSString *_str;
		char * asciiStr;

		printf ("HAVE STRING HERE!\n");
		_str = JS_ValueToString(cx, id);
#if JS_VERSION < 185
		asciiStr = JS_GetStringBytes(_str);
#else
		asciiStr = JS_EncodeString(cx,_str);
#endif
		printf ("we have as a parameter :%s:\n",asciiStr);
#if JS_VERSION >= 185
		JS_free(cx,asciiStr);
#endif
		#endif

	}
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("_standardMFGetProperty finishing; element is %u\n",(unsigned int)*vp);
	#endif

	return JS_TRUE;
}

JSBool doMFToString(JSContext *cx, JSObject *obj, const char *className, jsval *rval)
{
    JSString *_str, *_tmpStr;
    jsval _v;
	char *_buff, *_tmp_valStr, *_tmp_buff;
	const char *_empty_array = "[]";
    int len = 0, i;
	size_t buff_size = 0, tmp_valStr_len = 0, tmp_buff_len = 0;
	JSBool isString = JS_FALSE;
	JSBool isImage = JS_FALSE;
#if JS_VERSION >= 185
	JSBool encodedTmpValstr = JS_FALSE;
#endif

    if (!JS_GetProperty(cx, obj, MF_LENGTH_FIELD, &_v)) {
		printf( "JS_GetProperty failed for \"%s\" in doMFToString for %s.\n",
				MF_LENGTH_FIELD,className);
        return JS_FALSE;
	}
	len = JSVAL_TO_INT(_v);

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("doMFToString, obj %p len %d\n",obj, len);
	printJSNodeType (cx,obj);
	#endif

	if (len == 0) {
		_str = JS_NewStringCopyZ(cx, _empty_array);
		*rval = STRING_TO_JSVAL(_str);
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("doMFToString, len is zero, returning JS_TRUE, and %d\n",(int)*rval);
		#endif
		return JS_TRUE;
	}

	if (!strcmp(className, "MFString")) {
		isString = JS_TRUE;
	}
	if (!strcmp(className, "SFImage")) {
		isImage = JS_TRUE;
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("doMFToString - doing an image\n");
		#endif
	}

	buff_size = LARGESTRING;
	_buff = MALLOC(char *, buff_size * sizeof(char));
	memset(_buff, 0, buff_size);

    for (i = 0; i < len; i++) {
		if (!JS_GetElement(cx, obj, i, &_v)) {
			printf("warning, no element %d of %d in doMFToString for a type of %s.\n",
				i, len,className);
			_tmp_valStr = "NULL";
		} else {

			#ifdef JSVRMLCLASSESVERBOSE
			if (JSVAL_IS_NUMBER(_v)) printf ("is a number\n");
			if (JSVAL_IS_INT(_v)) printf ("is an integer\n");
			if (JSVAL_IS_DOUBLE(_v)) printf ("is an double\n");
			#endif

			_tmpStr = JS_ValueToString(cx, _v);
			if (_tmpStr==NULL) {
				_tmp_valStr = "NULL";
			} else {
#if JS_VERSION < 185
				_tmp_valStr = JS_GetStringBytes(_tmpStr);
#else
				_tmp_valStr = JS_EncodeString(cx,_tmpStr);
				encodedTmpValstr = JS_TRUE;
#endif
			}
		}

		#ifdef JSVRMLCLASSESVERBOSE
		printf ("doMFToString, element %d is %#x, string %s\n",i,(unsigned int)_v,_tmp_valStr);
	
		#endif
		tmp_valStr_len = strlen(_tmp_valStr) + 1;
		tmp_buff_len = strlen(_buff);

		if ((buff_size - (tmp_buff_len + 1)) < (tmp_valStr_len + 1)) {
			buff_size += LARGESTRING;
			if ((_buff =
				 (char *)
				 JS_realloc(cx, _buff, buff_size * sizeof(char *)))
				== NULL) {
				printf( "JS_realloc failed for %d in doMFToString for %s.\n", i, className);
#if JS_VERSION >= 185
				if (encodedTmpValstr == JS_TRUE) JS_free(cx,_tmp_valStr);
#endif
				return JS_FALSE;
			}
		}

		if (len == 1) {
			if (isString) {
				sprintf(_buff, "[ \"%.*s\" ]", (int) tmp_valStr_len, _tmp_valStr);
			} else {
				sprintf(_buff, "[ %.*s ]", (int) tmp_valStr_len, _tmp_valStr);
			}
#if JS_VERSION >= 185
			if (encodedTmpValstr == JS_TRUE) {
				JS_free(cx,_tmp_valStr);
				encodedTmpValstr = JS_FALSE;
			}
#endif
			break;
		}

		_tmp_buff = MALLOC(char *, (tmp_buff_len + 1) * sizeof(char));
		memset(_tmp_buff, 0, tmp_buff_len + 1);
		memmove(_tmp_buff, _buff, tmp_buff_len);
		memset(_buff, 0, buff_size);

		if (i == 0 && len > 1) {
			if (isString) {
				sprintf(_buff, "[ \"%.*s\"",
						(int) tmp_valStr_len, _tmp_valStr);
			} else {
				sprintf(_buff, "[ %.*s", (int) tmp_valStr_len, _tmp_valStr);
			}
		} else if (i == (len - 1)) {
			if (isString) {
				sprintf(_buff, "%.*s, \"%.*s\" ]",
					(int) tmp_buff_len, _tmp_buff, (int) tmp_valStr_len, _tmp_valStr);
			} else {
				sprintf(_buff, "%.*s, %.*s ]",
					(int) tmp_buff_len, _tmp_buff, (int) tmp_valStr_len, _tmp_valStr);
			}
		} else {
			if (isString) {
				sprintf(_buff, "%.*s, \"%.*s\"",
					(int) tmp_buff_len, _tmp_buff, (int) tmp_valStr_len, _tmp_valStr);
			} else {
				sprintf(_buff, "%.*s, %.*s",
					(int) tmp_buff_len, _tmp_buff, (int) tmp_valStr_len, _tmp_valStr);
			}
		}

		FREE_IF_NZ (_tmp_buff);
#if JS_VERSION >= 185
		if (encodedTmpValstr == JS_TRUE) {
			JS_free(cx,_tmp_valStr);
			encodedTmpValstr = JS_FALSE;
		}
#endif
    }

	/* PixelTextures are stored in Javascript as MFInt32s but in FreeWRL/Perl as an ascii string.
	   we have to remove some characters to make it into a valid VRML image string */
	if (isImage) {
		for (i=0; i<strlen(_buff); i++) {
			if (_buff[i] == ',') _buff[i]=' ';
			if (_buff[i] == ']') _buff[i]=' ';
			if (_buff[i] == '[') _buff[i]=' ';
		}
	}
	/* printf ("domfstring, buff %s\n",_buff);*/
	_str = JS_NewStringCopyZ(cx, _buff);
	*rval = STRING_TO_JSVAL(_str);

	FREE_IF_NZ (_buff);
    return JS_TRUE;
}

JSBool
#if JS_VERSION < 185
doMFAddProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp, char *name) {
#else
doMFAddProperty(JSContext *cx, JSObject *obj, jsid iid, jsval *vp, char *name) {
#endif
	JSString *str;
	jsval v;
	char *p;
	int len, ind;
#if JS_VERSION >= 185
	jsval id;
	if (!JS_IdToValue(cx,iid,&id)) {
		printf("\tdoMFAddProperty:%s JS_IdToValue failed\n",name);
		return JS_FALSE;
	}
#endif
	len = 0;
	ind = JSVAL_TO_INT(id);

	#ifdef JSVRMLCLASSESVERBOSE
		printf("\tdoMFAddProperty:%s id %d (%d) NodeType: ",name,(int)id,ind);
		printJSNodeType(cx,obj);
	#endif

	str = JS_ValueToString(cx, id);
#if JS_VERSION < 185
	p = JS_GetStringBytes(str);
#else
	p = JS_EncodeString(cx,str);
#endif
	#ifdef JSVRMLCLASSESVERBOSE
		printf("\tid string  %s\n ",p);
	#endif

	if (!strcmp(p, MF_LENGTH_FIELD) ||
		!strcmp(p, "MF_ECMA_has_changed") ||
		!strcmp(p, "_parentField") ||
		!strcmp(p, "toString") ||
		!strcmp(p, "setTransform") ||
		!strcmp(p, "assign") ||
		!strcmp(p, "inverse") ||
		!strcmp(p, "transpose") ||
		!strcmp(p, "multLeft") ||
		!strcmp(p, "multRight") ||
		!strcmp(p, "multVecMatrix") ||
		!strcmp(p, "multMatrixVec") ||
		!strcmp(p, "constructor") ||
		!strcmp(p, "getTransform")) {
		#ifdef JSVRMLCLASSESVERBOSE
			printf("property \"%s\" is one of the standard properties. Do nothing.\n", p);
		#endif
#if JS_VERSION >= 185
		JS_free(cx,p);
#endif
		return JS_TRUE;
	}

	#ifdef JSVRMLCLASSESVERBOSE
		printf("\tdoMFAddProperty:%s id %d NodeType: ",name,(int)id);
		printJSNodeType(cx,obj);
		printf("\tdoMFAddProperty:%s id %d string %s ",name,(int)id,p);
	#endif
#if JS_VERSION >= 185
	JS_free(cx,p);
#endif

	if (!JSVAL_IS_INT(id)){ 
		printf( "JSVAL_IS_INT failed for id in doMFAddProperty.\n");
		return JS_FALSE;
	}
	if (!JS_GetProperty(cx, obj, MF_LENGTH_FIELD, &v)) {
		printf( "JS_GetProperty failed for \"%s\" in doMFAddProperty.\n",MF_LENGTH_FIELD);
		return JS_FALSE;
	}

	len = JSVAL_TO_INT(v);
	if (ind >= len) {
		len = ind + 1;

		#ifdef JSVRMLCLASSESVERBOSE
		printf ("doMFAddProperty, len %d ind %d\n",len,ind);
		#endif

		v = INT_TO_JSVAL(len);
		if (!JS_SetProperty(cx, obj, MF_LENGTH_FIELD, &v)) {
			printf( "JS_SetProperty failed for \"%s\" in doMFAddProperty.\n",MF_LENGTH_FIELD);
			return JS_FALSE;
		}
	}

	#ifdef JSVRMLCLASSESVERBOSE
		printf("index = %d, length = %d\n", ind, len);
	#endif

	return JS_TRUE;
}


JSBool
#if JS_VERSION < 185
doMFSetProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp, int type) {
#else
doMFSetProperty(JSContext *cx, JSObject *obj, jsid iid, jsval *vp, int type) {
#endif
	JSString *_sstr;
	jsval myv;
	int i;
	double dd;

        int ii;

	JSObject *par;
	JSObject *me;
	jsval pf;
	jsval nf;
	char * _cc;
	jsid oid;
#if JS_VERSION >= 185
	jsval id;
	if (!JS_IdToValue(cx,iid,&id)) {
		printf("doMFSetProperty, JS_IdToValue failed.\n");
		return JS_FALSE;
	}
#endif

	#ifdef JSVRMLCLASSESVERBOSE
	JSString *_str;
	char * _c;
		printf ("doMFSetProperty, for object %p, vp %u\n", obj,(unsigned int)*vp);
		_str = JS_ValueToString(cx, id);
#if JS_VERSION < 185
		_c = JS_GetStringBytes(_str);
#else
		_c = JS_EncodeString(cx,_str);
#endif
		printf ("id is %s\n",_c);

		_sstr = JS_ValueToString(cx, *vp);
		printf ("looking up value for %d %#x object %p\n",(int)*vp,(unsigned int)*vp,obj);
#if JS_VERSION < 185
			_cc = JS_GetStringBytes(_sstr);
#else
			_cc = JS_EncodeString(cx,_sstr);
#endif
			printf("\tdoMFSetProperty:%d: obj = %p, id = %s, vp = %s\n",type,
			   obj, _c, _cc);
		if (JSVAL_IS_OBJECT(*vp)) { printf ("doMFSet, vp is an OBJECT\n"); }
		if (JSVAL_IS_PRIMITIVE(*vp)) { printf ("doMFSet, vp is an PRIMITIVE\n"); }

		printf ("parent is a "); printJSNodeType(cx,obj);
		/* printf ("jsval is is a "); printJSNodeType(cx,*vp);  */
#if JS_VERSION >= 185
		JS_free(cx,_c);
		JS_free(cx,_cc);
#endif
	#endif

	/* should this value be checked for possible conversions */
	if (type == FIELDTYPE_MFInt32) {
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("doMFSetProperty, this should be an int \n");
		#endif

		if (!JSVAL_IS_INT(*vp)) {
			#ifdef JSVRMLCLASSESVERBOSE
			printf ("is NOT an int\n");
			#endif

			if (!JS_ValueToInt32(cx, *vp, &i)) {
				_sstr = JS_ValueToString(cx, *vp);
#if JS_VERSION < 185
				_cc = JS_GetStringBytes(_sstr);
#else
				_cc = JS_EncodeString(cx,_sstr);
#endif
				printf ("can not convert %s to an integer in doMFAddProperty for type %d\n",_cc,type);
#if JS_VERSION >= 185
				JS_free(cx,_cc);
#endif
				return JS_FALSE;
			}

			*vp = INT_TO_JSVAL(i);
		}
	} else if ((type == FIELDTYPE_MFFloat) || (type == FIELDTYPE_MFTime)) {
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("doMFSetProperty - ensure that this is a DOUBLE ");
				_sstr = JS_ValueToString(cx, *vp);
#if JS_VERSION < 185
				_cc = JS_GetStringBytes(_sstr);
#else
				_cc = JS_EncodeString(cx,_sstr);
#endif
				printf ("value is  %s \n",_cc);
#if JS_VERSION >= 185
				JS_free(cx,_cc);
#endif
		#endif

		if (JSVAL_IS_INT(*vp)) {
			ii = JSVAL_TO_INT(*vp);
			dd = (double) ii;
			/* printf ("integer is %d doulbe %lf\n",ii,dd); */
	                if (JS_NewNumberValue(cx,dd,vp) == JS_FALSE) {
               		        printf( "JS_NewNumberValue failed for %f in simplecopyelements.\n",dd);
                       		return JS_FALSE;
               		}
		}
	}

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("setting changed flag on %p\n",obj);
	#endif     

	/* is this an MF ECMA type that uses the MF_ECMA_has_changed flag? */
	switch (type) {
		case FIELDTYPE_MFInt32:
		case FIELDTYPE_MFBool:
		case FIELDTYPE_MFTime:
		case FIELDTYPE_MFFloat:
		case FIELDTYPE_MFString: {
			SET_MF_ECMA_HAS_CHANGED
			break;
		}
		default: {}
	}


	if (JSVAL_IS_INT(id)) {
		/* save this element into the parent at index */

		#ifdef JSVRMLCLASSESVERBOSE
		printf ("saving element %d\n",JSVAL_TO_INT(id));
		#endif

		if (!JS_DefineElement(cx, obj, JSVAL_TO_INT(id), *vp,
			JS_GET_PROPERTY_STUB, JS_SET_PROPERTY_CHECK,
			JSPROP_ENUMERATE)) {
			printf( "JS_DefineElement failed in doMFSetProperty.\n");
			return JS_FALSE;
		}

		/* has the length changed? */
		if (!JS_GetProperty(cx, obj, MF_LENGTH_FIELD, &myv)) {
			printf("JS_GetProperty failed for \"%s\" in doMFSetProperty.\n", MF_LENGTH_FIELD);
			return JS_FALSE;
		}

		#ifdef JSVRMLCLASSESVERBOSE
		printf ("object %p old length %d, possibly new length is going to be %d\n",obj,JSVAL_TO_INT(myv), JSVAL_TO_INT(id)+1);
		#endif

		if (JSVAL_TO_INT(myv) < (JSVAL_TO_INT(id)+1)) {
			printf ("new length is %d\n",JSVAL_TO_INT(id)+1);
			myv = INT_TO_JSVAL(JSVAL_TO_INT(id)+1);
			if (!JS_SetProperty(cx, obj, MF_LENGTH_FIELD, &myv)) {
				printf("JS_SetProperty failed for \"%s\" in doMFSetProperty.\n", MF_LENGTH_FIELD);
				return JS_FALSE;
			}
		}
	}

	#ifdef JSVRMLCLASSESVERBOSE
	printf ("doMFSetProperty, lets see if we have an SFNode somewhere up the chain...\n");
	#endif

        /* ok - if we are setting an MF* field by a thing like myField[10] = new String(); the
           set method does not really get called. So, we go up the parental chain until we get
           either no parent, or a SFNode. If we get a SFNode, we call the "save this" function
           so that the X3D scene graph gets the updated array value. To make a long story short,
           here's the call to find the parent for the above. */

	me = obj;
	par = JS_GetParent(cx, me);
	while (par != NULL) {
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("for obj %p: ",me);
			printJSNodeType(cx,me);
		printf ("... parent %p\n",par);
			printJSNodeType(cx,par);
		#endif

		if (JS_InstanceOf (cx, par, &SFNodeClass, NULL)) {
			#ifdef JSVRMLCLASSESVERBOSE
			printf (" the parent IS AN SFNODE - it is %p\n",par);
			#endif


			if (!JS_GetProperty(cx, obj, "_parentField", &pf)) {
				printf ("doMFSetProperty, can not get parent field from this object\n");
				return JS_FALSE;
			}

			nf = OBJECT_TO_JSVAL(me);

			#ifdef JSVRMLCLASSESVERBOSE
			#if JS_VERSION < 185
			printf ("parentField is %u \"%s\"\n", (unsigned int)pf, JS_GetStringBytes(JSVAL_TO_STRING(pf)));
			#else
			_cc = JS_EncodeString(cx,JSVAL_TO_STRING(pf));
			printf ("parentField is %u \"%s\"\n", (unsigned int)pf, _cc);
			JS_free(cx,_cc);
			#endif
			#endif

			if (!JS_ValueToId(cx,pf,&oid)) {
				printf("doMFSetProperty: JS_ValueToId failed.\n");
				return JS_FALSE;
			}

			if (!setSFNodeField (cx, par, oid,
#if JS_VERSION >= 185
			    JS_FALSE,
#endif
			    &nf)) {
				printf ("could not set field of SFNode\n");
			}

		}
		me = par;
		par = JS_GetParent(cx, me);
	}

	return JS_TRUE;
}

JSBool
doMFStringUnquote(JSContext *cx, jsval *vp)
{
	JSString *_str, *_vpStr;
	char *_buff, *_tmp_vpStr;
	size_t _buff_len;
	unsigned int i, j = 0;

	_str = JS_ValueToString(cx, *vp);
#if JS_VERSION < 185
	_buff = JS_GetStringBytes(_str);
#else
	_buff = JS_EncodeString(cx,_str);
#endif
	_buff_len = strlen(_buff) + 1;

	#ifdef JSVRMLCLASSESVERBOSE
		printf("doMFStringUnquote: vp = \"%s\"\n", _buff);
	#endif

	if (memchr(_buff, '"', _buff_len) != NULL) {
		_tmp_vpStr = MALLOC(char *, _buff_len * sizeof(char));

		memset(_tmp_vpStr, 0, _buff_len);

		for (i = 0; i <= (_buff_len-1); i++) {
			if (_buff[i] != '"' ||
				(i > 0 && _buff[i - 1] == '\\')) {
				_tmp_vpStr[j++] = _buff[i];
			}
		}
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("new unquoted string %s\n",_tmp_vpStr);
		#endif

		_vpStr = JS_NewStringCopyZ(cx, _tmp_vpStr);
		*vp = STRING_TO_JSVAL(_vpStr);

		FREE_IF_NZ (_tmp_vpStr);
	}
#if JS_VERSION >= 185
	JS_free(cx,_buff);
#endif

	return JS_TRUE;
}


JSBool
#if JS_VERSION < 185
globalResolve(JSContext *cx, JSObject *obj, jsval id)
#else
globalResolve(JSContext *cx, JSObject *obj, jsid id)
#endif
{
	UNUSED(cx);
	UNUSED(obj);
	UNUSED(id);
	return JS_TRUE;
}


/* load the FreeWRL extra classes */
JSBool loadVrmlClasses(JSContext *context, JSObject *globalObj) {
	jsval v;
	int i;

	JSObject *myProto;
	
	i=0;
	while (JSLoadProps[i].class != NULL) {
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("loading %s\n",JSLoadProps[i].id);
		#endif

		/* v = 0; */
		if (( myProto = JS_InitClass(context, globalObj, NULL, JSLoadProps[i].class,
			  JSLoadProps[i].constr, INIT_ARGC, JSLoadProps[i].Properties,
			  JSLoadProps[i].Functions, NULL, NULL)) == NULL) {
			printf("JS_InitClass for %s failed in loadVrmlClasses.\n",JSLoadProps[i].id);
			return JS_FALSE;
		}
		v = OBJECT_TO_JSVAL(myProto);
		if (!JS_SetProperty(context, globalObj, JSLoadProps[i].id, &v)) {
			printf("JS_SetProperty for %s failed in loadVrmlClasses.\n",JSLoadProps[i].id);
			return JS_FALSE;
		}

		i++;
	}
	return JS_TRUE;
}

/* go through and see if the setECMA routine touched this name */
int findNameInECMATable(JSContext *context, char *toFind) {
	int i;
	ppjsVRMLClasses p = (ppjsVRMLClasses)gglobal()->jsVRMLClasses.prv;
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("findNameInECMATable, looking for %s context %p\n",toFind,context);
	#endif
	
	i=0;
	while (i < p->maxECMAVal) { 
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("	%d: %s==%s cx %p==%p\n",i,p->ECMAValues[i].name,toFind,p->ECMAValues[i].context,context);
		#endif

		
		if ((p->ECMAValues[i].context == context) && (strcmp(p->ECMAValues[i].name,toFind)==0)) {
			#ifdef JSVRMLCLASSESVERBOSE
			printf ("fineInECMATable: found value at %d\n",i);
			#endif
			return p->ECMAValues[i].valueChanged;
		}
		i++;
	}
	
	/* did not find this one, add it */
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("findInECMATable - did not find %s\n",toFind);
	#endif

	return FALSE;
}

/* go through the ECMA table, and reset the valueChanged flag. */
void resetNameInECMATable(JSContext *context, char *toFind) {
	int i;
	ppjsVRMLClasses p = (ppjsVRMLClasses)gglobal()->jsVRMLClasses.prv;
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("findNameInECMATable, looking for %s\n",toFind);
	#endif
	
	i=0;
	while (i < p->maxECMAVal) { 
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("	%d: %s==%s cx %p==%p\n",i,p->ECMAValues[i].name,toFind,p->ECMAValues[i].context,context);
		#endif

		
		if ((p->ECMAValues[i].context == context) && (strcmp(p->ECMAValues[i].name,toFind)==0)) {
			#ifdef JSVRMLCLASSESVERBOSE
			printf ("fineInECMATable: found value at %d\n",i);
			#endif
			p->ECMAValues[i].valueChanged = FALSE;
			return;
		}
		i++;
	}
}

/* set the valueChanged flag - add a new entry to the table if required */
void setInECMATable(JSContext *context, char *toFind) {
	int i;
	ppjsVRMLClasses p = (ppjsVRMLClasses)gglobal()->jsVRMLClasses.prv;
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("setInECMATable, looking for %s\n",toFind);
	#endif
	
	i=0;
	while (i < p->maxECMAVal) { 
		#ifdef JSVRMLCLASSESVERBOSE
		printf ("	%d: %s==%s cx %p==%p\n",i,p->ECMAValues[i].name,toFind,p->ECMAValues[i].context,context);
		#endif

		
		if ((p->ECMAValues[i].context == context) && (strcmp(p->ECMAValues[i].name,toFind)==0)) {
			#ifdef JSVRMLCLASSESVERBOSE
			printf ("setInECMATable: found value at %d\n",i);
			#endif
			p->ECMAValues[i].valueChanged = TRUE;
			return;
		}
		i++;
	}
	
	/* did not find this one, add it */
	#ifdef JSVRMLCLASSESVERBOSE
	printf ("setInECMATable - new entry at %d for %s\n",p->maxECMAVal, toFind);
	#endif

	p->maxECMAVal ++;
	if (p->maxECMAVal == ECMAValueTableSize) {
		ConsoleMessage ("problem in setInECMATable for scripting\n");
		p->maxECMAVal = ECMAValueTableSize - 10;
	}
#if JS_VERSION < 185
	/* Dangerous code, taking a string and casting it directly -- why does this happen? */
	p->ECMAValues[p->maxECMAVal-1].JS_address = (jsval) toFind;
#else
	/* since this seems to never be used anyways .. */
	p->ECMAValues[p->maxECMAVal-1].JS_address = JSVAL_ZERO;
#endif
	p->ECMAValues[p->maxECMAVal-1].valueChanged = TRUE;
	p->ECMAValues[p->maxECMAVal-1].name = STRDUP(toFind);
	p->ECMAValues[p->maxECMAVal-1].context = context;
}

JSBool
#if JS_VERSION < 185
setECMANative(JSContext *context, JSObject *obj, jsval id, jsval *vp)
#else
setECMANative(JSContext *context, JSObject *obj, jsid iid, JSBool strict, jsval *vp)
#endif
{
	JSString *_idStr;
	JSString *_vpStr, *_newVpStr;
	JSBool ret = JS_TRUE;
	char *_id_c;

	char *_vp_c, *_new_vp_c;
	size_t len = 0;
#if JS_VERSION >= 185
	jsval id;
	if (!JS_IdToValue(context,iid,&id)) {
		printf( "JS_IdToValue failed\n");
		return JS_FALSE;
	}
#endif

	_idStr = JS_ValueToString(context, id);
#if JS_VERSION < 185
	_id_c = JS_GetStringBytes(_idStr);
#else
	_id_c = JS_EncodeString(context,_idStr);
#endif

        /* "register" this ECMA value for routing changed flag stuff */
       	setInECMATable(context, _id_c);

	if (JSVAL_IS_STRING(*vp)) {
		_vpStr = JS_ValueToString(context, *vp);
#if JS_VERSION < 185
		_vp_c = JS_GetStringBytes(_vpStr);
#else
		_vp_c = JS_EncodeString(context,_vpStr);
#endif

		len = strlen(_vp_c);


		/* len + 3 for '\0' and "..." */
		_new_vp_c = MALLOC(char *, (len + 3) * sizeof(char));
		/* JAS - do NOT prepend/append double quotes to this string*/
		/* JAS - only for the null terminator*/
		/* JAS len += 3;*/
		len += 1;

		memset(_new_vp_c, 0, len);
		/* JAS sprintf(_new_vp_c, "\"%.*s\"", len, _vp_c);*/
		sprintf(_new_vp_c, "%.*s", (int) len, _vp_c);
		_newVpStr = JS_NewStringCopyZ(context, _new_vp_c);
		*vp = STRING_TO_JSVAL(_newVpStr);

		#ifdef JSVRMLCLASSESVERBOSE
			printf("setECMANative: have string obj = %p, id = \"%s\", vp = %s\n",
				   obj, _id_c, _new_vp_c);
		#endif
		FREE_IF_NZ (_new_vp_c);
#if JS_VERSION >= 185
		JS_free(context,_vp_c);
#endif
	} else {
		#ifdef JSVRMLCLASSESVERBOSE
		_vpStr = JS_ValueToString(context, *vp);
#if JS_VERSION < 185
		_vp_c = JS_GetStringBytes(_vpStr);
#else
		_vp_c = JS_EncodeString(context,_vpStr);
#endif
		printf("setECMANative: obj = %p, id = \"%s\", vp = %s\n",
			   obj, _id_c, _vp_c);
#if JS_VERSION >= 185
		JS_free(context,_vp_c);
#endif
		#endif
	}
#if JS_VERSION >= 185
	JS_free(context,_id_c);
#endif
	return ret;
}

/* used mostly for debugging */
JSBool
#if JS_VERSION < 185
getAssignProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
#else
getAssignProperty(JSContext *cx, JSObject *obj, jsid iid, jsval *vp)
#endif
{
	#ifdef JSVRMLCLASSESVERBOSE
	JSString *_idStr, *_vpStr;
	char *_id_c, *_vp_c;

#if JS_VERSION >= 185
	jsval id;
	if (!JS_IdToValue(cx,iid,&id)) {
		printf("getAssignProperty: JS_IdToValue failed -- returning JS_TRUE anyways\n");
	}
#endif

		_idStr = JS_ValueToString(cx, id);
		_vpStr = JS_ValueToString(cx, *vp);
#if JS_VERSION < 185
		_id_c = JS_GetStringBytes(_idStr);
		_vp_c = JS_GetStringBytes(_vpStr);
#else
		_id_c = JS_EncodeString(cx,_idStr);
		_vp_c = JS_EncodeString(cx,_vpStr);
#endif
		printf("getAssignProperty: obj = %p, id = \"%s\", vp = %s\n",
			   obj, _id_c, _vp_c);
	printf ("what is vp? \n");
	if (JSVAL_IS_OBJECT(*vp)) printf ("is OBJECT\n");
	if (JSVAL_IS_STRING(*vp)) printf ("is STRING\n");
	if (JSVAL_IS_INT(*vp)) printf ("is INT\n");
	if (JSVAL_IS_DOUBLE(*vp)) printf ("is DOUBLE\n");

#if JS_VERSION >= 185
		JS_free(cx,_id_c);
		JS_free(cx,_vp_c);
#endif
	#endif
	return JS_TRUE;
}


/* a kind of hack to replace the use of JSPROP_ASSIGNHACK */
JSBool
#if JS_VERSION < 185
setAssignProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
#else
setAssignProperty(JSContext *cx, JSObject *obj, jsid iid, JSBool strict, jsval *vp)
#endif
{
	JSObject *_o;
	JSString *_str;
	const uintN _argc = 2;
#ifdef _MSC_VER
	jsval newVal, initVal, _argv[2]; /* win32 complains cant allocate array of size 0 */
#else
	jsval newVal, initVal, _argv[_argc];
#endif
	char *_id_c;
#if JS_VERSION >= 185
	jsval id;
	if (!JS_IdToValue(cx,iid,&id)) {
		printf("setAssignProperty: JS_IdToValue failed.\n");
		return JS_FALSE;
	}
#endif

	if (JSVAL_IS_STRING(id)) {
		if (!JS_ConvertValue(cx, *vp, JSTYPE_OBJECT, &newVal)) {
			printf( "JS_ConvertValue failed in setAssignProperty.\n");
			return JS_FALSE;
		}

		_str = JSVAL_TO_STRING(id);
#if JS_VERSION < 185
		_id_c = JS_GetStringBytes(_str);
#else
		_id_c = JS_EncodeString(cx,_str);
#endif
		if (!JS_GetProperty(cx, obj, _id_c, &initVal)) {
			printf( "JS_GetProperty failed in setAssignProperty.\n");
#if JS_VERSION >= 185
			JS_free(cx,_id_c);
#endif
			return JS_FALSE;
		}
		#ifdef JSVRMLCLASSESVERBOSE
			printf("setAssignProperty: obj = %p, id = \"%s\", from = %d, to = %d\n",
				   obj, _id_c, (int)newVal, (int)initVal);

                if (JSVAL_IS_OBJECT(initVal)) printf ("initVal is an OBJECT\n");
                if (JSVAL_IS_STRING(initVal)) printf ("initVal is an STRING\n");
                if (JSVAL_IS_NUMBER(initVal)) printf ("initVal is an NUMBER\n");
                if (JSVAL_IS_DOUBLE(initVal)) printf ("initVal is an DOUBLE\n");
                if (JSVAL_IS_INT(initVal)) printf ("initVal is an INT\n");

                if (JSVAL_IS_OBJECT(newVal)) printf ("newVal is an OBJECT\n");
                if (JSVAL_IS_STRING(newVal)) printf ("newVal is an STRING\n");
                if (JSVAL_IS_NUMBER(newVal)) printf ("newVal is an NUMBER\n");
                if (JSVAL_IS_DOUBLE(newVal)) printf ("newVal is an DOUBLE\n");
                if (JSVAL_IS_INT(newVal)) printf ("newVal is an INT\n");

                if (JSVAL_IS_OBJECT(id)) printf ("id is an OBJECT\n");
                if (JSVAL_IS_STRING(id)) printf ("id is an STRING\n");
                if (JSVAL_IS_NUMBER(id)) printf ("id is an NUMBER\n");
                if (JSVAL_IS_DOUBLE(id)) printf ("id is an DOUBLE\n");
                if (JSVAL_IS_INT(id)) printf ("id is an INT\n");

#if JS_VERSION < 185
		printf ("id is %s\n",JS_GetStringBytes(JS_ValueToString(cx,id)));
		printf ("initVal is %s\n",JS_GetStringBytes(JS_ValueToString(cx,initVal)));
		printf ("newVal is %s\n",JS_GetStringBytes(JS_ValueToString(cx,newVal)));
#else
		printf ("id is %s\n",JS_EncodeString(cx,JS_ValueToString(cx,id)));
		printf ("initVal is %s\n",JS_EncodeString(cx,JS_ValueToString(cx,initVal)));
		printf ("newVal is %s\n",JS_EncodeString(cx,JS_ValueToString(cx,newVal)));
#endif
		#endif
#if JS_VERSION >= 185
		JS_free(cx,_id_c);
#endif


		_o = JSVAL_TO_OBJECT(initVal);

		#ifdef xxJSVRMLCLASSESVERBOSE
			printf ("in setAssignProperty, o is %u type ",_o);
			printJSNodeType(cx,_o);
			printf ("\n");
		#endif
 
		_argv[0] = newVal;
		_argv[1] = id;

		if (!JS_CallFunctionName(cx, _o, "assign", _argc, _argv, vp)) {
			printf( "JS_CallFunctionName failed in setAssignProperty.\n");
			return JS_FALSE;
		}
	} else {
		#ifdef JSVRMLCLASSESVERBOSE
			_str = JS_ValueToString(cx, id);
#if JS_VERSION < 185
			_id_c = JS_GetStringBytes(_str);
#else
			_id_c = JS_EncodeString(cx,_str);
#endif
			printf("setAssignProperty: obj = %p, id = \"%s\"\n",
				   obj, _id_c);
#if JS_VERSION >= 185
			JS_free(cx,_id_c);
#endif
		#endif
	}

	return JS_TRUE;
}
#endif /* HAVE_JAVASCRIPT */
