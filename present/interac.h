/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_INTERAC
#define MODULE_INTERAC

#include "theclou.h"

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_GFX
#include "gfx\gfx.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_DATABASE
#include "data\database.h"
#endif

#define X_OFFSET    112
#define NRBLINES    5

#define BG_ACTIVE_COLOR   252
#define VG_ACTIVE_COLOR   254

#define BG_TXT_COLOR      252
#define VG_TXT_COLOR      254

#define BG_BAR_COLOR      230
#define VG_BAR_COLOR      230

#define TXT_MENU_TIMEOUT        (-2)

extern ubyte ChoiceOk (ubyte choice, ubyte xit, LIST *l);

extern ubyte Bubble(LIST *bubble, ubyte activ, void (*func)(ubyte), ulong waitTime);
extern ubyte Menu (LIST *menu, ulong possibility, ubyte activ, void (*func)(ubyte), ulong waitTime);

extern void RefreshMenu(void);

extern void SetBubbleType (uword type);
extern void SetPictID (uword PictID);
extern void SetMenuTimeOutFunc(void *func);

extern ubyte GetExtBubbleActionInfo(void);

#endif
