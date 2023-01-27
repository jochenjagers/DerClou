/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms which should be contained with this
  distribution.
 ****************************************************************************/
#ifndef TC_NEWSOUND_H
#define TC_NEWSOUND_H

#include "theclou.h"

extern int currMusicVolume;
extern int targetMusicVolume;

void sndDoFading(void);

void sndInit(void);
void sndDone(void);

void sndPlaySound(char *name, ulong mode);
void sndStopSound(ubyte dummy);
void sndFading(short int targetVol);

char *sndGetCurrSoundName(void);

#endif

