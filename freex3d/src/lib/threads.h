/*
  $Id: threads.h,v 1.13 2013/08/23 15:27:27 dug9 Exp $

  FreeWRL support library.
  Threads & process (fork).

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


#ifndef __LIBFREEWRL_THREADS_H__
#define __LIBFREEWRL_THREADS_H__

/* for some reason, OSX now needs this one, too (July 2010) */
#ifdef AQUA
#include <system_threads.h>
#endif

#ifdef _MSC_VER
#include <system_threads.h>
#endif

#ifdef _ANDROID
#include <system_threads.h>
#endif



int freewrlSystem(const char *string);

/**
 *   Gather here all thread function prototypes & thread variables
 *
 *   We have 4[5] threads in FreeWRL:
 *   1: main thread
 *   2: display thread
 *   [3: shape compiler ONLY on Mac and if OpenGL multi-threaded]
 *   4: input parser
 *   5: texture loader
 */

#define FREEWRL_MAX_THREADS 5

#define FREEWRL_THREAD_MAIN 1
#define FREEWRL_THREAD_DISPLAY 2
#define FREEWRL_THREAD_PARSER 3
#define FREEWRL_THREAD_TEXTURE 4

extern pthread_t mainThread; /* main (default) thread */
extern pthread_t DispThrd;   /* display thread */
extern pthread_t PCthread;   /* parser thread */
extern pthread_t loadThread; /* texture thread */

/**
 *   Gather here all functions that create threads 
 */
/* DISPLAY THREAD */
#if !defined(FRONTEND_HANDLES_DISPLAY_THREAD)
void _displayThread(void *globalcontext);
#endif /* FRONTEND_HANDLES_DISPLAY_THREAD */

/* PARSER THREAD */
void _inputParseThread (void *globalcontext);

/* TEXTURE THREAD */
void _textureThread(void *globalcontext);

int fw_thread_id();
#ifdef FREEWRL_THREAD_COLORIZED
int fw_thread_color(int thread_id);
#endif
/* void fw_thread_dump(); */

#define ENTER_THREAD(_str) trace_enter_thread(_str)
void trace_enter_thread(const char *str);

extern pthread_mutex_t mutex_resource_tree;
extern pthread_mutex_t mutex_resource_list;
extern pthread_mutex_t mutex_texture_list;
extern pthread_cond_t texture_list_condition;
extern pthread_cond_t resource_list_condition;


#endif /* __LIBFREEWRL_THREADS_H__ */
