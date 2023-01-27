/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

// includes
#include "data\database.h"
#include "data\database_p.h"
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
		// LucyG 2017-10-31 : Buckingham Palace crashed with key=""
		Log("%s|%s: dbEncode(\"%s\")", __FILE__, __func__, key);
		//NewErrorMsg(Internal_Error, __FILE__, __func__, 1);
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
 * Some object structs need rework
 * for correct memory alignment!
 *
 * We could also convert the data files
 * to the new format instead...
 */
static ubyte dbLoadOrSaveObject(struct dbObjectHeader *objHd, void *obj, FILE *file, ulong structSize, char mode)
{
	void (*func)(FILE*, void*,ulong);

	if (mode == 0) // Read mode
	{
		func = dskRead;

		switch (objHd->type)
		{
		case Object_Person:
			dbcheckSize(sizeof(*(Person)obj),structSize);

			func(file, &((Person)obj)->PictID, sizeof(unsigned short));
			EndianW(&((Person)obj)->PictID);
			func(file, &((Person)obj)->Job, sizeof(unsigned short));
			EndianW(&((Person)obj)->Job);
			func(file, &((Person)obj)->Sex, sizeof(unsigned short));
			EndianW(&((Person)obj)->Sex);
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
			EndianL(&((Person)obj)->TalkBits);
			func(file, &((Person)obj)->TalkFileID, sizeof(unsigned char));
			func(file, &((Person)obj)->OldHealth, sizeof(unsigned char));
			break;
		case Object_Player:
			dbcheckSize(sizeof(*(Player)obj),structSize);

			func(file, &((Player)obj)->Money, sizeof(unsigned long));
			EndianL(&((Player)obj)->Money);
			func(file, &((Player)obj)->StolenMoney, sizeof(unsigned long));
			EndianL(&((Player)obj)->StolenMoney);
			func(file, &((Player)obj)->MyStolenMoney, sizeof(unsigned long));
			EndianL(&((Player)obj)->MyStolenMoney);
			func(file, &((Player)obj)->NrOfBurglaries, sizeof(unsigned char));
			func(file, &((Player)obj)->JobOfferCount, sizeof(unsigned char));
			func(file, &((Player)obj)->MattsPart, sizeof(unsigned char));
			func(file, &((Player)obj)->CurrScene, sizeof(unsigned long));
			EndianL(&((Player)obj)->CurrScene);
			func(file, &((Player)obj)->CurrDay, sizeof(unsigned long));
			EndianL(&((Player)obj)->CurrDay);
			func(file, &((Player)obj)->CurrMinute, sizeof(unsigned long));
			EndianL(&((Player)obj)->CurrMinute);
			func(file, &((Player)obj)->CurrLocation, sizeof(unsigned long));
			EndianL(&((Player)obj)->CurrLocation);
			break;
		case Object_Car:
			dbcheckSize(sizeof(*(Car)obj),structSize);

			func(file, &((Car)obj)->PictID, sizeof(unsigned short));
			EndianW(&((Car)obj)->PictID);
			func(file, &((Car)obj)->Land, sizeof(unsigned short));
			EndianW(&((Car)obj)->Land);
			func(file, &((Car)obj)->Value, sizeof(unsigned long));
			EndianL(&((Car)obj)->Value);
			func(file, &((Car)obj)->YearOfConstruction, sizeof(unsigned short));
			EndianW(&((Car)obj)->YearOfConstruction);
			func(file, &((Car)obj)->ColorIndex, sizeof(unsigned short));
			EndianW(&((Car)obj)->ColorIndex);
			func(file, &((Car)obj)->Strike, sizeof(unsigned char));
			func(file, &((Car)obj)->Capacity, sizeof(unsigned long));
			EndianL(&((Car)obj)->Capacity);
			func(file, &((Car)obj)->PS, sizeof(unsigned char));
			func(file, &((Car)obj)->Speed, sizeof(unsigned char));
			func(file, &((Car)obj)->State, sizeof(unsigned char));
			func(file, &((Car)obj)->MotorState, sizeof(unsigned char));
			func(file, &((Car)obj)->BodyWorkState, sizeof(unsigned char));
			func(file, &((Car)obj)->TyreState, sizeof(unsigned char));
			func(file, &((Car)obj)->PlacesInCar, sizeof(unsigned char));
			break;
		case Object_Ability:
			dbcheckSize(sizeof(*(Ability)obj),structSize);

			func(file, &((Ability)obj)->Name, sizeof(unsigned short));
			EndianW(&((Ability)obj)->Name);
			func(file, &((Ability)obj)->Use, sizeof(unsigned long));
			EndianL(&((Ability)obj)->Use);
			break;
		case Object_Tool:
			dbcheckSize(sizeof(*(Tool)obj),structSize);

			func(file, &((Tool)obj)->PictID, sizeof(unsigned short));
			EndianW(&((Tool)obj)->PictID);
			func(file, &((Tool)obj)->Value, sizeof(unsigned long));
			EndianL(&((Tool)obj)->Value);
			func(file, &((Tool)obj)->Danger, sizeof(unsigned char));
			func(file, &((Tool)obj)->Volume, sizeof(unsigned char));
			func(file, &((Tool)obj)->Effect, sizeof(unsigned char));
			break;
		case Object_LSObject:
			dbcheckSize(sizeof(*(LSObject)obj),structSize);

			func(file, &((LSObject)obj)->us_OffsetFact, sizeof(unsigned short));
			EndianW(&((LSObject)obj)->us_OffsetFact);
			func(file, &((LSObject)obj)->us_DestX, sizeof(unsigned short));
			EndianW(&((LSObject)obj)->us_DestX);
			func(file, &((LSObject)obj)->us_DestY, sizeof(unsigned short));
			EndianW(&((LSObject)obj)->us_DestY);
			func(file, &((LSObject)obj)->uch_ExactX, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactY, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactX1, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactY1, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Size, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Visible, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Chained, sizeof(unsigned char));
			func(file, &((LSObject)obj)->ul_Status, sizeof(unsigned long));
			EndianL(&((LSObject)obj)->ul_Status);
			func(file, &((LSObject)obj)->Type, sizeof(unsigned long));
			EndianL(&((LSObject)obj)->Type);
			break;
		case Object_Building:
			dbcheckSize(sizeof(*(Building)obj),structSize);

			func(file, &((Building)obj)->LocationNr, sizeof(unsigned long));
			EndianL(&((Building)obj)->LocationNr);
			func(file, &((Building)obj)->PoliceTime, sizeof(unsigned short));
			EndianW(&((Building)obj)->PoliceTime);
			func(file, &((Building)obj)->GTime, sizeof(unsigned short));
			EndianW(&((Building)obj)->GTime);
			func(file, &((Building)obj)->Exactlyness, sizeof(unsigned char));
			func(file, &((Building)obj)->GRate, sizeof(unsigned char));
			func(file, &((Building)obj)->Strike, sizeof(unsigned char));
			func(file, &((Building)obj)->Values, sizeof(unsigned long));
			EndianL(&((Building)obj)->Values);
			func(file, &((Building)obj)->EscapeRoute, sizeof(unsigned short));
			EndianW(&((Building)obj)->EscapeRoute);
			func(file, &((Building)obj)->EscapeRouteLength, sizeof(unsigned char));
			func(file, &((Building)obj)->RadioGuarding, sizeof(unsigned char));
			func(file, &((Building)obj)->MaxVolume, sizeof(unsigned char));
			func(file, &((Building)obj)->GuardStrength, sizeof(unsigned char));
			func(file, &((Building)obj)->CarXPos, sizeof(unsigned short));
			EndianW(&((Building)obj)->CarXPos);
			func(file, &((Building)obj)->CarYPos, sizeof(unsigned short));
			EndianW(&((Building)obj)->CarYPos);
			if (bProfidisk)
				func(file, &((Building)obj)->DiskId, sizeof(unsigned char));
			break;
		case Object_LSArea:
			dbcheckSize(sizeof(*(LSArea)obj),structSize);

			func(file, &((LSArea)obj)->us_Coll16ID, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_Coll16ID);
			func(file, &((LSArea)obj)->us_Coll32ID, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_Coll32ID);
			func(file, &((LSArea)obj)->us_Coll48ID, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_Coll48ID);
			func(file, &((LSArea)obj)->us_PlanColl16ID, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_PlanColl16ID);
			func(file, &((LSArea)obj)->us_PlanColl32ID, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_PlanColl32ID);
			func(file, &((LSArea)obj)->us_PlanColl48ID, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_PlanColl48ID);
			func(file, &((LSArea)obj)->us_FloorCollID, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_FloorCollID);
			func(file, &((LSArea)obj)->us_PlanFloorCollID, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_PlanFloorCollID);
			func(file, &((LSArea)obj)->us_Width, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_Width);
			func(file, &((LSArea)obj)->us_Height, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_Height);
			func(file, &((LSArea)obj)->ul_ObjectBaseNr, sizeof(unsigned long));
			EndianL(&((LSArea)obj)->ul_ObjectBaseNr);
			func(file, &((LSArea)obj)->uch_Darkness, sizeof(unsigned char));
			func(file, &((LSArea)obj)->us_StartX0, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartX0);
			func(file, &((LSArea)obj)->us_StartX1, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartX1);
			func(file, &((LSArea)obj)->us_StartX2, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartX2);
			func(file, &((LSArea)obj)->us_StartX3, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartX3);
			func(file, &((LSArea)obj)->us_StartX4, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartX4);
			func(file, &((LSArea)obj)->us_StartX5, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartX5);
			func(file, &((LSArea)obj)->us_StartY0, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartY0);
			func(file, &((LSArea)obj)->us_StartY1, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartY1);
			func(file, &((LSArea)obj)->us_StartY2, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartY2);
			func(file, &((LSArea)obj)->us_StartY3, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartY3);
			func(file, &((LSArea)obj)->us_StartY4, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartY4);
			func(file, &((LSArea)obj)->us_StartY5, sizeof(unsigned short));
			EndianW(&((LSArea)obj)->us_StartY5);
			break;

		case Object_Location:
			dbcheckSize(sizeof(*(Location)obj),structSize);

			func(file, &((Location)obj)->LocationNr, sizeof(unsigned long));
			EndianL(&((Location)obj)->LocationNr);

			func(file, &((Location)obj)->OpenFromMinute, sizeof(unsigned short));
			EndianW(&((Location)obj)->OpenFromMinute);

			func(file, &((Location)obj)->OpenToMinute, sizeof(unsigned short));
			EndianW(&((Location)obj)->OpenToMinute);
			break;
		case Object_Item:
			dbcheckSize(sizeof(*(Item)obj),structSize);

			func(file, &((Item)obj)->Type, sizeof(unsigned short));
			EndianW(&((Item)obj)->Type);

			func(file, &((Item)obj)->OffsetFact, sizeof(unsigned short));
			EndianW(&((Item)obj)->OffsetFact);

			func(file, &((Item)obj)->HExactXOffset, sizeof(unsigned short));
			EndianW(&((Item)obj)->HExactXOffset);

			func(file, &((Item)obj)->HExactYOffset, sizeof(unsigned short));
			EndianW(&((Item)obj)->HExactYOffset);

			func(file, &((Item)obj)->HExactWidth, sizeof(unsigned short));
			EndianW(&((Item)obj)->HExactWidth);

			func(file, &((Item)obj)->HExactHeight, sizeof(unsigned short));
			EndianW(&((Item)obj)->HExactHeight);

			func(file, &((Item)obj)->VExactXOffset, sizeof(unsigned short));
			EndianW(&((Item)obj)->VExactXOffset);

			func(file, &((Item)obj)->VExactYOffset, sizeof(unsigned short));
			EndianW(&((Item)obj)->VExactYOffset);

			func(file, &((Item)obj)->VExactWidth, sizeof(unsigned short));
			EndianW(&((Item)obj)->VExactWidth);

			func(file, &((Item)obj)->VExactHeight, sizeof(unsigned short));
			EndianW(&((Item)obj)->VExactHeight);

			func(file, &((Item)obj)->Size, sizeof(unsigned char));
			func(file, &((Item)obj)->ColorNr, sizeof(unsigned char));
			break;
		case Object_Environment:
			dbcheckSize(sizeof(*(Environment)obj),structSize);

			func(file, &((Environment)obj)->MattHasHotelRoom, sizeof(unsigned char));
			func(file, &((Environment)obj)->MattHasIdentityCard, sizeof(unsigned char));
			func(file, &((Environment)obj)->WithOrWithoutYou, sizeof(unsigned char));
			func(file, &((Environment)obj)->MattIsInLove, sizeof(unsigned char));
			func(file, &((Environment)obj)->SouthhamptonHappened, sizeof(unsigned char));
			func(file, &((Environment)obj)->Present, sizeof(unsigned char));
			func(file, &((Environment)obj)->FirstTimeInSouth, sizeof(unsigned char));
			
			if (bProfidisk)
				func(file, &((Environment)obj)->PostzugDone, sizeof(unsigned char));

			break;
		case Object_London:
			dbcheckSize(sizeof(*(London)obj),structSize);

			func(file, &((London)obj)->Useless, sizeof(unsigned char));
			break;
		case Object_Evidence:
			dbcheckSize(sizeof(*(Evidence)obj),structSize);

			func(file, &((Evidence)obj)->pers, sizeof(unsigned long));
			EndianL(&((Evidence)obj)->pers);
			func(file, &((Evidence)obj)->Recognition, sizeof(unsigned char));
			func(file, &((Evidence)obj)->WalkTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->WaitTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->WorkTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->KillTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->CallTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->PaperTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->FotoTrail, sizeof(unsigned char));
			break;
		case Object_Loot:
			dbcheckSize(sizeof(*(Loot)obj),structSize);

			func(file, &((Loot)obj)->Type, sizeof(unsigned short));
			EndianW(&((Loot)obj)->Type);
			func(file, &((Loot)obj)->Name, sizeof(unsigned short));
			EndianW(&((Loot)obj)->Name);
			func(file, &((Loot)obj)->Volume, sizeof(unsigned long));
			EndianL(&((Loot)obj)->Volume);
			func(file, &((Loot)obj)->Weight, sizeof(unsigned short));
			EndianW(&((Loot)obj)->Weight);
			func(file, &((Loot)obj)->PictID, sizeof(unsigned short));
			EndianW(&((Loot)obj)->PictID);
			break;
		case Object_CompleteLoot:
			dbcheckSize(sizeof(*(CompleteLoot)obj),structSize);
			

			func(file, &((CompleteLoot)obj)->Bild, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->Bild);
			func(file, &((CompleteLoot)obj)->Gold, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->Gold);
			func(file, &((CompleteLoot)obj)->Geld, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->Geld);
			func(file, &((CompleteLoot)obj)->Juwelen, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->Juwelen);
			func(file, &((CompleteLoot)obj)->Delikates, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->Delikates);
			func(file, &((CompleteLoot)obj)->Statue, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->Statue);
			func(file, &((CompleteLoot)obj)->Kuriositaet, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->Kuriositaet);
			func(file, &((CompleteLoot)obj)->HistKunst, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->HistKunst);
			func(file, &((CompleteLoot)obj)->GebrauchsArt, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->GebrauchsArt);
			func(file, &((CompleteLoot)obj)->Vase, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->Vase);
			func(file, &((CompleteLoot)obj)->TotalWeight, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->TotalWeight);
			func(file, &((CompleteLoot)obj)->TotalVolume, sizeof(unsigned long));
			EndianL(&((CompleteLoot)obj)->TotalVolume);
			break;
		case Object_LSOLock:
			dbcheckSize(sizeof(*(LSOLock)obj),structSize);

			func(file, &((LSOLock)obj)->Type, sizeof(unsigned short));
			EndianW(&((LSOLock)obj)->Type);
			break;
		case Object_Police:
			dbcheckSize(sizeof(*(Police)obj),structSize);

			func(file, &((Police)obj)->PictID, sizeof(unsigned short));
			EndianW(&((Police)obj)->PictID);
			func(file, &((Police)obj)->LivingID, sizeof(unsigned char));
			break;
		case Object_LSRoom:
			dbcheckSize(sizeof(*(LSRoom)obj),structSize);

			func(file, &((LSRoom)obj)->us_LeftEdge, sizeof(unsigned short));
			EndianW(&((LSRoom)obj)->us_LeftEdge);
			func(file, &((LSRoom)obj)->us_TopEdge, sizeof(unsigned short));
			EndianW(&((LSRoom)obj)->us_TopEdge);
			func(file, &((LSRoom)obj)->us_Width, sizeof(unsigned short));
			EndianW(&((LSRoom)obj)->us_Width);
			func(file, &((LSRoom)obj)->us_Height, sizeof(unsigned short));
			EndianW(&((LSRoom)obj)->us_Height);
			break;
		default:
			//func(file, obj, objHd->size);
			Log("Error: Unknown object type %d |dbLoadOrSaveObject" ,objHd->type);
			break;
		}
	}
	else if (mode == 1) // Write mode
	{
				func = dskWrite;
				switch (objHd->type)
		{
		case Object_Person:
			dbcheckSize(sizeof(*(Person)obj),structSize);

			EndianW(&((Person)obj)->PictID);
			func(file, &((Person)obj)->PictID, sizeof(unsigned short));

			EndianW(&((Person)obj)->Job);
			func(file, &((Person)obj)->Job, sizeof(unsigned short));
			
			EndianW(&((Person)obj)->Sex);
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

			EndianL(&((Person)obj)->TalkBits);
			func(file, &((Person)obj)->TalkBits, sizeof(unsigned long));
			func(file, &((Person)obj)->TalkFileID, sizeof(unsigned char));
			func(file, &((Person)obj)->OldHealth, sizeof(unsigned char));
			break;
		case Object_Player:
			dbcheckSize(sizeof(*(Player)obj),structSize);

			EndianL(&((Player)obj)->Money);
			func(file, &((Player)obj)->Money, sizeof(unsigned long));

			EndianL(&((Player)obj)->StolenMoney);
			func(file, &((Player)obj)->StolenMoney, sizeof(unsigned long));

			EndianL(&((Player)obj)->MyStolenMoney);
			func(file, &((Player)obj)->MyStolenMoney, sizeof(unsigned long));
			func(file, &((Player)obj)->NrOfBurglaries, sizeof(unsigned char));
			func(file, &((Player)obj)->JobOfferCount, sizeof(unsigned char));
			func(file, &((Player)obj)->MattsPart, sizeof(unsigned char));

			EndianL(&((Player)obj)->CurrScene);
			func(file, &((Player)obj)->CurrScene, sizeof(unsigned long));

			EndianL(&((Player)obj)->CurrDay);
			func(file, &((Player)obj)->CurrDay, sizeof(unsigned long));

			EndianL(&((Player)obj)->CurrMinute);
			func(file, &((Player)obj)->CurrMinute, sizeof(unsigned long));

			EndianL(&((Player)obj)->CurrLocation);
			func(file, &((Player)obj)->CurrLocation, sizeof(unsigned long));

			break;
		case Object_Car:
			dbcheckSize(sizeof(*(Car)obj),structSize);

			EndianW(&((Car)obj)->PictID);
			func(file, &((Car)obj)->PictID, sizeof(unsigned short));

			EndianW(&((Car)obj)->Land);
			func(file, &((Car)obj)->Land, sizeof(unsigned short));

			EndianL(&((Car)obj)->Value);
			func(file, &((Car)obj)->Value, sizeof(unsigned long));

			EndianW(&((Car)obj)->YearOfConstruction);
			func(file, &((Car)obj)->YearOfConstruction, sizeof(unsigned short));

			EndianW(&((Car)obj)->ColorIndex);
			func(file, &((Car)obj)->ColorIndex, sizeof(unsigned short));
			func(file, &((Car)obj)->Strike, sizeof(unsigned char));
			
			EndianL(&((Car)obj)->Capacity);
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
			dbcheckSize(sizeof(*(Ability)obj),structSize);

			EndianW(&((Ability)obj)->Name);
			func(file, &((Ability)obj)->Name, sizeof(unsigned short));

			EndianL(&((Ability)obj)->Use);
			func(file, &((Ability)obj)->Use, sizeof(unsigned long));

			break;
		case Object_Tool:
			dbcheckSize(sizeof(*(Tool)obj),structSize);

			EndianW(&((Tool)obj)->PictID);
			func(file, &((Tool)obj)->PictID, sizeof(unsigned short));

			EndianL(&((Tool)obj)->Value);
			func(file, &((Tool)obj)->Value, sizeof(unsigned long));
			func(file, &((Tool)obj)->Danger, sizeof(unsigned char));
			func(file, &((Tool)obj)->Volume, sizeof(unsigned char));
			func(file, &((Tool)obj)->Effect, sizeof(unsigned char));
			break;
		case Object_LSObject:
			dbcheckSize(sizeof(*(LSObject)obj),structSize);

			EndianW(&((LSObject)obj)->us_OffsetFact);
			func(file, &((LSObject)obj)->us_OffsetFact, sizeof(unsigned short));

			EndianW(&((LSObject)obj)->us_DestX);
			func(file, &((LSObject)obj)->us_DestX, sizeof(unsigned short));

			EndianW(&((LSObject)obj)->us_DestY);
			func(file, &((LSObject)obj)->us_DestY, sizeof(unsigned short));
			func(file, &((LSObject)obj)->uch_ExactX, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactY, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactX1, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_ExactY1, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Size, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Visible, sizeof(unsigned char));
			func(file, &((LSObject)obj)->uch_Chained, sizeof(unsigned char));

			EndianL(&((LSObject)obj)->ul_Status);
			func(file, &((LSObject)obj)->ul_Status, sizeof(unsigned long));

			EndianL(&((LSObject)obj)->Type);
			func(file, &((LSObject)obj)->Type, sizeof(unsigned long));

			break;
		case Object_Building:
			dbcheckSize(sizeof(*(Building)obj),structSize);

			EndianL(&((Building)obj)->LocationNr);
			func(file, &((Building)obj)->LocationNr, sizeof(unsigned long));

			EndianW(&((Building)obj)->PoliceTime);
			func(file, &((Building)obj)->PoliceTime, sizeof(unsigned short));

			EndianW(&((Building)obj)->GTime);
			func(file, &((Building)obj)->GTime, sizeof(unsigned short));
			func(file, &((Building)obj)->Exactlyness, sizeof(unsigned char));
			func(file, &((Building)obj)->GRate, sizeof(unsigned char));
			func(file, &((Building)obj)->Strike, sizeof(unsigned char));

			EndianL(&((Building)obj)->Values);
			func(file, &((Building)obj)->Values, sizeof(unsigned long));

			EndianW(&((Building)obj)->EscapeRoute);
			func(file, &((Building)obj)->EscapeRoute, sizeof(unsigned short));
			func(file, &((Building)obj)->EscapeRouteLength, sizeof(unsigned char));
			func(file, &((Building)obj)->RadioGuarding, sizeof(unsigned char));
			func(file, &((Building)obj)->MaxVolume, sizeof(unsigned char));
			func(file, &((Building)obj)->GuardStrength, sizeof(unsigned char));

			EndianW(&((Building)obj)->CarXPos);
			func(file, &((Building)obj)->CarXPos, sizeof(unsigned short));

			EndianW(&((Building)obj)->CarYPos);
			func(file, &((Building)obj)->CarYPos, sizeof(unsigned short));

			if (bProfidisk)
				func(file, &((Building)obj)->DiskId, sizeof(unsigned char));
			break;
		case Object_LSArea:
			dbcheckSize(sizeof(*(LSArea)obj),structSize);

						EndianW(&((LSArea)obj)->us_Coll16ID);
			func(file, &((LSArea)obj)->us_Coll16ID, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_Coll32ID);
			func(file, &((LSArea)obj)->us_Coll32ID, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_Coll48ID);
			func(file, &((LSArea)obj)->us_Coll48ID, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_PlanColl16ID);
			func(file, &((LSArea)obj)->us_PlanColl16ID, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_PlanColl32ID);
			func(file, &((LSArea)obj)->us_PlanColl32ID, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_PlanColl48ID);
			func(file, &((LSArea)obj)->us_PlanColl48ID, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_FloorCollID);
			func(file, &((LSArea)obj)->us_FloorCollID, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_PlanFloorCollID);
			func(file, &((LSArea)obj)->us_PlanFloorCollID, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_Width);
			func(file, &((LSArea)obj)->us_Width, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_Height);
			func(file, &((LSArea)obj)->us_Height, sizeof(unsigned short));

			EndianL(&((LSArea)obj)->ul_ObjectBaseNr);
			func(file, &((LSArea)obj)->ul_ObjectBaseNr, sizeof(unsigned long));
			func(file, &((LSArea)obj)->uch_Darkness, sizeof(unsigned char));

			EndianW(&((LSArea)obj)->us_StartX0);
			func(file, &((LSArea)obj)->us_StartX0, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartX1);
			func(file, &((LSArea)obj)->us_StartX1, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartX2);
			func(file, &((LSArea)obj)->us_StartX2, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartX3);
			func(file, &((LSArea)obj)->us_StartX3, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartX4);
			func(file, &((LSArea)obj)->us_StartX4, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartX5);
			func(file, &((LSArea)obj)->us_StartX5, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartY0);
			func(file, &((LSArea)obj)->us_StartY0, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartY1);
			func(file, &((LSArea)obj)->us_StartY1, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartY2);
			func(file, &((LSArea)obj)->us_StartY2, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartY3);
			func(file, &((LSArea)obj)->us_StartY3, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartY4);
			func(file, &((LSArea)obj)->us_StartY4, sizeof(unsigned short));

			EndianW(&((LSArea)obj)->us_StartY5);
			func(file, &((LSArea)obj)->us_StartY5, sizeof(unsigned short));

			break;

		case Object_Location:
			dbcheckSize(sizeof(*(Location)obj),structSize);

			EndianL(&((Location)obj)->LocationNr);
			func(file, &((Location)obj)->LocationNr, sizeof(unsigned long));

			EndianW(&((Location)obj)->OpenFromMinute);
			func(file, &((Location)obj)->OpenFromMinute, sizeof(unsigned short));
			
			EndianW(&((Location)obj)->OpenToMinute);
			func(file, &((Location)obj)->OpenToMinute, sizeof(unsigned short));
			
			break;
		case Object_Item:
			dbcheckSize(sizeof(*(Item)obj),structSize);

			EndianW(&((Item)obj)->Type);
			func(file, &((Item)obj)->Type, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->OffsetFact);
			func(file, &((Item)obj)->OffsetFact, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->HExactXOffset);
			func(file, &((Item)obj)->HExactXOffset, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->HExactYOffset);
			func(file, &((Item)obj)->HExactYOffset, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->HExactWidth);
			func(file, &((Item)obj)->HExactWidth, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->HExactHeight);
			func(file, &((Item)obj)->HExactHeight, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->VExactXOffset);
			func(file, &((Item)obj)->VExactXOffset, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->VExactYOffset);
			func(file, &((Item)obj)->VExactYOffset, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->VExactWidth);
			func(file, &((Item)obj)->VExactWidth, sizeof(unsigned short));
			
			EndianW(&((Item)obj)->VExactHeight);
			func(file, &((Item)obj)->VExactHeight, sizeof(unsigned short));
			
			func(file, &((Item)obj)->Size, sizeof(unsigned char));
			func(file, &((Item)obj)->ColorNr, sizeof(unsigned char));
			break;

		case Object_Environment:
			dbcheckSize(sizeof(*(Environment)obj),structSize);

			func(file, &((Environment)obj)->MattHasHotelRoom, sizeof(unsigned char));
			func(file, &((Environment)obj)->MattHasIdentityCard, sizeof(unsigned char));
			func(file, &((Environment)obj)->WithOrWithoutYou, sizeof(unsigned char));
			func(file, &((Environment)obj)->MattIsInLove, sizeof(unsigned char));
			func(file, &((Environment)obj)->SouthhamptonHappened, sizeof(unsigned char));
			func(file, &((Environment)obj)->Present, sizeof(unsigned char));
			func(file, &((Environment)obj)->FirstTimeInSouth, sizeof(unsigned char));

			if (bProfidisk)
				func(file, &((Environment)obj)->PostzugDone, sizeof(unsigned char));

			break;
		case Object_London:
			dbcheckSize(sizeof(*(London)obj),structSize);

			func(file, &((London)obj)->Useless, sizeof(unsigned char));
			break;
		case Object_Evidence:
			dbcheckSize(sizeof(*(Evidence)obj),structSize);

			EndianL(&((Evidence)obj)->pers);
			func(file, &((Evidence)obj)->pers, sizeof(unsigned long));

			func(file, &((Evidence)obj)->Recognition, sizeof(unsigned char));
			func(file, &((Evidence)obj)->WalkTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->WaitTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->WorkTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->KillTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->CallTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->PaperTrail, sizeof(unsigned char));
			func(file, &((Evidence)obj)->FotoTrail, sizeof(unsigned char));
			break;
		case Object_Loot:
			dbcheckSize(sizeof(*(Loot)obj),structSize);

			EndianW(&((Loot)obj)->Type);
			func(file, &((Loot)obj)->Type, sizeof(unsigned short));

			EndianW(&((Loot)obj)->Name);
			func(file, &((Loot)obj)->Name, sizeof(unsigned short));

			EndianL(&((Loot)obj)->Volume);
			func(file, &((Loot)obj)->Volume, sizeof(unsigned long));

			EndianW(&((Loot)obj)->Weight);
			func(file, &((Loot)obj)->Weight, sizeof(unsigned short));

			EndianW(&((Loot)obj)->PictID);
			func(file, &((Loot)obj)->PictID, sizeof(unsigned short));

			break;
		case Object_CompleteLoot:
			dbcheckSize(sizeof(*(CompleteLoot)obj),structSize);

			EndianL(&((CompleteLoot)obj)->Bild);
			func(file, &((CompleteLoot)obj)->Bild, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->Gold);
			func(file, &((CompleteLoot)obj)->Gold, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->Geld);
			func(file, &((CompleteLoot)obj)->Geld, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->Juwelen);
			func(file, &((CompleteLoot)obj)->Juwelen, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->Delikates);
			func(file, &((CompleteLoot)obj)->Delikates, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->Statue);
			func(file, &((CompleteLoot)obj)->Statue, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->Kuriositaet);
			func(file, &((CompleteLoot)obj)->Kuriositaet, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->HistKunst);
			func(file, &((CompleteLoot)obj)->HistKunst, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->GebrauchsArt);
			func(file, &((CompleteLoot)obj)->GebrauchsArt, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->Vase);
			func(file, &((CompleteLoot)obj)->Vase, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->TotalWeight);
			func(file, &((CompleteLoot)obj)->TotalWeight, sizeof(unsigned long));

			EndianL(&((CompleteLoot)obj)->TotalVolume);
			func(file, &((CompleteLoot)obj)->TotalVolume, sizeof(unsigned long));

			break;
		case Object_LSOLock:
			dbcheckSize(sizeof(*(LSOLock)obj),structSize);

			EndianW(&((LSOLock)obj)->Type);
			func(file, &((LSOLock)obj)->Type, sizeof(unsigned short));

			break;
		case Object_Police:
			dbcheckSize(sizeof(*(Police)obj),structSize);

			EndianW(&((Police)obj)->PictID);
			func(file, &((Police)obj)->PictID, sizeof(unsigned short));

			func(file, &((Police)obj)->LivingID, sizeof(unsigned char));
			break;
		case Object_LSRoom:
			dbcheckSize(sizeof(*(LSRoom)obj),structSize);

			EndianW(&((LSRoom)obj)->us_LeftEdge);
			func(file, &((LSRoom)obj)->us_LeftEdge, sizeof(unsigned short));

			EndianW(&((LSRoom)obj)->us_TopEdge);
			func(file, &((LSRoom)obj)->us_TopEdge, sizeof(unsigned short));

			EndianW(&((LSRoom)obj)->us_Width);
			func(file, &((LSRoom)obj)->us_Width, sizeof(unsigned short));

			EndianW(&((LSRoom)obj)->us_Height);
			func(file, &((LSRoom)obj)->us_Height, sizeof(unsigned short));

			break;
		default:
			//func(file, obj, objHd->size);
			Log("Error: Unknown object type %d |dbLoadOrSaveObject" ,objHd->type);
			break;
		}
	}
	return(1);
}

void	dbcheckSize(ulong objSize, ulong structSize)
{
    if (objSize != structSize)
    { 
        Log("Error: wrong size (%d != %d) |dbLoadOrSaveObject", objSize, structSize);
    }
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

			dskRead(fh, &objHd.nr,sizeof(unsigned long));
			dskRead(fh, &objHd.type,sizeof(unsigned long));
			dskRead(fh, &objHd.size,sizeof(unsigned long));

			EndianL(&objHd.nr);
			EndianL(&objHd.type);
			EndianL(&objHd.size);


			if ((objHd.nr != 0xffffffff) && (objHd.type != 0xffffffff) && (objHd.size != 0xffffffff))
			{
				LIST *list = NULL;
				void *obj;
				char *name = NULL;
				ulong objSize = dbGetObjLoadSize(objHd.type);

				if (ObjectLoadMode)
				{
					if (list = txtGoKey(OBJECTS_TXT, NULL))      // MOD: old version GoNextKey
					{
						name = NODE_NAME(LIST_HEAD(list));
					}
				}


				if (!(obj = dbNewObject(objHd.nr, objHd.type, objSize, name, realNr++)))
				{
					dskClose(fh);
					dbDeleteAllObjects(0L,0L);
					return 0;
				}

				if (list)
				{
					RemoveList(list);
				}

				dbLoadOrSaveObject(&objHd, obj, fh, objSize,0);
			}
		}

		dskClose(fh);

		return 1;
	}

	return 0;
}

ulong dbGetObjLoadSize(ulong structType)
{
	switch (structType) 
	{
	case Object_Item:
		return Object_Item_Size;
	case Object_London:
		return Object_London_Size;
	case Object_Evidence:
		return Object_Evidence_Size;
	case Object_Location:
		return Object_Location_Size;
	case Object_Person:
		return Object_Person_Size;
	case Object_Car:
		return Object_Car_Size;
	case Object_Tool:
		return Object_Tool_Size;
	case Object_CompleteLoot:
		return Object_CompleteLoot_Size;
	case Object_Ability:
		return Object_Ability_Size;
	case Object_Environment:
		return Object_Environment_Size;
	case Object_Player:
		return Object_Player_Size;
	case Object_LSOLock:
		return Object_LSOLock_Size;
	case Object_LSObject:
		return Object_LSObject_Size;
	case Object_Loot:
		return Object_Loot_Size;
	case Object_LSRoom:
		return Object_LSRoom_Size;
	case Object_Building:
		return Object_Building_Size;
	case Object_Police:
		return Object_Police_Size;
	case Object_LSArea:
		return Object_LSArea_Size;
	default:
		return 0;
	}
}

ulong dbGetObjSaveSize(struct dbObjectHeader *objHd)
{
   switch (objHd->type) 
	{
	case Object_Item:
		return 22;
	case Object_London:
		return 1;
	case Object_Evidence:
		return 12;
	case Object_Location:
		return 8;
	case Object_Person:
		return 25;
	case Object_Car:
		return 24;
	case Object_Tool:
		return 9;
	case Object_CompleteLoot:
		return 48;
	case Object_Ability:
		return 6;
	case Object_Environment:
		if (bProfidisk)
		{
			return 8;
		}
		else
		{
			return 7;
		}
	case Object_Player:
		return 31;
	case Object_LSOLock:
		return 2;
	case Object_LSObject:
		return 21;
	case Object_Loot:
		return 12;
	case Object_LSRoom:
		return 8;
	case Object_Building:
		if (bProfidisk)
		{
			return 26;
		}
		else
		{
			return 25;
		}
	case Object_Police:
		return 3;
	case Object_LSArea:
		return 49;
	default:
		return 0;
	} 
}

ubyte dbSaveAllObjects(char *fileName, ulong offset, ulong size, uword diskId)
{
	FILE *fh;
	struct dbObject *obj;
	ulong realNr = 1;
	ulong dbSize = dbGetObjectCountOfDB(offset, size);
	ulong objSize;

	if (fh = dskOpen(fileName, "wb", diskId))
	{
		while (realNr <= dbSize)
		{
			if (obj = dbFindRealObject (realNr++, offset, size))
			{
				struct dbObjectHeader objHd;

				objHd.nr   = obj->nr;
				objHd.type = obj->type;
				objHd.size = dbGetObjSaveSize(&objHd);
				
				objSize = NODE_SIZE(obj) - sizeof(struct dbObject);

				EndianL(&objHd.nr);
				EndianL(&objHd.type);
				EndianL(&objHd.size);

				dskWrite(fh, &objHd.nr, sizeof(objHd.nr));
				dskWrite(fh, &objHd.type, sizeof(objHd.type));
				dskWrite(fh, &objHd.size, sizeof(objHd.size));

				if ((objHd.nr != 0xffffffff) && (objHd.type != 0xffffffff) && (objHd.size != 0xffffffff))
				{
					dbLoadOrSaveObject(&objHd,obj+1,fh, objSize, 1);
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
		ObjectListPrivate = (LIST*)CreateList(0L);
	else
	{
		RemoveList(ObjectList);
		ObjectList = (LIST*)CreateList(0L);
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
	LIST *newList = (LIST*)CreateList(0L);
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
	struct ObjectNode *n1, *n2, *pred, *newNode;
	long i = 0;

	if (!LIST_EMPTY(*objectList))
	{
		newList = (LIST*)CreateList(0L);

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

         newNode = (struct ObjectNode *)CreateNode(NULL, sizeof(struct ObjectNode), NODE_NAME(n1));
         newNode->nr = n1->nr;
         newNode->type = n1->type;
         newNode->data = n1->data;

         if (pred)
         {
				if (pred == (struct ObjectNode *) LIST_HEAD(newList))
					AddHeadNode(newList, newNode);
				else
					AddNode(newList, newNode, NODE_PRED(pred));
			}
			else
				AddTailNode(newList, newNode);
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

   if (!(ObjectList = (LIST*)CreateList(0L)))
	NewErrorMsg(No_Mem, __FILE__, __func__, 3);

   for (objHashValue = 0; objHashValue < OBJ_HASH_SIZE; objHashValue++)
   {
      if (!(objHash[objHashValue] = (LIST*)CreateList(0L)))
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

/* Functions for the return of the correct value 
for the double assigned values of std and profi version */
ulong getValue(DoubleAssignValueE val)
{
	switch (val) 
	{
	case _Ability_Kampf:
		if (bProfidisk) 
			return 179;
		else
			return 146;
	case _Location_Cars_Vans_Office:
		if (bProfidisk) 
			return 100;
		else
			return  82;
	case _London_London_1:
		if (bProfidisk) 
			return  30;
		else 
			return  22;
	case _Location_Fat_Mans_Pub:
		if (bProfidisk) 
			return  99;
		else
			return  81;
	case _Player_Player_1:
		if (bProfidisk) 
			return  29;
		else 
			return  21;
	case _Ability_Sprengstoff:
		if (bProfidisk) 
			return 175;
		else
			return 142;
	case _Location_Highgate_Out:
		if (bProfidisk) 
			return 116;
		else
			return  98;
	case _Ability_Elektronik:
		if (bProfidisk) 
			return 177;
		else
			return 144;
	case _Location_Holland_Street:
		if (bProfidisk) 
			return  95;
		else
			return  77;
	case _Location_Parker:
		if (bProfidisk) 
			return 146;
		else
			return 128;
	case _London_Escape:
		if (bProfidisk) 
			return  32;
		else
			return  24;
	case _Location_Walrus:
		if (bProfidisk) 
			return 143;
		else
			return 125;
	case _Location_Maloya:
		if (bProfidisk) 
			return 147;
		else
			return 129;
	case _Location_Nirvana:
		if (bProfidisk) 
			return 151;
		else
			return 133;
	case _Location_Policestation:
		if (bProfidisk) 
			return 102;
		else
			return  84;
	case _Ability_Aufpassen:
		if (bProfidisk) 
			return 178;
		else
			return 145;
	case _Ability_Schloesser:
		if (bProfidisk) 
			return 174;
		else
			return 141;
	case _Ability_Safes:
		if (bProfidisk) 
			return 176;
		else
			return 143;
	case _CompleteLoot_LastLoot:
		if (bProfidisk) 
			return  35;
		else
			return  27;
	case _London_Jail:
		if (bProfidisk) 
			return  31;
		else
			return  23;
	case _Location_Hotel:
		if (bProfidisk) 
			return 142;
		else
			return 124;
	case _Person_Old_Matt:
		if (bProfidisk) 
			return  36;
		else
			return  28;
	case _Ability_Autos:
		if (bProfidisk) 
			return 173;
		else
			return 140;
	case _Environment_TheClou:
		if (bProfidisk) 
			return  28;
		else 
			return  20;
	case _Evidence_Evidence_1:
		if (bProfidisk) 
			return  33;
		else
			return  25;
	case _Location_Tools_Shop:
		if (bProfidisk) 
			return 101;
		else
			return  83;
	case _Location_Pooly:
		if (bProfidisk) 
			return 148;
		else
			return 130;
	default:
		return 0;
	}
}