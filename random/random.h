/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_RANDOM
#define MODULE_RANDOM

#include "theclou.h"

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#ifndef __TIME_H
#include <time.h>
#endif

extern void rndInit(void);
extern void rndDone(void);

extern ulong CalcRandomNr(ulong l_limit, ulong u_limit);

#endif
