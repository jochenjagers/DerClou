/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#define   ORG_DISP_GUY_Y           60
#define   ORG_DISP_ABILITIES_Y     75
#define   ORG_DISP_GUY_WIDTH       80
#define   ORG_DISP_LINE            10

#define   ORG_PICT_ID				  3

void tcInitDisplayOrganisation(void)
{
	gfxChangeColors(l_wrp, 5, GFX_FADE_OUT, 0);
	gfxClearArea(l_wrp);
	RefreshDisplayConfig();
}

void RefreshDisplayConfig(void)
{
	gfxSetDestRPForShow(&RefreshRP);
	gfxShow(ORG_PICT_ID, GFX_NO_REFRESH|GFX_BLEND_UP, 0, -1, -1);

	CurrentBackground = BGD_PLANUNG;
	ShowMenuBackground();
}

void tcDoneDisplayOrganisation(void)
{
	gfxChangeColors(l_wrp, 5, GFX_FADE_OUT, 0);
	gfxClearArea(l_wrp);
	gfxSetPens(m_wrp, GFX_SAME_PEN, GFX_SAME_PEN, 0);
}

void tcDisplayOrganisation(void)
{
	tcDisplayCommon();
	tcDisplayPerson(ORG_DISP_ABILITIES);

	gfxBlit(&RefreshRP, 0, 0, l_wrp, 0, 0, 320, 140, GFX_ONE_STEP);
}

void tcDisplayCommon(void)
{
	LIST     *texts;
	char    line[TXT_KEY_LENGTH], name[TXT_KEY_LENGTH];
	Building building;

	texts = txtGoKey(BUSINESS_TXT,"PLAN_COMMON_DATA");

	gfxSetDestRPForShow(&RefreshRP);
	gfxShow(ORG_PICT_ID, GFX_ONE_STEP|GFX_NO_REFRESH, 0, -1, -1);

	/* Gebäude anzeigen  */

	gfxSetFont  (&RefreshRP, menuFont);
	gfxSetDrMd  (&RefreshRP, GFX_JAM_1);
	gfxSetRect  (0, 320);

	gfxSetPens(&RefreshRP, 249, 254, GFX_SAME_PEN);

	if(Organisation.BuildingID)
	{
		building = (Building) dbGetObject(Organisation.BuildingID);

		dbGetObjectName (Organisation.BuildingID,line);
		gfxPrint (&RefreshRP, line, 9, GFX_PRINT_CENTER|GFX_PRINT_SHADOW);
	}


	gfxSetFont(&RefreshRP, bubbleFont);
	gfxSetPens(&RefreshRP, 249, GFX_SAME_PEN, GFX_SAME_PEN);

	/*************************************************************
	 *  Fluchtwagen anzeigen
	*/

	gfxSetRect (0,106);
	strcpy  (line, NODE_NAME(GetNthNode(texts,0L)));

	if (Organisation.CarID)
	{
		dbGetObjectName(Organisation.CarID, name);
		tcCutName(name, (ubyte)' ', 12);
		strcat(line, name);
	}
	else
	    strcat(line, " ? ");

	gfxPrint (&RefreshRP, line, 25, GFX_PRINT_LEFT);

	/*************************************************************
	 *  Zulassung anzeigen
	*/

	gfxSetRect(106,106);
	strcpy(line, NODE_NAME(GetNthNode(texts, 2L)));

	if (Organisation.CarID)
	{
		sprintf(name, "%d",Organisation.PlacesInCar);
		strcat        (line, name);
	}
	else
	    strcat(line, " ? ");

	gfxPrint (&RefreshRP, line, 25, GFX_PRINT_CENTER);

	/*************************************************************
	 *  Fahrer anzeigen
	*/

	gfxSetRect (212,106);
	strcpy  (line, NODE_NAME(GetNthNode(texts, 1L)));

	if (Organisation.DriverID)
	{
		dbGetObjectName (Organisation.DriverID, name);
		strcat        (line, name);
	}
	else
	    strcat(line, " ? ");

	gfxPrint (&RefreshRP, line, 25, GFX_PRINT_RIGHT);

	/*************************************************************
	 *  Fluchtweg
	*/

	gfxSetRect (0,106);
	strcpy (line, NODE_NAME(GetNthNode(texts, 3L)));	// "Fluchtweg: "

	if (Organisation.BuildingID)
	{
		LIST *enums = txtGoKey (OBJECTS_ENUM_TXT,"enum_RouteE");

		strcpy(name,NODE_NAME(GetNthNode(enums,building->EscapeRoute)));
		strcat(line,name);

		RemoveList (enums);
	}
	else
	    strcat(line, " ? ");

	gfxPrint (&RefreshRP, line, 35, GFX_PRINT_LEFT);

	/*************************************************************
	 *  Fluchtweg-l„nge
	*/

	gfxSetRect (106,106);
	strcpy  (line, NODE_NAME(GetNthNode(texts, 4L)));

	if (Organisation.BuildingID)
	{
		/* 2014-06-28 LucyG : space added */
		sprintf(name," %d (km)",building->EscapeRouteLength);
		strcat(line, name);
	}
	else
	    strcat(line, " ? ");

	gfxPrint (&RefreshRP, line, 35, GFX_PRINT_CENTER);

	/*************************************************************
	 *  mein Anteil
	*/

	gfxSetRect (212,106);
	strcpy  (line, NODE_NAME(GetNthNode(texts, 5L)));

	/* 2014-06-28 LucyG : missing percent sign */
	sprintf (name, " %d%%",tcCalcMattsPart());
	strcat (line, name);

	gfxPrint (&RefreshRP, line, 35, GFX_PRINT_RIGHT);

	RemoveList(texts);
}

void tcDisplayPerson(ulong displayMode)
{
	ulong  objNr,i;
	NODE   *node;
	char  line[TXT_KEY_LENGTH];
	LIST   *guys;

	joined_byAll(Person_Matt_Stuvysunt, OLF_INCLUDE_NAME|OLF_PRIVATE_LIST, Object_Person);
	guys = ObjectListPrivate;
	dbSortObjectList (&guys, dbStdCompareObjects);

	for (node=(NODE*)LIST_HEAD(guys),i=0;NODE_SUCC(node);node = (NODE*)NODE_SUCC(node),i++)
	{
		objNr =  ((struct ObjectNode *)node)->nr;

		if (strlen(NODE_NAME(node)) >= 16)
			tcGetLastName (NODE_NAME(node), line, 15);
		else
		    strcpy(line, NODE_NAME(node));

		gfxSetRect (ORG_DISP_GUY_WIDTH * i + 5, ORG_DISP_GUY_WIDTH -5);
		gfxSetPens(&RefreshRP, 248, GFX_SAME_PEN, GFX_SAME_PEN);
		gfxSetFont (&RefreshRP,menuFont);
		gfxSetDrMd (&RefreshRP, GFX_JAM_1);
		gfxPrint (&RefreshRP, line, ORG_DISP_GUY_Y, GFX_PRINT_CENTER);
		gfxSetFont (&RefreshRP,bubbleFont);

		if(displayMode & ORG_DISP_ABILITIES)    tcDisplayAbilities(objNr,i);
	}

	RemoveList (guys);
}

void tcDisplayAbilities(ulong personNr,ulong displayData)
{
	LIST  *abilities;
	NODE  *node;
	ulong i,abiNr, ability;
	char line[TXT_KEY_LENGTH];

	hasAll(personNr,OLF_PRIVATE_LIST|OLF_INCLUDE_NAME,Object_Ability);
	abilities = ObjectListPrivate;

	prSetBarPrefs(&RefreshRP, ORG_DISP_GUY_WIDTH - 5, ORG_DISP_LINE + 1, 251, 250, 249);

	if(!(LIST_EMPTY(abilities)))
	{
		for (node=(NODE*)LIST_HEAD(abilities),i=0;NODE_SUCC(node);node=(NODE*)NODE_SUCC(node),i++)
		{
			abiNr =  ((struct ObjectNode *)GetNthNode(abilities,(ulong)i))->nr;
			ability = hasGet(personNr, abiNr);

			/* 2014-06-27 templer
			missing percent sign in the following line to display the percent sign in the output
			*/
			sprintf(line, "%s %d%%",NODE_NAME(node),(uword)((ability * 100) / 255));

			prDrawTextBar(line,ability, 255L,
				displayData * ORG_DISP_GUY_WIDTH + 5,
				ORG_DISP_ABILITIES_Y + i * (ORG_DISP_LINE + 4));
		}
	}
	else
	{
		i = 0; // 2014-06-30 LucyG: i is used, but was never initialised
		txtGetFirstLine (BUSINESS_TXT, "PLAN_NO_CAPABILITY", line);
		gfxSetRect (ORG_DISP_GUY_WIDTH * i + 5, ORG_DISP_GUY_WIDTH -5);
		gfxSetDrMd (&RefreshRP, GFX_JAM_1);
		gfxPrint (&RefreshRP, line, ORG_DISP_ABILITIES_Y + ORG_DISP_LINE, GFX_PRINT_LEFT);
	}

	RemoveList (abilities);
}
