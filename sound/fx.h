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
#ifndef TC_FX_H
#define TC_FX_H

#include "theclou.h"
#include "sound/mxr.h"

#define	SND_FREQUENCY				44100
#define SND_MAX_VOLUME              255

//#define SND_BUFFER_SIZE				247226	// 65536
#define SND_MUSIC_BUFFER_SIZE		65536
#define SND_SAMPLES                 1024

extern MXR_Mixer *pAudioMixer;

extern void sndInitFX(void);
extern void sndDoneFX(void);
extern void sndPlayFX(const char *Name);

extern void InitAudio(void);
extern void RemoveAudio(void);

#endif

