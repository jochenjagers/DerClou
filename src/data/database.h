/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_DATABASE
#define MODULE_DATABASE

// includes
#include "theclou.h"

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_TCDATA
#include "data/tcdata.h"
#endif

#define GET_OUT ((ubyte)-1)  // CHG: !!!!!

// public defines
#define DB_LOAD_MODE_STD 1
#define DB_LOAD_MODE_NO_NAME 0

#define OLF_NORMAL (0)
#define OLF_INCLUDE_NAME (1 << 0)
#define OLF_INSERT_STAR (1 << 1)
#define OLF_PRIVATE_LIST (1 << 2)
#define OLF_ADD_PREV_STRING (1 << 3)
#define OLF_ADD_SUCC_STRING (1 << 4)
#define OLF_ALIGNED (1 << 5)

#define OL_NAME(n) NODE_NAME(n)
#define OL_NR(n) (((struct ObjectNode *)(n))->nr)
#define OL_TYPE(n) (((struct ObjectNode *)(n))->type)
#define OL_DATA(n) (((struct ObjectNode *)(n))->data)

#define OBJ_HASH_SIZE 31

// public structures
struct dbObject
{
    NODE link;
    uint32_t nr;
    uint32_t type;
    uint32_t realNr;
};

struct dbObjectHeader
{
    uint32_t nr;
    uint32_t type;
    uint32_t size;
};

struct ObjectNode
{
    NODE Link;
    uint32_t nr;
    uint32_t type;
    void *data;
};

// public global data
extern LIST *ObjectList;
extern LIST *ObjectListPrivate;
extern uint32_t ObjectListWidth;
extern char *(*ObjectListPrevString)(uint32_t, uint32_t, void *);
extern char *(*ObjectListSuccString)(uint32_t, uint32_t, void *);

extern LIST *objHash[OBJ_HASH_SIZE];

// public prototypes - OBJECTS
ubyte dbLoadAllObjects(char *fileName, uword diskId);
ubyte dbSaveAllObjects(char *fileName, uint32_t offset, uint32_t size, uword diskId);
void dbDeleteAllObjects(uint32_t offset, uint32_t size);

uint32_t dbGetObjectCountOfDB(uint32_t offset, uint32_t size);

void dbSetLoadObjectsMode(ubyte mode);

// public prototypes - OBJECT
void *dbNewObject(uint32_t nr, uint32_t type, uint32_t size, char *name, uint32_t realNr);
void dbDeleteObject(uint32_t nr);

void *dbGetObject(uint32_t nr);
uint32_t dbGetObjectNr(void *key);
char *dbGetObjectName(uint32_t nr, char *objName);

void *dbIsObject(uint32_t nr, uint32_t type);

// public prototypes - OBJECTNODE
struct ObjectNode *dbAddObjectNode(LIST *objectList, uint32_t nr, uint32_t flags);
void dbRemObjectNode(LIST *objectList, uint32_t nr);
struct ObjectNode *dbHasObjectNode(LIST *objectList, uint32_t nr);

void SetObjectListAttr(uint32_t flags, uint32_t type);
void BuildObjectList(void *key);
void ExpandObjectList(LIST *objectList, char *expandItem);

word dbStdCompareObjects(struct ObjectNode *obj1, struct ObjectNode *obj2);
int32_t dbSortObjectList(LIST **objectList, word (*processNode)(struct ObjectNode *, struct ObjectNode *));
void dbSortPartOfList(LIST *objectList, struct ObjectNode *start, struct ObjectNode *end,
                      word (*processNode)(struct ObjectNode *, struct ObjectNode *));

struct ObjectNode *dbAddObjectNode(LIST *objectList, uint32_t nr, uint32_t flags);
void dbRemObjectNode(LIST *objectList, uint32_t nr);

// public prototypes
void dbInit(void);
void dbDone(void);

#endif
