/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "sound\newsound.h"


#define  SND_BUFFER_SIZE		14259

#ifndef __COSP__
// extern sound system, found in playcode.obj
extern far short int NEOSOUNDSYSTEM(short int Command, short int SoundOff, short int SoundSeg);
#else
short NEOSOUNDSYSTEM(short Command, short SoundOff, short SoundSeg)
{
	switch (Command)
	{
		case -1:	// fade out
		case 0:		// play(SoundOff,SoundSeg)
		case 1:		// quit
		case 2:		// init
		case 3:		// volume = SoundOff
		break;
	}
	return(0);
}
#endif

void *sndBuffer = NULL;
char  currSoundName[TXT_KEY_LENGTH];


void sndInit(void)
{
	currSoundName[0] = '\0';

	if (NEOSOUNDSYSTEM(2, 0, 0) != -1)
		sndBuffer = (void *)MemAlloc(SND_BUFFER_SIZE);
}

void sndDone(void)
{
	if (sndBuffer)
	{
		NEOSOUNDSYSTEM(1, 0, 0);
		MemFree(sndBuffer, SND_BUFFER_SIZE);
	}
}

void sndPlaySound(char *name, ulong mode)
{
	char path[TXT_KEY_LENGTH];

	if (sndBuffer)
	{
		if (strcmp(currSoundName, name) != 0)
		{
			strcpy(currSoundName, name);

			dskBuildPathName(SOUND_DIRECTORY, name, path);

			// geschissen:
			// fr die CDROM Version der Profidisk muá man Sounds entweder
			// von Festplatte oder von CDROM laden. Einziges Entscheidungs-
			// kriterium: der Name sndXX.XXX ist auf der Festplatte zu suchen
			#ifdef THECLOU_PROFIDISK
				#ifdef THECLOU_CDROM_VERSION
					if (strncmp(name, "snd", 3) == 0)
						sprintf(path, "%s\\%s", SOUND_DIRECTORY, name);
				#endif
			#endif

			dskLoad(path, sndBuffer, 0);

			NEOSOUNDSYSTEM(0, (short int)(((long)sndBuffer) & 0xffff), (short int)(((long)sndBuffer) >> 16));
		}
	}
}


void sndStopSound(ubyte dummy)
{
	if (sndBuffer)
		NEOSOUNDSYSTEM(1, 0, 0);
}


void sndFadeOut(void)
{
	if (sndBuffer)
		NEOSOUNDSYSTEM(-1, 0, 0);
}


char *sndGetCurrSoundName(void)
{
	return currSoundName;
}


void sndFading(short int targetVol)
{
	if (sndBuffer)
		NEOSOUNDSYSTEM(3, targetVol, 0);
}
