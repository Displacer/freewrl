/*
=INSERT_TEMPLATE_HERE=

$Id: Component_Text.c,v 1.60 2013/09/13 18:37:39 crc_canada Exp $

X3D Text Component

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

#include <system_fonts.h>

#include <display.h>
#include <internal.h>

#include <libFreeWRL.h>

#include "../vrml_parser/Structs.h"
#include "../input/InputFunctions.h"
#include "../main/headers.h"
#include "../opengl/Textures.h"
#include "../scenegraph/Viewer.h"
#include "../opengl/OpenGL_Utils.h"

#include "Collision.h"
#include "LinearAlgebra.h"
#include "Component_Shape.h"
#include "../scenegraph/Tess.h"
#include "../scenegraph/Polyrep.h"


#ifdef _ANDROID
#ifdef ANDROID_DEBUG
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif //ANDROID_DEBUG
#endif //ANDROID


#define XRES 96
#define YRES 96
#define PPI 72
#define POINTSIZE 20


#define TOPTOBOTTOM (fsparam & 0x04)
#define LEFTTORIGHT (!(fsparam & 0x02))

#define OUT2GL(a) (p->x_size * (0.0 +a) / ((1.0*(p->font_face[p->myff]->height)) / PPI*XRES))

/* now defined in system_fonts.h 
include <ft2build.h>
** include <ftoutln.h>
include FT_FREETYPE_H
include FT_GLYPH_H */


typedef struct pComponent_Text{

#ifdef _ANDROID
	// Android UI sends in file descriptors and open file for fonts.
	// files are in the assets folder; we assume that the fd is open and fseek'd properly.

	FILE *androidFontFile;
	int fileLen;
#endif //ANDROID

	/* initialize the library with this variable */
	FT_Library library; /* handle to library */

	#define num_fonts 32
	FT_Face font_face[num_fonts];           /* handle to face object */
	int     font_opened[num_fonts];         /* is this font opened   */


	/* we load so many gliphs into an array for processing */
	#define         MAX_GLYPHS      2048
	FT_Glyph        glyphs[MAX_GLYPHS];
	int             cur_glyph;
	int             TextVerbose;// = FALSE;


	/* decompose interface func pointer */
	FT_Outline_Funcs FW_outline_interface;


	/* lets store the font paths here */
	#define fp_name_len 256
	char *font_directory;// = NULL;
	char thisfontname[fp_name_len];

	/* where are we? */
	double pen_x, pen_y;

	/* if this is a status bar, put depth different than 0.0 */
	float TextZdist;

	double x_size;          /* size of chars from file */
	double y_size;          /* size of chars from file */
	int   myff;             /* which index into font_face are we using  */


	/* for keeping track of tesselated points */
	int FW_RIA[500];        /* pointer to which point is returned by tesselator  */
	int FW_RIA_indx;                        /* index into FW_RIA                         */
	struct X3D_PolyRep *FW_rep_;    /* this is the internal rep of the polyrep           */
	int FW_pointctr;                /* how many points used so far? maps into rep-_coord */
	int indx_count;                 /* maps intp FW_rep_->cindex                         */
	int coordmaxsize;               /* maximum coords before needing to REALLOC          */
	int cindexmaxsize;              /* maximum cindexes before needing to REALLOC        */


	/* Outline callbacks and global vars */
	int contour_started;
	FT_Vector last_point;
	int FW_Vertex;

	/* flag to determine if we need to call the open_font call */
	int started;// = FALSE;


}* ppComponent_Text;
void *Component_Text_constructor(){
	void *v = malloc(sizeof(struct pComponent_Text));
	memset(v,0,sizeof(struct pComponent_Text));
	return v;
}
void Component_Text_init(struct tComponent_Text *t){
	//public
	//private
	t->prv = Component_Text_constructor();
	{
		ppComponent_Text p = (ppComponent_Text)t->prv;

		p->TextVerbose = FALSE;
		p->font_directory = NULL;
		/* flag to determine if we need to call the open_font call */
		p->started = FALSE;
	}
}
//	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;


/* function prototypes */
static void FW_NewVertexPoint(double Vertex_x, double Vertex_y);
static int FW_moveto (FT_Vector* to, void* user);
static int FW_lineto(FT_Vector* to, void* user);
static int FW_conicto(FT_Vector* control, FT_Vector* to, void* user);
static int FW_cubicto(FT_Vector* control1, FT_Vector* control2, FT_Vector* to, void* user);
static void FW_make_fontname (int num);
static int FW_init_face(void);
static double FW_extent (int start, int length);
static FT_Error FW_Load_Char(unsigned int idx);
static void FW_draw_outline(FT_OutlineGlyph oglyph);
static void FW_draw_character(FT_Glyph glyph);
static int open_font(void);

#ifdef _ANDROID
/* Android UI finds the font file(s) and sends them in here */
void fwg_AndroidFontFile(FILE *myFile,int len) {
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;
	p->androidFontFile = myFile;
	p->fileLen = len;
}

#endif //ANDROID



void render_Text (struct X3D_Text * node)
{
    COMPILE_POLY_IF_REQUIRED (NULL, NULL, NULL, NULL);
    DISABLE_CULL_FACE;
    render_polyrep(node);
}

void FW_NewVertexPoint (double Vertex_x, double Vertex_y)
{
    GLDOUBLE v2[3];
	ppComponent_Text p;
	ttglobal tg = gglobal();
	p = (ppComponent_Text)tg->Component_Text.prv;

    UNUSED(Vertex_x);
    UNUSED(Vertex_y);

    /* printf ("FW_NewVertexPoint setting coord index %d %d %d\n", */
    /*  p->FW_pointctr, p->FW_pointctr*3+2,p->FW_rep_->actualCoord[p->FW_pointctr*3+2]); */
    p->FW_rep_->actualCoord[p->FW_pointctr*3+0] = (float) OUT2GL(p->last_point.x + p->pen_x);
    p->FW_rep_->actualCoord[p->FW_pointctr*3+1] = (float) (OUT2GL(p->last_point.y) + p->pen_y);
    p->FW_rep_->actualCoord[p->FW_pointctr*3+2] = p->TextZdist;

    /* the following should NEVER happen.... */
    if (p->FW_RIA_indx >500) {
        ConsoleMessage ("Text, relative index too small\n");
        freewrlDie("FW_NewVertexPoint: this should never happen...");
    }

    p->FW_RIA[p->FW_RIA_indx]=p->FW_pointctr;
    v2[0]=p->FW_rep_->actualCoord[p->FW_pointctr*3+0];
    v2[1]=p->FW_rep_->actualCoord[p->FW_pointctr*3+1];
    v2[2]=p->FW_rep_->actualCoord[p->FW_pointctr*3+2];

	/* printf("glu s.b. rev 1.2 or newer, is: %s\n",gluGetString(GLU_VERSION)); */
    FW_GLU_TESS_VERTEX(tg->Tess.global_tessobj,v2,&p->FW_RIA[p->FW_RIA_indx]);

    if (p->TextVerbose) {
        printf ("FW_NewVertexPoint %f %f %f index %d\n",
                p->FW_rep_->actualCoord[p->FW_pointctr*3+0],
                p->FW_rep_->actualCoord[p->FW_pointctr*3+1],
                p->FW_rep_->actualCoord[p->FW_pointctr*3+2],
                p->FW_RIA_indx);
    }
    p->FW_pointctr++;
    p->FW_RIA_indx++;

    if (p->FW_pointctr >= p->coordmaxsize) {
        p->coordmaxsize+=800;
        p->FW_rep_->actualCoord = (float *)REALLOC(p->FW_rep_->actualCoord, 
                                                sizeof(*(p->FW_rep_->actualCoord))*p->coordmaxsize*3);
		printf("realloc actualCoord=%p\n",p->FW_rep_->actualCoord);
    }
}
#define GLU_UNKNOWN     100124
int FW_moveto (FT_Vector* to, void* user)
{
	ppComponent_Text p;
	ttglobal tg = gglobal();
	p = (ppComponent_Text)tg->Component_Text.prv;
    UNUSED(user);

    /* Have we started a new line */
    if (p->contour_started) {
       FW_GLU_NEXT_CONTOUR(tg->Tess.global_tessobj,GLU_UNKNOWN);
    }

    /* well if not, tell us that we have started one */
    p->contour_started = TRUE;

    p->last_point.x = to->x; p->last_point.y = to->y;

    if (p->TextVerbose)
        printf ("FW_moveto tox %ld toy %ld\n",to->x, to->y);



    return 0;
}

int FW_lineto (FT_Vector* to, void* user)
{
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;
    UNUSED(user);


    if ((p->last_point.x == to->x) && (p->last_point.y == to->y)) {
        /* printf ("FW_lineto, early return\n"); */
        return 0;
    }

    p->last_point.x = to->x; p->last_point.y = to->y;
    if (p->TextVerbose) {
        printf ("FW_lineto, going to %ld %ld\n",to->x, to->y);
    }

    FW_NewVertexPoint(OUT2GL(p->last_point.x+p->pen_x), OUT2GL(p->last_point.y + p->pen_y));



    return 0;
}


int FW_conicto (FT_Vector* control, FT_Vector* to, void* user)
{
    FT_Vector ncontrol;
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;



    /* Bezier curve calcs; fairly rough, but makes ok characters */

    if (p->TextVerbose)
        printf ("FW_conicto\n");

    /* Possible fix here!!! */
    ncontrol.x = (int) ((double) 0.25*p->last_point.x + 0.5*control->x + 0.25*to->x);
    ncontrol.y =(int) ((double) 0.25*p->last_point.y + 0.5*control->y + 0.25*to->y);

    /* printf ("Cubic points (%d %d) (%d %d) (%d %d)\n", p->last_point.x,p->last_point.y, */
    /* ncontrol.x, ncontrol.y, to->x,to->y); */

    FW_lineto (&ncontrol,user);
    FW_lineto (to,user);



    return 0;
}

int FW_cubicto (FT_Vector* control1, FT_Vector* control2, FT_Vector* to, void* user)
{
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;
    /* really ignore control points */
    if (p->TextVerbose)
        printf ("FW_cubicto\n");

    FW_lineto (control1, user);
    FW_lineto (control2, user);
    FW_lineto (to, user);
    return 0;
}


/* make up the font name */
void FW_make_fontname(int num) {
/*
    bit:    0       BOLD        (boolean)
    bit:    1       ITALIC      (boolean)
    bit:    2       SERIF
    bit:    3       SANS
    bit:    4       TYPEWRITER
    
    JAS - May 2005 - The Vera freely distributable ttf files
    are:

    Vera.ttf
    VeraMono.ttf
    VeraSeBd.ttf
    VeraSe.ttf
    VeraMoBI.ttf
    VeraMoIt.ttf
    VeraIt.ttf
    VeraMoBd.ttf
    VeraBd.ttf
    VeraBI.ttf
    
    The files that were included were copyright Bitstream;
    the Vera files are also from Bitstream, but are
    freely distributable. See the copyright file in the
    fonts directory.
*/

    ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;
    #ifdef HAVE_FONTCONFIG
    FcPattern *FW_fp=NULL;
    FcPattern *FW_fm=NULL;
    FcChar8 *FW_file=NULL;
    FcResult fcjunkresult;
    #else

    if (!p->font_directory) {
        printf("Internal error: no font directory.\n");
        return;
    }
    strcpy (p->thisfontname, p->font_directory);
    #endif

    switch (num) {
    case 0x04:			/* Serif */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"serif",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	#else
	strcat (p->thisfontname,"/VeraSe.ttf");
	#endif
	break; 
    case 0x05: 			/* Serif Bold */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"serif",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"bold");
	#else
	strcat (p->thisfontname,"/VeraSeBd.ttf");
	#endif
	break; 
    case 0x06:			/* Serif Ital */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"serif",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"italic");
	FcPatternAddString(FW_fp,FC_STYLE,"oblique");
	#else
	strcat (p->thisfontname,"/VeraSe.ttf");
	#endif
	break;
    case 0x07:			/* Serif Bold Ital */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"serif",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"bold italic");
	FcPatternAddString(FW_fp,FC_STYLE,"bold oblique");
	#else
	strcat (p->thisfontname,"/VeraSeBd.ttf");
	#endif
	break;
    case 0x08:			/* Sans */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"sans",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	#else
	strcat (p->thisfontname,"/Vera.ttf");
	#endif
	break;
    case 0x09: 			/* Sans Bold */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"sans",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"bold");
	#else
	strcat (p->thisfontname,"/VeraBd.ttf");
	#endif
	break; 
    case 0x0a: 			/* Sans Ital */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"sans",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"italic");
	FcPatternAddString(FW_fp,FC_STYLE,"oblique");
	#else
	strcat (p->thisfontname,"/VeraIt.ttf"); 
	#endif
	break; 
    case 0x0b: 			/* Sans Bold Ital */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"sans",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"bold italic");
	FcPatternAddString(FW_fp,FC_STYLE,"bold oblique");
	#else
	strcat (p->thisfontname,"/VeraBI.ttf"); 
	#endif
	break; 
    case 0x10:			/* Monospace */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"monospace",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	#else
	strcat (p->thisfontname,"/VeraMono.ttf");
	#endif
	break; 
    case 0x11: 			/* Monospace Bold */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"monospace",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"bold");
	#else
	strcat (p->thisfontname,"/VeraMoBd.ttf"); 
	#endif
	break; 
    case 0x12: /* Monospace Ital */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"monospace",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"italic");
	FcPatternAddString(FW_fp,FC_STYLE,"oblique");
	#else
	strcat (p->thisfontname,"/VeraMoIt.ttf"); 
	#endif
	break; 
    case 0x13: /* Monospace Bold Ital */
	#ifdef HAVE_FONTCONFIG
	FW_fp=FcPatternBuild(NULL,FC_FAMILY,FcTypeString,"monospace",FC_OUTLINE,FcTypeBool,FcTrue,NULL);
	FcPatternAddString(FW_fp,FC_STYLE,"bold italic");
	FcPatternAddString(FW_fp,FC_STYLE,"bold oblique");
	#else
	strcat (p->thisfontname,"/VeraMoBI.ttf"); 
	#endif
	break; 
    default:
	printf ("dont know how to handle font id %x\n",num);
	return;
    }

    #ifdef HAVE_FONTCONFIG
    FcConfigSubstitute(0,FW_fp,FcMatchPattern);
    FcDefaultSubstitute(FW_fp);
    if (!(FW_fm = FcFontMatch(0,FW_fp,&fcjunkresult))) {
	/* do whatever is done when no match found */
	printf ("could not find font for id %x\n",num);
    } else {
	if (FcPatternGetString(FW_fm,FC_FILE,0,&FW_file) != FcResultMatch) {
	    printf ("could not find font for id %x\n",num);
	} else {
	    /* strcpy didn't work, use strncpy and set the null character by hand */
	    strncpy(p->thisfontname,(char *)FW_file,strlen((char *)FW_file));
	    p->thisfontname[strlen((char *)FW_file)] = NULL;
	}
	FcPatternDestroy(FW_fm);
    }
    FcPatternDestroy(FW_fp);
    #endif
}

/* initialize the freetype library */
static int FW_init_face() 
{
    int err;
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;

#ifdef _ANDROID
        FT_Open_Args myArgs;

    if ((p->fileLen == 0) || (p->androidFontFile ==NULL)) {
	ConsoleMessage ("FW_init_face, fileLen and/or androidFontFile issue");
	return FALSE;

    }

#ifdef ANDROID_DEBUG
   {
	struct stat buf;
   	int fh,result;

ConsoleMessage ("TEXT INITIALIZATION - checking on the font file before doing anything");
   if (0 == fstat(fileno(p->androidFontFile), &buf)) {
      ConsoleMessage("TEXT INITIALIZATION file size is %ld\n", buf.st_size);
      ConsoleMessage("TEXT INITIALIZATION time modified is %s\n", ctime(&buf.st_atime));
   }

    } 
#endif //ANDROID_DEBUG


    // ConsoleMessage("FT_Open_Face looks ok to go");
	
    unsigned char *myFileData = malloc (p->fileLen+1);
    size_t frv;
    frv = fread (myFileData, (size_t)p->fileLen, (size_t)1, p->androidFontFile);
    myArgs.flags  = FT_OPEN_MEMORY;
    myArgs.memory_base = myFileData;
    myArgs.memory_size = p->fileLen;
  
    err = FT_Open_Face(p->library, &myArgs, 0, &p->font_face[p->myff]);
        if (err) {
            char line[2000];
            sprintf  (line,"FreeWRL - FreeType, can not set char size for font %s\n",p->thisfontname);
            ConsoleMessage(line);
            return FALSE;
        } else {
            p->font_opened[p->myff] = TRUE;
        }


#ifdef ANDROID_DEBUG
   {
        struct stat buf;
        int fh,result;

   if (0 == fstat(fileno(p->androidFontFile), &buf)) {
      ConsoleMessage("FIN TEXT INITIALIZATION file size is %ld\n", buf.st_size);
      ConsoleMessage("FIN TEXT INITIALIZATION time modified is %s\n", ctime(&buf.st_atime));
   }
}
#endif //ANDROID_DEBUG

	fclose(p->androidFontFile);
	p->androidFontFile = NULL;


#else //ANDROID
    /* load a font face */
    err = FT_New_Face(p->library, p->thisfontname, 0, &p->font_face[p->myff]);
#endif //ANDROID

    if (err) {
        printf ("FreeType - can not use font %s\n",p->thisfontname);
        return FALSE;
    } else {
        /* access face content */
        err = FT_Set_Char_Size(p->font_face[p->myff], /* handle to face object           */
                               POINTSIZE*64,    /* char width in 1/64th of points  */
                               POINTSIZE*64,    /* char height in 1/64th of points */
                               XRES,            /* horiz device resolution         */
                               YRES);           /* vert device resolution          */

        if (err) {
            printf ("FreeWRL - FreeType, can not set char size for font %s\n",p->thisfontname);
            return FALSE;
        } else {
            p->font_opened[p->myff] = TRUE;
        }
    }
    return TRUE;
}

/* calculate extent of a range of characters */
double FW_extent (int start, int length)
{
    int count;
    double ret = 0;
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;

    for (count = start; count <length+start; count++) {
        ret += p->glyphs[count]->advance.x >> 10;
    }
    return ret;
}

/* Load a character, a maximum of MAX_GLYPHS are here. Note that no
   line formatting is done here; this is just for pre-calculating
   extents, etc.

   NOTE: we store the handles to each glyph object for each
   character in the glyphs array
*/
FT_Error  FW_Load_Char(unsigned int idx)
{
    FT_Glyph  glyph;
    FT_UInt glyph_index;
    int error;
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;

    if (p->cur_glyph >= MAX_GLYPHS) {
        return 1;
    }

    /* retrieve glyph index from character code */
    glyph_index = FT_Get_Char_Index(p->font_face[p->myff],idx);

    /* loads the glyph in the glyph slot */

    error = FT_Load_Glyph(p->font_face[p->myff], glyph_index, FT_LOAD_DEFAULT) ||
        FT_Get_Glyph(p->font_face[p->myff]->glyph, &glyph);

    if (!error) { p->glyphs[p->cur_glyph++] = glyph; }
    return error;
}

static void FW_draw_outline (FT_OutlineGlyph oglyph)
{
    int thisptr = 0;
    int retval = 0;
	ppComponent_Text p;
	ttglobal tg = gglobal();
	p = (ppComponent_Text)tg->Component_Text.prv;

    /* gluTessBeginPolygon(global_tessobj,NULL); */

    FW_GLU_BEGIN_POLYGON(tg->Tess.global_tessobj);
    p->FW_Vertex = 0;

    /* thisptr may possibly be null; I dont think it is use in freetype */
    retval = FT_Outline_Decompose( &oglyph->outline, &p->FW_outline_interface, &thisptr);

    /* gluTessEndPolygon(global_tessobj); */
    FW_GLU_END_POLYGON(tg->Tess.global_tessobj);

    if (retval != FT_Err_Ok)
        printf("FT_Outline_Decompose, error %d\n",retval);
}

/* draw a glyph object */
static void FW_draw_character (FT_Glyph glyph)
{
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;
    if (glyph->format == ft_glyph_format_outline) {
        FW_draw_outline ((FT_OutlineGlyph) glyph);
        p->pen_x +=  (glyph->advance.x >> 10);
    } else {
        printf ("FW_draw_character; glyphformat  -- need outline for %s %s\n",
                p->font_face[p->myff]->family_name,p->font_face[p->myff]->style_name);
    }
    if (p->TextVerbose) printf ("done character\n");
}

/* UTF-8 to UTF-32 conversion - 
// x3d and wrl strings are supposed to be in UTF-8
// when drawing strings, FreeType will take a UTF-32"
// http://en.wikipedia.org/wiki/UTF-32/UCS-4
// conversion method options:
// libicu
//    - not used - looks big
//    - http://site.icu-project.org/   LIBICU  - opensource, C/C++ and java.
// mbstocws 
//    - win32 version didn't seem to do any converting
// iconv - gnu libiconv 
//    - http://gnuwin32.sourceforge.net/packages/libiconv.htm 
//    - the win32 version didn't run - bombs on open
// guru code:
//    - adopted - or at least the simplest parts
//    - not rigourous checking though - should draw bad characters if 
//      if you have it wrong in the file
//    - http://floodyberry.wordpress.com/2007/04/14/utf-8-conversion-tricks/
//    - not declared opensource, so we are using the general idea 
//      in our own code
*/
const unsigned int Replacement = ( 0xfffd );
const unsigned char UTF8TailLengths[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,4,4,4,4,5,5,0,0
};

unsigned int utf8_to_utf32_char(unsigned char *s, unsigned char *end, unsigned int *inc ) {
	unsigned int tail, i, c;
	c = (*s);
	s++;
	(*inc)++;
	if( c < 0x80 )
		return c; //regular ASCII
	tail = UTF8TailLengths[c];
	if( !tail  || (s + tail > end))
		return Replacement;

	//decoding loop
	c &= ( 0x3f >> tail );
	for(i=0;i<tail;++i) {
		if( (s[i] & 0xc0) != 0x80 )
			break;
		c = (c << 6) + (s[i] & 0x3f);
	}

	//s += i;
	(*inc) += i;
	if( i != tail )
		return Replacement;
	return c;
}

unsigned int *utf8_to_utf32(unsigned char *utf8string, unsigned int *len32)
{
	//does the UTF-8 to UTF-32 conversion
	//it allocates the unsigned int array it returns - please FREE() it
	//it adds an extra on the end and null-terminates
	//len32 is the length, not including the null/0 termination.
	unsigned int *to, *to0;
	unsigned char *start, *end;
	int lenchar, l32;
	lenchar = (int)strlen((const char *)utf8string);
	to0 = to = (unsigned int*)malloc((lenchar + 1)*sizeof(unsigned int));
	start = utf8string;
	end = (unsigned char *)&utf8string[lenchar];
	l32 = 0;
	while ( start < end ) {
		while ( ( *start < 0x80 ) && ( start < end ) ) {
			*to++ = *start++;
			l32++;
		}
		if ( start < end ) {
			unsigned int inc = 0;
			*to++ = utf8_to_utf32_char(start,end,&inc);
			start += inc;
			//start++; //done in above function
			l32++;
		}
	}
	to0[l32] = 0;
	*len32 = l32;
	return to0;
}
int utf8_to_utf32_bytes(unsigned char *s, unsigned char *end)
{
	unsigned int tail, c;
	int inc =0;
	c = (*s);
	s++;
	inc++;
	if( c < 0x80 )
		return 1; //regular ASCII 1 byte
	tail = UTF8TailLengths[c];
	tail = s + tail > end ? (unsigned int)end - (unsigned int)s : tail; //min(tail,end-s)
	return tail + 1;
}
int len_utf8(unsigned char *utf8string)
{
	unsigned char *start, *end;
	int lenchar, l32;
	lenchar = (int)strlen((const char *)utf8string);
	start = utf8string;
	end = (unsigned char *)&utf8string[lenchar];
	l32 = 0;
	while ( start < end ) {
		while ( ( *start < 0x80 ) && ( start < end ) ) {
			start++;
			l32++; //ASCII char
		}
		if ( start < end ) {
			start += utf8_to_utf32_bytes(start,end);
			l32++;
		}
	}
	return l32;
}


/* take a text string, font spec, etc, and make it into an OpenGL Polyrep.
   Note that the text comes EITHER from a SV (ie, from perl) or from a directstring,
   eg, for placing text on the screen from within FreeWRL itself */

void FW_rendertext(unsigned int numrows,struct Uni_String **ptr, char *directstring, 
                   unsigned int nl, double *length, double maxext, 
                   double spacing, double mysize, unsigned int fsparam,
                   struct X3D_PolyRep *rp)
{
    unsigned char *str = NULL; /* string pointer- initialization gets around compiler warning */
    unsigned int i,row;
    double shrink = 0;
    double rshrink = 0;
    int counter=0;
    int char_count=0;
    int est_tri=0;
	ppComponent_Text p;
	ttglobal tg = gglobal();
	p = (ppComponent_Text)tg->Component_Text.prv;

    /* fsparam has the following bitmaps:

    bit:    0       horizontal  (boolean)
    bit:    1       leftToRight (boolean)
    bit:    2       topToBottom (boolean)
    (style)
    bit:    3       BOLD        (boolean)
    bit:    4       ITALIC      (boolean)
    (family)
    bit:    5       SERIF
    bit:    6       SANS
    bit:    7       TYPEWRITER
    bit:    8       indicates exact font pointer (future use)
    (Justify - major)
    bit:    9       FIRST
    bit:    10      BEGIN
    bit:    11      MIDDLE
    bit:    12      END
    (Justify - minor)
    bit:    13      FIRST
    bit:    14      BEGIN
    bit:    15      MIDDLE
    bit:    16      END

    bit: 17-31      spare
    */

    /* z distance for text - only the status bar has anything other than 0.0 */
    if (directstring) {
#ifdef CALCAULATEANGLETAN
        float angletan;
        /* convert fieldofview into radians */
        angletan = fieldofview / 360.0f * PI * 2;

        /* take half of the angle; */
        angletan = angletan / 2.0f;

        /* find the tan of it; */
        angletan = tanf (angletan);

        /* and, divide the "general" text size by it */
        p->TextZdist = -0.010/angletan;
        //printf ("fov %f tzd %f \n",(float) fieldofview, (float) p->TextZdist);
#else
        /* the equation should be simple, but it did not work. Lets try the following: */
        if (Viewer()->fieldofview < 12.0f) {
            p->TextZdist = -12.0f;
        } else if (Viewer()->fieldofview < 46.0f) {
            p->TextZdist = -0.2f;
        } else if (Viewer()->fieldofview  < 120.0f) {
            p->TextZdist = +2.0f;
        } else {
            p->TextZdist = + 2.88f;
        }
#endif
    } else {
        p->TextZdist = 0.0f;
    }

    /* have we done any rendering yet */
    /* do we need to call open font? */
    if (!p->started) {
        if (open_font()) {
            p->started = TRUE;
        } else {
            printf ("Could not find System Fonts for Text nodes\n");
            return;
        }
    }

    if (p->TextVerbose) 
        printf ("entering FW_Render_text \n");


    p->FW_rep_ = rp;

    p->FW_RIA_indx = 0;            /* index into FW_RIA                                  */
    p->FW_pointctr=0;              /* how many points used so far? maps into rep-_coord  */
    p->indx_count=0;               /* maps intp FW_rep_->cindex                          */
    p->contour_started = FALSE;

    p->pen_x = 0.0; p->pen_y = 0.0;
    p->cur_glyph = 0;
    p->x_size = mysize;            /* global variable for size */
    p->y_size = mysize;            /* global variable for size */

    /* is this font opened */
    p->myff = (fsparam >> 3) & 0x1F;

#if defined (ANDROID)
// Android - for now, all fonts are identical
p->myff = 4;
#endif

    if (p->myff <4) {
        /* we dont yet allow externally specified fonts, so one of
           the font style bits HAS to be set. If there was no FontStyle
           node, this will be blank, so... */
        p->myff = 4;
    }

    if (!p->font_opened[p->myff]) {
        FW_make_fontname(p->myff);
        if (!FW_init_face()) {
            /* tell this to render as fw internal font */
            FW_make_fontname (0);
            FW_init_face();
        }
    }

    /* type 1 fonts different than truetype fonts */
    if (p->font_face[p->myff]->units_per_EM != 1000)
        p->x_size = p->x_size * p->font_face[p->myff]->units_per_EM/1000.0;

    /* if we have a direct string, then we only have ONE, so initialize it here */
    if (directstring != 0) str = (unsigned char *)directstring;

    /* load all of the characters first... */
    for (row=0; row<numrows; row++) {
        if (directstring == 0) 
            str = (unsigned char *)ptr[row]->strptr;
		if(0){
			for(i=0; i<strlen((const char *)str); i++) {
					FW_Load_Char(str[i]);
				char_count++;
			}
		}else{
			/* utf8_to_utf32 */
			unsigned int len32;
			unsigned int *utf32;
			utf32 = utf8_to_utf32(str,&len32);
			for(i=0;i<len32;i++)
				FW_Load_Char(utf32[i]);
			char_count += len32;
			free(utf32);

		}
    }

    if (p->TextVerbose) {
        printf ("Text: rows %d char_count %d\n",numrows,char_count);
    }

    /* what is the estimated number of triangles? assume a certain number of tris per char */
    est_tri = char_count*800; /* 800 was TESS_MAX_COORDS - REALLOC if needed */
    p->coordmaxsize=est_tri;
    p->cindexmaxsize=est_tri;
    p->FW_rep_->cindex=MALLOC(GLuint *, sizeof(*(p->FW_rep_->cindex))*est_tri);
    p->FW_rep_->actualCoord = MALLOC(float *, sizeof(*(p->FW_rep_->actualCoord))*est_tri*3);

    if(maxext > 0) {
        double maxlen = 0;
        double l;
        int counter = 0;

        for(row = 0; row < numrows; row++) {
			int lenchars = 0;
            if (directstring == 0) str = (unsigned char *)ptr[row]->strptr;
			if(0){
				lenchars = (int)strlen((const char *)str);
			}else{
				/* utf8_to_utf32 */
				lenchars = len_utf8(str);
			}
            l = FW_extent(counter,(int) lenchars);
            counter += (int) lenchars;
            if(l > maxlen) {maxlen = l;}
        }

        if(maxlen > maxext) {shrink = maxext / OUT2GL(maxlen);}
    }

    /* topToBottom */
    if (TOPTOBOTTOM) {
        spacing =  -spacing;  /* row increment */
        p->pen_y = 0.0;
    } else {
        p->pen_y -= numrows-1;
    }

    /* leftToRight */
    if (LEFTTORIGHT) {
        FW_GL_ROTATE_D(180.0,0.0,1.0,0.0);
    }


    for(row = 0; row < numrows; row++) {
		unsigned int lenchars;
        double rowlen;

        if (directstring == 0) str = (unsigned char *)ptr[row]->strptr;
        if (p->TextVerbose)
            printf ("text2 row %d :%s:\n",row, str);
        p->pen_x = 0.0;
        rshrink = 0.0;
		if(0)
			lenchars = (int)strlen((const char *)str);
		else
			lenchars = len_utf8(str); 
        rowlen = FW_extent(counter,lenchars);
        if((row < nl) && (APPROX(length[row],0.0))) {
            rshrink = length[row] / OUT2GL(rowlen);
        }
        if(shrink>0.0001) { FW_GL_SCALE_D(shrink,1.0,1.0); }
        if(rshrink>0.0001) { FW_GL_SCALE_D(rshrink,1.0,1.0); }

        /* Justify, FIRST, BEGIN, MIDDLE and END */

        /* MIDDLE */
        if (fsparam & 0x800) { p->pen_x = -rowlen/2.0; }

        /* END */
        if ((fsparam & 0x1000) && (fsparam & 0x01)) {
            /* printf ("rowlen is %f\n",rowlen); */
            p->pen_x = -rowlen;
        }



        for(i=0; i<lenchars; i++) {
            /* FT_UInt glyph_index; */
            /* int error; */
            int x;

            tg->Tess.global_IFS_Coord_count = 0;
            p->FW_RIA_indx = 0;


            FW_draw_character (p->glyphs[counter+i]);


            FT_Done_Glyph (p->glyphs[counter+i]);



            /* copy over the tesselated coords for the character to
             * the rep structure */

            for (x=0; x<tg->Tess.global_IFS_Coord_count; x++) {
                 /*printf ("copying %d\n",global_IFS_Coords[x]); */

                /* did the tesselator give us back garbage? */

                if ((tg->Tess.global_IFS_Coords[x] >= p->cindexmaxsize) ||
                    (p->indx_count >= p->cindexmaxsize) ||
                    (tg->Tess.global_IFS_Coords[x] < 0)) {
                     if (p->TextVerbose)  
                     printf ("Tesselated index %d out of range; skipping indx_count, %d cindexmaxsize %d global_IFS_Coord_count %d\n", 
                     tg->Tess.global_IFS_Coords[x],p->indx_count,p->cindexmaxsize,tg->Tess.global_IFS_Coord_count); 
                    /* just use last point - this sometimes happens when */
                    /* we have intersecting lines. Lets hope first point is */
                    /* not invalid... JAS */
                    p->FW_rep_->cindex[p->indx_count] = p->FW_rep_->cindex[p->indx_count-1];
                    if (p->indx_count < (p->cindexmaxsize-1)) p->indx_count ++;
                } else {
					/*
                    printf("global_ifs_coords is %d indx_count is %d \n",global_IFS_Coords[x],p->indx_count); 
                    printf("filling up cindex; index %d now points to %d\n",p->indx_count,global_IFS_Coords[x]); 
					*/
                    p->FW_rep_->cindex[p->indx_count++] = tg->Tess.global_IFS_Coords[x];
                }
            }

            if (p->indx_count > (p->cindexmaxsize-400)) {
                p->cindexmaxsize += 800; /* 800 was TESS_MAX_COORDS; */
                p->FW_rep_->cindex=(GLuint *)REALLOC(p->FW_rep_->cindex,sizeof(*(p->FW_rep_->cindex))*p->cindexmaxsize);
            }
        }
        counter += lenchars;

        p->pen_y += spacing * p->y_size;
    }
    /* save the triangle count (note, we have a "vertex count", not a "triangle count" */
    p->FW_rep_->ntri=p->indx_count/3;



    /* set these variables so they are not uninitialized */
    p->FW_rep_->ccw=FALSE;

    /* if indx count is zero, DO NOT get rid of MALLOCd memory - creates a bug as pointers cant be null */
    if (p->indx_count !=0) {
        /* REALLOC bug in linux - this causes the pointers to be eventually lost... */
        /* REALLOC (p->FW_rep_->cindex,sizeof(*(p->FW_rep_->cindex))*p->indx_count); */
        /* REALLOC (p->FW_rep_->actualCoord,sizeof(*(p->FW_rep_->actualCoord))*p->FW_pointctr*3); */
    }

    /* now, generate normals */
    p->FW_rep_->normal = MALLOC(float *, sizeof(*(p->FW_rep_->normal))*p->indx_count*3);
    for (i = 0; i<(unsigned int)p->indx_count; i++) {
        p->FW_rep_->normal[i*3+0] = 0.0f;
        p->FW_rep_->normal[i*3+1] = 0.0f;
        p->FW_rep_->normal[i*3+2] = 1.0f;
    }

    /* do we have texture mapping to do? */
    if (HAVETODOTEXTURES) {
        p->FW_rep_->GeneratedTexCoords = MALLOC(float *, sizeof(*(p->FW_rep_->GeneratedTexCoords))*(p->FW_pointctr+1)*3);
        /* an attempt to try to make this look like the NIST example */
        /* I can't find a standard as to how to map textures to text JAS */
        for (i=0; i<(unsigned int)p->FW_pointctr; i++) {
            p->FW_rep_->GeneratedTexCoords[i*3+0] = p->FW_rep_->actualCoord[i*3+0]*1.66f;
            p->FW_rep_->GeneratedTexCoords[i*3+1] = 0.0f;
            p->FW_rep_->GeneratedTexCoords[i*3+2] = p->FW_rep_->actualCoord[i*3+1]*1.66f;
        }
    }



    if (p->TextVerbose) printf ("exiting FW_Render_text\n");
}

int open_font() 
{
    int len;
    int err;
	ppComponent_Text p = (ppComponent_Text)gglobal()->Component_Text.prv;

    if (p->TextVerbose)
        printf ("open_font called\n");

    p->FW_outline_interface.move_to = (FT_Outline_MoveTo_Func)FW_moveto;
    p->FW_outline_interface.line_to = (FT_Outline_LineTo_Func)FW_lineto;
    p->FW_outline_interface.conic_to = (FT_Outline_ConicTo_Func)FW_conicto;
    p->FW_outline_interface.cubic_to = (FT_Outline_CubicTo_Func)FW_cubicto;
    p->FW_outline_interface.shift = 0;
    p->FW_outline_interface.delta = 0;

#ifndef _ANDROID

#ifndef HAVE_FONTCONFIG
    /* where are the fonts stored? */
    p->font_directory = makeFontDirectory();
	ConsoleMessage("font directory=%s\n",p->font_directory);
    /* were fonts not found? */
    if (p->font_directory == NULL) {
#ifdef AQUA
        ConsoleMessage ("No Fonts; this should not happen on OSX computers; contact FreeWRL team\n");
#else
        ConsoleMessage ("No Fonts; check the build parameter --with-fontsdir, or set FREEWRL_FONTS_DIR environment variable\n");
#endif
        return FALSE;
    }
#endif //HAVE_FONTCONFIG
#endif //ANDROID

    /* lets initialize some things */
    for (len = 0; len < num_fonts; len++) {
        p->font_opened[len] = FALSE;
    }

    if ((err = FT_Init_FreeType(&p->library))) {
        fprintf(stderr, "FreeWRL FreeType Initialize error %d\n",err);
        return FALSE;
    }

    return TRUE;
}

int avatarCollisionVolumeIntersectMBBf(double *modelMatrix, float *minVals, float *maxVals);

void collide_Text (struct X3D_Text *node)
{
	ttglobal tg = gglobal();
    GLDOUBLE awidth = tg->Bindable.naviinfo.width; /*avatar width*/
    GLDOUBLE atop = tg->Bindable.naviinfo.width; /*top of avatar (relative to eyepoint)*/
    GLDOUBLE abottom = -tg->Bindable.naviinfo.height; /*bottom of avatar (relative to eyepoint)*/
    GLDOUBLE astep = -tg->Bindable.naviinfo.height+tg->Bindable.naviinfo.step;
    GLDOUBLE modelMatrix[16];
    //GLDOUBLE upvecmat[16];


    /*JAS - normals are always this way - helps because some
      normal calculations failed because of very small triangles
      which made collision calcs fail, which moved the Viewpoint...
      so, if there is no need to calculate normals..., why do it? */
    struct point_XYZ delta = {0,0,-1};
    struct X3D_PolyRep pr;
    int change = 0;

    /* JAS - first pass, intern is probably zero */
    if (node->_intern == NULL) return;

    /* JAS - no triangles in this text structure */
    if (node->_intern->ntri == 0) return;

    /*save changed state.*/
    if (node->_intern)
        change = node->_intern->irep_change;

    COMPILE_POLY_IF_REQUIRED(NULL, NULL, NULL, NULL);

    if (node->_intern)
        node->_intern->irep_change = change;

    /* restore changes state, invalidates compile_polyrep work done, so it can be done
       correclty in the RENDER pass */

    pr = *(node->_intern);

    /* do the triangle test again, now that we may have compiled the node. */
    if (pr.ntri == 0) {
        /* printf ("TRIANGLE NOW HAS ZERO NODES...\n"); */
        return;
    }

    FW_GL_GETDOUBLEV(GL_MODELVIEW_MATRIX, modelMatrix);

	matmultiply(modelMatrix,modelMatrix,FallInfo()->avatar2collision); 
	//dug9july2011 matmultiply(modelMatrix,FallInfo()->avatar2collision,modelMatrix); 

	if(!avatarCollisionVolumeIntersectMBBf(modelMatrix,pr.minVals,pr.maxVals) )return;
    delta = planar_polyrep_disp(abottom,atop,astep,awidth,pr,modelMatrix,PR_DOUBLESIDED,delta); 
    /* delta used as zero */

    vecscale(&delta,&delta,-1);

    accumulate_disp(CollisionInfo(),delta);

#ifdef COLLISIONVERBOSE
    if((fabs(delta.x) != 0. || fabs(delta.y) != 0. || fabs(delta.z) != 0.)) {
        fprintf(stderr,"COLLISION_TXT: (%f %f %f) (%f %f %f)\n",
                t_orig.x, t_orig.y, t_orig.z,
                delta.x, delta.y, delta.z);
    }
#endif
}

void make_Text (struct X3D_Text *node) 
{
    struct X3D_PolyRep *rep_ = node->_intern;
    double spacing = 1.0;
    double size = 1.0;
    unsigned int fsparams = 0;

    /* We need both sides */
    DISABLE_CULL_FACE;

    if (node->fontStyle) {
        /* We have a FontStyle. Parse params (except size and spacing) and
           make up an unsigned int with bits indicating params, to be
           passed to the Text Renderer
           
           bit:    0       horizontal  (boolean)
           bit:    1       leftToRight (boolean)
           bit:    2       topToBottom (boolean)
           (style)
           bit:    3       BOLD        (boolean)
           bit:    4       ITALIC      (boolean)
           (family)
           bit:    5       SERIF
           bit:    6       SANS
           bit:    7       TYPEWRITER
           bit:    8       indicates exact font pointer (future use)
           (Justify - major)
           bit:    9       FIRST
           bit:    10      BEGIN
           bit:    11      MIDDLE
           bit:    12      END
           (Justify - minor)
           bit:    13      FIRST
           bit:    14      BEGIN
           bit:    15      MIDDLE
           bit:    16      END

           bit: 17-31      spare
        */

        struct X3D_FontStyle *fsp;
        unsigned char *lang;
        unsigned char *style;
        struct Multi_String family;
        struct Multi_String justify;
        int tmp; int tx;
        struct Uni_String **svptr;
        unsigned char *stmp;

        /* step 0 - is the FontStyle a proto? */
        POSSIBLE_PROTO_EXPANSION(struct X3D_FontStyle *, node->fontStyle,fsp);

        /* fsp = (struct X3D_FontStyle *)node->fontStyle; */
        if (fsp->_nodeType != NODE_FontStyle) {
            ConsoleMessage ("Text node has FontStyle of %s",stringNodeType(fsp->_nodeType));
            node->fontStyle = NULL; /* stop dumping these messages */
        }

        /* step 0.5 - now that we know FontStyle points ok, go for
         * the other pointers */
        lang = (unsigned char *)fsp->language->strptr;
        style = (unsigned char *)fsp->style->strptr;

        family = fsp->family;
        justify = fsp->justify;

        /* Step 1 - record the spacing and size, for direct use */
        spacing = fsp->spacing;
        size = fsp->size;

        /* Step 2 - do the SFBools */
        fsparams = (fsp->horizontal)|(fsp->leftToRight<<1)|(fsp->topToBottom<<2);

        /* Step 3 - the SFStrings - style and language */
        /* actually, language is not parsed yet */

        if (strlen((const char *)style)) {
            if (!strcmp((const char *)style,"ITALIC")) {fsparams |= 0x10;}
            else if(!strcmp((const char *)style,"BOLD")) {fsparams |= 0x08;}
            else if (!strcmp((const char *)style,"BOLDITALIC")) {fsparams |= 0x18;}
            else if (strcmp((const char *)style,"PLAIN")) {
                printf ("Warning - FontStyle style %s  assuming PLAIN\n",style);}
        }
        if (strlen((const char *)lang)) {
            printf ("Warning - FontStyle - language param unparsed\n");
        }


        /* Step 4 - the MFStrings now. Family, Justify. */
        /* family can be blank, or one of the pre-defined ones. Any number of elements */

        svptr = family.p;
        for (tmp = 0; tmp < family.n; tmp++) {
            stmp = (unsigned char *)svptr[tmp]->strptr;
            if (strlen((const char *)stmp) == 0) {fsparams |=0x20; }
            else if (!strcmp((const char *)stmp,"SERIF")) { fsparams |= 0x20;}
            else if(!strcmp((const char *)stmp,"SANS")) { fsparams |= 0x40;}
            else if (!strcmp((const char *)stmp,"TYPEWRITER")) { fsparams |= 0x80;}
            /* else { printf ("Warning - FontStyle family %s unknown\n",stmp);}*/
        }

        svptr = justify.p;
        tx = justify.n;
        /* default is "BEGIN" "FIRST" */
        if (tx == 0) { fsparams |= 0x2400; }
        else if (tx == 1) { fsparams |= 0x2000; }
        else if (tx > 2) {
            printf ("Warning - FontStyle, max 2 elements in Justify\n");
            tx = 2;
        }

        for (tmp = 0; tmp < tx; tmp++) {
            stmp = (unsigned char *)svptr[tmp]->strptr;
            if (strlen((const char *)stmp) == 0) {
                if (tmp == 0) {
                    fsparams |= 0x400;
                } else {
                    fsparams |= 0x2000;
                }
            }
            else if (!strcmp((const char *)stmp,"FIRST")) { fsparams |= (0x200<<(tmp*4));}
            else if(!strcmp((const char *)stmp,"BEGIN")) { fsparams |= (0x400<<(tmp*4));}
            else if (!strcmp((const char *)stmp,"MIDDLE")) { fsparams |= (0x800<<(tmp*4));}
            else if (!strcmp((const char *)stmp,"END")) { fsparams |= (0x1000<<(tmp*4));}
            /* else { printf ("Warning - FontStyle family %s unknown\n",stmp);}*/
        }
    } else {
        /* send in defaults */
        fsparams = 0x2427;
    }

    /*  do the Text parameters, guess at the number of triangles required*/
    rep_->ntri = 0;

    /* 
       printf ("Text, calling FW_rendertext\n");
       call render text - NULL means get the text from the string 
    */

    FW_rendertext(((node->string).n),((node->string).p),NULL,
                  ((node->length).n),(double *) ((node->length).p),
                  (node->maxExtent),spacing,size,fsparams,rep_);

}
