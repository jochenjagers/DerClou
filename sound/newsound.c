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

static char bMuted = 0;

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

			currMusicVolume = 0;
			targetMusicVolume = Config.MusicVolume;
			MXR_SetInputVolume(pAudioMixer, MXR_INPUT_MUSIC, currMusicVolume);	// 2018-09-25
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
	// 2018-09-25: volume can now be changed any time
	if (bMuted) {
		targetMusicVolume = Config.MusicVolume / 4;
	} else {
		targetMusicVolume = Config.MusicVolume;
	}

	if (currMusicVolume < targetMusicVolume) {
		currMusicVolume++;
	} else if (currMusicVolume > targetMusicVolume) {
		currMusicVolume--;
	}

	MXR_SetInputVolume(pAudioMixer, MXR_INPUT_MUSIC, currMusicVolume);
	MXR_SetInputVolume(pAudioMixer, MXR_INPUT_FX, Config.SfxVolume);
	MXR_SetInputVolume(pAudioMixer, MXR_INPUT_VOICE, Config.VoiceVolume);
}

void sndFading(short int targetVol)
{
    if (pAudioMixer) {
		/* 2014-07-17 LucyG : this is called from dialog.c (and intro.c)
		   with targetVol = 16 before and 0 after voice playback */
		if (!targetVol) {
			bMuted = 0;
			targetMusicVolume = Config.MusicVolume;
		} else {
			bMuted = 1;
			targetMusicVolume = Config.MusicVolume / 4;
		}
    }
}

void sndStopSound(ubyte dummy)
{
	MXR_SetInput(pAudioMixer, MXR_INPUT_MUSIC, NULL);
}
