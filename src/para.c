/*----------------------------------------------------------------------
 *
 * Desc: the main program
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
#define _freedroid_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"
#include "paratext.h"
#include "paravars.h"
#include "ship.h"


// Die folgenden Schalter schalten Debugmeldungen in gewissen speziellen */
// Funktionen aus oder ein
#undef DEBUG_MYRANDOM

/* Schalter fuer die bedingte Kompilierung */
#undef DREHSCHUESSE
#undef SCHUSSDEBUG
#undef INFLUNCECLPOSDEBUG
#undef INFLUENCEWALLDEBUG
#undef DEBUGREPORT
#undef BULLETSPEEDUPONRICHTUNG

#undef SAYJOYPOS
#undef SPEICHERSPAREN

#define TITLE_EIN

#define MOVEENEMYSOFF 		FALSE
#define ANIMATEENEMYSOFF 	FALSE
#define INFLUENCEGETROFFENOFF 	FALSE

#define FENSTEROFF 		FALSE
#define ENERGIEBALKENOFF 	TRUE
#define REDRAW001OFF 		FALSE
#define USEINTSOFF 		FALSE

/* Scroll- Fenster */
#define SCROLLSTARTX		USERFENSTERPOSX
#define SCROLLSTARTY		SCREENHOEHE
#define CHARSPERLINE		(int)(USERFENSTERBREITE/FONTBREITE)

int ThisMessageTime;
float LastGotIntoBlastSound = 2;
float LastRefreshSound = 2;

int card = 0;
int device = 0;
void *handle;

struct timeval now, oneframetimestamp, tenframetimestamp,
  onehundredframetimestamp, differenz;

long oneframedelay = 0;
long tenframedelay = 0;
long onehundredframedelay = 0;
// float oneframedelay, tenframedelay, onehundredframedelay;
float FPSover1 = 10;
float FPSover10 = 10;
float FPSover100 = 10;
int framenr = 0;
int SkipAFewFrames = 0;
long Overall_Frames_Displayed=0;
float Overall_Average=0.041;

int TestSound (void);
void CalibratedDelay (long);
void Debriefing (void);
void Pause (void);
void UpdateCountersForThisFrame (void);



/*@Function============================================================
@Desc: 

 * This function is the key to independence of the framerate for various game elements.
 * It returns the average time needed to draw one frame.
 * Other functions use this to calculate new positions of moving objects, etc..
 *
 * BEWARE!  Using the average over 1 frame (the previous frame that is) is no good.
 * The sound server tends to case moderate distortions in the frame rate, which seems
 * not to matter too much on my system at least, when using overalls over 10 frames.

 * Also there is of course a serious problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.

 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.

 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
float
Frame_Time (void)
{
  float Rate_To_Be_Returned;
  
  if ( SkipAFewFrames ) 
    {
      Rate_To_Be_Returned = Overall_Average;
      return Rate_To_Be_Returned;
    }


  // Rate_To_Be_Returned = (1.0 / FPSover10);
  /*
  if ( Overall_Frames_Displayed < 100 )
    {
      Rate_To_Be_Returned = (1.0 / FPSover10);
      return Rate_To_Be_Returned;
    }
  */

  // Rate_To_Be_Returned = (1.0 / FPSover100);
  // Rate_To_Be_Returned = (1.0 / FPSover10);
  Rate_To_Be_Returned = (1.0 / FPSover1);

  // if (FPSover10 > 20)
  /* if (FPSover100 > 20)
    {
      Rate_To_Be_Returned = (1.0 / FPSover100);
    }
  else
    {
      Rate_To_Be_Returned = (1.0 / 20.0);
    }
  */

  return Rate_To_Be_Returned;

} // float Frame_Time(void)

/*@Function============================================================
@Desc: 

 * This function is the key to independence of the framerate for various game elements.
 * It returns the average time needed to draw one frame.
 * Other functions use this to calculate new positions of moving objects, etc..
 *
 * BEWARE!  Using the average over 1 frame (the previous frame that is) is no good.
 * The sound server tends to case moderate distortions in the frame rate, which seems
 * not to matter too much on my system at least, when using overalls over 10 frames.

 * Also there is of course a serious problem when some interuption occurs, like e.g.
 * the options menu is called or the debug menu is called or the console or the elevator
 * is entered or a takeover game takes place.

 * To counter unwanted effects after such events we have the SkipAFewFramerates counter,
 * which instructs Rate_To_Be_Returned to return only the overall default framerate since
 * no better substitute exists at this moment.

 * This counter is most conveniently set via the function Activate_Conservative_Frame_Computation,
 * which can be conveniently called from eveywhere.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void 
Activate_Conservative_Frame_Computation(void)
{
  // SkipAFewFrames=212;
  // SkipAFewFrames=22;
  SkipAFewFrames=3;
} // void Activate_Conservative_Frame_Computation(void)

/*-----------------------------------------------------------------
 * @Desc: the heart of the Game 
 *
 * @Ret: void
 *
 *-----------------------------------------------------------------*/
int
main (int argc, char *const argv[])
{
  int i;
  int Robot_Counter;
  int AllRobotsDead;

  GameOver = FALSE;
  QuitProgram = FALSE;
  Conceptview = FALSE;

  sound_on = TRUE;	 /* default value, can be overridden by command-line */
  debug_level = 0;       /* 0=no debug 1=first debug level (at the moment=all) */
  fullscreen_on = FALSE; /* use X11-window or full screen */

  /*
   *  Parse command line and set global switches 
   *  this function exits program when error, so we don't need to 
   *  check its success  (dunno if that's good design?)
   */
  parse_command_line (argc, argv);

  /* Initialisieren der globalen Variablen und Arrays */
  InitParaplus ();

#if REDRAW001OFF == 0
  RedrawInfluenceNumber ();
#endif

  DebugPrintf
    ("\nvoid main(void): Vor Beginn der !QuitProgram - Schreife....\n");

  while (!QuitProgram)
    {

      /* InitNewGame */
      InitNewGame ();

      GameOver = FALSE;

      DebugPrintf
	("void main(void): Vor Beginn der !GameOver && !QuitProgram - Schleife....\n");

      while (!GameOver && !QuitProgram)
	{
	  /* This ensures, that 0 is never an encountered framenr,
	   * therefore count to 100 here
	   * Take the time now for calculating the frame rate
	   * (DO NOT MOVE THIS COMMAND PLEASE!) */
	  framenr++;

	  gettimeofday (&oneframetimestamp, NULL);
	  if (framenr % 10 == 1)
	    gettimeofday (&tenframetimestamp, NULL);
	  if (framenr % 100 == 1)
	    gettimeofday (&onehundredframetimestamp, NULL);

	  UpdateCountersForThisFrame ();


	  DebugPrintf
	    ("void main(void): Innerhalb der !GameOver && !QuitProgram - Schleife....\n");
	  keyboard_update ();

	  if ( QPressed() )
	    {
	      DebugPrintf
		("\n*****************************************************");
	      DebugPrintf
		("\nvoid main(void): Termination cause of Q-Pressing!!!!!");
	      DebugPrintf
		("\n*****************************************************\n\n");
	      Terminate (0);
	    }
	  if ( DPressed() )
	    Me.energy = 0;
	  if ( LPressed() ) 
	      ShowHighscoreList ();
	  if ( IPressed() )
	    ShowDebugInfos ();
	  if ( CPressed() ) 
	      Cheatmenu ();
	  if ( EscapePressed() )
	    EscapeMenu ();
	  if ( PPressed () )
	    Pause ();

	  if (ShipEmptyCounter == 1)
	    GameOver = TRUE;

	  LastBlastHit++;

	  MoveLevelDoors ();	/* Die Tueren im Level auf und zu bewegen */
	  AnimateRefresh ();	/* Refreshes Animieren */

	  for (i = 0; i < NumEnemys; i++)
	    {
	      if (Feindesliste[i].warten > 0) 
		{
		  Feindesliste[i].warten -= Frame_Time() ;
		  if (Feindesliste[i].warten < 0) Feindesliste[i].warten = 0;
		}
	      if (Feindesliste[i].firewait > 0) 
		{
		  Feindesliste[i].firewait -= Frame_Time() ;
		  if (Feindesliste[i].firewait <= 0) Feindesliste[i].firewait=0;
		}
	    }

	  MoveBullets ();   
	  ExplodeBlasts ();	/* Blasts in der Explosionsphase weiterbewegen */

	  GetView ();		/* Einen Ausschnitt aus der Gesamtlevelkarte machen */

	  GetInternFenster (SHOW_ALL);

	  PutInternFenster ( TRUE );
	  
	  YIFF_Server_Check_Events ();

	  UpdateInfoline ();
	  for (i = 0; i < MAXBULLETS; i++)
	    CheckBulletCollisions (i);
	  PutMessages ();

	  JoystickControl ();	// Wenn vorhanden: Joystick einlesen 
	  MoveInfluence ();	// change Influ-speed depending on keys pressed
	  MoveEnemys ();	// Auch die Feinde bewegen 
	  AnimateInfluence ();	// Bei animierten Influencer die Phasen weiterzaehlen 
	  AnimateEnemys ();	// Bei den Feinden auch Phasen weiterzaehlen 

	  /* Raeder bremsen die Fahrt des Influencers erheblich */
	  DebugPrintf ("\nvoid main: Me.speed.x ist jetzt: ");
	  DebugPrintfFloat (Me.speed.x);
	  DebugPrintf ("\nvoid main: Me.speed.y ist jetzt: ");
	  DebugPrintfFloat (Me.speed.y);

	  Reibung ();

	  DebugPrintf ("\nvoid main: Me.speed.x nach REIBUNG ist jetzt: ");
	  DebugPrintfFloat (Me.speed.x);

	  // gl_printf( -1 , -1 , "\nmain: Me.energy:%f!" , Me.energy );
	  // gl_printf(-1,-1,"\nmain: speed:%f/%f!", Me.speed.x, Me.speed.y);
	  // gl_printf(-1,-1,"\nmain: phase:%f!", Me.phase );

	  /* Influencedruid nach der momentanen Geschwindigkeit weiterbewegen */
	  Me.pos.x += Me.speed.x * Frame_Time ();
	  Me.pos.y += Me.speed.y * Frame_Time ();
	  AdjustSpeed ();

	  BounceInfluencer ();	/* Testen ob der Weg nicht durch Mauern verstellt ist */
	  InfluenceEnemyCollision ();
	  RotateBulletColor ();

	  AllRobotsDead=TRUE;
	  for ( Robot_Counter=0 ; Robot_Counter < MAX_ENEMYS_ON_SHIP ; Robot_Counter++ )
	    {
	      if (Feindesliste[Robot_Counter].energy) AllRobotsDead=FALSE;
	    }
	  if (AllRobotsDead) 
	    {
	      EndTitle();
	      GameOver=TRUE;
	    }
	  
	  if (CurLevel->empty == 2)
	    {
	      LevelGrauFaerben ();
	      CurLevel->empty = TRUE;
	    }			/* if */

	  // calculate the framerate:
	  gettimeofday (&now, NULL);

	  oneframedelay =
	    (now.tv_usec - oneframetimestamp.tv_usec) + (now.tv_sec -
							 oneframetimestamp.
							 tv_sec) * 1000000;
	  if (framenr % 10 == 0)
	    tenframedelay =
	      ((now.tv_usec - tenframetimestamp.tv_usec)) + (now.tv_sec -
							     tenframetimestamp.
							     tv_sec) *
	      1000000;
	  if ((framenr % 100) == 0)
	    {
	      onehundredframedelay =
		(now.tv_sec - onehundredframetimestamp.tv_sec) * 1000000 +
		(now.tv_usec - onehundredframetimestamp.tv_usec);
	      framenr = 0;
	      // printf("\n Current Frame_Time: %f.", Frame_Time());
	    }

	  FPSover1 = 1000000 * 1 / (float) oneframedelay;
	  FPSover10 = 1000000 * 10 / (float) tenframedelay;
	  FPSover100 = 1000000 * 100 / (float) onehundredframedelay;

	  // gl_printf(1,30,"   1fr: %d ms FPS1: %f \n",oneframedelay,FPSover1);
	  // gl_printf(-1,-1," 10fr: %d ms FPS10: %f \n",tenframedelay, (1/FPSover10) );
	  // gl_printf( -1 , -1 , " Overall: %f \n" , Overall_Average );
	  //       gl_printf(1,35,"100fr: %d ms FPS100: %f \n",onehundredframedelay,FPSover100);
	  // gl_printf( -1 , -1 , "Frame_Time(): %f \n" , Frame_Time() );
	  // gl_printf(-1,-1,"sec : %d usec : %d \n",now.tv_sec,now.tv_usec);
	  // gl_printf(-1,-1,"sec : %d usec : %d \n",onehundredframetimestamp.tv_sec,onehundredframetimestamp.tv_usec);
	  // gl_printf(-1,-1,"sec : %d usec : %d \n",now.tv_sec-onehundredframetimestamp.tv_sec,now.tv_usec-onehundredframetimestamp.tv_usec);
	  //      gl_printf(-1,-1,"sec : %d \n",onehundredframedelay);
	  // gl_printf(-1,-1,"sec : %d \n",framenr % 100);

	  // gl_printf(-1,-1,"%f\n",oneframetimestamp);
	  //      gl_printf(-1,-1,ltoa((long)onehundredframedelay,Dummystring,10));
	} /* while !GameOver */
    } /* while !QuitProgram */
  Terminate (0);
  return (0);
}				// void main(void)

/*@Function============================================================
@Desc: Diese Funktion Sprengt den Influencer und beendet das Programm

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtDefeated (void)
{
  int i, j;

  DebugPrintf ("\nvoid ThouArtDefeated(void): Real function call confirmed.");
  Me.status = TERMINATED;
  ThouArtDefeatedSound ();
  ExplodeInfluencer ();

  for (i = 0; i < WAIT_AFTER_KILLED; i++)
    {
      UpdateInfoline();
      SetInfoline();
      DisplayRahmen (RealScreen);
      GetInternFenster (SHOW_ALL);
      PutInternFenster (TRUE);
      ExplodeBlasts ();
      MoveBullets ();
      MoveEnemys ();
      AnimateEnemys ();
      for (j = 0; j < MAXBULLETS; j++)
	CheckBulletCollisions (j);
      RotateBulletColor ();
      usleep (10000);
    }

  /* Ein Flimmer zieht "uber den Schirm */
  Flimmern (4);  /* type 4 flimmer */
  Debriefing ();

  /* Soundblaster soll keine Toene mehr spucken */
  //PORT sbfm_silence();

  GameOver = TRUE;

  DebugPrintf
    ("\nvoid ThouArtDefeated(void): Usual end of function reached.");
}				// void ThouArtDefeated(void)

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
ThouArtVictorious (void)
{
  ShipEmptyCounter = WAIT_SHIPEMPTY;
  GameOver = TRUE;		/*  */

  KillTastaturPuffer ();
  ClearUserFenster ();
  DebugPrintf (" BRAVO ! Sie haben es geschafft ! ");
  getchar ();
}

/* **********************************************************************
   Diese Funktion updated die Highscoreliste
**********************************************************************/
void
Debriefing (void)
{
  char *Scoretext;
  HallElement *Oldptr;
  HallElement *Newptr;
  HallElement *SaveHallptr = Hallptr;
  int DebriefColor;

  DebriefColor = FONT_WHITE;

  Me.status = DEBRIEFING;
  if (!PlusExtentionsOn)
    {
      Scoretext = MyMalloc (1000);
      if (RealScore > GreatScore)
	{
	  SetUserfenster (DebriefColor, RealScreen);	// KON_BG_COLOR
	  SetTextColor (DebriefColor, KON_TEXT_COLOR);	// KON_BG_COLOR
	  strcpy (Scoretext, "\n    Great Score !\n Enter your name:");
	  DisplayText (Scoretext, USERFENSTERPOSX, USERFENSTERPOSY,
		       RealScreen, FALSE);
	  // free (GreatScoreName);
	  // SwapScreen();
	  PrepareScaledSurface(TRUE);

	  GreatScoreName = GetString (10, 2);
	  GreatScore = RealScore;
	}
      else if (RealScore < LowestScoreOfDay)
	{
	  SetUserfenster (DebriefColor, RealScreen);	// KON_BG_COLOR
	  SetTextColor (DebriefColor, KON_TEXT_COLOR);	// KON_BG_COLOR
	  strcpy (Scoretext, "\n   Lowest Score of Day! \n Enter your name:");
	  DisplayText (Scoretext, USERFENSTERPOSX, USERFENSTERPOSY,
		       RealScreen, FALSE);
	  // SwapScreen();
	  PrepareScaledSurface(TRUE);
	  // free (LowestName);
	  LowestName = GetString (10, 2);
	  LowestScoreOfDay = RealScore;
	}
      else if (RealScore > HighestScoreOfDay)
	{
	  SetUserfenster (DebriefColor, RealScreen);	// KON_BG_COLOR
	  SetTextColor (DebriefColor, KON_TEXT_COLOR);	// KON_BG_COLOR
	  strcpy (Scoretext,
		  "\n   Highest Score of Day! \n Enter your name:");
	  DisplayText (Scoretext, USERFENSTERPOSX, USERFENSTERPOSY,
		       RealScreen, FALSE);
	  // free (HighestName);
	  // SwapScreen();
	  PrepareScaledSurface(TRUE);
	  HighestName = GetString (10, 2);
	  HighestScoreOfDay = RealScore;
	}
      free (Scoretext);

    }
  else
    {
      SaveHallptr = Hallptr;

      /* Wir brauchen keine Versager ! */
      if (RealScore == 0)
	return;
      /* Setzten der Umgebung */
      SetUserfenster (KON_BG_COLOR, RealScreen);
      SetTextColor (KON_BG_COLOR, KON_TEXT_COLOR);
      DisplayText
	(" You have gained entry to the hall\n of fame!\nEnter your name:\n  ",
	 USERFENSTERPOSX, USERFENSTERPOSY, RealScreen, FALSE);

      // SwapScreen();
	  PrepareScaledSurface(TRUE);

      /* Den neuen Eintrag in die Liste integrieren */
      if (Hallptr->PlayerScore < RealScore)
	{
	  Oldptr = Hallptr;
	  Hallptr = MyMalloc (sizeof (HallElement) + 1);
	  Hallptr->PlayerScore = RealScore;
	  Hallptr->PlayerName = GetString (18, 2);
	  Hallptr->NextPlayer = Oldptr;
	  SaveHallptr = Hallptr;
	}
      else
	{
	  Oldptr = Hallptr;
	  while (Hallptr->PlayerScore > RealScore)
	    {
	      Hallptr = Hallptr->NextPlayer;
	      if (Hallptr->PlayerScore > RealScore)
		Oldptr = Oldptr->NextPlayer;
	    }
	  Newptr = MyMalloc (sizeof (HallElement) + 1);
	  Newptr->PlayerScore = RealScore;
	  Newptr->PlayerName = GetString (18, 2);
	  Newptr->NextPlayer = Hallptr;
	  Oldptr->NextPlayer = Newptr;
	}

      /* Message an exit */
      DisplayText ("You are now added to the hall\n of fame!\n",
		   USERFENSTERPOSX, USERFENSTERPOSY, RealScreen, FALSE);
      Hallptr = SaveHallptr;
      // SwapScreen();
	  PrepareScaledSurface(TRUE);
      getchar ();
    } /* if (ParaPlusExtensions) */

  printf ("\nSurvived Debriefing! \n");

  return;

} /* Debriefing() */

/*-----------------------------------------------------------------
 * Desc: realise Pause-Mode: the game process is halted,
 * 	while the graphics and animations are not.  This mode 
 *	can further be toggled from PAUSE to CHEESE, which is
 * 	a feature from the original program that should probably
 * 	allow for better screenshots.
 *
 *      We have incorporated the "CHEESE" feature for completeness.
 *
 *-----------------------------------------------------------------*/
void
Pause (void)
{
  int Pause = TRUE;

  Me.status = PAUSE;
  SetInfoline ();
  UpdateInfoline ();
  GetView ();
  GetInternFenster (SHOW_ALL);
  PutInternFenster (TRUE);

  while ( Pause )
    {
      usleep (30000);
      AnimateInfluence ();
      AnimateRefresh ();
      RotateBulletColor ();
      AnimateEnemys ();
      GetView ();
      GetInternFenster (SHOW_ALL);
      PutInternFenster (TRUE);

      if (CPressed ())
	{
	  Me.status = CHEESE;
	  SetInfoline ();
	  UpdateInfoline ();
	  GetView ();
	  GetInternFenster (SHOW_ALL);
	  PutInternFenster (TRUE);

	  while (!SpacePressed ()); /* stay CHEESE until Space pressed */
	  while ( SpacePressed() ); /* then wait for Space released */
	  
	  Me.status = PAUSE;       /* return to normal PAUSE */
	  SetInfoline ();
	  UpdateInfoline ();
	} /* if (CPressed) */

      if ( SpacePressed() )
	{
	  Pause = FALSE;
	  while ( SpacePressed() );  /* wait for release */
	}

    } /* while (Pause) */

  return;

} /* Pause () */

void
UpdateCountersForThisFrame (void)
{
  // The next couter counts the frames displayed by freedroid during this
  // whole run!!  DO NOT RESET THIS COUNTER WHEN THE GAME RESTARTS!!
  Overall_Frames_Displayed++;
  Overall_Average = (Overall_Average*(Overall_Frames_Displayed-1) + Frame_Time()) / Overall_Frames_Displayed;

  // Here are some things, that were previously done by some periodic */
  // interrupt function
  ThisMessageTime++;

  LastGotIntoBlastSound += Frame_Time ();
  LastRefreshSound += Frame_Time ();

  if ( SkipAFewFrames ) SkipAFewFrames--;

  if ( Me.firewait > 0 )
    {
      Me.firewait-=Frame_Time()*20;
      if (Me.firewait < 0) Me.firewait=0;
    }
  if (ShipEmptyCounter > 1)
    ShipEmptyCounter--;
  if (WaitElevatorCounter > 0)
    WaitElevatorCounter--;
  if (CurLevel->empty > 2)
    CurLevel->empty--;
  if (RealScore > ShowScore)
    ShowScore++;
  if (RealScore < ShowScore)
    ShowScore--;
  UpdateInfoline ();
  SetInfoline ();

} /* UpdateCountersForThisFrame() */


#undef _freedroid_c
