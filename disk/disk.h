/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_DISK
#define MODULE_DISK

#include "theclou.h"

#ifndef __STDIO_H
#include <stdio.h>
#endif

#ifndef __STDARG_H
#include <stdarg.h>
#endif

#ifndef __STRING_H
#include <string.h>
#endif

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

extern char CDDrive[256];

#define DSK_PATHNAMELENGTH	  	255L
#define DSK_DUMMY_DISK_ID		  0L	/* to support functions with usefull parameter */

#define ERR_DISK_OPEN_FAILED	1
#define ERR_DISK_WRITE_FAILED 	2
#define ERR_DISK_READ_FAILED	3

extern void dskSetRootPath(const char *newRootPath);
extern char *dskGetRootPath(char *result);

extern long dskLoad(const char *puch_Pathname, void *p_MemDest, uword us_DiskId);
extern void dskBuildPathName(const char *puch_Directory, const char *puch_Filename, char *puch_Result);
extern void dskClose(FILE *p_File);
extern void dskWrite(FILE *p_File, void *p_MemSource, ulong ul_Size);
extern void dskRead(FILE *p_File, void *p_MemDest, ulong ul_Size);

extern long dskFileLength (const char *puch_Pathname);

extern FILE *dskOpen(const char *puch_Pathname, const char *puch_Mode, uword us_DiskId);

extern int dskIsEOF(FILE *p_File);

extern char *dskGets(char *text, int n, FILE *file);

#endif
