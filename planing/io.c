/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "planing\io.h"
#include "port\port.h"

struct IOData
{
   NODE  io_Link;
   ubyte io_Data;
};

static char *Planing_Open[3] = { "r", "w", "r" };

// Loading & saving functions
void plSaveTools(FILE *fh)
{
	if (fh)
	{
		struct ObjectNode *n;
		hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Tool);

		fprintf(fh, PLANING_PLAN_TOOL_BEGIN_ID "\n");
		for (n = (struct ObjectNode *)LIST_HEAD(ObjectList); NODE_SUCC(n); n = (struct ObjectNode *)NODE_SUCC(n))
		{
			fprintf(fh, "%ld\n", OL_NR(n));
		}
		fprintf(fh, PLANING_PLAN_TOOL_END_ID "\n");
	}
}

LIST *plLoadTools(FILE *fh)
{
	LIST *l = txtGoKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_1");
	ubyte foundAll = 1, canGet = 2, toolsNr = 0;
	char buffer[32];

	buffer[0] = '\0';

	if (fh)
	{
		while (dskGets(buffer, 31, fh))
		{
			if (!strcmp(buffer, PLANING_PLAN_TOOL_END_ID)) break;

			if (atol(buffer))
			{
				toolsNr++;

				if (!has(Person_Matt_Stuvysunt, atol(buffer)))
				{
					if (has(Person_Mary_Bolton, atol(buffer)))
					{
						canGet++;
						dbAddObjectNode(l, atol(buffer), OLF_INCLUDE_NAME);
					}

					foundAll = 0;
				}
			}
		}
	}

	if (foundAll)
	{
		RemoveList(l);
		l = NULL;
	}
	else
	{
		LIST *extList = NULL;
		NODE *n;
		if (canGet == 2)
		{
			extList = txtGoKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_3");
		}
		else
		{
			if ((toolsNr - canGet) > 1)
			{
				extList = txtGoKeyAndInsert(PLAN_TXT, "SYSTEM_TOOLS_MISSING_2", (ulong)(toolsNr - canGet));
			}
			else if (toolsNr - canGet)
			{
				extList = txtGoKey(PLAN_TXT, "SYSTEM_TOOLS_MISSING_4");
			}
		}
		if (extList)
		{
			for (n = (NODE *)LIST_HEAD(extList); NODE_SUCC(n); n = (NODE *)NODE_SUCC(n))
			{
				CreateNode(l, 0, NODE_NAME(n));
			}
			RemoveList(extList);
		}
	}
	return(l);
}

ubyte plOpen(ulong objId, ubyte mode, FILE **fh)
{
	if (GamePlayMode & GP_GUARD_DESIGN)
	{
		if (grdInit(fh, Planing_Open[mode], objId, lsGetActivAreaID()))
			  return PLANING_OPEN_OK;
	}
	else
	{
		LIST *PlanList;
		ubyte i;
		FILE *pllFh;
		ulong pllData = 0L;
		char  pllPath[TXT_KEY_LENGTH],
				name1[TXT_KEY_LENGTH],
				name2[TXT_KEY_LENGTH],
				expan[TXT_KEY_LENGTH];
		struct IOData *ioData;

		dbGetObjectName(lsGetActivAreaID(), name1);
		name1[strlen(name1)-1] = '\0';	// remove number

		// MOD 25-04-94 HG - new paths on pc
		sprintf(name2, "%s%s",name1,PLANING_PLAN_LIST_EXTENSION);

		dskBuildPathName(DATADISK, name2, pllPath);

		if (pllFh = dskOpen(pllPath, "r", PLANING_PLAN_DISK))
		{
			dskGets(name2, TXT_KEY_LENGTH-1, pllFh);
			pllData = atol(name2);
			dskClose(pllFh);

			if ((mode == PLANING_OPEN_WRITE_PLAN) || pllData)
			{
				sprintf(name2, "MODE_%d", mode);
				plMessage(name2, PLANING_MSG_REFRESH);

				PlanList = (LIST*)CreateList(0L);

				dbGetObjectName(objId, name1);

				expan[0] = EOS;

				for (i = 0; i < PLANING_NR_PLANS; i++)
				{
					if ((mode == PLANING_OPEN_WRITE_PLAN) || (pllData & (1L << i)))
					{
						if (mode == PLANING_OPEN_WRITE_PLAN)
						{
							if (pllData & (1L << i))
								txtGetFirstLine(PLAN_TXT, "ATTENTION_1", expan);
							else
								txtGetFirstLine(PLAN_TXT, "ATTENTION_2", expan);
						}

						sprintf(name2, "*%s Plan %d    %s", name1, i+1, expan);
						ioData = (struct IOData *)CreateNode(PlanList, sizeof(struct IOData), name2);
						ioData->io_Data = i;
					}
				}

				sprintf (name2, "EXPAND_MODE_%d", mode);
				txtGetFirstLine(PLAN_TXT, name2, expan);
				ExpandObjectList (PlanList, expan);

				i = Bubble(PlanList, 0, NULL, 0L);

				if (ChoiceOk(i, GET_OUT, PlanList))
				{
					ioData = (struct IOData *)GetNthNode(PlanList, i);
					i = ioData->io_Data;

					dbGetObjectName(lsGetActivAreaID(), name1);
					name1[strlen(name1)-1] = '\0';

					sprintf(name2,"%s%d%s",name1, i+1, PLANING_PLAN_EXTENSION);

					dskBuildPathName(DATADISK, name2, name1);

					*fh = dskOpen(name1, Planing_Open[mode], PLANING_PLAN_DISK);

					if (mode == PLANING_OPEN_WRITE_PLAN)
					{
						pllData |= 1L<<i;

						if (pllFh = dskOpen(pllPath, "w", PLANING_PLAN_DISK))
						{
							fprintf (pllFh, "%ld", pllData);
							dskClose (pllFh);
						}
					}

					if (*fh)
					{
						RemoveList(PlanList);
						return PLANING_OPEN_OK;
					}
					else
						NewErrorMsg(Disk_Defect, __FILE__, __func__, 0);
				}

				RemoveList (PlanList);

				return PLANING_OPEN_ERR_NO_CHOICE;
			}

			return PLANING_OPEN_ERR_NO_PLAN;
		}
	}

	NewErrorMsg(Disk_Defect, __FILE__, __func__, 0);
	return 0;
}

void plSave(ulong objId)
{
	FILE *fh = NULL;

	if (plOpen(objId, PLANING_OPEN_WRITE_PLAN, &fh) == PLANING_OPEN_OK)
	{
		if (GamePlayMode & GP_GUARD_DESIGN)
		{
			grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr, GUARDS_DO_SAVE);
		}
      else
      {
         ubyte i;

         SaveSystem(fh, plSys);
         plSaveTools(fh);

         for (i = 0; i < BurglarsNr; i++)
				SaveHandler(fh, plSys, OL_NR(GetNthNode(PersonsList, i)));
      }

      dskClose(fh);
   }
}

void plSaveChanged(ulong objId)
{
   if (PlanChanged)
   {
      LIST  *l = txtGoKey(PLAN_TXT, "PLAN_CHANGED");

      inpTurnESC(0);

		if (Bubble(l, 0, NULL, 0L) == 0)
		{
			inpTurnESC(1);

			if (!plAllInCar(objId))
				plSay("PLAN_NOT_FINISHED", 0);

			plSave(objId);
		}
		else
			inpTurnESC(1);

		RemoveList(l);
	}
}

void plLoad(ulong objId)
{
	FILE *fh = NULL;
	ubyte ret;

	if (objId == Building_Starford_Kaserne)
	{
		while ((ret = plOpen(objId, PLANING_OPEN_READ_PLAN, &fh)) != PLANING_OPEN_OK);
	}
	else
	{
		ret = plOpen(objId, PLANING_OPEN_READ_PLAN, &fh);
	}

	if (ret == PLANING_OPEN_OK)
	{
		if (GamePlayMode & GP_GUARD_DESIGN)
		{
			grdDo(fh, plSys, PersonsList, BurglarsNr, PersonsNr, GUARDS_DO_LOAD);
		}
		else
		{
			LIST *l = NULL;
			ubyte i;
			ubyte goon = 1;

			if (l = LoadSystem(fh, plSys))
			{
				inpTurnESC(0);
				Bubble(l, 0, NULL, 0L);
				inpTurnESC(1);
				RemoveList(l);

				goon = 0;
				l 	  = NULL;
			}

			if (l = plLoadTools(fh))
			{
				inpTurnESC(0);
				Bubble(l, 0, NULL, 0L);
				inpTurnESC(1);
				RemoveList(l);

				goon = 0;
			}

			if (goon)
			{
				for (i = 0; i < BurglarsNr; i++)
					LoadHandler(fh, plSys, OL_NR(GetNthNode(PersonsList, i)));
			}
		}

		dskClose (fh);
	}
	else
	{
		if (ret == PLANING_OPEN_ERR_NO_PLAN)
         plMessage("NO_PLAN", PLANING_MSG_REFRESH|PLANING_MSG_WAIT);
   }
}
