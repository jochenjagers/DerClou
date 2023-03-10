/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "memory/memory.h"

#include <stdlib.h>

enum
{
    ERR_MEMORY_NO_MEM = 1
};

static int32_t l_MemAllocated = 0;
static int32_t l_MemMaxAllocated = 0;

#ifdef THECLOU_DEBUG_ALLOC
typedef struct Allocation Allocation;
struct Allocation
{
    Allocation *next;
    void *mem;
    uint32_t size;
    char name[64];
};
static Allocation Alloc;

void MemInit(void) { memset(&Alloc, 0, sizeof(Allocation)); }

void MemQuit(void)
{
    // alle verbleibenden auflisten
    Allocation *next;
    next = Alloc.next;
    while (next)
    {
        Log("MemQuit: 0x%08x %d %s", next->mem, next->size, next->name);
        next = next->next;
    }
}

static void AddAlloc(void *ptr, uint32_t size, const char *func)
{
    Allocation *next;
    next = &Alloc;
    while (next->next)
    {
        next = next->next;
    }
    next->next = (Allocation *)calloc(1, sizeof(Allocation));
    if (next->next)
    {
        next = next->next;
        next->next = NULL;
        next->mem = ptr;
        next->size = size;
        strncpy(next->name, func, 63);
        next->name[63] = '\0';
    }
}

static void RemAlloc(void *ptr, uint32_t size, const char *func)
{
    Allocation *next, *prev;
    next = &Alloc;
    prev = next;
    while (next)
    {
        if (next->mem == ptr)
        {
            if (next->size != size)
            {
                Log("RemAlloc: different size 0x%08x %d (%d) %s (%s)", ptr, size, next->size, func, next->name);
            }
            prev->next = next->next;
            free(next);
            return;
        }
        prev = next;
        next = next->next;
    }
    Log("RemAlloc: not found 0x%08x %d %s", ptr, size, func);
}
#endif

#ifdef THECLOU_DEBUG_ALLOC
void *MemAllocDbg(uint32_t size, const char *file, const char *func)
#else
void *MemAlloc(uint32_t size)
#endif
{
#ifdef THECLOU_DEBUG_ALLOC
// Log("%s|%s ... %s(%d)", file, func, __func__, size);
#endif
    void *mem = NULL;
    if (size)
    {
        if ((mem = calloc(1, size)))
        {
            l_MemAllocated += (int32_t)size;
            if (l_MemMaxAllocated < l_MemAllocated)
            {
                l_MemMaxAllocated = l_MemAllocated;
            }
#ifdef THECLOU_DEBUG_ALLOC
            AddAlloc(mem, size, func);
#endif
        }
        else
        {
            NewErrorMsg(No_Mem, __FILE__, __func__, ERR_MEMORY_NO_MEM);
        }
    }

    return mem;
}

#ifdef THECLOU_DEBUG_ALLOC
void MemFreeDbg(void *mem, uint32_t size, const char *file, const char *func)
#else
void MemFree(void *mem, uint32_t size)
#endif
{
#ifdef THECLOU_DEBUG_ALLOC
// Log("%s|%s ... %s(%d)", file, func, __func__, size);
#endif
    if (mem)
    {
        free(mem);
        l_MemAllocated -= (int32_t)size;
#ifdef THECLOU_DEBUG_ALLOC
        RemAlloc(mem, size, func);
#endif
    }
}

int32_t MemGetAllocated(void) { return (l_MemAllocated); }

int32_t MemGetMaxAllocated(void) { return (l_MemMaxAllocated); }
