/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
/****************************************************************************
  Portions copyright (c) 2005 Vasco Alexandre da Silva Costa
  Portions copyright (c) 2005 Jens Granseuer

  Please read the license terms which should be contained with this
  distribution.
 ****************************************************************************/
#include "SDL.h"

#include "base/base.h"

#include "sound/mxr.h"
#include "sound/fx.h"

#define clamp(x, lower, upper)  ((x) < (lower) ? (lower) : (x) > (upper) ? (upper) : (x))

MXR_Mixer *pAudioMixer = NULL;

static int SfxChannelOn = FALSE;

void InitAudio(void)
{
	MXR_Format fmt;
	fmt.nSamplesPerSec = SND_FREQUENCY;
	fmt.nBitsPerChannel = 16;
	fmt.nNumChannels = 2;
	fmt.nSampleSize = 4;
	pAudioMixer = MXR_CreateMixer(&fmt);

	MXR_SetOutputVolume(pAudioMixer, SND_MAX_VOLUME);

	currMusicVolume = Config.MusicVolume;
}

void RemoveAudio(void)
{
	MXR_DestroyMixer(pAudioMixer);
	pAudioMixer = NULL;
}

void sndInitFX(void)
{
    SDL_LockAudio();

    SfxChannelOn = FALSE;

    SDL_UnlockAudio();
}

void sndDoneFX(void)
{
    SDL_LockAudio();

    SfxChannelOn = FALSE;

    SDL_UnlockAudio();
}

void sndPlayFX(const char *name)
{
    char fileName[256];

	sndDoneFX();

    if (pAudioMixer) {
        dskBuildPathName(SAMPLES_DIRECTORY, name, fileName);
		MXR_SetInput(pAudioMixer, MXR_INPUT_FX, MXR_CreateInputVOC(fileName));
		MXR_SetInputVolume(pAudioMixer, MXR_INPUT_FX, Config.SfxVolume);	// 2018-09-25
		SfxChannelOn = TRUE;
    }
}
