/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_TEXT
#define MODULE_TEXT

// includes

#ifndef __CTYPE_H
#include <ctype.h>
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


// public defines
#define TXT_LANG_ENGLISH   0
#define TXT_LANG_GERMAN    1
#define TXT_LANG_FRENCH    2
#define TXT_LANG_SPANISH   3
#define TXT_LANG_LAST      TXT_LANG_SPANISH

#define TXT_KEY_LENGTH     256


#define txtGetFirstLine(id, key, dest) (txtGetNthString(id, key, 0, dest))


// public prototypes - TEXT
void txtInit(ubyte lang);
void txtDone(void);

void txtLoad(ulong textId);
void txtUnLoad(ulong textId);

void txtPrepare(ulong textId);
void txtUnPrepare(ulong textId);

void txtReset(ulong textId);


// public prototypes - KEY
char *txtGetKey(uword keyNr, char *key);
ulong txtGetKeyAsULONG(uword keyNr, char *key);

LIST *txtGoKey(ulong textId, char *key);
LIST *txtGoKeyAndInsert(ulong textId, char *key, ...);

ubyte txtKeyExists(ulong textId, char *key);
ulong txtCountKey(char *key);


// public prototypes - STRING
char *txtGetString(ulong textId, char *key, char *dest);
char *txtGetNthString(ulong textId, char *key, ulong nth, char *dest);
void txtPutCharacter(LIST *list, uword pos, ubyte c);

// public prototypes - LANGUAGE
void txtSetLanguage(ubyte lang);
ubyte txtGetLanguage(void);

// get a temporary string
char *txtGetTemporary(void);

#endif
