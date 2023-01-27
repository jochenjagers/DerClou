/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_DATACALC
#define MODULE_DATACALC

#include "theclou.h"

#ifndef __STDARG_H
#include <stdarg.h>
#endif

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_RANDOM
#include "random\random.h"
#endif

#define   tcDerivation(min,max)          (CalcRandomNr(min,max))

extern long Round(long v,long p);
extern long CalcValue(long value, long min, long max, long fact, long perc);
extern long ChangeAbs(long item, long value, long min, long max);
extern long RoundF(float num);

#endif
