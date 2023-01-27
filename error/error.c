/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "error\error.h"
#include "text\text.h"

#define ERR_EXIT_ERROR      20L
#define ERR_EXIT_SHUTDOWN   30L

struct ErrorHandler
{
	void	(*fp_CloseAll)(void);

	ubyte	puch_Filename[DSK_PATHNAMELENGTH];
	ubyte	uch_OutputToFile;
};

/* implementation */

struct ErrorHandler ErrorHandler;

void Log(const char *s, ...)
{
	static char log_buffer[1024];
	FILE *file;
	if (s)
	{
		if (file = fopen(LOG_FILENAME, "a"))
		{
			va_list ap;
			va_start(ap, s);
			vsprintf(log_buffer, s, ap);
			#ifndef __LCC__
			va_end(ap);
			#endif
			fprintf(file, "%s\n", log_buffer);
			fclose(file);
		}
	}
}

long pcErrOpen(long l_Mode, void (*fp_CloseAll)(), ubyte *puch_ErrorFilename)
{
	if (fp_CloseAll)
	{
		ErrorHandler.fp_CloseAll = fp_CloseAll;
		return(1);
	}
	return(0);
}

void pcErrClose()
{
}

void NewErrorMsg(long l_ErrorType, const char *filename, const char *funcname, ulong ul_ErrorId)
{
	ubyte lang = txtGetLanguage();	/* 2014-06-27 templer */

	Log("Error %ld in %s|%s", ul_ErrorId, filename, funcname);

	if (ErrorHandler.fp_CloseAll)
		ErrorHandler.fp_CloseAll();

	switch (l_ErrorType)
	{
		case Internal_Error:
			if (lang == TXT_LANG_GERMAN) {
				Log("Interner Fehler!");
			} else {
				Log("Internal Error!");
			}
		break;

		case No_Mem:
			if (lang == TXT_LANG_GERMAN) {
				Log("Sie haben nicht genug Speicher!");
			} else {
				Log("You don't have enough memory!");
			}
		break;

		case Disk_Defect:
			if (lang == TXT_LANG_GERMAN) {
				Log("Kann Datei nicht oeffnen!");
			} else {
				Log("Can't open file!");
			}
		break;
	}

	exit(-1);
}
