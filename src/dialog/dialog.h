/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_DIALOG
#define MODULE_DIALOG

#include "theclou.h"

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_INTERAC
#include "present/interac.h"
#endif

#ifndef MODULE_TCDATA
#include "data/tcdata.h"
#endif

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#ifndef MODULE_DATABASE
#include "data/database.h"
#endif

#ifndef MODULE_GAMEPLAY
#include "gameplay/gp.h"
#endif

#ifndef MODULE_SOUND
#include "sound/newsound.h"
#endif

#define DLG_TALKMODE_BUSINESS 1
#define DLG_TALKMODE_STANDARD 2

extern void DynamicTalk(uint32_t Person1ID, uint32_t Person2ID, ubyte TalkMode);
extern ubyte Say(uint32_t TextID, ubyte activ, uword person, char *text);
extern uint32_t Talk(void);

#endif
