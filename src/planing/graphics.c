/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "planing/graphics.h"

void plPrintInfo(char *person)
{
    char info[80];

    sprintf(info, "%s", &person[1]);

    gfxSetRect(2, 320);
    gfxSetPens(m_wrp, 249, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxPrint(m_wrp, info, 12, GFX_PRINT_CENTER);
}

void plMessage(char *msg, ubyte flags)
{
    LIST *m = txtGoKey(PLAN_TXT, msg);

    if (flags & PLANING_MSG_REFRESH) ShowMenuBackground();

    if (m) plPrintInfo((char *)NODE_NAME(LIST_HEAD(m)));

    RemoveList(m);

    if (flags & PLANING_MSG_WAIT)
    {
        inpSetWaitTicks(120);  // 140
        inpWaitFor(INP_LBUTTONP | INP_TIME);
        inpSetWaitTicks(0L);
    }
}

void plDisplayTimer(uint32_t time, ubyte doSpotsImmediatly)
{
    char info[80];
    static uint32_t oldTimer = -1; /* 2014-06-28 LucyG : static */

    if (!time) time = CurrentTimer(plSys) / PLANING_CORRECT_TIME;

    if (GamePlayMode & GP_GUARD_DESIGN)
    {
        sprintf(info, "x:%d, y:%d   %s %02u:%02u:%02u %s", livGetXPos(Planing_Name[CurrentPerson]),
                livGetYPos(Planing_Name[CurrentPerson]), txtTimer, (uint32_t)(time / 3600),
                (uint32_t)((time / 60) % 60), (uint32_t)(time % 60), txtSeconds);

        gfxSetPens(m_wrp, 0, 0, 0);
        gfxRectFill(m_wrp, 120, 0, 320, 10);

        gfxSetRect(2, 320);
        gfxSetPens(m_wrp, 248, GFX_SAME_PEN, GFX_SAME_PEN);
        gfxPrint(m_wrp, info, 2, GFX_PRINT_RIGHT);
    }
    else
    {
        sprintf(info, "%s %02u:%02u:%02u %s", txtTimer, (uint32_t)(time / 3600), (uint32_t)((time / 60) % 60),
                (uint32_t)(time % 60), txtSeconds);

        gfxSetPens(m_wrp, 0, 0, 0);
        gfxRectFill(m_wrp, 220, 0, 320, 10);

        gfxSetRect(2, 320);
        gfxSetPens(m_wrp, 248, GFX_SAME_PEN, GFX_SAME_PEN);
        gfxPrint(m_wrp, info, 2, GFX_PRINT_RIGHT);
    }

    if (doSpotsImmediatly || (oldTimer != CurrentTimer(plSys)))
    {
        oldTimer = CurrentTimer(plSys);
        lsMoveAllSpots(time);
    }
}

void plDisplayInfo(void)
{
    char info[80];

    dbGetObjectName(OL_NR(GetNthNode(PersonsList, CurrentPerson)), info);

    gfxSetPens(m_wrp, 0, 0, 0);
    gfxRectFill(m_wrp, 0, 0, 120, 10);

    gfxSetRect(2, 320);
    gfxSetPens(m_wrp, 248, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxPrint(m_wrp, info, 2, GFX_PRINT_LEFT | GFX_PRINT_SHADOW);
}

ubyte plSay(char *msg, uint32_t persId)
{
    LIST *l = txtGoKey(PLAN_TXT, msg);
    ubyte choice = 0;

    SetPictID(((Person)dbGetObject(OL_NR(GetNthNode(PersonsList, persId))))->PictID);

    inpTurnESC(0);
    inpTurnFunctionKey(0);

    choice = Bubble(l, 0, NULL, 200);

    inpTurnFunctionKey(1);
    inpTurnESC(1);

    RemoveList(l);

    plDisplayTimer(0, 1);
    plDisplayInfo();

    return choice;
}

void plDrawWait(uint32_t sec)
{
    char time[10];

    sprintf(time, "%02u:%02u", (uint32_t)(sec / 60), (uint32_t)(sec % 60));

    gfxSetDrMd(m_wrp, GFX_JAM_2);
    gfxSetPens(m_wrp, 248, GFX_SAME_PEN, GFX_SAME_PEN);
    gfxSetRect(0, 320);
    gfxPrint(m_wrp, time, 31, GFX_PRINT_CENTER);
    gfxSetDrMd(m_wrp, GFX_JAM_1);
}

void plRefresh(uint32_t ItemId)
{
    LSObject obj = (LSObject)dbGetObject(ItemId);

    if (lsIsLSObjectInActivArea(obj)) lsFastRefresh(obj);

    if (lsGetStartArea() == lsGetActivAreaID()) lsShowEscapeCar();  // Auto neu zeichnen

    lsRefreshAllLootBags();  // alle LootBags neu zeichnen
}
