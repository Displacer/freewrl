/*******************************************************************************
 * $Id: npfreewrl.c,v 1.7 2002/08/13 18:15:09 ayla Exp $
 *
 * FreeWRL Netscape Plugin, Copyright (c) 2001 CRC Canada, based on
 *
 * Simple LiveConnect Sample Plugin
 * Copyright (c) 1996 Netscape Communications. All rights reserved.
 *
 * and...
 *
 * XSwallow , by Caolan.McNamara@ul.ie
 *
 ******************************************************************************/


#include <sys/types.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include "npapi.h"
#include "pluginUtils.h"


#define FWRL 0
#define NP   1


#define NPDEBUG
#define REPARENT_LOOPS 50
#define GIVEUP 4000

#ifndef _DEBUG
#    define _DEBUG 0
#endif

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/*******************************************************************************
 * Instance state information about the plugin.
 ******************************************************************************/

typedef struct _PluginInstance
{
	NPWindow*	fWindow;
	uint16		fMode;
	Window		window;
	Display		*display;
	uint32		x, y;
	uint32		width, height;
	Widget		netscapeWidget;
	XtIntervalId	swallowTimer;
	Window 		victim;
	int		fullsize;  /* window to swallow = win area */
	Widget 		resizeWatch;
	int		resizeEvent;
	int		childPID;
	char 		*fName;
	int		count; 	/* try to swallow count	*/
	int		fd[2];	/* socket file descriptors (via socketpair) */
	int		fwrlAlive;
	/* url request data (NPN_GetURLNotify, NPP_URLNotify) */
} PluginInstance;

typedef void (* Sigfunc) (int);

/*
 * Global variables:
 * 
 * A global copy of the plugin's socket descriptor is needed by
 * the signal handler.
 * 
 */
static int np_fd;
static int abortFlag;

#if _DEBUG
    static FILE *log;
#endif

void abortSwallowX(Widget, XtPointer , XEvent *);

/* Private plugin functions: */
static void resizeCB (Widget, PluginInstance *, XEvent *, Boolean *);
static void Redraw(Widget w, XtPointer closure, XEvent *event);
static void do_swallow (PluginInstance * );
static void swallow_check(PluginInstance *);
static int run_child (NPP, const char *, int, int, int[]);
static void signalHandler(int);
static int freewrlReceive(int);
static int init_socket(int, Boolean);

/* These are diagnostic tools. */
static void printXEvent(XEvent *);
static void printXError(const char *, unsigned int);

Sigfunc signal(int, Sigfunc func);

void resizeCB(Widget w, PluginInstance * data, XEvent * event, Boolean * cont) {
	Arg args[2];
	Widget temp;
#if _DEBUG
	unsigned int err;
#endif

    printXEvent(event);

	 /* This will be "netscapeEmbed", go to "drawingArea" */
    temp = data->netscapeWidget;
    while(strcmp(XtName(temp),"drawingArea")) {
		temp = XtParent(temp);
    }

	if (data->fullsize == FALSE) {
#if _DEBUG
		fprintf(log, "Function resizeCB with data->fullsize == FALSE:\n");
		err = XReparentWindow(data->display, data->victim, XtWindow (data->resizeWatch), 0, 0);
		printXError("XReparentWindow", err);
#else
		fprintf(stderr,"resizeCB, !!! fullsize is FALSE\n");
		XReparentWindow(data->display, data->victim, XtWindow (data->resizeWatch), 0, 0);
#endif
		XSync(data->display, FALSE);
	} else {
#if _DEBUG
		fprintf(log, "Function resizeCB with data->fullsize == TRUE:\n");
#endif

#if _DEBUG
		fprintf(log, "\tCalling XtSetArg for width, and again for height.\n");
#endif
		XtSetArg (args[0], XtNwidth, (XtArgVal) &(data->width));
		XtSetArg (args[1], XtNheight, (XtArgVal) &(data->height));
#if _DEBUG
		fprintf(log, "\tCalling XtGetValues.\n");
#endif
		XtGetValues(temp, args, 2);
#if _DEBUG
		fprintf(log, "\tCalling XResizeWindow with data->window, w=%lu, h=%lu.\n",
				data->width, data->height);
		err = XResizeWindow (data->display, data->window, data->width, data->height);
		printXError("XResizeWindow", err);

		fprintf(log, "\tCalling XResizeWindow with data->victim w=%lu, h=%lu.\n",
				data->width, data->height);
		err = XResizeWindow (data->display, data->victim, data->width, data->height);
		printXError("XResizeWindow", err);
#else
		XResizeWindow (data->display, data->window, data->width, data->height);
		XResizeWindow (data->display, data->victim, data->width, data->height);
#endif
	}
}

void do_swallow (PluginInstance * This) {
	 /*add a timer*/
#if _DEBUG
    fprintf(log, "Function do_swallow:\n\tCalling XtAppAddTimeOut.\n");
#endif
	This->swallowTimer = XtAppAddTimeOut(XtDisplayToApplicationContext
										 (This->display), 333, (XtTimerCallbackProc) swallow_check, 
										 (XtPointer)This);
}


void swallow_check (PluginInstance * This)
{
    Arg args[2];
    Widget temp;
    int i, k, l, j, FoundIt = FALSE;
    int width, height;
    char *windowname;
    unsigned int number_of_subkids=0, number_of_kids=0, number_of_subsubkids=0;
    Window root, parent, *children = NULL, *subchildren = NULL, *subsubchildren = NULL;

    Atom type_ret;
    int fmt_ret;
    unsigned long nitems_ret;
    unsigned long bytes_after_ret;
    Window *win = NULL;
    Atom _XA_WM_CLIENT_LEADER;
    char FreeWRLName[30];

#if _DEBUG
    unsigned int err;
    fprintf(log, "Function swallow_check:\n");
#endif

    This->swallowTimer = -1;
    sprintf (FreeWRLName,"fw%d",This->childPID);

    if ((This->count < GIVEUP) && (abortFlag == 0)) {
        This->count++;

		if (children != (Window *) NULL) {
#if _DEBUG
			err = XFree(children);
			printXError("XFree(children)", err);
#else
			XFree (children);
#endif
		}
    
		 /* find the number of children on the root window */
        if (0 != XQueryTree (This->display, RootWindowOfScreen (XtScreen 
																(This->netscapeWidget)), &root,
							 &parent, &children, &number_of_kids)) 
    
			 /* go through the children, and see if one matches our FreeWRL win */
			for (i = 0; i < number_of_kids; i++) {
				if (0 != XFetchName (This->display, children[i], &windowname)) {
					if (!strncmp (windowname, FreeWRLName, strlen (FreeWRLName))) {
						 /* Found it!!! */
#if _DEBUG
						fprintf(log,
								"\tFound FreeWRL among the children of the root window.\n");
#endif
						FoundIt = TRUE;
						This->victim = children[i];
					}
#if _DEBUG
					err = XFree(windowname);
					printXError("XFree(windowname)", err);
#else
					XFree(windowname);
#endif
				}
    
				 /* nope, go through the sub-children */
				if (FoundIt == FALSE) {
					if (subchildren != (Window *) NULL) {
#if _DEBUG
						err = XFree(subchildren);
						printXError("XFree(subchildren)", err);
#else
						XFree(subchildren);
#endif
					}

					if (0 != XQueryTree (This->display, children[i], &root, &parent, 
										 &subchildren, &number_of_subkids))
						for (k = 0; k < number_of_subkids; k++) {
							if (0 != XFetchName (This->display, subchildren[k], &windowname)) {
								if (!strncmp (windowname, FreeWRLName, strlen (FreeWRLName))) {
#if _DEBUG
									fprintf(log,
											"\tFound FreeWRL among the subchildren of the root window.\n");
#endif
									FoundIt = TRUE;
									This->victim = subchildren[k];
								}
#if _DEBUG
								err = XFree(windowname);
								printXError("XFree(windowname)", err);
#else
								XFree (windowname);
#endif
							}
							if (FoundIt == FALSE) {
								if (subsubchildren != (Window *) NULL) {
#if _DEBUG
									err = XFree(subchildren);
									printXError("XFree(subchildren)", err);
#else
									XFree (subchildren);
#endif
								}
								if (0 != XQueryTree (This->display, subchildren[k], &root,
													 &parent, &subsubchildren, &number_of_subsubkids))
									for (l = 0; l < number_of_subsubkids; l++) {
										if (0 != XFetchName (This->display, subsubchildren[l], &windowname)) {
											if (!strncmp (windowname, FreeWRLName, strlen (FreeWRLName))) {
#if _DEBUG
												fprintf(log,
														"\tFound FreeWRL among the subsubchildren of the root window.\n");
#endif
												FoundIt = TRUE;
												This->victim = subsubchildren[l];
											}
#if _DEBUG
											err = XFree(windowname);
											printXError("XFree(windowname)", err);
#else
											XFree (windowname);
#endif
										}
									}
							}
						}
				}
			}
		 /* still in the for loop... */
		if (FoundIt == TRUE) {
			 /*search up the current tree to add a resize event handler */
            temp = XtWindowToWidget (This->display, This->window);

			 /* tree is:
		
			 netscape-communicator
			 Navigator
			 form	-------- While loop stops on this one.
			 mainForm
			 viewParent
			 scrollerForm
			 pane
			 scroller
			 drawingArea
			 form
			 pane
			 scroller
			 drawingArea
			 form
			 pane
			 scroller
			 drawingArea
			 netscapeEmbed
			 */
		
            while (strcmp (XtName (temp), "form")) {
				temp = XtParent (temp);
				if (!(strcmp (XtName (temp), "scroller"))) {
					XtSetArg (args[0], XtNwidth, (XtArgVal) & width);
					XtSetArg (args[1], XtNheight, (XtArgVal) & height);
					XtGetValues (temp, args, 2);
					if ((width == This->width) && (height == This->height))
						This->fullsize = TRUE;
				}
				if (!(strcmp (XtName (XtParent (temp)), "drawingArea"))) {
					temp = XtParent (temp);
				}
            }

			 /* remember - temp now points to "form" - see above tree */
			 /* we don't need this to be printed out. JAS

			 if (This->fullsize!=TRUE) {
			 #if _DEBUG
			 fprintf (log, "\tSurprise!!! This is not fullsize!\n");
			 #else
			 fprintf (stderr, "Surprise!!! This is not fullsize!\n");
			 #endif
			 }
			 */


            This->resizeWatch = temp;
            This->resizeEvent = TRUE;

			 /* when netscape is resized, the "form" will be resized, and */
			 /* we'll get a notification via this event handler		 */
            XtAddEventHandler(This->resizeWatch, StructureNotifyMask, 
							  False, (XtEventHandler) resizeCB, (XtPointer) This);
#if _DEBUG
            err = XResizeWindow(This->display, This->victim, This->width, This->height);
			printXError("XResizeWindow", err);

            err = XSync(This->display, FALSE);
			printXError("XSync", err);
#else
			 /* make it the right size for us; redundant if fullsize */
            XResizeWindow(This->display, This->victim, This->width, This->height);

            XSync(This->display, FALSE);
#endif

			 /* still have to figure the following couple of lines out. */
            _XA_WM_CLIENT_LEADER = XInternAtom (This->display, "WM_CLIENT_LEADER", False);

#if _DEBUG
            err = XGetWindowProperty ( /* Get value from property	*/

									  This->display, 		/* Server connection		*/
									  This->victim, 		/* Window ID, NOT Widget ID	*/
									  _XA_WM_CLIENT_LEADER, 	/* Atom id'ing property name	*/
									  0,			/* Offset (32 bit units to read)*/
                                        
									  sizeof (Window), 	/* # of 32 bit units to read	*/
									  False, 			/* delete value after read?	*/
									  AnyPropertyType,	/* Requested type		*/
									  &type_ret, 		/* Atom iding type actually fnd	*/
									  &fmt_ret, 		/* Fmt of elements - 8,16,32	*/
									  &nitems_ret, 		/* Number of items returned	*/
									  &bytes_after_ret,	/* Bytes left in property	*/
									  (unsigned char **) &win);/* Returend property value	*/

			printXError("XGetWindowProperty", err);
#else
            if (XGetWindowProperty ( /* Get value from property		*/

									This->display, 		/* Server connection		*/
									This->victim, 		/* Window ID, NOT Widget ID	*/
									_XA_WM_CLIENT_LEADER, 	/* Atom id'ing property name	*/
									0,			/* Offset (32 bit units to read)*/
                                        
									sizeof (Window), 	/* # of 32 bit units to read	*/
									False, 			/* delete value after read?	*/
									AnyPropertyType,	/* Requested type		*/
									&type_ret, 		/* Atom iding type actually fnd	*/
									&fmt_ret, 		/* Fmt of elements - 8,16,32	*/
									&nitems_ret, 		/* Number of items returned	*/
									&bytes_after_ret,	/* Bytes left in property	*/
									(unsigned char **) &win)/* Returend property value	*/
				== Success) 
#endif
				if (nitems_ret > 0)
					fprintf (stderr,
							 "FreeWRL Plugin: send in bug- WM_CLIENT_LEADER = %ld\n",
							 nitems_ret);
		
            if (win != (Window *) NULL) {
#if _DEBUG
                err = XFree(win);
				printXError("XFree(win)", err);
#else
                XFree(win);
#endif
			}
    
#if _DEBUG
            err = XSync(This->display, FALSE);
			printXError("XSync", err);

            err = XWithdrawWindow(This->display, This->victim, XScreenNumberOfScreen 
								  (XtScreen (This->netscapeWidget)));
			printXError("XWithdrawWindow", err);

            err = XSync(This->display, FALSE);
			printXError("XSync", err);

            err = XMapWindow(This->display, This->window);
			printXError("XMapWindow", err);

            err = XResizeWindow(This->display, This->window, This->width, This->height);
			printXError("XResizeWindow", err);

            err = XSync(This->display, FALSE);
			printXError("XSync", err);
#else
            XSync(This->display, FALSE);

            XWithdrawWindow(This->display, This->victim, XScreenNumberOfScreen 
							(XtScreen (This->netscapeWidget)));

            XSync(This->display, FALSE);

            XMapWindow(This->display, This->window);

            XResizeWindow(This->display, This->window, This->width, This->height);

            XSync(This->display, FALSE);
#endif

			 /* huh? One should do this... */
            for (j = 0; j < REPARENT_LOOPS; j++) {
				 /* more bloody serious dodginess */
#if _DEBUG
				err = XReparentWindow (This->display, This->victim, This->window, 0, 0);
				printXError("XReparentWindow", err);

				err = XSync (This->display, FALSE);
				printXError("XSync", err);
#else
				XReparentWindow (This->display, This->victim, This->window, 0, 0);
				XSync (This->display, FALSE);
#endif
            }
    
#if _DEBUG
            err = XMapWindow (This->display, This->victim);
			printXError("XMapWindow", err);

            err = XSync (This->display, FALSE);
			printXError("XSync", err);
#else
            XMapWindow (This->display, This->victim);

            XSync (This->display, FALSE);
 
#endif
            if (children != (Window *) NULL) {
#if _DEBUG
				err = XFree(children);
				printXError("XFree(children)", err);
#else
				XFree(children);
#endif
			}

            if (subchildren != (Window *) NULL) {
#if _DEBUG
				err = XFree(subchildren);
				printXError("XFree(subchildren)", err);
#else
				XFree(subchildren);
#endif
			}

            if (subsubchildren != (Window *) NULL) {
#if _DEBUG
				err = XFree(subsubchildren);
				printXError("XFree(subsubchildren)", err);
#else
				XFree(subsubchildren);
#endif
			}
		} else {
            This->swallowTimer = XtAppAddTimeOut (XtDisplayToApplicationContext 
												  (This->display), 333, (XtTimerCallbackProc) swallow_check, 
												  (XtPointer) This);
		}
    } else {
		 /* can't run */
#if _DEBUG
		fprintf (log, "\tFreeWRL invocation can not be found\n");
#else
		fprintf (stderr, "FreeWRL invocation can not be found\n");
#endif
    }
}

int run_child (NPP instance, const char *filename, int width, int height, int fd[]) {
    int  childPID;
    char geom[20];
    char fName[256];
    char childname[30];
    char childFd[256];
    char eaiAddress[20];
    char instanceStr[256];
    char *paramline[15]; /* parameter line */

#if _DEBUG
    fprintf(log, "Function run_child:\n");
#endif

    childPID = fork ();
    if (childPID == -1) {
#if _DEBUG
	fprintf(log, "\tFreeWRL: Fork for plugin failed: %s\n", strerror (errno));
#else
	fprintf(stderr, "\tFreeWRL: Fork for plugin failed: %s\n", strerror (errno));
#endif
	childPID = 0;
    } else if (childPID == 0) {
	pid_t mine = getpid();
	if (setpgid(mine,mine) < 0)
#if _DEBUG
	    fprintf(log, "\tFreeWRL child group set failed\n");
#else
	    fprintf(stderr, "\tFreeWRL child group set failed\n");
#endif

	else {
	    if (close(fd[NP]) < 0) /* close unused file desc. */
	    {
		perror("Call to close in child in run_child failed");
		return(NPERR_GENERIC_ERROR);
	    }

	    /*
	     * Invoke FreeWRL:
	     * freewrl cachefile -geom wwxhh
	     *                   -best
	     *                   -netscape PID
	     *                   -fd socketfd
	     *                   -instance pluginInstance (NPP)
	     */
	    paramline[0] = "nice";
	    paramline[1] = "freewrl";
	    paramline[2] = fName;
	    paramline[3] = "-geom";
	    paramline[4] = geom;
	    paramline[5] = "-best";
	    paramline[6] = "-plugin";
	    paramline[7] = childname;

	    /*
	     * Hard-code request for EAI connection
	     * until a better way to determine if EAI is
	     * needed can be found.
	     */
	    paramline[8] = "-eai";
	    paramline[9] = eaiAddress;

	    paramline[10] = "-fd";
	    paramline[11] = childFd;
	    paramline[12] = "-instance";
	    paramline[13] = instanceStr;
	    paramline[14] = NULL;

	    sprintf(fName,"%s",filename);
	    sprintf(geom,"%dx%d",width, height);

	    /* EAI runs locally and port 2000 is reserved. */
	    sprintf(eaiAddress, "localhost:2000");

	    sprintf(childname,"fw%d",mine);
	    sprintf(childFd, "%d", fd[FWRL]);
	    sprintf(instanceStr, "%u", (uint) instance);

	    execvp(paramline[0], (char* const *) paramline);
	}
#if _DEBUG
	fprintf(log, "\tFreeWRL Plugin: Couldn\'t run FreeWRL.\n");
#else
	fprintf(stderr, "\tFreeWRL Plugin: Couldn\'t run FreeWRL.\n");
#endif
    } else {
	if (close(fd[FWRL]) < 0) /* close unused file desc. */
	{
	    perror("Call to close in parent in run_child failed");
	    return(NPERR_GENERIC_ERROR);
	}
	return(childPID);
    }
    return(NPERR_NO_ERROR);
}

char*
NPP_GetMIMEDescription(void)
{
	return("x-world/x-vrml:wrl:FreeWRL VRML Browser;model/vrml:wrl:FreeWRL VRML Browser");
}

#define PLUGIN_NAME		"FreeWRL VRML Browser"
#define PLUGIN_DESCRIPTION	"Implements VRML for Netscape"

NPError
NPP_GetValue(void *future, NPPVariable variable, void *value)
{
    NPError err = NPERR_NO_ERROR;
    if (variable == NPPVpluginNameString)
		*((char **)value) = PLUGIN_NAME;
    else if (variable == NPPVpluginDescriptionString)
		*((char **)value) = PLUGIN_DESCRIPTION;
    else
		err = NPERR_GENERIC_ERROR;

    return err;
}

/*******************************************************************************
 * General Plug-in Calls
 ******************************************************************************/

/*
 * NPP_Initialize is called when your DLL is being loaded to do any
 * DLL-specific initialization.
 */
NPError
NPP_Initialize(void)
{
    return NPERR_NO_ERROR;
}

/*
 * NPP_Shutdown is called when your DLL is being unloaded to do any
 * DLL-specific shut-down. You should be a good citizen and declare that
 * you're not using your Java class any more. This allows java to unload
 * it, freeing up memory.
 */
void
NPP_Shutdown(void)
{
    return;
}

/*
 * NPP_New is called when your plugin is instantiated (i.e. when an EMBED
 * tag appears on a page).
 */
NPError 
NPP_New(NPMIMEType pluginType,
	NPP instance,
	uint16 mode,
	int16 argc,
	char* argn[],
	char* argv[],
	NPSavedData* saved)
{
    PluginInstance* This;
    NPError err = NPERR_NO_ERROR;

    if (instance == NULL)
	return NPERR_INVALID_INSTANCE_ERROR;
		
    instance->pdata = NPN_MemAlloc(sizeof(PluginInstance));
	
    This = (PluginInstance*) instance->pdata;

    if (This == NULL)
	return NPERR_OUT_OF_MEMORY_ERROR;

    This->fWindow = NULL;
    This->fMode = mode;
    This->window = 0;
    This->netscapeWidget = NULL;
    This->swallowTimer = -1;
    This->victim = 0;
    This->fullsize = FALSE;
    This->resizeEvent = FALSE;
    This->childPID = -1;
    This->fName = NULL;
    This->count = 0;
    This->fwrlAlive = FALSE;

    /* For debugging puposes: */
#if _DEBUG
    log = fopen("np_log", "w+");
    fprintf(log,
      "Function NPP_New:\n\tStarting plugin log for instance %u in process %d!\n",
	(uint) instance, getpid());
#endif

    /*
     * Assume plugin and FreeWRL child process run on the same machine.
     * For this reason, datagram (UDP) sockets are reasonably safe.
     */
    if (socketpair(AF_LOCAL, SOCK_DGRAM, 0, This->fd) < 0)
    {
	perror("Call to socketpair failed");
	return(NPERR_GENERIC_ERROR);
    }

#if _DEBUG
    fprintf(log, "\tSocketpair: %d(FWRL), %d(NP).\n", This->fd[FWRL], This->fd[NP]);
#endif
    np_fd = This->fd[NP];

    if (signal(SIGIO, signalHandler) == SIG_ERR) {
	return(NPERR_GENERIC_ERROR);
    }

    if (signal(SIGBUS, signalHandler) == SIG_ERR) {
	return(NPERR_GENERIC_ERROR);
    }

    /* Prepare sockets ... */
    if ( (err = init_socket(This->fd[FWRL], FALSE)) != NPERR_NO_ERROR) {
	return(err);
    }

    if ( (err = init_socket(This->fd[NP], TRUE)) != NPERR_NO_ERROR) {
	return(err);
    }

    return(err);
}


NPError
NPP_Destroy (NPP instance, NPSavedData ** save) {
    PluginInstance *This;

  if (instance == NULL)
    return(NPERR_INVALID_INSTANCE_ERROR);

  This = (PluginInstance *) instance->pdata;

#if _DEBUG
  fprintf(log, "Function NPP_Destroy:\n");
#endif

  if (This != NULL) {
    if (This->swallowTimer!= -1 ) 
      XtRemoveTimeOut(This->swallowTimer);   

    if (This->resizeEvent) {
      XtRemoveEventHandler (This->resizeWatch, StructureNotifyMask, False, 
		(XtEventHandler) resizeCB, (XtPointer) This);
    }

    /*kill child*/
    if (This->childPID != -1) {
      kill(This->childPID*-1, SIGQUIT);
    }

    if (This->fName != NULL) {
      free(This->fName);
    }

    if (This->fwrlAlive) {
	This->fwrlAlive = FALSE;
    }

    close(This->fd[NP]);
#if _DEBUG
    fprintf(log, "\nClosing plugin log.\n");
    fclose(log);
#endif

    NPN_MemFree (instance->pdata);
    instance->pdata = NULL;
  }
  return(NPERR_NO_ERROR);
}


NPError
NPP_SetWindow (NPP instance, NPWindow * window) {
  static int t;
  PluginInstance *This;

  if (instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  if (window == NULL)
    return NPERR_NO_ERROR;

  This = (PluginInstance *) instance->pdata;

  if (t == 0)
    This->window = (Window) window->window;

#if _DEBUG
    fprintf(log, "Function NPP_SetWindow with w=%lu, h=%lu:\n",
                                          window->width, window->height);
#endif

  This->x = window->x;
  This->y = window->y;
  This->width = window->width;
  This->height = window->height;
  This->display = ((NPSetWindowCallbackStruct *) window->ws_info)->display;

  /* the app wasn't run yet */
  if (This->window != (Window) window->window)
    fprintf (stderr, "FreeWRL Plugin: this should not be happening\n");
  else {

    /* initialize */
    This->window = (Window) window->window;
    This->netscapeWidget = XtWindowToWidget (This->display, This->window);

    /* add an event handler for the "starting freewrl" window, on exposure */
    XtAddEventHandler(This->netscapeWidget, ExposureMask, FALSE, 
		(XtEventHandler) Redraw, This);

    /* this one is the click to abort 
    XtAddEventHandler(This->netscapeWidget, ButtonPress, FALSE, 
		(XtEventHandler) abortSwallowX, This); */

    /* put a simple "starting FreeWRL" on the screen */
#if _DEBUG
    fprintf(log, "\tCalling Redraw with NULL event.\n");
#endif
    Redraw(This->netscapeWidget, (XtPointer) This, NULL);
  }
  return NPERR_NO_ERROR;
}


NPError 
NPP_NewStream(NPP instance,
	      NPMIMEType type,
	      NPStream *stream, 
	      NPBool seekable,
	      uint16 *stype)
{
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

#if _DEBUG
	fprintf(log, "Function NPP_NewStream:\n");
#endif

	if (stream->url == NULL) {
#if _DEBUG
	    fprintf(log, "\tError - stream url is null!\n");
#endif
	    return(NPERR_NO_DATA);
	}

	/* Lets tell netscape to save this to a file. */
	*stype = NP_ASFILEONLY;
	seekable = FALSE;

	return(NPERR_NO_ERROR);
}


/* PLUGIN DEVELOPERS:
 *	These next 2 functions are directly relevant in a plug-in which
 *	handles the data in a streaming manner. If you want zero bytes
 *	because no buffer space is YET available, return 0. As long as
 *	the stream has not been written to the plugin, Navigator will
 *	continue trying to send bytes.  If the plugin doesn't want them,
 *	just return some large number from NPP_WriteReady(), and
 *	ignore them in NPP_Write().  For a NP_ASFILE stream, they are
 *	still called but can safely be ignored using this strategy.
 */

int32 STREAMBUFSIZE = 0X0FFFFFFF; /*
                                   * If we are reading from a file in NP_ASFILE
                                   * mode so we can take any size stream in our
				   * write call (since we ignore it)
				   */

int32 
NPP_WriteReady(NPP instance, NPStream *stream)
{
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	/* Number of bytes ready to accept in NPP_Write() */
	return STREAMBUFSIZE;
}


int32 
NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{
	if (instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;
	return len; /* The number of bytes accepted. */
}


NPError 
NPP_DestroyStream(NPP instance, NPStream *stream, NPError reason)
{
	if (instance == NULL) return NPERR_INVALID_INSTANCE_ERROR;

#if _DEBUG
	fprintf(log, "Function NPP_DestroyStream with NPError %d:\n", reason);
#endif

	return NPERR_NO_ERROR;
}


void
NPP_StreamAsFile (NPP instance, NPStream * stream, const char *fname)
{
    size_t bytes = 0;
    PluginInstance *This = NULL;

    if (instance == NULL) {
	fprintf(stderr, "NPP instance NULL in NPP_StreamAsFile.\n");
	return;
    }

    This = (PluginInstance*) instance->pdata;

#if _DEBUG
    fprintf(log, "Function NPP_StreamAsFile:\n");
#endif

    /*
     * Ready to roll...
     * Wait until we have a window before we do the bizz.
     */

    abortFlag=0;

    if (!This->fwrlAlive) {
	if (This->netscapeWidget != NULL) {
	    This->childPID = run_child (instance, fname,
			    This->width,This->height, This->fd);
	    if (This->childPID == -1)
#if _DEBUG
		fprintf(log,"\tError: attempt to run FreeWRL failed.\n");
#else
		fprintf(stderr,"Attempt to run FreeWRL failed.\n");
#endif

	    else {
		setpgid(This->childPID,This->childPID);
		do_swallow (This); /* swallowTimer will be set away from -1*/
		This->fwrlAlive = TRUE; /* freewrl is now loaded */
	    }
	} else {
	    This->swallowTimer = -2; /*inform setwindow to run it instead*/
	    This->fName = (char *) malloc((strlen(fname) +1) *sizeof(char *));
	    strcpy(This->fName,fname);
	}
    } else {
	if (fname == NULL) {
#if _DEBUG
	    fprintf(log, "\tError: file could not be retrieved!\n");
#endif
	    /* Try sending an empty string! */
	    if (write(This->fd[NP], "", 1) < 0) {
		perror("Call to write failed"); 
	    }
	} else {
	    bytes = (strlen(fname) + 1) * sizeof(const char *);
#if _DEBUG
	    fprintf(log, "\tWriting %s (%u bytes) to socket %d.\n",
						fname, bytes, This->fd[NP]);
#endif
	    if (write(This->fd[NP], fname, bytes) < 0) {
		perror("Call to write failed"); 
	    }
	}
    }
}

void 
NPP_Print(NPP instance, NPPrint* printInfo)
{
	/* not used */
	    return;

}


/*******************************************************************************
 * NPP_URLNotify:
 * Notifies the instance of the completion of a URL request. 
 * 
 * NPP_URLNotify is called when Netscape completes a NPN_GetURLNotify or
 * NPN_PostURLNotify request, to inform the plug-in that the request,
 * identified by url, has completed for the reason specified by reason. The most
 * common reason code is NPRES_DONE, indicating simply that the request
 * completed normally. Other possible reason codes are NPRES_USER_BREAK,
 * indicating that the request was halted due to a user action (for example,
 * clicking the "Stop" button), and NPRES_NETWORK_ERR, indicating that the
 * request could not be completed (for example, because the URL could not be
 * found). The complete list of reason codes is found in npapi.h. 
 * 
 * The parameter notifyData is the same plug-in-private value passed as an
 * argument to the corresponding NPN_GetURLNotify or NPN_PostURLNotify
 * call, and can be used by your plug-in to uniquely identify the request. 
 ******************************************************************************/

void
NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
}

void
Redraw(Widget w, XtPointer closure, XEvent *event)
{
	PluginInstance* This = (PluginInstance*)closure;
	GC gc;
	XGCValues gcv;
#if _DEBUG
	unsigned int err;
#endif
	const char* text = "Starting FreeWRL";

#if _DEBUG
	fprintf(log, "Function Redraw - uses XtVaGetValues.\n");
	printXEvent(event);
#endif

	XtVaGetValues(w, XtNbackground, &gcv.background,
				  XtNforeground, &gcv.foreground, NULL);
#if _DEBUG
	fprintf(log, "\tCalling XCreateGC.\n");
#endif
	gc = XCreateGC(This->display, This->window, 
				   GCForeground|GCBackground, &gcv);
#if _DEBUG
/* 	err = XDrawRectangle(This->display, This->window, gc,  */
/* 				   0, 0, This->width-1, This->height-1); */
	
	err = XDrawRectangle(This->display, This->window, gc,
						 This->x, This->y, This->width-1, This->height-1);
	printXError("XDrawRectangle", err);

	err = XDrawString(This->display, This->window, gc, 
				This->width/2 - 100, This->height/2,
				text, strlen(text));
	printXError("XDrawString", err);
#else
	XDrawRectangle(This->display, This->window, gc, 
				   0, 0, This->width-1, This->height-1);

	XDrawString(This->display, This->window, gc, 
				This->width/2 - 100, This->height/2,
				text, strlen(text));
#endif
/* XFreeGC(Display *display, GC gc) */
}

Sigfunc
signal(int signo, Sigfunc func)
{
    struct sigaction action, old_action;

    action.sa_handler = func;
    /*
     * Initialize action's signal set as empty set
     * (see man page sigsetops(3)).
     */
    sigemptyset(&action.sa_mask);

    action.sa_flags = 0; /* Is this a good idea??? */

    /* Add option flags for handling signal: */
    action.sa_flags |= SA_NOCLDSTOP;
#ifdef SA_NOCLDWAIT
    action.sa_flags |= SA_NOCLDWAIT;
#endif

    if (sigaction(signo, &action, &old_action) < 0) {
	perror("Call to sigaction failed");
	return(SIG_ERR);
    }
    /* Return the old action for the signal or SIG_ERR. */
     return(old_action.sa_handler);
}

void
signalHandler(int signo)
{
#if _DEBUG
    fprintf(log, "Signal %d caught from signalHandler!\n", signo);
#endif
    if (signo == SIGIO) {
	freewrlReceive(np_fd);
    }
    /* Should handle all except the uncatchable ones. */
    else {
#if _DEBUG
	fprintf(log, "\nClosing plugin log.\n");
	fclose(log);
#endif
	exit(1);
    }
}

int freewrlReceive(int fd)
{
    sigset_t newmask, oldmask;

    urlRequest request;
    size_t request_size = 0;
	int rv = 0;

    bzero(request.url, FILENAME_MAX);
    request.instance = 0;
    request.notifyCode = 0; /* not currently used */

    request_size = sizeof(request);

    /*
     * The signal handling code is based on the work of
     * W. Richard Stevens from Unix Network Programming,
     * Networking APIs: Sockets and XTI.
     */

    /* Init. the signal sets as empty sets. */
    if (sigemptyset(&newmask) < 0) {
	perror("Call to sigemptyset with arg newmask failed");
	return(NPERR_GENERIC_ERROR);
    }
    
    if (sigemptyset(&oldmask) < 0) {
	perror("Call to sigemptyset with arg oldmask failed");
	return(NPERR_GENERIC_ERROR);
    }

    if (sigaddset(&newmask, SIGIO) < 0) {
	perror("Call to sigaddset failed");
	return(NPERR_GENERIC_ERROR);
    }

    /* Code to block SIGIO while saving the old signal set. */
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
	perror("Call to sigprocmask failed");
	return(NPERR_GENERIC_ERROR);
    }
    
    if ( (read(fd, (urlRequest *) &request, request_size)) < 0) {
	/* If blocked or interrupted, be silent. */
    	if (errno != EINTR && errno != EAGAIN) {
	    perror("Call to read failed");
    	}
	return(NPERR_GENERIC_ERROR);
    }
    else {
	if ((rv = NPN_GetURL(request.instance, request.url, NULL)) != NPERR_NO_ERROR) {
#if _DEBUG
		fprintf(log, "Call to NPN_GetURL failed with error %d.\n", rv);
#else
		fprintf(stderr, "Call to NPN_GetURL failed with error %d.\n", rv);
#endif
	}
    }

    /* Restore old signal set, which unblocks SIGIO. */
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
	perror("Call to sigprocmask failed");
	return(NPERR_GENERIC_ERROR);
    }

    return(NPERR_NO_ERROR);
}

int
init_socket(int fd, Boolean nonblock)
{
    int io_flags;

    if (fcntl(fd, F_SETOWN, getpid()) < 0) {
	perror("Call to fcntl with command F_SETOWN failed");
	return(NPERR_GENERIC_ERROR);
    }

    if ( (io_flags = fcntl(fd, F_GETFL, 0)) < 0 ) {
	perror("Call to fcntl with command F_GETFL failed");
	return(NPERR_GENERIC_ERROR);
    }

    /*
     * O_ASYNC is specific to BSD and Linux.
     * Use ioctl with FIOASYNC for others.
     */
#ifndef __sgi
    io_flags |= O_ASYNC;
#endif

    if (nonblock) { io_flags |= O_NONBLOCK; }

    if ( (io_flags = fcntl(fd, F_SETFL, io_flags)) < 0 ) {
	perror("Call to fcntl with command F_SETFL failed");
	return(NPERR_GENERIC_ERROR);
    }
    return(NPERR_NO_ERROR);
}


void printXError(const char *func, unsigned int err)
{
#if _DEBUG
    char *result;
    FILE* stream;
    if (log) { stream = log; }
    else { stream = stderr; }
    /* from /usr/X11R6/include/X11/X.h */
    switch(err) {
	case Success:
	    result = "Success";
	    break;
	case BadRequest:
	    result = "BadRequest";
	    break;
	case BadValue:
	    result = "BadValue";
	    break;
	case BadWindow:
	    result = "BadWindow";
	    break;
	case BadPixmap:
	    result = "BadPixmap";
	    break;
	case BadAtom:
	    result = "BadAtom";
	    break;
	case BadCursor:
	    result = "BadCursor";
	    break;
	case BadFont:
	    result = "BadFont";
	    break;
	case BadMatch:
	    result = "BadMatch";
	    break;
	case BadDrawable:
	    result = "BadDrawable";
	    break;
	case BadAccess:
	    result = "BadAccess";
	    break;
	case BadAlloc:
	    result = "BadAlloc";
	    break;
	case BadColor:
	    result = "BadColor";
	    break;
	case BadGC:
	    result = "BadGC";
	    break;
	case BadIDChoice:
	    result = "BadIDChoice";
	    break;
	case BadName:
	    result = "BadName";
	    break;
	case BadLength:
	    result = "BadLength";
	    break;
	case BadImplementation:
	    result = "BadImplementation";
	    break;
	case FirstExtensionError:
	    result = "FirstExtensionError";
	    break;
	case LastExtensionError:
	    result = "LastExtensionError";
	    break;
	default:
	    result = "unknown value";
	    break;
    }
    fprintf(stream, "\t%s returned with %s (%u).\n", func, result, err);
#else
    return;
#endif
}

void printXEvent(XEvent *event)
{
#if _DEBUG
    char *result;
    FILE* stream;
    if (log) { stream = log; }
    else { stream = stderr; }

    if (event) {
	/* from /usr/X11R6/include/X11/X.h */
	switch(event->type) {
	    case KeyPress:
		result = "KeyPress";
		break;
	    case KeyRelease:
		result = "KeyRelease";
		break;
	    case ButtonPress:
		result = "ButtonPress";
		break;
	    case ButtonRelease:
		result = "ButtonRelease";
		break;
	    case MotionNotify:
		result = "MotionNotify";
		break;
	    case EnterNotify:
		result = "EnterNotify";
		break;
	    case LeaveNotify:
		result = "LeaveNotify";
		break;
	    case FocusIn:
		result = "FocusIn";
		break;
	    case FocusOut:
		result = "FocusOut";
		break;
	    case KeymapNotify:
		result = "KeymapNotify";
		break;
	    case Expose:
		result = "Expose";
		break;
	    case GraphicsExpose:
		result = "GraphicsExpose";
		break;
	    case NoExpose:
		result = "NoExpose";
		break;
	    case VisibilityNotify:
		result = "VisibilityNotify";
		break;
	    case CreateNotify:
		result = "CreateNotify";
		break;
	    case DestroyNotify:
		result = "DestroyNotify";
		break;
	    case UnmapNotify:
		result = "UnmapNotify";
		break;
	    case MapNotify:
		result = "MapNotify";
		break;
	    case MapRequest:
		result = "MapRequest";
		break;
	    case ReparentNotify:
		result = "ReparentNotify";
		break;
	    case ConfigureNotify:
		result = "ConfigureNotify";
		break;
	    case ConfigureRequest:
		result = "ConfigureRequest";
		break;
	    case GravityNotify:
		result = "GravityNotify";
		break;
	    case ResizeRequest:
		result = "ResizeRequest";
		break;
	    case CirculateNotify:
		result = "CirculateNotify";
		break;
	    case CirculateRequest:
		result = "CirculateRequest";
		break;
	    case PropertyNotify:
		result = "PropertyNotify";
		break;
	    case SelectionClear:
		result = "SelectionClear";
		break;
	    case SelectionRequest:
		result = "SelectionRequest";
		break;
	    case SelectionNotify:
		result = "SelectionNotify";
		break;
	    case ColormapNotify:
		result = "ColormapNotify";
		break;
	    case ClientMessage:
		result = "ClientMessage";
		break;
	    case MappingNotify:
		result = "MappingNotify";
		break;
	    case LASTEvent:
		result = "LASTEvent";
		break;
	    default:
		result = "unknown event";
		break;
	}
	fprintf(stream, "\tXEvent type is %s.\n", result);
    } else { fprintf(stream, "\tXEvent is NULL.\n"); }
#else
    return;
#endif
}
