/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_GAMEPLAY_APP
#define MODULE_GAMEPLAY_APP

#include "theclou.h"

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#ifndef MODULE_GFX
#include "gfx\gfx.h"
#endif

#ifndef MODULE_ANIM
#include "anim\sysanim.h"
#endif

#ifndef MODULE_STORY
#include "story\story.h"
#endif

#ifndef MODULE_DATAAPPL
#include "data\dataappl.h"
#endif

#ifndef MODULE_RANDOM
#include "random\random.h"
#endif

#ifndef MODULE_GAMEPLAY
#include "gameplay\gp.h"
#endif

#ifndef MODULE_SOUND
#include "sound\newsound.h"
#endif

#ifndef MODULE_EVIDENCE
#include "scenes\evidence.h"
#endif

#ifndef MODULE_SCENES
#include "scenes\scenes.h"
#endif

extern void TCGamePause(ubyte activ);

extern ubyte tcPersonIsHere(void);

extern void tcPlaySound(void);
extern void tcPlayStreetSound(void);
extern void tcPersonGreetsMatt(void);

extern ulong StdHandle(ulong choice);

extern void SetFunc(struct Scene *sc,void (*init)(void),void (*done)(void));
extern void PrintStatus(char *text);
extern void InitTaxiLocations(void);
extern void	LinkScenes(void);

extern void tcGetLastName(char *Name, char *dest, ulong maxLength);
extern void tcCutName(char *Name, char Sign, ulong maxLength);

extern ubyte tcLoadTheClou(void);
extern void tcSaveTheClou(int bAutoSave);
extern ulong tcBurglary(ulong buildingID);

extern ubyte tcLoadChangesInScenes(char *fileName);
extern ubyte tcSaveChangesInScenes(char *fileName);

extern void tcRefreshLocationInTitle(ulong locNr);
extern void ShowTime(ulong delay);
extern void RefreshAfterDiskRequester(void);

extern ubyte tcLoadIt(char activ);
extern void  tcRefreshAfterLoad(ubyte loaded);

extern ubyte tcBackToStartupMenu(void);

extern void ShowMenuBackground(void);

#endif
