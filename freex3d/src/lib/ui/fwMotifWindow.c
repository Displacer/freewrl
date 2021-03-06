/*
  $Id: fwMotifWindow.c,v 1.43 2013/10/29 16:59:42 crc_canada Exp $

  FreeWRL support library.
  Create Motif window, widget, menu. Manage events.

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
#if !(defined(IPHONE) || defined(_ANDROID) || defined(AQUA))

#include <system.h>
#include <display.h>
#if KEEP_FV_INLIB
#include <internal.h>

#include <libFreeWRL.h>

#include <threads.h>

#include "../main/MainLoop.h"
#include "../vrml_parser/Structs.h"
#include "../opengl/OpenGL_Utils.h"
#include "../scenegraph/Viewer.h"
#include "../ui/common.h"

#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/TextF.h>
#include <Xm/Separator.h>
#include <Xm/PanedW.h>
#include <Xm/Text.h>
#include <Xm/ScrolledW.h>
#include <Xm/FileSB.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#include <Xm/DrawingA.h> /* simple drawing area */

#define ABOUT_FREEWRL "FreeWRL Version %s\n \
%s %s.\n \n \
FreeWRL is a VRML/X3D Browser for OS X and Unix.\n \n \
Thanks to the Open Source community for all the help received.\n \
http://freewrl.sf.net"

#define DJ_KEEP_COMPILER_WARNING 0

void setDefaultBackground(int colour);

#ifdef OLDCODE
OLDCODE /* background colours - must be sequential range */
#endif //OLDCODE 

#define colourBlack     0

#ifdef OLDCODE
OLDCODE #define colourRed       1
OLDCODE #define colourGreen     2
OLDCODE #define colourBlue      3
OLDCODE #define colourMagenta   4
OLDCODE #define colourYellow    5
OLDCODE #define colourCyan      6
OLDCODE #define colourGrey      7
OLDCODE #define colourOrange    8
#endif //OLDCODE
#define colourWhite     9

/* because of threading issues in Linux, if we can only use 1 thread, we
   delay setting of info until this time. */
int colbut; int colbutChanged = FALSE;
int headbut; int headbutChanged = FALSE;
int fl, ex, wa; int navbutChanged = FALSE;
int msgChanged = FALSE;
char *consMsg = NULL; int consmsgChanged = FALSE;

char fpsstr[MAXSTAT+20];

/* static String defaultResources[200]; */
int MainWidgetRealized = FALSE;
int msgWindowOnscreen = FALSE;
int consWindowOnscreen = FALSE;

XtAppContext Xtcx;
Widget freewrlTopWidget, mainw, menubar,menuworkwindow;
Widget menumessagewindow = NULL; /* tested against NULL in code */
Widget frame, freewrlDrawArea;
Widget headlightButton;
Widget collisionButton;
Widget walkButton, flyButton, examineButton;
Widget menumessageButton;
Widget consolemessageButton;
Widget consoleTextArea;
Widget consoleTextWidget;
Widget about_widget;
Widget newFileWidget;
Widget tex128_button, tex256_button, texFull_button, texturesFirstButton;

/* colour selection for default background */
Widget backgroundColourSelector[colourWhite+1];
String BackString[] = {"Black Background", "Red Background", "Green Background", "Blue Background", "Magenta Background", "Yellow Background", "Cyan Background", "Grey Background", "Orange Background", "White Background"};
float backgroundColours[] = {
    0.0, 0.0, 0.0,              /* black */
    0.8, 0.0, 0.0,              /* red */
    0.0, 0.8, 0.0,              /* green */
    0.0, 0.0, 0.8,              /* blue */
    0.8, 0.0, 0.8,              /* magenta */
    0.8, 0.8, 0.0,              /* yellow */            
    0.0, 0.8, 0.8,              /* cyan */
    0.8, 0.8, 0.8,              /* grey */
    1.0, 0.6, 0.0,              /* orange */
    1.0, 1.0, 1.0};             /* white */

Arg args[10];
Arg buttonArgs[10]; int buttonArgc = 0;
Arg textArgs[10]; int textArgc = 0;

extern char myMenuStatus[];

void fv_createMenuBar(void);
void fv_createDrawingFrame(void);


void myXtManageChild (int c, Widget child)
{
#ifdef XTDEBUG
    printf ("at %d, managing %d\n",c, child);
#endif
    if (child != NULL) XtManageChild (child);
}


/* see if/when we become iconified - if so, dont bother doing OpenGL stuff */
void StateWatcher (Widget w, XtPointer unused, XEvent *event, Boolean *cont)
{
#ifdef XEVENT_VERBOSE
    // Used to track down TouchSensor loosing event with Motif (direct X11 is ok)
    TRACE_MSG("freewrlTopWidget [StateWatch] went through (xm callback): widget %p event %p\n", (void*)w, (void*)event);
#endif
    if (event->type == MapNotify) setDisplayed (TRUE);
    else if (event->type == UnmapNotify) setDisplayed (FALSE);
}

void fv_DrawArea_events (Widget w, XtPointer unused, XEvent *event, Boolean *cont)
{
#ifdef XEVENT_VERBOSE 
    // Used to track down TouchSensor loosing event with Motif (direct X11 is ok)

    XWindowAttributes attr;
    XSetWindowAttributes set_attr;

    TRACE_MSG("fv_DrawArea event went through (xm callback): widget %p event %p\n", (void*)w, (void*)event);

    memset(&attr, 0, sizeof(attr));
    memset(&set_attr, 0, sizeof(set_attr));

    /* Get window attributes and examine the event mask */
    XGetWindowAttributes(Xdpy, Xwin, &attr);
    TRACE_MSG("DrawArea event mask: %lu\n", attr.your_event_mask);
    if (!(attr.your_event_mask & PointerMotionMask)) {
	TRACE_MSG("DrawArea window not configured to receive PointerMotionMask...\n");
    }
    /* Set event mask to catch mouse motion events */
    set_attr.event_mask = attr.your_event_mask | PointerMotionMask;
    XChangeWindowAttributes(Xdpy, Xwin, CWEventMask, &set_attr);

#endif

    /* This event should be passed to FreeWRL (MainLoop) control */
    DEBUG_XEV("EVENT through MOTIF\n");
    handle_Xevents(*event);
}

/**
 *   create_main_window: (virtual) create the window with Motif.
 */
int fv_create_main_window(freewrl_params_t * params) //int argc, char *argv[])
{
	int argc_out = 0;
	char *argv_out[1] = { NULL };
	Dimension width, height;
	Arg initArgs[10]; int initArgc = 0;

	/* XtVaAppInitialize ??? */
	XtSetArg(initArgs[initArgc], XmNlabelString, XmStringCreate(getWindowTitle(), XmSTRING_DEFAULT_CHARSET)); initArgc++;
	XtSetArg(initArgs[initArgc], XmNheight, params->height); initArgc++;
	XtSetArg(initArgs[initArgc], XmNwidth, params->width); initArgc++;
	XtSetArg(initArgs[initArgc], XmNmappedWhenManaged, False); initArgc++;

	/**
	 *   This new initialization sequence let us create the Display and GLX context "� part" from Motif and use the
	 *   same routines for bare X11 and Motif ...
	 */
	XtToolkitInitialize();
	Xtcx = XtCreateApplicationContext();

	XtDisplayInitialize(Xtcx, Xdpy, "FreeWRL", "FreeWRL_class", NULL, 0, &argc_out, argv_out);

	freewrlTopWidget = XtAppCreateShell("FreeWRL", "FreeWRL_class", applicationShellWidgetClass, Xdpy, initArgs, initArgc);

	if (!freewrlTopWidget) {
		ERROR_MSG("Can't initialize Motif\n");
		return FALSE;
	}

	/* Inform Motif that we have our visual and colormap already ... (before top level is realized) */
	XtVaSetValues(freewrlTopWidget,
		      XmNdepth, Xvi->depth,
		      XmNvisual, Xvi->visual,
		      XmNcolormap, colormap,
		      NULL);
	
	mainw = XmCreateMainWindow(freewrlTopWidget, getWindowTitle(), NULL, 0);
	if (!mainw)
		return FALSE;
	
	myXtManageChild(29, mainw);
	
	/* Create a menu bar. */
	fv_createMenuBar();
	
	/* Create a framed drawing area for OpenGL rendering. */
	fv_createDrawingFrame();
	
	/* Set up the application's window layout. */
	XtVaSetValues(mainw, 
		      XmNworkWindow, frame,
		      XmNcommandWindow,  NULL,
		      XmNmenuBar, menubar,
		      XmNmessageWindow, menumessagewindow,
		      NULL);
	
	
	XtRealizeWidget (freewrlTopWidget);

	/* FIXME: see fwBareWindow.c */
	/* Roberto Gerson */
	/* If -d is setted, so reparent the window */
	if (params->winToEmbedInto != INT_ID_UNDEFINED){
		printf("fwMotifWindow::Trying to reparent window: %ld, to new parent: %ld\n",
			XtWindow(freewrlTopWidget),
			params->winToEmbedInto);

		XReparentWindow(XtDisplay(freewrlTopWidget),
				XtWindow(freewrlTopWidget),
				(Window) params->winToEmbedInto, 0, 0);

		XMapWindow(XtDisplay(freewrlTopWidget), XtWindow(freewrlTopWidget));
	}

	XFlush(XtDisplay(freewrlTopWidget));

	MainWidgetRealized = XtIsRealized(freewrlTopWidget); /*TRUE;*/
	TRACE_MSG("fv_create_main_window: top widget realized: %s\n", BOOL_STR(MainWidgetRealized));
	
	/* let the user ask for this one We do it here, again, because on Ubuntu,
	 * this pops up on startup. Maybe because it has scrollbars, or other internal
	 * widgets? */
	XtUnmanageChild(consoleTextWidget);
	
	Xwin = XtWindow(freewrlTopWidget);
	GLwin = XtWindow(freewrlDrawArea);
	
	/* now, lets tell the OpenGL window what its dimensions are */
	
	XtVaGetValues(freewrlDrawArea, XmNwidth, &width, XmNheight, &height, NULL);
	/* printf("%s,%d fv_create_main_window %d, %d\n",__FILE__,__LINE__,width,height); */
	fv_setScreenDim(width,height);
	
	/* lets see when this goes iconic */
	XtAddEventHandler(freewrlTopWidget, StructureNotifyMask, FALSE, StateWatcher, NULL);
	/* all events for DrawArea should be passed to FreeWRL (MainLoop) control */
	XtAddEventHandler(freewrlDrawArea, event_mask, False, fv_DrawArea_events, NULL);

	return TRUE;
}

/************************************************************************

Callbacks to handle button presses, etc.

************************************************************************/

/* Label strings are "broken" on some Motifs. See:
 * http://www.faqs.org/faqs/motif-faq/part5/
 */
/* both of these fail on Ubuntu 6.06 */
/* diastring = XmStringCreateLtoR(ns,XmFONTLIST_DEFAULT_TAG); */
/*diastring = XmStringCreateLocalized(ns); */

XmString xec_NewString(char *s)
{
    XmString xms1;
    XmString xms2;
    XmString line;
    XmString separator;
    char     *p;
    char     *t = XtNewString(s);   /* Make a copy for strtok not to */
                                    /* damage the original string    */

    separator = XmStringSeparatorCreate();
    p         = strtok(t,"\n");
    xms1      = XmStringCreateLocalized(p);

    /* FIXME: ???? why NULL here */
    while ((p = strtok(NULL,"\n")))
    {
        line = XmStringCreateLocalized(p);
        xms2 = XmStringConcat(xms1,separator);
        XmStringFree(xms1);
        xms1 = XmStringConcat(xms2,line);
        XmStringFree(xms2);
        XmStringFree(line);
    }

    XmStringFree(separator);
    XtFree(t);
    return xms1;
}

/* Callbacks */
void fv_aboutFreeWRLpopUp (Widget w, XtPointer data, XtPointer callData)
{ 

    int ac;
    Arg args[10];
    const char *ver;
    char *msg, *rdr, *vendor;
    XmString diastring;
    ac = 0;

    ver = libFreeWRL_get_version();

    rdr = (char *)gglobal()->display.rdr_caps.renderer;
    vendor = (char *)gglobal()->display.rdr_caps.vendor;

    msg = malloc(strlen(ABOUT_FREEWRL) + strlen(ver) 
		 + strlen(rdr) + strlen(vendor));
    sprintf(msg, ABOUT_FREEWRL, ver, rdr, vendor);

    diastring = xec_NewString(msg);     
    XtSetArg(args[ac], XmNmessageString, diastring); ac++;
    XtSetValues(about_widget, args, ac);
    XmStringFree(diastring);
    FREE(msg);

    myXtManageChild(2,about_widget);
}

/* quit selected */
void fv_quitMenuBar (Widget w, XtPointer data, XtPointer callData)
{ 
    fwl_doQuit();
}

void fv_reloadFile (Widget w, XtPointer data, XtPointer callData)
{
	ConsoleMessage ("reloading %s", BrowserFullPath);
	/* FIXME: implement reload function */
}

void fv_ViewpointFirst (Widget w, XtPointer data, XtPointer callData) {
    fwl_First_ViewPoint();
}

void fv_ViewpointLast (Widget w, XtPointer data, XtPointer callData)
{
    fwl_Last_ViewPoint();
}

void fv_ViewpointNext (Widget w, XtPointer data, XtPointer callData)
{
    fwl_Next_ViewPoint();
}

void fv_ViewpointPrev (Widget w, XtPointer data, XtPointer callData)
{
    fwl_Prev_ViewPoint();
}

/* selecting default background colours */

void fv_BackColour(Widget w, XtPointer data, XtPointer callData)
{
	int color = (int) data;
	setDefaultBackground(color);
}

void fv_Tex128(Widget w, XtPointer data, XtPointer callData)
{
	/* does nothing right now */
}

void fv_Tex256(Widget w, XtPointer data, XtPointer callData)
{
	/* does nothing right now */
}

void fv_TexFull(Widget w, XtPointer data, XtPointer callData)
{
	/* does nothing right now */
}


/* do we want a message window displaying fps, viewpoint, etc? */
void fv_toggleMessagebar (Widget w, XtPointer data, XtPointer callData)
{
    msgWindowOnscreen = !msgWindowOnscreen; /* keep track of state */
    XmToggleButtonSetState (menumessageButton,msgWindowOnscreen,FALSE); /* display blip if on */
    if (msgWindowOnscreen) myXtManageChild (3,menumessagewindow); /* display (or not) message window */
    else XtUnmanageChild (menumessagewindow);
}

/* do we want a console window displaying errors, etc? */
void fv_toggleConsolebar (Widget w, XtPointer data, XtPointer callData)
{
    consWindowOnscreen = !consWindowOnscreen; /* keep track of state */
    XmToggleButtonSetState (consolemessageButton,consWindowOnscreen,FALSE); /* display blip if on */
    if (consWindowOnscreen) myXtManageChild (30,consoleTextWidget); /* display (or not) console window */
    else XtUnmanageChild (consoleTextWidget);
}

void fv_WalkMode (Widget w, XtPointer data, XtPointer callData)
{
    fwl_set_viewer_type (VIEWER_WALK);
}

void fv_ExamineMode (Widget w, XtPointer data, XtPointer callData)
{
    fwl_set_viewer_type (VIEWER_EXAMINE);
}

void fv_FlyMode (Widget w, XtPointer data, XtPointer callData)
{
    fwl_set_viewer_type (VIEWER_FLY);
}

void fv_Headlight (Widget w, XtPointer data, XtPointer callData)
{
    fwl_toggle_headlight();
}

void fv_Collision (Widget w, XtPointer data, XtPointer callData)
{
/*    fwl_setp_collision(!fwl_getp_collision());*/
    toggle_collision();
}

void fv_ViewpointStraighten (Widget w, XtPointer data, XtPointer callData)
{
    printf ("not yet implemented\n");
}

/* file selection dialog box, ok button pressed */
void fv_fileSelectPressed (Widget w, XtPointer data, XmFileSelectionBoxCallbackStruct *callData)
{
    char *newfile;

    /* get the filename */
    XmStringGetLtoR(callData->value, 
                    XmSTRING_DEFAULT_CHARSET, &newfile);

    if (!Anchor_ReplaceWorld(newfile)) {
	    /* error message */
    }
    XtUnmanageChild(w);
}

/* file selection dialog box cancel button - just get rid of widget */
void fv_unManageMe (Widget widget, XtPointer client_data, 
                 XmFileSelectionBoxCallbackStruct *selection)
{
    XtUnmanageChild(widget);
}

/* new file popup - user wants to load a new file */ 
void fv_newFilePopup(Widget cascade_button, char *text, XmPushButtonCallbackStruct *cbs)
{
    myXtManageChild(4,newFileWidget);
    XtPopup(XtParent(newFileWidget), XtGrabNone); 
}

#ifdef DOESNOTGETICONICSTATE
/* resize, configure events */
void fv_GLAreaexpose (Widget w, XtPointer data, XtPointer callData)
{
    XmDrawingAreaCallbackStruct *cd = (XmDrawingAreaCallbackStruct *) callData;
    switch (cd->reason) {
    case XmCR_EXPOSE: printf ("got expose event \n");
    default: printf ("not known event, %d\n",cd->reason);
    }
}
#endif

/* resize, configure events */
void fv_GLArearesize (Widget w, XtPointer data, XtPointer callData)
{
/*     XmDrawingAreaCallbackStruct *cd = (XmDrawingAreaCallbackStruct *) callData; */
    Dimension width, height;

    XtVaGetValues (w, XmNwidth, &width, XmNheight, &height, NULL);
    /* printf("%s,%d GLArearesize %d, %d\n",__FILE__,__LINE__,width,height); */
    fv_setScreenDim (width,height);
}

/* Mouse, keyboard input when focus is in OpenGL window. */
void fv_GLAreainput (Widget w, XtPointer data, XtPointer callData)
{
    XmDrawingAreaCallbackStruct *cd = (XmDrawingAreaCallbackStruct *) callData;

#ifdef XTDEBUG
    printEvent(*(cd->event));
#endif

    handle_Xevents(*(cd->event));
}


/* remove this button from this SelectionBox widget */
void fv_removeWidgetFromSelect (Widget parent, 
#if NeedWidePrototypes
                             unsigned int 
#else
                             unsigned char
#endif
                             button) {

    Widget tmp;

    tmp = XmSelectionBoxGetChild(parent, button);
    if (tmp == NULL) {
        printf ("hmmm - button does not exist\n");
    } else {
        XtUnmanageChild(tmp);
    }
}

/* start up the browser, and point it to www.crc.ca/FreeWRL */
void fv_freewrlHomePopup (Widget w, XtPointer data, XtPointer callData)
{ 
#if DJ_KEEP_COMPILER_WARNING
	#define MAXLINE 2000
#endif
	const char *browser;
	char *sysline;
	const char pattern[] = "%s http://www.crc.ca/FreeWRL &";

	browser = freewrl_get_browser_program();
	if (!browser) {
		browser = BROWSER;
	}
	sysline = MALLOC(char *, strlen(browser)+strlen(pattern));
	sprintf(sysline, pattern, browser);

	freewrlSystem(sysline);

	FREE(sysline);
}

#ifdef XTDEBUG
/* for debugging... */
printEvent (XEvent event)
{
    switch (event.type) {
    case KeyPress: printf ("KeyPress"); break;
    case KeyRelease: printf ("KeyRelease"); break;
    case ButtonPress: printf ("ButtonPress"); break;
    case ButtonRelease: printf ("ButtonRelease"); break;
    case MotionNotify: printf ("MotionNotify"); break;
    case EnterNotify: printf ("EnterNotify"); break;
    case LeaveNotify: printf ("LeaveNotify"); break;
    case FocusIn: printf ("FocusIn"); break;
    case FocusOut: printf ("FocusOut"); break;
    case KeymapNotify: printf ("KeymapNotify"); break;
    case Expose: printf ("Expose"); break;
    case GraphicsExpose: printf ("GraphicsExpose"); break;
    case NoExpose: printf ("NoExpose"); break;
    case VisibilityNotify: printf ("VisibilityNotify"); break;
    case CreateNotify: printf ("CreateNotify"); break;
    case DestroyNotify: printf ("DestroyNotify"); break;
    case UnmapNotify: printf ("UnmapNotify"); break;
    case MapNotify: printf ("MapNotify"); break;
    case MapRequest: printf ("MapRequest"); break;
    case ReparentNotify: printf ("ReparentNotify"); break;
    case ConfigureNotify: printf ("ConfigureNotify"); break;
    case ConfigureRequest: printf ("ConfigureRequest"); break;
    case GravityNotify: printf ("GravityNotify"); break;
    case ResizeRequest: printf ("ResizeRequest"); break;
    case CirculateNotify: printf ("CirculateNotify"); break;
    case CirculateRequest: printf ("CirculateRequest"); break;
    case PropertyNotify: printf ("PropertyNotify"); break;
    case SelectionClear: printf ("SelectionClear"); break;
    case SelectionRequest: printf ("SelectionRequest"); break;
    case SelectionNotify: printf ("SelectionNotify"); break;
    case ColormapNotify: printf ("ColormapNotify"); break;
    case ClientMessage: printf ("ClientMessage"); break;
    case MappingNotify: printf ("MappingNotify"); break;
    default :printf ("Event out of range - %d",event.type);
    }
    printf ("\n");
}
#endif

/* File pulldown menu */
void fv_createFilePulldown()
{
    Widget menupane, btn, cascade;

    XmString mask;
    int ac;
    Arg args[10];
           
    /* Create the FileSelectionDialog */     
    memset(args, 0, sizeof(args));
    ac = 0;
    mask  = XmStringCreateLocalized("*.wrl");
    XtSetArg(args[ac], XmNdirMask, mask); ac++;

    /* newFileWidget = XmCreateFileSelectionDialog(menubar, "select", args, 1); */
    newFileWidget = XmCreateFileSelectionDialog(mainw, "select", args, 1);        

    XtAddCallback(newFileWidget, XmNokCallback, (XtCallbackProc)fv_fileSelectPressed, NULL);
    XtAddCallback(newFileWidget, XmNcancelCallback, (XtCallbackProc)fv_unManageMe, NULL);
    /* delete buttons not wanted */
    fv_removeWidgetFromSelect(newFileWidget,XmDIALOG_HELP_BUTTON);
    XtUnmanageChild(newFileWidget);


    menupane = XmCreatePulldownMenu (menubar, "menupane", NULL, 0);
    btn = XmCreatePushButton (menupane, "Reload", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_reloadFile, NULL);
    myXtManageChild (5,btn);
    btn = XmCreatePushButton (menupane, "New...", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_newFilePopup, NULL);
    myXtManageChild (6,btn);

    btn = XmCreatePushButton (menupane, "Quit", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_quitMenuBar, NULL);
    myXtManageChild (7,btn);
    XtSetArg (args[0], XmNsubMenuId, menupane);
    cascade = XmCreateCascadeButton (menubar, "File", args, 1);
    myXtManageChild (8,cascade);
}

/* Navigate pulldown menu */
void fv_createNavigatePulldown()
{
    Widget cascade, btn, menupane;

    menupane = XmCreatePulldownMenu (menubar, "menupane", NULL, 0);
        
    /* Viewpoints */
    btn = XmCreatePushButton (menupane, "First Viewpoint", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_ViewpointFirst, NULL);
    myXtManageChild (30,btn);
    btn = XmCreatePushButton (menupane, "Next Viewpoint", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_ViewpointNext, NULL);
    myXtManageChild (9,btn);
    btn = XmCreatePushButton (menupane, "Prev Viewpoint", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_ViewpointPrev, NULL);
    myXtManageChild (10,btn);
    btn = XmCreatePushButton (menupane, "Last Viewpoint", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_ViewpointLast, NULL);
    myXtManageChild (31,btn);


    /* Navigation Mode Selection */
    myXtManageChild(11,XmCreateSeparator (menupane, "sep1", NULL, 0));

    walkButton = XtCreateManagedWidget("Walk Mode", xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback (walkButton, XmNvalueChangedCallback, (XtCallbackProc)fv_WalkMode, NULL);
    myXtManageChild (12,walkButton);

    examineButton = XtCreateManagedWidget("Examine Mode", xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback (examineButton, XmNvalueChangedCallback, (XtCallbackProc)fv_ExamineMode, NULL);
    myXtManageChild (13,examineButton);

    flyButton = XtCreateManagedWidget("Fly Mode", xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback (flyButton, XmNvalueChangedCallback, (XtCallbackProc)fv_FlyMode, NULL);
    myXtManageChild (14,flyButton);

    /* Headlight, Collision */
    myXtManageChild(15,XmCreateSeparator (menupane, "sep1", NULL, 0));

    headlightButton = XtCreateManagedWidget("Headlight",
                                            xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback(headlightButton, XmNvalueChangedCallback, 
                  (XtCallbackProc)fv_Headlight, NULL);
    myXtManageChild (16,headlightButton);

    collisionButton = XtCreateManagedWidget("Collision",
                                            xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback(collisionButton, XmNvalueChangedCallback, 
                  (XtCallbackProc)fv_Collision, NULL);
    myXtManageChild (17,collisionButton);
        
    /* Straighten */
    /* BUTTON NOT WORKING - so make insensitive */
    XtSetArg (buttonArgs[buttonArgc], XmNsensitive, FALSE); 
    myXtManageChild(18,XmCreateSeparator (menupane, "sep1", NULL, 0));
    btn = XmCreatePushButton (menupane, "Straighten", buttonArgs, buttonArgc+1); /* NOTE THE +1 here for sensitive */
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_ViewpointStraighten, NULL);
    myXtManageChild (19,btn);

    consolemessageButton = XtCreateManagedWidget("Console Display",
                                                 xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback(consolemessageButton, XmNvalueChangedCallback, 
                  (XtCallbackProc)fv_toggleConsolebar, NULL);
    myXtManageChild (21,consolemessageButton);
    menumessageButton = XtCreateManagedWidget("Message Display",
                                              xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback(menumessageButton, XmNvalueChangedCallback, 
                  (XtCallbackProc)fv_toggleMessagebar, NULL);
    myXtManageChild (20,menumessageButton);
        
    XtSetArg (args[0], XmNsubMenuId, menupane);
    cascade = XmCreateCascadeButton (menubar, "Navigate", args, 1);
    myXtManageChild (22,cascade);
}

/* Preferences pulldown menu */
void fv_createPreferencesPulldown()
{
    Widget cascade, menupane;
    int count;

    menupane = XmCreatePulldownMenu (menubar, "menupane", NULL, 0);

    /* texture size on loading */       
    myXtManageChild(11,XmCreateSeparator (menupane, "sep1", NULL, 0));

    tex128_button = XtCreateManagedWidget("128x128 Textures", xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback (tex128_button, XmNvalueChangedCallback, (XtCallbackProc)fv_Tex128, NULL);
    myXtManageChild (12,tex128_button);

    tex256_button = XtCreateManagedWidget("256x256 Textures", xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback (tex256_button, XmNvalueChangedCallback, (XtCallbackProc)fv_Tex256, NULL);
    myXtManageChild (13,tex256_button);

    texFull_button = XtCreateManagedWidget("Fullsize Textures", xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
    XtAddCallback (texFull_button, XmNvalueChangedCallback, (XtCallbackProc)fv_TexFull, NULL);
    myXtManageChild (14,texFull_button);

    /* default Background colour */     
    myXtManageChild(11,XmCreateSeparator (menupane, "sep1", NULL, 0));

    for (count = colourBlack; count <= colourWhite; count++ ){
        backgroundColourSelector[count] = 
            XtCreateManagedWidget(BackString[count], xmToggleButtonWidgetClass, menupane, buttonArgs, buttonArgc);
        XtAddCallback (backgroundColourSelector[count], XmNvalueChangedCallback, (XtCallbackProc)fv_BackColour, (XtPointer)count);
        myXtManageChild (40,backgroundColourSelector[count]);
    }
    XmToggleButtonSetState (backgroundColourSelector[colourBlack], TRUE, FALSE);

    XtSetArg (args[0], XmNsubMenuId, menupane);
    cascade = XmCreateCascadeButton (menubar, "Preferences", args, 1);
    myXtManageChild (22,cascade);
}

void fv_createHelpPulldown()
{
    Widget btn, menupane, cascade;
    int ac;
    Arg args[10];


    menupane = XmCreatePulldownMenu (menubar, "menupane", NULL, 0);

    /* Helpity stuff */
    ac = 0;
    /*
      sprintf (ns,ABOUT_FREEWRL,getLibVersion(),"","");
      diastring = xec_NewString(ns);

      XtSetArg(args[ac], XmNmessageString, diastring); ac++;
    */
    XtSetArg(args[ac], XmNmessageAlignment,XmALIGNMENT_CENTER); ac++;
    about_widget = XmCreateInformationDialog(menubar, "about", args, ac);        
    XtAddCallback(about_widget, XmNokCallback, (XtCallbackProc)fv_unManageMe, NULL);
    fv_removeWidgetFromSelect (about_widget, XmDIALOG_CANCEL_BUTTON);
    /*
      causes segfault on Core3 fv_removeWidgetFromSelect (about_widget, XmDIALOG_HELP_BUTTON);
    */


    btn = XmCreatePushButton (menupane, "About FreeWRL...", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_aboutFreeWRLpopUp, NULL);
    myXtManageChild (23,btn);
    btn = XmCreatePushButton (menupane, "FreeWRL Homepage...", NULL, 0);
    XtAddCallback (btn, XmNactivateCallback, (XtCallbackProc)fv_freewrlHomePopup, NULL);
    myXtManageChild (24,btn);

    XtSetArg (args[0], XmNsubMenuId, menupane);
    cascade = XmCreateCascadeButton (menubar, "Help", args, 1);
    myXtManageChild (25,cascade);
}

/**********************************/
void fv_createMenuBar(void)
{
    Arg menuArgs[10]; int menuArgc = 0;

    /* create the menu bar */
    memset(menuArgs, 0, sizeof(menuArgs));
    menuArgc = 0;
        
    /* the following XtSetArg is not required; it only "pretties" up the display
       in some circumstances. It came out in Motif 2.0, and is not always found */
#ifdef XmNscrolledWindowChildType
    XtSetArg(menuArgs[menuArgc], XmNscrolledWindowChildType, XmMENU_BAR); menuArgc++;
#endif

    menubar = XmCreateMenuBar (mainw, "menubar", menuArgs, menuArgc);
    myXtManageChild (26,menubar);

    menumessagewindow = 
        XtVaCreateWidget ("Message:", xmTextFieldWidgetClass, mainw,
                          XmNeditable, False,
                          XmNmaxLength, 200,
                          NULL);

    /* generic toggle button resources */
    XtSetArg(buttonArgs[buttonArgc], XmCVisibleWhenOff, TRUE); buttonArgc++;
    XtSetArg(buttonArgs[buttonArgc],XmNindicatorType,XmN_OF_MANY); buttonArgc++;

    if (!RUNNINGASPLUGIN) fv_createFilePulldown();
    fv_createNavigatePulldown();
    fv_createPreferencesPulldown();
    fv_createHelpPulldown();

}

/**********************************************************************************/
/*
  create a frame for FreeWRL, and for messages
*/
void fv_createDrawingFrame(void)
{
    /* frame holds everything here */
    frame = XtVaCreateManagedWidget("form", xmPanedWindowWidgetClass, mainw, NULL);
    consoleTextWidget = XtVaCreateManagedWidget ("console text widget", xmScrolledWindowWidgetClass, frame,
                                                 XmNtopAttachment, XmATTACH_FORM,
                                                 XmNleftAttachment, XmATTACH_FORM,
                                                 XmNrightAttachment, XmATTACH_FORM,
                                                 XmNworkWindow, consoleTextArea,
                                                 NULL);
    consoleTextArea = XtVaCreateManagedWidget ("console text area ", xmTextWidgetClass, consoleTextWidget,
                                               XmNrows, 5,
                                               XmNcolumns, 0,
                                               XmNeditable, False,
                                               XmNeditMode, XmMULTI_LINE_EDIT,
                                               NULL);

    /* create the FreeWRL OpenGL drawing area, and map it. */

#if 0 /* MB: do not create a glwDrawingArea but a simple widget
	 we have our own initialization of OpenGL ...
	 in the near future we could remove completely the GLwDrawA files...
      */
    freewrlDrawArea = XtVaCreateManagedWidget ("freewrlDrawArea", glwDrawingAreaWidgetClass,
                                               frame, "visualInfo", Xvi, 
                                               XmNtopAttachment, XmATTACH_WIDGET,
                                               XmNbottomAttachment, XmATTACH_FORM,
                                               XmNleftAttachment, XmATTACH_FORM,
                                               XmNrightAttachment, XmATTACH_FORM,
                                               NULL);
#endif

    freewrlDrawArea = XmCreateDrawingArea (frame, "drawing_a", NULL, 0);

#ifdef DOESNOTGETICONICSTATE
    XtAddCallback (freewrlDrawArea, XmNexposeCallback, fv_GLAreaexpose, NULL);
#endif

    XtAddCallback (freewrlDrawArea, XmNresizeCallback, fv_GLArearesize, NULL);

    myXtManageChild(27,freewrlDrawArea);

    /* let the user ask for this one */
    XtUnmanageChild(consoleTextWidget);
}

void setConsoleMessage (char *str)
{
    char *tptr;
    int nl;

    /* is the consoleTextWidget created yet?? */
    if (IS_DISPLAY_INITIALIZED != TRUE) {
	    ERROR_MSG("display not initialized: can't write ConsoleMessage: %s\n", str);
    } else {
        /* make sure console window is on screen */
        if (!consWindowOnscreen) {
            consWindowOnscreen = TRUE;
            myXtManageChild (1,consoleTextWidget); /* display console window */
            XmToggleButtonSetState (consolemessageButton,consWindowOnscreen,FALSE); /* display blip if on */
        }
                
        /* put the text here */
        nl = strlen(str);
        tptr = MALLOC (char *, nl+10);
        strcpy (tptr,str);
                        
        /* copy old string, if it exists */
        FREE_IF_NZ (consMsg);
        consMsg = tptr;
        consmsgChanged = TRUE;
    }
}



void frontendUpdateButtons()
{
    if (colbutChanged) {
        XmToggleButtonSetState (collisionButton,colbut,FALSE);
        colbutChanged = FALSE;
    }
    if (headbutChanged) {
        XmToggleButtonSetState (headlightButton,headbut,FALSE);
        headbutChanged = FALSE;
    }
    if (navbutChanged) {
        XmToggleButtonSetState (walkButton,wa,FALSE);
        XmToggleButtonSetState (flyButton,fl,FALSE);
        XmToggleButtonSetState (examineButton,ex,FALSE);
        navbutChanged = FALSE;
    }
    if (msgChanged) {
        XmTextSetString(menumessagewindow,fpsstr);
        msgChanged = FALSE;
    }
    if (consmsgChanged) {
        /* printf ("frontendUpateButtons, consmggchanged, posn %d oldstr %s consmsg %s\n",
           strlen(XmTextGetString(consoleTextArea)),
           XmTextGetString(consoleTextArea),
           consMsg);*/
        XmTextInsert (consoleTextArea, strlen(XmTextGetString(consoleTextArea)),consMsg);
        consmsgChanged = FALSE;
    }
}

void getMotifWindowedGLwin(Window *win)
{
    *win = XtWindow(freewrlDrawArea);
}

void setDefaultBackground(int colour)
{
    int count;

    if ((colour<colourBlack) || (colour > colourWhite)) return; /* an error... */

    for (count = colourBlack; count <= colourWhite; count++) {
        XmToggleButtonSetState (backgroundColourSelector[count], FALSE, FALSE);
    }
    XmToggleButtonSetState (backgroundColourSelector[colour], TRUE, FALSE);
    setglClearColor (&(backgroundColours[colour*3]));

    /* rather use fwl_set_glClearColor (float red , float green , float blue , float alpha); */

}


#endif /* IPHONE */
#endif /* KEEP_FV_INLIB */
