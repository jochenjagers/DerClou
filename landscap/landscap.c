/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "landscap\landscap.h"
#include "landscap\landscap_p.h"

static void lsInitDoorRefresh(ulong ObjId);

ulong     ConsistOfRelationID = 0;
ulong     hasLockRelationID   = 0;
ulong     hasAlarmRelationID  = 0;
ulong     hasPowerRelationID  = 0;
ulong     hasLootRelationID   = 0;
ulong     hasRoomRelationID   = 0;

struct    LandScape *ls = NULL;

static void lsSafeRectFill(word x, word y, word x1, word y1)
	{
	if (x  <   0)     x  =   0;
	if (x  > 638)     x  = 638;

	if (x1 <   0)     x1 =   0;
	if (x1 > 639)     x1 = 639;

	if (y  <   0)     y  =   0;
	if (y  > 254)     y  = 254;

	if (y1 <   0)     y1 =   0;
	if (y1 > 255)     y1 = 255;

	if ((x1 > x) && (y1 > y))
		gfxNCH4RectFill(l_wrp, x, y, x1, y1);
	}

void lsSetVisibleWindow(uword x, uword y)
	{
	word halfX, halfY, wX, wY;

	halfX = LS_VISIBLE_X_SIZE / 2;
	halfY = LS_VISIBLE_Y_SIZE / 2;

	if ((wX = x - halfX) < 0)
		wX = 0;

	if ((wY = y - halfY) < 0)
		wY = 0;

	/* -1 is important, so that no position 320, 128 is possible     */
	/* (window from (320, 128) - (640, 256) instead of (639, 255) !!!*/
	wX = min(wX, LS_MAX_AREA_WIDTH  - LS_VISIBLE_X_SIZE - 1);
	wY = min(wY, LS_MAX_AREA_HEIGHT - LS_VISIBLE_Y_SIZE - 1);

	ls->us_WindowXPos = (uword) wX;
	ls->us_WindowYPos = (uword) wY;

	gfxNCH4SetViewPort(ls->us_WindowXPos,ls->us_WindowYPos);

	livSetVisLScape(ls->us_WindowXPos,ls->us_WindowYPos);
	}

static void lsShowRooms(void)
{
	if (GamePlayMode & GP_SHOW_ROOMS)
	{
	LIST *rooms = lsGetRoomsOfArea(ls->ul_AreaID);
	NODE *room;

	for (room = LIST_HEAD(rooms); NODE_SUCC(room); room = NODE_SUCC(room))
		{
		LSRoom myroom = (LSRoom)OL_DATA(room);

		gfxSetPens(l_wrp, 249, 0, 249);

		gfxRectFill(l_wrp, myroom->us_LeftEdge, myroom->us_TopEdge,
						myroom->us_LeftEdge + myroom->us_Width - 1,
						myroom->us_TopEdge + myroom->us_Height - 1);
		}

	RemoveList(rooms);
	}
}

void lsBuildScrollWindow(void)
{
	long i, j;
	NODE *node;
	LSArea area = (LSArea) dbGetObject(ls->ul_AreaID);
	ubyte colortable[768];
	LSObject lso;

	gfxSetColorRange(0, 127);
	gfxChangeColors(NULL, 0, GFX_FADE_OUT, 0);

	/* Boden aufbauen */
	for(i = 0;i < LS_FLOORS_PER_COLUMN; i++)
		{
		for(j = 0; j < LS_FLOORS_PER_LINE; j++)
			{
			//falls kein Boden vorhanden ist, mit Kollisionsfarbe fllen!

			if (LS_NO_FLOOR((ls->p_CurrFloor[i * LS_FLOORS_PER_LINE + j].uch_FloorType)))
				{
				gfxSetPens(l_wrp, LS_COLLIS_COLOR_2, GFX_SAME_PEN, LS_COLLIS_COLOR_2);
				lsSafeRectFill(j * 32, i * 32, j * 32 + 31, i * 32 + 31);
				}
			else
				lsBlitFloor((i * LS_FLOORS_PER_LINE + j), j * 32, i * 32);
			}
		}

	/* Objekte setzen - zuerst W„nde*/
	for (node = (NODE*)LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node); node = (NODE*) NODE_SUCC(node))
		{
		lso = (LSObject)OL_DATA(node);

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_WALL|LS_SHOW_PREPARE_FROM_XMS))
			lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);

		}

	/* dann andere */
	for (node = (NODE*)LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node); node = (NODE*)NODE_SUCC(node))
		{
		lso = (LSObject)OL_DATA(node);

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_OTHER_0|LS_SHOW_PREPARE_FROM_XMS))
			lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);
		}

	/* jetzt noch ein paar Sondef„lle (Kassa, Vase, ...) */
	for (node = (NODE*)LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node); node = (NODE*)NODE_SUCC(node))
		{
		lso = (LSObject)OL_DATA(node);

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_OTHER_1|LS_SHOW_PREPARE_FROM_XMS))
			lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);

		// Statue mssen wegen der unsauberen Scheiáe, speziell refresht werden
		// wrg!!!
		if (lso->Type == Item_Statue)
			if (lso->uch_Visible != LS_OBJECT_VISIBLE)
				lsRefreshStatue(lso);
		}

	// jetzt alle fr alle Tren & Special objects Refresh erzeugen
	for (node = (NODE*)LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node); node = (NODE*) NODE_SUCC(node))
		{
		lso = (LSObject)OL_DATA(node);

		if (lsIsObjectADoor(lso))
			lsInitDoorRefresh(OL_NR(node));
		else if (lsIsObjectSpecial(lso))
			lsInitDoorRefresh(OL_NR(node));
		}

	// zuletzt mssen am PC die Tren & Specials aufgebaut werden
	for (node = (NODE*)LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node); node = (NODE*)NODE_SUCC(node))
		{
		lso = (LSObject)OL_DATA(node);

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_SPECIAL|LS_SHOW_PREPARE_FROM_XMS))
			lsTurnObject(lso, lso->uch_Visible, LS_COLLISION);
		}

	for (node = (NODE*)LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(node); node = (NODE*) NODE_SUCC(node))
		{
		lso = (LSObject)OL_DATA(node);

		if (lsShowOneObject(lso, LS_STD_COORDS, LS_STD_COORDS, LS_SHOW_DOOR|LS_SHOW_PREPARE_FROM_XMS))
			{
			if (!(lso->ul_Status & 1<<Const_tcOPEN_CLOSE_BIT))
				lsTurnObject((LSObject)OL_DATA(node), lso->uch_Visible, LS_COLLISION);
			else
				lsTurnObject((LSObject)OL_DATA(node), lso->uch_Visible, LS_NO_COLLISION);
			}
		}

	/* und weiter gehts mit der Musi... */
	BobSetDarkness(area->uch_Darkness);
	lsSetDarkness(area->uch_Darkness);

	lsShowAllSpots(0, LS_ALL_VISIBLE_SPOTS);

	/* auf gehts Burschn, schp ma wos flottes */
	/* 32er Collection einblenden */
	gfxPrepareColl(area->us_Coll32ID);
	gfxGetColorTable(area->us_Coll32ID, colortable);
	gfxSetColorRange(0, 127);

	// Hintergrund nur kopieren !
	for (i = 64 * 3; i < 65 * 3; i++)
	{
		colortable[i] = colortable[i - 64 * 3];
	}
	// andere Farben berechnen (verdunkeln! )
	for (i = 65 * 3; i < 128 * 3; i++)
	{
		colortable[i]          = colortable[i - 64 * 3];	// hell
		colortable[i - 64 * 3] >>= 1;						// dunkel
	}

	gfxChangeColors(NULL, 5, GFX_BLEND_UP, colortable);

	/* und jetzt die Farben der Maxis einblenden */
	gfxPrepareColl(137);
	gfxGetColorTable(137, colortable);
	gfxSetColorRange(128, 191);
	gfxChangeColors(NULL, 0, GFX_BLEND_UP, colortable);

	/* und zur Sicherheit auch noch Menufarben setzen */
	tcSetPermanentColors();	// mod 31-06-94 nach diversesn Beschwerden (hg)

	lsShowRooms();
}

void lsSetDarkness(ubyte value) // entweder
{
	gfxSetDarkness(value);
}

void lsTurnObject(LSObject lso, ubyte status, ubyte Collis)
	{
	uword floorIndex;

	floorIndex=lsGetFloorIndex(lso->us_DestX,lso->us_DestY);

	lso->uch_Visible = status;

	if (status == LS_OBJECT_VISIBLE)
		LS_SET_OBJECT(ls->p_CurrFloor[floorIndex].uch_FloorType);

	if (lso->Type == Item_Mikrophon)
		LS_SET_MICRO_ON_FLOOR(ls->p_CurrFloor[floorIndex].uch_FloorType);
	}

ubyte lsIsInside(LSObject lso,uword x,uword y,uword x1,uword y1)
	{
	uword lsoX, lsoY, lsoX1, lsoY1;

	lsCalcExactSize(lso, &lsoX,&lsoY,&lsoX1,&lsoY1);

	if((lsoX <= x1) && (lsoX1 >= x) &&
		(lsoY <= y1) && (lsoY1 >= y))
		return(1);
	else
		return(0);
	}

void lsSetActivLiving(char *Name,uword x,uword y)
	{
	if(Name)
		{
		strcpy((char *)ls->uch_ActivLiving, Name);

		if(x == (uword) -1)    x = livGetXPos(Name);
		if(y == (uword) -1)    y = livGetYPos(Name);

		lsSetVisibleWindow (x, y);
		livRefreshAll();

		ls->us_PersonXPos = (uword) (x - ls->us_WindowXPos);
		ls->us_PersonYPos = (uword) (y - ls->us_WindowYPos);
		}
	}

void lsSetObjectState(ulong objID, ubyte bitNr, ubyte value)
	{
	LSObject object = (LSObject) dbGetObject(objID);

	// bei einer Stechuhr darf sich der Status nicht „ndern!
	if (object->Type != Item_Stechuhr)
		{
		if (value == 0)
			object->ul_Status &= (0xffffffff - (1L<<bitNr));

		if(value == 1)
			object->ul_Status |= (1L<<bitNr);
		}
	}

word lsSortByXCoord(struct ObjectNode *n1, struct ObjectNode *n2)
	{
	LSObject lso1, lso2;

	lso1 = (LSObject)OL_DATA(n1);
	lso2 = (LSObject)OL_DATA(n2);

	if (lso1->us_DestX > lso2->us_DestX)
		return(1);
	else
		return((word)-1);
	}

word lsSortByYCoord(struct ObjectNode *n1, struct ObjectNode *n2)
	{
	LSObject lso1, lso2;

	lso1 = (LSObject)OL_DATA(n1);
	lso2 = (LSObject)OL_DATA(n2);

	if(lso1->us_DestY  < lso2->us_DestY)
		return(1);
	else
		return((word)-1);
	}

static void lsSortObjectList(LIST **l)
	{
	LSObject lso1, lso2;
	NODE *node, *node1, *next;
	ubyte lastNode = 0;

	if (!(LIST_EMPTY((*l))))
		{
		dbSortObjectList(l, lsSortByYCoord);

		for (node = (NODE*) LIST_HEAD((*l)); (!(lastNode)) && NODE_SUCC(NODE_SUCC(node));)
			{
			node1 = node;

			do
				{
				node1 = (NODE*) NODE_SUCC(node1);
				lso1  = (LSObject)OL_DATA(node);
				lso2  = (LSObject)OL_DATA(node1);
				}
				while((lso1->us_DestY == lso2->us_DestY) && NODE_SUCC(NODE_SUCC(node1)));

			next = node1;

			/* wenn Abbruch wegen NODE_SUCC(NODE_SUCC(.. erflogte, darf
			 * nicht der NODE_PRED(node1) genomen werden!
			 * nach dem Sortieren ist auáerdem Schluá!
			 */

			if ((lso1->us_DestY != lso2->us_DestY))
				node1 = (NODE*) NODE_PRED(node1);
			else
				lastNode = 1;

			if (node != node1)
				{
				dbSortPartOfList ((*l), (struct ObjectNode *)node,
												(struct ObjectNode *)node1, lsSortByXCoord);
				node = next;
				}
			else
				node = (NODE*) NODE_SUCC(node);
			}
		}
	}

void lsRefreshObjectList(ulong areaID)
	{
	SetObjectListAttr (OLF_PRIVATE_LIST, Object_LSObject);
	AskAll (dbGetObject(areaID), ConsistOfRelationID, BuildObjectList);
	ls->p_ObjectRetrieval = ObjectListPrivate;

	lsSortObjectList(&ls->p_ObjectRetrieval);
	}

ulong lsAddLootBag(uword x, uword y, ubyte bagNr) /* bagNr : 1 - 8! */
	{
	LSObject lso = (LSObject)dbGetObject (9700 + bagNr);

	lso->uch_Visible = LS_OBJECT_VISIBLE;

	/* add loot bag */
	if (!hasLootBag(ls->ul_AreaID, (ulong) (9700 + bagNr)))
		{
		lso->us_DestX = x;
		lso->us_DestY = y;

		BobSet(lso->us_OffsetFact, x, y, LS_LOOTBAG_X_OFFSET, LS_LOOTBAG_Y_OFFSET);
		BobVis(lso->us_OffsetFact);

		hasLootBagSet(ls->ul_AreaID, (ulong)(9700 + bagNr));
		}

	return ((ulong)(9700 + bagNr));
	}

void lsRemLootBag(ulong bagId)
	{
	LSObject lso = (LSObject)dbGetObject(bagId);

	lso->uch_Visible = LS_OBJECT_INVISIBLE;

	BobInVis(lso->us_OffsetFact);

	hasLootBagUnSet(ls->ul_AreaID, bagId);
	}

void lsRefreshAllLootBags(void)
	{
	ulong i;
	LSObject lso;

	livPrepareAnims();

	for (i = 1; i < 9; i++)
		{
		lso = (LSObject)dbGetObject(9700 + i);

		if ((lso->uch_Visible == LS_OBJECT_VISIBLE) && (hasLootBag(ls->ul_AreaID, (ulong) (9700 + i))))
			{
			BobSet(lso->us_OffsetFact, lso->us_DestX, lso->us_DestY, LS_LOOTBAG_X_OFFSET, LS_LOOTBAG_Y_OFFSET);
			BobVis(lso->us_OffsetFact);
			}
		}
	}

void lsGuyInsideSpot(uword *us_XPos, uword *us_YPos, ulong *areaId)
	{
	LIST *spots = lsGetSpotList();
	struct Spot *s;
	long i;

	for (s = (struct Spot *) LIST_HEAD(spots); NODE_SUCC(s); s = (struct Spot *) NODE_SUCC(s))
		{
		if (s->uch_Status & LS_SPOT_ON)
			{
			for (i = 0; i < 4; i++)
				{
				if ((us_XPos[i] != (uword)-1) && (us_YPos[i] != (uword)-1))
					{
					if (areaId[i] == s->ul_AreaId)
						{
						if (((struct Spot*)s)->p_CurrPos)
							{
							long x    = s->p_CurrPos->us_XPos; /* linke, obere */
							long y    = s->p_CurrPos->us_YPos; /* Ecke des Spot! */
							long size = s->us_Size - 4;

							if ((us_XPos[i] < x + size - 2) && (us_XPos[i] > x + 2))
								if ((us_YPos[i] < y + size - 2) && (us_YPos[i] > y + 2))
									Search.SpotTouchCount[i]++;
							}
						}
					}
				}
			}
		}
	}

void lsWalkThroughWindow(LSObject lso, uword us_LivXPos, uword us_LivYPos, uword *us_XPos, uword *us_YPos)
	{
	word deltaX, deltaY;

	(*us_XPos) = us_LivXPos;
	(*us_YPos) = us_LivYPos;

	if (us_LivXPos < lso->us_DestX)		deltaX =  25;
	else                          		deltaX = -25;

	if (us_LivYPos < lso->us_DestY)		deltaY =  25;
	else                          		deltaY = -25;

	if (lso->ul_Status & (1<<Const_tcHORIZ_VERT_BIT))
		(*us_XPos) += deltaX;	/* vertikal */
	else
		(*us_YPos) += deltaY;	/* horizontal */
	}

/* for some objects the LDesigner does not set the status bits correctly,
   so they need to patch at runtime here*/
void lsPatchObjects(void)
{
	NODE *n;
	LSObject lso;
	Item item;

	((Item)dbGetObject(Item_Fenster))->OffsetFact = 16;

	for (n = (NODE*) LIST_HEAD(ls->p_ObjectRetrieval); NODE_SUCC(n); n = (NODE*) NODE_SUCC(n))
	{
		lso = (LSObject)OL_DATA(n);

		item = (Item)dbGetObject(lso->Type);

		lso->uch_Size = item->Size;	// Größen stimmen nicht überall überein!

		switch(lso->Type)
		{
		case Item_Statue:
			lso->ul_Status |= (1L<<Const_tcACCESS_BIT);
			break;
		case Item_WC:
		case Item_Kuehlschrank:
		case Item_Nachtkaestchen:
			lso->ul_Status |= (1L<<Const_tcLOCK_UNLOCK_BIT);  /* unlocked! */
			break;
		case Item_Steinmauer:
		case Item_Tresen:
		case Item_Vase:
		case Item_Sockel:
		case Item_Leiter:
			lso->ul_Status &= (~(1L<<Const_tcACCESS_BIT));      /* stone wall and so on, no Access */
			break;
		default:
			break;
		}

		if (bProfidisk)
		{
			switch (lso->Type)
			{
			case Item_Beichtstuhl:
			case Item_Postsack:
				lso->ul_Status |= (1L<<Const_tcLOCK_UNLOCK_BIT);  /* unlocked! */
				break;
			case Item_Leiter:
				lso->ul_Status &= (~(1L<<Const_tcACCESS_BIT));      /* ladder, no Access */
				break;
			default:
				break;
			}
		}

		if (OL_NR(n) == tcLAST_BURGLARY_LEFT_CTRL_OBJ)
			lso->ul_Status |= (1 << Const_tcON_OFF);

		lsSetOldState(lso);   /* muá sein! */

		/* change for correcting amiga plans (corrects walls) before using pc level desinger
		lso->us_DestX -= 9;
		lso->us_DestY += 17;
		*/
	}
}

void lsCalcExactSize(LSObject lso, uword *x0, uword *y0, uword *x1, uword *y1)
	{
	Item item = (Item)dbGetObject(lso->Type);
	ubyte vertical = 0;

	(*x0) = lso->us_DestX;
	(*y0) = lso->us_DestY;

	// keine Ahnung warum bei Bild & Gem„lde, OPEN_CLOSE_BIT & HORIZ_VERT_BIT
	// vertauscht sind, aber es ist so (O.T. Helmut)
	if ((lso->Type == Item_Bild) || (lso->Type == Item_Gemaelde))
		vertical = lso->ul_Status & 3;
	else
		vertical = lso->ul_Status & 1;

	if (vertical) 	// vertical
		{
		(*x0) += item->VExactXOffset;
		(*y0) += item->VExactYOffset;

		(*x1) = (*x0) + item->VExactWidth;
		(*y1) = (*y0) + item->VExactHeight;
		}
	else
		{
		(*x0) += item->HExactXOffset;
		(*y0) += item->HExactYOffset;

		(*x1) = (*x0) + item->HExactWidth;
		(*y1) = (*y0) + item->HExactHeight;
		}
	}

// kopiert den Hintergrund, der durch eine Tür verdeckt wird
// in einen XMS Buffer
static void lsInitDoorRefresh(ulong ObjId)
	{
	LSObject lso  = (LSObject)dbGetObject(ObjId);
	uword    width, height, destX, destY;
	struct   LSDoorRefreshNode *drn;
	ubyte		found = 0;

	for (drn = (struct LSDoorRefreshNode *) LIST_HEAD(ls->p_DoorRefreshList);
		  NODE_SUCC(drn); drn = (struct LSDoorRefreshNode *) NODE_SUCC(drn))
			if (drn->lso == lso)
				found = 1;

	if (!found)
	{
		/* 2014-07-06 LucyG */
		ulong buffer_size = LS_PREPARE_BUFFER_SIZE;
		if (buffer_size > (LS_DOOR_REFRESH_XMS_RP.us_Width * LS_DOOR_REFRESH_XMS_RP.us_Height)) {
			buffer_size = (LS_DOOR_REFRESH_XMS_RP.us_Width * LS_DOOR_REFRESH_XMS_RP.us_Height);
			//Log("Warning: %s|%s: buffer size changed from %d to %d", __FILE__, __func__, LS_PREPARE_BUFFER_SIZE, buffer_size);
			memset(LS_PREPARE_BUFFER, 0, LS_PREPARE_BUFFER_SIZE);
		}
        memcpy(LS_PREPARE_BUFFER, LS_DOOR_REFRESH_XMS_RP.p_MemHandle, buffer_size);

		width  = lso->uch_Size;
		height = lso->uch_Size;

		destX = ls->us_DoorXOffset;
		destY = ls->us_DoorYOffset;

		gfxNCH4PutNCH4ToMCGA(l_wrp->p_BitMap, LS_PREPARE_BUFFER, lso->us_DestX, lso->us_DestY, destX, destY, width, height, 160, 320);

        memcpy(LS_DOOR_REFRESH_XMS_RP.p_MemHandle, LS_PREPARE_BUFFER, buffer_size);

		drn = (struct LSDoorRefreshNode *)
			CreateNode(ls->p_DoorRefreshList, sizeof(struct LSDoorRefreshNode), 0L);

		drn->lso = lso;

		drn->us_XOffset = ls->us_DoorXOffset;
		drn->us_YOffset = ls->us_DoorYOffset;

		if ((ls->us_DoorXOffset + width) >= 320)
			{
			if (ls->us_DoorYOffset <= 128)	// overflow verhindern
				{
				ls->us_DoorXOffset = 0;
				ls->us_DoorYOffset += height;
				}
			}
		else
			ls->us_DoorXOffset += width;
		}
	}

void lsDoDoorRefresh(LSObject lso)
	// restauriert aus einem XMS Buffer den Hintergrund einer Tür
	{
	struct   LSDoorRefreshNode *drn;
	uword width, height;

	/* 2014-07-06 LucyG */
	ulong buffer_size = LS_PREPARE_BUFFER_SIZE;
	if (buffer_size > (LS_DOOR_REFRESH_XMS_RP.us_Width * LS_DOOR_REFRESH_XMS_RP.us_Height)) {
		buffer_size = (LS_DOOR_REFRESH_XMS_RP.us_Width * LS_DOOR_REFRESH_XMS_RP.us_Height);
		//Log("Warning: %s|%s: buffer size changed from %d to %d", __FILE__, __func__, LS_PREPARE_BUFFER_SIZE, buffer_size);
		memset(LS_PREPARE_BUFFER, 0, LS_PREPARE_BUFFER_SIZE);
	}
    memcpy(LS_PREPARE_BUFFER, LS_DOOR_REFRESH_XMS_RP.p_MemHandle, buffer_size);

	for (drn = (struct LSDoorRefreshNode *) LIST_HEAD(ls->p_DoorRefreshList);
		  NODE_SUCC(drn); drn = (struct LSDoorRefreshNode *) NODE_SUCC(drn))
			if (drn->lso == lso)
				break;

	width  = lso->uch_Size;
	height = lso->uch_Size;

	gfxNCH4OLMCGAToNCH4Mask(LS_PREPARE_BUFFER, l_wrp->p_BitMap, drn->us_XOffset, drn->us_YOffset, lso->us_DestX, lso->us_DestY, width, height, 320, 160);
	}
