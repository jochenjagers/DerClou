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
#include "error/error.h"
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

// public defines
#define TXT_LANG_ENGLISH 0
#define TXT_LANG_GERMAN 1
#define TXT_LANG_FRENCH 2
#define TXT_LANG_SPANISH 3
#define TXT_LANG_LAST TXT_LANG_SPANISH

#define TXT_KEY_LENGTH 256

#define txtGetFirstLine(id, key, dest) (txtGetNthString(id, key, 0, dest))

// public prototypes - TEXT
void txtInit(ubyte lang);
void txtDone(void);

void txtLoad(uint32_t textId);
void txtUnLoad(uint32_t textId);

void txtPrepare(uint32_t textId);
void txtUnPrepare(uint32_t textId);

void txtReset(uint32_t textId);

// public prototypes - KEY
char *txtGetKey(uword keyNr, char *key);
uint32_t txtGetKeyAsULONG(uword keyNr, char *key);

LIST *txtGoKey(uint32_t textId, char *key);
LIST *txtGoKeyAndInsert(uint32_t textId, char *key, ...);

ubyte txtKeyExists(uint32_t textId, char *key);
uint32_t txtCountKey(char *key);

// public prototypes - STRING
char *txtGetString(uint32_t textId, char *key, char *dest);
char *txtGetNthString(uint32_t textId, char *key, uint32_t nth, char *dest);
void txtPutCharacter(LIST *list, uword pos, ubyte c);

// public prototypes - LANGUAGE
void txtSetLanguage(ubyte lang);
ubyte txtGetLanguage(void);

// get a temporary string
char *txtGetTemporary(void);

#endif
