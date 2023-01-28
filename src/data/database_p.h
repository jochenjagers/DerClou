/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_DATABASE_PH
#define MODULE_DATABASE_PH

// includes

// private defines
#define dbGetObjectReal(key) (((struct dbObject *)key) - 1)
#define dbGetObjectKey(obj) ((void *)(obj + 1))
#define dbGetObjectHashNr(nr) ((ubyte)(nr % OBJ_HASH_SIZE))

#define EOS ((char)'\0')

// private definitions
extern LIST *objHash[OBJ_HASH_SIZE];
extern char decodeStr[11];

extern uint32_t ObjectListType;
extern uint32_t ObjectListFlags;

// private prototypes - RELATION
int dbCompare(KEY key1, KEY key2);
char *dbDecode(KEY key);
KEY dbEncode(char *key);

// private prototypes - OBJECT
struct dbObject *dbFindRealObject(uint32_t realNr, uint32_t offset, uint32_t size);

//
void dbcheckSize(uint32_t objSize, uint32_t structSize);
uint32_t dbGetObjLoadSize(uint32_t structType);
uint32_t dbGetObjSaveSize(struct dbObjectHeader *objHd);

#endif
