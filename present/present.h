/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PRESENT
#define MODULE_PRESENT

#include <math.h>

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

#ifndef MODULE_DATABASE
#include "data\database.h"
#endif

#ifndef MODULE_RELATION
#include "data\relation.h"
#endif

#ifndef MODULE_TCDATA
#include "data\tcdata.h"
#endif

#ifndef MODULE_DATAAPPL
#include "data\dataappl.h"
#endif

#ifndef MODULE_SCENES
#include "scenes\scenes.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap\landscap.h"
#endif

#define PRESENT_AS_TEXT       1
#define PRESENT_AS_BAR        2
#define PRESENT_AS_NUMBER     3

extern ubyte Present (ulong nr, const char *presentationText,void (*initPresentation)(ulong,LIST *,LIST*));

extern void InitPersonPresent(ulong nr,LIST *presentationData,LIST *texts);
extern void InitCarPresent(ulong nr,LIST *presentationData,LIST *texts);
extern void InitPlayerPresent(ulong nr,LIST *presentationData,LIST *texts);
extern void InitBuildingPresent(ulong nr,LIST *presentationData,LIST *texts);
extern void InitToolPresent(ulong nr,LIST *presentationData,LIST *texts);
extern void InitObjectPresent(ulong nr, LIST *presentationData, LIST *texts);
extern void InitEvidencePresent(ulong nr, LIST *presentationData, LIST *texts);
extern void InitLootPresent(ulong nr, LIST *presentationData, LIST *texts);
extern void InitOneLootPresent(ulong nr, LIST *presentationData, LIST *texts);

extern void prSetBarPrefs(struct RastPort *p_RP,uword us_BarWidth,uword us_BarHeight,ubyte uch_FCol,ubyte uch_BCol, ubyte uch_TCol);
extern void prDrawTextBar(char *puch_Text,ulong ul_Value,ulong ul_Max,uword us_XPos,uword us_YPos);

extern void DrawPresent (LIST *present, ubyte firstLine, struct RastPort *rp, ubyte max);
extern void AddPresentLine(LIST *l,ubyte presentHow,ulong data,ulong max,LIST *texts,uword textNr);

#endif
