/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_SCENES
#define MODULE_SCENES

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_INTERAC
#include "present/interac.h"
#endif

#ifndef MODULE_PRESENT
#include "present/present.h"
#endif

#ifndef MODULE_DIALOG
#include "dialog/dialog.h"
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

#ifndef MODULE_DATAAPPL
#include "data/dataappl.h"
#endif

#ifndef MODULE_TCDATA
#include "data/tcdata.h"
#endif

#ifndef MODULE_GAMEPLAY_APP
#include "gameplay/gp_app.h"
#endif

#ifndef MODULE_RASTER
#include "landscap/raster.h"
#endif

extern uword	CurrentBackground;

/* standard scene functions - look at scenes.c */

extern uint32_t Go(LIST *succ);
extern uint32_t tcTelefon(void);

extern void Look(uint32_t locNr);
extern void Information(void);
extern void tcWait(void);

/* taxi functions */

extern void AddTaxiLocation(uint32_t locNr);
extern void RemTaxiLocation(uint32_t locNr);

/* Done Funktionen */

void DoneHotelRoom(void);
void DoneTaxi(void);
void DoneGarage(void);
void DoneParking(void);
void DoneTools(void);
void DoneDealer(void);
void DoneInsideHouse(void);

void tcInitFahndung(void);
void tcDoneFahndung(void);

/* Hilfsfunktionen */

void Investigate(char *location);

void SetCarColors(ubyte index);

void tcSellCar(uint32_t ObjectID);
void tcColorCar(Car car);
void tcRepairCar(Car car,char *repairWhat);
void tcCarGeneralOverhoul(Car car);
void tcToolsShop(void);

int32_t tcEscapeFromBuilding(uint32_t escBits);
int32_t tcEscapeByCar(uint32_t escBits, int32_t timeLeft);

uint32_t tcChooseCar(uint32_t backgroundNr);

LIST  *tcMakeLootList(uint32_t containerID, uint32_t relID);

char *tcShowPriceOfCar(uint32_t nr, uint32_t type, void *data);
char *tcShowPriceOfTool(uint32_t nr, uint32_t type, void *data);

void tcDealerSays(Person dealer, ubyte textNr, int32_t perc);
void tcDealerOffer(Person dealer, ubyte which);

void  tcBuyCar(void);
void  tcCarInGarage(uint32_t carID);

ubyte tcBuyTool(ubyte choice);
void  tcSellTool(void);
ubyte tcShowTool(ubyte choice);
ubyte tcDescTool(ubyte choice);
void  tcDealerDlg(void);

uint32_t tcStartEvidence(void);
uint32_t tcGoInsideOfHouse(uint32_t buildingID);

void  tcShowLootInfo(Loot loot);
void  tcInsideOfHouse(uint32_t buildingID, uint32_t areaID, ubyte perc);
void  tcShowObjectData(uint32_t areaID, NODE *node, ubyte perc);

#endif
