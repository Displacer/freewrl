/* $Id: avt.h,v 1.1.2.3 2000/09/07 23:40:33 rcoscali Exp $
 * 
 * -- avt.h --
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * Public header for Average Load BTree implementation
 *
 * $Log: avt.h,v $
 * Revision 1.1.2.3  2000/09/07 23:40:33  rcoscali
 * Add copy notice & headers ]ignoreAdde
 *
 */

#ifndef _AVT_H__
#define _AVT_H__

struct AVdat_st {
    void *opaque;
    struct AVdat_st *next;
};

typedef struct AVdat_st *AVdat_t;

struct AVnode_st;

typedef struct AVnode_st *AVnode_t;
typedef struct AVnode_st *AVtree_t;

struct AVnode_st {
    double		z;
    struct AVnode_st   *l, *r;
    int			h;
    AVdat_t		dat;
};

typedef enum AVway_en {
    AVt_ASCENDING_WAY = -1, 
    AVt_DESCENDING_WAY = 1
} AVway_t;

typedef void(*AVcb_t)(int, double, void *);

extern AVtree_t AVt_reset    ( AVtree_t tree );
extern AVtree_t AVt_insert   ( AVtree_t tree, double z, AVdat_t dat);
extern void	AVt_apply    ( AVtree_t tree, AVway_t way, AVcb_t cbFunc );
extern AVdat_t  AVt_allocDat ( void *opaque )

#endif /* _AVT_H__ */
