/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "disk\disk.h"
#include "disk\disk.eh"

char RootPathName[256];

#ifdef THECLOU_CDROM_VERSION
char CDDrive[8] = "X:\\";
#endif

void dskSetRootPath(char *newRootPath)
{
	strcpy(RootPathName, newRootPath);
}

char *dskGetRootPath(char *result)
{
	strcpy(result, RootPathName);
	return result;
}

FILE *dskOpen(ubyte *puch_Pathname, ubyte *puch_Mode, uword us_DiskId)
{
	FILE *p_File = NULL;

	us_DiskId = 0; /* just to have no warnings */

	if (!(p_File = fopen(puch_Pathname, puch_Mode)))
	{
		Log(puch_Pathname);
		NewErrorMsg(Disk_Defect, __FILE__, __func__, ERR_DISK_OPEN_FAILED);
	}

	return(p_File);
}

void dskLoad(ubyte *puch_Pathname, void *p_MemDest, uword us_DiskId)
{
	FILE *p_File = NULL;
	long ul_SizeOfFile;

	if (ul_SizeOfFile = dskFileLength(puch_Pathname))
	{
		if (p_File = dskOpen(puch_Pathname, "rb", us_DiskId))
		{
			dskRead(p_File, p_MemDest, ul_SizeOfFile);
			dskClose(p_File);
		}
	}
}

void dskBuildPathName(ubyte *puch_Directory, ubyte *puch_Filename, ubyte *puch_Result)
{
	#ifdef THECLOU_CDROM_VERSION
	{
		if (!strcmp(puch_Directory, DATADISK))
			sprintf(puch_Result, "%s\\%s", puch_Directory, puch_Filename);
		else
		#ifdef THECLOU_PROFIDISK
		if (!strcmp(puch_Directory, TEXT_DIRECTORY))
			sprintf(puch_Result, "%s\\%s", puch_Directory, puch_Filename);
		else
			sprintf(puch_Result, "%s%s\\%s", CDDrive, puch_Directory, puch_Filename);
		#else
			sprintf(puch_Result, "%s%s\\%s", CDDrive, puch_Directory, puch_Filename);
		#endif
	}
	#else
	sprintf(puch_Result, "%s\\%s\\%s", RootPathName, puch_Directory, puch_Filename);
	#endif
}

long dskFileLength (ubyte *puch_Pathname)
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
	int c, i;
	for (i = 0; i < n-1; i++)
	{
		c = fgetc(file);
		if (c == EOF)
		{
			if (!i) return(NULL);
			break;
		}

		if (c == 13) i--;	// ignore
		else if (c == 10) break;
		else text[i] = c;
	}
	text[i] = 0;
	return(text);
}
