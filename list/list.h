/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_LIST
#define MODULE_LIST

#include "theclou.h"

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

/*****************
**  Structures  **
*****************/

struct Node
{
  struct Node  * Succ;
  struct Node  * Pred;
  char  * Name;
  uword Size;
  uword	Pad;
};
typedef struct Node NODE;

struct List
{
  NODE  * Head;
  NODE  * Tail;
  NODE  * TPred;
  uword Size;
  uword	Pad;
};
typedef struct List LIST;

/*************
**  Macros  **
*************/

#define NODE_SUCC(node)   ((NODE *)(node))->Succ
#define NODE_PRED(node)   ((NODE *)(node))->Pred
#define NODE_NAME(node)   ((NODE *)(node))->Name
#define NODE_SIZE(node)   ((NODE *)(node))->Size

#define LIST_HEAD(list)   ((LIST *)(list))->Head
#define LIST_TAIL(list)   ((LIST *)(list))->Tail
#define LIST_TPRED(list)  ((LIST *)(list))->TPred
#define LIST_SIZE(list)   ((LIST *)(list))->Size

#define LIST_EMPTY(list)  (((LIST *)(list))->TPred == (NODE *)list)

/*****************
**  Prototypes  **
*****************/

extern void *CreateList(uword size);
extern void RemoveList(void *list);
extern void FreeList(void *list);
extern void *AddNode(void *list, void *node, void *predNode);
extern void *AddTailNode(void *list, void *node);
extern void *AddHeadNode(void *list, void *node);
extern void *RemNode(void *node);
extern void *RemHeadNode(void *list);
extern void *RemTailNode(void *list);
extern void *CreateNode(void *list, uword size, char *name);
extern void RemoveNode(void *list, char *name);
extern void FreeNode(void *node);
extern void *GetNode(void *list, char *name);
extern void *GetNthNode(void *list, ulong nth);
extern ulong GetNrOfNodes(void *list);
extern ulong GetNodeNrByAddr(void *list, void *node);
extern ulong GetNodeNr(void *list, char *name);
extern void foreach(void *list, void (*processNode)(void *));
extern void Link(void *list, void *node, void *predNode);
extern void *UnLinkByAddr(void *list, void *node, void **predNode);
extern void *UnLink(void *list, char *name, void **predNode);
extern void ReplaceNodeByAddr(void *list, void *node, void *newNode);
extern void ReplaceNode(void *list, char *name, void *newNode);
extern ulong ReadList(void *list, uword size, char *fileName, uword diskId);
extern void WriteList(void *list, char *fileName, uword diskId);

#endif /* MODULE_LIST */
