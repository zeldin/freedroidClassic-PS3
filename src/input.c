/* 
 *
 *   Copyright (c) 1994, 2002, 2003  Johannes Prix
 *   Copyright (c) 1994, 2002, 2003  Reinhard Prix
 *
 *
 *  This file is part of Freedroid
 *
 *  Freedroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  Freedroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Freedroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

/*----------------------------------------------------------------------
 *
 * Desc: functions for keyboard and joystick handling
 *
 *----------------------------------------------------------------------*/

#define _input_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

// earlier SDL versions didn't define these...
#ifndef SDL_BUTTON_WHEELUP 
#define SDL_BUTTON_WHEELUP 4
#endif
#ifndef SDL_BUTTON_WHEELDOWN
#define SDL_BUTTON_WHEELDOWN 5
#endif

bool show_cursor;    // show mouse-cursor or not?
#define CURSOR_KEEP_VISIBLE  3000   // ticks to keep mouse-cursor visible without mouse-input

int WheelUpEvents=0;    // count number of not read-out wheel events
int WheelDownEvents=0;
Uint32 last_mouse_event = 0;  // record when last mouse event took place (SDL ticks)

SDLMod current_modifiers;

SDL_Event event;

#ifdef ANDROID
Uint32 last_menu_press = 0;
#endif

int input_state[KEY_PACK(INPUT_LAST)];	// array of states (pressed/released) of all keys

int key_cmds[CMD_LAST][3] =  // array of mappings {key1,key2,key3 -> cmd}
  {
    {SDLK_UP, 	  JOY_UP, 	'w' },		// CMD_UP
    {SDLK_DOWN,	  JOY_DOWN, 	's' },		// CMD_DOWN
    {SDLK_LEFT,   JOY_LEFT, 	'a' },		// CMD_LEFT
    {SDLK_RIGHT,  JOY_RIGHT, 	'd' },		// CMD_RIGHT
    {SDLK_SPACE,  JOY_BUTTON1,   MOUSE_BUTTON1 },// CMD_FIRE  
    {SDLK_RETURN, SDLK_RSHIFT, 	'e' }, 		// CMD_ACTIVATE
    {SDLK_SPACE,  JOY_BUTTON2,   MOUSE_BUTTON2 },// CMD_TAKEOVER
    {'q', 	  'q', 		 'q'  }, 	// CMD_QUIT,
    {SDLK_PAUSE, 'p', 'p'  }, 			// CMD_PAUSE,
    {SDLK_F12, SDLK_F12, SDLK_F12 }  		// CMD_SCREENSHOT
  };

char *keystr[KEY_PACK(INPUT_LAST)];

char *cmd_strings[CMD_LAST] = 
  {
    "UP", 
    "DOWN",
    "LEFT",
    "RIGHT",
    "FIRE",
    "ACTIVATE",
    "TAKEOVER",
    "QUIT",
    "PAUSE",
    "SCREENSHOT"
  };

#ifdef __PPU__
static int joy_button_map[] = {
  SDLK_LEFT,
  SDLK_DOWN,
  SDLK_RIGHT,
  SDLK_UP,
  SDLK_ESCAPE,   // map start button onto 'ESC'
  0 /*R3*/,
  0 /*L3*/,
  0 /*SELECT*/,
  SDLK_RETURN,   // map [] button onto 'RETURN' , i.e Activate
  MOUSE_BUTTON1, // X button onto fire, 
  MOUSE_BUTTON2, // and O button onto takeover
  0 /* /\ */,
  0 /*R1*/,
  0 /*L1*/,
  0 /*R2*/,
  0 /*L2*/
};
#define JOY_BUTTON_COUNT (sizeof(joy_button_map)/sizeof(joy_button_map[0]))
#else
#ifdef ANDROID
static int joy_button_map[] = {
  MOUSE_BUTTON1, // A button onto fire, 
  MOUSE_BUTTON2, // and B button onto takeover
  0 /* C */,
  SDLK_RETURN,   // map X button onto 'RETURN' , i.e Activate
  0 /* Y */,
  0 /* Z */,
  0 /* L1 */,
  0 /* R1 */,
  0 /* L2 */,
  0 /* R2 */,
  0 /* THUMBL */,
  0 /* THUMBR */,
  SDLK_AC_BACK,   // map start button onto 'BACK'
  0 /*SELECT*/,
  0 /*MODE*/
};
#define JOY_BUTTON_COUNT (sizeof(joy_button_map)/sizeof(joy_button_map[0]))
#endif
#endif

#define FRESH_BIT   	(0x01<<8)
#define OLD_BIT		(0x01<<9)
#define LONG_PRESSED	(TRUE|OLD_BIT)
#define PRESSED		(TRUE|FRESH_BIT)
#define RELEASED	(FALSE|FRESH_BIT)

#define is_down(x) ((x) & (~FRESH_BIT) )
#define just_pressed(x) ( (x) & PRESSED == PRESSED)

#define clear_fresh(x) do { (x) &= ~FRESH_BIT; } while(0)


void
init_keystr (void)
{
  keystr[KEY_PACK(SDLK_BACKSPACE)]	= "BS";
  keystr[KEY_PACK(SDLK_TAB)]		= "Tab";
  keystr[KEY_PACK(SDLK_CLEAR)]		= "Clear";
  keystr[KEY_PACK(SDLK_RETURN)]		= "Return";
  keystr[KEY_PACK(SDLK_PAUSE)]		= "Pause";
  keystr[KEY_PACK(SDLK_ESCAPE)]		= "Esc";
  keystr[KEY_PACK(SDLK_SPACE)]		= "Space";
  keystr[KEY_PACK(SDLK_EXCLAIM)]	= "!";
  keystr[KEY_PACK(SDLK_QUOTEDBL)]	= "\"";
  keystr[KEY_PACK(SDLK_HASH)]		= "#";
  keystr[KEY_PACK(SDLK_DOLLAR)]		= "$";
  keystr[KEY_PACK(SDLK_AMPERSAND)]	= "&";
  keystr[KEY_PACK(SDLK_QUOTE)]		= "'";
  keystr[KEY_PACK(SDLK_LEFTPAREN)]	= "(";
  keystr[KEY_PACK(SDLK_RIGHTPAREN)]	=")";
  keystr[KEY_PACK(SDLK_ASTERISK)]	= "*";
  keystr[KEY_PACK(SDLK_PLUS)]		= "+";
  keystr[KEY_PACK(SDLK_COMMA)]		= ",";
  keystr[KEY_PACK(SDLK_MINUS)]		= "-";
  keystr[KEY_PACK(SDLK_PERIOD)]		= ".";
  keystr[KEY_PACK(SDLK_SLASH)]		= "/";
  keystr[KEY_PACK(SDLK_0)]		= "0";	
  keystr[KEY_PACK(SDLK_1)]		= "1";
  keystr[KEY_PACK(SDLK_2)]		= "2";
  keystr[KEY_PACK(SDLK_3)]		= "3";
  keystr[KEY_PACK(SDLK_4)]		= "4";
  keystr[KEY_PACK(SDLK_5)]		= "5";
  keystr[KEY_PACK(SDLK_6)]		= "6";
  keystr[KEY_PACK(SDLK_7)]		= "7";
  keystr[KEY_PACK(SDLK_8)]		= "8";
  keystr[KEY_PACK(SDLK_9)]		= "9";
  keystr[KEY_PACK(SDLK_COLON)]		= ":";
  keystr[KEY_PACK(SDLK_SEMICOLON)]	= ";";
  keystr[KEY_PACK(SDLK_LESS)]		= "<";
  keystr[KEY_PACK(SDLK_EQUALS)]		= "=";
  keystr[KEY_PACK(SDLK_GREATER)]	= ">";
  keystr[KEY_PACK(SDLK_QUESTION)]	= "?";
  keystr[KEY_PACK(SDLK_AT)]		= "@";
  keystr[KEY_PACK(SDLK_LEFTBRACKET)]	= "[";
  keystr[KEY_PACK(SDLK_BACKSLASH)]	= "\\";
  keystr[KEY_PACK(SDLK_RIGHTBRACKET)]	= "]";
  keystr[KEY_PACK(SDLK_CARET)]		= "^";
  keystr[KEY_PACK(SDLK_UNDERSCORE)]	= "_";
  keystr[KEY_PACK(SDLK_BACKQUOTE)]	= "`";
  keystr[KEY_PACK(SDLK_a)]		= "a";
  keystr[KEY_PACK(SDLK_b)]		= "b";
  keystr[KEY_PACK(SDLK_c)]		= "c";
  keystr[KEY_PACK(SDLK_d)]		= "d";
  keystr[KEY_PACK(SDLK_e)]		= "e";
  keystr[KEY_PACK(SDLK_f)]		= "f";
  keystr[KEY_PACK(SDLK_g)]		= "g";
  keystr[KEY_PACK(SDLK_h)]		= "h";
  keystr[KEY_PACK(SDLK_i)]		= "i";
  keystr[KEY_PACK(SDLK_j)]		= "j";
  keystr[KEY_PACK(SDLK_k)]		= "k";
  keystr[KEY_PACK(SDLK_l)]		= "l";
  keystr[KEY_PACK(SDLK_m)]		= "m";
  keystr[KEY_PACK(SDLK_n)]		= "n";
  keystr[KEY_PACK(SDLK_o)]		= "o";
  keystr[KEY_PACK(SDLK_p)]		= "p";
  keystr[KEY_PACK(SDLK_q)]		= "q";
  keystr[KEY_PACK(SDLK_r)]		= "r";
  keystr[KEY_PACK(SDLK_s)]		= "s";
  keystr[KEY_PACK(SDLK_t)]		= "t";
  keystr[KEY_PACK(SDLK_u)]		= "u";
  keystr[KEY_PACK(SDLK_v)]		= "v";
  keystr[KEY_PACK(SDLK_w)]		= "w";
  keystr[KEY_PACK(SDLK_x)]		= "x";
  keystr[KEY_PACK(SDLK_y)]		= "y";
  keystr[KEY_PACK(SDLK_z)]		= "z";
  keystr[KEY_PACK(SDLK_DELETE)]		= "Del";


  /* Numeric keypad */
  keystr[KEY_PACK(SDLK_KP0)]		= "Num[0]";
  keystr[KEY_PACK(SDLK_KP1)]		= "Num[1]";
  keystr[KEY_PACK(SDLK_KP2)]		= "Num[2]";
  keystr[KEY_PACK(SDLK_KP3)]		= "Num[3]";
  keystr[KEY_PACK(SDLK_KP4)]		= "Num[4]";
  keystr[KEY_PACK(SDLK_KP5)]		= "Num[5]";
  keystr[KEY_PACK(SDLK_KP6)]		= "Num[6]";
  keystr[KEY_PACK(SDLK_KP7)]		= "Num[7]";
  keystr[KEY_PACK(SDLK_KP8)]		= "Num[8]";
  keystr[KEY_PACK(SDLK_KP9)]		= "Num[9]";
  keystr[KEY_PACK(SDLK_KP_PERIOD)]	= "Num[.]";
  keystr[KEY_PACK(SDLK_KP_DIVIDE)]	= "Num[/]";
  keystr[KEY_PACK(SDLK_KP_MULTIPLY)]	= "Num[*]";
  keystr[KEY_PACK(SDLK_KP_MINUS)]	= "Num[-]";
  keystr[KEY_PACK(SDLK_KP_PLUS)]	= "Num[+]";
  keystr[KEY_PACK(SDLK_KP_ENTER)]	= "Num[Enter]";
  keystr[KEY_PACK(SDLK_KP_EQUALS)]	= "Num[=]";

  /* Arrows + Home/End pad */
  keystr[KEY_PACK(SDLK_UP)]		= "Up";
  keystr[KEY_PACK(SDLK_DOWN)]		= "Down";
  keystr[KEY_PACK(SDLK_RIGHT)]		= "Right";
  keystr[KEY_PACK(SDLK_LEFT)]		= "Left";
  keystr[KEY_PACK(SDLK_INSERT)]		= "Insert";
  keystr[KEY_PACK(SDLK_HOME)]		= "Home";
  keystr[KEY_PACK(SDLK_END)]		= "End";
  keystr[KEY_PACK(SDLK_PAGEUP)]		= "PageUp";
  keystr[KEY_PACK(SDLK_PAGEDOWN)]	= "PageDown";

  /* Function keys */
  keystr[KEY_PACK(SDLK_F1)]		= "F1";
  keystr[KEY_PACK(SDLK_F2)]		= "F2";
  keystr[KEY_PACK(SDLK_F3)]		= "F3";
  keystr[KEY_PACK(SDLK_F4)]		= "F4";
  keystr[KEY_PACK(SDLK_F5)]		= "F5";
  keystr[KEY_PACK(SDLK_F6)]		= "F6";
  keystr[KEY_PACK(SDLK_F7)]		= "F7";
  keystr[KEY_PACK(SDLK_F8)]		= "F8";
  keystr[KEY_PACK(SDLK_F9)]		= "F9";
  keystr[KEY_PACK(SDLK_F10)]		= "F10";
  keystr[KEY_PACK(SDLK_F11)]		= "F11";
  keystr[KEY_PACK(SDLK_F12)]		= "F12";
  keystr[KEY_PACK(SDLK_F13)]		= "F13";
  keystr[KEY_PACK(SDLK_F14)]		= "F14";
  keystr[KEY_PACK(SDLK_F15)]		= "F15";

  /* Key state modifier keys */
  keystr[KEY_PACK(SDLK_NUMLOCK)]	= "NumLock";
  keystr[KEY_PACK(SDLK_CAPSLOCK)]	= "CapsLock";
  keystr[KEY_PACK(SDLK_SCROLLOCK)]	= "ScrlLock";
  keystr[KEY_PACK(SDLK_RSHIFT)]		= "RShift";
  keystr[KEY_PACK(SDLK_LSHIFT)]		= "LShift";
  keystr[KEY_PACK(SDLK_RCTRL)]		= "RCtrl";
  keystr[KEY_PACK(SDLK_LCTRL)]		= "LCtrl";
  keystr[KEY_PACK(SDLK_RALT)]		= "RAlt";
  keystr[KEY_PACK(SDLK_LALT)]		= "LAlt";
  keystr[KEY_PACK(SDLK_RMETA)]		= "RMeta";
  keystr[KEY_PACK(SDLK_LMETA)]		= "LMeta";
  keystr[KEY_PACK(SDLK_LSUPER)]		= "LSuper";
  keystr[KEY_PACK(SDLK_RSUPER)]		= "RSuper";
  keystr[KEY_PACK(SDLK_MODE)]		= "Mode";
  keystr[KEY_PACK(SDLK_COMPOSE)]	= "Compose";
    
  /* Miscellaneous function keys */
  keystr[KEY_PACK(SDLK_HELP)]		= "Help";
  keystr[KEY_PACK(SDLK_PRINT)]		= "Print";
  keystr[KEY_PACK(SDLK_SYSREQ)]		= "SysReq";
  keystr[KEY_PACK(SDLK_BREAK)]		= "Break";
  keystr[KEY_PACK(SDLK_MENU)]		= "Menu";
  keystr[KEY_PACK(SDLK_POWER)]		= "Power";
  keystr[KEY_PACK(SDLK_EURO)]		= "Euro";
  keystr[KEY_PACK(SDLK_UNDO)]		= "Undo";

  /* Mouse und Joy buttons */
  keystr[KEY_PACK(MOUSE_BUTTON1)] 	= "Mouse1";
  keystr[KEY_PACK(MOUSE_BUTTON2)] 	= "Mouse2";
  keystr[KEY_PACK(MOUSE_BUTTON3)] 	= "Mouse3";
  keystr[KEY_PACK(MOUSE_WHEELUP)] 	= "WheelUp";
  keystr[KEY_PACK(MOUSE_WHEELDOWN)]	="WheelDown";

  keystr[KEY_PACK(JOY_UP)]		= "JoyUp";
  keystr[KEY_PACK(JOY_DOWN)]		= "JoyDown"; 
  keystr[KEY_PACK(JOY_LEFT)]		= "JoyLeft"; 
  keystr[KEY_PACK(JOY_RIGHT)]		= "JoyRight"; 
  keystr[KEY_PACK(JOY_BUTTON1)] 	= "Joy1";
  keystr[KEY_PACK(JOY_BUTTON2)] 	= "Joy2";
  keystr[KEY_PACK(JOY_BUTTON3)] 	= "Joy3";

  return;
} // init_keystr()


int sgn (int x)
{
  return (x ? ((x)/abs(x)) : 0);
}

void Init_Joy (void)
{
  int num_joy;

  if (SDL_InitSubSystem (SDL_INIT_JOYSTICK) == -1)
    {
      fprintf(stderr, "Couldn't initialize SDL-Joystick: %s\n", SDL_GetError());
      Terminate(ERR);
    } else
      DebugPrintf(1, "\nSDL Joystick initialisation successful.\n");


  DebugPrintf (1, " %d Joysticks found!\n", num_joy = SDL_NumJoysticks ());

  if (num_joy > 0)
    joy = SDL_JoystickOpen (0);

  if (joy)
    {
      DebugPrintf (1, "Identifier: %s\n", SDL_JoystickName (0));
      DebugPrintf (1, "Number of Axes: %d\n", joy_num_axes = SDL_JoystickNumAxes(joy));
      DebugPrintf (1, "Number of Buttons: %d\n", SDL_JoystickNumButtons(joy));

      /* aktivate Joystick event handling */
      SDL_JoystickEventState (SDL_ENABLE); 

    }
  else 
    joy = NULL;  /* signals that no yoystick is present */


  return;
}

// FIXME: remove that obsolete stuff...
void 
ReactToSpecialKeys(void)
{

  if ( cmd_is_active(CMD_QUIT) ) 
    QuitGameMenu();

  if ( cmd_is_activeR(CMD_PAUSE) )
    Pause ();

  if ( cmd_is_active (CMD_SCREENSHOT) )
    TakeScreenshot();

  // this stuff remains hardcoded to keys
  if ( KeyIsPressedR('c') && AltPressed() && CtrlPressed() && ShiftPressed() ) 
    Cheatmenu ();

  if ( EscapePressedR() )
    EscapeMenu ();


} // void ReactToSpecialKeys(void)

//----------------------------------------------------------------------
// main input-reading routine
//----------------------------------------------------------------------
int 
update_input (void)
{
  Uint8 axis; 

  // switch mouse-cursor visibility as a function of time of last activity
  if (SDL_GetTicks () - last_mouse_event > CURSOR_KEEP_VISIBLE)
    show_cursor = FALSE;
  else
    show_cursor = TRUE;

  while( SDL_PollEvent( &event ) )
    {
      switch( event.type )
	{
	case SDL_QUIT:
	  printf("\n\nUser requestet Termination...\n\nTerminating...");
	  Terminate(0);
	  break;
	  /* Look for a keypress */

	case SDL_KEYDOWN:
#ifdef ANDROID
	  if (event.key.keysym.sym == SDLK_MENU)
	    last_menu_press = SDL_GetTicks();
#endif
	  current_modifiers = event.key.keysym.mod;
	  input_state[KEY_PACK(event.key.keysym.sym)] = PRESSED;
	  break;
	case SDL_KEYUP:
	  current_modifiers = event.key.keysym.mod;
	  input_state[KEY_PACK(event.key.keysym.sym)] = RELEASED;
	  break;

	case SDL_JOYAXISMOTION:
	  axis = event.jaxis.axis;
	  if (axis == 0 || ((joy_num_axes >= 5) && (axis == 3)) ) /* x-axis */
	    {
#if !defined(__PPU__) && !defined(ANDROID)
	      input_axis.x = event.jaxis.value;
#endif

	      // this is a bit tricky, because we want to allow direction keys
	      // to be soft-released. When mapping the joystick->keyboard, we 
	      // therefore have to make sure that this mapping only occurs when
	      // and actual _change_ of the joystick-direction ('digital') occurs
	      // so that it behaves like "set"/"release"
	      if (joy_sensitivity*event.jaxis.value > 10000)   /* about half tilted */
		{
		  if (input_state[KEY_PACK(JOY_RIGHT)] == RELEASED)
		      input_state[KEY_PACK(JOY_RIGHT)] = PRESSED;
		  input_state[KEY_PACK(JOY_LEFT)] = RELEASED;
		}
	      else if (joy_sensitivity*event.jaxis.value < -10000)
		{
		  if (input_state[KEY_PACK(JOY_LEFT)] == RELEASED)
		      input_state[KEY_PACK(JOY_LEFT)] = PRESSED;
		  input_state[KEY_PACK(JOY_RIGHT)] = RELEASED;
		}
	      else
		{
		  input_state[KEY_PACK(JOY_LEFT)] = RELEASED;
		  input_state[KEY_PACK(JOY_RIGHT)] = RELEASED;
		}
	    }
	  else if ((axis == 1) || ((joy_num_axes >=5) && (axis == 4))) /* y-axis */
	    {
#if !defined(__PPU__) && !defined(ANDROID)
	      input_axis.y = event.jaxis.value;
#endif

	      if (joy_sensitivity*event.jaxis.value > 10000)  
		{
		  if (input_state[KEY_PACK(JOY_DOWN)] == RELEASED)
		      input_state[KEY_PACK(JOY_DOWN)] = PRESSED;
		  input_state[KEY_PACK(JOY_UP)] =  RELEASED;
		}
	      else if (joy_sensitivity*event.jaxis.value < -10000)
		{
		  if (input_state[KEY_PACK(JOY_UP)] == RELEASED)
		      input_state[KEY_PACK(JOY_UP)] = PRESSED;
		  input_state[KEY_PACK(JOY_DOWN)]= RELEASED;
		}
	      else
		{
		  input_state[KEY_PACK(JOY_UP)] = RELEASED;
		  input_state[KEY_PACK(JOY_DOWN)] = RELEASED;
		}
	    }
#if defined(__PPU__) || defined(ANDROID)
	  else if (axis == 2 || axis == 3) {
	    int active;
	    if (axis == 2)
	      input_axis.x = event.jaxis.value;
	    else
	      input_axis.y = event.jaxis.value;
	    active =
	      (input_axis.x < -10000 || input_axis.x > 10000 ||
	       input_axis.y < -10000 || input_axis.y > 10000);
	    if (!active)
	      input_state[KEY_PACK(JOY_BUTTON1)] = RELEASED;
	    else if(input_state[KEY_PACK(JOY_BUTTON1)] == RELEASED)
	      input_state[KEY_PACK(JOY_BUTTON1)] = PRESSED;
	    axis_is_active = active;
	  }
#endif
		
	  break;
	  
	case SDL_JOYBUTTONDOWN: 
#ifdef JOY_BUTTON_COUNT
	  if (event.jbutton.button < JOY_BUTTON_COUNT)
	    input_state[KEY_PACK(joy_button_map[event.jbutton.button])] = PRESSED;
#else
	  // first button
	  if (event.jbutton.button == 0)
	    input_state[KEY_PACK(JOY_BUTTON1)] = PRESSED;

	  // second button
	  else if (event.jbutton.button == 1) 
	    input_state[KEY_PACK(JOY_BUTTON2)] = PRESSED;

	  // and third button
	  else if (event.jbutton.button == 2) 
	    input_state[KEY_PACK(JOY_BUTTON3)] = PRESSED;
#endif

#if !defined(__PPU__) && !defined(ANDROID)
	  axis_is_active = TRUE;
#endif
	  break;

	case SDL_JOYBUTTONUP:
#ifdef JOY_BUTTON_COUNT
	  if (event.jbutton.button < JOY_BUTTON_COUNT)
	    input_state[KEY_PACK(joy_button_map[event.jbutton.button])] = FALSE;
#else
	  // first button 
	  if (event.jbutton.button == 0)
	    input_state[KEY_PACK(JOY_BUTTON1)] = FALSE;

	  // second button
	  else if (event.jbutton.button == 1) 
	    input_state[KEY_PACK(JOY_BUTTON2)] = FALSE;

	  // and third button
	  else if (event.jbutton.button == 2) 
	    input_state[KEY_PACK(JOY_BUTTON3)] = FALSE;
#endif

#if !defined(__PPU__) && !defined(ANDROID)
	  axis_is_active = FALSE;
#endif
	  break;

	case SDL_MOUSEMOTION:
	  input_axis.x = event.button.x - UserCenter_x + 16; 
	  input_axis.y = event.button.y - UserCenter_y + 16; 	  

	  last_mouse_event = SDL_GetTicks ();

	  break;
	  
	  /* Mouse control */
	case SDL_MOUSEBUTTONDOWN:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      input_state[KEY_PACK(MOUSE_BUTTON1)] = PRESSED;
	      axis_is_active = TRUE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    input_state[KEY_PACK(MOUSE_BUTTON2)] = PRESSED;

	  if (event.button.button == SDL_BUTTON_MIDDLE)  
	    input_state[KEY_PACK(MOUSE_BUTTON3)] = PRESSED;

	  // wheel events are immediately released, so we rather
	  // count the number of not yet read-out events
	  if (event.button.button == SDL_BUTTON_WHEELUP)
	      WheelUpEvents ++;

	  if (event.button.button == SDL_BUTTON_WHEELDOWN)
	      WheelDownEvents ++;

	  last_mouse_event = SDL_GetTicks();
	  break;

        case SDL_MOUSEBUTTONUP:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    {
	      input_state[KEY_PACK(MOUSE_BUTTON1)] = FALSE;
	      axis_is_active = FALSE;
	    }

	  if (event.button.button == SDL_BUTTON_RIGHT)
	    input_state[KEY_PACK(MOUSE_BUTTON2)] = FALSE;

	  if (event.button.button == SDL_BUTTON_MIDDLE)
	    input_state[KEY_PACK(MOUSE_BUTTON3)] = FALSE;

	  break;

 	default:
 	  break;
 	}

    }

  return 0;
}

/*-----------------------------------------------------------------
 * Desc: should do roughly what getchar() does, but in raw 
 * 	 (SLD) keyboard mode. 
 * 
 * Return: the (SDLKey) of the next key-pressed event cast to (int)
 *
 *-----------------------------------------------------------------*/
int
getchar_raw (void)
{
  SDL_Event event;
  int Returnkey = 0;
  
  //  keyboard_update ();   /* treat all pending keyboard-events */

  while ( !Returnkey )
    {
      while (!SDL_WaitEventTimeout (&event, 10))    /* wait for next event */
	SDL_Flip (ne_screen);
      
      switch (event.type)
	{
	case SDL_KEYDOWN:
	  /* 
	   * here we use the fact that, I cite from SDL_keyboard.h:
	   * "The keyboard syms have been cleverly chosen to map to ASCII"
	   * ... I hope that this design feature is portable, and durable ;)  
	   */
	  Returnkey = (int) event.key.keysym.sym;
	  if ( event.key.keysym.mod & KMOD_SHIFT ) 
	    Returnkey = toupper( (int)event.key.keysym.sym );
	  break;

	case SDL_JOYBUTTONDOWN: 
#ifdef JOY_BUTTON_COUNT
	  if (event.jbutton.button < JOY_BUTTON_COUNT)
	    Returnkey = joy_button_map[event.jbutton.button];
#else
	  if (event.jbutton.button == 0)
	    Returnkey = JOY_BUTTON1;
	  else if (event.jbutton.button == 1) 
	    Returnkey = JOY_BUTTON2;
	  else if (event.jbutton.button == 2) 
	    Returnkey = JOY_BUTTON3;
#endif
	  break;

	case SDL_MOUSEBUTTONDOWN:
	  if (event.button.button == SDL_BUTTON_LEFT)
	    Returnkey = MOUSE_BUTTON1;
	  else if (event.button.button == SDL_BUTTON_RIGHT)
	    Returnkey = MOUSE_BUTTON2;
	  else if (event.button.button == SDL_BUTTON_MIDDLE)  
	    Returnkey = MOUSE_BUTTON3;
	  else if (event.button.button == SDL_BUTTON_WHEELUP)
	    Returnkey = MOUSE_WHEELUP;
	  else if (event.button.button == SDL_BUTTON_WHEELDOWN)
	    Returnkey = MOUSE_WHEELDOWN;
	  break;

	default:
	  SDL_PushEvent (&event);  /* put this event back into the queue */
	  update_input ();  /* and treat it the usual way */
	  continue;
	}

    } /* while(1) */

  return ( Returnkey );

} /* getchar_raw() */

// forget the wheel-counters
void
ResetMouseWheel (void)
{
  WheelUpEvents = WheelDownEvents = 0;
  return;
}

bool
WheelUpPressed (void)
{
  update_input();
  if (WheelUpEvents)
    return (WheelUpEvents--);
  else
    return (FALSE);
}

bool
WheelDownPressed (void)
{
  update_input();
  if (WheelDownEvents)
    return (WheelDownEvents--);
  else
    return (FALSE);
}

bool
KeyIsPressed (SDLKey key)
{
  update_input();

  return( (input_state[KEY_PACK(key)] & PRESSED) == PRESSED );
}


// does the same as KeyIsPressed, but automatically releases the key as well..
bool
KeyIsPressedR (SDLKey key)
{
  bool ret;

  ret = KeyIsPressed (key);

  ReleaseKey (key);
  return (ret);
}

bool
MenuPressedR (void)
{
  bool ret =
    (last_menu_press != 0 &&
     ((Uint32)(SDL_GetTicks()-last_menu_press)) < 500);
  last_menu_press = 0;
  return ret;
}

void 
ReleaseKey (SDLKey key)
{
  input_state[KEY_PACK(key)] = FALSE;
  return;
}

bool
ModIsPressed (SDLMod mod)
{
  bool ret;
  update_input();
  ret = ( (current_modifiers & mod) != 0) ;
  return (ret);
}

bool
NoDirectionPressed (void)
{
  if ( (axis_is_active && (input_axis.x || input_axis.y)) ||
      DownPressed () || UpPressed() || LeftPressed() || RightPressed() )
    return ( FALSE );
  else
    return ( TRUE );
} // int NoDirectionPressed(void)


//----------------------------------------------------------------------
// check if a particular key has been pressed



// check if any keys or buttons1 are pressed
bool
any_key_pressed (void)
{
  int i;
  bool ret = FALSE;

  update_input();

  for (i=0; i<KEY_PACK(SDLK_LAST); i++)
    if ( just_pressed(input_state[i]) )
      { 
	clear_fresh(input_state[i]);
	ret = TRUE; 
	break;
      }
  if ( just_pressed(input_state[KEY_PACK(JOY_BUTTON1)]) )
    {
      clear_fresh (input_state[KEY_PACK(JOY_BUTTON1)]);
      ret = TRUE;
    }

  if ( just_pressed(input_state[KEY_PACK(MOUSE_BUTTON1)]) )
    {
      ret = TRUE;
      clear_fresh (input_state[KEY_PACK(MOUSE_BUTTON1)]);
    }

  return (ret);

}  // any_key_pressed()

bool
cmd_is_active (enum _cmds cmd)
{
  if (cmd >= CMD_LAST)
    {
      DebugPrintf (0, "ERROR: Illegal command '%d'\n", cmd);
      Terminate (ERR);
    }

  if ( KeyIsPressed( key_cmds[cmd][0] ) || 
       KeyIsPressed( key_cmds[cmd][1] ) || 
       KeyIsPressed( key_cmds[cmd][2] ))
    return (TRUE);
  else
    return (FALSE);

} // cmd_is_active()

// --------------------------------------------------
// the same but release the keys: use only for menus!
// --------------------------------------------------
bool
cmd_is_activeR (enum _cmds cmd)
{
  if (cmd >= CMD_LAST)
    {
      DebugPrintf (0, "ERROR: Illegal command '%d'\n", cmd);
      Terminate (ERR);
    }

  if ( KeyIsPressedR( key_cmds[cmd][0] ) || 
       KeyIsPressedR( key_cmds[cmd][1] ) || 
       KeyIsPressedR( key_cmds[cmd][2] ))
    return (TRUE);
  else
    return (FALSE);

} // cmd_is_active()

#undef _intput_c
