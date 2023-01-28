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
#include "list/list.h"
#endif

#ifndef MODULE_RANDOM
#include "random/random.h"
#endif

#define tcDerivation(min, max) (CalcRandomNr(min, max))

extern int32_t Round(int32_t v, int32_t p);
extern int32_t CalcValue(int32_t value, int32_t min, int32_t max, int32_t fact, int32_t perc);
extern int32_t ChangeAbs(int32_t item, int32_t value, int32_t min, int32_t max);
extern int32_t RoundF(float num);

#endif
