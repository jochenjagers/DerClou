/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PLANING_PREPARE
#define MODULE_PLANING_PREPARE

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


// Gfx prepare modes
#define PLANING_GFX_LANDSCAPE  1L<<0
#define PLANING_GFX_SPRITES    1L<<1
#define PLANING_GFX_BACKGROUND 1L<<2

// Sys modes
#define PLANING_INIT_PERSONSLIST    1L<<0
#define PLANING_HANDLER_OPEN        1L<<1
#define PLANING_HANDLER_CLOSE       1L<<2
#define PLANING_HANDLER_CLEAR       1L<<3
#define PLANING_HANDLER_SET         1L<<4
#define PLANING_HANDLER_ADD         1L<<5
#define PLANING_GUARDS_LOAD         1L<<6

// Counts
#define PLANING_NR_PERSONS          4
#define PLANING_NR_GUARDS           4
#define PLANING_NR_LOOTS            8
#define PLANING_NR_PLANS            NRBLINES-1

// getting correct has loot relation
#define hasLoot(current)            (((LSArea)dbGetObject(livWhereIs(Planing_Name[current])))->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET)


extern LIST *PersonsList;
extern LIST *BurglarsList;

extern ubyte PersonsNr;
extern ubyte BurglarsNr;
extern ulong CurrentPerson;

extern ulong Planing_Weight[PLANING_NR_PERSONS];
extern ulong Planing_Volume[PLANING_NR_PERSONS];

extern ubyte Planing_Loot[PLANING_NR_LOOTS];
extern ubyte Planing_Guard[PLANING_NR_GUARDS];

extern char Planing_Name[PLANING_NR_PERSONS + PLANING_NR_GUARDS][20];

extern LIST *Planing_GuardRoomList[PLANING_NR_GUARDS];

extern ulong Planing_BldId;



void plBuildHandler(NODE *n);
void plClearHandler(NODE *n);
void plCloseHandler(NODE *n);

void plPrepareData(void);
void plPrepareSprite(ulong livNr, ulong areaId);
void plPrepareNames(void);

void plPrepareGfx(ulong objId, ubyte landscapMode, ubyte prepareMode);
void plPrepareRel(void);
void plPrepareSys(ulong currPer, ulong objId, ubyte sysMode);

void plUnprepareGfx(void);
void plUnprepareRel(void);
void plUnprepareSys(void);

#endif
