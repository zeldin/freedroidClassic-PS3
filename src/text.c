/*----------------------------------------------------------------------
 *
 * Desc: contains all functions dealing with the HUGE, BIG font used for
 *	the top status line, the score and the text displayed during briefing
 *	and highscore inverview.  This has NOTHING to do with the fonts
 *	of the SVGALIB or the fonts used for the horizontal srolling
 *      message line!
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
#define _text_c

#include "system.h"

#include "defs.h"
#include "struct.h"
#include "proto.h"
#include "global.h"
#include "text.h"

int DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background );

char *Wordpointer;
unsigned char *Fontpointer;
unsigned char *Zeichenpointer[110];	/* Pointer-Feld auf Buchstaben-Icons */
unsigned int CurrentFontFG = FIRST_FONT_FG;	/* Momentane Schrift-Farben */
unsigned int CurrentFontBG = FIRST_FONT_BG;

int CharsPerLine;		/* Zeilenlaenge: veraltet */

/* Aktuelle Text-Einfuege-Position: */
int MyCursorX;
int MyCursorY;

/* Buffer fuer Text-Environment */
int StoreCursorX;
int StoreCursorY;

unsigned int StoreTextBG;
unsigned int StoreTextFG;

/* ----------------------------------------------------------------------
 * This function does the communication routine when the influencer in
 * transfer mode touched a friendly droid.
 *
 * ---------------------------------------------------------------------- */
void 
ChatWithFriendlyDroid( int Enum )
{
  char* RequestString;
  int i;
  SDL_Surface* Background;

  Activate_Conservative_Frame_Computation( );
  MakeGridOnScreen();
  // Now the background is basically there as we need it.  We store it
  // in its current pure form for later use as background for scrolling
  //
  Background = SDL_DisplayFormat( ne_screen );
  

  DisplayTextWithScrolling ( 
			    "Transfer channel protocol set up for text transfer...\n\n" , 
			    User_Rect.x , User_Rect.y , NULL , Background );

  printf_SDL( ne_screen, -1 , -1 , " Hello, this is %s unit \n" , Druidmap[AllEnemys[Enum].type].druidname  );

  while (1)
    {
      DisplayTextWithScrolling ( 
				"\nWhat do you say? " ,
				-1 , -1 , NULL , Background );
      DisplayTextWithScrolling ( ">" , -1 , -1 , NULL , Background );

      SDL_Flip ( ne_screen );
      RequestString = GetString( 20 , FALSE );
      // printf_SDL( ne_screen, -1 , -1 , "\n" ); // without this, we would write text over the entered string
      DisplayTextWithScrolling ( "\n    " , -1 , -1 , NULL , Background ); // without this, we would write text over the entered string

      //--------------------
      // Cause we do not want to deal with upper and lower case difficulties, we simpy convert 
      // the given input string into lower cases.  That will make pattern matching afterwards
      // much more reliable.
      //
      i=0;
      while ( RequestString[i] != 0 ) { RequestString[i]=tolower( RequestString[i] ); i++; }

      //--------------------
      // the quit command is always simple and clear.  We just need to end
      // the communication function. hehe.
      //
      if ( !strcmp ( RequestString , "quit" ) ) return;

      //--------------------
      // the help command is always simple and clear.  We just need to print out the 
      // following help text describing common command and keyword options and that's it.
      //
      if ( !strcmp ( RequestString , "help" ) ) 
	{
	  // DisplayTextWithScrolling("You have opend a communication channel to a friendly droid 
	  // by touching it while in transfer mode.\n\
	  // You can enter command phrases to make the droid perform some action.\n\
	  // Or you can ask the droid for valuable information by entering the keywords you request information about.\n\
	  // Most useful command phrases are: follow stay\n\
	  // Often useful information requests are: job name status MS\n\
	  // Type quit to cancel communication." , MyCursorX , MyCursorY , NULL , Background );
	  DisplayTextWithScrolling("You can enter command phrases or ask about some keyword.\n\
Most useful command phrases are: FOLLOW STAY STATUS \n\
Often useful information requests are: JOB NAME MS HELLO \n\
Of course you can ask the droid about anything else it has told you or about what you have heard somewhere else." , 
				   MyCursorX , MyCursorY , NULL , Background );
	  continue;
	}
      
      //--------------------
      // If the player requested the robot to follow him, this robot should switch to 
      // following mode and follow the 001 robot.  But this is at the moment not implemented.
      // Instead the robot will just print out the message, that he would follow, but don't
      // do anything at this time.
      //
      // Same holds true for the 'stay' command
      //
      if ( !strcmp ( RequestString , "follow" ) ) 
	{
	  DisplayText( 
		      "Ok.  I'm on your tail.  I hope you know where you're going.  I'll do my best to keep up." , 
		      MyCursorX , MyCursorY , NULL );
	  continue;
	}
      if ( !strcmp ( RequestString , "stay" ) )
	{
	  DisplayText( 
		      "Ok.  I'll stay here and wait for further instructions from you.  \n\
I hope you know what you're doing." , 
		      MyCursorX , MyCursorY , NULL );
	  continue;
	}

      //--------------------
      // At this point we know, that none of the default answers applied.
      // We therefore will search this robots question-answer-list for a
      // match in the question entries and if applicable print out the
      // matching answer of course.
      //

      for ( i = 0 ; i < MAX_CHAT_KEYWORDS_PER_DROID ; i++ )
	{
	  if ( !strcmp ( RequestString , AllEnemys[ Enum ].QuestionResponseList[ i * 2 ] ) ) // even entries = questions
	    {
	      DisplayTextWithScrolling ( AllEnemys[ Enum ].QuestionResponseList[ i * 2 + 1 ] , 
					 -1 , -1 , NULL , Background );
	      break;
	    }
	}


      //--------------------
      // In case non of the default keywords was said and also none of the
      // special keywords this droid would understand were said, then the
      // droid obviously hasn't understood the message and should also say
      // so.
      //
      if ( i == MAX_CHAT_KEYWORDS_PER_DROID )
	{
	  DisplayTextWithScrolling ( "Sorry, but of that I know entirely nothing." , 
				     -1 , -1 , NULL , Background );
	}
    }
}; // void ChatWithFriendlyDroid( int Enum );

void 
EnemyHitByBulletText( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Hit_Text ) return;
  
  ThisRobot->TextVisibleTime=0;
  if ( !ThisRobot->Friendly )
    switch (MyRandom(4))
    {
    case 0:
      ThisRobot->TextToBeDisplayed="Unhandled exception fault.  Press ok to reboot.";
      break;
    case 1:
      ThisRobot->TextToBeDisplayed="System fault. Please buy a newer version.";
      break;
    case 2:
      ThisRobot->TextToBeDisplayed="System error. Might be a virus.";
      break;
    case 3:
      ThisRobot->TextToBeDisplayed="System error. Pleae buy an upgrade from MS.";
      break;
    case 4:
      ThisRobot->TextToBeDisplayed="System error. Press any key to reboot.";
      break;
    }
  else
    ThisRobot->TextToBeDisplayed="Aargh, I got hit.  Ugh, I got a bad feeling...";
}; // void EnemyHitByBullet( int Enum );

void 
EnemyInfluCollisionText ( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Bump_Text ) return;
  
  ThisRobot->TextVisibleTime=0;
	      
  if ( ThisRobot->Friendly )
    {
      ThisRobot->TextToBeDisplayed="Ah, good, that we have an open collision avoiding standard, isn't it.";
    }
  else
    {
      switch (MyRandom(1))
	{
	case 0:
	  ThisRobot->TextToBeDisplayed="Hey, I'm from MS! Walk outa my way!";
	  break;
	case 1:
	  ThisRobot->TextToBeDisplayed="Hey, I know the big MS boss! You better go.";
	  break;
	}
    }

} // void AddStandingAndAimingText( int Enum )


void 
AddStandingAndAimingText ( int Enum )
{
  Enemy ThisRobot=&AllEnemys[ Enum ];

  if ( !GameConfig.Enemy_Aim_Text ) return;
  
  ThisRobot->TextVisibleTime=0;
	      
  if ( ( fabsf (Me.speed.x) < 1 ) && ( fabsf (Me.speed.y) < 1 ) )
    {
      ThisRobot->TextToBeDisplayed="Yeah, stay like that, haha.";
    }
  else
    {
      ThisRobot->TextToBeDisplayed="Stand still while I aim at you.";
    }

} // void AddStandingAndAimingText( int Enum )


void
AddInfluBurntText( void )
{
  int FinalTextNr;

  if ( !GameConfig.Influencer_Blast_Text ) return;
  
  Me.TextVisibleTime=0;
  
  FinalTextNr=MyRandom ( 6 );
  switch ( FinalTextNr )
    {
    case 0:
      Me.TextToBeDisplayed="Aaarrgh, aah, that burnt me!";
      break;
    case 1:
      Me.TextToBeDisplayed="Hell, that blast was hot!";
      break;
    case 2:
      Me.TextToBeDisplayed="Ghaart, I hate to stain my chassis like that.";
      break;
    case 3:
      Me.TextToBeDisplayed="Oh no!  I think I've burnt a cable!";
      break;
    case 4:
      Me.TextToBeDisplayed="Oh no, my poor transfer connectors smolder!";
      break;
    case 5:
      Me.TextToBeDisplayed="I hope that didn't melt any circuits!";
      break;
    case 6:
      Me.TextToBeDisplayed="So that gives some more black scars on me ol' dented chassis!";
      break;
    default:
      printf("\nError in AddInfluBurntText! That shouldn't be happening.");
      Terminate(ERR);
      break;
    }
} // void AddInfluBurntText

/*-----------------------------------------------------------------
 * @Desc: Setzt die Palettenwerten (und nicht die RGB-Werte) der
 * 	Proportionalschrift Null als Farbwert bewirkt keinen Effekt
 * 	Sicherheitsabfrage, ob die schrift nicht durch
 * 	Kontrastzerst"orung vernichtet wird
 * 
 * sets only, if color != 0 and other then old color
 * 
 *
 *-----------------------------------------------------------------*/
void
SetTextColor (unsigned char bg, unsigned char fg)
{

  return;

}				/* SetTextcolor */

/* ====================================================================== 
   Diese Funktion soll die momentane Farbsituation des Textes wiedergeben.
	Dazu werden zwei Pointer "ubergeben, damit sie auch ver"andert werden.
		
	Grund f"ur die Funktion: so sparen wir zwei weitere globale Variablen
   ---------------------------------------------------------------------- */
void
GetTextColor (unsigned int *bg, unsigned int *fg)
{
  *bg = CurrentFontBG;
  *fg = CurrentFontFG;
}

/*@Function============================================================
@Desc: SetTextCursor(x, y): Setzt Cursor fuer folgende Textausgaben

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void
SetTextCursor (int x, int y)
{
  MyCursorX = x;
  MyCursorY = y;

  return;
}

/*-----------------------------------------------------------------
 *  scrolls a given text down inside the User-window, 
 *  defined by the global SDL_Rect User_Rect
 *
 *  startx/y give the Start-position, 
 *  EndLine is the last line (?)
 *
 *-----------------------------------------------------------------*/
int
ScrollText (char *Text, int startx, int starty, int EndLine , char* TitlePictureName )
{
  int Number_Of_Line_Feeds = 0;		/* Anzahl der Textzeilen */
  char *textpt;			/* bewegl. Textpointer */
  int InsertLine = starty;
  int speed = +4;
  int maxspeed = 8;

  // printf("\nScrollTest should be starting to scroll now...");

  // getchar();

   /* Zeilen zaehlen */
  textpt = Text;
  while (*textpt++)
    if (*textpt == '\n')
      Number_Of_Line_Feeds++;

  while ( !SpacePressed () )
    {
      if (UpPressed ())
	{
	  speed--;
	  if (speed < -maxspeed)
	    speed = -maxspeed;
	}
      if (DownPressed ())
	{
	  speed++;
	  if (speed > maxspeed)
	    speed = maxspeed;
	}

      usleep (30000);

      DisplayImage ( find_file(TitlePictureName,GRAPHICS_DIR, FALSE) );
      MakeGridOnScreen();
      // ClearUserFenster(); 

      if (!DisplayText (Text, startx, InsertLine, &User_Rect))
	{
	  // JP: I've disabled this, since with this enabled we won't even
	  // see a single line of the first section of the briefing.
	  // But this leads to that we currently NEVER can see the second 
	  // or third part of the briefing text, cause it will not start
	  // the new text part when the lower end of the first text part
	  // is reached.  I don't consider this bug release-critical.
	  //
	  // break;  /* Text has been scrolled outside User_Rect */
	}

      InsertLine -= speed;

      SDL_Flip (ne_screen);

      /* Nicht bel. nach unten wegscrollen */
      if (InsertLine > SCREENHEIGHT - 10 && (speed < 0))
	{
	  InsertLine = SCREENHEIGHT - 10;
	  speed = 0;
	}

    } /* while !Space_Pressed */

  return OK;
}				// void ScrollText(void)

/*-----------------------------------------------------------------
 * This function is much like DisplayText, but with the main difference,
 * that in case of the whole clipping window filled, the function will
 * display a ---more--- line, wait for a key and then scroll the text
 * further up.
 *
 * @Desc: prints *Text beginning at positions startx/starty, 
 * 
 *	and respecting the text-borders set by clip_rect
 *      -> this includes clipping but also automatic line-breaks
 *      when end-of-line is reached
 * 
 *      if clip_rect==NULL, no clipping is performed
 *      
 *      NOTE: the previous clip-rectange is restored before
 *            the function returns!
 *
 *      NOTE2: this function _does not_ update the screen
 *
 * @Ret: TRUE if some characters where written inside the clip rectangle
 *       FALSE if not (used by ScrollText to know if Text has been scrolled
 *             out of clip-rect completely)
 *-----------------------------------------------------------------*/
int
DisplayTextWithScrolling (char *Text, int startx, int starty, const SDL_Rect *clip , SDL_Surface* Background )
{
  char *tmp;	/* Beweg. Zeiger auf aktuelle Position im Ausgabe-Text */
  // SDL_Rect Temp_Clipping_Rect; // adding this to prevent segfault in case of NULL as parameter

  SDL_Rect store_clip;

  if ( startx != -1 ) MyCursorX = startx;		
  if ( starty != -1 ) MyCursorY = starty;

  SDL_GetClipRect (ne_screen, &store_clip);  /* store previous clip-rect */
  if (clip)
    SDL_SetClipRect (ne_screen, clip);
  else
    {
      clip = & User_Rect;
    }


  tmp = Text;			/* running text-pointer */

  while ( *tmp && (MyCursorY < clip->y + clip->h) )
    {
      if ( *tmp == '\n' )
	{
	  MyCursorX = clip->x;
	  MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;

	  //--------------------
	  // Here we plant in the question for ---more--- and a key to be pressed,
	  // before we clean the screen and restart displaying text from the top
	  // of the given Clipping rectangle
	  //
	  if ( ( clip->h + clip->y - MyCursorY ) <= 2 * FontHeight ( GetCurrentFont() ) * TEXT_STRETCH )
	    {
	      DisplayText( "--- more --- more --- more --- more ---\n" , MyCursorX , MyCursorY , clip );
	      SDL_Flip( ne_screen );
	      while ( !SpacePressed() );
	      while (  SpacePressed() );
	      SDL_BlitSurface( Background , NULL , ne_screen , NULL );
	      MyCursorY = clip->y;
	      SDL_Flip( ne_screen );
	    };
	}
      else
	DisplayChar (*tmp);

      tmp++;

      if (clip)
	ImprovedCheckUmbruch(tmp, clip);   /* dont write over right border */

    } // while !FensterVoll()

   SDL_SetClipRect (ne_screen, &store_clip); /* restore previous clip-rect */

  /*
   * ScrollText() wants to know if we still wrote something inside the
   * clip-rectangle, of if the Text has been scrolled out
   */
   if ( clip && ((MyCursorY < clip->y) || (starty > clip->y + clip->h) ))
     return FALSE;  /* no text was written inside clip */
   else
     return TRUE; 

};

/*-----------------------------------------------------------------
 * @Desc: prints *Text beginning at positions startx/starty, 
 * 
 *	and respecting the text-borders set by clip_rect
 *      -> this includes clipping but also automatic line-breaks
 *      when end-of-line is reached
 * 
 *      if clip_rect==NULL, no clipping is performed
 *      
 *      NOTE: the previous clip-rectange is restored before
 *            the function returns!
 *
 *      NOTE2: this function _does not_ update the screen
 *
 * @Ret: TRUE if some characters where written inside the clip rectangle
 *       FALSE if not (used by ScrollText to know if Text has been scrolled
 *             out of clip-rect completely)
 *-----------------------------------------------------------------*/
int
DisplayText (char *Text, int startx, int starty, const SDL_Rect *clip)
{
  char *tmp;	/* Beweg. Zeiger auf aktuelle Position im Ausgabe-Text */
  SDL_Rect Temp_Clipping_Rect; // adding this to prevent segfault in case of NULL as parameter

  SDL_Rect store_clip;

  if ( startx != -1 ) MyCursorX = startx;		
  if ( starty != -1 ) MyCursorY = starty;


  SDL_GetClipRect (ne_screen, &store_clip);  /* store previous clip-rect */
  if (clip)
    SDL_SetClipRect (ne_screen, clip);
  else
    {
      clip = & Temp_Clipping_Rect;
      Temp_Clipping_Rect.x=0;
      Temp_Clipping_Rect.y=0;
      Temp_Clipping_Rect.w=SCREENLEN;
      Temp_Clipping_Rect.h=SCREENHEIGHT;
    }


  tmp = Text;			/* running text-pointer */

  while ( *tmp && (MyCursorY < clip->y + clip->h) )
    {
      if ( *tmp == '\n' )
	{
	  MyCursorX = clip->x;
	  MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;
	}
      else
	DisplayChar (*tmp);

      tmp++;

      if (clip)
	ImprovedCheckUmbruch(tmp, clip);   /* dont write over right border */

    } // while !FensterVoll()

   SDL_SetClipRect (ne_screen, &store_clip); /* restore previous clip-rect */

  /*
   * ScrollText() wants to know if we still wrote something inside the
   * clip-rectangle, of if the Text has been scrolled out
   */
   if ( clip && ((MyCursorY < clip->y) || (starty > clip->y + clip->h) ))
     return FALSE;  /* no text was written inside clip */
   else
     return TRUE; 

} // DisplayText(...)

/*-----------------------------------------------------------------
 * @Desc: This function displays a char.  It uses Menu_BFont now
 * to do this.  MyCursorX is  updated to new position.
 *
 *
 -----------------------------------------------------------------*/
void
DisplayChar (unsigned char c)
{

  if ( !isprint(c) ) // don't accept non-printable characters
    {
      printf ("Illegal char passed to DisplayChar(): %d \n", c);
      Terminate(ERR);
    }

  PutChar (ne_screen, MyCursorX, MyCursorY, c);

  // After the char has been displayed, we must move the cursor to its
  // new position.  That depends of course on the char displayed.
  //
  MyCursorX += CharWidth ( GetCurrentFont() , c);
  
} // void DisplayChar(...)


/*@Function============================================================
  @Desc: This function checks if the next word still fits in this line
  of text and initiates a carriage return/line feed if not.
  Very handy and convenient, for that means it is no longer nescessary
  to enter \n in the text every time its time for a newline. cool.
  
  The function could perhaps still need a little improvement.  But for
  now its good enough and improvement enough in comparison to the old
  CheckUmbruch function.

  rp: added argument clip, which contains the text-window we're writing in
       (formerly known as "TextBorder")

  @Ret: 
  @Int:
* $Function----------------------------------------------------------*/
void
ImprovedCheckUmbruch (char* Resttext, const SDL_Rect *clip)
{
  int i;
  int NeededSpace=0;
#define MAX_WORD_LENGTH 100

  // In case of a space, see if the next word will still fit on the line
  // and do a carriage return/line feed if not
  if ( *Resttext == ' ' ) {
    for (i=1;i<MAX_WORD_LENGTH;i++) 
      {
	if ( (Resttext[i] != ' ') && (Resttext[i] != 0) )
	  { 
	    NeededSpace+=CharWidth( GetCurrentFont() , Resttext[i] );
	    if ( MyCursorX+NeededSpace > clip->x + clip->w - 10 )
	      {
		MyCursorX = clip->x;
		MyCursorY += FontHeight ( GetCurrentFont() ) * TEXT_STRETCH;
		return;
	      }
	  }
	else 
	  return;
      }
  }
} // void ImprovedCheckUmbruch(void)


/*-----------------------------------------------------------------
 * @Desc: reads a string of "MaxLen" from User-input, and echos it 
 *        either to stdout or using graphics-text, depending on the
 *        parameter "echo":	echo=0    no echo
 * 		               	echo=1    print using printf
 *         			echo=2    print using graphics-text
 *
 *     values of echo > 2 are ignored and treated like echo=0
 *
 *  NOTE: MaxLen is the maximal _strlen_ of the string (excl. \0 !)
 * 
 * @Ret: char *: String is allocated _here_!!!
 *       (dont forget to free it !)
 * 
 *-----------------------------------------------------------------*/
char *
GetString (int MaxLen, int echo)
{
  char *input;		/* Pointer auf eingegebenen String */
  int key;             /* last 'character' entered */
  int curpos;		/* zaehlt eingeg. Zeichen mit */
  int finished;
  int x0, y0, height;
  SDL_Rect store_rect, tmp_rect;
  SDL_Surface *store = NULL;

  if (echo == 1)		/* echo to stdout */
    {
      printf ("\nGetString(): sorry, echo=1 currently not implemented!\n");
      return NULL;
    }

  x0 = MyCursorX;
  y0 = MyCursorY;
  height = FontHeight (GetCurrentFont());
  
  store = SDL_CreateRGBSurface(0, SCREENLEN, height, ne_bpp, 0, 0, 0, 0);
  Set_Rect (store_rect, x0, y0, SCREENLEN, height);
  SDL_BlitSurface (ne_screen, &store_rect, store, NULL);

  /* Speicher fuer Eingabe reservieren */
  input     = MyMalloc (MaxLen + 5);

  memset (input, '.', MaxLen);
  input[MaxLen] = 0;
  
  finished = FALSE;
  curpos = 0;

  while ( !finished  )
    {
      Copy_Rect( store_rect, tmp_rect);
      SDL_BlitSurface (store, NULL, ne_screen, &tmp_rect);
      PutString (ne_screen, x0, y0, input);
      SDL_Flip (ne_screen);
      
      key = getchar_raw ();  
      
      if (key == SDLK_RETURN) 
	{
	  input[curpos] = 0;
	  finished = TRUE;
	}
      else if (isprint (key) && (curpos < MaxLen) )  
	{
	  /* printable characters are entered in string */
	  input[curpos] = (char) key;   
	  curpos ++;
	}
      else if (key == SDLK_BACKSPACE)
	{
	  if ( curpos > 0 ) curpos --;
	  input[curpos] = '.';
	}
      
    } /* while(!finished) */

  DebugPrintf (2, "\n\nchar *GetString(..):  The final string is:\n");
  DebugPrintf (2,  input );
  DebugPrintf (2, "\n\n");


  return (input);

} /* GetString() */

/*-----------------------------------------------------------------
 *
 * similar to putchar(), using SDL via the BFont-fct PutChar().
 *
 * sets MyCursor[XY], and allows passing (-1,-1) as coords to indicate
 *  using the current cursor position.
 *
 *-----------------------------------------------------------------*/
int
putchar_SDL (SDL_Surface *Surface, int x, int y, int c)
{
  int ret;
  if (x == -1) x = MyCursorX;
  if (y == -1) y = MyCursorY;
  
  MyCursorX = x + CharWidth (GetCurrentFont(), c);
  MyCursorY = y;

  ret = PutChar (Surface, x, y, c);

  SDL_Flip (Surface);

  return (ret);
}


/*-----------------------------------------------------------------
 * behaves similarly as gl_printf() of svgalib, using the BFont
 * print function PrintString().
 *  
 *  sets current position of MyCursor[XY],  
 *     if last char is '\n': to same x, next line y
 *     to end of string otherwise
 *
 * Added functionality to PrintString() is: 
 *  o) passing -1 as coord uses previous x and next-line y for printing
 *  o) Screen is updated immediatly after print, using SDL_flip()                       
 *
 *-----------------------------------------------------------------*/
void
printf_SDL (SDL_Surface *screen, int x, int y, char *fmt, ...)
{
  va_list args;
  int i;

  char *tmp;
  va_start (args, fmt);

  if (x == -1) x = MyCursorX;
  else MyCursorX = x;

  if (y == -1) y = MyCursorY;
  else MyCursorY = y;

  tmp = (char *) MyMalloc (10000 + 1);
  vsprintf (tmp, fmt, args);
  PutString (screen, x, y, tmp);

  SDL_Flip (screen);

  if (tmp[strlen(tmp)-1] == '\n')
    {
      MyCursorX = x;
      MyCursorY = y+ 1.1* (GetCurrentFont()->h);
    }
  else
    {
      for (i=0; i < strlen(tmp); i++)
	MyCursorX += CharWidth (GetCurrentFont(), tmp[i]);
      MyCursorY = y;
    }

  free (tmp);
  va_end (args);
}


#undef _text_c
