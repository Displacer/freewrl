/*
=INSERT_TEMPLATE_HERE=

$Id: Snapshot.c,v 1.32 2013/10/25 21:07:05 crc_canada Exp $

CProto ???

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

#if !defined(FRONTEND_DOES_SNAPSHOTS)

#include <system.h>
#include <display.h>
#include <internal.h>

#include <libFreeWRL.h>

#include "../vrml_parser/Structs.h"
#include "headers.h"
#include "../vrml_parser/CParseGeneral.h"
#include "../world_script/jsUtils.h"
#include "../world_script/CScripts.h"
#include "Snapshot.h"
#include "../scenegraph/Collision.h"
#include "../scenegraph/quaternion.h"
#include "../scenegraph/Viewer.h"
#include "../input/SensInterps.h"
#include "../x3d_parser/Bindable.h"

#if HAVE_DIRENT_H
# include <dirent.h>
#endif


///* snapshot stuff */
//int snapRawCount=0;
//int snapGoodCount=0;
//
//#if defined(DOSNAPSEQUENCE)
///* need to re-implement this for OSX generating QTVR */
//int snapsequence=FALSE;		/* --seq - snapshot sequence, not single click  */
//int maxSnapImages=100; 		/* --maximg command line parameter 		*/
//char *snapseqB = NULL;		/* --seqb - snap sequence base filename		*/
//#endif
//
//int snapGif = FALSE;		/* --gif save as an animated GIF, not mpg	*/
//char *snapsnapB = NULL;		/* --snapb -single snapshot files		*/
//const char default_seqtmp[] = "freewrl_tmp"; /* default value for seqtmp        */
//char *seqtmp = NULL;		/* --seqtmp - directory for temp files		*/
//int doSnapshot = FALSE;		/* are we doing a snapshot?			*/
//int doPrintshot = FALSE; 	/* are we taking a snapshot in order to print? */
//int savedSnapshot = FALSE;


typedef struct pSnapshot{
/* snapshot stuff */
int snapRawCount;//=0;
int snapGoodCount;//=0;

#if defined(DOSNAPSEQUENCE)
/* need to re-implement this for OSX generating QTVR */
int snapsequence;//=FALSE;		/* --seq - snapshot sequence, not single click  */
int maxSnapImages;//=100; 		/* --maximg command line parameter 		*/
char *snapseqB;// = NULL;		/* --seqb - snap sequence base filename		*/
#endif

int snapGif;// = FALSE;		/* --gif save as an animated GIF, not mpg	*/
char *snapsnapB;// = NULL;		/* --snapb -single snapshot files		*/
const char *default_seqtmp;// = "freewrl_tmp"; /* default value for seqtmp        */
char *seqtmp;// = NULL;		/* --seqtmp - directory for temp files		*/
int doSnapshot;// = FALSE;		/* are we doing a snapshot?			*/
int doPrintshot;// = FALSE; 	/* are we taking a snapshot in order to print? */
int savedSnapshot;// = FALSE;
int modeTesting; //when generating test fixtures and playback with commandline -R,-F,-P - for linux just save .rgb don't convert image
}* ppSnapshot;
void* Snapshot_constructor()
{
	ppSnapshot p = (ppSnapshot)malloc(sizeof(struct pSnapshot));
	memset(p,0,sizeof(struct pSnapshot));
/* snapshot stuff */
p->snapRawCount=0;
p->snapGoodCount=0;

#if defined(DOSNAPSEQUENCE)
/* need to re-implement this for OSX generating QTVR */
p->snapsequence=FALSE;		/* --seq - snapshot sequence, not single click  */
p->maxSnapImages=100; 		/* --maximg command line parameter 		*/
p->snapseqB = NULL;		/* --seqb - snap sequence base filename		*/
#endif

p->snapGif = FALSE;		/* --gif save as an animated GIF, not mpg	*/
p->snapsnapB = NULL;		/* --snapb -single snapshot files		*/
p->default_seqtmp = "freewrl_tmp"; /* default value for seqtmp        */
p->seqtmp = NULL;		/* --seqtmp - directory for temp files		*/
p->doSnapshot = FALSE;		/* are we doing a snapshot?			*/
p->doPrintshot = FALSE; 	/* are we taking a snapshot in order to print? */
p->savedSnapshot = FALSE;
p->modeTesting = FALSE;
return (void*)p;
}
//void Snapshot_destructor(void *t)
//{
//	struct pSnapshot* tt = (struct tSnapshot*)t;
//	free(tt);
//}
void Snapshot_init(struct tSnapshot* t)
{
	t->prv = Snapshot_constructor();
	t->doSnapshot = FALSE;
}
//bool do_Snapshot(){
//	if( ((struct tSnapshot*)(gglobal()->Snapshot))->doSnapshot )return true;
//	return false;
//}
void set_snapsequence(int on)
{
#ifdef DOSNAPSEQUENCE
	//struct pSnapshot* p = (struct pSnapshot*)gglobal()->Snapshot.prv;
	ppSnapshot p = (ppSnapshot)gglobal()->Snapshot.prv;
	p->snapsequence = on;
#endif
}
#ifdef DOSNAPSEQUENCE
/* need to re-implement this for OSX generating QTVR */
void saveSnapSequence();
#endif
void set_snapshotModeTesting(int value)
{
	ppSnapshot p = (ppSnapshot)gglobal()->Snapshot.prv;
	p->modeTesting = value;
}
int isSnapshotModeTesting()
{
	struct tSnapshot* t = &gglobal()->Snapshot;
	struct pSnapshot* p = (struct pSnapshot*)t->prv;
	return p->modeTesting;
}

void fwl_set_SeqFile(const char* file)
{
#if defined(DOSNAPSEQUENCE)
    /* need to re-implement this for OSX generating QTVR */
	//struct pSnapshot* p = (struct pSnapshot*)gglobal()->Snapshot.prv;
	ppSnapshot p = (ppSnapshot)gglobal()->Snapshot.prv;
    p->snapseqB = strdup(file);
    printf("snapseqB is %s\n", p->snapseqB);
#else
    WARN_MSG("Call to fwl_set_SeqFile when Snapshot Sequence not compiled in.\n");
#endif
}

void fwl_set_SnapFile(const char* file)
{
	ppSnapshot p = (ppSnapshot)gglobal()->Snapshot.prv;

	p->snapsnapB = strdup(file);
    TRACE_MSG("snapsnapB set to %s\n", p->snapsnapB);
	printf("%s\n",p->snapsnapB);
}

void fwl_set_MaxImages(int max)
{
#if defined(DOSNAPSEQUENCE)
    /* need to re-implement this for OSX generating QTVR */
	//struct pSnapshot* p = (struct pSnapshot*)gglobal()->Snapshot.prv;
	ppSnapshot p = (ppSnapshot)gglobal()->Snapshot.prv;

    if (max <=0)
	max = 100;
    p->maxSnapImages = max;
#else
    WARN_MSG("Call to fwl_set_MaxImages when Snapshot Sequence not compiled in.\n");
#endif
}
//typedef struct tSnapshot* ttSnapshot;
//typedef struct pSnapshot* ppSnapshot;
//#define TSNAPSHOT &gglobal()->Snapshot
//#define PSNAPSHOT (ppSnapshot)&gglobal()->Snapshot.prv
//typedef tglobal* ttglobal;
void fwl_set_SnapTmp(const char* file)
{
	{
		ttglobal tg = gglobal();
		tg->Snapshot.doSnapshot = FALSE;
		//{
		//	((ppSnapshot)tg->Snapshot.prv)->seqtmp = strdup(file);
		//}
		{
			ppSnapshot p = (ppSnapshot)tg->Snapshot.prv;
			p->seqtmp = strdup(file);
			TRACE_MSG("seqtmp set to %s\n", p->seqtmp);
		}

	}
	//{
	//	ttSnapshot t = TSNAPSHOT;
	//	ppSnapshot p = PSNAPSHOT;
	//	t->doSnapshot = FALSE;
	//	p->seqtmp = strdup(file);
	//	TRACE_MSG("seqtmp set to %s\n", p->seqtmp);
	//}
	//{
	//	struct tSnapshot* t = &gglobal()->Snapshot;
	//	struct pSnapshot* p = (struct pSnapshot*)t->prv;
	//	p->seqtmp = strdup(file);
	//	TRACE_MSG("seqtmp set to %s\n", p->seqtmp);
	//}
	//{
	//	struct pSnapshot* p = (struct pSnapshot*)gglobal()->Snapshot.prv;
	//	p->seqtmp = strdup(file);
	//	TRACE_MSG("seqtmp set to %s\n", p->seqtmp);
	//}
}



#ifdef _MSC_VER
static char * grabScreen(int bytesPerPixel, int x, int y, int width, int height)
{
	/* copies opengl window pixels into a buffer */
	int pixelType;
	char *buffer;
	if(bytesPerPixel == 3) pixelType = GL_RGB;
	if(bytesPerPixel == 4) pixelType = GL_RGBA;
	buffer = MALLOC (GLvoid *, bytesPerPixel*width*height*sizeof(char));

	/* grab the data */
	FW_GL_PIXELSTOREI (GL_UNPACK_ALIGNMENT, 1);
	FW_GL_PIXELSTOREI (GL_PACK_ALIGNMENT, 1);
	
	FW_GL_READPIXELS (x,y,width,height,pixelType,GL_UNSIGNED_BYTE, buffer);
	return buffer;
}
#endif //_MSC_VER

#if defined( _MSC_VER) || defined (IPHONE)
/* stubbs for now */
void setSnapshot() {}
void fwl_toggleSnapshot(){}
void fwl_init_SnapGif(){}
void saveSnapSequence() {}
#endif

#ifdef IPHONE
void Snapshot () {}
#endif

#ifndef IPHONE

//#ifndef _MSC_VER
//#include <windows.h>
//#define FDWORD DWORD
//#define FLONG LONG
//#define FWORD WORD
//#define FBYPTE BYTE
//#define FBI_RGB BI_RGB
//#define FWBITMAPINFOHEADER BITMAPINFOHEADER
//#define FWBITMAPFILEHEADER BITMAPFILEHEADER
//#define FWBITMAPINFO BITMAPINFO
//#else
////#include <windows.h>
#define FDWORD unsigned long
#define FLONG long
#define FWORD unsigned short
#define FBYTE unsigned char
#define FBI_RGB        0L

typedef struct {
        FDWORD      biSize;
        FLONG       biWidth;
        FLONG       biHeight;
        FWORD       biPlanes;
        FWORD       biBitCount;
        FDWORD      biCompression;
        FDWORD      biSizeImage;
        FLONG       biXPelsPerMeter;
        FLONG       biYPelsPerMeter;
        FDWORD      biClrUsed;
        FDWORD      biClrImportant;
} FWBITMAPINFOHEADER;
//#include <pshpack2.h> //puspack and poppack are all to fix the WORD bfType struct 4-byte alignment problem
//I just took bfType out, then I don't need special packing on a 32bit system. Not sure about 64.
typedef struct  {
        //FWORD    bfType;
        FDWORD   bfSize;
        FWORD    bfReserved1;
        FWORD    bfReserved2;
        FDWORD   bfOffBits;
} FWBITMAPFILEHEADER;
//#include <poppack.h>
typedef struct {
        FBYTE    rgbBlue;
        FBYTE    rgbGreen;
        FBYTE    rgbRed;
        FBYTE    rgbReserved;
} FWRGBQUAD;

typedef struct {
    FWBITMAPINFOHEADER    bmiHeader;
    FWRGBQUAD             bmiColors[1];
} FWBITMAPINFO;
//#endif

static void fromLong(unsigned long  myword, char *buffer)
{
  buffer[0] = (myword & 0x000000ff) >> 0;
  buffer[1] = (myword & 0x0000ff00) >> 8;
  buffer[2] = (myword & 0x00ff0000) >> 16;
  buffer[3] = (myword & 0xff000000) >> 24;
}

static void fromShort(unsigned short  myword, char *buffer)
{
  buffer[0] = (myword & 0x00ff) >> 0;
  buffer[1] = (myword & 0xff00) >> 8;
}


//#include "Vfw.h" //.avi headers
void saveSnapshotBMP(char *pathname, char *buffer,int bytesPerPixel,int width, int height)
{
	//tested for bytesPerPixel == 3 and incoming alignment 1 only
	//(outgoing/written is byte-aligned 4)
	int rowlength, extra, alignedwidth, i;

	FWBITMAPINFOHEADER bi; 
	FWORD    bfType;
	FWBITMAPFILEHEADER bmph;
	char filler[3] = {'\0','\0','\0'};
	FILE *fout;

	//fname = "freewrl_snapshot.bmp";
	//if(p->snapsnapB) fname = p->snapsnapB;
	fout = fopen(pathname,"w+b");

	if(bytesPerPixel == 3) bi.biCompression = FBI_RGB;
	bi.biHeight = height;
	bi.biWidth = width;
	bi.biPlanes = 1;
	bi.biBitCount  = 8 * bytesPerPixel;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	//bi.biSizeImage = bytesPerPixel*bi.biHeight*bi.biWidth;
 // The width must be DWORD (4byte) aligned unless the bitmap is RLE 
    // compressed.
	rowlength = width*bytesPerPixel;
	extra = 4 - (rowlength % 4);
	if(extra == 4) extra = 0;
	alignedwidth = rowlength + extra;
    //bi.biSizeImage = ((bi.biWidth * 8*bytesPerPixel +31) & ~31) /8
    //                              * bi.biHeight; 
	bi.biSizeImage = alignedwidth * height;
	bi.biSize = sizeof(bi);
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;
	//printf("width=%d height=%d rowlengthmod4= %d extra=%d\n",width,height,rowlength%4,extra);
	if(0){
		//problem 1 - 64bit compiler may pad struct to 8 bytes
		//problem 2 - we may be writing on a big-endian machine, .bmp numbers should be little endian
		//memcpy(&bmph.bfType,"BM",2);
		memcpy(&bfType,"BM",2);
		bmph.bfReserved1 = 0;
		bmph.bfReserved2 = 0;
		bmph.bfOffBits = sizeof(bfType) + sizeof(FWBITMAPFILEHEADER) + sizeof(FWBITMAPINFOHEADER); 
		bmph.bfSize = sizeof(bfType) + sizeof(FWBITMAPFILEHEADER) + sizeof(FWBITMAPINFOHEADER) + bi.biSizeImage;
		fwrite(&bfType,sizeof(bfType),1,fout);
		fwrite(&bmph,sizeof(FWBITMAPFILEHEADER),1,fout);
		fwrite(&bi,sizeof(FWBITMAPINFO),1,fout); //this is wrong. I'm writing 4 bytes for a color map I don't need to
		if(true) //reverse colors
		{
			//swap GRB TO RGB //this is wrong because I wrote out 4 extra bytes above
			int i;
			char c;
			for(i=0;i<rowlength*height;i+=3)
			{
				c = buffer[i];
				buffer[i] = buffer[i+1];
				buffer[i+1] = c;
			}
		}
	}
	if(1){
		//solution 1: write each variable separately to buffer to elliminate struct padding
		//solution 2: convert any-endian to little-endian with byte-reordering functions
		char buf[128];
		fwrite("BM",2,1,fout);
		bi.biSize = 40; //9 longs x 4byte + 2 shorts x 2byte = 36 + 4 = 40
		bmph.bfOffBits = 2 + 12 + bi.biSize; //2 + 12 + 40 = 54
		bmph.bfSize = bmph.bfOffBits + bi.biSizeImage;
		bmph.bfReserved1 = 0;
		bmph.bfReserved2 = 0;

		fromLong(bmph.bfSize, &buf[0]); //4
		fromShort(bmph.bfReserved1, &buf[4]); //2
		fromShort(bmph.bfReserved2, &buf[6]); //2
		fromLong(bmph.bfOffBits, &buf[8]); //4
		fromLong(bi.biSize, &buf[12]); //4

		fromLong(bi.biWidth, &buf[16]);//4
		fromLong(bi.biHeight, &buf[20]);//4
		fromShort(bi.biPlanes, &buf[24]);//2
		fromShort(bi.biBitCount, &buf[26]);//2
		fromLong(bi.biCompression, &buf[28]);//4
		fromLong(bi.biSizeImage, &buf[32]);//4
		fromLong(bi.biXPelsPerMeter, &buf[36]);//4
		fromLong(bi.biYPelsPerMeter, &buf[40]);//4
		fromLong(bi.biClrUsed, &buf[44]);//4
		fromLong(bi.biClrImportant, &buf[48]);//4
		fwrite(buf,52,1,fout); 
		if(true) //reverse order
		{
			//swap BGR TO RGB
			int i;
			char c;
			for(i=0;i<rowlength*height;i+=3)
			{
				c = buffer[i];
				buffer[i] = buffer[i+2];
				buffer[i+2] = c;
			}
		}
	}
	//write by row - and do byte alignment 4 (assume incoming is byte aligned 1)
	for(i=0;i<height;i++)
	{
		int j=i*rowlength;
		fwrite(&buffer[j],rowlength,1,fout);
		if(extra)
			fwrite(filler,extra,1,fout);
	}
	fclose(fout);
}
#endif 

#ifdef _MSC_VER
int fw_mkdir(char* path);
void Snapshot () 
{
/* going to try just the single snapshot for windows, to .bmp format 
  (and future: remember .avi holds a sequence of DIBs. A .bmp holds 1 DIB.
  There is something in the 2003 platform SDK and online for AVI & RIFF/DIB/BMP
  http://msdn.microsoft.com/en-us/library/dd145119(v=VS.85).aspx storing a bitmap
  http://msdn.microsoft.com/en-us/library/dd183391(VS.85).aspx  .bmp
  http://msdn.microsoft.com/en-us/library/aa446563.aspx  sample program
 http://msdn.microsoft.com/en-us/library/ms706540(v=VS.85).aspx 
 http://msdn.microsoft.com/en-us/library/ms706415(v=VS.85).aspx  Vfw.h, Vfw32.lib 
*/
	char thisRawFile[2000];
	char *mysnapb, *mytmp;
	char *imgbuf;
	ppSnapshot p = (ppSnapshot)gglobal()->Snapshot.prv;
	
	imgbuf = grabScreen(3,0,0,gglobal()->display.screenWidth,gglobal()->display.screenHeight);
	if (p->snapsnapB == NULL)
		mysnapb = "freewrl.snap";
	else
		mysnapb = p->snapsnapB;
	
	if (p->seqtmp == NULL)    mytmp   = "freewrl_tmp";
	else mytmp = p->seqtmp;
	
	fw_mkdir(mytmp);
	p->snapRawCount ++;
	snprintf (thisRawFile, sizeof(thisRawFile),"%s/%s.%04d.bmp",mytmp,mysnapb,p->snapRawCount);
	saveSnapshotBMP(thisRawFile,imgbuf,3,gglobal()->display.screenWidth,gglobal()->display.screenHeight);
	FREE(imgbuf);
}
#endif /*ifdef win32*/
#if !(defined(_MSC_VER) || defined(IPHONE))

void fwl_init_SnapGif()
{
	//struct pSnapshot* p = (struct pSnapshot*)gglobal()->Snapshot.prv;
	ppSnapshot p = (ppSnapshot)gglobal()->Snapshot.prv;
    p->snapGif = TRUE;
}

void fwl_init_PrintShot() {
	//struct pSnapshot* p = (struct pSnapshot*)gglobal()->Snapshot.prv;
	ppSnapshot p = (ppSnapshot)gglobal()->Snapshot.prv;
	p->doPrintshot = TRUE;
	p->savedSnapshot = p->doSnapshot;
	p->doSnapshot = TRUE;
	printf("setting printshot/ snapshot\n");
}

/* turn snapshotting on; if sequenced; possibly turn off an convert sequence */
void fwl_toggleSnapshot() {
	struct tSnapshot* t = &gglobal()->Snapshot;

#ifdef DOSNAPSEQUENCE
	struct pSnapshot* p = (struct pSnapshot*)t->prv;

/* need to re-implement this for OSX generating QTVR */
	if (!t->doSnapshot) {
		t->doSnapshot = TRUE;
	} else {
		if (p->snapsequence) {
			t->doSnapshot = FALSE;
			saveSnapSequence();
		}
	}
#else
	t->doSnapshot = ! t->doSnapshot;
#endif
}

#ifdef DOSNAPSEQUENCE
/* need to re-implement this for OSX generating QTVR */

/* convert a sequence of snaps into a movie */
void saveSnapSequence() {
		char *mytmp, *myseqb;
		char sysline[2000];
		char thisRawFile[2000];
		char thisGoodFile[2000];
		int xx;
		struct tSnapshot* t = (struct tSnapshot*)gglobal()->Snapshot;
	
		/* make up base names - these may be command line parameters */
	        if (p->snapseqB == NULL)  myseqb  = "freewrl.seq";
	        else myseqb = p->snapseqB;
	        if (p->seqtmp == NULL)    mytmp   = "freewrl_tmp";
	        else mytmp = p->seqtmp;
	
		t->snapGoodCount++;
	
		if (t->snapGif) {
			snprintf (thisGoodFile, sizeof(thisGoodFile),"%s/%s.%04d.gif",mytmp,myseqb,t->snapGoodCount);
		} else {
			snprintf (thisGoodFile, sizeof(thisGoodFile),"%s/%s.%04d.mpg",mytmp,myseqb,t->snapGoodCount);
		}
		/* snprintf(sysline,sizeof(sysline),"%s -size %dx%d -depth 8 -flip %s/%s*rgb %s", */
	
		/* Dani Rozenbaum - In order to generate 
		 movies (e.g. with mencoder) images have to be three-band RGB (in other 
		 words 24-bits) */
		snprintf(sysline,sizeof(sysline), "%s -size %dx%d -depth 24 -colorspace RGB +matte -flip %s/%s*rgb %s",
			CONVERT, gglobal()->display.screenWidth, gglobal()->display.screenHeight,mytmp,myseqb,thisGoodFile);
	
		/* printf ("convert line %s\n",sysline); */
	
		if (system (sysline) != 0) {
			printf ("Freewrl: error running convert line %s\n",sysline);
		}
		printf ("[1] snapshot is:  %s\n",thisGoodFile);
		/* remove temporary files */
		for (xx=1; xx <= p->snapRawCount; xx++) {
			snprintf (thisRawFile, sizeof(thisRawFile), "%s/%s.%04d.rgb",mytmp,myseqb,xx);
			UNLINK (thisRawFile);
		}
		t->snapRawCount=0;
}
#endif


#ifdef AQUA

CGContextRef MyCreateBitmapContext(int pixelsWide, int pixelsHigh, unsigned char *buffer) { 
	CGContextRef context=NULL; 
	CGColorSpaceRef colorSpace; 
	unsigned char* bitmapData; 
	int bitmapByteCount; 
	int bitmapBytesPerRow; 
	int i;

	bitmapBytesPerRow =(pixelsWide*4); 
	bitmapByteCount =(bitmapBytesPerRow*pixelsHigh); 
	colorSpace=CGColorSpaceCreateWithName(kCGColorSpaceGenericRGB); 
	bitmapData=(unsigned char*) malloc(bitmapByteCount); 

	if(bitmapData==NULL) 
	{ 
		fprintf(stderr,"Memorynotallocated!"); 
		return NULL; 
	} 

	/* copy the saved OpenGL data, but, invert it */
	for (i=0; i<pixelsHigh; i++) {
		memcpy (&bitmapData[i*bitmapBytesPerRow], 
			&buffer[(pixelsHigh-i-1)*bitmapBytesPerRow], 
			bitmapBytesPerRow);
	}

	context=CGBitmapContextCreate(bitmapData, 
		pixelsWide, 
		pixelsHigh, 
		8, // bits per component 
		bitmapBytesPerRow, 
		colorSpace, 
		kCGImageAlphaPremultipliedLast); 
	if (context== NULL) 
	{ 
		free (bitmapData); 
		fprintf (stderr, "Context not created!"); 
		return NULL; 
	} 
	CGColorSpaceRelease( colorSpace ); 
	return context; 
} 
#endif

/* get 1 frame; convert if we are doing 1 image at a time */
void Snapshot () {
	GLvoid *buffer;
	DIR *mydir;
	
	#ifndef AQUA
	char sysline[2000];
	FILE * tmpfile;
	char thisRawFile[2000];
	#endif

	char thisGoodFile[2000];
	char *mytmp, *mysnapb;

	#ifdef AQUA
        CFStringRef     path;
        CFURLRef        url;
	CGImageRef	image;
	CGImageDestinationRef imageDest;
	CGRect myBoundingBox; 
	CGContextRef myBitmapContext;
	#endif
	struct tSnapshot* t = &gglobal()->Snapshot;
	struct pSnapshot* p = (struct pSnapshot*)t->prv;


	printf("do Snapshot ... \n");
	/* make up base names - these may be command line parameters */
	
#ifdef DOSNAPSEQUENCE
/* need to re-implement this for OSX generating QTVR */

	if (p->snapsequence) {
	        if (p->snapseqB == NULL)
	                mysnapb  = "freewrl.seq";
	        else
	                mysnapb = p->snapseqB;
	} else {
#endif
	        if (p->snapsnapB == NULL)
	                mysnapb = "freewrl.snap";
	        else
	                mysnapb = p->snapsnapB;
#ifdef DOSNAPSEQUENCE
/* need to re-implement this for OSX generating QTVR */

	}
#endif
	
	
	if (p->seqtmp == NULL)    mytmp   = "freewrl_tmp";
	else mytmp = p->seqtmp;
	
	/*does the directory exist? */
	if ((mydir = opendir(mytmp)) == NULL) {
		mkdir (mytmp,0755);
		if ((mydir = opendir(mytmp)) == NULL) {
			ConsoleMessage ("error opening Snapshot directory %s\n",mytmp);
			return;
		}
	}
	
#ifdef DOSNAPSEQUENCE
/* need to re-implement this for OSX generating QTVR */

	/* are we sequencing, or just single snapping? */
	if (!p->snapsequence) p->doSnapshot=FALSE;  	/* reset snapshot key */
#endif

	
	#ifdef AQUA	
		/* OSX needs 32 bits per byte. */
		/* MALLOC 4 bytes per pixel */
		buffer = MALLOC (GLvoid *, 4*gglobal()->display.screenWidth*gglobal()->display.screenHeight*sizeof(char));
	
		/* grab the data */
		FW_GL_PIXELSTOREI (GL_UNPACK_ALIGNMENT, 1);
		FW_GL_PIXELSTOREI (GL_PACK_ALIGNMENT, 1);
		FW_GL_READPIXELS (0,0,gglobal()->display.screenWidth,gglobal()->display.screenHeight,GL_RGBA,GL_UNSIGNED_BYTE, buffer);
	#else	
		/* Linux, etc, can get by with 3 bytes per pixel */
		/* MALLOC 3 bytes per pixel */
		buffer = MALLOC (GLvoid *, 3*gglobal()->display.screenWidth*gglobal()->display.screenHeight*sizeof(char));
	
		/* grab the data */
		FW_GL_PIXELSTOREI (GL_UNPACK_ALIGNMENT, 1);
		FW_GL_PIXELSTOREI (GL_PACK_ALIGNMENT, 1);
		FW_GL_READPIXELS (0,0,gglobal()->display.screenWidth,gglobal()->display.screenHeight,GL_RGB,GL_UNSIGNED_BYTE, buffer);
	#endif
	
	/* save this snapshot */
	p->snapRawCount ++;
#ifdef DOSNAPSEQUENCE
/* need to re-implement this for OSX generating QTVR */

	if (p->snapRawCount > maxSnapImages) {
		FREE_IF_NZ (buffer);
		return;
	}
#endif

	#ifdef AQUA

		myBoundingBox = CGRectMake (0, 0, gglobal()->display.screenWidth, gglobal()->display.screenHeight); 
		myBitmapContext = MyCreateBitmapContext (gglobal()->display.screenWidth, gglobal()->display.screenHeight,buffer); 

		image = CGBitmapContextCreateImage (myBitmapContext); 
		CGContextDrawImage(myBitmapContext, myBoundingBox, image); 
		char *bitmapData = CGBitmapContextGetData(myBitmapContext); 

		CGContextRelease (myBitmapContext); 
		if (bitmapData) free(bitmapData); 


		p->snapGoodCount++;
		if (t->doPrintshot) {
			snprintf (thisGoodFile, sizeof(thisGoodFile), "/tmp/FW_print_snap_tmp.png");
			p->doPrintshot = FALSE;
			t->doSnapshot = p->savedSnapshot;
	        	path = CFStringCreateWithCString(NULL, thisGoodFile, kCFStringEncodingUTF8); 
			printf("thisGoodFile is %s\n", thisGoodFile);
	        	url = CFURLCreateWithFileSystemPath (NULL, path, kCFURLPOSIXPathStyle, FALSE);
		} else {
			snprintf (thisGoodFile, sizeof(thisGoodFile),"%s/%s.%04d.png",mytmp,mysnapb,p->snapGoodCount);

	        	path = CFStringCreateWithCString(NULL, thisGoodFile, kCFStringEncodingUTF8); 
	        	url = CFURLCreateWithFileSystemPath (NULL, path, kCFURLPOSIXPathStyle, FALSE);
		}

		imageDest = CGImageDestinationCreateWithURL(url, CFSTR("public.png"), 1, NULL);
		CFRelease(url);
		CFRelease(path);

		if (!imageDest) {
			ConsoleMessage("[1] Snapshot cannot be written");
			return;
		}

		CGImageDestinationAddImage(imageDest, image, NULL);
		if (!CGImageDestinationFinalize(imageDest)) {
			ConsoleMessage ("[2] Snapshot cannot be written");
		}
		CFRelease(imageDest);
		CGImageRelease(image); 
	#else	
		/* save the file */
	if(p->modeTesting){
		snprintf (thisRawFile, sizeof(thisRawFile),"%s/%s.%04d.bmp",mytmp,mysnapb,p->snapRawCount);
		saveSnapshotBMP(thisRawFile,buffer,3,gglobal()->display.screenWidth, gglobal()->display.screenHeight);
		FREE_IF_NZ (buffer);
	}else{
		snprintf (thisRawFile, sizeof(thisRawFile),"%s/%s.%04d.rgb",mytmp,mysnapb,p->snapRawCount);
		tmpfile = fopen(thisRawFile,"w");
		if (tmpfile == NULL) {
			printf ("cannot open temp file (%s) for writing\n",thisRawFile);
			FREE_IF_NZ (buffer);
			return;
		}

		if (fwrite(buffer, 1, gglobal()->display.screenHeight*gglobal()->display.screenWidth*3, tmpfile) <= 0) {
			printf ("error writing snapshot to %s, aborting snapshot\n",thisRawFile);
			FREE_IF_NZ (buffer);
			return;
		}
		fclose (tmpfile);
	
		/* convert -size 450x300 -depth 8 -flip /tmp/snappedfile.rgb out.png works. */
	
		FREE_IF_NZ (buffer);
	
#ifdef DOSNAPSEQUENCE
/* need to re-implement this for OSX generating QTVR */

		/* now, if we are doing only 1, convert the raw into the good.... */
		if (!p->snapsequence) {
#endif
			//if(!p->modeTesting){
				t->snapGoodCount++;
				snprintf (thisGoodFile, sizeof(thisGoodFile),"%s/%s.%04d.png",mytmp,mysnapb,t->snapGoodCount);
				snprintf(sysline,sizeof(sysline),"%s -size %dx%d -depth 8 -flip %s %s",
				IMAGECONVERT,gglobal()->display.screenWidth, gglobal()->display.screenHeight,thisRawFile,thisGoodFile);
		
				if (system (sysline) != 0) {
					printf ("Freewrl: error running convert line %s\n",sysline);
				}
				printf ("[2] snapshot is:  %s\n",thisGoodFile);
				UNLINK (thisRawFile);
			//}
#ifdef DOSNAPSEQUENCE
/* need to re-implement this for OSX generating QTVR */

		}
#endif
	#endif
	}
}
#endif /*ifdef win32*/

#endif //FRONTEND_DOES_SNAPSHOTS
