/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PLANING_SUPPORT
#define MODULE_PLANING_SUPPORT

#include <stdio.h>

#include "theclou.h"

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

#ifndef MODULE_ERROR
#include "error/error.h"
#endif

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_DATABASE
#include "data/database.h"
#endif

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#ifndef MODULE_TCDATA
#include "data/tcdata.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap/landscap.h"
#endif

#ifndef MODULE_PLANING_PLANER
#include "planing/planer.h"
#endif

// Area defines
#define PLANING_AREA_PERSON 3
#define PLANING_AREA_CAR 16

// functions for bits
#define CHECK_STATE(v, b) ((v) & (1L << (b)))
#define BIT(x) (1L << (x))

// Alarms & power control
#define PLANING_ALARM_Z3 3
#define PLANING_ALARM_X5 8
#define PLANING_ALARM_TOP3 15
#define PLANING_POWER 1

// all times in 1/3 sec
#define PLANING_CORRECT_TIME 3

uint32_t plGetNextLoot(void);

ubyte plLivingsPosAtCar(uint32_t bldId);
ubyte plAllInCar(uint32_t bldId);
ubyte plIsStair(uint32_t objId);

void plCorrectOpened(LSObject obj, ubyte open);
ubyte plIgnoreLock(uint32_t objId);

void plMove(uint32_t current, ubyte direction);
void plWork(uint32_t current);

LIST *plGetObjectsList(uint32_t current, ubyte addLootBags);
void plInsertGuard(LIST *list, uint32_t current, uint32_t guard);
ubyte plObjectInReach(uint32_t current, uint32_t objId);

#endif
