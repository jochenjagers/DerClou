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
#include "error/error.h"
#endif

// #define THECLOU_DEBUG_ALLOC	1

#ifdef THECLOU_DEBUG_ALLOC
extern void MemInit(void);
extern void MemQuit(void);
extern void *MemAllocDbg(uint32_t size, const char *file, const char *func);
extern void MemFreeDbg(void *mem, uint32_t size, const char *file, const char *func);
#define MemAlloc(size) MemAllocDbg(size, __FILE__, __func__)
#define MemFree(mem, size) MemFreeDbg(mem, size, __FILE__, __func__)
#else
extern void *MemAlloc(uint32_t size);
extern void MemFree(void *mem, uint32_t size);
#endif
extern uint32_t MemAvail(void);
extern int32_t MemGetAllocated(void);
extern int32_t MemGetMaxAllocated(void);

#endif /* MODULE_MEMORY */
