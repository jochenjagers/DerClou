/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_BOB
#define MODULE_BOB

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

extern void BobSet(uword bobID, uword xPos, uword yPos, uword xOffset, uword yOffset);
extern void BobVis(uword bobID);
extern void BobInitLists(void);
extern uword BobInit(void *pBuffer, uword w, uword h);
extern void BobDone(uword bobID);
extern void BobSetDarkness(ubyte b);
extern void BobInVis(uword bobID);

#endif
