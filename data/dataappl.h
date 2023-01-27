/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_DATAAPPL
#define MODULE_DATAAPPL

#include "theclou.h"

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_RELATION
#include "data\relation.h"
#endif

#ifndef MODULE_DATABASE
#include "data\database.h"
#endif

#ifndef MODULE_DATACALC
#include "data\datacalc.h"
#endif

#ifndef MODULE_TCDATA
#include "data\tcdata.h"
#endif

#ifndef MODULE_EVIDENCE
#include "scenes\evidence.h"
#endif

#ifndef MODULE_LIVING
#include "living\living.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap\landscap.h"
#endif

extern ubyte tcSpendMoney(long money,ubyte breakAnim);

extern ulong GetObjNrOfLocation(ulong LocNr);
extern ulong GetObjNrOfBuilding(ulong LocNr);

extern void	 tcCalcCallValue(ulong callNr, ulong timer, ulong persId);
extern void  tcPersonLearns(ulong pId);
extern void  tcRefreshTimeClock(ulong buildId, ulong timerId);
extern ulong tcGetPersOffer(Person person, ubyte persCount);

extern long  tcCalcEscapeTime(void);
extern long  tcKillTheGuard(ulong guyId, ulong buildingId);
extern long  tcAlarmByPowerLoss (ulong powerId);
extern long  tcAlarmByTouch(ulong lsoId);
extern long  tcGuardDetectsGuy(LIST *roomsList, uword us_XPos, uword us_YPos, ubyte uch_ViewDirection, char *puch_GuardName, char *puch_LivingName);
extern long  tcGetCarStrike(Car car);
extern long  tcCalcEscapeOfTeam(void);
extern long  tcGuyTellsAll(Person p);
extern long  tcGuyCanEscape(Person p);
extern long  tcGetCarTraderOffer(Car car);
extern long  tcGetTeamMood(ulong *guyId, ulong timer);   /* ptr auf 4 ulongs */
extern long  tcGuyInAction(ulong persId, long exhaustion);
extern long  tcGuyIsWaiting(ulong persId, long exhaustion);
extern long  tcGetTrail(Person p, ubyte which);
extern long  tcGetDanger(ulong persId, ulong toolId, ulong itemId);
extern long  tcGetToolLoudness(ulong persId, ulong toolId, ulong itemId);
extern long  tcGetWalkLoudness(void);
extern long  tcGetTotalLoudness(long loudp0, long loudp1, long loudp2, long loudp3);
extern long  tcAlarmByLoudness(Building b, long totalLoudness);
extern long  tcAlarmByRadio(Building b);
extern long  tcAlarmByMicro(uword us_XPos, uword us_YPos, long loudness);
extern long  tcAlarmByPatrol(Building b, uword objChangedCount, uword totalCount, ubyte patrolCount);
extern long  tcWatchDogWarning(ulong persId);
extern long  tcWrongWatchDogWarning(ulong persId);
extern long  tcIsCarRecognised(Car car, long time);
extern long  tcGuardChecksObject(LSObject lso);
extern long  tcCheckTimeClocks(ulong builId);

extern long  tcCalcMattsPart(void);

extern ulong tcGuyEscapes(void);
extern ulong tcGuyUsesTool(ulong persId, Building b, ulong toolId, ulong itemId);
extern ulong tcGuyUsesToolInPlayer(ulong persId, Building b, ulong toolId, ulong itemId, ulong neededTime);

extern void  tcInsertGuard(LIST *list, LIST *roomsList, uword x, uword y, uword width, uword height, ulong guardId, ubyte livId, ulong areaId);

/* defines for Persons */
#define   tcPERSON_IS_ARRESTED          240 /* KnownToPolice */
#define   tcVALUE_OF_RING_OF_PATER      320

#define   tcSetPersKnown(p,v)           ((p)->Known = (v))
#define   tcChgPersPopularity(p, v)     ((p)->Popularity = (ubyte) ChangeAbs((p)->Popularity, v, 0, 255))

#define   tcGetPersHealth(p)            ((p)->Health)
#define   tcGetPersMood(p)              (((Person)p)->Mood)

#define   tcWeightPersCanCarry(p)       (((long)(p)->Stamina + (long)(p)->Strength) * 200)     /* 0 - 100000 (100 kg) in gramm ! */
#define   tcVolumePersCanCarry(p)       (((long)(p)->Stamina + (long)(p)->Strength) * 200)  /* in cm3 -> max 1 m3 fr eine Person */

#define   tcImprovePanic(p, v)          ((p)->Panic = (p)->Panic - ((p)->Panic / (v)))
#define   tcImproveKnown(p, v)          ((p)->Known = min(255, (v)))

/* defines for object Car */
#define   tcENGINE_PERC                 50   /* 50 % of costs of a car is engine   */
#define   tcBODY_PERC                   40   /* 40 % of costs of a car is bodywork */
#define   tcTYRE_PERC                   10   /* 10 % of costs of a car are tyres   */

#define   tcCalcCarState(car)           (car->State = ( (long)(car->MotorState) + (long)(car->BodyWorkState) + (long)(car->TyreState)) /3)

#define   tcCostsPerEngineRepair(car)   (max(((car->Value * tcENGINE_PERC) / 25500), 2))
#define   tcCostsPerBodyRepair(car)     (max(((car->Value * tcBODY_PERC  ) / 25500), 2))
#define   tcCostsPerTyreRepair(car)     (max(((car->Value * tcTYRE_PERC  ) / 25500), 1))
#define   tcCostsPerTotalRepair(car)    (max(((car->Value) / 255), 5))
#define   tcColorCosts(car)             (max((Round(car->Value / 200,1)), 1))

#define   tcGetCarSpeed(car)            (car->Speed)
#define   tcGetCarPS(car)               (car->PS)

#define   tcRGetCarAge(car)             (max((GetDay /365) - (car->YearOfConstruction), 1))
#define   tcRGetCarValue(car)           (car->Value)

#define   tcGetCarPrice(car)            (Round(max(((car->Value) * (car->State)) / 255, 100), 1))
#define   tcGetCarTotalState(car)       (((long)car->MotorState + (long)car->BodyWorkState + (long)car->TyreState) / 3)

#define   tcGetDealerPerc(d, p)         (min((p + (((long)p * ((long)127L - (long)d->Known)) / (long)1270)), 255))
#define   tcGetDealerOffer(v, p)        (((long)v * (long)p) / (long)255)
#define   tcAddDealerSymp(d, v)         ((d)->Known = ChangeAbs ((d)->Known, v, 0, 255))

#define   tcSetCarMotorState(car,v)     {car->MotorState = (ubyte)ChangeAbs((long)car->MotorState,(long)v, 0, 255); tcCalcCarState(car);}
#define   tcSetCarBodyState(car,v)      {car->BodyWorkState = (ubyte)ChangeAbs((long)car->BodyWorkState,(long)v, 0, 255); tcCalcCarState(car);}
#define   tcSetCarTyreState(car,v)      {car->TyreState = (ubyte)ChangeAbs((long)car->TyreState,(long)v, 0, 255); tcCalcCarState(car);}

/* defines for object Player */
#define   tcGetPlayerMoney              (((Player)dbGetObject(Player_Player_1))->Money)
#define   tcSetPlayerMoney(amount)      (((Player)dbGetObject(Player_Player_1))->Money = amount)
#define   tcAddPlayerMoney(amount)      (((Player)dbGetObject(Player_Player_1))->Money = (tcGetPlayerMoney + amount))

/* defines for object Building */
#define   tcRGetGRate(bui)              (bui->GRate)
#define   tcRGetBuildStrike(bui)        (bui->Strike)

#define   tcGetBuildPoliceT(bui)        (bui->PoliceTime)
#define   tcGetBuildGRate(bui)          (bui->GRate)

ulong     tcGetBuildValues(Building bui);

#define   tcAddBuildExactlyness(bui,v)  (bui->Exactlyness = (ubyte)ChangeAbs((long)bui->Exactlyness,(long)v, 0, 255))
#define   tcAddBuildStrike(bui,v)       (bui->Strike = (ubyte)ChangeAbs((long)bui->Strike,(long)v, 0, 255))

/* defines for object Tool */
#define   tcRGetDanger(tool)            (tool->Danger)
#define   tcRGetVolume(tool)            (tool->Volume)

#define   tcGetToolPrice(tool)          (tool->Value)
#define   tcGetToolTraderOffer(tool)    (max((tcGetToolPrice(tool) - (tcGetToolPrice(tool) / tcDerivation(10,20))), 1))

/* costs */
#define   tcCOSTS_FOR_HOTEL             10

/* Plan */
#define   tcEscapeTime(building)        (16400 / building->EscapeRouteLength)

#endif
