/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_SCENES
#define MODULE_SCENES

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_INTERAC
#include "present\interac.h"
#endif

#ifndef MODULE_PRESENT
#include "present\present.h"
#endif

#ifndef MODULE_DIALOG
#include "dialog\dialog.h"
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

#ifndef MODULE_DATAAPPL
#include "data\dataappl.h"
#endif

#ifndef MODULE_TCDATA
#include "data\tcdata.h"
#endif

#ifndef MODULE_GAMEPLAY_APP
#include "gameplay\gp_app.h"
#endif

#ifndef MODULE_RASTER
#include "landscap\raster.h"
#endif

extern uword	CurrentBackground;

/* standard scene functions - look at scenes.c */

extern ulong Go(LIST *succ);
extern ulong tcTelefon(void);

extern void Look(ulong locNr);
extern void Information(void);
extern void tcWait(void);

/* taxi functions */

extern void AddTaxiLocation(ulong locNr);
extern void RemTaxiLocation(ulong locNr);

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

void tcSellCar(ulong ObjectID);
void tcColorCar(Car car);
void tcRepairCar(Car car,char *repairWhat);
void tcCarGeneralOverhoul(Car car);
void tcToolsShop(void);

long tcEscapeFromBuilding(ulong escBits);
long tcEscapeByCar(ulong escBits, long timeLeft);

ulong tcChooseCar(ulong backgroundNr);

LIST  *tcMakeLootList(ulong containerID, ulong relID);

char *tcShowPriceOfCar(ulong nr, ulong type, void *data);
char *tcShowPriceOfTool(ulong nr, ulong type, void *data);

void tcDealerSays(Person dealer, ubyte textNr, long perc);
void tcDealerOffer(Person dealer, ubyte which);

void  tcBuyCar(void);
void  tcCarInGarage(ulong carID);

ubyte tcBuyTool(ubyte choice);
void  tcSellTool(void);
ubyte tcShowTool(ubyte choice);
ubyte tcDescTool(ubyte choice);
void  tcDealerDlg(void);

ulong tcStartEvidence(void);
ulong tcGoInsideOfHouse(ulong buildingID);

void  tcShowLootInfo(Loot loot);
void  tcInsideOfHouse(ulong buildingID, ulong areaID, ubyte perc);
void  tcShowObjectData(ulong areaID, NODE *node, ubyte perc);

#endif
