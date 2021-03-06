/*
=INSERT_TEMPLATE_HERE=

$Id: quaternion.h,v 1.8 2010/08/02 01:11:25 dug9 Exp $

Quaternion ???

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


#ifndef __FREEWRL_QUATERNION_H__
#define __FREEWRL_QUATERNION_H__


#define DELTA 0.0001

/* definitions for mapping matrix in OpenGL format to standard math */
#define MAT00 mat[0]
#define MAT01 mat[1]
#define MAT02 mat[2]
#define MAT03 mat[3]
#define MAT10 mat[4]
#define MAT11 mat[5]
#define MAT12 mat[6]
#define MAT13 mat[7]
#define MAT20 mat[8]
#define MAT21 mat[9]
#define MAT22 mat[10]
#define MAT23 mat[11]
#define MAT30 mat[12]
#define MAT31 mat[13]
#define MAT32 mat[14]
#define MAT33 mat[15]

/* definitions for "standard" matrix representation to map to OpenGL */
#define MATHEMATICS_MAT00 mat[0]
#define MATHEMATICS_MAT01 mat[4]
#define MATHEMATICS_MAT02 mat[8]
#define MATHEMATICS_MAT03 mat[12]
#define MATHEMATICS_MAT10 mat[1]
#define MATHEMATICS_MAT11 mat[5]
#define MATHEMATICS_MAT12 mat[9]
#define MATHEMATICS_MAT13 mat[13]
#define MATHEMATICS_MAT20 mat[2]
#define MATHEMATICS_MAT21 mat[6]
#define MATHEMATICS_MAT22 mat[10]
#define MATHEMATICS_MAT23 mat[14]
#define MATHEMATICS_MAT30 mat[3]
#define MATHEMATICS_MAT31 mat[7]
#define MATHEMATICS_MAT32 mat[11]
#define MATHEMATICS_MAT33 mat[15]

typedef struct quaternion {
	double w;
	double x;
	double y;
	double z;
} Quaternion;
void
matrix_to_quaternion (Quaternion *quat, double *mat) ;
void
quaternion_to_matrix (double *mat, Quaternion *quat) ;

void scale_to_matrix (double *mat, struct point_XYZ *scale);

void
vrmlrot_to_quaternion(Quaternion *quat,
					  const double x,
					  const double y,
					  const double z,
					  const double a);

void
quaternion_to_vrmlrot(const Quaternion *quat,
					  double *x,
					  double *y,
					  double *z,
					  double *a);

void
quaternion_conjugate(Quaternion *quat);

void
quaternion_inverse(Quaternion *ret,
		const Quaternion *quat);

double
quaternion_norm(const Quaternion *quat);

void
quaternion_normalize(Quaternion *quat);

void
quaternion_add(Quaternion *ret,
	const Quaternion *q1,
	const Quaternion *q2);

void
quaternion_multiply(Quaternion *ret,
		 const Quaternion *q1,
		 const Quaternion *q2);

void
quaternion_scalar_multiply(Quaternion *quat,
				const double s);

void
quaternion_rotation(struct point_XYZ *ret,
		 const Quaternion *quat,
		 const struct point_XYZ *v);

void
quaternion_togl(Quaternion *quat);

void
quaternion_set(Quaternion *ret,
	const Quaternion *quat);

void
quaternion_slerp(Quaternion *ret,
	  const Quaternion *q1,
	  const Quaternion *q2,
	  const double t);

void loadIdentityMatrix (double *);
void vrmlrot_multiply(float* ret, float *a, float *b);
void vrmlrot_normalize(float *ret);

#endif /* __FREEWRL_QUATERNION_H__ */
