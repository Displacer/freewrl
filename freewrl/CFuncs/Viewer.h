#ifndef __VIEWER_H_
#define __VIEWER_H_

/*
 * $Id: Viewer.h,v 1.12 2003/10/09 15:30:35 crc_canada Exp $
 *
 */


#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#ifdef AQUA 
#include <gl.h>
#include <glu.h>
#include <glext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#endif

#include "headers.h"
#include "quaternion.h"
#include "OpenGL_Utils.h"


#define NONE 0
#define EXAMINE 1
#define WALK 2
#define EXFLY 3
#define FLY 4

#define VIEWER_STRING(type) ( \
	type == NONE ? "NONE" : ( \
	type == EXAMINE ? "EXAMINE" : ( \
	type == WALK ? "WALK" : ( \
	type == EXFLY ? "EXFLY" : ( \
	type == FLY ? "FLY" : "UNKNOWN")))))

#define PRESS "PRESS"
#define PRESS_LEN 5

#define DRAG "DRAG"
#define DRAG_LEN 4

#define RELEASE "RELEASE"
#define RELEASE_LEN 7

#define KEYS_HANDLED 12
/* my %actions = ( */
/* 	a => sub {$aadd[2] -= $_[0]}, */
/* 	z => sub {$aadd[2] += $_[0]}, */
/* 	j => sub {$aadd[0] -= $_[0]}, */
/* 	l => sub {$aadd[0] += $_[0]}, */
/* 	p => sub {$aadd[1] += $_[0]}, */
/* 	';' => sub {$aadd[1] -= $_[0]}, */

/* 	8 => sub {$radd[0] += $_[0]}, */
/* 	k => sub {$radd[0] -= $_[0]}, */
/* 	u => sub {$radd[1] -= $_[0]}, */
/* 	o => sub {$radd[1] += $_[0]}, */
/* 	7 => sub {$radd[2] -= $_[0]}, */
/* 	9 => sub {$radd[2] += $_[0]}, */
/* ); */
#define KEYMAP {{ 'a', 0 }, { 'z', 0 }, { 'j', 0 }, { 'l', 0 }, { 'p', 0 }, { ';', 0 }, { '8', 0 }, { 'k', 0 }, { 'u', 0 }, { 'o', 0 }, { '7', 0 }, { '9', 0 }}

#define COORD_SYS 3
#define X_AXIS 0
#define Y_AXIS 1
#define Z_AXIS 2

#define STRING_SIZE 256

#define IN_FILE "/tmp/inpdev"
#define IN_FILE_BYTES 100
#define INPUT_LEN 9
#define INPUT_LEN_Z 8
#define X_OFFSET 8
#define Y_OFFSET 17
#define Z_OFFSET 0
#define QUAT_W_OFFSET 26
#define QUAT_X_OFFSET 35
#define QUAT_Y_OFFSET 44
#define QUAT_Z_OFFSET 53


/* extern struct pt ViewerPosition; */
/* extern struct orient ViewerOrientation; */


typedef struct viewer_walk {
	double SX;
	double SY;
	double XD;
	double YD;
	double ZD;
	double RD;
} VRML_Viewer_Walk;


typedef struct viewer_examine {
	struct pt Origin;
	Quaternion OQuat;
	Quaternion SQuat;
	double ODist;
	double SY;
} VRML_Viewer_Examine;

typedef struct key {
	char key;
	unsigned int hit;
} Key;


/* Modeled after Descent(tm) ;) */
typedef struct viewer_fly {
	double Velocity[COORD_SYS];
	double AVelocity[COORD_SYS];
	Key Down[KEYS_HANDLED];
	Key WasDown[KEYS_HANDLED];
	double lasttime;
} VRML_Viewer_Fly;


typedef struct viewer {
	struct pt Pos;
	struct pt AntiPos;
	Quaternion Quat;
	Quaternion AntiQuat;
	int headlight;
	double speed;
	double Dist;
	double eyehalf;
	double eyehalfangle;
	unsigned int buffer;
	int oktypes[6];		//boolean for types being acceptable.
	VRML_Viewer_Walk *walk;
	VRML_Viewer_Examine *examine;
	VRML_Viewer_Fly *fly;
} VRML_Viewer;


void
viewer_init(VRML_Viewer *viewer,
			int type);

void
print_viewer(VRML_Viewer *viewer);

unsigned int
get_buffer(VRML_Viewer *viewer);

void
set_buffer(VRML_Viewer *viewer,
		   const unsigned int buffer);

int
get_headlight(VRML_Viewer *viewer);

void
toggle_headlight(VRML_Viewer *viewer);

int
use_keys(void);

void
set_eyehalf(VRML_Viewer *viewer,
			const double eyehalf,
			const double eyehalfangle);

void
set_viewer_type(const int type);

void
resolve_pos(VRML_Viewer *viewer);

void
xy2qua(Quaternion *ret,
	   const double x,
	   const double y);

void
viewer_togl(VRML_Viewer *viewer,
			double fieldofview);


void handle(VRML_Viewer *viewer,
			 const char *mev,
			 const unsigned int button,
			 const double x,
			 const double y);

void
handle_walk(VRML_Viewer *viewer,
			const char *mev,
			const unsigned int button,
			const double x,
			const double y);

void
handle_examine(VRML_Viewer *viewer,
			   const char *mev,
			   const unsigned int button,
			   const double x,
			   const double y);

void
handle_key(VRML_Viewer *viewer,
		   const double time,
		   const char key);

void
handle_keyrelease(VRML_Viewer *viewer,
				  const double time,
				  const char key);

void
handle_tick(VRML_Viewer *viewer,
			const double time);

void
handle_tick_walk(VRML_Viewer *viewer,
				 const double time);

void
handle_tick_exfly(VRML_Viewer *viewer,
				  const double time);

void
handle_tick_fly(VRML_Viewer *viewer,
				const double time);

void
set_action(char *key);

void
set_stereo_offset(unsigned int buffer,
				  const double eyehalf,
				  const double eyehalfangle,
				  double fieldofview);

void
increment_pos(VRML_Viewer *viewer,
			  struct pt *vec);

void
bind_viewpoint(struct VRML_Viewpoint *node);

void
bind_geoviewpoint(struct VRML_GeoViewpoint *node);

#endif /* __VIEWER_H_ */
