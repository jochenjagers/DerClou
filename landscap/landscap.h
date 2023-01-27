/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_LANDSCAP
#define MODULE_LANDSCAP

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error/error.h"
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#ifndef MODULE_INPHDL
#include "inphdl/inphdl.h"
#endif

#ifndef MODULE_GFX
#include "gfx/gfx.h"
#endif

#ifndef MODULE_BASE
#include "base/base.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "data/database.h"
#endif

#ifndef MODULE_TCDATA
#include "data/tcdata.h"
#endif

#ifndef MODULE_LIVING
#include "living/living.h"
#endif

#ifndef MODULE_SPOT
#include "landscap/spot.h"
#endif

#define   REL_CONSIST_OFFSET       3
#define   REL_HAS_LOCK_OFFSET      4
#define   REL_HAS_ALARM_OFFSET     5
#define   REL_HAS_POWER_OFFSET     6
#define   REL_HAS_LOOT_OFFSET      7
#define   REL_HAS_ROOM_OFFSET     10

#define   LS_OBJECT_VISIBLE   	((ubyte) 1)
#define   LS_OBJECT_INVISIBLE 	((ubyte) 0)


#define   LS_SCROLL_CHECK     	  0
#define   LS_SCROLL_PREPARE   	  1

/* Scroll defines */
#define   LS_SCROLL_LEFT				1
#define   LS_SCROLL_RIGHT		     	2
#define   LS_SCROLL_UP		   	  	4
#define   LS_SCROLL_DOWN		     	8

#define   LS_NO_COLLISION     		0
#define   LS_COLLISION        		1


#define   LS_MAX_AREA_WIDTH        640
#define   LS_MAX_AREA_HEIGHT       256  /* so that wall fits, also in the last line */

#define   LS_FLOOR_X_SIZE           32
#define   LS_FLOOR_Y_SIZE           32

#define   LS_STD_SCROLL_SPEED        2
#define   LS_STD_MAX_PERSONS         7

#define   LS_PERSON_X_SIZE          14
#define   LS_PERSON_Y_SIZE          14

#define   LS_PERSON_ACCESS_FOCUS_X   7
#define   LS_PERSON_ACCESS_FOCUS_Y   7

#define   LS_DARKNESS              ((ubyte)255)
#define   LS_BRIGHTNESS            ((ubyte)0)
#define   LS_DARK_FUNNY            ((ubyte)85)

#define	LS_PC_CORRECT_X				(-9)	// bei Vorzeichenwechsel landscap.c
#define  LS_PC_CORRECT_Y				(19)

/* Coll Mode */
#define   LS_COLL_PLAN                 1
#define   LS_COLL_NORMAL               2
#define   LS_LEVEL_DESIGNER            4

#define   lsGetOldState(lso)           ((((lso)->ul_Status) >> 16) & 0xffff)
#define   lsGetNewState(lso)           (((lso)->ul_Status) & 0xffff)
#define   lsSetOldState(lso)           ((lso)->ul_Status = (((lso)->ul_Status << 16) & 0xffff0000) + (((lso)->ul_Status) & 0xffff))

/* globale functions ! */
extern void  lsInitLandScape(uint32_t bID,ubyte mode);
extern void  lsDoneLandScape(void);

extern void  lsSetScrollSpeed(ubyte pixel);
extern void  lsSetActivLiving(char *Name,uword x,uword y);

extern void  lsRefreshObjectList(uint32_t areaID);

extern uint32_t lsAddLootBag(uword x, uword y, ubyte bagNr); /* bagNr : 1 - 8! */
extern void  lsRemLootBag(uint32_t bagId);
extern void  lsRefreshAllLootBags(void);

extern void  lsShowEscapeCar(void);

extern void  lsFastRefresh(LSObject lso);

extern void  lsSetObjectState(uint32_t objID, ubyte bitNr, ubyte value);

extern void  lsSetViewPort(uword x,uword y);

extern void  lsSetDarkness(ubyte value);
extern void  lsSetCollMode(ubyte collMode);

extern LIST *lsGetRoomsOfArea(uint32_t ul_AreaId);

extern void  lsSetObjectRetrievalList(uint32_t ul_AreaId);
extern uint32_t lsGetCurrObjectRetrieval(void);

extern uint32_t lsGetObjectState(uint32_t objID);
extern uword lsGetObjectCount(void);
extern ubyte lsGetLoudness(uword x, uword y);

extern void  lsDoScroll(void);
extern void  lsScrollCorrectData(int32_t dx,int32_t dy);
extern ubyte lsScrollLandScape(ubyte direction);
extern ubyte lsInitScrollLandScape(ubyte direction, ubyte mode);

extern LIST *lsGetObjectsByList(uword x,uword y,uword width,uword height, ubyte showInvisible, ubyte addLootBags);

extern void  lsInitActivArea(uint32_t areaID,uword x,uword y,char *livingName);
extern void  lsDoneActivArea(uint32_t newAreaID);
extern void  lsTurnObject(LSObject lso,ubyte status, ubyte Collis);

extern void  lsCalcExactSize(LSObject lso, uword *x0, uword *y0, uword *x1, uword *y1);

extern void  lsDoDoorRefresh(LSObject lso);

extern uword lsGetFloorIndex(uword x,uword y);

extern void  lsGuyInsideSpot(uword *us_XPos, uword *us_YPos, uint32_t *areaId);

extern int32_t  lsIsObjectAWall(LSObject lso);
extern int32_t  lsIsObjectAnAddOn(LSObject lso);
extern int32_t  lsIsObjectADoor(LSObject lso);
extern int32_t  lsIsObjectAStdObj(LSObject lso);
extern int32_t  lsIsObjectSpecial(LSObject lso);

extern int32_t  lsIsLSObjectInActivArea(LSObject lso);

extern ubyte lsIsCollision(uword x,uword y, ubyte direction);      /* sprite needs it */

extern uint32_t lsGetCurrBuildingID(void);
extern uint32_t lsGetActivAreaID(void);

extern void  lsInitObjectDB(uint32_t bld, uint32_t areaID);
extern void  lsLoadGlobalData(uint32_t bld, uint32_t ul_AreaId);
extern void  lsDoneObjectDB(uint32_t areaID);
extern void  lsInitRelations(uint32_t areaID);
extern void  lsSetRelations(uint32_t areaID);

extern void  lsBlitFloor(uword floorIndex, uword destx, uword desty);

extern void  lsWalkThroughWindow(LSObject lso, uword us_LivXPos, uword us_LivYPos, uword *us_XPos, uword *us_YPos);

/* internal functions ! */
extern void  lsPatchObjects(void);

extern void  lsSetVisibleWindow(uword x, uword y);

extern ubyte lsIsInside(LSObject lso,uword x,uword y,uword x1,uword y1);

extern uint32_t lsGetStartArea(void);

extern void  lsBuildScrollWindow(void);
extern void  lsShowAlarmAndPower(LSObject lso, uint32_t x, uint32_t y);

extern int32_t  lsShowOneObject(LSObject lso, word destx, word desty, uint32_t ObjTypeFilter);

/* external functions */
extern void  lsInitGfx(void);
extern void  lsCloseGfx(void);

extern uint32_t ConsistOfRelationID;
extern uint32_t hasLockRelationID;
extern uint32_t hasAlarmRelationID;
extern uint32_t hasPowerRelationID;
extern uint32_t hasLootRelationID;
extern uint32_t hasRoomRelationID;

/* COSP: hardware.c */
extern void lsRefreshStatue(LSObject lso);

#endif
