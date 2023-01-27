/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_FX
#define MODULE_FX

#include "theclou.h"

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#ifndef MODULE_BASE
#include "base\base.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

struct FXBase
{
	void *	p_SoundBuffer;
	ubyte *	p_Data;
	ulong	ul_DataSize;
	ulong	ul_DataPlayed;
	ulong	ul_MaxSize;
	uword	us_SoundBlasterOk;
};

extern struct FXBase FXBase;

extern void sndInitFX(void);
extern void sndDoneFX(void);
extern void sndPrepareFX(ubyte *puch_Name);
extern void sndPlayFX(void);

extern int InitSBlaster(void);
extern void RemoveSBlast(void);

#endif
