/*=@Header==============================================================
 * $Source$
 *
 * @Desc: miscellaeous helpful functions for paraplus
 *	 
 * 	
 * $Revision$
 * $State$
 *
 * $Author$
 *
 *
 *-@Header------------------------------------------------------------*/

// static const char RCSid[]=\
// "$Id$";

#define _keyboard_c

#undef DIAGONAL_KEYS_AUS

#include <stdlib.h>
#include <stdio.h>
#include <vga.h>
#include <vgagl.h>
#include <vgakeyboard.h>

#include "defs.h"
#include "struct.h"
#include "global.h"
#include "proto.h"

// Die Definition f"ur eine Message kann ruhig lokal hier stehen, da sie
// nur innerhalb des Moduls gebraucht wird.


int LeftPressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_CURSORLEFT)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int LeftPressed(void)

int RightPressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_CURSORRIGHT)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int RightPressed(void)

int UpPressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_CURSORUP)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int UpPressed(void)

int DownPressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_CURSORDOWN)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int DownPressed(void)

int SpacePressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_SPACE)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int SpacePressed(void)

int CPressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_C)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int CPressed(void)

int PPressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_P)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int PPressed(void)

int QPressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_Q)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int QPressed(void)

int WPressed(void){
  keyboard_update();
  if(keyboard_keypressed(SCANCODE_W)) {
    return(TRUE);
  } else {
    return(FALSE);
  }
} // int WPressed(void)

/*@Function============================================================
  @Desc: Diese Funktion ermittelt, ob irgend eine Richtungstaste gedrueckt ist
  
  @Ret: wenn eine Richtungstaste gedrueckt ist FALSE
  ansonsten TRUE 
* $Function----------------------------------------------------------*/

int NoDirectionPressed(void){
  if (DownPressed()) return (0);
  if (UpPressed()) return (0);
  if (LeftPressed()) return (0);
  if (RightPressed()) return (0);
  return (1);
}  // int NoDirectionPressed(void)

/* ************************************************************** *
 * * Diese Funktion setzt die Zeichenwiederholrate der Tastatur * *
 * * Es wird zuerst das Befehlswort 0x0F3 gesendet und dann die * *
 * * neue Rate (0,1 delay 2,3,4,5,6 typematic) nach 0x60 gesandt. *
 * ************************************************************** */

int SetTypematicRate(unsigned char Rate){
  //	outportb(0x60,0x0f3);
  //	delay(1); 
  //	outportb(0x60,Rate);
  //	delay(1);
	return 0;
}

/*@Function============================================================
@Desc: 

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void JoystickControl(void){
	int JoyB, JoyX, JoyY;

	//	if (!GameAdapterPresent)
	//		return;
	//		
	//	asm{
	//		mov ah,84h
	//		mov dx,0
	//		int 15h
	//		jc NoJoy
	//		and al,00110000b
	//		mov cl,4
	//		shr al,cl
	//		xor al,00000011b
	//		mov ah,0
	//		mov JoyB,ax
	//	
	//		mov ah,84h
	//		mov dx,1
	//		int 15h
	//		jc NoJoy
	//		mov JoyX,ax
	//		mov JoyY,bx
	//		jmp AllOK
	//	}
	//NoJoy:
	//	gotoxy(4,4);
	//	printf(" No Joystick - kein Spieleadapter angeschlossen \n");
	//	getchar();
	//	GameAdapterPresent=FALSE;
	//
	//AllOK:
	//#ifdef SAYJOYPOS
	//	gotoxy(3,3);
	//	printf(" JoyX: %d JoyY: %d JoyB: %d !\n",JoyX,JoyY,JoyB);
	//#endif
	//
	//
	//	if ((SpacePressed) && (JoyB == FALSE)) SpaceReleased=TRUE;
	//	SpacePressed=JoyB;
	//	if (JoyX < 50) LeftPressed=TRUE; else LeftPressed=FALSE;
	//	if (JoyX >200) RightPressed=TRUE; else RightPressed=FALSE;
	//	if (JoyY < 50) UpPressed=TRUE; else UpPressed=FALSE;
	//	if (JoyY >200) DownPressed=TRUE; else DownPressed=FALSE;
	//
} // void JoystickControl(void)

/*@Function============================================================
@Desc: Diese Funktion wird bei jedem Tastendruck durch den Interrupt-
	handler 09h aufgerufen.	Die Installstion erfolgt durch die Funktion
	SetIntVect(void) am Beginn des Programms.	Das Ausklinken aus dem
	Interrupt erfolgt am Ende des Programms durch die Funktion
	RestoreIntVects(void). Das Schluesselwort interrupt bei der Deklaration
	sowohl im file "proto.h" als auch hier ist notwendig, um den Compiler
	anzuweisen, den Inhalt aller Register zu sichern, da diese durch einen
	Interrupt auf keinen Fall veraendert werden duerfen.

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
// void interrupt Interrupt09(void){

//	if( GameAdapterPresent ) return;
	
  //	KeyCode=inportb(0x60);
  //	switch (KeyCode){
  //		case 0x48 : UpPressed=1; break;
  //		case 0x50 : DownPressed=1; break;
  //		case 0x4B : LeftPressed=1; break;
  //		case 0x4D : RightPressed=1; break;
  //
  //#ifndef DIAGONAL_KEYS_AUS
  //		case 0x47 : LeftPressed=1; UpPressed=1; break;
  //		case 0x49 : RightPressed=1; UpPressed=1; break;
  //		case 0x51 : RightPressed=1; DownPressed=1; break;
  //		case 0x4F : LeftPressed=1; DownPressed=1; break;
  //
  //		case 0xC7 : LeftPressed=0; UpPressed=0; break;
  //		case 0xC9 : RightPressed=0; UpPressed=0; break;
  //		case 0xD1 : RightPressed=0; DownPressed=0; break;
  //		case 0xCF : LeftPressed=0; DownPressed=0; break;
  //#endif
  //		
  //		case 0xC8 : UpPressed=0; break;
  //		case 0xD0 : DownPressed=0; break;
  //		case 0xCB : LeftPressed=0; break;
  //		case 0xCD : RightPressed=0; break;
  //		case 0x39 : SpacePressed=1; break;
  //		case 0xB9 : { if (SpacePressed == TRUE) SpaceReleased=TRUE;
  //						  SpacePressed=0; break; }
  //		case 0x10 : QPressed=1; break;
  //		case 0x90 : QPressed=0; break;
  //	} /* switch */
  //
  //
  //	OldInt09h();
// }

/*@Function============================================================
@Desc:  This function drains the keyboard buffer of characters to
			prevent nasty beeps when it is overloaded

@Ret: 
@Int:
* $Function----------------------------------------------------------*/
void KillTastaturPuffer(void){
  //PORT: nix tun hier!
}







#undef _keyboard_c

