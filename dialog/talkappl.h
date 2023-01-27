/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_TALKAPPL
#define MODULE_TALKAPPL

#include "theclou.h"

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_DIALOG
#include "dialog\dialog.h"
#endif

#ifndef MODULE_RELATION
#include "data\relation.h"
#endif

#ifndef MODULE_DATABASE
#include "data\database.h"
#endif

#ifndef MODULE_DATACALC
#include "data\datacalc.h"
#endif

#ifndef MODULE_DATAAPPL
#include "data\dataappl.h"
#endif

extern void tcJobOffer(Person p);
extern void tcMyJobAnswer(Person p);
extern void tcPrisonAnswer(Person p);
extern void tcAbilityAnswer(ulong personID);

#endif
