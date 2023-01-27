/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "anim\sysanim.h"

#define	PIC_MODE_POS		1
#define	PIC_P_SEC_POS		2
#define	PIC_1_ID_POS		3
#define	ANIM_COLL_ID_POS	4
#define	PIC_COUNT_POS		5
#define	PHASE_WIDTH_POS	6
#define	PHASE_HEIGHT_POS	7
#define	PHASE_OFFSET_POS	8
#define	X_DEST_OFFSET_POS	9
#define	Y_DEST_OFFSET_POS	10
#define	PLAY_MODE_POS		11

/* Defines fueÅr Playmode */
#define	PM_NORMAL			     1
#define	PM_PING_PONG		     2
#define  PM_SYNCHRON            4

/* Defines fueÅr AnimPic Aufbau */
#define 	Y_OFFSET		           0	/* 1 Pixel zwischen 2 Reihen */

struct AnimHandler
{
	ubyte	*RunningAnimID;      /* Anim, die gerade laeuft */

	uword destX;
	uword destY;
	uword width;
	uword height;

	uword offset;
	uword frameCount;

	uword pictsPerRow;
	uword totalWidth;

	uword NrOfAnims;
	uword PictureRate;	      /* Rate mit der Anim gespielt wird */

	uword Repeatation;	      /* wie oft wiederholen */
	uword RepeatationCount;	/* wie oft schon wiederholt */

	ulong WaitCounter;

	uword AnimCollection;		/* einzelnen Animphasen */

	uword CurrPictNr;
	word  Direction;

	ubyte PlayMode;

	ubyte AnimatorState;
};

static struct AnimHandler Handler = {NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static char  RunningAnimLine[TXT_KEY_LENGTH];

#define ANIM_FRAME_XMS_RP	AnimRPInXMS
#define ANIM_FRAME_RP		PrepareRP

#define ANIM_STATE_SUSPENDED	1<<0

void	LoadAnim(char *AnimID);

static void GetAnim(char *AnimID,char *Dest);
static void PrepareAnim (char *AnimID);

/*
* init & dones
*/

void InitAnimHandler (void)
{
	Handler.RunningAnimID =(ubyte*) RunningAnimLine;
	strcpy((char*)Handler.RunningAnimID, "");
}

void CloseAnimHandler (void)
{
	StopAnim();
}

void SuspendAnim(void)
{
	Handler.AnimatorState |= ANIM_STATE_SUSPENDED;
}

void ContinueAnim(void)
{
	Handler.AnimatorState &= (0xff - ANIM_STATE_SUSPENDED);
}

/*
* prepare...
*/

// initialisert diverse Werte und kopiert anschliessend die Animphasen
// in den XMS Speicher
static void PrepareAnim (char *AnimID)
{
	char pict_list[TXT_KEY_LENGTH] = {0};
	struct	Collection *coll;

	GetAnim(AnimID, pict_list);

	if ((ulong)(txtCountKey(pict_list)) > PIC_1_ID_POS)
	{
		coll = gfxGetCollection((uword) txtGetKeyAsULONG((uword)ANIM_COLL_ID_POS, pict_list));

		Handler.frameCount = (uword) txtGetKeyAsULONG((uword)PIC_COUNT_POS, pict_list);

		Handler.width  = (uword) txtGetKeyAsULONG((uword)PHASE_WIDTH_POS, pict_list);
		Handler.height = (uword) txtGetKeyAsULONG((uword)PHASE_HEIGHT_POS, pict_list);

		Handler.offset = (uword) txtGetKeyAsULONG((uword)PHASE_OFFSET_POS, pict_list);

		Handler.destX  = (uword) txtGetKeyAsULONG((uword)X_DEST_OFFSET_POS, pict_list);
		Handler.destY  = (uword) txtGetKeyAsULONG((uword)Y_DEST_OFFSET_POS, pict_list);

		/* einen Offset muss ich zur Totalbreite addieren ! Beispiel :	 */
		/* 3 Bilder mit Width = 80, Offset = 2 -> TotalWidth = 244		 */
		/* 244 / 3 ist aber nur 2, obwohl in dieser Zeile 3 Bilder sind ! */
		Handler.pictsPerRow    = (coll->us_TotalWidth+Handler.offset)/(Handler.width+Handler.offset);
		Handler.totalWidth     = coll->us_TotalWidth;

		Handler.AnimCollection = coll->us_CollId;

		/* jetzt die Animphasen vorbereiten und ins XMS kopieren */
		gfxCopyCollToXMS(coll->us_CollId, &ANIM_FRAME_XMS_RP);
	}
}

/*
* PlayAnim
* StopAnim
*/

void PlayAnim (char *AnimID, word how_often, ulong mode)
{
	char pict_list[TXT_KEY_LENGTH] = {0};
	uword	pict_id=0, rate;

	GetAnim(AnimID, pict_list);

	if (pict_list[0] == '\0')
	{
		gfxClearArea(l_wrp);
	}
	else
	{
		StopAnim();

		PrepareAnim(AnimID);

		if (!(mode & GFX_DONT_SHOW_FIRST_PIC))
		{
			if (!mode)
			{
				mode = (ulong)txtGetKeyAsULONG((uword)PIC_MODE_POS,(char*)pict_list);
			}
			pict_id = (uword)txtGetKeyAsULONG((uword)PIC_1_ID_POS,(char*)pict_list);
		}

		if (pict_id)
		{
			gfxShow(pict_id, mode, 2, -1L, -1L);
		}

		if ((ulong)(txtCountKey((char*)pict_list)) > PIC_1_ID_POS)
		{
			rate = (uword)txtGetKeyAsULONG((uword)PIC_P_SEC_POS, (char*)pict_list);

			Handler.PlayMode    = (ubyte)txtGetKeyAsULONG((uword)PLAY_MODE_POS, (char*)pict_list);
			Handler.PictureRate = rate;
			Handler.Repeatation = how_often;

			Handler.CurrPictNr 		 = 0;
			Handler.Direction  		 = 1;
			Handler.RepeatationCount = 0;

			Handler.WaitCounter      = 1;

			/* DoAnim ist ready to play and our anim is decrunched */
			strcpy((char*)Handler.RunningAnimID, (char*)AnimID);

			ContinueAnim();	// falls Anim zuvor "suspended" wurde
		}
		else
		{
			strcpy((char*)Handler.RunningAnimID, "");
		}
	}
}

void StopAnim(void)
{
	char pict_list[TXT_KEY_LENGTH] = {0};
	struct Picture *pict;

	if (Handler.RunningAnimID)   /* es laeuft eine Anim */
	{
		if(strcmp((char*)Handler.RunningAnimID,"") != 0)
		{
			GetAnim((char*)Handler.RunningAnimID, pict_list);

			/* der Vollstaendigkeit wegen die Bilder "unpreparen" */
			pict = gfxGetPicture((uword)txtGetKeyAsULONG((uword)PIC_1_ID_POS,(char*)pict_list));
			/* 2015-01-07 LucyG: Baris reported NULL pointer access in exploding Jaguar scene */
			if (pict) {
				gfxUnPrepareColl ((uword)pict->us_CollId);
			} else {
				Log("%s|%s: gfxGetPicture(PIC_1_ID_POS, %s) = NULL", __FILE__, __func__, pict_list ? pict_list : "NULL");
			}

			/* der Vollstaendigkeit wegen die Bilder "unpreparen" */
			if(txtCountKey((char*)pict_list) > PIC_1_ID_POS) {
				gfxUnPrepareColl ((uword)txtGetKeyAsULONG((uword)ANIM_COLL_ID_POS,(char*)pict_list));
			}

			strcpy((char*)Handler.RunningAnimID,"");
		}
	}
}

/*
* access function
* GetAnim
*/

static void GetAnim(char *AnimID,char *Dest)
{
	long i;
	char ID[TXT_KEY_LENGTH] = {0};

	strcpy(ID, (char*)AnimID);

	for(i = 0;i < strlen(ID); i++)
		if(ID[i] == ',')   ID[i] = '_';

	txtGetNthString(ANIM_TXT, ID, 0, Dest);
}

/*
* Animator
*/

void animator(void)
{
	long destX = Handler.destX;
	long destY = Handler.destY;
	uword sourceX, sourceY;

	if (!(Handler.AnimatorState & ANIM_STATE_SUSPENDED))
	{
		if ((Handler.RunningAnimID) && (strcmp((char*)Handler.RunningAnimID,"") != 0))
		{
			if (Handler.RepeatationCount <= Handler.Repeatation)
			{
				if ((--Handler.WaitCounter) == 0)
				{
					Handler.WaitCounter = Handler.PictureRate + CalcRandomNr(0, 3);

					if(Handler.CurrPictNr == 0)
					{
						Handler.RepeatationCount++;

						Handler.Direction = 1;
					}

					if (Handler.CurrPictNr == (Handler.frameCount - 1))
					{
						Handler.RepeatationCount++;

						if (Handler.PlayMode & PM_NORMAL)
							Handler.CurrPictNr = 0;
						else
							Handler.Direction = -1;
					}

					if(Handler.RepeatationCount <= Handler.Repeatation)
					{
						sourceX = ((Handler.width+Handler.offset)*Handler.CurrPictNr) % (Handler.totalWidth);
						sourceY = (Handler.CurrPictNr / Handler.pictsPerRow) * (Handler.height+Y_OFFSET);

						/* sicherstellen, dass Animframes immer vorhanden sind */

						gfxCopyCollFromXMS(Handler.AnimCollection, ANIM_FRAME_RP.p_BitMap);

						gfxBlit(&ANIM_FRAME_RP, sourceX, sourceY, l_wrp, destX, destY,
							Handler.width, Handler.height, GFX_ONE_STEP);
					}

					Handler.CurrPictNr = Handler.CurrPictNr + Handler.Direction;
				}
			}
		}
	}
}
