/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifdef WIN32
#include <windows.h>
#endif
#include <stdlib.h>
#include "memory\memory.h"

#define ERR_MEMORY_NO_MEM 1

static long l_MemAllocated = 0;

#ifdef THECLOU_DEBUG_ALLOC
void *MemAllocDbg(unsigned long size, const char *file, const char *func)
#else
void *MemAlloc(unsigned long size)
#endif
{
	#ifdef THECLOU_DEBUG_ALLOC
	Log("%s|%s ... %s(%d)", file, func, __func__, size);
	#endif
	void *mem = NULL;
	if (size)
	{
		if (mem = calloc(1, size))
		{
			l_MemAllocated += (long)size;
		}
		else
		{
			NewErrorMsg(No_Mem, __FILE__, __func__, ERR_MEMORY_NO_MEM);
		}
	}

	return(mem);
}

#ifdef THECLOU_DEBUG_ALLOC
void MemFreeDbg(void *mem, unsigned long size, const char *file, const char *func)
#else
void MemFree(void *mem, unsigned long size)
#endif
{
	#ifdef THECLOU_DEBUG_ALLOC
	Log("%s|%s ... %s(%d)", file, func, __func__, size);
	#endif
	if (mem)
	{
		free(mem);
		l_MemAllocated -= (long)size;
	}
}

unsigned long MemAvail(void)
{
	unsigned long av = 0;
	#ifdef WIN32
	MEMORYSTATUS memStat;
	GlobalMemoryStatus(&memStat);
	av = memStat.dwAvailPhys;
	#else
	#error MemAvail() is not implemented
	#endif
	return(av);
}

long MemGetAllocated(void)
{
	return(l_MemAllocated);
}
