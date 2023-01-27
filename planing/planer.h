/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PLANING_PLANER
#define MODULE_PLANING_PLANER


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

#ifndef MODULE_PLANING_MAIN
#include "planing\main.h"
#endif

#ifndef MODULE_PLANING_GRAPHICS
#include "planing\graphics.h"
#endif

#ifndef MODULE_PLANING_IO
#include "planing\io.h"
#endif

#ifndef MODULE_PLANING_PREPARE
#include "planing\prepare.h"
#endif

#ifndef MODULE_PLANING_SUPPORT
#include "planing\support.h"
#endif

#ifndef MODULE_PLANING_SYSTEM
#include "planing\system.h"
#endif

#ifndef MODULE_PLANING_SYNC
#include "planing\sync.h"
#endif

#ifndef MODULE_PLANING_GUARDS
#include "planing\guards.h"
#endif

#ifndef MODULE_PLANING_PLAYER
#include "planing\player.h"
#endif

extern ubyte AnimCounter;
extern ubyte PlanChanged;


void plPlaner (ulong objId);

#endif
