/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "scenes\scenes.h"

ulong    CurrAreaId;
LSObject CurrLSO;  /* for FadeObjectInside */

static void FadeInsideObject(void)
{
	static ubyte status = 1;
	lsFadeRasterObject(CurrAreaId, CurrLSO, (status++) % 2);
}

ulong tcGoInsideOfHouse(ulong buildingID)
	{
	LIST  *menu = txtGoKey(MENU_TXT, "INSIDE_MENU"), *areas;
	ulong count, i, areaID = 0;

	consistsOfAll(buildingID, OLF_PRIVATE_LIST|OLF_INCLUDE_NAME, Object_LSArea);
	areas = ObjectListPrivate;
	dbSortObjectList(&areas, dbStdCompareObjects);

	count = GetNrOfNodes(areas) + 1;

	if (count > 2)
		{
		for(i = count; i < 5; i++)
			RemoveNode(menu,  NODE_NAME(GetNthNode(menu, count)));

		ShowMenuBackground();

		inpTurnFunctionKey(0);
		inpTurnESC(0);
		i = (ulong) Menu(menu, (1L << count) - 1, 0, 0L, 0L);
		inpTurnESC(1);
		inpTurnFunctionKey(1);

		consistsOfAll(buildingID, OLF_NORMAL, Object_LSArea);

		if(i)
			areaID = OL_NR(GetNthNode (areas, i-1));
		}

	ShowMenuBackground();
	ShowTime(0);

	RemoveList(areas);
	RemoveList(menu);

	return(areaID);
	}

void tcInsideOfHouse(ulong buildingID, ulong areaID, ubyte perc)
{
	LIST *objects;
	NODE *node, *n;
	ulong action, count;
	char name[TXT_KEY_LENGTH];
	char alarm[TXT_KEY_LENGTH], power[TXT_KEY_LENGTH];
	LSObject lso;
	LSArea area;
	LIST *menu;

	area = (LSArea)dbGetObject(areaID);
	menu = txtGoKey(MENU_TXT, "LookMenu");

	txtGetFirstLine (BUSINESS_TXT,"PROTECTED",alarm);
	txtGetFirstLine (BUSINESS_TXT,"SUPPLIED",power);

	inpTurnESC(0);
	inpTurnFunctionKey(0);

	/* liste und node initialisieren */
	SetObjectListAttr (OLF_PRIVATE_LIST, Object_LSObject);
	AskAll (area, ConsistOfRelationID, BuildObjectList);
	objects = ObjectListPrivate;

	//lsSortObjectList(&objects);

	count = (GetNrOfNodes(objects) * perc) / 255;
	node = lsGetSuccObject((NODE*) LIST_HEAD(objects));

	CurrAreaId = areaID;

	if ((GetNodeNrByAddr (objects, node)) > count) {
		SetBubbleType (THINK_BUBBLE);
		Say (BUSINESS_TXT, 0, MATT_PICTID, "CANT_LOOK");
	} else {
		action = 0;
		while (action != 4) {
			lso = (LSObject) OL_DATA(node);

			dbGetObjectName(lso->Type, name);

			if (lso->uch_Chained) {
				strcat(name,"(");
				if (lso->uch_Chained & Const_tcCHAINED_TO_ALARM) {
					strcat(name, alarm);
					if (lso->uch_Chained & Const_tcCHAINED_TO_POWER) {
						strcat(name, ", ");
						strcat(name, power);
					}
				} else {
					if (lso->uch_Chained & Const_tcCHAINED_TO_POWER) {
						strcat(name, power);
					}
				}

				strcat(name,")");
			}

			ShowMenuBackground();
			PrintStatus(name);

			CurrLSO = lso;

			SetMenuTimeOutFunc(FadeInsideObject);

			inpTurnFunctionKey(0);
			inpTurnESC(0);
			action = Menu(menu, 31, (ubyte) action, 0, 10);
			inpTurnESC(1);
			inpTurnFunctionKey(1);

			lsFadeRasterObject(areaID, lso, 1);

			switch (action) {
				case 0: {	// nächster Gegenstand
					n = lsGetSuccObject(node);
					if (GetNodeNrByAddr (objects, n) < (count-1)) {
						node = n;
					}
				} break;
				case 1: {	// vorheriger Gegenstand
					node = lsGetPredObject(node);
				} break;
				case 2: {	// genau ansehen
					tcShowObjectData(areaID, node, perc);
				} break;
				case 3: {	// Wächter
					isGuardedbyAll(buildingID, OLF_NORMAL, Object_Police);
					if (!(LIST_EMPTY(ObjectList))) {
						gfxSetPens(l_wrp, 4, GFX_SAME_PEN, GFX_SAME_PEN);
						grdDraw(l_wrp, buildingID, areaID);
					} else {
						Say (BUSINESS_TXT, 0, MATT_PICTID, "NO_GUARD");
					}
				} break;
				case 4: {	// Fertig
				} break;
				default: {
					action = 0;
				} break;
			}

			lso = (LSObject)OL_DATA(node);
			lsFadeRasterObject(areaID, lso, 1);
		}
	}

	RemoveList (menu);
	RemoveList (objects);
	inpSetWaitTicks (0);
}

void tcShowObjectData(ulong areaID, NODE *node, ubyte perc)
{
	NODE *n;

	/* Objekt selbst präsentieren */
	Present (OL_NR(node), "RasterObject", InitObjectPresent);

	/* Inhalt zusammenstellen */
	SetObjectListAttr (OLF_NORMAL, Object_Loot);
	AskAll (OL_DATA(node), hasLootRelationID, BuildObjectList);

	if (!LIST_EMPTY(ObjectList))
		{
		/* alle Loots durchgehen und anzeigen! */
		for(n = (NODE*) LIST_HEAD(ObjectList); NODE_SUCC(n); n = (NODE*) NODE_SUCC(n))
			{
			/* zur Variablenübergabe... (DIRTY, DIRTY...) */
			SetP (OL_DATA(n), hasLootRelationID, OL_DATA(n),
			GetP(OL_DATA(node), hasLootRelationID, OL_DATA(n)));

			Present (OL_NR(n), "RasterObject", InitOneLootPresent);

			/* kill these dirty things! */
			UnSet (OL_DATA(n), hasLootRelationID, OL_DATA(n));
			}
		}

	/* Verbindungen zeigen! */
	lsShowAllConnections(areaID, node, perc);
	}
