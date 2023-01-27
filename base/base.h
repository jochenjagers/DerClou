/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_BASE
#define MODULE_BASE

#ifndef __STDIO_H
#include <stdio.h>
#endif

#ifndef __STDARG_H
#include <stdarg.h>
#endif

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#ifndef __FCNTL_H
#include <fcntl.h>
#endif

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#ifndef MODULE_INPHDL
#include "inphdl\inphdl.h"
#endif

#ifndef MODULE_GFX
#include "gfx\gfx.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_ANIM
#include "anim\sysanim.h"
#endif

#ifndef MODULE_INTERAC
#include "present\interac.h"
#endif

#ifndef MODULE_RELATION
#include "data\relation.h"
#endif


#ifndef MODULE_DATABASE
#include "data\database.h"
#endif

#ifndef MODULE_GAMEPLAY
#include "gameplay\gp.h"
#endif

#ifndef MODULE_SOUND
#include "sound\newsound.h"
#endif

#ifndef MODULE_FX
#include "sound\fx.h"
#endif

#ifndef MODULE_CDROM
#include "cdrom\cdrom.h"
#endif

/*
 * Verwendungszweck:
 * StdBuffer0: geladenes Bild
 *
 * StdBuffer1: entpacktes Bild
 */

#define STD_BUFFER0_SIZE  ((long)320 * 140)
#define STD_BUFFER1_SIZE  ((long)61 * 1024)      /* weniger geht nicht! (Bild + CMAP)*/

extern char prgname[255];

extern void *StdBuffer0, *StdBuffer1;

extern void tcClearStdBuffer(void *p_Buffer);
extern void tcSetPermanentColors(void);

#endif
