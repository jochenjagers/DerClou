/*	_________  _______
   / ___/ __ \/ __/ _ \		 Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "planing/guards.h"

void grdDo(FILE *fh, struct System *sys, LIST *PersonsList, uint32_t BurglarsNr, uint32_t PersonsNr, ubyte grdAction)
{
    ubyte i = 0;
    struct ObjectNode *n = NULL;

    for (i = BurglarsNr; i < PersonsNr; i++)
    {
        n = (struct ObjectNode *)GetNthNode(PersonsList, i);
        switch (grdAction)
        {
            case GUARDS_DO_SAVE:
            {
                SaveHandler(fh, sys, OL_NR(n));
            }
            break;
            case GUARDS_DO_LOAD:
            {
                LoadHandler(fh, sys, OL_NR(n));
            }
            break;
        }
    }
}

ubyte grdInit(FILE **fh, char *mode, uint32_t bldId, uint32_t areaId)
{
    char bldName[TXT_KEY_LENGTH];
    char fileName[TXT_KEY_LENGTH];

    dbGetObjectName(areaId, fileName);
    fileName[strlen(fileName) - 1] = '\0';
    sprintf(bldName, "%s%s", fileName, GUARD_EXTENSION);

    dskBuildPathName(GUARD_DIRECTORY, bldName, fileName);

    if ((*fh = dskOpen(fileName, mode, 0)))
    {
        return 1;
    }
    return 0;
}

void grdDone(FILE *fh) { dskClose(fh); }

ubyte grdAddToList(uint32_t bldId, LIST *l)
{
    struct ObjectNode *n = NULL;

    isGuardedbyAll(bldId, OLF_INCLUDE_NAME | OLF_INSERT_STAR, Object_Police);

    if (!LIST_EMPTY(ObjectList))
    {
        for (n = (struct ObjectNode *)LIST_HEAD(ObjectList); NODE_SUCC(n); n = (struct ObjectNode *)NODE_SUCC(n))
        {
            dbAddObjectNode(l, OL_NR(n), OLF_INCLUDE_NAME | OLF_INSERT_STAR);
        }
        return 1;
    }
    return 0;
}

ubyte grdDraw(struct RastPort *rp, uint32_t bldId, uint32_t areaId)
{
    FILE *fh = NULL;
    ubyte ret = 0;
    LIST *GuardsList = NULL;
    struct System *grdSys = NULL;
    uint32_t GuardsNr = 0;
    uint32_t i = 0;
    uword xpos = 0;
    uword ypos = 0;
    struct Handler *h = NULL;
    struct Action *action = NULL;
    struct ObjectNode *n = NULL;
    LSArea area = NULL;

    GuardsList = (LIST *)CreateList(0L);

    if (grdAddToList(bldId, GuardsList))
    {
        if (grdInit(&fh, "r", bldId, areaId))
        {
            GuardsNr = GetNrOfNodes(GuardsList);
            grdSys = InitSystem();

            for (i = 0; i < GuardsNr; i++)
            {
                n = (struct ObjectNode *)GetNthNode(GuardsList, i);
                InitHandler(grdSys, OL_NR(n), SHF_AUTOREVERS);
            }

            grdDo(fh, grdSys, GuardsList, 0, GuardsNr, GUARDS_DO_LOAD);
            grdDone(fh);

            dbSortObjectList(&GuardsList, dbStdCompareObjects);

            for (i = 0; i < GuardsNr; i++)
            {
                n = (struct ObjectNode *)GetNthNode(GuardsList, i);
                if (areaId == isGuardedbyGet(bldId, OL_NR(n)))
                {
                    h = FindHandler(grdSys, OL_NR(n));

                    /* getting start coordinates */
                    area = (LSArea)dbGetObject(areaId);
                    switch (i)
                    {
                        case 0:
                        case 2:
                            xpos = area->us_StartX4 / 2;
                            ypos = area->us_StartY4 / 2;
                            break;
                        case 1:
                        case 3:
                            xpos = area->us_StartX5 / 2;
                            ypos = area->us_StartY5 / 2;
                            break;
                    }

                    xpos += 4;

                    gfxMoveCursor(rp, xpos, ypos);

                    /* drawing system */
                    for (action = (struct Action *)LIST_HEAD(h->Actions); NODE_SUCC(action);
                         action = (struct Action *)NODE_SUCC(action))
                    {
                        switch (action->Type)
                        {
                            case ACTION_GO:
                                switch ((ActionData(action, struct ActionGo *))->Direction)
                                {
                                    case DIRECTION_LEFT:
                                        xpos -= action->TimeNeeded;
                                        break;
                                    case DIRECTION_RIGHT:
                                        xpos += action->TimeNeeded;
                                        break;
                                    case DIRECTION_UP:
                                        ypos -= action->TimeNeeded;
                                        break;
                                    case DIRECTION_DOWN:
                                        ypos += action->TimeNeeded;
                                        break;
                                }
                                gfxDraw(rp, xpos, ypos);
                                break;
                            case ACTION_WAIT:
                            case ACTION_OPEN:
                            case ACTION_CLOSE:
                            case ACTION_CONTROL:
                                // DrawCircle(rp, xpos, ypos, 1); MOD:
                                break;
                        }
                    }
                }
            }

            ret = 1;

            for (i = 0; i < GuardsNr; i++)
            {
                CloseHandler(grdSys, OL_NR(GetNthNode(GuardsList, i)));
            }

            CloseSystem(grdSys);
        }
    }

    RemoveList(GuardsList);

    return ret;
}
