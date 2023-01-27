/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PLANING_GUARDS
#define MODULE_PLANING_GUARDS

#include <stdio.h>
#include "theclou.h"

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#ifndef MODULE_LIST
#include "list\list.h"
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

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap\landscap.h"
#endif

#ifndef MODULE_PLANING_PLANER
#include "planing\planer.h"
#endif


/* method definition */
#define GUARDS_DO_SAVE     1
#define GUARDS_DO_LOAD     2

/* disk definition */
#define GUARD_DISK         0
#define GUARD_EXTENSION    ".gua"
#define GUARD_DIRECTORY    DATA_DIRECTORY


// main method
void grdDo(FILE *fh, struct System *sys, LIST *PersonsList, ulong BurglarsNr, ulong PersonsNr, ubyte grdAction);

// support
ubyte grdAddToList(ulong bldId, LIST *l);
ubyte grdDraw(struct RastPort *rp, ulong bldId, ulong areaId);

// con- & destructor
ubyte grdInit(FILE **fh, char *mode, ulong bldId, ulong areaId);
void  grdDone(FILE *fh);

#endif
