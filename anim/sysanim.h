/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

/*
 * Aufbau der Anim.List Datei (Aufbau einer Zeile)
 *
 * 	PictureMode,PictsPerSecc,Pic1,AnimPic,PicAnzahl,Animphase
 *	Breite, Animphase Hoehe, Animphase offset, PlayMode
 *	XDest, YDest (als Offset zum 1. Bild)
 */

#ifndef MODULE_ANIM
#define MODULE_ANIM

#include "theclou.h"

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_GFX
#include "gfx\gfx.h"
#endif

#ifndef MODULE_RANDOM
#include "random\random.h"
#endif

/* global functions */
extern void	InitAnimHandler(void);
extern void	CloseAnimHandler(void);

extern void	PlayAnim(char *AnimID,word how_often,ulong mode);	/* -> docs vom 16.08.92 ! */
extern void	StopAnim(void);

extern void animator(void);

extern void SuspendAnim(void);
extern void ContinueAnim(void);

#endif

