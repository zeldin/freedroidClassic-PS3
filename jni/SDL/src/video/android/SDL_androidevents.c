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

#include "SDL_androidevents.h"

void
Android_PumpEvents(_THIS)
{
    static int isPaused = 0;
    /* No polling necessary */

    /*
     * Android_ResumeSem and Android_PauseSem are signaled from Java_org_libsdl_app_SDLActivity_nativePause and Java_org_libsdl_app_SDLActivity_nativeResume
     * When the pause semaphoe is signaled, if SDL_ANDROID_BLOCK_ON_PAUSE is defined the event loop will block until the resume signal is emitted.
     * When the resume semaphore is signaled, SDL_GL_CreateContext is called which in turn calls Java code
     * SDLActivity::createGLContext -> SDLActivity:: initEGL -> SDLActivity::createEGLSurface -> SDLActivity::createEGLContext
     */
    if (isPaused) {
#if SDL_ANDROID_BLOCK_ON_PAUSE
        if(SDL_SemWait(Android_ResumeSem) == 0) {
#else
        if(SDL_SemTryWait(Android_ResumeSem) == 0) {
#endif
            isPaused = 0;
            /* TODO: Should we double check if we are on the same thread as the one that made the original GL context?
             * This call will go through the following chain of calls in Java:
             * SDLActivity::createGLContext -> SDLActivity:: initEGL -> SDLActivity::createEGLSurface -> SDLActivity::createEGLContext
             * SDLActivity::createEGLContext will attempt to restore the GL context first, and if that fails it will create a new one
             * If a new GL context is created, the user needs to restore the textures manually (TODO: notify the user that this happened with a message)
             */
            SDL_GL_CreateContext(Android_Window);
        }
    }
    else {
        if(SDL_SemTryWait(Android_PauseSem) == 0) {
            /* If we fall in here, the system is/was paused */
            isPaused = 1;
        }
    }
}

/* vi: set ts=4 sw=4 expandtab: */
