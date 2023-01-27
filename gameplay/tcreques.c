/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "gameplay\gp.h"
#include "gameplay\gp_app.h"

void PrintStatus(char *text)
	{
	gfxSetDrMd(m_wrp,GFX_JAM_1);
	gfxSetRect(0,320);

	gfxSetPens(m_wrp, 249, 254, GFX_SAME_PEN);

	gfxPrint(m_wrp,text,10,GFX_PRINT_SHADOW|GFX_PRINT_CENTER);
	}

ulong ShowTheClouRequester(long error_class)
	{
	LIST *menu;
	ulong answer = 0;
	ubyte choices;
	Player player = (Player)dbGetObject(Player_Player_1); // MOD hg, 29-04-94

	inpTurnESC(0);

	ShowMenuBackground();

	switch(error_class)
		{
		case No_Error:

			ShowMenuBackground();

			menu=txtGoKey(MENU_TXT,"ESCMenu_STD");

			inpTurnFunctionKey(0);

			if (GamePlayMode & GP_DEMO)
				choices = Menu(menu, 3, 0, NULL, 0);
			else
				choices = Menu(menu, 15, 0, NULL, 0);

			inpTurnFunctionKey(1);

			switch(choices)
				{
				case 0:	/* Weiterspielen */
					player->CurrScene = 0L; // MOD hg, 29-04-94
					ShowMenuBackground();
					tcRefreshLocationInTitle(GetLocation);
					break;
				case 1:
					player->CurrScene = SCENE_THE_END;
					ShowMenuBackground();
					break;
				case 2:
					tcSaveTheClou(0);
					player->CurrScene = 0L; // MOD hg, 29-04-94
					ShowMenuBackground();
					tcRefreshLocationInTitle(GetLocation);
					break;
				case 3:	/* Laden */
					tcLoadTheClou();
					ShowMenuBackground();
					break;
				default:
					break;
				}

			RemoveList(menu);

			break;
		case Internal_Error:
			PrintStatus("Gravierender Fehler !");
			inpWaitFor(INP_LBUTTONP);
			ShowMenuBackground();
			break;
		case No_Mem:
			PrintStatus("Speichermangel !! - No Memory !!");
			inpWaitFor(INP_LBUTTONP);
			ShowMenuBackground();
			break;
		case Disk_Defect:
			PrintStatus("Disk Defekt !");
			inpWaitFor(INP_LBUTTONP);
			ShowMenuBackground();
			break;
		}

	inpTurnESC(1);

	return(answer);
	}
