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

char *SoundSampleFilenames[ALL_SOUNDS] = {
   "ERRORSOUND_NILL.NOWAV",
   "Combat_Background_Music.wav",
   "Takeover_Background_Music.wav",
   "Console_Background_Music.wav",
   "Blast_Sound_0.wav",
   "Collision_Sound_0.wav",
   "GotIntoBlast_Sound_0.wav",
   "MoveElevator_Sound_0.wav",
   "Refresh_Sound_0.wav",
   "LeaveElevator_Sound_0.wav",
   "EnterElevator_Sound_0.wav",
   "ThouArtDefeated_Sound_0.wav",
   "Got_Hit_Sound_0.wav",
   "TakeoverSetCapsule_Sound_0.wav",
   "Menu_Item_Selected_Sound_0.wav",
   "Move_Menu_Position_Sound_0.wav",
   "Takeover_Game_Won_Sound_0.wav",
   "Takeover_Game_Deadlock_Sound_0.wav",
   "Takeover_Game_Lost_Sound_0.wav",
   "Fire_Bullet_Pulse_Sound_0.wav",
   "Fire_Bullet_Single_Pulse_Sound_0.wav",
   "Fire_Bullet_Military_Sound_0.wav",
   "Fire_Bullet_Flash_Sound_0.wav",
   "Fire_Bullet_Exterminator_Sound_0.wav",
   "phaser.wav",
   "Cry_Sound_0.wav",
   "Takeover_Sound_0.wav"
};

#ifdef HAVE_LIBSDL_MIXER
Mix_Chunk *Loaded_WAV_Files[ALL_SOUNDS];
#endif

char *MusicFiles [NUM_COLORS] = {  // we have a background song per color now
  "AnarchyMenu1.mod",                 // RED
  "starpaws.mod",                     // YELLOW
  "The_Last_V8.mod",                  // GREEN
  "dreamfish-green_beret.mod",        // GRAY
  "dreamfish-sanxion.mod",            // BLUE
  "kollaps-tron.mod",                 // GREENBLUE
  "dreamfish-uridium2_loader.mod"     // DARK
};

#ifdef HAVE_LIBSDL_MIXER
Mix_Music *MusicSongs[NUM_COLORS];
Mix_Music *Tmp_MOD_File;
#endif

void 
Init_Audio(void)
{
#ifndef HAVE_LIBSDL_MIXER  
  return;
#else
  char *fpath;
  int i;
  int audio_rate = 22050;
  Uint16 audio_format = AUDIO_S16; 
  int audio_channels = 2;
  //  int audio_buffers = 4096;
  int audio_buffers = 2048;

  DebugPrintf(1, "\nInitializing SDL Audio Systems....\n");

  if ( !sound_on ) return;


  // Now SDL_AUDIO is initialized here:

  if ( SDL_InitSubSystem ( SDL_INIT_AUDIO ) == -1 ) 
    {
      fprintf(stderr, "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The SDL AUDIO SUBSYSTEM COULD NOT BE INITIALIZED.\n\
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
      Terminate(ERR);
    } else
      {
	printf("\nSDL Audio initialisation successful.\n");
      }

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
      DebugPrintf (1, "\nSuccessfully opened SDL audio channel." );
    }

  // Now that the audio channel is opend, its time to load all the
  // WAV files into memory, something we NEVER did while using the yiff,
  // because the yiff did all the loading, analyzing and playing...

  Loaded_WAV_Files[0]=NULL;
  for (i = 1; i < ALL_SOUNDS; i++)
    {
      fpath = find_file (SoundSampleFilenames[ i ], SOUND_DIR, FALSE);
      Loaded_WAV_Files[ i ] = Mix_LoadWAV(fpath);
      if ( Loaded_WAV_Files[i] == NULL )
	{
	  DebugPrintf (0, "Error: could not load Sound-sample: %s", SoundSampleFilenames[ i ]);
	  Terminate (ERR);
	} // if ( !Loaded_WAV...
      else
	{
	  DebugPrintf (1, "\nSuccessfully loaded file %s.", SoundSampleFilenames[i]);
	}
    } // for (i=1; ...

  for (i = 0; i < NUM_COLORS; i++)
    {
      fpath = find_file ( MusicFiles [ i ], SOUND_DIR, FALSE);
      MusicSongs [ i ] = Mix_LoadMUS( fpath );
      if ( MusicSongs[ i ] == NULL )
	{
	  DebugPrintf ( 0, "\nError loading sound-file: %s. Mixer-Error: %s\n", 
			MusicFiles[ i ] , Mix_GetError() ); 
	  Terminate(ERR);
	} // if ( !Loaded_WAV...
      else
	DebugPrintf ( 1 , "\nSuccessfully loaded file %s.", MusicFiles[ i ]);
    } // for
  

  //--------------------
  // Now that the music files have been loaded successfully, it's time to set
  // the music and sound volumes accoridingly, i.e. as specifies by the users
  // configuration.
  //
  Set_Sound_FX_Volume( GameConfig.Current_Sound_FX_Volume );

  // DebugPrintf (1, "done.");
  // fflush(stdout);
#endif // HAVE_SDL_MIXER
} // void InitAudio(void)


int i;
unsigned char *ptr;
unsigned char v = 128;
int SampleLaenge;

void 
Set_BG_Music_Volume(float NewVolume)
{

#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  if ( !sound_on ) return;

  // Set the volume IN the loaded files, if SDL is used...
  /*
    That is old and way used, when music was technically still a sound
  for ( i=1 ; i<5 ; i++ )
    {
      Mix_VolumeChunk( Loaded_WAV_Files[i], (int) rintf(NewVolume* MIX_MAX_VOLUME) );
    }
  */

  Mix_VolumeMusic( (int) rintf( NewVolume * MIX_MAX_VOLUME ) );

  // Switch_Background_Music_To ( COMBAT_BACKGROUND_MUSIC_SOUND );
#endif // HAVE_LIBSDL_MIXER
} // void Set_BG_Music_Volume(float NewVolume)

void 
Set_Sound_FX_Volume(float NewVolume)
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  if ( !sound_on ) return;

  // Set the volume IN the loaded files, if SDL is used...
  // This is done here for the Files 1,2,3 and 4, since these
  // are background music files.
  for ( i=5 ; i<ALL_SOUNDS ; i++ )
    {
      Mix_VolumeChunk( Loaded_WAV_Files[i], (int) rintf(NewVolume* MIX_MAX_VOLUME) );
    }

#endif // HAVE_LIBSDL_MIXER

} // void Set_BG_Music_Volume(float NewVolume)


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
  Play_Sound ( CRY_SOUND );
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
TransferSound (void)
{
  Play_Sound ( TRANSFER_SOUND );
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
Switch_Background_Music_To ( char* filename_raw )
{
  char* fpath;

#ifndef HAVE_LIBSDL_MIXER
  return;
#else

  if ( !sound_on ) return;


  if ( filename_raw == NULL ) 
    {
      //printf("\nOld Background music channel has been halted.");
      // fflush(stdout);
      Mix_HaltMusic( ); // this REALLY is a VOID-argument function!!
      return;
    }

  // New feature: choose background music by level-color:
  // if filename_raw==BYCOLOR then chose bg_music[color]
  if (!strcmp( filename_raw, BYCOLOR))
      Mix_PlayMusic (MusicSongs[CurLevel->color], -1);
  else
    {
      if (Tmp_MOD_File) Mix_FreeMusic(Tmp_MOD_File);      
      fpath = find_file (filename_raw, SOUND_DIR, FALSE);    
      Tmp_MOD_File = Mix_LoadMUS (fpath);
      if ( Tmp_MOD_File == NULL )
	{
	  DebugPrintf (0, "ERROR: Could not load soundfile: %s \n SDL-Error: %s\n", 
		       fpath , Mix_GetError() );
	  Terminate (ERR);
	} // if ( !Loaded_WAV...
      Mix_PlayMusic (Tmp_MOD_File, -1);
    }      
  
  Mix_VolumeMusic ( (int) rintf( GameConfig.Current_BG_Music_Volume * MIX_MAX_VOLUME ) );

#endif // HAVE_LIBSDL_MIXER

}; // void Switch_Background_Music_To(int Tune)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
Play_Sound (int Tune)
{
#ifndef HAVE_LIBSDL_MIXER
  return;
#else
  int Newest_Sound_Channel=0;

  if ( !sound_on ) return;

  Newest_Sound_Channel = Mix_PlayChannel(-1, Loaded_WAV_Files[Tune] , 0);
  if ( Newest_Sound_Channel == -1 )
    {
      fprintf (stderr,
	       "\n\
\n\
----------------------------------------------------------------------\n\
Freedroid has encountered a problem:\n\
The a SDL MIXER WAS UNABLE TO PLAY A CERTAIN FILE LOADED INTO MEMORY.\n\
\n\
The name of the problematic file is:\n\
%s \n\
\n\
Analysis of the error has returned the following explanation through SDL:\n\
%s \n\
The most likely cause for the problem however is, that too many sounds\n\
have been played in too rapid succession, which should be caught.\n\
If the problem persists, please inform the developers about it.\n\
\n\
In the meantime you can choose to play without sound.\n\
\n\
If you want this, use the appropriate command line option and Freedroid will \n\
not complain any more.  Freedroid will NOT be terminated now to draw attention \n\
to this sound problem, because the problem is not lethal and will not interfere\n\
with game performance in any way.  I think this is really not dangerous.\n\
----------------------------------------------------------------------\n\
\n" , SoundSampleFilenames[ Tune ] , Mix_GetError() );
      // Terminate (ERR);
    } // if ( ... = -1
  else
    {
      DebugPrintf( 2 , "\nSuccessfully playing file %s.", SoundSampleFilenames[ Tune ]);
      // DebugPrintf (1, "\nSuccessfully playing file %s.", SoundSampleFilenames[ Tune ]);
    }

#endif // HAVE_LIBSDL_MIXER
  
}  // void Play_Sound(int Tune)

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
MoveLiftSound (void)
{
  if (!sound_on) return;

  Play_Sound (MOVE_ELEVATOR_SOUND);
}				// void MoveLiftSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MenuItemSelectedSound (void)
{
  if (!sound_on) return;

  Play_Sound (MENU_ITEM_SELECTED_SOUND);
}				// void MoveLiftSound(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
MoveMenuPositionSound (void)
{
  if (!sound_on) return;

  Play_Sound (MOVE_MENU_POSITION_SOUND);
}				// void MoveLiftSound(void)


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
}				// void MoveLiftSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
EnterLiftSound (void)
{
  if (!sound_on) return;

  Play_Sound (ENTER_ELEVATOR_SOUND);
  return;
}				// void EnterLiftSound(void)


/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
LeaveLiftSound (void)
{
  if (!sound_on) return;

  Play_Sound (LEAVE_ELEVATOR_SOUND);

  return;
}				// void LeaveLiftSound(void)


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

      case EXTERMINATOR:
	Play_Sound ( FIRE_BULLET_EXTERMINATOR_SOUND );
	break;

      case LASER_RIFLE:
	Play_Sound ( FIRE_BULLET_LASER_RIFLE_SOUND );
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

  // Play_Sound (COMBAT_BACKGROUND_MUSIC_SOUND );
  Play_Sound ( COLLISIONSOUND );

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
