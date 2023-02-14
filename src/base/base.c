/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "base/base.h"

#include "SDL.h"
#include "intro/intro.h"

// cheat function
#include "base/fullenv.c"
#include "theclou.h"

Configuration_t Config;

void *StdBuffer1 = 0;

char prgname[255];

/************************************************/
char bProfidisk = 1;  // default = 1
/************************************************/

void tcClearStdBuffer(void *p_Buffer)
{
    int32_t i = 0;
    char *p = (char *)p_Buffer;
    for (i = 0; i < STD_BUFFER1_SIZE; i++) p[i] = 0;
}

static void tcDone(void)
{
    plDone();
    sndDoneFX();  // Attention!
    sndDone();

    RemoveAudio();

    dbDone();
    CloseAnimHandler();
    txtDone();
    inpCloseAllInputDevs();
    gfxDone();

    if (StdBuffer1) MemFree(StdBuffer1, STD_BUFFER1_SIZE);

    if (MemGetAllocated())
    {
        Log("WARNING! Memory Leak (%d bytes)", MemGetAllocated());

        Log("Total memory used: %d bytes", MemGetMaxAllocated());
    }

    pcErrClose();
}

/* 2014-06-27 templer
Suggestion for automatic detection of the language of the game
*/
static ubyte detectLanguage(void)
{
    char languageMark[4] = {'E', 'D', 'F', 'S'};
    int count = 0;

    for (count = 0; count < sizeof(languageMark) / sizeof(languageMark[0]); count++)
    {
        char fileName[TXT_KEY_LENGTH];
        char fileWithPath[TXT_KEY_LENGTH];
        FILE *p_File = NULL;

        sprintf(fileName, "tcmaine%c.txt", languageMark[count]);
        dskBuildPathName(TEXT_DIRECTORY, fileName, fileWithPath);
        if ((p_File = fopen(fileWithPath, "r")))
        {
            fclose(p_File);
            return (ubyte)count;
        }
    }

    Log("couldn't detect language of the game!");
    NewErrorMsg(Internal_Error, __FILE__, __func__, 1);
    return 0;
}

static int tcInit(void)
{
    pcErrOpen(ERR_NO_OUTPUT, tcDone, NULL);

    StdBuffer1 = (void *)MemAlloc(STD_BUFFER1_SIZE);

    if (StdBuffer1)
    {
        gfxInit();
        inpOpenAllInputDevs();

        txtInit(detectLanguage()); /* 2014-06-27 templer */

        InitAnimHandler();

        dbInit();

        InitAudio();

        sndInit();

        if (!(GamePlayMode & GP_NO_SAMPLES)) sndInitFX();

        plInit();

        gfxCopyCollToXMS(128, &StdRP0InXMS);  // Menü nach StdRP0InXMS
        gfxCopyCollToXMS(129, &StdRP1InXMS);  // Bubbles etc nach StdRP1InXMS

        CurrentBackground = BGD_LONDON;

        return 1;
    }
    return 0;
}

static void InitData(void)
{
    char MainData[TXT_KEY_LENGTH] = {0};
    char BuildData[TXT_KEY_LENGTH] = {0};
    char MainRel[TXT_KEY_LENGTH] = {0};
    char BuildRel[TXT_KEY_LENGTH] = {0};
    ubyte result = 0;

    dskBuildPathName(DATA_DIRECTORY, MAIN_DATA_NAME GAME_DATA_EXT, MainData);
    dskBuildPathName(DATA_DIRECTORY, BUILD_DATA_NAME GAME_DATA_EXT, BuildData);

    dskBuildPathName(DATA_DIRECTORY, MAIN_DATA_NAME GAME_REL_EXT, MainRel);
    dskBuildPathName(DATA_DIRECTORY, BUILD_DATA_NAME GAME_REL_EXT, BuildRel);

    txtReset(OBJECTS_TXT);

    if (dbLoadAllObjects((char *)MainData, 0))
    {
        if (dbLoadAllObjects((char *)BuildData, 0))
        {
            if (LoadRelations((char *)MainRel, 0))
            {
                if (LoadRelations((char *)BuildRel, 0))
                {
                    InitTaxiLocations();
                    result = 1;
                }
            }
        }
    }

    if (!result) NewErrorMsg(Disk_Defect, __FILE__, __func__, 1);
}

static void CloseData(void)
{
    RemRelations(0L, 0L);
    dbDeleteAllObjects(0L, 0L);
}

void tcSetPermanentColors(void)
{
    ubyte colortable[256][3];

    memset(&colortable[0][0], 0, 256 * 3);

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
    uint32_t activ = 0;
    char line[TXT_KEY_LENGTH] = COSP_TITLE " v" COSP_VERSION;
    ubyte ret = 0;

    ShowMenuBackground();

    // 2014-07-01 templer
    if (!bProfidisk)
    {
        strcat(line, " (Std. CD-ROM)");
    }
    else
    {
        strcat(line, " (Prof. CD-ROM)");
    }

    PrintStatus(line);

    inpTurnFunctionKey(0);
    inpTurnESC(0);

    activ = Menu(menu, 7L, 0, NULL, 0L);

    inpTurnESC(1);
    inpTurnFunctionKey(1);

    switch (activ)
    {
        case 0:  // New Game
            InitData();
            ret = 1;
            break;

        case 1:  // Load Game
            txtReset(OBJECTS_TXT);
            if (tcLoadTheClou())
            {
                film->StartScene = SceneArgs.ReturnValue;
                ret = 1;
            }
            break;

        case 2:  // Quit Game
            ret = 2;
            break;
    }

    RemoveList(menu);
    return ret;
}

static void tcDo(void)
{
    uint32_t sceneId = SCENE_NEW_GAME;

    gfxChangeColors(l_wrp, 0, GFX_FADE_OUT, 0);
    gfxChangeColors(m_wrp, 0, GFX_FADE_OUT, 0);

    tcSetPermanentColors();

    // to fade in the menu colors once:
    gfxShow(CurrentBackground, GFX_ONE_STEP | GFX_NO_REFRESH | GFX_BLEND_UP, 0, -1, -1);

    // set mouse cursor to white - on spec set color 15 and 16 to white
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

        while (!ret) ret = StartupMenu();

        if (ret != 2)
        {
            if (GamePlayMode & GP_FULL_ENV) SetFullEnvironment();

            sceneId = PlayStory();
        }
        else
            sceneId = SCENE_THE_END;

        CloseData();
        CloseStory();
    }
}

// remove tabs and spaces
static char *trimstr(char *str)
{
    char *p = NULL;

    str += strspn(str, " \t");
    while ((p = strrchr(str, ' ')))
    {
        *p = '\0';
    }
    while ((p = strrchr(str, '\t')))
    {
        *p = '\0';
    }
    return str;
}

static void loadConfig(const char *rootPath)
{
    char config_file[256];
    char line[256];
    char *pParam = NULL;
    char *pValue = NULL;
    char *p = NULL;
    FILE *file = NULL;

    // set defaults
    Config.gfxScreenWidth = 320;
    Config.gfxScreenHeight = 200;
    Config.gfxFullScreen = 0;
    Config.gfxNoFontShadow = 0;
    Config.NoIntro = 0;
    Config.MusicVolume = SND_MAX_VOLUME;
    Config.SfxVolume = SND_MAX_VOLUME;
    Config.VoiceVolume = SND_MAX_VOLUME;
    Config.UseJoystick = 0;

    sprintf(config_file, "%s" DIR_SEPARATOR "%s", rootPath, "cosp.cfg");
    file = dskOpen(config_file, "rb", 0);
    if (!file)
    {
        Log("Failed to open cosp.cfg");
        return;
    }

    while (dskGets(line, sizeof(line) - 1, file))
    {
        line[sizeof(line) - 1] = '\0';  // just to be safe

        if ((p = strchr(line, ';')))
        {
            *p = '\0';
        }
        if ((p = strchr(line, '#')))
        {
            *p = '\0';
        }

        if ((pValue = strchr(line, '=')))
        {
            *pValue++ = '\0';
            pValue = trimstr(pValue);
            if (*pValue == '\0')
            {  // no value after =
                continue;
            }
            pParam = trimstr(line);

            if (!strcasecmp(pParam, "screenwidth"))
            {
                Config.gfxScreenWidth = atoi(pValue);
            }
            else if (!strcasecmp(pParam, "screenheight"))
            {
                Config.gfxScreenHeight = atoi(pValue);
            }
            else if (!strcasecmp(pParam, "fullscreen"))
            {
                Config.gfxFullScreen = atoi(pValue) ? 1 : 0;
            }
            else if (!strcasecmp(pParam, "scale2x"))
            {
                Config.gfxScaleNx = atoi(pValue) ? 1 : 0;
            }
            else if (!strcasecmp(pParam, "soundvolume"))
            {
                Config.SfxVolume = atoi(pValue);
            }
            else if (!strcasecmp(pParam, "musicvolume"))
            {
                Config.MusicVolume = atoi(pValue);
            }
            else if (!strcasecmp(pParam, "voicevolume"))
            {
                Config.VoiceVolume = atoi(pValue);
            }
            else if (!strcasecmp(pParam, "nofontshadow"))
            {
                Config.gfxNoFontShadow = atoi(pValue) ? 1 : 0;
            }
            else if (!strcasecmp(pParam, "nointro"))
            {
                Config.NoIntro = atoi(pValue) ? 1 : 0;
            }
            else if (!strcasecmp(pParam, "usejoystick"))
            {
                Config.UseJoystick = atoi(pValue);
            }
            else if (!strcasecmp(pParam, "demo") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_DEMO | GP_STORY_OFF;
            }
            else if (!strcasecmp(pParam, "storyoff") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_STORY_OFF;
            }
            else if (!strcasecmp(pParam, "fullenv") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_FULL_ENV;
            }
            else if (!strcasecmp(pParam, "leveldesign") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_LEVEL_DESIGN;
            }
            else if (!strcasecmp(pParam, "guarddesign") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_GUARD_DESIGN;
            }
            else if (!strcasecmp(pParam, "nosamples") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_NO_SAMPLES;
            }
            else if (!strcasecmp(pParam, "nocollision") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_COLLISION_CHECKING_OFF;
            }
            else if (!strcasecmp(pParam, "showrooms") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_SHOW_ROOMS;
            }
            else if (!strcasecmp(pParam, "moremoney") && (atoi(pValue) == 42))
            {
                GamePlayMode |= GP_MORE_MONEY;
            }
        }
    }

    dskClose(file);
}

int tcStartGame(int argc, char **argv)
{
    char result[256];
    int32_t res = 0;
    unsigned long i = 0;

#ifdef THECLOU_DEBUG_ALLOC
    MemInit();
#endif

    remove(LOG_FILENAME);

    if (SDL_InitSubSystem(SDL_INIT_TIMER)) return (0);  // 2015-01-10 LucyG : SDL_Init is already called earlier

    strcpy(prgname, argv[0]);

    rndInit();

    strcpy(result, argv[0]);

    for (i = strlen(result) - 1; i > 0; i--)
    {
        if (result[i] == DIR_SEPARATOR_CHAR)
        {
            result[i] = '\0';
            break;
        }
    }

    // und den Pfad für BuildPathName setzen!
    dskSetRootPath(CMAKE_INSTALL_PREFIX);
    dskInitUserDataPath();

    loadConfig(result);

    if (Config.gfxScreenWidth < 320)
    {
        Config.gfxScreenWidth = 320;
    }
    if (Config.gfxScreenHeight < 200)
    {
        Config.gfxScreenHeight = 200;
    }
    if (Config.MusicVolume < 0)
    {
        Config.MusicVolume = 0;
    }
    else if (Config.MusicVolume > SND_MAX_VOLUME)
    {
        Config.MusicVolume = SND_MAX_VOLUME;
    }
    if (Config.SfxVolume < 0)
    {
        Config.SfxVolume = 0;
    }
    else if (Config.SfxVolume > SND_MAX_VOLUME)
    {
        Config.SfxVolume = SND_MAX_VOLUME;
    }
    if (Config.VoiceVolume < 0)
    {
        Config.VoiceVolume = 0;
    }
    else if (Config.VoiceVolume > SND_MAX_VOLUME)
    {
        Config.VoiceVolume = SND_MAX_VOLUME;
    }

    if ((res = tcInit()))
    {
        if (!Config.NoIntro)
        {
            tcIntro();  // 2014-06-29 LucyG
        }
        else
        {
            sndPlaySound("title.bk", 0);
        }
        tcDo();
    }

    tcDone();

    SDL_Quit();

    if (!res)
    {
        Log("Error: %s|%s|tcInit", __FILE__, __func__);
    }

#ifdef THECLOU_DEBUG_ALLOC
    MemQuit();
#endif
    return 0;
}
