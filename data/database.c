/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

// includes
#include "data\database.h"
#include "data\database.ph"
#include "port\port.h"

// public declaration
LIST *ObjectList           = NULL;
LIST *ObjectListPrivate    = NULL;
ulong ObjectListWidth      = 0L;
char *(*ObjectListPrevString)(ulong, ulong, void *) = NULL;
char *(*ObjectListSuccString)(ulong, ulong, void *) = NULL;


// private declaration
ulong ObjectListType       = 0L;
ulong ObjectListFlags      = OLF_NORMAL;

ubyte ObjectLoadMode			= DB_LOAD_MODE_STD;

LIST *objHash[OBJ_HASH_SIZE];
char decodeStr [11];


// private functions - RELATION
int dbCompare (KEY key1, KEY key2)
{
	return (key1 == key2);
}

char *dbDecode (KEY key)
{
   struct dbObject *obj = dbGetObjectReal(key);

   sprintf (decodeStr, "%ld", obj->nr);
   return decodeStr;
}

KEY dbEncode(char *key)
{
	void *obj = dbGetObject(atol(key));

	if (!obj)
	{
		NewErrorMsg(Internal_Error, __FILE__, __func__, 1);
	}
	return(obj);
}

struct dbObject *dbFindRealObject(ulong realNr, ulong offset, ulong size)
{
   struct dbObject *obj;
   ubyte            objHashValue;

   for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++)
	{
		for (obj = (struct dbObject *)LIST_HEAD(objHash[objHashValue]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
		{
			if (obj->nr > offset)
			{
				if (size && (obj->nr > offset+size))
					continue;

				if (obj->realNr == realNr)
					return obj;
			}
		}
	}

	return NULL;
}


// public functions - OBJECTS
void dbSetLoadObjectsMode(ubyte mode)
{
	ObjectLoadMode = mode;
}

/*
 * Some object structs need artwork
 * for correct memory alignment!
 *
 * We could also convert the data files
 * to the new format instead...
 */
static ubyte dbLoadOrSaveObject(void (*func)(FILE*,void*,ulong), struct dbObjectHeader *objHd, void *obj, FILE *file)
{
	switch (objHd->type)
	{
		case Object_Person:
			func(file, &((Person)obj)->PictID, sizeof(unsigned short));
			func(file, &((Person)obj)->Job, sizeof(unsigned short));
			func(file, &((Person)obj)->Sex, sizeof(unsigned short));
			func(file, &((Person)obj)->Age, sizeof(char));
			func(file, &((Person)obj)->Health, sizeof(unsigned char));
			func(file, &((Person)obj)->Mood, sizeof(unsigned char));
			func(file, &((Person)obj)->Intelligence, sizeof(unsigned char));
			func(file, &((Person)obj)->Strength, sizeof(unsigned char));
			func(file, &((Person)obj)->Stamina, sizeof(unsigned char));
			func(file, &((Person)obj)->Loyality, sizeof(unsigned char));
			func(file, &((Person)obj)->Skill, sizeof(unsigned char));
			func(file, &((Person)obj)->Known, sizeof(unsigned char));
			func(file, &((Person)obj)->Popularity, sizeof(unsigned char));
			func(file, &((Person)obj)->Avarice, sizeof(unsigned char));
			func(file, &((Person)obj)->Panic, sizeof(unsigned char));
			func(file, &((Person)obj)->KnownToPolice, sizeof(unsigned char));
			func(file, &((Person)obj)->TalkBits, sizeof(unsigned long));
			func(file, &((Person)obj)->TalkFileID, sizeof(unsigned char));
			func(file, &((Person)obj)->OldHealth, sizeof(unsigned char));
		break;
		case Object_Player:
			func(file, &((Player)obj)->Money, sizeof(unsigned long));
			func(file, &((Player)obj)->StolenMoney, sizeof(unsigned long));
			func(file, &((Player)obj)->MyStolenMoney, sizeof(unsigned long));
			func(file, &((Player)obj)->NrOfBurglaries, sizeof(unsigned char));
			func(file, &((Player)obj)->JobOfferCount, sizeof(unsigned char));
			func(file, &((Player)obj)->MattsPart, sizeof(unsigned char));
			func(file, &((Player)obj)->CurrScene, sizeof(unsigned long));
			func(file, &((Player)obj)->CurrDay, sizeof(unsigned long));
			func(file, &((Player)obj)->CurrMinute, sizeof(unsigned long));
			func(file, &((Player)obj)->CurrLocation, sizeof(unsigned long));
		break;
		case Object_Car:
			func(file, &((Car)obj)->PictID, sizeof(unsigned short));
			func(file, &((Car)obj)->Land, sizeof(unsigned short));
			func(file, &((Car)obj)->Value, sizeof(unsigned long));
			func(file, &((Car)obj)->YearOfConstruction, sizeof(unsigned short));
			func(file, &((Car)obj)->ColorIndex, sizeof(unsigned short));
			func(file, &((Car)obj)->Strike, sizeof(unsigned char));
			func(file, &((Car)obj)->Capacity, sizeof(unsigned long));
			func(file, &((Car)obj)->PS, sizeof(unsigned char));
			func(file, &((Car)obj)->Speed, sizeof(unsigned char));
			func(file, &((Car)obj)->State, sizeof(unsigned char));
			func(file, &((Car)obj)->MotorState, sizeof(unsigned char));
			func(file, &((Car)obj)->BodyWorkState, sizeof(unsigned char));
			func(file, &((Car)obj)->TyreState, sizeof(unsigned char));
			func(file, &((Car)obj)->PlacesInCar, sizeof(unsigned char));
		break;
		case Object_Ability:
			func(file, &((Ability)obj)->Name, sizeof(unsigned short));
			func(file, &((Ability)obj)->Use, sizeof(unsigned long));
		break;
		case Object_Tool:
			func(file, &((Tool)obj)->PictID, sizeof(unsigned short));
			func(file, &((Tool)obj)->Value, sizeof(unsigned long));
			func(file, &((Tool)obj)->Danger, sizeof(unsigned char));
			func(file, &((Tool)obj)->Volume, sizeof(unsigned char));
			func(file, &((Tool)obj)->Effect, sizeof(unsigned char));
		break;
		case Object_LSObject:
			func(file, &((LSObject)obj)->us_OffsetFact, sizeof(unsigned short));
			func(file, &((LSObject)obj)->us_DestX, sizeof(unsigned short));
			func(file, &((LSObject)obj)->us_DestY, sizeof(unsigned short));
			func(file, &((LSObject)obj)->uch_ExactX, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactY, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactX1, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactY1, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Size, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Visible, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Chained, sizeof(unsigned char));
			func(file, &((LSObject)obj)->ul_Status, sizeof(unsigned long));
			func(file, &((LSObject)obj)->Type, sizeof(unsigned long));
		break;
		case Object_Building:
			func(file, &((Building)obj)->LocationNr, sizeof(unsigned long));
			func(file, &((Building)obj)->PoliceTime, sizeof(unsigned short));
			func(file, &((Building)obj)->GTime, sizeof(unsigned short));
			func(file, &((Building)obj)->Exactlyness, sizeof(unsigned char));
			func(file, &((Building)obj)->GRate, sizeof(unsigned char));
			func(file, &((Building)obj)->Strike, sizeof(unsigned char));
			func(file, &((Building)obj)->Values, sizeof(unsigned long));
			func(file, &((Building)obj)->EscapeRoute, sizeof(unsigned short));
			func(file, &((Building)obj)->EscapeRouteLength, sizeof(unsigned char));
			func(file, &((Building)obj)->RadioGuarding, sizeof(unsigned char));
			func(file, &((Building)obj)->MaxVolume, sizeof(unsigned char));
			func(file, &((Building)obj)->GuardStrength, sizeof(unsigned char));
			func(file, &((Building)obj)->CarXPos, sizeof(unsigned short));
			func(file, &((Building)obj)->CarYPos, sizeof(unsigned short));
			#ifdef THECLOU_PROFIDISK
			func(file, &((Building)obj)->DiskId, sizeof(unsigned char));
			#endif
		break;
		case Object_LSArea:
			func(file, &((LSArea)obj)->us_Coll16ID, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_Coll32ID, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_Coll48ID, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_PlanColl16ID, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_PlanColl32ID, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_PlanColl48ID, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_FloorCollID, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_PlanFloorCollID, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_Width, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_Height, sizeof(unsigned short));
			func(file, &((LSArea)obj)->ul_ObjectBaseNr, sizeof(unsigned long));
			func(file, &((LSArea)obj)->uch_Darkness, sizeof(unsigned char));
			func(file, &((LSArea)obj)->us_StartX0, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartX1, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartX2, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartX3, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartX4, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartX5, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartY0, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartY1, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartY2, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartY3, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartY4, sizeof(unsigned short));
			func(file, &((LSArea)obj)->us_StartY5, sizeof(unsigned short));
		break;

		case Object_Location:
		case Object_Item:
		case Object_Environment:
		case Object_London:
		case Object_Evidence:
		case Object_Loot:
		case Object_CompleteLoot:
		case Object_LSOLock:
		case Object_Police:
		case Object_LSRoom:
		default:
			func(file, obj, objHd->size);
		break;
	}
	return(1);
}

ubyte dbLoadAllObjects(char *fileName, uword diskId)
{
	FILE *fh;

	if (fh = dskOpen(fileName, "rb", diskId))
	{
		ulong realNr = 1L;
		struct dbObjectHeader objHd;

		while (!dskIsEOF(fh))
		{
			objHd.nr   = 0L;
			objHd.type = 0L;
			objHd.size = 0L;

			dskRead(fh, &objHd, sizeof(struct dbObjectHeader));
			#ifdef __COSP__
			EndianL(&objHd.nr);
			EndianL(&objHd.type);
			EndianL(&objHd.size);
			#endif

			if ((objHd.nr != 0xffffffff) && (objHd.type != 0xffffffff) && (objHd.size != 0xffffffff))
			{
				LIST *list = NULL;
				void *obj;
				char *name = NULL;

				if (ObjectLoadMode)
				{
					if (list = txtGoKey(OBJECTS_TXT, NULL))      // MOD: old version GoNextKey
					{
						name = NODE_NAME(LIST_HEAD(list));
					}
				}

				if (!(obj = dbNewObject(objHd.nr, objHd.type, objHd.size, name, realNr++)))
				{
					dskClose(fh);
					dbDeleteAllObjects(0L,0L);
					return 0;
				}

				if (list)
				{
					RemoveList(list);
				}

				dbLoadOrSaveObject(dskRead, &objHd, obj, fh);
			}
		}

		dskClose(fh);

		return 1;
	}

	return 0;
}

ubyte dbSaveAllObjects(char *fileName, ulong offset, ulong size, uword diskId)
{
	FILE *fh;
	struct dbObject *obj;
	ulong realNr = 1;
	ulong dbSize = dbGetObjectCountOfDB(offset, size);

	if (fh = dskOpen(fileName, "wb", diskId))
	{
		while (realNr <= dbSize)
		{
			if (obj = dbFindRealObject (realNr++, offset, size))
			{
				struct dbObjectHeader objHd;

				objHd.nr   = obj->nr;
				objHd.type = obj->type;
				objHd.size = NODE_SIZE(obj) - sizeof(struct dbObject);

				dskWrite(fh, &objHd, sizeof(struct dbObjectHeader));
				if ((objHd.nr != 0xffffffff) && (objHd.type != 0xffffffff) && (objHd.size != 0xffffffff))
				{
					dbLoadOrSaveObject(dskWrite, &objHd, &((ubyte*)obj)[sizeof(struct dbObject)], fh);
				}
			}
		}

		dskClose(fh);

		return 1;
	}

	return 0;
}

void dbDeleteAllObjects(ulong offset, ulong size)
{
	struct dbObject *obj, *pred;
   ubyte objHashValue;

	for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++)
	{
      for (obj = (struct dbObject *)LIST_HEAD(objHash[objHashValue]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
      {
         if (obj->nr > offset)
         {
            if (size && (obj->nr > offset+size))
               continue;

            pred = (struct dbObject*)NODE_PRED(obj);
            RemNode(obj);     // MOD: old version Remove
            FreeNode(obj);
            obj = pred;
         }
      }
   }
}

ulong dbGetObjectCountOfDB(ulong offset, ulong size)
{
   ulong count = 0;
   struct dbObject *obj;
   ubyte i;

   for (i = 0; i < OBJ_HASH_SIZE; i++)
   {
      for (obj = (struct dbObject *)LIST_HEAD(objHash[i]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
      {
         if ((obj->nr > offset) && (obj->nr < (offset+size)))
            count++;
      }
   }

   return count;
}


// public functions - OBJECT
void *dbNewObject (ulong nr, ulong type, ulong size, char *name, ulong realNr)
{
	struct dbObject *obj;
	ubyte objHashValue = dbGetObjectHashNr(nr);

   if (!(obj = (struct dbObject *)CreateNode(objHash[objHashValue], sizeof(struct dbObject) + size, name)))
      return NULL;

   obj->nr = nr;
   obj->type = type;
   obj->realNr = realNr;

   return dbGetObjectKey(obj);
}

void dbDeleteObject (ulong nr)
{
   struct dbObject *obj = NULL;
   ubyte objHashValue = dbGetObjectHashNr(nr);

   for (obj = (struct dbObject *)LIST_HEAD(objHash[objHashValue]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
   {
      if (obj->nr == nr)
      {
         UnSetAll((KEY)(obj+1), NULL);
         RemNode(obj);     // MOD:
         FreeNode(obj);
         return;
      }
	}
}

void *dbGetObject(ulong nr)
{
   struct dbObject *obj;
   ubyte objHashValue = dbGetObjectHashNr(nr);

   for (obj = (struct dbObject *)LIST_HEAD(objHash[objHashValue]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
   {
      if (obj->nr == nr)
         return dbGetObjectKey(obj);
   }

   return NULL;
}

ulong dbGetObjectNr(void *key)
{
  return dbGetObjectReal(key)->nr;
}

char *dbGetObjectName(ulong nr, char *objName)
{
   struct dbObject *obj;
   ubyte objHashValue = dbGetObjectHashNr(nr);

   for (obj = (struct dbObject *)LIST_HEAD(objHash[objHashValue]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
   {
      if (obj->nr == nr)
      {
         strcpy(objName, NODE_NAME(obj));
         return objName;
      }
   }

   return NULL;
}

void *dbIsObject (ulong nr, ulong type)
{
   struct dbObject *obj;
   ubyte objHashValue = dbGetObjectHashNr(nr);

   for (obj = (struct dbObject *)LIST_HEAD(objHash[objHashValue]); NODE_SUCC(obj); obj = (struct dbObject *)NODE_SUCC(obj))
   {
      if (obj->nr == nr)
      {
         if (obj->type == type)
            return dbGetObjectKey(obj);

         break;
      }
   }

   return NULL;
}

// public prototypes - OBJECTNODE
struct ObjectNode *dbAddObjectNode (LIST *objectList, ulong nr, ulong flags)
{
   struct ObjectNode *n = NULL;
   struct dbObject *obj = dbGetObjectReal(dbGetObject(nr));
   char name[TXT_KEY_LENGTH] = {0};
   char *namePtr;

   name[0] = EOS;

   if (flags & OLF_INSERT_STAR)
      strcpy(name, "*");

   if (flags & OLF_INCLUDE_NAME)
   {
		char *succString;

		if ((flags & OLF_ADD_PREV_STRING) && ObjectListPrevString)
         strcat(name, ObjectListPrevString(obj->nr, obj->type, dbGetObjectKey(obj)));

      strcat(name, NODE_NAME(obj));
      namePtr = name;

      if ((flags & OLF_ADD_SUCC_STRING) && ObjectListSuccString)
         succString = ObjectListSuccString(obj->nr, obj->type, dbGetObjectKey(obj));

      if ((flags & (OLF_ADD_SUCC_STRING|OLF_ALIGNED)) && ObjectListWidth)
      {
         ubyte i, len = strlen(name) + strlen(succString);

         if (flags & OLF_INSERT_STAR)
            len--;

         for (i = len; i < ObjectListWidth; i++)
            strcat (name, " ");
		}

      if ((flags & OLF_ADD_SUCC_STRING) && ObjectListSuccString)
         strcat(name, succString);
   }
   else
      namePtr = NULL;

   if (n = (struct ObjectNode *)CreateNode(objectList, sizeof(struct ObjectNode), namePtr))
   {
      n->nr = obj->nr;
      n->type = obj->type;
      n->data = dbGetObjectKey(obj);
   }

   return n;
}

void dbRemObjectNode (LIST *objectList, ulong nr)
{
   struct ObjectNode *n = dbHasObjectNode (objectList, nr);

   if (n)
   {
     RemNode(n);     // MOD:
     FreeNode(n);
   }
}

struct ObjectNode *dbHasObjectNode(LIST *objectList, ulong nr)
{
   struct ObjectNode *n;

   for (n = (struct ObjectNode *)LIST_HEAD(objectList); NODE_SUCC(n); n = (struct ObjectNode *)NODE_SUCC(n))
   {
		if (OL_NR(n) == nr)
         return n;
   }

   return NULL;
}

void SetObjectListAttr(ulong flags, ulong type)
{
	ObjectListType  = type;
	ObjectListFlags = flags;

	if (ObjectListFlags & OLF_PRIVATE_LIST)
		ObjectListPrivate = CreateList(0L);
	else
	{
		RemoveList(ObjectList);
		ObjectList = CreateList(0L);
	}
}

void BuildObjectList(void *key)
{
	struct dbObject *obj = dbGetObjectReal(key);
	LIST *list;

	if (!ObjectListType || (obj->type == ObjectListType))
	{
		if (ObjectListFlags & OLF_PRIVATE_LIST)
			list = ObjectListPrivate;
		else
			list = ObjectList;

		dbAddObjectNode(list, obj->nr, ObjectListFlags);
	}
}

void ExpandObjectList (LIST *objectList, char *expandItem)
{
	struct ObjectNode *objNode;

	if (!(objNode = (struct ObjectNode *)CreateNode(objectList, sizeof(struct ObjectNode), expandItem)))
		NewErrorMsg(Internal_Error, __FILE__, __func__, 2);

   objNode->nr = 0;
   objNode->type = 0;
   objNode->data = NULL;
}


word dbStdCompareObjects(struct ObjectNode *obj1, struct ObjectNode *obj2)
{
	if (obj1->nr > obj2->nr)
      return -1;

   if (obj1->nr < obj2->nr)
      return 1;

   return 0;
}

void dbSortPartOfList(LIST *l,struct ObjectNode *start, struct ObjectNode *end, word (*processNode)(struct ObjectNode *, struct ObjectNode *))
{
	LIST *newList = CreateList(0L);
	struct ObjectNode *n, *n1, *startPred;
	long i, j;

	if (start == (struct ObjectNode *) LIST_HEAD(l))
		startPred = 0L;
	else
		startPred = (struct ObjectNode *) NODE_PRED(start);

	for (n = start, i = 1; n != end; n = (struct ObjectNode *) NODE_SUCC(n), i++);

	n = start;

	for (j = 0; j < i; j++)
	{
		n1 = n;
		n = (struct ObjectNode *) NODE_SUCC(n);

		RemNode(n1);   // MOD:
		AddHeadNode(newList, n1);
	}

	dbSortObjectList(&newList, processNode);

	for (n = (struct ObjectNode *) LIST_HEAD(newList); NODE_SUCC(n);)
	{
		n1 = n;
		n = (struct ObjectNode *) NODE_SUCC(n);

		RemNode(n1);   // MOD:
		AddNode(l, n1, startPred);

		startPred = n1;
	}

	RemoveList(newList);
}

long dbSortObjectList (LIST **objectList, word (*processNode)(struct ObjectNode *, struct ObjectNode *))
{
	LIST *newList;
	struct ObjectNode *n1, *n2, *pred, *new;
	long i = 0;

	if (!LIST_EMPTY(*objectList))
	{
		newList = CreateList(0L);

		for (n1 = (struct ObjectNode *) LIST_HEAD(*objectList); NODE_SUCC(n1); n1 = (struct ObjectNode *) NODE_SUCC(n1), i++)
		{
			pred = 0;

			if (!LIST_EMPTY(newList))
			{
				for (n2 = (struct ObjectNode *) LIST_HEAD(newList); !pred && NODE_SUCC(n2);  n2 = (struct ObjectNode *) NODE_SUCC(n2))
            {
               if (processNode(n1, n2) >= 0)
                  pred = n2;
            }
         }

         new = (struct ObjectNode *)CreateNode(NULL, sizeof(struct ObjectNode), NODE_NAME(n1));
         new->nr = n1->nr;
         new->type = n1->type;
         new->data = n1->data;

         if (pred)
         {
				if (pred == (struct ObjectNode *) LIST_HEAD(newList))
					AddHeadNode(newList, new);
				else
					AddNode(newList, new, NODE_PRED(pred));
			}
			else
				AddTailNode(newList, new);
      }

      if (!LIST_EMPTY(newList))
      {
         RemoveList(*objectList);
         *objectList = newList;
      }
      else
         RemoveList(newList);
   }

   return i;
}


// public prototypes
void dbInit(void)
{
   ubyte objHashValue;

   if (!(ObjectList = CreateList(0L)))
	NewErrorMsg(No_Mem, __FILE__, __func__, 3);

   for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++)
   {
      if (!(objHash[objHashValue] = CreateList(0L)))
	NewErrorMsg(No_Mem, __FILE__, __func__, 4);
   }

   CompareKey = dbCompare;
   EncodeKey = dbEncode;
	DecodeKey = dbDecode;
}

void dbDone(void)
{
   ubyte objHashValue;

   for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue ++)
   {
      if (objHash[objHashValue])
         RemoveList(objHash[objHashValue]);
   }

   if (ObjectList)
		RemoveList(ObjectList);
}
