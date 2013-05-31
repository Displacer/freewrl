/*
  $Id: fwWindow32.c,v 1.48 2013/05/31 21:26:24 dug9 Exp $

  FreeWRL support library.
  FreeWRL main window : win32 code.

*/
/* #define WIN32_LEAN_AND_MEAN 1*/



#include <config.h>

#if !(defined(IPHONE) || defined(_ANDROID) || defined(AQUA))

#include <system.h>
#include <display.h>
#include <main/headers.h>
#include <windows.h>
#include <shlwapi.h>

#include <internal.h>

#include <libFreeWRL.h>
#include <float.h>
#include "common.h"



HWND  ghWnd;   /* on a hunch I made these static so they are once per program */
HDC   ghDC; 
HGLRC ghRC; 

HWND fw_window32_hwnd()
{
	return ghWnd;
}
void fwl_do_keyPress(const char kp, int type);

/* from Blender GHOST_SystemWin32.cpp: Key code values not found in winuser.h */
#ifndef VK_MINUS
#define VK_MINUS 0xBD
#endif // VK_MINUS
#ifndef VK_SEMICOLON
#define VK_SEMICOLON 0xBA
#endif // VK_SEMICOLON
#ifndef VK_PERIOD
#define VK_PERIOD 0xBE
#endif // VK_PERIOD
#ifndef VK_COMMA
#define VK_COMMA 0xBC
#endif // VK_COMMA
#ifndef VK_QUOTE
#define VK_QUOTE 0xDE
#endif // VK_QUOTE
#ifndef VK_BACK_QUOTE
#define VK_BACK_QUOTE 0xC0
#endif // VK_BACK_QUOTE
#ifndef VK_SLASH
#define VK_SLASH 0xBF
#endif // VK_SLASH
#ifndef VK_BACK_SLASH
#define VK_BACK_SLASH 0xDC
#endif // VK_BACK_SLASH
#ifndef VK_EQUALS
#define VK_EQUALS 0xBB
#endif // VK_EQUALS
#ifndef VK_OPEN_BRACKET
#define VK_OPEN_BRACKET 0xDB
#endif // VK_OPEN_BRACKET
#ifndef VK_CLOSE_BRACKET
#define VK_CLOSE_BRACKET 0xDD
#endif // VK_CLOSE_BRACKET
#ifndef VK_GR_LESS
#define VK_GR_LESS 0xE2
#endif // VK_GR_LESS



static int oldx = 0, oldy = 0;
//extern int shutterGlasses;

int mouseX, mouseY;

static short gcWheelDelta = 0;

void swapbuffers32()
{
	ghDC = wglGetCurrentDC();
	SwapBuffers(ghDC); 
}


static bool m_fullscreen = false;
static bool dualmonitor = false;
static RECT smallrect;
void setLastCursor();
bool EnableFullscreen(int w, int h, int bpp)
{
#if defined(_MSC_VER)
    /* adapted from http://www.gamedev.net/community/forums/topic.asp?topic_id=418397 */
    /* normally I pass in bpp=32. If you set debugit=true below and do a run, you'll see the modes available */
    /* CDS_FULLSCREEN
       for dual-monitor the author warns to disable nVidia's nView (they hook 
       into changedisplaysettings but not changedisplaysettingsex)
       one idea: don't do the ex on single monitors - set dualmonitor=false above.
       Find out the name of the device this window - is on (this is for multi-monitor setups) 
    */
    MONITORINFOEX monInfo;
    LONG ret;
    bool ok;
    DWORD style, exstyle;
    bool debugit;
    DEVMODE dmode;
    bool foundMode;
    int i;

    HMONITOR hMonitor = MonitorFromWindow(ghWnd, MONITOR_DEFAULTTOPRIMARY);
    memset(&monInfo, 0, sizeof(MONITORINFOEX));
    monInfo.cbSize = sizeof(MONITORINFOEX);
    GetMonitorInfo(hMonitor, (LPMONITORINFO)&monInfo);

    /* Find the requested device mode */
    foundMode = false;
    memset(&dmode, 0, sizeof(DEVMODE));
    dmode.dmSize = sizeof(DEVMODE);
    debugit = false;
    for(i=0 ; EnumDisplaySettings(monInfo.szDevice, i, &dmode) && !foundMode ; ++i)
    {
        foundMode = (dmode.dmPelsWidth==(DWORD)w) && 
	    (dmode.dmPelsHeight==(DWORD)h) &&
	    (dmode.dmBitsPerPel==(DWORD)bpp);
	if(debugit)
	    ConsoleMessage("found w=%d h=%d bpp=%d\n",(int)dmode.dmPelsWidth, (int)dmode.dmPelsHeight, (int)dmode.dmBitsPerPel);
    }
    if(!foundMode || debugit )
    {
	ConsoleMessage("error: suitable display mode for w=%d h=%d bpp=%d not found\n",w,h,bpp);
        GetWindowRect(ghWnd, &smallrect);
	ConsoleMessage("window rect l=%d t=%d r=%d b=%d\n",smallrect.top,smallrect.left,smallrect.right,smallrect.bottom);
	return false;
    }
    dmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    /* If we're switching from a windowed mode to this fullscreen
       mode, save some information about the window so that it can
       be restored when switching back to windowed mode
    */
    style=0, exstyle=0;
    if(!m_fullscreen)
    {
        /* Save the current window position/size */
        GetWindowRect(ghWnd, &smallrect);

        /* Save the window style and set it for fullscreen mode */
        style = GetWindowLongPtr(ghWnd, GWL_STYLE);
        exstyle = GetWindowLongPtr(ghWnd, GWL_EXSTYLE);
        SetWindowLongPtr(ghWnd, GWL_STYLE, style & (~WS_OVERLAPPEDWINDOW));
        SetWindowLongPtr(ghWnd, GWL_EXSTYLE, exstyle | WS_EX_APPWINDOW | WS_EX_TOPMOST);
    }

    // Attempt to change the resolution
    if(dualmonitor)
    {
	ret = ChangeDisplaySettingsEx(monInfo.szDevice, &dmode, NULL, CDS_FULLSCREEN, NULL);
    }else{
	ret = ChangeDisplaySettings(&dmode, CDS_FULLSCREEN);
    }
    //LONG ret = ChangeDisplaySettings(&dmode, CDS_FULLSCREEN);
    ok = (ret == DISP_CHANGE_SUCCESSFUL);
    if(ok) m_fullscreen = true;

    /* If all was good resize & reposition the window
       to match the new resolution on the correct monitor
    */
    if(ok)
    {
        /* We need to call GetMonitorInfo() again becase
        // details may have changed with the resolution
	*/
        GetMonitorInfo(hMonitor, (LPMONITORINFO)&monInfo);

        /* Set the window's size and position so
        // that it covers the entire screen
	*/
        SetWindowPos(ghWnd, NULL, monInfo.rcMonitor.left, monInfo.rcMonitor.top, (int)w, (int)h,
                     SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER | SWP_NOREPOSITION | SWP_NOZORDER);
    }

    /* If the attempt failed and we weren't already
    // in fullscreen mode, restore the window styles
    */
    else if(!m_fullscreen)
    {
        SetWindowLongPtr(ghWnd, GWL_STYLE, style);
        SetWindowLongPtr(ghWnd, GWL_EXSTYLE, exstyle);
    }
    return ok;
#endif
    return FALSE;
}

void DisableFullscreen()
{
#if defined(_MSC_VER)
    if(m_fullscreen) {

        /* Find out the name of the device this window
           is on (this is for multi-monitor setups) */
        MONITORINFOEX monInfo;
	DWORD style,exstyle;
        HMONITOR hMonitor = MonitorFromWindow(ghWnd, MONITOR_DEFAULTTOPRIMARY);
        memset(&monInfo, 0, sizeof(MONITORINFOEX));
        monInfo.cbSize = sizeof(MONITORINFOEX);
        GetMonitorInfo(hMonitor, (LPMONITORINFO)&monInfo);

        /* Restore the display resolution */
        ChangeDisplaySettingsEx(monInfo.szDevice, NULL, NULL, 0, NULL);
        /*ChangeDisplaySettings(NULL, 0);*/

        m_fullscreen = false;

        /* Restore the window styles */
        style = GetWindowLongPtr(ghWnd, GWL_STYLE);
        exstyle = GetWindowLongPtr(ghWnd, GWL_EXSTYLE);
        SetWindowLongPtr(ghWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        SetWindowLongPtr(ghWnd, GWL_EXSTYLE, exstyle & (~(WS_EX_APPWINDOW | WS_EX_TOPMOST)));

        /* Restore the window size/position  */
        /*SetPosition(m_windowedX, m_windowedY);*/
        /* SetSize(m_windowedWidth, m_windowedHeight); */
	SetWindowPos(ghWnd ,       /* handle to window */
		     HWND_TOPMOST,  /* placement-order handle */
		     smallrect.left,     /* horizontal position */
		     smallrect.top,      /* vertical position */
		     smallrect.right - smallrect.left,  /* width */
		     smallrect.bottom - smallrect.top, /* height */
		     SWP_SHOWWINDOW /* window-positioning options); */
	    );
    }
#endif
}

BOOL bSetupPixelFormat(HDC hdc) 
{ 
	/* http://msdn.microsoft.com/en-us/library/dd318284(VS.85).aspx */
    PIXELFORMATDESCRIPTOR pfd, *ppfd; 
    int pixelformat; 
	int more;

	ppfd = &pfd; 

	memset(ppfd,0,sizeof(PIXELFORMATDESCRIPTOR));
 
    ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR); 
    ppfd->nVersion = 1; 
    ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; 
	if(gglobal()->display.shutterGlasses ==1)
		ppfd->dwFlags |= PFD_STEREO;
	ppfd->iLayerType = PFD_MAIN_PLANE;
    ppfd->iPixelType = PFD_TYPE_RGBA; /* PFD_TYPE_COLORINDEX; */
    ppfd->cColorBits = 24; 
	ppfd->cAlphaBits = 8;
    ppfd->cDepthBits = 32; 
    //not using accum now, using color masks ppfd->cAccumBits = 64; /*need accum buffer for shader anaglyph - 8 bits per channel OK*/
    ppfd->cStencilBits = 8; 
	ppfd->cAuxBuffers = 0;
	ppfd->cAccumBits = 0;
 
    /* pixelformat = ChoosePixelFormat(hdc, ppfd); */
	if ( (pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0 ) 
	{ 
		MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK); 
		return FALSE; 
	} 
 
	/*  seems to fail stereo gracefully/quietly, allowing you to detect with glGetbooleanv(GL_STEREO,) in shared code
	*/
	DescribePixelFormat(hdc, pixelformat, sizeof(PIXELFORMATDESCRIPTOR), ppfd);
	printf("Depth Bits = %d\n",(int)(ppfd->cDepthBits));
	if(gglobal()->display.shutterGlasses > 0)
		printf("got stereo? = %d\n",(int)(ppfd->dwFlags & PFD_STEREO));
	/**/
    if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
    { 
        MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK); 
        return FALSE; 
    } 
 
    return TRUE; 
} 

/**
 *   fv_create_GLcontext: create the main OpenGL context.
 *                     TODO: finish implementation for Mac and Windows.
 */
bool fv_create_GLcontext()
{	
	fwl_thread_dump();
	printf("starting createcontext32a\n");
	ghDC = GetDC(ghWnd); 
	printf("got hdc\n");
	if (!bSetupPixelFormat(ghDC))
		printf("ouch - bSetupPixelFormat failed\n");
	ghRC = wglCreateContext(ghDC); 
	printf("created context\n");
	return TRUE;
}

/**
 *   fv_bind_GLcontext: attache the OpenGL context to the main window.
 *                   TODO: finish implementation for Mac and Windows.
 */
bool fv_bind_GLcontext()
{
	RECT rect;
	fwl_thread_dump();

	if (wglMakeCurrent(ghDC, ghRC)) {
		GetClientRect(ghWnd, &rect); 
		gglobal()->display.screenWidth = rect.right; /*used in mainloop render_pre setup_projection*/
		gglobal()->display.screenHeight = rect.bottom;
		return TRUE;
	}

	return FALSE;
}
bool fv_create_and_bind_GLcontext(HWND hWnd)
{
	RECT rect;
	HDC hDC;
	HGLRC hRC;
	int width, height;
	/* create GL context */
	fwl_thread_dump();
	printf("starting createcontext32b\n");
	hDC = GetDC(hWnd); 
	printf("got hdc\n");
	if (!bSetupPixelFormat(hDC))
		printf("ouch - bSetupPixelFormat failed\n");
	hRC = wglCreateContext(hDC); 
	printf("created context\n");
	/* bind GL context */

	fwl_thread_dump();
	width = gglobal()->display.screenWidth;
	height = gglobal()->display.screenHeight;
	if (wglMakeCurrent(hDC, hRC)) {
		//GetClientRect(hWnd, &rect); 
		//gglobal()->display.screenWidth = rect.right; /*used in mainloop render_pre setup_projection*/
		//gglobal()->display.screenHeight = rect.bottom;
		return TRUE;
	}

	return FALSE;

}

static HCURSOR hSensor, hArrow;
static HCURSOR cursor;
void loadCursors()
{
	hSensor = LoadCursor(NULL,IDC_HAND); /* prepare sensor_cursor */
	hArrow = LoadCursor( NULL, IDC_ARROW );
}
void updateCursorStyle0(int cstyle)
{
	if(cstyle == SCURSE)
		SetCursor(hSensor);
	if(cstyle == ACURSE)
		SetCursor(hArrow);
}
/* values from WinUser.h */
#define PHOME_KEY  VK_HOME  //0x24
#define PPGDN_KEY  VK_NEXT  //0x22
#define PLEFT_KEY  VK_LEFT  //0x25
#define PEND_KEY   VK_END   //0x23
#define PUP_KEY    VK_UP    //0x26
#define PRIGHT_KEY VK_RIGHT //0x27
#define PPGUP_KEY  VK_PRIOR //0x21
#define PDOWN_KEY  VK_DOWN  //0x28
#define PF1_KEY  VK_F1 //0x70
#define PF2_KEY  VK_F2 //0x71
#define PF3_KEY  VK_F3 //0x72
#define PF4_KEY  VK_F4 //0x73
#define PF5_KEY  VK_F5 //0x74
#define PF6_KEY  VK_F6 //0x75
#define PF7_KEY  VK_F7 //0x76
#define PF8_KEY  VK_F8 //0x77
#define PF9_KEY  VK_F9 //0x78
#define PF10_KEY VK_F10 //0x79
#define PF11_KEY VK_F11 //0x7a
#define PF12_KEY VK_F12 //0x7b
#define PALT_KEY VK_MENU //0x12   
#define PCTL_KEY VK_CONTROL //0x11
#define PSFT_KEY VK_SHIFT   //0x10
#define PDEL_KEY VK_DELETE  //0x2E  //2E is DELETE 
#define PRTN_KEY VK_RETURN  //0x0D  //13
#define PNUM0 VK_NUMPAD0
#define PNUM1 VK_NUMPAD1
#define PNUM2 VK_NUMPAD2
#define PNUM3 VK_NUMPAD3
#define PNUM4 VK_NUMPAD4
#define PNUM5 VK_NUMPAD5
#define PNUM6 VK_NUMPAD6
#define PNUM7 VK_NUMPAD7
#define PNUM8 VK_NUMPAD8
#define PNUM9 VK_NUMPAD9
#define PNUMDEC VK_DECIMAL

/* from http://www.web3d.org/x3d/specifications/ISO-IEC-19775-1.2-X3D-AbstractSpecification/index.html
section 21.4.1 
Key Value
Home 13
End 14
PGUP 15
PGDN 16
UP 17
DOWN 18
LEFT 19
RIGHT 20
F1-F12  1 to 12
ALT,CTRL,SHIFT true/false
*/
//#define F1_KEY  1
//#define F2_KEY  2
//#define F3_KEY  3
//#define F4_KEY  4
//#define F5_KEY  5
//#define F6_KEY  6
//#define F7_KEY  7
//#define F8_KEY  8
//#define F9_KEY  9
//#define F10_KEY 10
//#define F11_KEY 11
//#define F12_KEY 12
//#define HOME_KEY 13
//#define END_KEY  14
//#define PGUP_KEY 15
//#define PGDN_KEY 16
//#define UP_KEY   17
//#define DOWN_KEY 18
//#define LEFT_KEY 19
//#define RIGHT_KEY 20
//#define ALT_KEY	30 /* not available on OSX */
//#define CTL_KEY 31 /* not available on OSX */
//#define SFT_KEY 32 /* not available on OSX */
//#define DEL_KEY 0XFFFF /* problem: I'm insterting this back into the translated char stream so 0xffff too high to clash with a latin? */
//#define RTN_KEY 13  //what about 10 newline?

#define KEYPRESS 1
#define KEYDOWN 2
#define KEYUP	3

int platform2web3dActionKeyWIN32(int platformKey)
{
	int key;

	key = 0; //platformKey;
	if(platformKey >= PF1_KEY && platformKey <= PF12_KEY)
		key = platformKey - PF1_KEY + F1_KEY;
	else 
		switch(platformKey)
		{
		case PHOME_KEY:
			key = HOME_KEY; break;
		case PEND_KEY:
			key = END_KEY; break;
		case PPGDN_KEY:
			key = PGDN_KEY; break;
		case PPGUP_KEY:
			key = PGUP_KEY; break;
		case PUP_KEY:
			key = UP_KEY; break;
		case PDOWN_KEY:
			key = DOWN_KEY; break;
		case PLEFT_KEY:
			key = LEFT_KEY; break;
		case PRIGHT_KEY:
			key = RIGHT_KEY; break;
		case PDEL_KEY:  
			key = DEL_KEY; break;
		case PALT_KEY:
			key = ALT_KEY; break;
		case PCTL_KEY:
			key = CTL_KEY; break;
		case PSFT_KEY:
			key = SFT_KEY; break;
		case PNUM0:
			key = NUM0; break;
		case PNUM1:
			key = NUM1; break;
		case PNUM2:
			key = NUM2; break;
		case PNUM3:
			key = NUM3; break;
		case PNUM4:
			key = NUM4; break;
		case PNUM5:
			key = NUM5; break;
		case PNUM6:
			key = NUM6; break;
		case PNUM7:
			key = NUM7; break;
		case PNUM8:
			key = NUM8; break;
		case PNUM9:
			key = NUM9; break;
		case PNUMDEC:
			key = NUMDEC; break;
		default:
			key = 0;
		}
	return key;
}
/* Print n as a binary number - scraped from www */
void printbitssimple(int n) {
	unsigned int i,j;
	j=0;
	i = 1<<(sizeof(n) * 8 - 1);
	while (i > 0) {
		if (n & i)
			printf("1");
		else
			printf("0");
		i >>= 1;
		j++;
		if(j%8 == 0) printf(" ");
	}
}
//void setArrowCursor0();
LRESULT CALLBACK PopupWndProc( 
    HWND hWnd, 
    UINT msg, 
    WPARAM wParam,
    LPARAM lParam )
{
    PAINTSTRUCT ps;
    LONG lRet = 1; 
    RECT rect; 
    char kp;
	int keyraw;
    int mev,err;
    int butnum;
	int updown;
	int actionKey;
static int altState = 0;
	int altDown;
	int lkeydata;
static int shiftState = 0;
    mev = 0;
    butnum = 0;
    ghWnd = hWnd;
    switch( msg ) {

    case WM_CREATE: 
	printf("wm_create\n");
	//fv_create_GLcontext();
	//fv_bind_GLcontext();
	fv_create_and_bind_GLcontext(hWnd);
	break; 
 
    case WM_SIZE: 
	GetClientRect(hWnd, &rect); 
	gglobal()->display.screenWidth = rect.right; /*used in mainloop render_pre setup_projection*/
	gglobal()->display.screenHeight = rect.bottom;
	resize_GL(rect.right, rect.bottom); 
	fwl_setScreenDim(rect.right,rect.bottom);
	break; 

    case WM_DISPLAYCHANGE:
	/*triggred when the display mode is changed ie changedisplaysettings window <> fullscreen 
	  or how about if you drag a window onto a second monitor?
	*/
	ghDC = GetDC(hWnd); 
	if (!bSetupPixelFormat(ghDC)) 
	    PostQuitMessage (0); 
	printf("WM_DISPLAYCHANGE happening now\n");

	/* ???? do we have to recreate an OpenGL context 
	   when display mode changed ? */
	ghRC = wglCreateContext(ghDC); 
	wglMakeCurrent(ghDC, ghRC); 
	GetClientRect(hWnd, &rect); 

	break; 

    case WM_CLOSE: 
	ghRC = wglGetCurrentContext();
	if (ghRC) 
	    wglDeleteContext(ghRC); 
	ghDC = GetDC(hWnd); 
	if (ghDC) 
	    ReleaseDC(hWnd, ghDC); 
	ghRC = 0; 
	ghDC = 0; 
	 
	DestroyWindow (hWnd);
	fwl_doQuit();
	break; 

  //  case WM_SETCURSOR:
	 //  //setLastCursor();
		//printf("_");
	 //  //setArrowCursor0();
	 //  break;

/**************************************************************\
 *     WM_PAINT:                                                *
\**************************************************************/

    case WM_PAINT:
		  
	ghDC = BeginPaint( hWnd, &ps );
	/*TextOut( ghDC, 10, 10, "Hello, Windows!", 13 ); */
	EndPaint( hWnd, &ps );
	break;

/**************************************************************\
 *     WM_COMMAND:                                              *
\**************************************************************/

    case WM_COMMAND:
	/*
	  switch( wParam ) {
	  case IDM_ABOUT:
	  DialogBox( ghInstance, "AboutDlg", hWnd, (DLGPROC)
	  AboutDlgProc );
	  break;
	  }
	*/
	break;

/**************************************************************\
 *     WM_DESTROY: PostQuitMessage() is called                  *
\**************************************************************/

    case WM_DESTROY: 
	ghRC = wglGetCurrentContext();
	if (ghRC) 
	    wglDeleteContext(ghRC); 
	ghDC = GetDC(hWnd); 
	if (ghDC) 
	    ReleaseDC(hWnd, ghDC); 
	 
	PostQuitMessage (0); 
	break; 

	case WM_KEYDOWN:
	case WM_KEYUP: 
		/* raw keystrokes with UP and DOWN separate, 
			used for fly navigation and KeySensor node */
	lkeydata = lParam;
	updown = KEYDOWN; //KeyPress;
	if(msg==WM_KEYUP) updown = KEYUP; //KeyRelease;
	if(updown==KeyPress)
		if(lkeydata & 1 << 30) 
			break; //ignor - its an auto-repeat
	//altDown = lkeydata & 1 << 29; //alt key is pressed while the current key is pressed
	/*
	printf("KF_ALTDOWN");
	printbitssimple((int)KF_ALTDOWN);
	printf("\n");
	printf("lkeydata  ");
	printbitssimple(lkeydata);
	printf(" %d %o %x \n",lkeydata,lkeydata,lkeydata);
	*/
	/*
	printbitssimple(wParam); printf("\n");
	//altDown = lkeydata & KF_ALTDOWN;
	//#define KF_ALTDOWN        0x2000
	if(altState && !altDown) fwl_do_rawKeyPress(ALT_KEY,KEYUP + 10);
	if(!altState && altDown) fwl_do_rawKeyPress(ALT_KEY,KEYDOWN + 10);
	altState = altDown;
	*/
	//kp = (char)wParam; 
	keyraw = (int) wParam;
	if(keyraw == VK_OEM_1) keyraw = (int)';'; //US
	if(updown==KEYUP && keyraw == VK_DELETE)
		fwl_do_rawKeyPress(DEL_KEY,KEYPRESS);
	actionKey = platform2web3dActionKeyWIN32(keyraw);
	if(actionKey)
		fwl_do_rawKeyPress(actionKey,updown+10);
	else
		fwl_do_rawKeyPress(keyraw,updown);

	//if(kp >= 'A' && kp <= 'Z' && shiftState ==0 ) kp = (char)tolower(wParam); //the F1 - F12 are small case ie y=121=F1
	//printf("      wParam %d %x\n",wParam, wParam);
	//x3d specs http://www.web3d.org/x3d/specifications/ISO-IEC-19775-1.2-X3D-AbstractSpecification/index.html
	//section 21.4.1 has a table of KeySensor ActionKey values which we must map to at some point
	// http://msdn.microsoft.com/en-us/library/ms646268(VS.85).aspx windows keyboard messages
	//switch (wParam) 
	//{ 
	//	//case VK_LEFT:     -- translation to web3d moved to platform2web3dActionKey(int platformKey)
	//	//	kp = 'j';//19;
	//	//	break; 
	//	//case VK_RIGHT: 
	//	//	kp = 'l';//20;
	//	//	break; 
	//	//case VK_UP: 
	//	//	kp = 'p';//17;
	//	//	break; 
	//	//case VK_DOWN: 
	//	//	kp =  ';';//18;
	//	//	break; 
	//	//case -70:
	//	//	kp = ';';
	//	//	break;
	//	//case VK_SHIFT: //0x10
	//	//	if(updown==KeyPress) shiftState = 1;
	//	//	if(updown==KeyRelease) shiftState = 0;
	//	//case VK_CONTROL: //0x11
	//	//case VK_MENU:  //ALT 0x12 - doesn't work like this: it alters the next key pressed
	//	//	break;
	//	/*
	//	case VK_OPEN_BRACKET:
	//		printf("[");
	//		// width, height, bpp of monitor
	//		EnableFullscreen(1680,1050,32);
	//		break;
	//	case VK_CLOSE_BRACKET:
	//		printf("]");
	//		DisableFullscreen();
	//		break;
	//	*/
	//	case VK_OEM_1:
	//		kp = ';'; //could be : or ; but tolower won't lowercase it, but returns same character if it can't
	//		break;
	//	default:
	//		///* we aren't using WCHAR so we will translate things like shift-/ to ? */
	//		//{ 
	//		//	/* http://msdn.microsoft.com/en-us/library/ms646267(VS.85).aspx  shows where to get the scan code */
	//		//	int k2; int i2; unsigned short k3;
	//		//	UINT scancode;
	//		//	//scancode = ((lParam << 8)>>8)>>16;
	//		//	scancode = lParam >>16;
	//		//	k2 = MapVirtualKeyEx(scancode,MAPVK_VSC_TO_VK,GetKeyboardLayout(0));
	//		//	k2 = MapVirtualKeyEx(k2,MAPVK_VK_TO_CHAR,NULL);
	//		//	if(k2) kp = k2;
	//		//	k3 = 0;
	//		//	i2 = ToAsciiEx(wParam,scancode,NULL,&k3,0,GetKeyboardLayout(0));
	//		//	if(i2>0)
	//		//		kp = k3;
	//		//}
	//		break;
	//}
	//fwl_do_rawKeyPressWIN32(keyraw, updown); 
	break; 

	case WM_CHAR:
		/* fully translated char, with any SHIFT applied, and not a modifier key itself.
		   used for keyboard commands to freewrl -except fly navigation- 
		   and web3d StringSensor node.
		*/
		//kp = (char)wParam;
		//fwl_do_keyPress(kp,KeyChar);
		keyraw = (int) wParam;
		fwl_do_rawKeyPress(keyraw,KEYPRESS);
		break;
	/* Mouse events, processed */
    case WM_LBUTTONDOWN:
	butnum = 1;
	mev = ButtonPress;
	break;
    case WM_MBUTTONDOWN:
	butnum = 2;
	mev = ButtonPress;
	break;
    case WM_RBUTTONDOWN:
	butnum = 3;
	mev = ButtonPress;
	break;
    case WM_LBUTTONUP:
	butnum = 1;
	mev = ButtonRelease;
	break;
    case WM_MBUTTONUP:
	butnum = 2;
	mev = ButtonRelease;
	break;
    case WM_RBUTTONUP:
	butnum = 3;
	mev = ButtonRelease;
	break;
    case WM_MOUSEMOVE:
    {
	POINTS pz;
	pz= MAKEPOINTS(lParam); 
	mouseX = pz.x;
	mouseY = pz.y;
    } 
    mev = MotionNotify;
    break;
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL                   0x020A
#endif
    case WM_MOUSEWHEEL:
	/* The WM_MOUSEWHEEL message is sent to the focus window 
	 * when the mouse wheel is rotated. The DefWindowProc 
	 * function propagates the message to the window's parent.
	 * There should be no internal forwarding of the message, 
	 * since DefWindowProc propagates it up the parent chain 
	 * until it finds a window that processes it.
	 */
	if(!(wParam & (MK_SHIFT | MK_CONTROL))) {
	    /* gcWheelDelta -= (short) HIWORD(wParam); windows snippet */
	    gcWheelDelta = (short) HIWORD(wParam);
	    mev = MotionNotify;
	    break;
	}

	/* falls through to default ? */

/**************************************************************\
 *     Let the default window proc handle all other messages    *
\**************************************************************/

    default:
	return( DefWindowProc( hWnd, msg, wParam, lParam ));
    }
    if(mev)
    {
	/*void fwl_handle_aqua(const int mev, const unsigned int button, int x, int y);*/
	/* butnum=1 left butnum=3 right (butnum=2 middle, not used by freewrl) */
	//int cstyle = getCursorStyle(hWnd);
	//if(cstyle == SCURSE)
	//	SetCursor(hSensor);
	//if(cstyle == ACURSE)
	//	SetCursor(hArrow);
	fwl_handle_aqua(mev,butnum,mouseX,mouseY); /* ,gcWheelDelta); */
    }
    return 0;
}

int doEventsWin32A()
{
    static int eventcount = 0;
    MSG msg;
    do
    {
	while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) == TRUE) 
        { 
            if (GetMessage(&msg, NULL, 0, 0) ) 
            { 
                TranslateMessage(&msg);
                DispatchMessage(&msg); 
            } else { 
                return TRUE; 
            } 

        } 
	eventcount++;
    }while(0); /*eventcount < 1000);*/
    eventcount = 0;
    return FALSE;
}
/*
//bool lastCursorArrow = true;
void setSensorCursor()
{
	//lastCursorArrow = false;
	SetCursor(hSensor);
	printf("S");
}
void setArrowCursor()
{
	//lastCursorArrow = true;
	SetCursor(hArrow);
	printf("^");
}
*/
//void setLastCursor()
//{
//	printf("_");
//	if(lastCursorArrow)
//		setArrowCursor();
//	else
//		setSensorCursor();
//}

//void setCursor()
//{
//	switch (ccurse) {
//	case SCURSE: cursor = hSensor;
//	case ACURSE: cursor = hArrow;
//	default:
//		DEBUG_MSG("setCursor: invalid value for ccurse: %d\n", ccurse);
//	}
//	SetCursor(cursor);
//}
void fv_setGeometry_from_cmdline(const char *gstring)
{
	int w,h,i;
	char *tok[2];
	char *str = malloc(sizeof(gstring)+1);
	strcpy(str,gstring);
	tok[0] = str;
	for(i=0;i<strlen(gstring);i++)
		if(str[i] == 'x' || str[i] == 'X')
		{
			str[i] = '\0';
			tok[1] = i+1;
			break;
		}
	sscanf(tok[0],"%d",&w);
	sscanf(tok[1],"%d",&h);
	gglobal()->display.win_width = w; 
    gglobal()->display.win_height = h; 
	free(str);

}
/*======== "VIRTUAL FUNCTIONS" ==============*/

void setWindowTitle() //char *window_title)
{
	//XStoreName(Xdpy, Xwin, window_title);
	//XSetIconName(Xdpy, Xwin, window_title);
	//http://msdn.microsoft.com/en-us/library/ms633546(VS.85).aspx
	//SetWindowText(
	 // __in      HWND hWnd,
	 // __in_opt  LPCTSTR lpString);
	
	//SetWindowText(ghWnd,fwl_getWindowTitle());
	SetWindowText(ghWnd,getWindowTitle()); //window_title);
}

/**
 *   open_display: setup up Win32.
 */
int fv_open_display()
{
	/* nothing to do */
	return TRUE;
}

static char *wgetpath = NULL;
TCHAR szPath[MAX_PATH];
static int wgetpathLoaded = 0;
char *getWgetPath()
{
	if(!wgetpathLoaded)
	{
		if( !GetModuleFileName( NULL, &szPath[1], MAX_PATH ) )
		{
			printf("Cannot install service (%d)\n", GetLastError());
			return 0;
		}
		else
		{
			int len;
			wgetpath = &szPath[1];
			PathRemoveFileSpec(wgetpath);
			PathAppend(wgetpath,"wget.exe"); 
			// c:\program files\ breaks up in the space so we "" around the path
			strcat(wgetpath,"\"");
			wgetpath = szPath;
			wgetpath[0] = '"';
		}
	}
	wgetpathLoaded = 1;
	return wgetpath;
}

/**
 *   create_main_window: setup up Win32 main window and TODO query fullscreen capabilities.
 */
int create_main_window0(int argc, char *argv[])
{
    HINSTANCE hInstance; 
    WNDCLASS wc;
    MSG msg;
    //RECT rect; 
	int width, height;
    int nCmdShow = SW_SHOW;

    printf("starting createWindow32\n"); 
    /* I suspect hInstance should be get() and passed in from the console program not get() in the dll, but .lib maybe ok */
    hInstance = (HANDLE)GetModuleHandle(NULL); 
    gglobal()->display.window_title = "FreeWRL";

/*  Blender Ghost
    WNDCLASS wc;
    wc.style= CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc= s_wndProc;
    wc.cbClsExtra= 0;
    wc.cbWndExtra= 0;
    wc.hInstance= ::GetModuleHandle(0);
    wc.hIcon = ::LoadIcon(wc.hInstance, "APPICON");
  		
    if (!wc.hIcon) {
    ::LoadIcon(NULL, IDI_APPLICATION);
    }
    wc.hCursor = ::LoadCursor(0, IDC_ARROW);
    wc.hbrBackground= (HBRUSH)::GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = 0;
    wc.lpszClassName= GHOST_WindowWin32::getWindowClassName();
    
    // Use RegisterClassEx for setting small icon
    if (::RegisterClass(&wc) == 0) {
    success = GHOST_kFailure;
    }
*/
	//hSensor = LoadCursor(NULL,IDC_HAND); /* prepare sensor_cursor */
	//hArrow = LoadCursor( NULL, IDC_ARROW );
	//loadCursors();
    wc.lpszClassName = "FreeWrlAppClass";
    wc.lpfnWndProc = PopupWndProc; //MainWndProc;
    wc.style = CS_VREDRAW | CS_HREDRAW; /* 0 CS_OWNDC |  */
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(wc.hInstance, "APPICON");
    if (!wc.hIcon) {
		wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	}
    wc.hCursor = NULL; //hArrow;
    wc.hbrBackground = (HBRUSH)( COLOR_WINDOW+1 );
    wc.lpszMenuName = 0; /* "GenericAppMenu"; */
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;

    RegisterClass( &wc );
	width  = gglobal()->display.win_width + 8;  //windows gui eats 4 on each side
	height = gglobal()->display.win_height + 34;  // and 26 for the menu bar

    ghWnd = CreateWindowEx( WS_EX_APPWINDOW, "FreeWrlAppClass", "freeWrl win32 rev 0.0", 
			    /* ghWnd = CreateWindow( "GenericAppClass", "Generic Application", */
			    WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
			    CW_USEDEFAULT, 
			    CW_USEDEFAULT, 
			    width, 
			    height, 
			    NULL, 
			    NULL, 
			    hInstance, 
			    NULL); 
    /* make sure window was created */ 

    if (!ghWnd) 
        return FALSE; 

    printf("made a window\n");

    //GetClientRect(ghWnd, &rect); 
   
    ShowWindow( ghWnd, SW_SHOW); /* SW_SHOWNORMAL); /*nCmdShow );*/
    printf("showed window\n");


    UpdateWindow(ghWnd); 
    printf("updated window - leaving createwindow\n");
	//setWindowTitle00();
	//ShowCursor(0); //turns off hArrow, hHand cursors
	setArrowCursor();
    return TRUE;
}

int fv_create_main_window(int argc, char *argv[])
{
	loadCursors();
	if( fwl_params.winToEmbedInto > 0 )
	{
		HWND hWnd;
		//if defined(FRONTEND_HANDLES_DISPLAY_THREAD) || defined(command line option with window handle)
		hWnd = (HWND)fwl_params.winToEmbedInto;
		//fv_create_GLcontext();
		//fv_bind_GLcontext();
		fv_create_and_bind_GLcontext(hWnd);
		return TRUE;
	}
	else
		return create_main_window0(argc, argv);
}

#endif /* IPHONE */
