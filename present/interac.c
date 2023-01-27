/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "present\interac.h"
#include "present\interac_p.h"

extern void PlayFromCDROM(void);

void SetBubbleType (uword type)
{
	CurrentBubbleType=type;
}

void SetPictID (uword PictID)
{
	ActivPersonPictID = PictID;
}

ubyte GetExtBubbleActionInfo(void)
{
	return ExtBubbleActionInfo;
}

void SetMenuTimeOutFunc(void *func)
{
	MenuTimeOutFunc = (void (*)()) func;
}

ubyte ChoiceOk (ubyte choice, ubyte xit, LIST *l)
{
	if (choice == GET_OUT)
		return 0;

	if (choice == xit)
		return 0;

	if (l && !LIST_EMPTY(l))
	{
		struct ObjectNode *objNode = (struct ObjectNode *)GetNthNode (l, choice);

		if (!objNode->nr && !objNode->type && !objNode->data)
			return 0;
	}

	return 1;
}

void DrawMenu (LIST *menu, ubyte nr, long mode)
{
	ubyte i;
	char *m1 = 0, *m2 = 0;
	long x = 8, lastx = 0;

	if (mode == ACTIV_POSS)
		gfxSetPens(m_wrp, 249, GFX_SAME_PEN, GFX_SAME_PEN);
	else
		gfxSetPens(m_wrp, 248, GFX_SAME_PEN, GFX_SAME_PEN);

	for (i=0; i<=nr; i+=2)
	{
		m1 = NODE_NAME(GetNthNode (menu, i));

		if ((i+1)<=nr)
			m2 = NODE_NAME(GetNthNode (menu, i+1));
		else
			m2 = NULL;

		if (m2)
		{
			if (strlen (m1) > strlen (m2))
			{
				lastx = gfxTextLength (m_wrp, m1, strlen (m1));
				x += lastx;
			}
			else
			{
				lastx = gfxTextLength (m_wrp, m2, strlen (m2));
				x += lastx;
			}
		}
		else
		{
			lastx = gfxTextLength (m_wrp, m1, strlen (m1));
			x += lastx;
		}
	}

	if (nr == (i-2))
		gfxPrintExact(m_wrp, m1, x + 8*nr - lastx, TXT_1ST_MENU_LINE_Y);
	else
	{
		if (m2)
			gfxPrintExact(m_wrp, m2, x + 8*(nr-1) - lastx, TXT_2ND_MENU_LINE_Y);
	}
}

char SearchActiv (word delta, ubyte activ, ulong possibility, ubyte ub_max)
{
	do
	{
		activ += delta;

		if (activ > ub_max)
			activ = ub_max;

		if (possibility & (1L << activ))
			return (char)activ;
	} while ((activ > 0) && (activ < ub_max));

	return -1;
}

char SearchMouseActiv(ulong possibility, ubyte ub_max)  /* MOD : 14.12.93 hg */
{
	char activ;
	uword x, y;

	inpGetMouseXY(m_wrp, &x, &y);

	if ((y > TXT_1ST_MENU_LINE_Y - 10) && (y < TXT_2ND_MENU_LINE_Y + 10))
	{
		ub_max -= 1;

		for (activ = 0; (x >= MenuCoords[activ]) && (activ < ub_max / 2 + 1); activ++);

		activ = (activ - 1) * 2;

		if (y > TXT_1ST_MENU_LINE_Y + 7)
			activ++;

		if (activ > ub_max)
			activ = ub_max;

		if (possibility & (1L << activ))
			return (char) activ;
	}

	return -1;
}

void RefreshMenu(void)
{
	ubyte ub_max, i;

	if (refreshMenu)
	{
		ub_max = GetNrOfNodes (refreshMenu);

		for (i=0; i < ub_max; i++)
		{
			if (refreshPoss & (1L << i))
				DrawMenu (refreshMenu, i, INACTIV_POSS);
		}

		DrawMenu (refreshMenu, refreshActiv, ACTIV_POSS);
	}
}

ubyte Menu (LIST *menu, ulong possibility, ubyte activ, void (*func)(ubyte), ulong waitTime)
{
	ubyte i;
	long action;
	char nextActiv;
	ubyte ub_max;
	char ende = FALSE;
	uword x;
	NODE *n;
	uword l1, l2;

	if (menu && !LIST_EMPTY(menu))
	{
		if (!possibility)
			return 0;

		x = 0;

		for (ub_max = 0, n = (NODE*) LIST_HEAD(menu); NODE_SUCC(n); n = (NODE*) NODE_SUCC(n), ub_max++)
		{
			if ((ub_max % 2) == 0)
			{
				l1 = 0;
				l2 = 0;

				MenuCoords[ub_max/2] = x;

				l1 = gfxTextLength (m_wrp, NODE_NAME(n), strlen(NODE_NAME(n))) + 11;

				if(NODE_SUCC(NODE_SUCC(n)))
					l2 = gfxTextLength (m_wrp, NODE_NAME(NODE_SUCC(n)), strlen(NODE_NAME(NODE_SUCC(n)))) + 11;

				x += max(l1, l2);
			}
		}

		for (i=0; i < ub_max; i++)
		{
			if (possibility & (1L << i))
				DrawMenu (menu, i, INACTIV_POSS);
		}

		DrawMenu (menu, activ, ACTIV_POSS);

		if (func)
			func (activ);

		if (waitTime)
			inpSetWaitTicks (waitTime);

		while (!ende)
		{
			action = INP_LEFT     | INP_RIGHT | INP_UP | INP_DOWN |
						INP_LBUTTONP;

			if (waitTime)
					action |= INP_TIME;

			action = inpWaitFor(action);

			if (action & INP_TIME)
			{
				refreshMenu =NULL;

				if (MenuTimeOutFunc)
					MenuTimeOutFunc();
				else
					return (ubyte)TXT_MENU_TIMEOUT;
			}

			if (action & INP_FUNCTION_KEY)
			{
				refreshMenu  = menu;
				refreshPoss  = possibility;
				refreshActiv = activ;
				return ((ubyte)-1);
			}

			if ((action & INP_ESC) || (action & INP_RBUTTONP))
				return GET_OUT;

			if (action & INP_LBUTTONP)
				ende = TRUE;

			if (action & INP_MOUSE)    /* MOD : 14.12.93 hg */
			{
				if ((nextActiv = SearchMouseActiv (possibility, ub_max)) != ((char)-1))
				{
					if (nextActiv != activ)
					{
						DrawMenu (menu, activ, INACTIV_POSS);
						activ = nextActiv;
						DrawMenu (menu, activ, ACTIV_POSS);

						if (func)
							func (activ);
					}
				}				
			}
			else if (action & INP_MOUSEWHEEL)    
			{
				i = 0;

				if (action & INP_UP)
				{
					i = 1;
					do
					{
						nextActiv = SearchActiv(-i, activ, possibility, ub_max);
						i++;

						if (i = 255 && nextActiv == (char)-1)
						{
							nextActiv = activ;
							break;
						}

					} while (nextActiv == (char)-1);
				}
				else if (action & INP_DOWN)
				{
					i = 1;
					do
					{
						nextActiv = SearchActiv(i, activ, possibility, ub_max);
						i++;

						if (i = 255 && nextActiv == (char)-1)
						{
							nextActiv = activ;
							break;
						}

					} while (nextActiv == (char)-1);
				}

				DrawMenu(menu, activ, INACTIV_POSS);
				activ = nextActiv;
				DrawMenu(menu, activ, ACTIV_POSS);

				if (func)
					func(activ);
			}
			else
			{
				if (action & INP_UP || action & INP_DOWN || action & INP_LEFT || action & INP_RIGHT)
				{
					i = 0;

					if (action & INP_UP)
					{
						i = 1;
						do
						{
							nextActiv = SearchActiv(-i, activ, possibility, ub_max);
							i++;

							if (i = 255 && nextActiv == (char)-1)
							{
								nextActiv = activ;								
								break;
							}

						} while (nextActiv == (char)-1);
					}
					else if (action & INP_DOWN)
					{
						i = 1;
						do
						{
							nextActiv = SearchActiv(i, activ, possibility, ub_max);
							i++;

							if (i = 255 && nextActiv == (char)-1)
							{
								nextActiv = activ;
								break;
							}

						} while (nextActiv == (char)-1);
					}
					else if (action & INP_LEFT)
					{
						i = 2;
						do
						{
							nextActiv = SearchActiv(-i, activ, possibility, ub_max);
							i--;
						} while (nextActiv == (char)-1);
					}
					else if (action & INP_RIGHT)
					{
						i = 2;
						do
						{
							nextActiv = SearchActiv(i, activ, possibility, ub_max);							
							i--;	
						} while (nextActiv == (char)-1);
					}

					DrawMenu(menu, activ, INACTIV_POSS);
					activ = nextActiv;
					DrawMenu(menu, activ, ACTIV_POSS);

					if (func)
						func(activ);
				}
			}
		}

		refreshMenu  = NULL;

		return activ;
	}

	refreshMenu  = NULL;

	return activ;
}

void DrawBubble (LIST *bubble, ubyte firstLine, ubyte activ, struct RastPort *rp, ulong ul_max)
{
	int i, j;
	char *line = NULL;

	gfxSetPens (&RefreshRP, 224, 224, 224);
	gfxRectFill (&RefreshRP, X_OFFSET, 3, X_OFFSET+INT_BUBBLE_WIDTH, 49);

	if (firstLine)
	{
		gfxSetDestRPForShow(&RefreshRP);
		gfxShow(145, GFX_OVERLAY|GFX_NO_REFRESH, 0, X_OFFSET+135, 5);
	}

	if ((ul_max > (firstLine + NRBLINES)))
	{
		gfxSetDestRPForShow(&RefreshRP);
		gfxShow(146, GFX_OVERLAY|GFX_NO_REFRESH, 0, X_OFFSET+135, 40);
	}

	for (i = firstLine, j = 4; (ul_max < firstLine + NRBLINES) ? i < ul_max : i < firstLine + NRBLINES; i++, j += 9)
	{
		line = NODE_NAME(GetNthNode (bubble, i));

		if (!line)
			break;

		if (*line != '*')
		{
			if (!Config.gfxNoFontShadow) {
				gfxSetPens (&RefreshRP, BG_TXT_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
				gfxPrintExact(&RefreshRP, line, X_OFFSET, j+1);
			}

			gfxSetPens (&RefreshRP, VG_TXT_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
			gfxPrintExact(&RefreshRP, line, X_OFFSET, j);
		}
		else
		{
			line = line+1;

			if (!Config.gfxNoFontShadow) {
				if (activ == i)
					gfxSetPens (&RefreshRP, BG_ACTIVE_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
				else
					gfxSetPens (&RefreshRP, BG_INACTIVE_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);

				gfxPrintExact(&RefreshRP, line, X_OFFSET+1, j+1);
			}

			if (activ == i)
				gfxSetPens (&RefreshRP, VG_ACTIVE_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);
			else
				gfxSetPens (&RefreshRP, VG_INACTIVE_COLOR, GFX_SAME_PEN, GFX_SAME_PEN);

			gfxPrintExact(&RefreshRP, line, X_OFFSET, j);
		}
	}

	gfxBlit(&RefreshRP, X_OFFSET, 3, rp, X_OFFSET, 3, INT_BUBBLE_WIDTH - 3, 46, GFX_ONE_STEP);
}

ubyte Bubble(LIST *bubble, ubyte activ, void (*func)(ubyte), ulong waitTime)
{
	ubyte firstVis = 0;
	ubyte ende = FALSE;
	ulong action;
	long l_max = GetNrOfNodes(bubble);
	uword x, y;
	ubyte newactiv;

	SuspendAnim();

	gfxPrepareRefresh();

	if (activ == GET_OUT)
		activ = 0;

	if (activ >= NRBLINES)
		firstVis = activ-NRBLINES+1;

	if (func)
		func (activ);

	if (ActivPersonPictID == (uword)-1)
		gfxShow ((uword) CurrentBubbleType, GFX_NO_REFRESH|GFX_OVERLAY, 0, -1, -1);
	else
	{
		gfxShow ((uword) ActivPersonPictID, GFX_NO_REFRESH|GFX_OVERLAY|GFX_BLEND_UP, 0, -1, -1);
		gfxShow ((uword) CurrentBubbleType, GFX_NO_REFRESH|GFX_OVERLAY, 0, -1, -1);

		if (CurrentBubbleType == SPEAK_BUBBLE)
			gfxShow((uword) 9, GFX_NO_REFRESH|GFX_OVERLAY, 0, -1, -1);

		if (CurrentBubbleType == THINK_BUBBLE)
			gfxShow((uword)10, GFX_NO_REFRESH|GFX_OVERLAY, 0, -1, -1);
	}

	gfxSetDrMd (&RefreshRP, GFX_JAM_1);
	gfxSetFont (&RefreshRP, bubbleFont);
	DrawBubble (bubble, firstVis, activ, u_wrp, l_max);

	
	if (bCDRom)
	{
		PlayFromCDROM();
	}

	if (waitTime)
	{
		inpSetWaitTicks (waitTime);
		action = inpWaitFor (INP_LBUTTONP | INP_TIME | INP_RBUTTONP);

		if (action & INP_LBUTTONP)
			activ = 1;

		if ((action & INP_ESC) || (action & INP_RBUTTONP) || (action & INP_TIME))
			 activ = GET_OUT;

		inpSetWaitTicks (0L);

		ExtBubbleActionInfo = action;
	}
	else
	{
		while (!ende)
		{
			action = inpWaitFor (INP_UP | INP_DOWN | INP_LBUTTONP | INP_RBUTTONP | INP_LEFT | INP_RIGHT );

			ExtBubbleActionInfo = action;

			if ((action & INP_ESC) || (action & INP_RBUTTONP))
			{
				activ = GET_OUT;
				ende  = TRUE;
			}

			if (!ende)
			{
				if (action & INP_LBUTTONP)
					ende = TRUE;

				if (action & INP_MOUSE)
				{
					inpGetMouseXY(u_wrp, &x, &y);

					if ((x >= X_OFFSET) && (x <= X_OFFSET + INT_BUBBLE_WIDTH))
					{
						if ((y < 4) && (firstVis > 0))      /* Scroll up */
						{
							while ((y < 4) && (firstVis > 0))
							{
								firstVis -= 1; activ = firstVis;

								DrawBubble (bubble, firstVis, activ, u_wrp, l_max);

								if (func)
									func (activ);

								y = inpGetMouseY(u_wrp);
							}
						}
						else if ((y > 48) && (y <= 52) && (firstVis < (l_max - 5))) /* Scroll down */
						{
							while ((y > 48) && (y <= 52) && (firstVis < (l_max - 5)))
							{
								firstVis += 1; activ = firstVis + 4;

								if (activ > (l_max - 1))
									activ = l_max - 1;

								DrawBubble (bubble, firstVis, activ, u_wrp, l_max);

								if (func)
									func (activ);

								y = inpGetMouseY(u_wrp);
							}
						}
						else if ((y >= 4) && (y <= 48))
						{
							newactiv = firstVis + (y - 4) / 9;

							if (newactiv != activ)
							{
								activ = newactiv;

								if (activ > (l_max - 1))
									activ = l_max - 1;

								if (activ < firstVis)
									activ = firstVis;

								if (activ > firstVis + 4)
									activ = firstVis + 4;

								DrawBubble (bubble, firstVis, activ, u_wrp, l_max);

								if (func)
									func (activ);
							}
						}
					}
				}
				else if (action & INP_MOUSEWHEEL)
				{
					if (action & INP_UP)
					{
						if (activ > 0)
						{
							int cl = abs(firstVis - activ) + 1;

							while ((activ > 0) && cl)
							{
								activ--;
								cl--;

								if (*NODE_NAME(GetNthNode(bubble, activ)) == '*')
									break;
							}

							if (activ < firstVis)
								firstVis = activ;

							DrawBubble(bubble, firstVis, activ, u_wrp, l_max);

							if (func)
								func(activ);
						}
					}
					else if (action & INP_DOWN)
					{
						if (activ < l_max - 1)
						{
							int cl = NRBLINES - abs(firstVis - activ);

							while ((activ < l_max - 1) && cl)
							{
								activ++;
								cl--;

								if (*NODE_NAME(GetNthNode(bubble, activ)) == '*')
									break;
							}

							if (activ >(firstVis + NRBLINES - 1))
								firstVis = activ - NRBLINES + 1;

							DrawBubble(bubble, firstVis, activ, u_wrp, l_max);

							if (func)
								func(activ);
						}
					}
				}
				else
				{
					if (action & INP_UP)
					{
						if (activ > 0)
						{
							int cl = abs (firstVis-activ) + 1;

							while ((activ > 0) && cl)
							{
								activ--;
								cl--;

								if (*NODE_NAME(GetNthNode (bubble, activ)) == '*')
									break;
							}

							if (activ < firstVis)
								firstVis = activ;

							DrawBubble (bubble, firstVis, activ, u_wrp, l_max);

							if (func)
								func (activ);
						}
					}

					if (action & INP_DOWN)
					{
						if (activ < l_max-1)
						{
							int cl = NRBLINES - abs (firstVis-activ);

							while ((activ < l_max-1) && cl)
							{
								activ++;
								cl--;

								if (*NODE_NAME(GetNthNode (bubble, activ)) == '*')
									break;
							}

							if (activ > (firstVis + NRBLINES-1))
								firstVis = activ - NRBLINES + 1;

							DrawBubble (bubble, firstVis, activ, u_wrp, l_max);

							if (func)
								func (activ);
						}
					}
				}
			}
		}
	}

	if (activ != GET_OUT)
	{
		if (*NODE_NAME(GetNthNode (bubble, 0L)) != '*')
			activ=0;
	}

	SetBubbleType (SPEAK_BUBBLE);
	SetPictID (MATT_PICTID);

	gfxRefresh();

	ContinueAnim();

	return(activ);
}
