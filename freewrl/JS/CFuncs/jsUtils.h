/*
 * Copyright (C) 1998 Tuomas J. Lukka, 2002 John Stewart, Ayla Khan CRC Canada
 * DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 * See the GNU Library General Public License
 * (file COPYING in the distribution) for conditions of use and
 * redistribution, EXCEPT on the files which belong under the
 * Mozilla public license.
 *
 * $Id: jsUtils.h,v 1.1.2.3 2002/08/30 04:53:50 ayla Exp $
 */

#ifndef __jsUtils_h__
#define __jsUtils_h__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <EXTERN.h>
#include <perl.h>


#include "jsapi.h" /* JS compiler */
#include "jsdbgapi.h" /* JS debugger */

#ifndef FALSE
#define FALSE 0
#endif /* FALSE */

#define LARGESTRING 2048
#define STRING 512
#define SMALLSTRING 128

#define BROWMAGIC 12345

#define FNAME_STUB "file"
#define LINENO_STUB 0

#define UNUSED(v) ((void) v)


extern JSBool verbose;
static JSBool reportWarnings = JS_TRUE;

extern void
doPerlCallMethod(SV *jssv, const char *methodName);

void
reportWarningsOn(void);

void
reportWarningsOff(void);

void
errorReporter(JSContext *cx,
			  const char *message,
			  JSErrorReport *report);


#endif /* __jsUtils_h__ */
