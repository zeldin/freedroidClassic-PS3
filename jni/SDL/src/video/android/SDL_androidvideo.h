/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2011 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#ifndef _SDL_androidvideo_h
#define _SDL_androidvideo_h

#include "SDL_mutex.h"
#include "../SDL_sysvideo.h"

/* Called by the JNI layer when the screen changes size or format */
extern void Android_SetScreenResolution(int width, int height, Uint32 format);

/* Private display data */

extern int Android_ScreenWidth;
extern int Android_ScreenHeight;
extern Uint32 Android_ScreenFormat;
extern SDL_sem *Android_PauseSem, *Android_ResumeSem;
extern SDL_Window *Android_Window;


#endif /* _SDL_androidvideo_h */

/* vi: set ts=4 sw=4 expandtab: */
