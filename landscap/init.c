/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "landscap\landscap.h"
#include "landscap\landscap_p.h"

static void lsInitObjects(void);

/*------------------------------------------------------------------------------
 *   global functions for landscape
 *------------------------------------------------------------------------------*/

void lsInitLandScape(ulong bID,ubyte mode)   	/* initialisiert das Landschaftsmodul */
	{
	long i;

	if(!ls)
		ls = (struct LandScape*)MemAlloc(sizeof(struct LandScape));

	ls->ul_BuildingID = bID;

	ls->us_RasInfoScrollX = 0;
	ls->us_RasInfoScrollY = 0;

	lsInitGfx();   /* dont change location of this line */

	ls->uch_ScrollSpeed    = LS_STD_SCROLL_SPEED;
	ls->uch_ShowObjectMask = 0;

	lsSetCollMode(mode);

	lsInitObjects();

	lsInitSpots(l_wrp);

	lsLoadAllSpots();

	ls->uch_FloorsPerWindowColumn = (ubyte)(LS_FLOORS_PER_COLUMN);
	ls->uch_FloorsPerWindowLine   = (ubyte)(LS_FLOORS_PER_LINE);

	ls->ul_AreaID = lsGetStartArea(); /* !! MOD 04-01 - vor Sprites!!! */

	BobInitLists();

	ls->us_EscapeCarBobId = BobInit(LS_LOOTBAG_BUFFER, 14, 14);

	// lootbags mÅssen vor den Livings initialisiert werden, da sie
	// einer kleiner Prioritaet haben (unter den Maxis erscheinen)
	for (i = 9701; i <= 9708; i++)
		{
		LSObject lso = (LSObject)dbGetObject(i);

		// OffsetFact wird am PC nicht fÅr den Offset in der Plane
		// sondern als Handle des Bob verwendet
		lso->us_OffsetFact = BobInit(LS_LOOTBAG_BUFFER, 14, 14);

		hasLootBagUnSet(ls->ul_AreaID, (ulong)i);
		}

	livInit(0,0,LS_VISIBLE_X_SIZE,LS_VISIBLE_Y_SIZE,LS_MAX_AREA_WIDTH,LS_MAX_AREA_HEIGHT, 8, ls->ul_AreaID);

	ls->p_CurrFloor = NULL;

	ls->us_DoorXOffset = 0;
	ls->us_DoorYOffset = 32;	// oben sind noch die 16er Objekte

	ls->p_DoorRefreshList = (LIST*)CreateList(0);

	lsInitFloorSquares();

	lsInitActivArea(ls->ul_AreaID,(uword)-1,(uword)-1,NULL);
	lsShowEscapeCar();
	}

void lsInitActivArea(ulong areaID,uword x,uword y, char *livingName)
	{
	LSArea area = (LSArea) dbGetObject(areaID);

	ls->ul_AreaID = areaID;

	lsSetRelations(areaID);
	lsSetObjectRetrievalList(areaID);

	ls->us_WindowXSize  = area->us_Width;
	ls->us_WindowYSize  = area->us_Height;

	if(x==(uword)-1)    x = area->us_StartX0;
	if(y==(uword)-1)    y = area->us_StartY0;

	lsSetVisibleWindow(x,y);

	livSetActivAreaId(areaID);

	lsSetCurrFloorSquares(areaID);

	gfxCopyCollToXMS(area->us_Coll16ID, &LS_COLL16_XMS_RP);
	gfxCopyCollToXMS(area->us_Coll32ID, &LS_COLL32_XMS_RP);
	gfxCopyCollToXMS(area->us_Coll48ID, &LS_COLL48_XMS_RP);

	gfxCopyCollToXMS(area->us_FloorCollID, &LS_FLOOR_XMS_RP);

	lsBuildScrollWindow();
	lsRefreshAllLootBags();
	}

void lsInitRelations(ulong areaID)
	{
	LSArea area = (LSArea)dbGetObject(areaID);

	AddRelation(area->ul_ObjectBaseNr + REL_CONSIST_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_LOCK_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_ALARM_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_POWER_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET);
	AddRelation(area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET);
	}

void lsSetRelations(ulong areaID)
	{
	LSArea area = (LSArea)dbGetObject(areaID);

	ConsistOfRelationID = area->ul_ObjectBaseNr + REL_CONSIST_OFFSET;
	hasLockRelationID   = area->ul_ObjectBaseNr + REL_HAS_LOCK_OFFSET;
	hasAlarmRelationID  = area->ul_ObjectBaseNr + REL_HAS_ALARM_OFFSET;
	hasPowerRelationID  = area->ul_ObjectBaseNr + REL_HAS_POWER_OFFSET;
	hasLootRelationID   = area->ul_ObjectBaseNr + REL_HAS_LOOT_OFFSET;
	hasRoomRelationID   = area->ul_ObjectBaseNr + REL_HAS_ROOM_OFFSET;
	}

static void lsInitObjects(void)
	{
	ulong  areaCount = 0, i;
	LIST  *areas;
	NODE  *n;

	/* alle Relationen erzeugen */
	consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
	areas = ObjectListPrivate;

	/* jetzt alle Stockwerke durchgehen! */
	for  (i = 0; i < 3; i++)
		{
		ls->ul_ObjectRetrievalAreaId[i] = 0;
		ls->p_ObjectRetrievalLists[i] = NULL;
		}

	for (n = (NODE *)LIST_HEAD(areas); NODE_SUCC(n); n = (NODE *) NODE_SUCC(n))
		{
		lsInitRelations(OL_NR(n));

		/* Daten laden */
		lsInitObjectDB(ls->ul_BuildingID, OL_NR(n));

		lsSetRelations(OL_NR(n));
		lsRefreshObjectList(OL_NR(n));     /* ObjectRetrievalList erstellen */

		// es gibt einiges zu patchen!
		lsPatchObjects();

		ls->p_ObjectRetrievalLists[areaCount]   = ls->p_ObjectRetrieval; /* und merken */
		ls->ul_ObjectRetrievalAreaId[areaCount] = OL_NR(n);

		areaCount++;
		}

	lsLoadGlobalData(ls->ul_BuildingID, OL_NR(NODE_PRED(n)));

	RemoveList(areas);
	}

void lsLoadGlobalData(ulong bld, ulong ul_AreaId)
	{
	char areaName[TXT_KEY_LENGTH], fileName[TXT_KEY_LENGTH];

	dbGetObjectName(ul_AreaId, areaName);
	areaName[strlen(areaName)-1] = '\0';

	strcat(areaName, OBJ_GLOBAL_REL_EXTENSION);

	dskBuildPathName(DATA_DIRECTORY, areaName, fileName);



	if(!(LoadRelations(fileName, 0)))
		NewErrorMsg(Disk_Defect, __FILE__, __func__, 2);
	}

void lsInitObjectDB(ulong bld, ulong areaID)
	{
	char fileName[TXT_KEY_LENGTH],areaName[TXT_KEY_LENGTH];

	dbGetObjectName(areaID ,areaName);
	strcat(areaName,OBJ_DATA_EXTENSION);
	dskBuildPathName(DATA_DIRECTORY,areaName,fileName);


	dbSetLoadObjectsMode(DB_LOAD_MODE_NO_NAME);	// dont fetch names of objects

	if (dbLoadAllObjects(fileName, 0))
		{
		dbGetObjectName(areaID,areaName);
		strcat(areaName,OBJ_REL_EXTENSION);
		dskBuildPathName(DATA_DIRECTORY,areaName,fileName);


		if (!(LoadRelations(fileName, 0)))
			NewErrorMsg(Disk_Defect, __FILE__, __func__, 3);
		}
	else
		NewErrorMsg(Disk_Defect, __FILE__, __func__, 4);

	dbSetLoadObjectsMode(DB_LOAD_MODE_STD);
	}

// lÑdt im Gegensatz zur Amigaversion alle Bodendaten gleichzeitig
// in den Speicher
static void lsInitFloorSquares(void)
	{
	ulong count;
	long i;
	char fileName[TXT_KEY_LENGTH], areaName[TXT_KEY_LENGTH];
	NODE *n;
	LIST *areas;

	for  (i = 0; i < 3; i++)
		ls->p_AllFloors[i] = NULL;

	count = LS_FLOORS_PER_LINE * LS_FLOORS_PER_COLUMN;

	consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
	areas = ObjectListPrivate;

	/* jetzt alle Stockwerke durchgehen! */
	for (n = (NODE *)LIST_HEAD(areas), i = 0; NODE_SUCC(n); n = (NODE *) NODE_SUCC(n), i++)
		{
		ulong size = (ulong) count * (sizeof(struct LSFloorSquare));

		ls->p_AllFloors[i]    = (struct LSFloorSquare*)MemAlloc(size);
		ls->ul_FloorAreaId[i] = OL_NR(n);

		dbGetObjectName(ls->ul_FloorAreaId[i],areaName);
		strcat(areaName,FLOOR_DATA_EXTENSION);

		dskBuildPathName(DATA_DIRECTORY,areaName,fileName);


		dskLoad(fileName, ls->p_AllFloors[i], 0);
		}

	RemoveList(areas);
	}

static void lsLoadAllSpots(void)
	{
	char fileName[TXT_KEY_LENGTH];
	LIST  *areas;
	NODE  *n;

	consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST|OLF_INCLUDE_NAME, Object_LSArea);
	areas = ObjectListPrivate;

	n = (NODE *)LIST_HEAD(areas);

	strcpy(fileName, NODE_NAME(n));

	fileName[strlen(fileName) - 1] = '\0';

	strcat(fileName, SPOT_DATA_EXTENSION);

	lsLoadSpots(ls->ul_BuildingID, fileName);

	RemoveList(areas);
	}

static void lsSetCurrFloorSquares(ulong areaId)
	{
	long i;

	for (i = 0; i < 3; i++)
		if (areaId == ls->ul_FloorAreaId[i])
			ls->p_CurrFloor = ls->p_AllFloors[i];
	}



/*------------------------------------------------------------------------------
 *   done functions for landscape
 *------------------------------------------------------------------------------*/

// gibt alle Bodendaten wieder frei
static void lsDoneFloorSquares(void)
	{
	ulong count, i, size;

	for (i = 0; i < 3; i++)
		{
		if (ls->p_AllFloors[i])
			{
			count = LS_FLOORS_PER_LINE * LS_FLOORS_PER_COLUMN;

			size = (ulong) count * (sizeof(struct LSFloorSquare));

			MemFree(ls->p_AllFloors[i], size);

			ls->p_AllFloors[i] = NULL;
			}
		}

	}

void lsDoneObjectDB(ulong areaID)
	{
	LSArea area = (LSArea)dbGetObject(areaID);

	RemRelations(area->ul_ObjectBaseNr, DB_tcBuild_SIZE);
	dbDeleteAllObjects(area->ul_ObjectBaseNr, DB_tcBuild_SIZE);

	/* globale Relation wieder lîschen */
	RemRelations(Relation_hasClock - 1, 3);

	/* und die "Relationsheader" wieder anlegen */
	AddRelation(Relation_hasClock);
	AddRelation(Relation_ClockTimer);
	AddRelation(Relation_StairConnects);
	}

void lsDoneLandScape(void)
	{
	NODE *n;
	long areaCount = 0, i;

	if(ls)
		{
		LIST *areas;

		consistsOfAll(ls->ul_BuildingID, OLF_PRIVATE_LIST, Object_LSArea);
		areas = ObjectListPrivate;

		for (n = (NODE *)LIST_HEAD(areas); NODE_SUCC(n); n = (NODE *) NODE_SUCC(n), areaCount++)
			{
			lsDoneObjectDB(OL_NR(n));

			if(ls->p_ObjectRetrievalLists[areaCount])
				{
				RemoveList(ls->p_ObjectRetrievalLists[areaCount]);
				ls->p_ObjectRetrievalLists[areaCount] = NULL;
				ls->ul_ObjectRetrievalAreaId[areaCount] = 0;
				}
			}

		RemoveList(areas);

		lsDoneActivArea(0L);

		livDone();

		for (i = 9701; i <= 9708; i++)
			{
			LSObject lso = (LSObject)dbGetObject(i);

			BobDone(lso->us_OffsetFact);
			}

		BobDone(ls->us_EscapeCarBobId);

		lsDoneSpots();

		lsCloseGfx();

		lsDoneFloorSquares();

		if (ls->p_DoorRefreshList)
			RemoveList(ls->p_DoorRefreshList);

		MemFree(ls, sizeof(struct LandScape));
		ls=NULL;
		}
	}

void lsDoneActivArea(ulong newAreaID)
	{
	livSetAllInvisible();                    /* MOD 14-01-94 */
	}
