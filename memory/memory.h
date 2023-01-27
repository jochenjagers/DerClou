/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_MEMORY
#define MODULE_MEMORY

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

#ifdef THECLOU_DEBUG_ALLOC
extern void *MemAllocDbg(unsigned long size, const char *file, const char *func);
extern void MemFreeDbg(void *mem, unsigned long size, const char *file, const char *func);
#define MemAlloc(size)		MemAllocDbg(size,__FILE__,__func__)
#define MemFree(mem,size)	MemFreeDbg(mem,size,__FILE__,__func__)
#else
extern void *MemAlloc(unsigned long size);
extern void MemFree(void *mem, unsigned long size);
#endif
extern unsigned long MemAvail(void);
extern long MemGetAllocated(void);

#define memGetAllocatedMem()	MemGetAllocated()
#define xmsCopyDown(handle,offset,ptr,size)	memcpy(ptr,handle,size)
#define xmsCopyUp(handle,offset,ptr,size)	memcpy(handle,ptr,size)

#endif /* MODULE_MEMORY */
