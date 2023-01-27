/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_SOUND
#define MODULE_SOUND

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif


void sndInit(void);
void sndDone(void);

void sndPlaySound(char *name, ulong mode);
void sndStopSound(ubyte dummy);

void sndFadeOut(void);
void sndFading(short int targetVol);

char *sndGetCurrSoundName(void);

#endif
