/*******************************************************************
 Copyright (C) 1998 Tuomas J. Lukka 2003 John Stewart, Ayla Khan CRC Canada
 Portions Copyright (C) 1998 John Breen
 DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 See the GNU Library General Public License (file COPYING in the distribution)
 for conditions of use and redistribution.
*********************************************************************/

/*
 * $Id: Viewer.c,v 1.21 2003/08/14 16:50:07 crc_canada Exp $
 *
 */

#include "headers.h"
#include "Structs.h"
#include "Viewer.h"

/* The global viewer - should be passed in by pointer JAS  */
extern VRML_Viewer Viewer;

extern int navi_tos;

static int viewer_type = NONE;
static int viewer_initialized = FALSE;
static VRML_Viewer_Walk viewer_walk = { 0, 0, 0, 0, 0, 0 };
static VRML_Viewer_Examine viewer_examine = { { 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, 0, 0 };
static VRML_Viewer_Fly viewer_fly = { { 0, 0, 0 }, { 0, 0, 0 }, KEYMAP, KEYMAP, -1 };

static int translate[COORD_SYS] = { 0, 0, 0 }, rotate[COORD_SYS] = { 0, 0, 0 };

static FILE *exfly_in_file;

void viewer_init (VRML_Viewer *viewer, int type) {
	Quaternion q_i;

	/* what type are we? used for handle events below */
	viewer_type = type;

	/* if we are brand new, set up our defaults */
	if (!viewer_initialized) {	
		viewer_initialized = TRUE;

		(viewer->Pos).x = 0;
		(viewer->Pos).y = 0;
		(viewer->Pos).z = 10;

		(viewer->AntiPos).x = 0;
		(viewer->AntiPos).y = 0;
		(viewer->AntiPos).z = 0;


		vrmlrot_to_quaternion (&Viewer.Quat,1.0,0.0,0.0,0.0);
		vrmlrot_to_quaternion (&q_i,1.0,0.0,0.0,0.0);
		inverse(&(Viewer.AntiQuat),&q_i);

/* 		Navi => undef, */
/* 		$this->{Navi} = VRML::Scene->new_node("NavigationInfo", */
/* 							VRML::Nodes->{NavigationInfo}{Defaults}); */
		/* speed & headlight are the only NavigationInfo fields used */
		/* check out set_ViewerParams for what happens during a NavigationInfo bind */
		viewer->headlight = TRUE;
		viewer->speed = 1.0;
/* 		Dist = 10.0; */
		viewer->Dist = 10.0;
		viewer->eyehalf = 0.0;
		viewer->eyehalfangle = 0.0;
		viewer->buffer = 0;

		viewer->walk = &viewer_walk;
		viewer->examine = &viewer_examine;
		viewer->fly = &viewer_fly;
	} else {
	}

	resolve_pos(viewer);
}


void
print_viewer(VRML_Viewer *viewer)
{
	struct orient or;
	quaternion_to_vrmlrot(&(viewer->Quat), &(or.x), &(or.y), &(or.z), &(or.a));

	printf("Viewer {\n\tPosition [ %.4g, %.4g, %.4g ]\n\tQuaternion [ %.4g, %.4g, %.4g, %.4g ]\n\tOrientation [ %.4g, %.4g, %.4g, %.4g ]\n}\n", (viewer->Pos).x, (viewer->Pos).y, (viewer->Pos).z, (viewer->Quat).w, (viewer->Quat).x, (viewer->Quat).y, (viewer->Quat).z, or.x, or.y, or.z, or.a);
}

unsigned int
get_buffer(VRML_Viewer *viewer)
{
	return(viewer->buffer);
}

void
set_buffer(VRML_Viewer *viewer, const unsigned int buffer)
{
	viewer->buffer = buffer;
}

int
get_headlight(VRML_Viewer *viewer)
{
	return(viewer->headlight);
}

void
toggle_headlight(VRML_Viewer *viewer)
{
	if (viewer->headlight == TRUE) {
		viewer->headlight = FALSE;
	} else {
		viewer->headlight = TRUE;
	}
}

void
set_eyehalf(VRML_Viewer *viewer, const double eyehalf, const double eyehalfangle)
{
	viewer->eyehalf = eyehalf;
	viewer->eyehalfangle = eyehalfangle;
}

void
set_viewer_type(const int type)
{
	/* can the currently bound viewer type handle this */
	/* if there is no bound viewer, just ignore (happens on initialization) */
	if (navi_tos != -1)
		if (Viewer.oktypes[type]==FALSE) return;

	viewer_init(&Viewer,type);

	/* tell the status bar what we are */
	viewer_type_status (type);

	switch(type) {
	case NONE:
	case EXAMINE:
	case WALK:
	case EXFLY:
	case FLY:
		viewer_type = type;
		break;
	default:
		fprintf(stderr, "Viewer type %d is not supported. See Viewer.h.\n", type);
		viewer_type = NONE;
		break;
	}
}


int
use_keys()
{ 	
	if (viewer_type == FLY) {
		return TRUE;
	}
	return FALSE;
}


void
resolve_pos(VRML_Viewer *viewer)
{ 
	/* my($this) = @_; */
	struct pt rot, z_axis = { 0, 0, 1 };
	Quaternion q_inv;
	double dist = 0;
	VRML_Viewer_Examine *examine = viewer->examine;

	if (viewer_type == EXAMINE) {
		/* my $z = $this->{Quat}->invert->rotate([0,0,1]); */
		inverse(&q_inv, &(viewer->Quat));
		rotation(&rot, &q_inv, &z_axis);

		/* my $d = 0; for(0..2) {$d += $this->{Pos}[$_] * $z->[$_]} */
		dist = VECPT(viewer->Pos, rot);

		/*
		 * Fix the rotation point to be 10m in front of the user (dist = 10.0)  
		 * or, try for the origin. Preferential treatment would be to choose
		 * the shape within the center of the viewpoint. This information is
		 * found in the matrix, and is used for collision calculations - we
		 * need to better store it.
		 */

		/* $d = abs($d); $this->{Dist} = $d; */
		viewer->Dist = fabs(dist);

		/* $this->{Origin} = [ map {$this->{Pos}[$_] - $d * $z->[$_]} 0..2 ]; */
		(examine->Origin).x = (viewer->Pos).x - viewer->Dist * rot.x;
		(examine->Origin).y = (viewer->Pos).y - viewer->Dist * rot.y;
		(examine->Origin).z = (viewer->Pos).z - viewer->Dist * rot.z;

		// printf ("examine origin = %f %f %f\n",examine->Origin.x,examine->Origin.y,examine->Origin.z);
	}
}

void
viewer_togl(VRML_Viewer *viewer, double fieldofview)
{

	/* GLdouble modelMatrix[16]; 

        glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
	printf ("Viewer_togl Matrix: \n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n",
                modelMatrix[0],  modelMatrix[4],  modelMatrix[ 8],  modelMatrix[12],
                modelMatrix[1],  modelMatrix[5],  modelMatrix[ 9],  modelMatrix[13],
                modelMatrix[2],  modelMatrix[6],  modelMatrix[10],  modelMatrix[14],
                modelMatrix[3],  modelMatrix[7],  modelMatrix[11],  modelMatrix[15]);
	*/


/* 	if ($this->{buffer}!=&VRML::OpenGL::GL_BACK) */
	if (viewer->buffer != GL_BACK) {
		set_stereo_offset(viewer->buffer, viewer->eyehalf, viewer->eyehalfangle, fieldofview);
/* 		VRML::VRMLFunc::set_stereo_offset ($this->{buffer}, $this->{eyehalf},$this->{eyehalfangle}); */
	}

	togl(&(viewer->Quat));
	glTranslated(-(viewer->Pos).x, -(viewer->Pos).y, -(viewer->Pos).z);
	glTranslated((viewer->AntiPos).x, (viewer->AntiPos).y, (viewer->AntiPos).z);
	togl(&(viewer->AntiQuat));

        /* glGetDoublev(GL_MODELVIEW_MATRIX, modelMatrix);
       printf ("Viewer end _togl Matrix: \n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n\t%f %f %f %f\n",
                modelMatrix[0],  modelMatrix[4],  modelMatrix[ 8],  modelMatrix[12],
                modelMatrix[1],  modelMatrix[5],  modelMatrix[ 9],  modelMatrix[13],
                modelMatrix[2],  modelMatrix[6],  modelMatrix[10],  modelMatrix[14],
                modelMatrix[3],  modelMatrix[7],  modelMatrix[11],  modelMatrix[15]);
	*/

}


void
handle_walk(VRML_Viewer *viewer, const char *mev, const unsigned int button, const double x, const double y)
{
	VRML_Viewer_Walk *walk = viewer->walk;

	if (strncmp(mev, PRESS, PRESS_LEN) == 0) {
		walk->SY = y;
		walk->SX = x;
	} else if (strncmp(mev, DRAG, DRAG_LEN) == 0) {
		if (button == 1) {
			walk->ZD = (y - walk->SY) * viewer->speed;
			walk->RD = (x - walk->SX) * 0.1;
		} else if (button == 3) {
			walk->XD = (x - walk->SX) * viewer->speed;
			walk->YD = -(y - walk->SY) * viewer->speed;
		}
	} else if (strncmp(mev, RELEASE, RELEASE_LEN) == 0) {
		if (button == 1) {
			walk->ZD = 0;
			walk->RD = 0;
		} else if (button == 3) {
			walk->XD = 0;
			walk->YD = 0;
		}
	}
}


void
handle_examine(VRML_Viewer *viewer, const char *mev, const unsigned int button, double x, double y)
{
/* 	my($this, $mev, $but, $mx, $my) = @_; */
	Quaternion q, q_i, arc;
	struct pt p = { 0, 0, viewer->Dist };
	VRML_Viewer_Examine *examine = viewer->examine;
	double squat_norm;

	if (strncmp(mev, PRESS, PRESS_LEN) == 0) {
		if (button == 1) {
			xy2qua(&(examine->SQuat), x, y);
			set(&(examine->OQuat), &(viewer->Quat));
		} else if (button == 3) {
			examine->SY = y;
			examine->ODist = viewer->Dist;
		}
	} else if (strncmp(mev, DRAG, DRAG_LEN) == 0) {
		if (button == 1) {
			squat_norm = norm(&(examine->SQuat));
			/* we have missed the press */
			if (APPROX(squat_norm, 0)) {
				fprintf(stderr, "Viewer handle_examine: mouse event DRAG - missed press\n");
				/* 			$this->{SQuat} = $this->xy2qua($mx,$my); */
				xy2qua(&(examine->SQuat), x, y);
				/* 			$this->{OQuat} = $this->{Quat}; */
				set(&(examine->OQuat), &(viewer->Quat));
			} else {
				/* my $q = $this->xy2qua($mx,$my); */
				xy2qua(&q, x, y);
				/* my $arc = $q->multiply($this->{SQuat}->invert()); */
				inverse(&q_i, &(examine->SQuat));
				multiply(&arc, &q, &q_i);

		
				/* $this->{Quat} = $arc->multiply($this->{OQuat}); */
				multiply(&(viewer->Quat), &arc, &(examine->OQuat));
			}
		} else if (button == 3) {
			viewer->Dist = examine->ODist * exp(examine->SY - y);

		}
 	}
	inverse(&q_i, &(viewer->Quat));
	rotation(&(viewer->Pos), &q_i, &p);

	viewer->Pos.x += (examine->Origin).x;
	viewer->Pos.y += (examine->Origin).y;
	viewer->Pos.z += (examine->Origin).z;
}


void handle(VRML_Viewer *viewer, const char *mev, const unsigned int button, const double x, const double y)
{

/* printf("Viewer handle: viewer_type %s, mouse event %s, button %u, x %f, y %f\n", */
/* 	   VIEWER_STRING(viewer_type), mev, button, x, y); */

	if (button == 2) {
		return;
	}

	switch(viewer_type) {
	case NONE:
		break;
	case EXAMINE:
		handle_examine(viewer, mev, button, x, y);
		break;
	case WALK:
		handle_walk(viewer, mev, button, x, y);
		break;
	case EXFLY:
		break;
	case FLY:
		break;
	default:
		break;
	}
}


void
handle_key(VRML_Viewer *viewer, const double time, const char key)
{
	/* my($this,$time,$key) = @_; */
	VRML_Viewer_Fly *fly = viewer->fly;
	char _key;
	int i;

	UNUSED(time);

	if (viewer_type == FLY) {
		/* $key = lc $key; */
		_key = (char) tolower((int) key);

		for (i = 0; i < KEYS_HANDLED; i++) {
			if ((fly->Down[i]).key  == _key) {
				/* $this->{Down}{$key} = 1; */
				(fly->Down[i]).hit = 1;
			}
		}
	}
}


void
handle_keyrelease(VRML_Viewer *viewer, const double time, const char key)
{
	/* my($this,$time,$key) = @_; */
	VRML_Viewer_Fly *fly = viewer->fly;
	char _key;
	int i;

	UNUSED(time);

	if (viewer_type == FLY) {
		/* $key = lc $key; */
		_key = (char) tolower((int) key);

		for (i = 0; i < KEYS_HANDLED; i++) {
			if ((fly->Down[i]).key  == _key) {
				/* $this->{WasDown}{$key} += $this->{Down}{$key}; */
				(fly->WasDown[i]).hit += (fly->Down[i]).hit;
				/* delete $this->{Down}{$key}; */
				(fly->Down[i]).hit = 0;
			}
		}
	}
}


/*
 * handle_tick_walk: called once per frame.
 *
 * Sets viewer to next expected position.
 * This should be called before position sensor calculations
 * (and event triggering) take place.
 * Position dictated by this routine is NOT final, and is likely to
 * change if the viewer is left in a state of collision. (ncoder)
 */

void
handle_tick_walk(VRML_Viewer *viewer, const double time)
{
/* 	my($this, $time) = @_; */
	VRML_Viewer_Walk *walk = viewer->walk;
	Quaternion q = { (viewer->Quat).w,
					 (viewer->Quat).x,
					 (viewer->Quat).y,
					 (viewer->Quat).z },
		/* my $nq = new VRML::Quaternion(1-0.2*$this->{RD},0,0.2*$this->{RD},0); */
		nq = { 1 - 0.2 * walk->RD,
			   0,
			   0.2 * walk->RD,
			   0 };
	struct pt p = { 0.15 * walk->XD, 0.15 * walk->YD, 0.15 * walk->ZD };

	UNUSED(time);

	/* my $nv = $this->{Quat}->invert->rotate([0.15*$this->{XD},0.15*$this->{YD},0.15*$this->{ZD}]); */
	/* for(0..2) {$this->{Pos}[$_] += $nv->[$_]} */
	increment_pos(viewer, &p);

	/* $nq->normalize_this; */
	normalize(&nq);
	/* $this->{Quat} = $nq->multiply($this->{Quat}); */
	multiply(&(viewer->Quat), &nq, &q);
    
	/* info passed to Collision routines */
	/* VRML::VRMLFunc::set_viewer_delta($this->{XD},
	 * $this->{YD},$this->{ZD}); #interresting idea, but not quite.
	 */
}


/* formerly package VRML::Viewer::ExFly
 * entered via the "f" key.
 *
 * External input for x,y,z and quat. Reads in file
 * /tmp/inpdev (macro IN_FILE), which is a single line file that is
 * updated by some external program.
 *
 * eg:
 *    9.67    -1.89    -1.00  0.99923 -0.00219  0.01459  0.03640
 *
 * Do nothing for the mouse.
 */

/* my $in_file = "/tmp/inpdev"; */
/* #JAS my $in_file_date = stat($in_file)->mtime; */
/* my $string = ""; */
/* my $inc = 0; */
/* my $inf = 0; */

void
handle_tick_exfly(VRML_Viewer *viewer, const double time)
{
/* 	my($this, $time) = @_; */
	size_t len = 0;
	char string[STRING_SIZE];
	float px,py,pz,q1,q2,q3,q4;

	UNUSED(time);

	memset(string, 0, STRING_SIZE * sizeof(char));

	/*
	 * my $chk_file_date = stat($in_file)->mtime;
	 * following uncommented as time on file only change
	 * once per second - should change this... 
     *
	 * $in_file_date = $chk_file_date;
	 */

/* 	sysopen ($inf, $in_file, O_RDONLY) or  */
/* 		die "Error reading external sensor input file $in_file\n"; */
/* 	$inc = sysread ($inf, $string, 100); */
/* 	close $inf; */
	if ((exfly_in_file = fopen(IN_FILE, "r")) == NULL) {
		fprintf(stderr,
				"Viewer handle_tick_exfly: could not open %s for read, returning to EXAMINE mode.\nSee the FreeWRL man page for further details on the usage of Fly - External Sensor input mode.\n",
				IN_FILE);

		/* allow the user to continue in default Viewer mode */
		viewer_type = EXAMINE;
		return;
	}
	fread(string, sizeof(char), IN_FILE_BYTES, exfly_in_file);
	if (ferror(exfly_in_file)) {
		fprintf(stderr,
				"Viewer handle_tick_exfly: error reading from file %s.",
				IN_FILE);
		fclose(exfly_in_file);
		return;
	}
	fclose(exfly_in_file);

/* 	if (length($string)>0) */
	if ((len = strlen(string)) > 0) {
		len = sscanf (string, "%f %f %f %f %f %f %f",&px,&py,&pz,
			&q1,&q2,&q3,&q4);

		/* read error? */
		if (len != 7) return;

		(viewer->Pos).x = px;
		(viewer->Pos).y = py;
		(viewer->Pos).z = pz;

		(viewer->Quat).w = q1;
		(viewer->Quat).x = q2;
		(viewer->Quat).y = q3;
		(viewer->Quat).z = q4;
	}
}


/* $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ */

/* package VRML::Viewer::Fly; # Modeled after Descent(tm) ;) */
/* @VRML::Viewer::Fly::ISA=VRML::Viewer; */
/* # */
/* # Members: */
/* #  Velocity - current velocity as 3-vector */
/* #   */

/* # Do nothing for the mouse */


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

void
set_action(char *key)
{
	switch(*key) {
	case 'a':
		translate[Z_AXIS] -= 1;
		break;
	case 'z':
		translate[Z_AXIS] += 1;
		break;
	case 'j':
		translate[X_AXIS] -= 1;
		break;
	case 'l':
		translate[X_AXIS] += 1;
		break;
	case 'p':
		translate[Y_AXIS] -= 1;
		break;
	case ';':
		translate[Y_AXIS] += 1;
		break;
	case '8':
		rotate[X_AXIS] += 1;
		break;
	case 'k':
		rotate[X_AXIS] -= 1;
		break;
	case 'u':
		rotate[Y_AXIS] -= 1;
		break;
	case 'o':
		rotate[Y_AXIS] += 1;
		break;
	case '7':
		rotate[Z_AXIS] -= 1;
		break;
	case '9':
		rotate[Z_AXIS] += 1;
		break;
	default:
		break;
	}
}

void
handle_tick_fly(VRML_Viewer *viewer, const double time)
{
/* 	my($this, $time) = @_; */

	VRML_Viewer_Fly *fly = viewer->fly;
	Key ps[KEYS_HANDLED] = KEYMAP;
	Quaternion q_v, nq = { 1, 0, 0, 0 };
	struct pt v;
	double changed = 0, time_diff = -1;
	int i;

	if (fly->lasttime < 0) {
		fly->lasttime = time;
		return;
	} else {
		time_diff = time - fly->lasttime;
		fly->lasttime = time;
		if (APPROX(time_diff, 0)) {
			return;
		}
	}
	
	/* first, get all the keypresses since the last time */
	for (i = 0; i < KEYS_HANDLED; i++) {
		(ps[i]).hit += (fly->Down[i]).hit;
	}

	for (i = 0; i < KEYS_HANDLED; i++) {
		(ps[i]).hit += (fly->WasDown[i]).hit;
		(fly->WasDown[i]).hit = 0;
	}

	memset(translate, 0, sizeof(int) * COORD_SYS);
	memset(rotate, 0, sizeof(int) * COORD_SYS);

	for (i = 0; i < KEYS_HANDLED; i++) {
		if ((ps[i]).hit) {
			set_action(&(ps[i]).key);
		}
	}

	/* has anything changed? if so, then re-render */
	for (i = 0; i < COORD_SYS; i++) {
		fly->Velocity[i] *= pow(0.06, time_diff);

		fly->Velocity[i] += time_diff * translate[i] * 14.5;

		if (fabs(fly->Velocity[i]) >9.0) {
			fly->Velocity[i] /= (fabs(fly->Velocity[i]) /9.0);
		}
		changed += fly->Velocity[i];
	//printf ("vel %d %f\n",i,fly->Velocity[i]);
	}

	v.x = fly->Velocity[0] * time_diff;
	v.y = fly->Velocity[1] * time_diff;
	v.z = fly->Velocity[2] * time_diff;

	increment_pos(viewer, &v);

	for (i = 0; i < COORD_SYS; i++) {
		fly->AVelocity[i] *= pow(0.04, time_diff);
		fly->AVelocity[i] += time_diff * rotate[i] * 0.1;

		if (fabs(fly->AVelocity[i]) > 0.8) {
			fly->AVelocity[i] /= (fabs(fly->AVelocity[i]) / 0.8);
		}
		changed += fly->AVelocity[i];
	//printf ("avel %d %f\n",i,fly->AVelocity[i]);
	}
	
	nq.x = fly->AVelocity[0];
	nq.y = fly->AVelocity[1];
	nq.z = fly->AVelocity[2];
	normalize(&nq);

	set(&q_v, &(viewer->Quat));
	multiply(&(viewer->Quat), &nq, &q_v);
}

void
handle_tick(VRML_Viewer *viewer, const double time)
{

/* printf("Viewer handle_tick: viewer_type %s, time %f\n", */
/* 	   VIEWER_STRING(viewer_type), time); */

	switch(viewer_type) {
	case NONE:
		break;
	case EXAMINE:
		break;
	case WALK:
		handle_tick_walk(viewer, time);
		break;
	case EXFLY:
		handle_tick_exfly(viewer, time);
		break;
	case FLY:
		handle_tick_fly(viewer, time);
		break;
	default:
		break;
	}
}



/*
 * Semantics: given a viewpoint and orientation,
 * we take the center to revolve around to be the closest point to origin
 * on the z axis.
 * Changed Feb27 2003 JAS - by fixing $d to 10.0, we make the rotation
 * point to be 10 metres in front of the user.
 */

/* ArcCone from TriD */
void
xy2qua(Quaternion *ret, const double x, const double y)
{
/* 	my($this, $x, $y) = @_; */
/* 	$x -= 0.5; $y -= 0.5; $x *= 2; $y *= 2; */
/* 	$y = -$y; */
	double _x = x - 0.5, _y = y - 0.5, _z, dist;
	_x *= 2;
	_y *= -2;

/* 	my $dist = sqrt($x**2 + $y**2); */
	dist = sqrt((_x * _x) + (_y * _y));

/* 	if($dist > 1.0) {$x /= $dist; $y /= $dist; $dist = 1.0} */
	if (dist > 1.0) {
		_x /= dist;
		_y /= dist;
		dist = 1.0;
	}
/* 	my $z = 1-$dist; */
	_z = 1 - dist;

/* 	my $qua = VRML::Quaternion->new(0,$x,$y,$z); */
	ret->w = 0;
	ret->x = _x;
	ret->y = _y;
	ret->z = _z;
/* 	$qua->normalize_this(); */
	normalize(ret);
/* 	return $qua; */
}

void
set_stereo_offset(unsigned int buffer, const double eyehalf, const double eyehalfangle, const double fieldofview)
{
      double x = 0.0, correction, angle = 0.0;

      /*
	   * correction for fieldofview
       * 18.0: builtin fieldOfView of human eye
       * 45.0: default fieldOfView of VRML97 viewpoint
       */

      correction = 18.0 / fieldofview;
      if (buffer == GL_BACK_LEFT) {
              x = eyehalf;
              angle = eyehalfangle * correction;
      } else if (buffer == GL_BACK_RIGHT) {
              x = -eyehalf;
              angle = -eyehalfangle * correction;
      }
      glTranslated(x, 0.0, 0.0);
      glRotated(angle, 0.0, 1.0, 0.0);
}

void
increment_pos(VRML_Viewer *viewer, struct pt *vec)
{
	struct pt nv;
	Quaternion q_i;

	inverse(&q_i, &(viewer->Quat));
	rotation(&nv, &q_i, vec);

	(viewer->Pos).x += nv.x;
	(viewer->Pos).y += nv.y;
	(viewer->Pos).z += nv.z;
}



void
bind_viewpoint (struct VRML_Viewpoint *vp) {
	Quaternion q_i;

	/* set Viewer position and orientation */

	/*printf ("bind_viewpoint, setting Viewer to %f %f %f orient %f %f %f %f\n",vp->position.c[0],vp->position.c[1],
	vp->position.c[2],vp->orientation.r[0],vp->orientation.r[1],vp->orientation.r[2],
	vp->orientation.r[3]);
	printf ("	node %d fieldOfView %f\n",vp,vp->fieldOfView); */
	

	Viewer.Pos.x = vp->position.c[0];
	Viewer.Pos.y = vp->position.c[1];
	Viewer.Pos.z = vp->position.c[2];
	Viewer.AntiPos.x = vp->position.c[0];
	Viewer.AntiPos.y = vp->position.c[1];
	Viewer.AntiPos.z = vp->position.c[2];

	vrmlrot_to_quaternion (&Viewer.Quat,vp->orientation.r[0],
		vp->orientation.r[1],vp->orientation.r[2],vp->orientation.r[3]);

	vrmlrot_to_quaternion (&q_i,vp->orientation.r[0],
		vp->orientation.r[1],vp->orientation.r[2],vp->orientation.r[3]);
	inverse(&(Viewer.AntiQuat),&q_i);

	resolve_pos(&Viewer);
}
