/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PLANING_SYNC
#define MODULE_PLANING_SYNC

#include <stdio.h>
#include "theclou.h"

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_DATABASE
#include "data\database.h"
#endif

#ifndef MODULE_RELATION
#include "data\relation.h"
#endif

#ifndef MODULE_TCDATA
#include "data\tcdata.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap\landscap.h"
#endif

#ifndef MODULE_PLANING_PLANER
#include "planing\planer.h"
#endif

// Sync modes
#define PLANING_ANIMATE_NO     (1L<<0)
#define PLANING_ANIMATE_STD    (1L<<1)
#define PLANING_ANIMATE_FOCUS  (1L<<2)


void plSync(ubyte animate, ulong targetTime, ulong times, ubyte direction);

#endif
