/*
=INSERT_TEMPLATE_HERE=

$Id: ProdCon.h,v 1.9 2013/05/27 17:45:01 istakenv Exp $

General functions declarations.

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


#ifndef __FREEWRL_PRODCON_MAIN_H__
#define __FREEWRL_PRODCON_MAIN_H__


void registerBindable(struct X3D_Node *);

/* note , this should probably move to a different header and not be implemented in ProdCon */
int EAI_CreateX3d(const char *tp, const char *inputstring, struct X3D_Group *where);

#endif /* __FREEWRL_PRODCON_MAIN_H__ */
