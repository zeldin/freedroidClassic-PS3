/*----------------------------------------------------------------------
 *
 * Desc:  all functions dealing with sound are contained in this file.
 *
 *----------------------------------------------------------------------*/

/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
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
#ifndef _sound_c
#define _sound_c
#endif

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"



// The following is the definition of the sound file names used in freedroid
// DO NOT EVER CHANGE THE ORDER OF APPEARENCE IN THIS LIST PLEASE!!!!!
// The order of appearance here should match the order of appearance 
// in the enum-Environment located in defs.h!

#define ALL_SOUNDS 24
char *SoundSampleFilenames[ALL_SOUNDS] = {
  "/../sound/ERRORSOUND_NILL",
  "/../sound/Combat_Background_Music.wav",
  "/../sound/Takeover_Background_Music.wav",
  "/../sound/Console_Background_Music.wav",
  "/../sound/Classical_Beep_Beep_Background_Music.wav",
  "/../sound/Blast_Sound_0.wav",
  "/../sound/Collision_Sound_0.wav",
  "/../sound/GotIntoBlast_Sound_0.wav",
  "/../sound/MoveElevator_Sound_0.wav",
  "/../sound/Refresh_Sound_0.wav",
  "/../sound/LeaveElevator_Sound_0.wav",
  "/../sound/EnterElevator_Sound_0.wav",
  "/../sound/ThouArtDefeated_Sound_0.wav",
  "/../sound/Got_Hit_Sound_0.wav",
  "/../sound/TakeoverSetCapsule_Sound_0.wav",
  "/../sound/Menu_Item_Selected_Sound_0.wav",
  "/../sound/Move_Menu_Position_Sound_0.wav",
  "/../sound/Takeover_Game_Won_Sound_0.wav",
  "/../sound/Takeover_Game_Deadlock_Sound_0.wav",
  "/../sound/Takeover_Game_Lost_Sound_0.wav",
  "/../sound/Fire_Bullet_Pulse_Sound_0.wav",
  "/../sound/Fire_Bullet_Single_Pulse_Sound_0.wav",
  "/../sound/Fire_Bullet_Military_Sound_0.wav",
  "/../sound/Fire_Bullet_Flash_Sound_0.wav"
};

char *ExpandedSoundSampleFilenames[ALL_SOUNDS];

char *ExpandFilename(char *LocalFilename);

#if HAVE_LIBY2
// The following Lines define several channels for sound output to the yiff sound server!!!
// Background Music Cannel 
YConnection *BackgroundMusic_con;
YEventSoundObjectAttributes BackgroundMusic_sndobj_attrib;
YID BackgroundMusic_play_id;
YID play_id;
YEvent event;
int handle = -1;
int setting = 0x000C000D;	// 12 fragments size 8kb
int channels = 0;		// 0=mono 1=stereo
int format = AFMT_U8;
int rate = 8000;
// The above Lines define several channels for sound output to the yiff sound server!!!
#endif /* HAVE_LIBY2 */

#ifdef USE_SDL_AUDIO
Mix_Chunk *Loaded_WAV_Files[ALL_SOUNDS];
int Background_Music_Channel = -1;
#endif


void 
Init_Audio(void)
{
  int i;
#ifdef USE_SDL_AUDIO
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16; 
  int audio_channels = 2;
  //  int audio_buffers = 4096;
  int audio_buffers = 2048;
#endif

  // At first we set audio volume to medium value.
  // This might be replaced later with values from a 
  // private user Freedroid config file.  But for now
  // this code is good enough...
  Current_BG_Music_Volume=0.5;
  Current_Sound_FX_Volume=0.5;

  /* Because the yiff does not nescessarily have the same origin for */
  /* relative paths as paradroid does, it is nescessary to first translate */
  /* our path names to absolute pathnames.  */
  // Also is the yiff is not used at all, the expanded file names cannot
  // hurt at all, since they are convenient for the SDL MIXER too.

  for (i = 0; i < ALL_SOUNDS; i++)
    {
      ExpandedSoundSampleFilenames[i] =
	ExpandFilename (SoundSampleFilenames[i]);
    }


#ifdef USE_SDL_AUDIO

  // Only if SDL AUDIO is used for sound output may SDL_AUDIO
  // or SDL_TIMER be initialized.  Otherwise the yiff will not
  // start up later.  So we really cannot do both here!

  if ( SDL_InitSubSystem ( SDL_INIT_TIMER ) == -1 ) 
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      printf("\nSDL Timer initialisation successful.\n");
  if ( SDL_InitSubSystem ( SDL_INIT_AUDIO ) == -1 ) 
    {
      fprintf(stderr, "Couldn't initialize SDL: %s\n",SDL_GetError());
      Terminate(ERR);
    } else
      printf("\nSDL Audio initialisation successful.\n");

  // Now that we have initialized the audio SubSystem, we must open
  // an audio channel.  This will be done here (see code from Mixer-Tutorial):

  if ( Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) ) 
    {
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL AUDIO CHANNEL COULD NOT BE OPEND.\n\
\n\
Please check that your sound card is properly configured,\n\
i.e. if other applications are able to play sounds.\n\
\n\
If you for some reason cannot get your sound card ready, \n\
you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" );
      Terminate (ERR);
    }
  else 
    {
      printf("\nSuccessfully loaded sound file %s.", 
	     ExpandedSoundSampleFilenames[ COMBAT_BACKGROUND_MUSIC_SOUND ] );
    }

  // Now that the audio channel is opend, its time to load all the
  // WAV files into memory, something we NEVER did while using the yiff,
  // because the yiff did all the loading, analyzing and playing...

  Loaded_WAV_Files[0]=NULL;
  for (i = 1; i < ALL_SOUNDS; i++)
    {
      Loaded_WAV_Files[i] = Mix_LoadWAV( ExpandedSoundSampleFilenames[ i ] );
      if ( !Loaded_WAV_Files[i] )
	{
	  fprintf (stderr,
		   "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL MIXER WAS UNABLE TO LOAD A CERTAIN FILE INTO MEMORY.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
If the problem persists and you do not find this sound file in the\n\
Freedroid archive, please inform the developers about the problem.\n\
\n\
In the meantime you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , ExpandedSoundSampleFilenames[ i ]);
	  Terminate (ERR);
	} // if ( !Loaded_WAV...
      else
	{
	  printf("\nSuccessfully loaded file %s.", ExpandedSoundSampleFilenames[i]);
	}
    } // for (i=0; ...



#else

  // Only if sound output via YIFF and NOT via SDL is used,
  // then we init the YIFF sound server.
  Init_YIFF_Sound_Server ();

#endif



}


void
ExitProc ()
{
  if (handle != -1)
      close (handle);
}

int i;
unsigned char *ptr;
unsigned char v = 128;
int SampleLaenge;


void 
Set_BG_Music_Volume(float NewVolume)
{
  Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );
} // void Set_BG_Music_Volume(float NewVolume)

void 
Set_Sound_FX_Volume(float NewVolume)
{
  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );
} // void Set_BG_Music_Volume(float NewVolume)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void 
YIFF_Server_Check_Events(void)
{

  if (!sound_on) return;
  /* This function can and should only be compiled on machines, that have */
  /* the YIFF sound server installed.  Compilation therefore is optional and */
  /* can be toggled with the following  definition. */
#if HAVE_LIBY2
  if (YGetNextEvent (BackgroundMusic_con, &event, False) > 0)
    {
      // Sound object stopped playing? 
      if ((event.type == YSoundObjectKill))
	{
	  // Our play has stopped. 
	  DebugPrintf ("Done playing.\n");
	}
      // Server disconnected us? 
      else if (event.type == YDisconnect)
	{
	  // Got disconnected.
	  fprintf (stderr, 
		  "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:  The YIFF sound server has DISCONNECTED\n\
our connection.  The reason reported by the server is: %i.\n\
Under these cirmumstances Freedroid will stop to work in order to draw attention\n\
to this problem.  Please contact the developers and report this strange problem,\n\
which we have never ever encountered in all the test phases.\n\
If the problem persists, you might also prefer to use command line options to turn\n\
off sound such that you can play uninteruptedly.\n\
For now however Freedroid will terminate due to unresolvable sound problems.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , event.disconnect.reason );
	  Terminate (ERR);
	}
      // Server shutdown? 
      else if (event.type == YShutdown)
	{
	  // Server shutdown. 
	  fprintf (stderr, 
		  "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:  The YIFF sound server has SHUT DOWN\n\
our connection.  The reason reported by the server is: %i.\n\
Under these cirmumstances Freedroid will stop to work in order to draw attention\n\
to this problem.  Please contact the developers and report this strange problem,\n\
which we have never ever encountered in all the test phases.\n\
If the problem persists, you might also prefer to use command line options to turn\n\
off sound such that you can play uninteruptedly.\n\
For now however Freedroid will terminate due to unresolvable sound problems.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n", event.shutdown.reason);
	  Terminate (ERR);
	}
      else
	{
	  
	  // Some other Y event, ignore. 
	}
    }

#endif /* HAVE_LIBY2 */
}  // void YIFF_Server_Check_Events(void)


/*@Function============================================================
  @Desc: When accessing the yiff sound server, we need to supply absolute path
  names or relativ ones from some freely configurable directorys in yiffconfig.
  Of course we will take the secure path and supply absolute pathnames, for we
  do NOT know how the yiff is configured on each system.
  Therefore it is nescessary to expand relative pathnames into absolute ones.
  This function is supposed to do this. 

  @Ret: 
  @Int:
  * $Function----------------------------------------------------------*/
char *
ExpandFilename (char *LocalFilename)
{
  char *tmp;

  tmp = malloc (strlen (LocalFilename) + strlen (getcwd (NULL, 0)) + 1);
  strcpy (tmp, getcwd (NULL, 0));

  /* cut out the "/src" at the end of the sting, that is, make it */
  /* 4 chars shorter */
  /* tmp[strlen(tmp)-4]=0 */

  strcat (tmp, LocalFilename);
  return (tmp);
}				// char *ExpandFilename(char *LocalFilename){


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void 
YIFF_Server_Close_Connections (void)
{

  /* This function can and should only be compiled on machines, that have */
  /* the YIFF sound server installed.  Compilation therefore is optional and */
  /* can be toggled with the following definition. */
#if HAVE_LIBY2

  /* Disconnect from the Y server. We need to pass the
   * original connection pointer con to close that
   * connection to the Y server. Note that con may be
   * NULL at this point if the Y server disconnected us
   * already, passing NULL is okay.
   *
   * The second argument asks us do we want to leave the
   * Y server up when we disconnect. If we were the
   * program that started the Y erver and the second
   * argument is set to False then the Y server will
   * be automatically shut down.  To ensure that the Y
   * server stays running, you can pass True instead.
   */

  YCloseConnection (BackgroundMusic_con, False);
  BackgroundMusic_con = NULL;

#endif /* HAVE_LIBY2 */

} /* void YIFF_Server_Close_Connections(void) */

/*@Function============================================================
@Desc: Starts a Tune.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
StartSound (int Tune)
{

} /* void StartSound(int Tune) */

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
CrySound (void)
{

}

/*@Function============================================================
@Desc: 

This function is intended to provide a convenient way of switching
between different backround sounds in freedroid.
If no background sound was yet running, the function should start playing
the given background music.
If some background sound was already playing, the function should shut down
the old background music and start playing the new one.

Technical details:



@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Switch_Background_Music_To (int Tune)
{
#ifndef USE_SDL_AUDIO
  static int Current_Tune = SILENCE;
#endif



#ifndef USE_SDL_AUDIO
#if HAVE_LIBY2
  YEventSoundPlay Music_Parameters;

  if (!sound_on) return;

  DebugPrintf
    ("\nvoid Switch_Background_Music_To(int Tune):  Real function call confirmed.\n");

  DebugPrintf
    ("\nvoid Switch_Background_Music_To(int Tune):  Shutting down old background music track...\n");

  if (Current_Tune == SILENCE)
    {

      DebugPrintf
	("\nvoid Switch_Background_Music_To(int Tune):  No old music there to be shut down...\n");

    }
  else
    {

      DebugPrintf
	("\nvoid Switch_Background_Music_To(int Tune):  Old music track detected...\n");

      YDestroyPlaySoundObject (BackgroundMusic_con, BackgroundMusic_play_id);

      DebugPrintf
	("\nvoid Switch_Background_Music_To(int Tune):  Old music track stopped..\n");

      DebugPrintf
	("\nvoid Switch_Background_Music_To(int Tune):  SILENCE --> end of function reached.\n");

      if ( Tune == SILENCE ) return;

    }

  Current_Tune = SILENCE;

  if (YGetSoundObjectAttributes
      (BackgroundMusic_con, ExpandedSoundSampleFilenames[Tune],
       &BackgroundMusic_sndobj_attrib))
    {

      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:  The YIFF sound server has reported,\n\
that a certain file it wanted to playback was MISSING OR CORRUPT.\n\
This error message will also appear if the YIFF sound server has not been\n\
initialized at all i.e. no attempt to initialize is has been made.\n\
The file name of this missing or corrupt file was determined as:\n\
%s\n\
Under these cirmumstances Freedroid will stop to work in order to draw attention\n\
to this problem.  Please contact the developers and report this strange problem,\n\
which we have never ever encountered in all the test phases.\n\
If the problem persists, you might also prefer to use command line options to turn\n\
off sound such that you can play uninteruptedly.\n\
For now however Freedroid will terminate due to unresolvable sound problems.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n", ExpandedSoundSampleFilenames[Tune]);

      printf (" CWD: %s \n\n", getcwd (NULL, 0));
      Terminate (ERR);
    }
  else
    {

      DebugPrintf 
	("\nvoid Switch_Background_Music_To(int Tune):  Now starting new background tune...\n");
      DebugPrintf 
	("\nvoid Switch_Background_Music_To(int Tune):  The following file will be loaded: ");
      DebugPrintf (ExpandedSoundSampleFilenames[ Tune ]);

      BackgroundMusic_play_id = YStartPlaySoundObjectSimple (BackgroundMusic_con, 
							     ExpandedSoundSampleFilenames[ Tune ] );
      DebugPrintf ("\nvoid Switch_Background_Music_To(int Tune):  Tune has been loaded: ");
      DebugPrintf ( ExpandedSoundSampleFilenames[ Tune ] );
      DebugPrintf ("\nvoid Switch_Background_Music_To(int Tune):  preparing endless loop...\n");

      Current_Tune = Tune;

      Music_Parameters.repeats = 0;
      Music_Parameters.total_repeats = -1;	// -1 here means to repeat indefinately
      Music_Parameters.left_volume = Current_BG_Music_Volume;
      Music_Parameters.right_volume = Current_BG_Music_Volume;
      Music_Parameters.sample_rate =
	BackgroundMusic_sndobj_attrib.sample_rate;
      Music_Parameters.length = BackgroundMusic_sndobj_attrib.sample_size;
      Music_Parameters.position = 0;
      Music_Parameters.yid = BackgroundMusic_play_id;
      Music_Parameters.flags = 0xFFFFFFFF;


      YSetPlaySoundObjectValues (BackgroundMusic_con, BackgroundMusic_play_id,
				 &Music_Parameters);

      DebugPrintf
	("\nvoid Switch_Background_Music_To(int Tune):  New tune should be played endlessly now.\n");

    }

  DebugPrintf
    ("\nvoid Switch_Background_Music_To(int Tune):  end of function reached.\n");

#endif /* HAVE_LIBY2 */
#else
  //
  // Here comes the SDL-BASED Background music code:

  if ( Background_Music_Channel >= 0 )
    {
      Mix_HaltChannel( Background_Music_Channel );
      printf("\nOld Background music channel has been halted.");
      Background_Music_Channel = -1;
    }

  if (Background_Music_Channel < 0)
    {
      Background_Music_Channel = Mix_PlayChannel(-1, Loaded_WAV_Files[ Tune ], -1);
    }

#endif /* USE_SDL_AUDIO */
} // void Switch_Background_Music_To(int Tune)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Play_Sound (int Tune)
{
#ifndef USE_SDL_AUDIO
  YEventSoundPlay Music_Parameters;
#else
  int Newest_Sound_Channel=0;
#endif

  if ( !sound_on ) return;

  /* This function can and should only be compiled on machines, that have */
  /* the YIFF sound server installed.  Compilation therefore is optional and */
  /* can be toggled with the following  definition.*/

#ifndef USE_SDL_AUDIO
#if HAVE_LIBY2
  DebugPrintf
    ("\nvoid Play_Sound(int Tune):  Real function call confirmed.");
  DebugPrintf
    ("\nvoid Play_Sound(int Tune):  Playback is about to start!");

  Music_Parameters.repeats = 0;
  Music_Parameters.total_repeats = 1;	// -1 here means to repeat indefinately
  Music_Parameters.left_volume = Current_Sound_FX_Volume;
  Music_Parameters.right_volume = Current_Sound_FX_Volume;
  Music_Parameters.sample_rate =
    BackgroundMusic_sndobj_attrib.sample_rate;
  Music_Parameters.length = BackgroundMusic_sndobj_attrib.sample_size;
  Music_Parameters.position = 0;
  Music_Parameters.yid = BackgroundMusic_play_id;
  Music_Parameters.flags = YPlayValuesFlagVolume;

  play_id = YStartPlaySoundObject(BackgroundMusic_con, ExpandedSoundSampleFilenames[Tune], &Music_Parameters);

#endif /* HAVE_LIBY2 */
#else
  Newest_Sound_Channel = Mix_PlayChannel(-1, Loaded_WAV_Files[Tune] , 0);
  if ( Newest_Sound_Channel == -1 )
    {
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL MIXER WAS UNABLE TO PLAY A CERTAIN FILE LOADES INTO MEMORY.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
If the problem persists, please inform the developers about it.\n\
\n\
In the meantime you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve.\n\
Sorry...\n\
----------------------------------------------------------------------\n\
\n" , ExpandedSoundSampleFilenames[ Tune ]);
      Terminate (ERR);
    } // if ( ... = -1
  else
    {
      printf("\nSuccessfully playing file %s.", ExpandedSoundSampleFilenames[ Tune ]);
    }
  
#endif /* USE_SDL_AUDIO */

}  // void Play_Sound(int Tune)

/*@Function============================================================
@Desc: 

Connect to the Y server. (That is the yiff.) 
We pass NULL as the start argument, this means the Y server will not be started if it was detected to be not running. 
The connection argument is CON_ARG which is defined above as a constant. 
The connection argument is a string of the format ":".

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
int
Init_YIFF_Sound_Server (void)
{
  /* This function can and should only be compiled on machines, that have */
  /* the YIFF sound server installed.  Compilation therefore is optional */
  /* and can be toggled with the following definition. */
#if HAVE_LIBY2

  DebugPrintf
    ("\nint Init_YIFF_Sound_Server(void): real function call confirmed.\n");

  // Now a new connection to the yiff server can be opend.  The first argument to open is not NULL,
  // therefore a yiff server will be started even if none is running!!  great!!
  BackgroundMusic_con = YOpenConnection ("/usr/sbin/yiff", CON_ARG);

  // Attention!! First channel is to be opend now!
  if (BackgroundMusic_con == NULL)
    {
      // Failed to connect to the Y server. 
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:  The a CONNECTION TO THE YIFF SOUND \n\
SERVER COULD NOT BE ESTABLISHED.  Maybe the problem is that the YIFF sound\n\
server could not be started at all.  The connection argument was : \n\
%s\n\
Please see if you really have installed (and configured) the YIFF.\n\
If you do not have installed the YIFF, you can obtain if from: \n\
http://wolfpack.twu.net/YIFF/\n\
If you for some reason cannot install the yiff, you can choose to play \n\
without sound.\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  But for now Freedroid will terminate to draw attention \n\
to the sound problem it could not resolve. Sorry...\n\
----------------------------------------------------------------------\n\
\n", CON_ARG);
      Terminate (ERR);
    }

  // The connection to the sound server should now be established...
  // Printing debug message and going on...

  DebugPrintf
    ("\nint Init_YIFF_Sound_Server(void): The connection to the sound server was established successfully...");
  DebugPrintf
    ("\nint Init_YIFF_Sound_Server(void): end of function reached.\n");

#endif
  return (OK);
} // void Init_YIFF_Sound_Server(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
GotHitSound (void)
{
  if (!sound_on) return;

  Play_Sound (GOT_HIT_SOUND);
}				// void GotHitSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
GotIntoBlastSound (void)
{
  if (!sound_on) return;

  Play_Sound (GOT_INTO_BLAST_SOUND);
  return;
}				// void GotIntoBlastSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
RefreshSound (void)
{
  if (!sound_on) return;

  Play_Sound (REFRESH_SOUND);
  return;
}				// void RefreshSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveElevatorSound (void)
{
  if (!sound_on) return;

  Play_Sound (MOVE_ELEVATOR_SOUND);
}				// void MoveElevatorSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MenuItemSelectedSound (void)
{
  if (!sound_on) return;

  Play_Sound ( MENU_ITEM_SELECTED_SOUND );
}				// void MoveElevatorSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveMenuPositionSound (void)
{
  if (!sound_on) return;

  Play_Sound ( MOVE_MENU_POSITION_SOUND );
}				// void MoveElevatorSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtDefeatedSound (void)
{
  if (!sound_on) return;
  Play_Sound (THOU_ART_DEFEATED_SOUND);
}				// void MoveElevatorSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
EnterElevatorSound (void)
{
  if (!sound_on) return;

  Play_Sound (ENTER_ELEVATOR_SOUND);
  return;
}				// void EnterElevatorSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
LeaveElevatorSound (void)
{
  if (!sound_on) return;

  Play_Sound (LEAVE_ELEVATOR_SOUND);

  return;
}				// void LeaveElevatorSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Fire_Bullet_Sound (int BulletType)
{
  if (!sound_on) return;

  switch (BulletType)
    {
      case PULSE:
	Play_Sound ( FIRE_BULLET_PULSE_SOUND );
	break;

      case SINGLE_PULSE:
	Play_Sound ( FIRE_BULLET_SINGLE_PULSE_SOUND );
	break;

      case MILITARY:
	Play_Sound ( FIRE_BULLET_MILITARY_SOUND );
	break;

      case FLASH:
	Play_Sound ( FIRE_BULLET_FLASH_SOUND );
	break;

    }
}				// void FireBulletSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Takeover_Set_Capsule_Sound (void)
{
  if (!sound_on) return;

  Play_Sound (TAKEOVER_SET_CAPSULE_SOUND);
}				// void FireBulletSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Takeover_Game_Won_Sound (void)
{
  if (!sound_on) return;

  Play_Sound ( TAKEOVER_GAME_WON_SOUND );
}				// void FireBulletSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Takeover_Game_Deadlock_Sound (void)
{
  if (!sound_on) return;

  Play_Sound ( TAKEOVER_GAME_DEADLOCK_SOUND );
}				// void FireBulletSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Takeover_Game_Lost_Sound (void)
{
  if (!sound_on) return;

  Play_Sound ( TAKEOVER_GAME_LOST_SOUND );
}				// void FireBulletSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
BounceSound (void)
{
  if (!sound_on) return;

  Play_Sound (COLLISIONSOUND);

}				// void BounceSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
DruidBlastSound (void)
{
  if (!sound_on) return;

  Play_Sound (BLASTSOUND);

}				// void BounceSound(void)


#undef _sound_c
