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
#include "list/list.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "data/database.h"
#endif

#ifndef MODULE_DATACALC
#include "data/datacalc.h"
#endif

#ifndef MODULE_TCDATA
#include "data/tcdata.h"
#endif

#ifndef MODULE_EVIDENCE
#include "scenes/evidence.h"
#endif

#ifndef MODULE_LIVING
#include "living/living.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap/landscap.h"
#endif

extern ubyte tcSpendMoney(int32_t money,ubyte breakAnim);

extern uint32_t GetObjNrOfLocation(uint32_t LocNr);
extern uint32_t GetObjNrOfBuilding(uint32_t LocNr);

extern void	 tcCalcCallValue(uint32_t callNr, uint32_t timer, uint32_t persId);
extern void  tcPersonLearns(uint32_t pId);
extern void  tcRefreshTimeClock(uint32_t buildId, uint32_t timerId);
extern uint32_t tcGetPersOffer(Person person, ubyte persCount);

extern int32_t  tcCalcEscapeTime(void);
extern int32_t  tcKillTheGuard(uint32_t guyId, uint32_t buildingId);
extern int32_t  tcAlarmByPowerLoss (uint32_t powerId);
extern int32_t  tcAlarmByTouch(uint32_t lsoId);
extern int32_t  tcGuardDetectsGuy(LIST *roomsList, uword us_XPos, uword us_YPos, ubyte uch_ViewDirection, char *puch_GuardName, char *puch_LivingName);
extern int32_t  tcGetCarStrike(Car car);
extern int32_t  tcCalcEscapeOfTeam(void);
extern int32_t  tcGuyTellsAll(Person p);
extern int32_t  tcGuyCanEscape(Person p);
extern int32_t  tcGetCarTraderOffer(Car car);
extern int32_t  tcGetTeamMood(uint32_t *guyId, uint32_t timer);   /* ptr auf 4 uint32_ts */
extern int32_t  tcGuyInAction(uint32_t persId, int32_t exhaustion);
extern int32_t  tcGuyIsWaiting(uint32_t persId, int32_t exhaustion);
extern int32_t  tcGetTrail(Person p, ubyte which);
extern int32_t  tcGetDanger(uint32_t persId, uint32_t toolId, uint32_t itemId);
extern int32_t  tcGetToolLoudness(uint32_t persId, uint32_t toolId, uint32_t itemId);
extern int32_t  tcGetWalkLoudness(void);
extern int32_t  tcGetTotalLoudness(int32_t loudp0, int32_t loudp1, int32_t loudp2, int32_t loudp3);
extern int32_t  tcAlarmByLoudness(Building b, int32_t totalLoudness);
extern int32_t  tcAlarmByRadio(Building b);
extern int32_t  tcAlarmByMicro(uword us_XPos, uword us_YPos, int32_t loudness);
extern int32_t  tcAlarmByPatrol(Building b, uword objChangedCount, uword totalCount, ubyte patrolCount);
extern int32_t  tcWatchDogWarning(uint32_t persId);
extern int32_t  tcWrongWatchDogWarning(uint32_t persId);
extern int32_t  tcIsCarRecognised(Car car, int32_t time);
extern int32_t  tcGuardChecksObject(LSObject lso);
extern int32_t  tcCheckTimeClocks(uint32_t builId);

extern int32_t  tcCalcMattsPart(void);

extern uint32_t tcGuyEscapes(void);
extern uint32_t tcGuyUsesTool(uint32_t persId, Building b, uint32_t toolId, uint32_t itemId);
extern uint32_t tcGuyUsesToolInPlayer(uint32_t persId, Building b, uint32_t toolId, uint32_t itemId, uint32_t neededTime);

extern void  tcInsertGuard(LIST *list, LIST *roomsList, uword x, uword y, uword width, uword height, uint32_t guardId, ubyte livId, uint32_t areaId);

/* defines for Persons */
#define   tcPERSON_IS_ARRESTED          240 /* KnownToPolice */
#define   tcVALUE_OF_RING_OF_PATER      320

#define   tcSetPersKnown(p,v)           ((p)->Known = (v))
#define   tcChgPersPopularity(p, v)     ((p)->Popularity = (ubyte) ChangeAbs((p)->Popularity, v, 0, 255))

#define   tcGetPersHealth(p)            ((p)->Health)
#define   tcGetPersMood(p)              (((Person)p)->Mood)

#define   tcWeightPersCanCarry(p)       (((int32_t)(p)->Stamina + (int32_t)(p)->Strength) * 200)     /* 0 - 100000 (100 kg) in gramm ! */
#define   tcVolumePersCanCarry(p)       (((int32_t)(p)->Stamina + (int32_t)(p)->Strength) * 200)  /* in cm3 -> max 1 m3 für eine Person */

#define   tcImprovePanic(p, v)          ((p)->Panic = (p)->Panic - ((p)->Panic / (v)))
#define   tcImproveKnown(p, v)          ((p)->Known = min(255, (v)))

/* defines for object Car */
#define   tcENGINE_PERC                 50   /* 50 % of costs of a car is engine   */
#define   tcBODY_PERC                   40   /* 40 % of costs of a car is bodywork */
#define   tcTYRE_PERC                   10   /* 10 % of costs of a car are tyres   */

#define   tcCalcCarState(car)           (car->State = ( (int32_t)(car->MotorState) + (int32_t)(car->BodyWorkState) + (int32_t)(car->TyreState)) /3)

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
#define   tcGetCarTotalState(car)       (((int32_t)car->MotorState + (int32_t)car->BodyWorkState + (int32_t)car->TyreState) / 3)

#define   tcGetDealerPerc(d, p)         (min((p + (((int32_t)p * ((int32_t)127L - (int32_t)d->Known)) / (int32_t)1270)), 255))
#define   tcGetDealerOffer(v, p)        (((int32_t)v * (int32_t)p) / (int32_t)255)
#define   tcAddDealerSymp(d, v)         ((d)->Known = ChangeAbs ((d)->Known, v, 0, 255))

#define   tcSetCarMotorState(car,v)     {car->MotorState = (ubyte)ChangeAbs((int32_t)car->MotorState,(int32_t)v, 0, 255); tcCalcCarState(car);}
#define   tcSetCarBodyState(car,v)      {car->BodyWorkState = (ubyte)ChangeAbs((int32_t)car->BodyWorkState,(int32_t)v, 0, 255); tcCalcCarState(car);}
#define   tcSetCarTyreState(car,v)      {car->TyreState = (ubyte)ChangeAbs((int32_t)car->TyreState,(int32_t)v, 0, 255); tcCalcCarState(car);}

/* defines for object Player */
#define   tcGetPlayerMoney              (((Player)dbGetObject(Player_Player_1))->Money)
#define   tcSetPlayerMoney(amount)      (((Player)dbGetObject(Player_Player_1))->Money = amount)
#define   tcAddPlayerMoney(amount)      (((Player)dbGetObject(Player_Player_1))->Money = (tcGetPlayerMoney + amount))

/* defines for object Building */
#define   tcRGetGRate(bui)              (bui->GRate)
#define   tcRGetBuildStrike(bui)        (bui->Strike)

#define   tcGetBuildPoliceT(bui)        (bui->PoliceTime)
#define   tcGetBuildGRate(bui)          (bui->GRate)

uint32_t     tcGetBuildValues(Building bui);

#define   tcAddBuildExactlyness(bui,v)  (bui->Exactlyness = (ubyte)ChangeAbs((int32_t)bui->Exactlyness,(int32_t)v, 0, 255))
#define   tcAddBuildStrike(bui,v)       (bui->Strike = (ubyte)ChangeAbs((int32_t)bui->Strike,(int32_t)v, 0, 255))

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
