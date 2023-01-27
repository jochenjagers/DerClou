/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa

  Please read the license terms which should be contained with this
  distribution.
 ****************************************************************************/
#include "SDL.h"

#include "disk/disk.h"

#include "sound/fx.h"
#include "sound/newsound.h"

#include "sound/mxr.h"

char currSoundName[256];

int currMusicVolume = 0;
int targetMusicVolume = 0;

void sndInit(void)
{
    currSoundName[0] = '\0';

	currMusicVolume = Config.MusicVolume;
	targetMusicVolume = Config.MusicVolume;
}

void sndDone(void)
{
}

void sndPlaySound(char *name, ulong mode)
{
	char path[256];

	if (pAudioMixer) {
		if (stricmp(currSoundName, name)) {
			strcpy(currSoundName, name);

			dskBuildPathName(SOUND_DIRECTORY, name, path);

			MXR_SetInput(pAudioMixer, MXR_INPUT_MUSIC, MXR_CreateInputHSC(path));

			targetMusicVolume = Config.MusicVolume;
		}
	}
}

char *sndGetCurrSoundName(void)
{
    return currSoundName;
}

// 2014-07-17 LucyG : called from inpDoPseudoMultiTasking
void sndDoFading(void)
{
	if (currMusicVolume < targetMusicVolume) {
		currMusicVolume++;
	} else if (currMusicVolume > targetMusicVolume) {
		currMusicVolume--;
	}
}

void sndFading(short int targetVol)
{
    if (pAudioMixer) {
		/* 2014-07-17 LucyG : this is called from dialog.c (and intro.c)
		   with targetVol = 16 before and 0 after voice playback */
		if (!targetVol) {
			targetMusicVolume = Config.MusicVolume;
		} else {
			targetMusicVolume = Config.MusicVolume / 4;
		}
    }
}

void sndStopSound(ubyte dummy)
{
	MXR_SetInput(pAudioMixer, MXR_INPUT_MUSIC, NULL);
}
