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

#ifdef THECLOU_CDROM_VERSION
extern char CDDrive[8];
#endif

#define DSK_PATHNAMELENGTH	  	255L
#define DSK_DUMMY_DISK_ID		  0L	/* to support functions with usefull parameter */

extern void dskSetRootPath(char *newRootPath);
extern char *dskGetRootPath(char *result);

extern void dskLoad(ubyte *puch_Pathname, void *p_MemDest, uword us_DiskId);
extern void dskBuildPathName(ubyte *puch_Directory,ubyte *puch_Filename,ubyte *puch_Result);
extern void dskClose(FILE *p_File);
extern void dskWrite(FILE *p_File, void *p_MemSource, ulong ul_Size);
extern void dskRead(FILE *p_File, void *p_MemDest, ulong ul_Size);

extern long dskFileLength (ubyte *puch_Pathname);

extern FILE *dskOpen(ubyte *puch_Pathname, ubyte *puch_Mode, uword us_DiskId);

extern int dskIsEOF(FILE *p_File);

extern char *dskGets(char *text, int n, FILE *file);

#endif
