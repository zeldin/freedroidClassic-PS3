/* 
 *
 *   Copyright (c) 1994, 2002 Johannes Prix
 *   Copyright (c) 1994, 2002 Reinhard Prix
 *
 *
 *  This file is part of FreeDroid
 *
 *  FreeDroid is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  FreeDroid is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with FreeDroid; see the file COPYING. If not, write to the 
 *  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
 *  MA  02111-1307  USA
 *
 */

/* *********************************************************************** */
/* *************************  P R O T O T Y P E N  *********************** */
/* *********************************************************************** */
#ifndef _proto_h
#define _proto_h

#include "SDL.h"

/* paraplus.c */
#undef EXTERN
#ifdef _paraplus_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN float Frame_Time (void);
EXTERN void Activate_Conservative_Frame_Computation(void);
EXTERN int ShipEmptyCounter;
EXTERN int WaitElevatorCounter;
EXTERN int TimerFlag;
EXTERN void PutEnergy (void);
EXTERN void ThouArtDefeated (void);
EXTERN void ThouArtVictorious (void);
EXTERN void StandardEnergieLieferungen (void);

/* paraplus.c */
#undef EXTERN
#ifdef _parainit_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void parse_command_line (int argc, char *const argv[]);
EXTERN void Title (void);
EXTERN void EndTitle (void);
EXTERN void InitParaplus (void);
EXTERN void InitNewGame (void);

/* influ.c */
#undef EXTERN
#ifdef _influ_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN int isignf (float);
EXTERN void MoveInfluence (void);
EXTERN void AdjustSpeed (void);
EXTERN void BounceInfluencer (void);
EXTERN void Reibung (void);
EXTERN void CheckEnergieLevel (void);
EXTERN void AnimateInfluence (void);
EXTERN void InfluenceEnemyCollision (void);
EXTERN void RefreshInfluencer (void);
EXTERN void ExplodeInfluencer (void);

/* bullet.c */
#undef EXTERN
#ifdef _bullet_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void FireBullet (void);
EXTERN void MoveBullets (void);
EXTERN void DeleteBullet (int num);
EXTERN void StartBlast (int x, int y, int type);
EXTERN void ExplodeBlasts (void);
EXTERN void DeleteBlast (int num);
EXTERN void CheckBulletCollisions (int num);
EXTERN void CheckBlastCollisions (int num);

/* view.c */
#undef EXTERN
#ifdef _view_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN unsigned char *FeindZusammenstellen (const char *, int);
// EXTERN void SetColors(int,int,char*);
EXTERN void RotateBulletColor (void);
EXTERN void GetView (void);
EXTERN void ShowPosition (void);
EXTERN void DisplayView (void);
EXTERN void GetInternFenster (int mask);
EXTERN void PutInfluence (void);
EXTERN void PutBullet (int);
EXTERN void PutBlast (int);
EXTERN void PutEnemy (int);
EXTERN void PutInternFenster (void);
EXTERN int PutObject (int x, int y, unsigned char *pic, int check);
EXTERN void DrawDigit (unsigned char *, unsigned char *);
EXTERN void RedrawInfluenceNumber (void);
EXTERN void SetUserfenster (int color, unsigned char *screen);
EXTERN void ShowRobotPicture (int PosX, int PosY, int Number,
			      unsigned char *Screen);


/* blocks.c */
#undef EXTERN
#ifdef _blocks_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void SmallBlock (int, int, int, unsigned char *, int);
EXTERN void SmallEnemy (int, int, int, unsigned char *, int);
EXTERN void SmallBlast (int, int, int, int, unsigned char *, int);
EXTERN void SmallBullet (int, int, int, int, unsigned char *, int);

EXTERN void GetMapBlocks (void);
EXTERN void GetShieldBlocks (void);
EXTERN void IsolateBlock (unsigned char *screen,
			  unsigned char *target,
			  int BlockEckLinks,
			  int BlockEckOben, int Blockbreite, int Blockhoehe);

EXTERN unsigned char *GetBlocks (char *picfile, int line, int num);
EXTERN int MergeBlockToWindow (register unsigned char *source,
			       register unsigned char *target,
			       int WinLineLen, int check);

EXTERN void GetDigits (void);

EXTERN void DisplayBlock (int x, int y,
			  unsigned char *block,
			  int len, int height, unsigned char *screen);


EXTERN void DisplayMergeBlock (int x, int y,
			       unsigned char *block,
			       int len, int height, unsigned char *screen);

EXTERN void CopyMergeBlock (unsigned char *target,
			    unsigned char *source, int mem);

EXTERN int MergeBlockToWindow (unsigned char *,
			       unsigned char *, int WinLineLen, int check);

/* graphics.c */
#undef EXTERN
#ifdef _graphics_c
#define EXTERN
#else
#define EXTERN extern
#endif

// EXTERN void Lock_SDL_Screen(void);
// EXTERN void Unlock_SDL_Screen(void);
// EXTERN void Update_SDL_Screen(void);
// EXTERN Uint32 getpixel(SDL_Surface *surface, int x, int y);
// EXTERN void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
EXTERN void display_bmp(char *file_name);
EXTERN void MakeGridOnScreen(void);
EXTERN int InitPictures (void);
EXTERN void SwapScreen (void);
EXTERN void CopyScreenToInternalScreen(void);
EXTERN void ClearVGAScreen (void);
EXTERN void Monitorsignalunterbrechung (int);
EXTERN void SetColors (int FirstCol, int PalAnz, char *PalPtr);
EXTERN void SetPalCol (unsigned int palpos, unsigned char rot,
		       unsigned char gruen, unsigned char blau);
EXTERN void SetPalCol2 (unsigned int palpos, color Farbwert);
EXTERN int InitLevelColorTable (void);
EXTERN int InitPalette (void);
EXTERN void SetLevelColor (int);
EXTERN void Load_PCX_Image (char *, unsigned char *, int);
EXTERN void LadeLBMBild (char *LBMDateiname, unsigned char *Screen,
			 int LoadPal);
EXTERN void TransparentLadeLBMBild (char *LBMDateiname, unsigned char *Screen,
				    int LoadPal);
EXTERN void Set_SVGALIB_Video_ON (void);
EXTERN void Set_SVGALIB_Video_OFF (void);
EXTERN void WaitVRetrace (void);
EXTERN void UnfadeLevel (void);
EXTERN void FadeLevel (void);
EXTERN void FadeColors1 (void);
EXTERN void FadeColors2 (void);
EXTERN void LadeZeichensatz (char *Zeichensatzname);
EXTERN void RotateColors (int, int);
EXTERN void LevelGrauFaerben (void);
EXTERN void ClearGraphMem (unsigned char *screen);
EXTERN void Flimmern (void);

/* map.c */
#undef EXTERN
#ifdef _map_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN unsigned char GetMapBrick (Level deck, float x, float y);
EXTERN int GetCurrentElevator (void);
EXTERN void ActSpecialField (float, float);

EXTERN int LoadShip (char *shipname);
EXTERN Level LevelToStruct (char *data);
EXTERN int GetDoors (Level Lev);
EXTERN int GetWaypoints (Level Lev);
EXTERN int GetRefreshes (Level Lev);
EXTERN int GetElevatorConnections (char *shipname);
EXTERN int GetCrew (char *shipname);

EXTERN void AnimateRefresh (void);
EXTERN void MoveLevelDoors (void);
EXTERN int IsPassable (int x, int y, int Checkpos);
EXTERN int DruidPassable (int x, int y);
EXTERN int IsVisible (Finepoint objpos);
EXTERN int TranslateMap (Level Lev);

/* sound.c  OR nosound.c */
#undef EXTERN
#ifdef _sound_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void YIFF_Server_Check_Events (void);
EXTERN int Init_YIFF_Sound_Server (void);
EXTERN void YIFF_Server_Close_Connections (void);
EXTERN void Switch_Background_Music_To (int);
EXTERN void Play_YIFF_Server_Sound (int);
EXTERN void StartSound (int);
EXTERN void GotHitSound (void);
EXTERN void GotIntoBlastSound (void);
EXTERN void CrySound (void);
EXTERN void RefreshSound (void);
EXTERN void MoveElevatorSound (void);
EXTERN void MenuItemSelectedSound (void);
EXTERN void MoveMenuPositionSound (void);
EXTERN void EnterElevatorSound (void);
EXTERN void LeaveElevatorSound (void);
// EXTERN void FireBulletSound (void);
EXTERN void Fire_Bullet_Sound (int);
EXTERN void BounceSound (void);
EXTERN void DruidBlastSound (void);
EXTERN void ThouArtDefeatedSound (void);
EXTERN void Takeover_Set_Capsule_Sound (void);
EXTERN void Takeover_Game_Won_Sound (void);
EXTERN void Takeover_Game_Deadlock_Sound (void);
EXTERN void Takeover_Game_Lost_Sound (void);

/* keyboard.c */
#undef EXTERN
#ifdef _keyboard_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void Init_SDL_Keyboard(void);
EXTERN int keyboard_update(void);
EXTERN void ClearKbState (void);
EXTERN int LeftPressed (void);
EXTERN int RightPressed (void);
EXTERN int UpPressed (void);
EXTERN int DownPressed (void);
EXTERN int SpacePressed (void);
EXTERN int EnterPressed (void);
EXTERN int EscapePressed (void);
EXTERN int CPressed (void);
EXTERN int PPressed (void);
EXTERN int QPressed (void);
EXTERN int WPressed (void);
EXTERN int DPressed (void);
EXTERN int LPressed (void);
EXTERN int IPressed (void);
EXTERN int NoDirectionPressed (void);
EXTERN int SetTypematicRate (unsigned char);
EXTERN void KillTastaturPuffer (void);
EXTERN void JoystickControl (void);

/* misc.c */
#undef EXTERN
#ifdef _misc_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void DebugPrintf (char *);
EXTERN void DebugPrintfInt (int);
EXTERN void DebugPrintfFloat (float);
EXTERN void gotoxy (int, int);
EXTERN int MyRandom (int);
EXTERN void reverse (char *);
EXTERN char *itoa (int, char *, int);
EXTERN char *ltoa (long, char *, int);
EXTERN void Armageddon (void);
EXTERN void Teleport (int LNum, int X, int Y);
EXTERN void Cheatmenu (void);
EXTERN void OptionsMenu (void);
EXTERN void InsertNewMessage (void);
EXTERN void Terminate (int);
EXTERN void KillQueue (void);
EXTERN void PutMessages (void);
EXTERN void InsertMessage (char *MText);
EXTERN void *MyMalloc (long);
EXTERN void DirToVect (int dir, Vect vector);

EXTERN long my_sqrt (long);
EXTERN int my_abs (int);

EXTERN void ShowDebugInfos (void);

/* enemy.c */
#undef EXTERN
#ifdef _enemy_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void InitEnemys (void);
EXTERN void ShuffleEnemys (void);
EXTERN int EnemyEnemyCollision (int enemynum);
EXTERN void MoveEnemys (void);
EXTERN void AttackInfluence (int enemynum);
EXTERN void AnimateEnemys (void);
EXTERN void ClearEnemys (void);
EXTERN int ClassOfDruid (int druidtype);

/* ship.c */
#undef EXTERN
#ifdef _ship_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void ShowDeckMap (Level deck);
EXTERN void EnterElevator (void);
EXTERN void EnterKonsole (void);
EXTERN void AlleLevelsGleichFaerben (void);
EXTERN void HilightLevel (int);
EXTERN void HilightElevator (int);
EXTERN int LevelEmpty (void);
EXTERN int ShipEmpty (void);
EXTERN void ClearUserFenster (void);


/* paratext.c */
#undef EXTERN
#ifdef _paratext_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void StoreTextEnvironment (void);
EXTERN void RestoreTextEnvironment (void);

EXTERN int InitParaplusFont (void);

EXTERN void SetTextColor (unsigned bg, unsigned fg);
EXTERN void GetTextColor (unsigned int *bg, unsigned int *fg);

EXTERN void SetTextBorder (int left, int upper, int right, int lower,
			   int chars);
EXTERN void ClearTextBorder (unsigned char *screen, int color);
EXTERN void SetTextCursor (int x, int y);
EXTERN void SetLineLength (int);

EXTERN void DisplayText (char *text, int startx, int starty,
			 unsigned char *screen, int EnterCursor);
EXTERN void DisplayChar (unsigned char Zeichen, unsigned char *screen);
EXTERN int ScrollText (char *text, int startx, int starty, int EndLine);

EXTERN void CheckUmbruch (void);
EXTERN void ImprovedCheckUmbruch(char *);
EXTERN void MakeUmbruch (void);
EXTERN int FensterVoll (void);
EXTERN char *PreviousLine (char *textstart, char *text);
EXTERN char *NextLine (char *text);
EXTERN char *GetString (int);

/* rahmen.c */
#undef EXTERN
#ifdef _rahmen_c
#define EXTERN
#else
#define EXTERN extern
#endif
EXTERN void SayLeftInfo (char *text, unsigned char *screen);
EXTERN void SayRightInfo (char *text, unsigned char *screen);
EXTERN void DisplayRahmen (unsigned char *screen);
EXTERN void SetInfoline (void);
EXTERN void UpdateInfoline (void);


/* takeover.c */
#undef EXTERN
#ifdef _takeover_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN int Takeover (int enemynum);
EXTERN void ChooseColor (void);
EXTERN void PlayGame (void);
EXTERN void EnemyMovements (void);

EXTERN int GetTakeoverGraphics (void);
EXTERN void ShowPlayground (void);
EXTERN void InventPlayground (void);

EXTERN void ProcessPlayground (void);
EXTERN void ProcessDisplayColumn (void);
EXTERN void ProcessCapsules (void);

EXTERN void ClearPlayground (void);
EXTERN int IsActive (int color, int row);
EXTERN void InitTakeover (void);

#undef EXTERN
#ifdef _svgaemu_c
#define EXTERN
#else
#define EXTERN extern
#endif

EXTERN void PrepareScaledSurface(void);
EXTERN void Lock_SDL_Screen(void);
EXTERN void Unlock_SDL_Screen(void);
EXTERN void Update_SDL_Screen(void);
EXTERN Uint32 getpixel(SDL_Surface *surface, int x, int y);
EXTERN void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);



EXTERN void gl_printf(int x, int y, const char *fmt,...);
EXTERN void gl_expandfont(int fw, int fh, int c, void *sfdp, void *dfdp);
EXTERN void gl_setfont(int fw, int fh, void *fdp);
EXTERN void gl_colorfont(int fw, int fh, int c, void *fdp);
EXTERN void gl_setwritemode(int wm);
EXTERN void gl_write(int x, int y, char *s);
EXTERN void gl_writen(int x, int y, int n, char *s);
EXTERN void gl_setfontcolors(int bg, int fg);
EXTERN void gl_setpalettecolor(int c, int r, int b, int g);
EXTERN void gl_getpalettecolor(int c, int *r, int *b, int *g);
EXTERN void gl_setpalettecolors(int s, int n, void *dp);
EXTERN void gl_getpalettecolors(int s, int n, void *dp);
EXTERN void gl_setpalette(void *p);
EXTERN void gl_getpalette(void *p);
EXTERN void gl_setrgbpalette(void);
EXTERN void gl_clearscreen(int c);
EXTERN void gl_scalebox(int w1, int h1, void *sb, int w2, int h2, void *db);
EXTERN void gl_setdisplaystart(int x, int y);
EXTERN void gl_enableclipping(void);
EXTERN void gl_setclippingwindow(int x1, int y1, int x2, int y2);
EXTERN void gl_disableclipping(void);
EXTERN void gl_putbox(int x, int y, int w, int h, void *dp);
EXTERN int gl_setcontextvga(int m);
EXTERN void gl_hline(int x1, int y, int x2, int c);
EXTERN int keyboard_init(void);
EXTERN int keyboard_init_return_fd(void);
EXTERN void keyboard_close(void);
EXTERN void keyboard_setdefaulteventhandler(void);
EXTERN char *keyboard_getstate(void);
EXTERN void keyboard_clearstate(void);
EXTERN void keyboard_translatekeys(int mask);
EXTERN int keyboard_keypressed(int scancode);

EXTERN int vga_setmode(int mode);
EXTERN int vga_hasmode(int mode);
EXTERN int vga_setflipchar(int c);
EXTERN int vga_clear(void);
EXTERN int vga_flip(void);
EXTERN int vga_getxdim(void);
EXTERN int vga_getydim(void);
EXTERN int vga_getcolors(void);
EXTERN int vga_setpalette(int index, int red, int green, int blue);
EXTERN int vga_getpalette(int index, int *red, int *green, int *blue);
EXTERN int vga_setpalvec(int start, int num, int *pal);
EXTERN int vga_getpalvec(int start, int num, int *pal);
EXTERN int vga_screenoff(void);
EXTERN int vga_screenon(void);
EXTERN int vga_setcolor(int color);
EXTERN int vga_drawpixel(int x, int y);
EXTERN int vga_drawline(int x1, int y1, int x2, int y2);
EXTERN int vga_drawscanline(int line, unsigned char *colors);
EXTERN int vga_drawscansegment(unsigned char *colors, int x, int y, int length);
EXTERN int vga_getpixel(int x, int y);      /* Added. */
EXTERN int vga_getscansegment(unsigned char *colors, int x, int y, int length);
EXTERN int vga_getch(void);
EXTERN int vga_dumpregs(void);
EXTERN int vga_init(void); 
EXTERN int vga_white(void);
EXTERN void vga_waitretrace(void);
EXTERN int vga_getdefaultmode(void);


#endif
