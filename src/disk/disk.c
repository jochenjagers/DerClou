/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "disk/disk.h"

#include <SDL.h>
#include <ctype.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#include "disk/disk_e.h"

#if (defined(_WIN32) || defined(__WIN32__))
#include <direct.h> /* _mkdir */
#define mkdir(A, B) _mkdir(A)
#endif

static char RootPathName[FILENAME_MAX] = {0};
static char UserPathName[FILENAME_MAX] = {0};

static const char *USER_GAME_DATA_DIR = "DerClou";

void dskSetRootPath(const char *newRootPath) { strcpy(RootPathName, newRootPath); }

void dskMakeDirectory(const char *path)
{
    int result = mkdir(path, 0755);
    if (result && errno != EEXIST)
    {
        Log("The directory \"%s\" could not be created.", path);
    }
}

void dskInitUserDataPath(void)
{
    if (UserPathName[0] == '\0')
    {
        char *path = SDL_GetPrefPath("", USER_GAME_DATA_DIR);
        if (!path)
        {
            NewErrorMsg(Disk_Defect, __FILE__, __func__, 1);
        }
        strncpy(UserPathName, path, FILENAME_MAX);
        SDL_free(path);
        dskMakeDirectory(UserPathName);
        char result[FILENAME_MAX];
        dskBuildPathNameUser(DATADISK_DIRECTORY, "", &result[0]);
        dskMakeDirectory(result);
    }
}

char *dskGetRootPath(char *result)
{
    strcpy(result, RootPathName);
    return result;
}

/* 2019-11-22 LucyG  */
static int dskModfilesPathName(const char *pszPath, char *pszModfilesPath)
{
    int rootPathLen = strlen(RootPathName);
    if (!memcmp(pszPath, RootPathName, rootPathLen))
    {
        rootPathLen++;  // also skip the separator
        dskBuildPathName(MODFILES_DIRECTORY, &pszPath[rootPathLen], pszModfilesPath);
        return 1;
    }
    return 0;
}

FILE *dskOpen(const char *puch_Pathname, const char *puch_Mode, uword us_DiskId)
{
    FILE *p_File = NULL;

    /* 2019-11-22 LucyG : Modfiles support */
    if (puch_Mode[0] == 'r')
    {
        static char szModfilesPath[260];
        if (dskModfilesPathName(puch_Pathname, szModfilesPath))
        {
            /* try to open modded file */
            p_File = fopen(szModfilesPath, puch_Mode);
        }
    }

    if (!p_File)
    {
        /* open original file */
        p_File = fopen(puch_Pathname, puch_Mode);
    }

    if (!p_File)
    {
        Log("Could not open file %s\n", puch_Pathname);
        NewErrorMsg(Disk_Defect, __FILE__, __func__, ERR_DISK_OPEN_FAILED);
    }

    return p_File;
}

int32_t dskLoad(const char *puch_Pathname, void *p_MemDest)
{
    FILE *p_File = NULL;
    int32_t ul_SizeOfFile = 0;

    if ((ul_SizeOfFile = dskFileLength(puch_Pathname)))
    {
        if ((p_File = dskOpen(puch_Pathname, "rb", 0)))
        {
            dskRead(p_File, p_MemDest, ul_SizeOfFile);
            dskClose(p_File);
            return ul_SizeOfFile;
        }
    }
    return 0;
}

static void dskBuildPathNameGeneric(const char *puch_Directory, const char *puch_Filename, char *puch_Result,
                                    const char *rootPath)
{
    sprintf(puch_Result, "%s" DIR_SEPARATOR "%s" DIR_SEPARATOR "%s", rootPath, puch_Directory, puch_Filename);
    // convert all chars after rootPath to upper case as all data file names are upper case
    char *cur = &puch_Result[strlen(rootPath)];
    while (*cur != '\0')
    {
        *cur = (char)toupper(*cur);
        ++cur;
    }
}

void dskBuildPathName(const char *puch_Directory, const char *puch_Filename, char *puch_Result)
{
    dskBuildPathNameGeneric(puch_Directory, puch_Filename, puch_Result, RootPathName);
}

void dskBuildPathNameUser(const char *puch_Directory, const char *puch_Filename, char *puch_Result)
{
    dskBuildPathNameGeneric(puch_Directory, puch_Filename, puch_Result, UserPathName);
}

int32_t dskFileLength(const char *puch_Pathname)
{
    int32_t l_Size = 0;
    FILE *file = NULL;

    if ((file = fopen(puch_Pathname, "rb")))
    {
        fseek(file, 0, SEEK_END);
        l_Size = (int32_t)ftell(file);
        fclose(file);
    }

    return l_Size;
}

void dskClose(FILE *p_File)
{
    if (p_File) fclose(p_File);
}

void dskWrite(FILE *p_File, void *p_MemSource, uint32_t ul_Size)
{
    if (ul_Size != fwrite(p_MemSource, 1, ul_Size, p_File))
    {
        NewErrorMsg(Disk_Defect, __FILE__, __func__, ERR_DISK_WRITE_FAILED);
    }
}

void dskRead(FILE *p_File, void *p_MemDest, uint32_t ul_Size)
{
    uint32_t ul_Read = 0;
    ul_Read = fread(p_MemDest, 1, ul_Size, p_File);
    if (ul_Read != ul_Size)
    {
        NewErrorMsg(Disk_Defect, __FILE__, __func__, ERR_DISK_READ_FAILED);
    }
}

// feof() didn't seem to work
// as expected...
int dskIsEOF(FILE *p_File)
{
    if (fgetc(p_File) == EOF) return (-1);
    fseek(p_File, -1, SEEK_CUR);
    return 0;
}

// fgets() has this CR/LF problem
char *dskGets(char *text, int n, FILE *file)
{
    char *p = NULL;
    if (fgets(text, n, file))
    {
        if ((p = strrchr(text, 13)))
        {
            *p = '\0';
        }
        if ((p = strrchr(text, 10)))
        {
            *p = '\0';
        }
        return text;
    }
    return NULL;
}
