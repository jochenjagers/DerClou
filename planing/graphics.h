/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PLANING_GRAPHICS
#define MODULE_PLANING_GRAPHICS


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


// Messages modes
#define PLANING_MSG_REFRESH   1
#define PLANING_MSG_WAIT      2

// RefreshRP
#define PLANING_REFRESH_RP_WIDTH    200
#define PLANING_REFRESH_RP_HEIGHT   50
#define PLANING_REFRESH_RP_DEPTH    2

void plPrintInfo(char *person);
void plMessage(char *msg, ubyte flags);
void plPersonPict(ulong personId);
void plDisplayAbilities (void);
void plDisplayTimer(ulong time, ubyte doSpotsImmediatly);
void plDisplayInfo(void);
void plRefresh(ulong ItemId);
void plDrawWait(ulong sec);
ubyte plSay(char *msg, ulong persId);

#endif
