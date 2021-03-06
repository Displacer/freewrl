/*
=INSERT_TEMPLATE_HERE=

$Id: system_net.h,v 1.9 2011/05/17 13:58:29 crc_canada Exp $

FreeWRL support library.
Internal header: network dependencies.

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


#ifndef __LIBFREEWRL_SYSTEM_NET_H__
#define __LIBFREEWRL_SYSTEM_NET_H__

#if !defined(WIN32)

#if HAVE_SYS_SOCKET_H
# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif
# include <sys/socket.h>
#endif

#include <netinet/in.h> 
#if defined(_ANDROID)
#include <linux/msg.h>
#else
#include <sys/msg.h> 
#endif

#else

#include <winsock.h>

#endif


#endif /* __LIBFREEWRL_SYSTEM_NET_H__ */
