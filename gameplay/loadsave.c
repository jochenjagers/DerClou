/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "gameplay/gp.h"
#include "gameplay/gp_app.h"
#include "organisa/organisa.h"
#include "port/port.h"

void tcSaveTheClou(int bAutoSave)
{
    char line[TXT_KEY_LENGTH];
    char location[TXT_KEY_LENGTH];
    char date[TXT_KEY_LENGTH];
    char RootPath[TXT_KEY_LENGTH];
    char pathname[TXT_KEY_LENGTH];
    LIST *games = (LIST *)CreateList(0);
    NODE *game = NULL;
    uword activ = 0;
    Player player = (Player)dbGetObject(Player_Player_1);

    /* in welche Datei ?? */
    if (!bAutoSave)
    {
        ShowMenuBackground();
    }
    txtGetFirstLine(THECLOU_TXT, "SaveGame", line);

    player->CurrScene = film->act_scene->EventNr;
    player->CurrDay = GetDay;
    player->CurrMinute = GetMinute;
    player->CurrLocation = GetLocation;

    dskGetRootPath(RootPath);

    dskBuildPathName(DATADISK_DIRECTORY, GAMES_LIST_TXT, pathname);
    if (ReadList(games, 0L, pathname, 0))
    {
        if (!bAutoSave)
        {
            inpTurnESC(1);
            inpTurnFunctionKey(0);
            activ = (uword)Menu(games, 15L, 0, NULL, 0L);
            inpTurnFunctionKey(1);
        }
        else
        {
            activ = 0;
        }

        /* Name erstellen */

        if (activ != GET_OUT)
        {
            strcpy(location, GetCurrLocName());
            BuildDate(GetDay, txtGetLanguage(), date);

            tcCutName(location, (ubyte)' ', 15);

            strcat(location, ", ");
            strcat(location, date);

            sprintf(date, "(%d)", activ + 1);
            strcat(location, date);

            /* Games.list abspeichern */

            game = (NODE *)CreateNode(0L, 0L, location);

            ReplaceNode(games, NODE_NAME(GetNthNode(games, activ)), game);

            ShowMenuBackground();
            txtGetFirstLine(THECLOU_TXT, "SAVING", line);
            PrintStatus(line);

            WriteList(games, pathname, 0);

            /*
            // Speichern von tcMain
            sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, MAIN_DATA_NAME, activ, GAME_DATA_EXT);
            dbSaveAllObjects (line, DB_tcMain_OFFSET, DB_tcMain_SIZE, 0);

            sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, MAIN_DATA_NAME, activ, GAME_REL_EXT);
            SaveRelations(line ,DB_tcMain_OFFSET, DB_tcMain_SIZE, 0);

            // Speichern von tcBuild
            sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, BUILD_DATA_NAME, activ, GAME_DATA_EXT);
            dbSaveAllObjects (line, (uint32_t) (DB_tcBuild_OFFSET), (uint32_t) (DB_tcBuild_SIZE), 0);

            sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, BUILD_DATA_NAME, activ, GAME_REL_EXT);
            SaveRelations(line ,(uint32_t) DB_tcBuild_OFFSET, (uint32_t) DB_tcBuild_SIZE, 0);

            // Speichern der Story
            sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, STORY_DATA_NAME, activ, GAME_DATA_EXT);
            tcSaveChangesInScenes(line);
            */

            /* 2014-06-25 templer */

            // Speichern von tcMain
            sprintf(line, "%s%d%s", MAIN_DATA_NAME, activ, GAME_DATA_EXT);
            dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
            dbSaveAllObjects(pathname, DB_tcMain_OFFSET, DB_tcMain_SIZE, 0);

            sprintf(line, "%s%d%s", MAIN_DATA_NAME, activ, GAME_REL_EXT);
            dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
            SaveRelations(pathname, DB_tcMain_OFFSET, DB_tcMain_SIZE, 0);

            // Speichern von tcBuild
            sprintf(line, "%s%d%s", BUILD_DATA_NAME, activ, GAME_DATA_EXT);
            dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
            dbSaveAllObjects(pathname, (uint32_t)DB_tcBuild_OFFSET, (uint32_t)DB_tcBuild_SIZE, 0);

            sprintf(line, "%s%d%s", BUILD_DATA_NAME, activ, GAME_REL_EXT);
            dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
            SaveRelations(pathname, (uint32_t)DB_tcBuild_OFFSET, (uint32_t)DB_tcBuild_SIZE, 0);

            // Speichern der Story
            sprintf(line, "%s%d%s", STORY_DATA_NAME, activ, GAME_DATA_EXT);
            dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
            tcSaveChangesInScenes(pathname);
        }
    }

    RemoveList(games);
}

ubyte tcLoadIt(char activ)
{
    char line[TXT_KEY_LENGTH];
    ubyte loaded = 0;
    char RootPath[TXT_KEY_LENGTH];
    char pathname[TXT_KEY_LENGTH];

    dskGetRootPath(RootPath);

    ShowMenuBackground();
    txtGetFirstLine(THECLOU_TXT, "LOADING", line);
    PrintStatus(line);

    /* alte Daten löschen */

    tcResetOrganisation();

    RemRelations((uint32_t)DB_tcMain_OFFSET, (uint32_t)DB_tcMain_SIZE);
    RemRelations((uint32_t)DB_tcBuild_OFFSET, (uint32_t)DB_tcBuild_SIZE);

    dbDeleteAllObjects((uint32_t)DB_tcMain_OFFSET, (uint32_t)DB_tcMain_SIZE);
    dbDeleteAllObjects((uint32_t)DB_tcBuild_OFFSET, (uint32_t)DB_tcBuild_SIZE);

    /* neue Daten laden ! */

    txtReset(OBJECTS_TXT);

    /*
    sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, MAIN_DATA_NAME, (int)activ, GAME_DATA_EXT);
    if(dbLoadAllObjects (line, 0))
    {
            sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, BUILD_DATA_NAME, (int)activ, GAME_DATA_EXT);

            if(dbLoadAllObjects (line, 0))
            {
                    sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, MAIN_DATA_NAME, (int)activ,
    GAME_REL_EXT);

                    if(LoadRelations (line, 0))
                    {
                            sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, BUILD_DATA_NAME, (int)activ,
    GAME_REL_EXT);

                            if(LoadRelations (line, 0))
                            {
                                    sprintf(line, "%s"DIR_SEPARATOR"%s%d%s", DATADISK_DIRECTORY, STORY_DATA_NAME,
    (int)activ, GAME_DATA_EXT);

                                    if(tcLoadChangesInScenes(line))
                                            loaded = 1;
                            }
                    }
            }
    }
    */

    /* 2014-06-25 templer */

    sprintf(line, "%s%d%s", MAIN_DATA_NAME, (int)activ, GAME_DATA_EXT);
    dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
    if (dbLoadAllObjects(pathname, 0))
    {
        sprintf(line, "%s%d%s", BUILD_DATA_NAME, (int)activ, GAME_DATA_EXT);
        dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
        if (dbLoadAllObjects(pathname, 0))
        {
            sprintf(line, "%s%d%s", MAIN_DATA_NAME, (int)activ, GAME_REL_EXT);
            dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
            if (LoadRelations(pathname, 0))
            {
                sprintf(line, "%s%d%s", BUILD_DATA_NAME, (int)activ, GAME_REL_EXT);
                dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
                if (LoadRelations(pathname, 0))
                {
                    sprintf(line, "%s%d%s", STORY_DATA_NAME, (int)activ, GAME_DATA_EXT);
                    dskBuildPathName(DATADISK_DIRECTORY, line, pathname);
                    if (tcLoadChangesInScenes(pathname)) loaded = 1;
                }
            }
        }
    }

    return loaded;
}

ubyte tcBackToStartupMenu(void)
{
    /* clear old data */

    tcResetOrganisation();

    RemRelations((uint32_t)DB_tcMain_OFFSET, (uint32_t)DB_tcMain_SIZE);
    RemRelations((uint32_t)DB_tcBuild_OFFSET, (uint32_t)DB_tcBuild_SIZE);

    dbDeleteAllObjects((uint32_t)DB_tcMain_OFFSET, (uint32_t)DB_tcMain_SIZE);
    dbDeleteAllObjects((uint32_t)DB_tcBuild_OFFSET, (uint32_t)DB_tcBuild_SIZE);

    return 0;
}

ubyte tcLoadTheClou(void)
{
    char line[TXT_KEY_LENGTH], loaded = 0;
    LIST *games = (LIST *)CreateList(0);
    LIST *origin = (LIST *)CreateList(0);
    uint32_t activ = 0;
    Player player = NULL;
    char pathname1[TXT_KEY_LENGTH];
    char pathname2[TXT_KEY_LENGTH];

    dskBuildPathName(DATADISK_DIRECTORY, GAMES_LIST_TXT, pathname1);
    dskBuildPathName(DATADISK_DIRECTORY, GAMES_ORIG_TXT, pathname2);

    if (ReadList(games, 0L, pathname1, 0) && ReadList(origin, 0L, pathname2, 0))
    {
        ShowMenuBackground();
        txtGetFirstLine(THECLOU_TXT, "LoadAGame", line);

        inpTurnFunctionKey(0);
        inpTurnESC(1);
        activ = (uint32_t)Menu(games, 15L, 0, NULL, 0L);
        inpTurnFunctionKey(1);

        if ((activ != GET_OUT) &&
            (strcmp(NODE_NAME(GetNthNode(games, (int32_t)activ)), NODE_NAME(GetNthNode(origin, (int32_t)activ)))))
        {
            loaded = tcLoadIt((ubyte)activ);
        }
        else
        {
            ShowMenuBackground();

            txtGetFirstLine(THECLOU_TXT, "NOT_LOADING", line);
            PrintStatus(line);
            inpWaitFor(INP_LBUTTONP);

            ShowMenuBackground();
            SetLocation(-1);

            RemoveList(games);
            RemoveList(origin);

            if ((player = (Player)dbGetObject(Player_Player_1))) /* MOD 04-02 */
            {
                player->CurrScene = film->act_scene->EventNr;

                SceneArgs.ReturnValue = film->act_scene->EventNr;
            }

            return 0;
        }
    }
    else
        NewErrorMsg(Disk_Defect, __FILE__, __func__, 1);

    tcRefreshAfterLoad(loaded);

    RemoveList(games);
    RemoveList(origin);

    return loaded;
}

void tcRefreshAfterLoad(ubyte loaded)
{
    Player player = (Player)dbGetObject(Player_Player_1); /* muß hier geholt werden -> sonst alte Adresse */

    if (!loaded)
    {
        if (player) player->CurrScene = 0L;
        NewErrorMsg(Disk_Defect, __FILE__, __func__, 2);
    }
    else
    {
        if (player)
        {
            SetDay(player->CurrDay);
            SetMinute(player->CurrMinute);
            SetLocation(-1); /* auf alle Fälle ein Refresh! */

            SceneArgs.ReturnValue = GetLocScene(player->CurrLocation)->EventNr;
        }
    }
}

ubyte tcSaveChangesInScenes(char *fileName)
{
    int32_t i = 0;
    ubyte back = 0;
    FILE *file = NULL;

    if ((file = dskOpen(fileName, "w", 0)))
    {
        fprintf(file, "%u\n", film->EnabledChoices);

        for (i = 0; i < film->AmountOfScenes; i++)
        {
            fprintf(file, "%ld\n", film->gameplay[i].EventNr);
            fprintf(file, "%d\n", film->gameplay[i].Geschehen);
        }

        dskClose(file);
        back = 1;
    }

    return (back);
}

ubyte tcLoadChangesInScenes(char *fileName)
{
    int32_t i = 0;
    char buffer[TXT_KEY_LENGTH];
    ubyte back = 1;
    uint32_t eventNr = 0, count = 0;
    FILE *file = NULL;
    struct Scene *sc = NULL;

    if ((file = dskOpen(fileName, "r", 0)))
    {
        dskGets(buffer, TXT_KEY_LENGTH - 1, file);
        SetEnabledChoices(atol(buffer));

        for (i = 0; i < film->AmountOfScenes; i++)
        {
            dskGets(buffer, TXT_KEY_LENGTH - 1, file);
            eventNr = atol(buffer);
            dskGets(buffer, TXT_KEY_LENGTH - 1, file);
            count = atol(buffer);

            if ((sc = GetScene(eventNr)))
                sc->Geschehen = (uword)count;
            else
                back = 0;
        }
        dskClose(file);
    }
    else
        back = 0;

    return (back);
}
