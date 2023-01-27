/*
** MXR - A Simple Audio Mixer
** Copyright (c) 2017 by Oliver Gantert.
**
** Permission is given by the author to use this version of MXR freely,
** provided that the message "Copyright (c) 2017 by Oliver Gantert" in this
** and associated files is not changed or removed.
**
** MXR features on-the-fly resampling and mixing of 8bit and 16bit streams.
** Inputs may provide sample data in whatever (supported) format they prefer,
** MXR handles the conversion to the desired output format.
**
** Resampling may introduce artifacts and audible aliasing effects (defects)
** depending on sample rate conversion ratio, output buffer size and the
** occasional rounding error.
**
** MXR was purposefully designed with integer operations, no floating points
** were harmed in the resampling nor mixing code. However, inputs may use
** floating point operations however they see fit.
**
**     Supported input/output formats :
**
**         - unsigned 8bit mono
**         - unsigned 8bit stereo interleaved
**         - signed 16bit mono
**         - signed 16bit stereo interleaved
**
**    _________  _______
**   / ___/ __ \/ __/ _ \      Der Clou!
**  / /__/ /_/ /\ \/ ___/ Open Source Project
**  \___/\____/___/_/ https://cosp.sourceforge.io
**   Based on the original by neo Software GmbH
**
** This version was adapted to the Clou Open Source Project <cosp.sourceforge.io>
** (which served as the initial test bed for this piece of software)
** and uses SDL for audio output.
**
*/
#ifndef mxr_h
#define mxr_h

/******************************************************************************/

#include "theclou.h"
#include "memory/memory.h"

#define MXR_MemAlloc(size)		MemAlloc(size)
#define MXR_MemFree(mem,size)	MemFree((mem),(size))

#define OPL_INTERNAL_FREQ	3579545	/* 3.6 MHz... */
#define OPL_FREQUENCY		44100
#define	OPL_NUM_CHIPS		1
#define OPL_CHIP0			0

/******************************************************************************/

/*
** The audio buffer size (in samples)
*/
#define MXR_BUFFER_SAMPLES	1024

/*
** Maximum number of inputs
*/
#define MXR_MAX_INPUTS	3		// 0 = music, 1 = soundfx, 2 = voice

#define MXR_INPUT_MUSIC	0
#define MXR_INPUT_FX	1
#define MXR_INPUT_VOICE	2

/******************************************************************************/

typedef struct MXR_Mixer	MXR_Mixer;
typedef struct MXR_Input	MXR_Input;

typedef struct {
	unsigned long	nSamplesPerSec;		/* 22050, 44100 etc. */
	unsigned long	nBitsPerChannel;	/* 8 or 16 */
	unsigned long	nNumChannels;		/* 1 or 2 */
	unsigned long	nSampleSize;		/* (nNumChannels * nBitsPerChannel / 8) */
} MXR_Format;

typedef unsigned long (*MXR_ProcessInputFunc)(MXR_Input *, void *, unsigned long);
typedef void (*MXR_DestroyInputFunc)(MXR_Input *);

struct MXR_Input {
	MXR_Format				fmt;
	unsigned long			nVolume;

	MXR_ProcessInputFunc	pProcess;
	MXR_DestroyInputFunc	pDestroy;
};

#define MXR_MillisecToSamples(ms,hz)	((ms)*(hz)/1000)

/******************************************************************************/

extern MXR_Mixer *MXR_CreateMixer(const MXR_Format *pFormat);
extern void MXR_DestroyMixer(MXR_Mixer *pMixer);

extern void MXR_SetInput(MXR_Mixer *pMixer, long nInput, MXR_Input *pInput);

extern unsigned char MXR_SetOutputVolume(MXR_Mixer *pMixer, unsigned char nVolume);
extern unsigned char MXR_SetInputVolume(MXR_Mixer *pMixer, long nInput, unsigned char nVolume);

extern MXR_Input *MXR_CreateInputVOC(const char *pszFileName);
extern MXR_Input *MXR_CreateInputWAV(const char *pszFileName);
extern MXR_Input *MXR_CreateInputHSC(const char *pszFileName);

/******************************************************************************/

#endif /* mxr_h */
