/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_EVIDENCE
#define MODULE_EVIDENCE

#include "theclou.h"

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_DIALOG
#include "dialog\dialog.h"
#endif

#ifndef MODULE_INTERAC
#include "present\interac.h"
#endif

#ifndef MODULE_PRESENT
#include "present\present.h"
#endif

#ifndef MODULE_TCDATA
#include "data\tcdata.h"
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

#define FAHN_NOT_ESCAPED      1
#define FAHN_ESCAPED          2

#define FAHN_ALARM            1L<<0
#define FAHN_ALARM_LOUDN      1L<<1
#define FAHN_ALARM_RADIO      1L<<2
#define FAHN_ALARM_PATRO      1L<<3
#define FAHN_SURROUNDED       1L<<4
#define FAHN_ESCAPE           1L<<5
#define FAHN_QUIET_ALARM      1L<<6
#define FAHN_STD_ESCAPE       1L<<7     /* wenn Einbruch glatt geht (Standardflucht) */
#define FAHN_ALARM_ALARM      1L<<8
#define FAHN_ALARM_TIMECLOCK  1L<<9
#define FAHN_ALARM_POWER      1L<<10
#define FAHN_ALARM_TIMER      1L<<11
#define FAHN_ALARM_MICRO      1L<<12
#define FAHN_ALARM_GUARD      1L<<13

struct Search	/* wegen Arrays nicht in objects */
	{
	word	GuyXPos[4], GuyYPos[4];  /* letzte Position im Falle einer Flucht! */

	ubyte	Exhaust[4];

	uword	WalkTime[4];
	uword	WaitTime[4];
	uword	WorkTime[4];
	uword	KillTime[4];

	ulong DeriTime;         /* Abweichung vom Soll */

	ulong	TimeOfBurglary;   /* Zeitpunkt! */
	ulong TimeOfAlarm;      /* Zeitpunkt! */

	ulong BuildingId;       /* Einbruch */
	ulong LastAreaId;       /* zum Zeitpunkt der Flucht */

	ulong EscapeBits;

	word  CallValue;			/* "Wert" der Funkspr., word!!! */
	uword	CallCount;			/* Anzahl d. FUnkspr. */

	uword	WarningCount;		/* Anzahl der Warnungen */
	ubyte	SpotTouchCount[4];

	ubyte KaserneOk;        /* ob die Kaserne geschafft wurde! */
	};

extern long tcCalcCarEscape(long timeLeft);
extern void tcForgetGuys(void);

extern struct Search Search;

#endif
