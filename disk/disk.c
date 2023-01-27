/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "disk\disk.h"
#include "disk\disk_e.h"

char RootPathName[256];


void dskSetRootPath(const char *newRootPath)
{
	strcpy(RootPathName, newRootPath);
}

char *dskGetRootPath(char *result)
{
	strcpy(result, RootPathName);
	return result;
}

FILE *dskOpen(const char *puch_Pathname, const char *puch_Mode, uword us_DiskId)
{
	FILE *p_File = NULL;

	us_DiskId = 0; /* just to have no warnings */

	//Log("%s|%s: %s(%s)", __FILE__, __func__, puch_Pathname, puch_Mode);

	if (!(p_File = fopen(puch_Pathname, puch_Mode)))
	{
		NewErrorMsg(Disk_Defect, __FILE__, __func__, ERR_DISK_OPEN_FAILED);
	}

	return(p_File);
}

long dskLoad(const char *puch_Pathname, void *p_MemDest, uword us_DiskId)
{
	FILE *p_File = NULL;
	long ul_SizeOfFile;

	if (ul_SizeOfFile = dskFileLength(puch_Pathname))
	{
		if (p_File = dskOpen(puch_Pathname, "rb", us_DiskId))
		{
			dskRead(p_File, p_MemDest, ul_SizeOfFile);
			dskClose(p_File);
			return(ul_SizeOfFile);
		}
	}
	return(0);
}

void dskBuildPathName(const char *puch_Directory, const char *puch_Filename, char *puch_Result)
{
	sprintf(puch_Result, "%s\\%s\\%s", RootPathName, puch_Directory, puch_Filename);
}

long dskFileLength (const char *puch_Pathname)
{
	long l_Size = 0;
	FILE *file;

	if (file = fopen(puch_Pathname, "rb"))
	{
		fseek(file, 0, SEEK_END);
		l_Size = ftell(file);
		fclose(file);
	}

	return l_Size;
}

void dskClose(FILE *p_File)
{
	if (p_File)
		fclose(p_File);
}

void dskWrite(FILE *p_File, void *p_MemSource, ulong ul_Size)
{
	if (ul_Size != fwrite(p_MemSource, 1, ul_Size, p_File))
	{
		NewErrorMsg(Disk_Defect, __FILE__, __func__, ERR_DISK_WRITE_FAILED);
	}
}

void dskRead(FILE *p_File, void *p_MemDest, ulong ul_Size)
{
	ulong ul_Read;
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
	if (fgetc(p_File) == EOF) return(-1);
	fseek(p_File, -1, SEEK_CUR);
	return(0);
}

// fgets() has this CR/LF problem
char *dskGets(char *text, int n, FILE *file)
{
	char *p;
	if (fgets(text, n, file)) {
		if (p = strrchr(text, 13)) {
			*p = '\0';
		}
		if (p = strrchr(text, 10)) {
			*p = '\0';
		}
		return(text);
	}
	return(NULL);
}
