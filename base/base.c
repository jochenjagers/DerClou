/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "SDL.h"
#include "base\base.h"

// cheat function
#include "base\fullenv.c"

void *StdBuffer0 = 0;
void *StdBuffer1 = 0;

char prgname[255];

void tcClearStdBuffer(void *p_Buffer)
{
	long i;
	char *p = (char *) p_Buffer;
	long size = 0;

	if (p == StdBuffer0)
		size = STD_BUFFER0_SIZE;

	if (p == StdBuffer1)
		size = STD_BUFFER1_SIZE;

	for (i = 0; i < size; i++)
		p[i] = 0;
}

static void tcDone(void)
{
	plDone();
	sndDoneFX(); // achtung
	sndDone();
	if (FXBase.us_SoundBlasterOk) RemoveSBlast();
	dbDone();
	CloseAnimHandler();
	txtDone();
	inpCloseAllInputDevs();
	gfxDone();
	rndDone();

	#ifdef THECLOU_CDROM_VERSION
	if (CDRomInstalled)
	{
		CDROM_StopAudioTrack();
		CDROM_UnInstall();
	}
	#endif

	if (StdBuffer0)
		MemFree(StdBuffer0, STD_BUFFER0_SIZE);

	if (StdBuffer1)
		MemFree(StdBuffer1, STD_BUFFER1_SIZE);

	if (memGetAllocatedMem())
		Log("WARNING! Memory Leak (%d bytes)", memGetAllocatedMem());

	pcErrClose();
}

static long tcInit(void)
{
	pcErrOpen(ERR_NO_OUTPUT, tcDone, NULL);

	long *p, i;

	if (!InitSBlaster()) FXBase.us_SoundBlasterOk = 1;

	StdBuffer0 = (void *)MemAlloc(STD_BUFFER0_SIZE);
	StdBuffer1 = (void *)MemAlloc(STD_BUFFER1_SIZE);

	#ifdef THECLOU_CDROM_VERSION
	if (CDRomInstalled = CDROM_Install())
		CDROM_WaitForMedia();
	else return(0);
	#endif

	if (StdBuffer0 && StdBuffer1)
	{
		gfxInit();
		inpOpenAllInputDevs();

		#ifdef THECLOU_VERSION_ENGLISH
		txtInit(TXT_LANG_ENGLISH);
		#else
		txtInit(TXT_LANG_GERMAN);
		#endif

		InitAnimHandler();

		dbInit();

		sndInit();

		if (!(GamePlayMode & GP_NO_SAMPLES))
			sndInitFX();

		plInit();

		gfxCopyCollToXMS(128, &StdRP0InXMS);    // MenÅ nach StdRP0InXMS
		gfxCopyCollToXMS(129, &StdRP1InXMS);    // Bubbles etc nach StdRP1InXMS

		CurrentBackground = BGD_LONDON;

		return(1);
	}
	return(0);
}

static void InitData(void)
{
	ubyte MainData[TXT_KEY_LENGTH] = {0};
	ubyte BuildData[TXT_KEY_LENGTH] = {0};
	ubyte MainRel[TXT_KEY_LENGTH] = {0};
	ubyte BuildRel[TXT_KEY_LENGTH] = {0};
	ubyte RootPath[TXT_KEY_LENGTH] = {0};
	ubyte result = 0;

	dskGetRootPath(RootPath);

	sprintf(MainData,  "%s\\%s\\%s%s",RootPath, DATA_DIRECTORY,MAIN_DATA_NAME,GAME_DATA_EXT);
	sprintf(BuildData, "%s\\%s\\%s%s",RootPath, DATA_DIRECTORY,BUILD_DATA_NAME, GAME_DATA_EXT);

	sprintf(MainRel,  "%s\\%s\\%s%s",RootPath, DATA_DIRECTORY,MAIN_DATA_NAME,GAME_REL_EXT);
	sprintf(BuildRel, "%s\\%s\\%s%s",RootPath, DATA_DIRECTORY,BUILD_DATA_NAME, GAME_REL_EXT);

	#ifdef THECLOU_PROFIDISK
	#ifdef THECLOU_CDROM_VERSION
	sprintf(MainData,  "%s\\%s%s", DATA_DIRECTORY,MAIN_DATA_NAME,GAME_DATA_EXT);
	sprintf(BuildData, "%s\\%s%s", DATA_DIRECTORY,BUILD_DATA_NAME, GAME_DATA_EXT);

	sprintf(MainRel,  "%s\\%s%s", DATA_DIRECTORY,MAIN_DATA_NAME,GAME_REL_EXT);
	sprintf(BuildRel, "%s\\%s%s", DATA_DIRECTORY,BUILD_DATA_NAME, GAME_REL_EXT);
	#endif
	#endif

	txtReset(OBJECTS_TXT);

	if (dbLoadAllObjects(MainData, 0))
	{
		if (dbLoadAllObjects(BuildData, 0))
		{
			if (LoadRelations(MainRel, 0))
			{
				if (LoadRelations(BuildRel, 0))
				{
					InitTaxiLocations();
					result = 1;
				}
			}
		}
	}

	if (!result)
		NewErrorMsg(Disk_Defect, __FILE__, __func__, 1);
}

static void CloseData(void)
{
	RemRelations(0L, 0L);
	dbDeleteAllObjects(0L, 0L);
}

void tcSetPermanentColors(void)
{
	ubyte colortable[256][3];

	memset(&colortable[0][0], 0, 256*3);

	// RGB8

	colortable[248][0] = 116;
	colortable[248][1] = 224;
	colortable[248][2] = 142;

	colortable[249][0] = 224;
	colortable[249][1] = 224;
	colortable[249][2] = 224;

	colortable[250][0] = 55;
	colortable[250][1] = 12;
	colortable[250][2] = 32;

	colortable[251][0] = 128;
	colortable[251][1] = 29;
	colortable[251][2] = 75;

	colortable[252][0] = 170;
	colortable[252][1] = 170;
	colortable[252][2] = 170;

	colortable[253][0] = 104;
	colortable[253][1] = 104;
	colortable[253][2] = 104;

	colortable[254][0] = 0;
	colortable[254][1] = 0;
	colortable[254][2] = 0;

	gfxSetColorRange(248, 254);

	gfxChangeColors(NULL, 0, GFX_BLEND_UP, &colortable[0][0]);
}

static ubyte StartupMenu(void)
{
	LIST *menu = txtGoKey(MENU_TXT, "STARTUP_MENU");
	ulong activ;
	FILE *fh;
	ubyte line[TXT_KEY_LENGTH] = {0};
	ubyte ret = 0;

	ShowMenuBackground();

	#ifndef THECLOU_PROFIDISK
	#ifndef THECLOU_CDROM_VERSION
	//txtGetFirstLine(THECLOU_TXT, "BITTE_WARTEN_PC", line);
	strcpy(line, "Der Clou! Deluxe V" THECLOU_VERSION " (Standard)");
	#else
	//txtGetFirstLine(THECLOU_TXT, "BITTE_WARTEN_PC_CD_ROM", line);
	strcpy(line, "Der Clou! Deluxe V" THECLOU_VERSION " (Std. CD-ROM)");
	#endif
	#else
	#ifndef THECLOU_CDROM_VERSION
	//txtGetFirstLine(THECLOU_TXT, "BITTE_WARTEN_PC_PROFI", line);
	strcpy(line, "Der Clou! Deluxe V" THECLOU_VERSION " (Profidisk)");
	#else
	//txtGetFirstLine(THECLOU_TXT, "BITTE_WARTEN_PC_CD_ROM_PROFI", line);
	strcpy(line, "Der Clou! Deluxe V" THECLOU_VERSION " (Prof. CD-ROM)");
	#endif
	#endif

	PrintStatus(line);

	inpTurnFunctionKey(0);
	inpTurnESC(0);

	activ = Menu(menu, 7L, 0, NULL, 0L);

	inpTurnESC(1);
	inpTurnFunctionKey(1);

	switch (activ)
	{
		case 0:			// Neues Spiel
			InitData();
			ret = 1;
		break;

		case 1:			// Spiel laden
			txtReset(OBJECTS_TXT);
			if (tcLoadTheClou())
			{
				film->StartScene = SceneArgs.ReturnValue;
				ret = 1;
			}
		break;

		case 2:			// Beenden
			ret = 2;
		break;
	}

	RemoveList (menu);
	return(ret);
}

static void tcDo(void)
{
	ulong sceneId = SCENE_NEW_GAME;

	gfxChangeColors(l_wrp, 0, GFX_FADE_OUT, 0);
	gfxChangeColors(m_wrp, 0, GFX_FADE_OUT, 0);

	tcSetPermanentColors();

	// um die Menufarben auch einmal zu blenden:
	gfxShow(CurrentBackground, GFX_ONE_STEP|GFX_NO_REFRESH|GFX_BLEND_UP, 0, -1, -1);

	// Maus auf weiss - auf Verdacht 15 und 16 setzen
	gfxSetRGB(NULL, 15, 255, 255, 255);
	gfxSetRGB(NULL, 16, 255, 255, 255);

	SetBubbleType(SPEAK_BUBBLE);

	ShowMenuBackground();

	while (sceneId == SCENE_NEW_GAME)
	{
		ubyte ret = 0;

		if (!(GamePlayMode & GP_DEMO))
			InitStory(STORY_DAT);
		else
		    InitStory(STORY_DAT_DEMO);

		while (!ret)
			ret = StartupMenu();

		if (ret != 2)
		{
			if (GamePlayMode & GP_FULL_ENV)
				SetFullEnvironment();

			sceneId = PlayStory();
		}
		else
		    sceneId = SCENE_THE_END;

		CloseData();
		CloseStory();
	}
}

static int ArgsIndex(const char *str, int argc, char **argv)
{
	int i;
	for (i = 1; i < argc; i++)
	{
		if (!strnicmp(argv[i], str, strlen(str))) return(i);
	}
	return(0);
}

int SDL_main(int argc, char **argv)
{
	remove(LOG_FILENAME);

	if (SDL_Init(SDL_INIT_TIMER)) return(0);

	char result[256], res;
	long i;
	int t;

	strcpy(prgname, argv[0]);

	rndInit();

	// KB Buffer lîschen
	inpClearKbBuffer();

	strcpy(result, argv[0]);

	for (i = strlen(result) - 1; i > 0; i--)
	{
		if (result[i] == '\\')
		{
			result[i] = '\0';
			break;
		}
	}

	if (argc > 1)
	{
		/* Options */

		if (ArgsIndex("full", argc, argv)||ArgsIndex("screen", argc, argv))
			bFullscreen = 1;
		else if (ArgsIndex("small", argc, argv)||ArgsIndex("original", argc, argv))
			bScale2x = 0;

		/* Cheats/Debugging (start with '+') */

		if (ArgsIndex("+demo", argc, argv))
			GamePlayMode |= GP_DEMO|GP_STORY_OFF;

		if (ArgsIndex("+storyoff", argc, argv))
			GamePlayMode |= GP_STORY_OFF;

		if (ArgsIndex("+fullenv", argc, argv))
			GamePlayMode |= GP_FULL_ENV;

		if (ArgsIndex("+leveldesign", argc, argv))
			GamePlayMode |= GP_LEVEL_DESIGN;

		if (ArgsIndex("+guarddesign", argc, argv))
			GamePlayMode |= GP_GUARD_DESIGN;

		if (ArgsIndex("+nosamples", argc, argv))
			GamePlayMode |= GP_NO_SAMPLES;

		if (ArgsIndex("+nocollision", argc, argv))
			GamePlayMode |= GP_COLLISION_CHECKING_OFF;

		if (ArgsIndex("+showrooms", argc, argv))
			GamePlayMode |= GP_SHOW_ROOMS;
	}

	// und den Pfad fÅr BuildPathName setzen!
	dskSetRootPath(result);

	if (res = tcInit())
		tcDo();

	tcDone();

	SDL_Quit();

	if (!res)
	{
		Log("Error: %s|%s|tcInit", __FILE__, __func__);
	}
	return(0);
}
