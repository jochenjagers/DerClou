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
#define dbGetObjectReal(key)  (((struct dbObject *)key)-1)
#define dbGetObjectKey(obj)   ((void *)(obj+1))
#define dbGetObjectHashNr(nr) ((ubyte)(nr % OBJ_HASH_SIZE))

#define EOS	 					((char)'\0')


// private definitions
extern LIST *objHash[OBJ_HASH_SIZE];
extern char decodeStr[11];

extern ulong ObjectListType;
extern ulong ObjectListFlags;


// private prototypes - RELATION
int dbCompare(KEY key1, KEY key2);
char *dbDecode(KEY key);
KEY dbEncode(char *key);

// private prototypes - OBJECT
struct dbObject *dbFindRealObject(ulong realNr, ulong offset, ulong size);

//
void  dbcheckSize(ulong objSize, ulong structSize);
ulong dbGetObjLoadSize(ulong structType);
ulong dbGetObjSaveSize(struct dbObjectHeader *objHd);

#endif
