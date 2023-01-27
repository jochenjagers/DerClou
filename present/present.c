/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "present\present.h"
#include "present\present_p.h"

struct PresentControl PresentControl = {
	NULL, 0, 0};

#include "present\presenta.c"

void DrawPresent (LIST *present, ubyte firstLine, struct RastPort *rp, ubyte max)
{
	int i, j, k;
	struct presentationInfo *p;
	char txt [70],s[10];

	gfxSetPens (&RefreshRP, 224, 224, 224);
	gfxRectFill (&RefreshRP, 88, 3, 320, 49);

	if (firstLine)
	{
		gfxSetDestRPForShow(&RefreshRP);
		gfxShow(145, GFX_OVERLAY|GFX_NO_REFRESH, 0, 120, 5);
	}

	if ((max > (firstLine + NRBLINES)))
	{
		gfxSetDestRPForShow(&RefreshRP);
		gfxShow(146, GFX_OVERLAY|GFX_NO_REFRESH, 0, 120, 40);
	}

	for (i = firstLine, j = 4; (max < firstLine + NRBLINES) ? i<max : i<firstLine + NRBLINES; i++, j += 9)
	{
		p = (struct presentationInfo *)GetNthNode (present, i);

		strcpy (txt, NODE_NAME(p));

		switch (p->presentHow)
		{
		case PRESENT_AS_TEXT :
			if(p->extendedText)
			{
				for (k = 0; k < (57-strlen (NODE_NAME(p))-strlen(p->extendedText)); k++)
					strcat (txt, " ");

				strcat (txt, p->extendedText);
			}
			break;

		case PRESENT_AS_BAR  :
			gfxSetPens  (&RefreshRP, 250, 250, 251);
			gfxRectFill (&RefreshRP, 206, j, 316, j+7);
			gfxSetPens  (&RefreshRP, 251, 251, 251);
			gfxRectFill (&RefreshRP, 206, j, 206 + ((316-206)*p->extendedNr)/p->maxNr, j+7);

			gfxSetPens  (&RefreshRP, 249, 252, 253);

			gfxSetRect  (206, 316-206);
			sprintf  (s,"%ld %%",(p->extendedNr * 100)/(p->maxNr));	// 2017-10-28 LucyG : %%

			gfxPrint    (&RefreshRP,s,j,GFX_PRINT_CENTER);
			break;
		}

		if (!Config.gfxNoFontShadow) {	// 2014-07-13 LucyG : forgot this in v0.4
			gfxSetPens (&RefreshRP, 252, 224, GFX_SAME_PEN);
			gfxPrintExact(&RefreshRP, txt, 89, j+1);
		}

		gfxSetPens (&RefreshRP, 254, 224, GFX_SAME_PEN);
		gfxPrintExact(&RefreshRP, txt, 88, j);
	}

	gfxBlit(&RefreshRP, 88, 3, rp, 88, 3, 228, 46, GFX_ONE_STEP);
}

ubyte Present (ulong nr, const char *presentationText,void (*initPresentation)(ulong, LIST*, LIST*))
{
	ubyte firstVis;
	ubyte maxNr;
	ubyte exitLoop;
	ulong action;
	LIST *presentationData, *list;
	Person obj;
	Evidence e;
	uword y;

	presentationData = (LIST*)CreateList(0);

	SuspendAnim();

	gfxPrepareRefresh();

	inpTurnFunctionKey(0);

	if (dbIsObject(nr, Object_Person)) {
		obj = (Person)dbGetObject(nr);
		gfxShow(obj->PictID, GFX_NO_REFRESH|GFX_OVERLAY|GFX_BLEND_UP, 0, -1, -1);
	} else if (dbIsObject(nr, Object_Evidence)) {
		e = (Evidence)dbGetObject(nr);
		gfxShow((uword)((Person)dbGetObject(e->pers))->PictID, GFX_NO_REFRESH|GFX_OVERLAY|GFX_BLEND_UP, 0, -1, -1);
	} else if (dbIsObject(nr, Object_Tool)) {
		gfxShow(((Tool)(dbGetObject(nr)))->PictID, GFX_NO_REFRESH|GFX_OVERLAY|GFX_BLEND_UP, 0, -1, -1);
	} else if (dbIsObject(nr, Object_Loot)) {
		gfxShow(((Loot)(dbGetObject(nr)))->PictID, GFX_NO_REFRESH|GFX_OVERLAY|GFX_BLEND_UP, 0, -1, -1);
	}

	gfxShow((uword)BIG_SHEET, GFX_NO_REFRESH|GFX_OVERLAY, 0, -1, -1); /* nur die Farben ! */

	list = txtGoKey(PRESENT_TXT, (char *)presentationText);

	if (list) {
		initPresentation(nr, presentationData, list);
	}

	maxNr = GetNrOfNodes(presentationData);

	gfxSetDrMd(&RefreshRP, GFX_JAM_1);
	gfxSetFont(&RefreshRP, bubbleFont);

	firstVis = 0;
	DrawPresent(presentationData, firstVis, u_wrp, maxNr);

	exitLoop = 0;
	while (!exitLoop) {
		action = inpWaitFor (INP_UP+INP_DOWN+INP_LBUTTONP+INP_RBUTTONP);

		if ((action & INP_ESC) || (action & INP_RBUTTONP)) {
			exitLoop = 1;
		}

		if (action & INP_LBUTTONP) {
			exitLoop = 2;
		}

		if (action & INP_MOUSE) {
			y = inpGetMouseY(u_wrp);

			while ((y < 9) && (firstVis > 0)) {      /* Scroll up */
				DrawPresent(presentationData, --firstVis, u_wrp, maxNr);
				y = inpGetMouseY(u_wrp);
			}

			while ((y > 50) && (y <= 59) && (firstVis < (maxNr - 5))) { /* Scroll down */
				DrawPresent (presentationData, ++firstVis, u_wrp, maxNr);
				y = inpGetMouseY(u_wrp);
			}
		} else {
			if (action & INP_UP) {
				if (firstVis > 0) {
					firstVis --;
					DrawPresent (presentationData, firstVis, u_wrp, maxNr);
				}
			}

			if (action & INP_DOWN) {
				if ((maxNr-NRBLINES > 0) && (firstVis < (maxNr - NRBLINES))) {
					firstVis++;
					DrawPresent (presentationData, firstVis, u_wrp, maxNr);
				}
			}
		}
	}

	RemoveList (list);
	RemoveList (presentationData);

	gfxRefresh();

	ContinueAnim();

	if (exitLoop == 1) {
		return ((ubyte)GET_OUT);
	} else {
	    return ((ubyte)(exitLoop - 1));
	}
}

void AddPresentLine(LIST *l, ubyte presentHow, ulong data, ulong maxNr, LIST *texts, uword textNr)
{
	char *name = NULL;
	struct presentationInfo *p;

	if (textNr != ((uword)-1)) {
		name = NODE_NAME(GetNthNode(texts, (ulong)textNr));
	}

	p = (struct presentationInfo *)CreateNode(l, (ulong)sizeof(struct presentationInfo), name);

	p->presentHow = presentHow;
	p->maxNr      = maxNr;

	switch (presentHow) {
		case PRESENT_AS_TEXT: {
			if (data) {
				strcpy(p->extendedText, (char *)data);
			} else {
				strcpy (p->extendedText, "");
			}
		} break;
		case PRESENT_AS_NUMBER: {
			sprintf(p->extendedText, "%ld", data);
			p->presentHow = PRESENT_AS_TEXT;
		} break;
		case PRESENT_AS_BAR: {
			p->extendedNr = data;
		} break;
	}
}

void prSetBarPrefs(struct RastPort *p_RP,uword us_BarWidth,uword us_BarHeight,ubyte uch_FCol,ubyte uch_BCol, ubyte uch_TCol)
{
	PresentControl.p_CurrRP     = p_RP;
	PresentControl.us_BarWidth  = us_BarWidth;
	PresentControl.us_BarHeight = us_BarHeight;
	PresentControl.uch_FCol     = uch_FCol;
	PresentControl.uch_BCol     = uch_BCol;
	PresentControl.uch_TCol		= uch_TCol;
}

void prDrawTextBar(char *puch_Text,ulong ul_Value,ulong ul_Max,uword us_XPos,uword us_YPos)
{
	struct RastPort *p_RP = PresentControl.p_CurrRP;
	uword us_Width        = PresentControl.us_BarWidth;
	uword us_Height       = PresentControl.us_BarHeight;

	if(p_RP)
	{
		gfxSetRect  (us_XPos, us_Width);

		gfxSetPens  (p_RP, PresentControl.uch_BCol, GFX_SAME_PEN, PresentControl.uch_FCol);
		gfxRectFill (p_RP, us_XPos, us_YPos, us_XPos + us_Width - 1, us_YPos + us_Height - 1);

		us_Width = (uword) ((us_Width * ul_Value) / ul_Max);

		gfxSetPens  (p_RP, PresentControl.uch_FCol, GFX_SAME_PEN, GFX_SAME_PEN);
		gfxRectFill (p_RP, us_XPos, us_YPos, us_XPos + us_Width - 1, us_YPos + us_Height - 1);

		gfxSetPens  (p_RP, PresentControl.uch_TCol, GFX_SAME_PEN, GFX_SAME_PEN);

		if(puch_Text)
		{
			gfxSetDrMd(p_RP, GFX_JAM_1);
			gfxPrint  (p_RP, puch_Text, us_YPos + 2, GFX_PRINT_CENTER);
		}
	}
}
