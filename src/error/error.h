/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_ERROR
#define MODULE_ERROR

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "theclou.h"

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#define LOG_FILENAME "clou.log"
void Log(const char *s, ...);

#define ERROR_MODULE_BASE 3
#define ERROR_MODULE_TXT 4
#define ERROR_MODULE_DISK 5
#define ERROR_MODULE_MEMORY 6
#define ERROR_MODULE_DATABASE 7
#define ERROR_MODULE_GAMEPLAY 8
#define ERROR_MODULE_LOADSAVE 9
#define ERROR_MODULE_LANDSCAP 10
#define ERROR_MODULE_LIVING 11
#define ERROR_MODULE_PLANING 12

/* Parameter : mode */

#define ERR_NO_OUTPUT 1L
#define ERR_OUTPUT_TO_DISK 2L

/* Parameter : Fehlerart */

#define ERR_WARNING 2L   /* Programm läuft weiter */
#define ERR_ERROR 4L     /* Programm bricht mit Resourcenfreigabe ab */
#define ERR_SHUT_DOWN 8L /* Programm bricht ohne Resourcenfreigabe ab */
                         /* zur Resuurcenfreigabe wird CloseAll angesprungen */

#define ERR_REQUESTER                                    \
    16L /* zeige Requester - Fehlertext muß weniger als \
                                                                            78 Zeichen lang sein */

/* some error types */

enum
{
    No_Error = 0,
    Internal_Error,
    No_Mem,
    Disk_Defect,
    Insert_Disk,
    Lib_Error,
    Last_Error
};

extern int32_t pcErrOpen(int32_t l_Mode, void (*fp_CloseAll)(), ubyte *puch_ErrorFilename);
_Noreturn extern void NewErrorMsg(int32_t l_ErrorType, const char *filename, const char *funcname, uint32_t ul_ErrorId);
extern void pcErrClose(void);

#endif
