/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PLANING_IO
#define MODULE_PLANING_IO

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


// Extensions
#define PLANING_PLAN_EXTENSION       ".pln"
#define PLANING_PLAN_LIST_EXTENSION  ".pll"

// Disks
#define PLANING_PLAN_DISK         	  0

// Tool sequences in file
#define PLANING_PLAN_TOOL_BEGIN_ID  "TOOB"      // TOOl Begin
#define PLANING_PLAN_TOOL_END_ID    "TOOE"      // TOOl End

// open cmd
#define PLANING_OPEN_READ_PLAN        0
#define PLANING_OPEN_WRITE_PLAN       1
#define PLANING_OPEN_READ_BURGLARY    2

// open error
#define PLANING_OPEN_OK               0
#define PLANING_OPEN_ERR_NO_CHOICE    1
#define PLANING_OPEN_ERR_NO_PLAN      2


ubyte plOpen(ulong objId, ubyte mode, FILE **fh);
void  plLoad(ulong objId);
void  plSave(ulong objId);
void  plSaveChanged (ulong objId);

LIST *plLoadTools(FILE *fh);
void plSaveTools(FILE *fh);

#endif
