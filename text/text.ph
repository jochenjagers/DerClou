/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_TXT_PH
#define MODULE_TXT_PH

// includes
#include "theclou.h"
#include "disk\disk.h"
#include "list\list.h"
#include "base\base.h"


// private defines
#define TXT_DISK_ID     0
#define TXT_LIST        "TEXTS.LST"
#define TXT_SUFFIX      ".TXT"

#define TXT_BUFFER_LOAD    StdBuffer0
#define TXT_BUFFER_WORK    StdBuffer1

#define TXT_CHAR_EOL            13
#define TXT_CHAR_EOS            '\0'
#define TXT_CHAR_MARK           '#'
#define TXT_CHAR_REMARK         ';'
#define TXT_CHAR_KEY_SEPERATOR  ','
#define TXT_CHAR_EOF		'^'

#define TXT_XOR_VALUE				0x75


// private structures
struct TextControl
{
   LIST *tc_Texts;
   
   ubyte tc_Language;
ubyte Padding[3];
};

struct Text
{
   NODE  txt_Link;

   void *txt_Handle;
   char *txt_LastMark;

   uword txt_Size;
uword Padding;
};


// private gobals definition
extern char *txtLanguageMark[];
extern struct TextControl *txtBase;
extern char keyBuffer[];


// private prototypes - LINE
static char *txtGetLine(struct Text *txt, ubyte lineNr);

#endif
