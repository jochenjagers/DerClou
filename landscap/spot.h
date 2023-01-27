/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_SPOT
#define MODULE_SPOT

#include "theclou.h"

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_GFX
#include "gfx\gfx.h"
#endif

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap\landscap.h"
#endif

#define LS_SPOT_NAME					((ubyte*)"s")

#define LS_SPOT_SMALL_SIZE         16
#define LS_SPOT_MEDIUM_SIZE        32
#define LS_SPOT_LARGE_SIZE         48

#define LS_SPOT_ON                  1
#define LS_SPOT_OFF                 2

#define LS_ALL_VISIBLE_SPOTS        1
#define LS_ALL_INVISIBLE_SPOTS      2

struct SpotPosition {
	NODE	Link;

	uword	us_XPos;
	uword	us_YPos;
};

struct Spot {
	NODE	Link;

	uword	us_Size;
	uword	us_Speed;	     /* secconds per move */

	ulong 	ul_CtrlObjId;  /* data : objId */

	ulong	ul_AreaId;

	ubyte	uch_Status;
	ubyte	Padding;

	uword	us_OldXPos;
	uword	us_OldYPos;

	uword	us_PosCount;

	LIST *	p_positions;

	struct SpotPosition *p_CurrPos; /* for fast access */
};

extern void lsInitSpots(struct RastPort *rp);
extern void lsDoneSpots(void);

extern void lsMoveAllSpots(ulong time);
extern void lsShowAllSpots(ulong time, ulong mode);

extern void lsSetSpotStatus(ulong CtrlObjId, ubyte uch_Status);

extern void lsLoadSpots(ulong bldId, char *uch_FileName);
extern void lsWriteSpots(char *uch_FileName);
extern void lsRemSpot(struct Spot *spot);

extern LIST *lsGetSpotList(void);

#endif
