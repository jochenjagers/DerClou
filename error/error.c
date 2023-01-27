/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "error\error.h"

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
	Log("Error %ld in %s|%s", ul_ErrorId, filename, funcname);

	if (ErrorHandler.fp_CloseAll)
		ErrorHandler.fp_CloseAll();

	switch (l_ErrorType)
	{
		case Internal_Error:
			#ifdef THECLOU_VERSION_ENGLISH
			Log("Internal Error!");
			#else
			Log("Interner Fehler!");
			#endif
		break;

		case No_Mem:
			#ifdef THECLOU_VERSION_ENGLISH
			Log("You don't have enough memory!");
			#else
			Log("Sie haben nicht genug Speicher!");
			#endif
		break;

		case Disk_Defect:
			#ifdef THECLOU_VERSION_ENGLISH
			Log("Can't open file!");
			#else
			Log("Kann Datei nicht oeffnen!");
			#endif
		break;
	}

	exit(-1);
}
