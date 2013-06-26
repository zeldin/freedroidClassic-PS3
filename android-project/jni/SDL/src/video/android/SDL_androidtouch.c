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

#include <android/log.h>

#include "SDL_events.h"
#include "../../events/SDL_mouse_c.h"
#include "../../events/SDL_touch_c.h"

#include "SDL_androidtouch.h"


#define ACTION_DOWN 0
#define ACTION_UP 1
#define ACTION_MOVE 2
#define ACTION_CANCEL 3
#define ACTION_OUTSIDE 4
// The following two are deprecated but it seems they are still emitted (instead the corresponding ACTION_UP/DOWN) as of Android 3.2
#define ACTION_POINTER_1_DOWN 5
#define ACTION_POINTER_1_UP 6

void Android_OnTouch(int touch_device_id_in, int pointer_finger_id_in, int action, float x, float y, float p) 
{
    SDL_TouchID touchDeviceId = 0;
    SDL_FingerID fingerId = 0;
    
    if (!Android_Window) {
        return;
    }
    
    touchDeviceId = (SDL_TouchID)touch_device_id_in;
    if (!SDL_GetTouch(touchDeviceId)) {
        SDL_Touch touch;
        memset( &touch, 0, sizeof(touch) );
        touch.id = touchDeviceId;
        touch.x_min = 0.0f;
        touch.x_max = (float)Android_ScreenWidth;
        touch.native_xres = touch.x_max - touch.x_min;
        touch.y_min = 0.0f;
        touch.y_max = (float)Android_ScreenHeight;
        touch.native_yres = touch.y_max - touch.y_min;
        touch.pressure_min = 0.0f;
        touch.pressure_max = 1.0f;
        touch.native_pressureres = touch.pressure_max - touch.pressure_min;
        if (SDL_AddTouch(&touch, "") < 0) {
             SDL_Log("error: can't add touch %s, %d", __FILE__, __LINE__);
        }
    }

    
    fingerId = (SDL_FingerID)pointer_finger_id_in;
    switch (action) {
        case ACTION_DOWN:
        case ACTION_POINTER_1_DOWN:
            SDL_SendFingerDown(touchDeviceId, fingerId, SDL_TRUE, x, y, p);
            break;
        case ACTION_MOVE:
            SDL_SendTouchMotion(touchDeviceId, fingerId, SDL_FALSE, x, y, p);
            break;
        case ACTION_UP:
        case ACTION_POINTER_1_UP:
            SDL_SendFingerDown(touchDeviceId, fingerId, SDL_FALSE, x, y, p);
            break;
        default:
            break;
    } 
}

/* vi: set ts=4 sw=4 expandtab: */
