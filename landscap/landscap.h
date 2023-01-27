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
#include "error\error.h"
#endif

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#ifndef MODULE_INPHDL
#include "inphdl\inphdl.h"
#endif

#ifndef MODULE_GFX
#include "gfx\gfx.h"
#endif

#ifndef MODULE_BASE
#include "base\base.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_RELATION
#include "data\relation.h"
#endif

#ifndef MODULE_DATABASE
#include "data\database.h"
#endif

#ifndef MODULE_TCDATA
#include "data\tcdata.h"
#endif

#ifndef MODULE_LIVING
#include "living\living.h"
#endif

#ifndef MODULE_SPOT
#include "landscap\spot.h"
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
extern void  lsInitLandScape(ulong bID,ubyte mode);
extern void  lsDoneLandScape(void);

extern void  lsSetScrollSpeed(ubyte pixel);
extern void  lsSetActivLiving(char *Name,uword x,uword y);

extern void  lsRefreshObjectList(ulong areaID);

extern ulong lsAddLootBag(uword x, uword y, ubyte bagNr); /* bagNr : 1 - 8! */
extern void  lsRemLootBag(ulong bagId);
extern void  lsRefreshAllLootBags(void);

extern void  lsShowEscapeCar(void);

extern void  lsFastRefresh(LSObject lso);

extern void  lsSetObjectState(ulong objID, ubyte bitNr, ubyte value);

extern void  lsSetViewPort(uword x,uword y);

extern void  lsSetDarkness(ubyte value);
extern void  lsSetCollMode(ubyte collMode);

extern LIST *lsGetRoomsOfArea(ulong ul_AreaId);

extern void  lsSetObjectRetrievalList(ulong ul_AreaId);
extern ulong lsGetCurrObjectRetrieval(void);

extern ulong lsGetObjectState(ulong objID);
extern uword lsGetObjectCount(void);
extern ubyte lsGetLoudness(uword x, uword y);

extern void  lsDoScroll(void);
extern void  lsScrollCorrectData(long dx,long dy);
extern ubyte lsScrollLandScape(ubyte direction);
extern ubyte lsInitScrollLandScape(ubyte direction, ubyte mode);

extern LIST *lsGetObjectsByList(uword x,uword y,uword width,uword height, ubyte showInvisible, ubyte addLootBags);

extern void  lsInitActivArea(ulong areaID,uword x,uword y,char *livingName);
extern void  lsDoneActivArea(ulong newAreaID);
extern void  lsTurnObject(LSObject lso,ubyte status, ubyte Collis);

extern void  lsCalcExactSize(LSObject lso, uword *x0, uword *y0, uword *x1, uword *y1);

extern void  lsDoDoorRefresh(LSObject lso);

extern uword lsGetFloorIndex(uword x,uword y);

extern void  lsGuyInsideSpot(uword *us_XPos, uword *us_YPos, ulong *areaId);

extern long  lsIsObjectAWall(LSObject lso);
extern long  lsIsObjectAnAddOn(LSObject lso);
extern long  lsIsObjectADoor(LSObject lso);
extern long  lsIsObjectAStdObj(LSObject lso);
extern long  lsIsObjectSpecial(LSObject lso);

extern long  lsIsLSObjectInActivArea(LSObject lso);

extern ubyte lsIsCollision(uword x,uword y, ubyte direction);      /* sprite needs it */

extern ulong lsGetCurrBuildingID(void);
extern ulong lsGetActivAreaID(void);

extern void  lsInitObjectDB(ulong bld, ulong areaID);
extern void  lsLoadGlobalData(ulong bld, ulong ul_AreaId);
extern void  lsDoneObjectDB(ulong areaID);
extern void  lsInitRelations(ulong areaID);
extern void  lsSetRelations(ulong areaID);

extern void  lsBlitFloor(uword floorIndex, uword destx, uword desty);

extern void  lsWalkThroughWindow(LSObject lso, uword us_LivXPos, uword us_LivYPos, uword *us_XPos, uword *us_YPos);

/* internal functions ! */
extern void  lsPatchObjects(void);

extern void  lsSetVisibleWindow(uword x, uword y);

extern ubyte lsIsInside(LSObject lso,uword x,uword y,uword x1,uword y1);

extern ulong lsGetStartArea(void);

extern void  lsBuildScrollWindow(void);
extern void  lsShowAlarmAndPower(LSObject lso, ulong x, ulong y);

extern long  lsShowOneObject(LSObject lso, word destx, word desty, ulong ObjTypeFilter);

/* external functions */
extern void  lsInitGfx(void);
extern void  lsCloseGfx(void);

extern ulong ConsistOfRelationID;
extern ulong hasLockRelationID;
extern ulong hasAlarmRelationID;
extern ulong hasPowerRelationID;
extern ulong hasLootRelationID;
extern ulong hasRoomRelationID;

/* COSP: hardware.c */
extern void lsRefreshStatue(LSObject lso);

#endif
