/*
** MXR - A Simple Audio Mixer
** Copyright (c) 2017 by Oliver Gantert.
**
** Permission is given by the author to use this version of MXR freely,
** provided that the message "Copyright (c) 2017 by Oliver Gantert" in this
** and associated files is not changed or removed.
**
** See "mxr.h" for details.
*/
#include "SDL.h"
#include "mxr.h"
#include "sound/fmopl.h"

/******************************************************************************/

struct MXR_Mixer {
	MXR_Format		fmt;
	unsigned long	nVolume;

	MXR_Input *		pInput[MXR_MAX_INPUTS];
	unsigned char	buffer[MXR_BUFFER_SAMPLES * 2 * sizeof(short)];	/* (hopefully) the largest required buffer */
};

#define ClampValue(v,a,z)	((v) < (a) ? (a) : ((v) > (z) ? (z) : (v)))

/******************************************************************************/

static void MXR__ConvertU8ToS16(unsigned char *pBuffer, unsigned long nValues)
{
	long i;
	short *pDest;
	long s;

	pDest = (short *)pBuffer;
	for (i = (long)nValues - 1; i >= 0; --i) {
		s = pBuffer[i];
		s <<= 8;
		s -= 0x008000;
		pDest[i] = (short)s;
	}
}

/******************************************************************************/

static void MXR__ConvertS16ToU8(short *pBuffer, unsigned long nValues)
{
	unsigned long i;
	unsigned char *pDest;
	long s;

	pDest = (unsigned char *)pBuffer;
	for (i = 0; i < nValues; i++) {
		s = pBuffer[i];
		s += 0x008000;
		s >>= 8;
		pDest[i] = (unsigned char)s;
	}
}

/******************************************************************************/

static void MXR__ResampleAndMixS16MonoToMono(MXR_Mixer *pMixer, short *pStream, unsigned long nStreamSamples, MXR_Input *pInput, unsigned long nInputSamples)
{
	long s, s2;
	long rem;
	unsigned long i, ixSamples, j, j2;
	short *pBuffer;

	pBuffer = (short *)pMixer->buffer;

	if (nStreamSamples == nInputSamples) {
		for (i = 0; i < nStreamSamples; i++) {
			s = pBuffer[i];

			s *= pInput->nVolume;
			s /= 255;
			s += pStream[i];
			pStream[i] = (short)ClampValue(s, -32768, 32767);
		}
		return;
	}

	for (i = 0; i < nStreamSamples; i++) {
		ixSamples = i * nInputSamples;
		rem = ixSamples % nStreamSamples;
		j = ixSamples / nStreamSamples;
		j2 = (j >= (nInputSamples - 1)) ? (nInputSamples - 1) : (j + 1);

		s = pBuffer[j];
		s2 = pBuffer[j2];
		s = s + (((s2 - s) * rem) / (long)nStreamSamples);

		s *= pInput->nVolume;
		s /= 255;
		s += pStream[i];
		pStream[i] = (short)ClampValue(s, -32768, 32767);
	}
}

/******************************************************************************/

static void MXR__ResampleAndMixS16MonoToStereo(MXR_Mixer *pMixer, short *pStream, unsigned long nStreamSamples, MXR_Input *pInput, unsigned long nInputSamples)
{
	long s, s2;
	long rem;
	unsigned long i, i2, ixSamples, j, j2;
	short *pBuffer;

	pBuffer = (short *)pMixer->buffer;

	if (nStreamSamples == nInputSamples) {
		for (i = 0; i < nStreamSamples; i++) {
			s = pBuffer[i];

			s *= pInput->nVolume;
			s /= 255;
			s2 = s;
			i2 = i << 1;
			s += pStream[i2];
			s2 += pStream[i2 + 1];
			pStream[i2] = (short)ClampValue(s, -32768, 32767);
			pStream[i2 + 1] = (short)ClampValue(s2, -32768, 32767);
		}
		return;
	}

	for (i = 0; i < nStreamSamples; i++) {
		ixSamples = i * nInputSamples;
		rem = ixSamples % nStreamSamples;
		j = ixSamples / nStreamSamples;
		j2 = (j >= (nInputSamples - 1)) ? (nInputSamples - 1) : (j + 1);

		s = pBuffer[j];
		s2 = pBuffer[j2];
		s = s + (((s2 - s) * rem) / (long)nStreamSamples);

		s *= pInput->nVolume;
		s /= 255;
		s2 = s;
		i2 = i << 1;
		s += pStream[i2];
		s2 += pStream[i2 + 1];
		pStream[i2] = (short)ClampValue(s, -32768, 32767);
		pStream[i2 + 1] = (short)ClampValue(s2, -32768, 32767);
	}
}

/******************************************************************************/

static void MXR__ResampleAndMixS16StereoToMono(MXR_Mixer *pMixer, short *pStream, unsigned long nStreamSamples, MXR_Input *pInput, unsigned long nInputSamples)
{
	long s, s2, l, r;
	long rem;
	unsigned long i, i2, ixSamples, j, j2, jx, j2x;
	short *pBuffer;

	pBuffer = (short *)pMixer->buffer;

	if (nStreamSamples == nInputSamples) {
		for (i = 0; i < nStreamSamples; i++) {
			i2 = i << 1;

			s = pBuffer[i2];
			s += pBuffer[i2 + 1];
			s /= 2;

			s *= pInput->nVolume;
			s /= 255;
			s += pStream[i];
			pStream[i] = (short)ClampValue(s, -32768, 32767);
		}
		return;
	}

	for (i = 0; i < nStreamSamples; i++) {
		ixSamples = i * nInputSamples;
		rem = ixSamples % nStreamSamples;
		j = ixSamples / nStreamSamples;
		j2 = (j >= (nInputSamples - 1)) ? (nInputSamples - 1) : (j + 1);

		jx = j << 1;
		j2x = j2 << 1;
		s = pBuffer[jx];
		s2 = pBuffer[j2x];
		l = s + (((s2 - s) * rem) / (long)nStreamSamples);
		s = pBuffer[jx + 1];
		s2 = pBuffer[j2x + 1];
		r = s + (((s2 - s) * rem) / (long)nStreamSamples);

		s = (l + r) / 2;

		s *= pInput->nVolume;
		s /= 255;
		s += pStream[i];
		pStream[i] = (short)ClampValue(s, -32768, 32767);
	}
}

/******************************************************************************/

static void MXR__ResampleAndMixS16StereoToStereo(MXR_Mixer *pMixer, short *pStream, unsigned long nStreamSamples, MXR_Input *pInput, unsigned long nInputSamples)
{
	long s, s2;
	long rem;
	unsigned long i, i2, ixSamples, j, j2, jx, j2x;
	short *pBuffer;

	pBuffer = (short *)pMixer->buffer;

	if (nStreamSamples == nInputSamples) {
		for (i = 0; i < nStreamSamples; i++) {
			i2 = i << 1;

			s = pBuffer[i2];
			s *= pInput->nVolume;
			s /= 255;
			s += pStream[i2];

			pStream[i2] = (short)ClampValue(s, -32768, 32767);

			s = pBuffer[i2+1];
			s *= pInput->nVolume;
			s /= 255;
			s += pStream[i2+1];

			pStream[i2+1] = (short)ClampValue(s, -32768, 32767);
		}
		return;
	}

	for (i = 0; i < nStreamSamples; i++) {
		ixSamples = i * nInputSamples;
		rem = ixSamples % nStreamSamples;
		j = ixSamples / nStreamSamples;
		j2 = (j >= (nInputSamples - 1)) ? (nInputSamples - 1) : (j + 1);

		i2 = i << 1;

		jx = j << 1;
		j2x = j2 << 1;
		s = pBuffer[jx];
		s2 = pBuffer[j2x];
		s = s + (((s2 - s) * rem) / (long)nStreamSamples);
		s *= pInput->nVolume;
		s /= 255;
		s += pStream[i2];
		pStream[i2] = (short)ClampValue(s, -32768, 32767);

		s = pBuffer[jx + 1];
		s2 = pBuffer[j2x + 1];
		s = s + (((s2 - s) * rem) / (long)nStreamSamples);
		s *= pInput->nVolume;
		s /= 255;
		s += pStream[i2+1];
		pStream[i2+1] = (short)ClampValue(s, -32768, 32767);
	}
}

/******************************************************************************/

static void MXR__ResampleAndMixU8MonoToMono(MXR_Mixer *pMixer, unsigned char *pStream, unsigned long nStreamSamples, MXR_Input *pInput, unsigned long nInputSamples)
{
}

/******************************************************************************/

static void MXR__ResampleAndMixU8MonoToStereo(MXR_Mixer *pMixer, unsigned char *pStream, unsigned long nStreamSamples, MXR_Input *pInput, unsigned long nInputSamples)
{
}

/******************************************************************************/

static void MXR__ResampleAndMixU8StereoToMono(MXR_Mixer *pMixer, unsigned char *pStream, unsigned long nStreamSamples, MXR_Input *pInput, unsigned long nInputSamples)
{
}

/******************************************************************************/

static void MXR__ResampleAndMixU8StereoToStereo(MXR_Mixer *pMixer, unsigned char *pStream, unsigned long nStreamSamples, MXR_Input *pInput, unsigned long nInputSamples)
{
}

/******************************************************************************/
typedef void (*SDL_AudioCallbackFunc)(void *, unsigned char *, int);

static void MXR_ProcessMixer(MXR_Mixer *pMixer, unsigned char *pStream, int nStreamSize)
{
	int i;
	MXR_Input *pInput;
	unsigned long nStreamSamples, nInputSamples;

	nStreamSamples = nStreamSize / pMixer->fmt.nSampleSize;	/* should(!) be MXR_BUFFER_SAMPLES */

	/* the inputs are additive, so we should silence the stream first */
	if (pMixer->fmt.nBitsPerChannel == 8) {
		memset(pStream, 0x80, nStreamSize);
	} else {
		memset(pStream, 0, nStreamSize);
	}

	for (i = 0; i < MXR_MAX_INPUTS; i++) {
		pInput = pMixer->pInput[i];
		if (pInput && pInput->pProcess) {
			nInputSamples = nStreamSamples * pInput->fmt.nSamplesPerSec / pMixer->fmt.nSamplesPerSec;

			if (pInput->fmt.nBitsPerChannel == 8) {
				memset(pMixer->buffer, 0x80, sizeof(pMixer->buffer));
			} else {
				memset(pMixer->buffer, 0, sizeof(pMixer->buffer));
			}

			if (pInput->pProcess(pInput, pMixer->buffer, nInputSamples) <= 0) {
				if (pInput->pDestroy) {
					pInput->pDestroy(pInput);
				}
				pMixer->pInput[i] = NULL;
			} else {
				/* convert pMixer->buffer to output format */
				if (pInput->fmt.nBitsPerChannel < pMixer->fmt.nBitsPerChannel) {
					MXR__ConvertU8ToS16(pMixer->buffer, nInputSamples * pInput->fmt.nNumChannels);
				} else if (pInput->fmt.nBitsPerChannel > pMixer->fmt.nBitsPerChannel) {
					MXR__ConvertS16ToU8(pMixer->buffer, nInputSamples * pInput->fmt.nNumChannels);
				}

				if (pMixer->fmt.nBitsPerChannel == 8) {
					if (pInput->fmt.nNumChannels == 2) {
						if (pMixer->fmt.nNumChannels == 2) {
							MXR__ResampleAndMixU8StereoToStereo(pMixer, pStream, nStreamSamples, pInput, nInputSamples);
						} else {
							MXR__ResampleAndMixU8StereoToMono(pMixer, pStream, nStreamSamples, pInput, nInputSamples);
						}
					} else {
						if (pMixer->fmt.nNumChannels == 2) {
							MXR__ResampleAndMixU8MonoToStereo(pMixer, pStream, nStreamSamples, pInput, nInputSamples);
						} else {
							MXR__ResampleAndMixU8MonoToMono(pMixer, pStream, nStreamSamples, pInput, nInputSamples);
						}
					}
				} else {
					if (pInput->fmt.nNumChannels == 2) {
						if (pMixer->fmt.nNumChannels == 2) {
							MXR__ResampleAndMixS16StereoToStereo(pMixer, pStream, nStreamSamples, pInput, nInputSamples);
						} else {
							MXR__ResampleAndMixS16StereoToMono(pMixer, pStream, nStreamSamples, pInput, nInputSamples);
						}
					} else {
						if (pMixer->fmt.nNumChannels == 2) {
							MXR__ResampleAndMixS16MonoToStereo(pMixer, pStream, nStreamSamples, pInput, nInputSamples);
						} else {
							MXR__ResampleAndMixS16MonoToMono(pMixer, pStream, nStreamSamples, pInput, nInputSamples);
						}
					}
				}

			}
		}
	}
}

/******************************************************************************/

MXR_Mixer *MXR_CreateMixer(const MXR_Format *pFormat)
{
	MXR_Mixer *pMixer;
	SDL_AudioSpec spec;
	int i;

	pMixer = (MXR_Mixer *)MXR_MemAlloc(sizeof(MXR_Mixer));
	if (!pMixer) {
		return(NULL);
	}

	pMixer->fmt.nSamplesPerSec	= pFormat->nSamplesPerSec;
	pMixer->fmt.nBitsPerChannel	= pFormat->nBitsPerChannel;
	pMixer->fmt.nNumChannels	= pFormat->nNumChannels;
	pMixer->fmt.nSampleSize		= (pFormat->nNumChannels * pFormat->nBitsPerChannel) >> 3;
	pMixer->nVolume				= 128;
	for (i = 0; i < MXR_MAX_INPUTS; i++) {
		pMixer->pInput[i] = NULL;
	}

	spec.freq		= pMixer->fmt.nSamplesPerSec;
	if (pMixer->fmt.nBitsPerChannel == 8) {
		spec.format	= AUDIO_U8;
	} else {
		pMixer->fmt.nBitsPerChannel = 16;
		spec.format	= AUDIO_S16;
	}
	spec.channels	= pMixer->fmt.nNumChannels;
	spec.samples	= MXR_BUFFER_SAMPLES;
	spec.callback	= (SDL_AudioCallbackFunc)MXR_ProcessMixer;
	spec.userdata	= pMixer;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
		MXR_MemFree(pMixer, sizeof(MXR_Mixer));
		return(NULL);
	}

	if (SDL_OpenAudio(&spec, NULL)) {
		Log("SDL_OpenAudio: %s", SDL_GetError());
		MXR_MemFree(pMixer, sizeof(MXR_Mixer));
		return(NULL);
	}

	/* YM3812 emulation for the HSC player */
	YM3812Init(OPL_NUM_CHIPS, OPL_INTERNAL_FREQ, OPL_FREQUENCY);

	SDL_PauseAudio(0);

	return(pMixer);
}

/******************************************************************************/

void MXR_DestroyMixer(MXR_Mixer *pMixer)
{
	int i;

    SDL_CloseAudio();

	/* YM3812 emulation for the HSC player */
	YM3812Shutdown();

    if (SDL_WasInit(SDL_INIT_AUDIO) != 0) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }

	if (pMixer) {
		for (i = 0; i < MXR_MAX_INPUTS; i++) {
			if (pMixer->pInput[i] && pMixer->pInput[i]->pDestroy) {
				pMixer->pInput[i]->pDestroy(pMixer->pInput[i]);
			}
			pMixer->pInput[i] = NULL;
		}
		MXR_MemFree(pMixer, sizeof(MXR_Mixer));
	}
}

/******************************************************************************/

void MXR_SetInput(MXR_Mixer *pMixer, long nInput, MXR_Input *pInput)
{
	MXR_Input *pOldInput;

	SDL_LockAudio();

	pOldInput = pMixer->pInput[nInput];
	pMixer->pInput[nInput] = pInput;

	SDL_UnlockAudio();

	if (pOldInput && pOldInput->pDestroy) {
		pOldInput->pDestroy(pOldInput);
	}
}

/******************************************************************************/

unsigned char MXR_SetOutputVolume(MXR_Mixer *pMixer, unsigned char nVolume)
{
	unsigned char nOldVolume;

	SDL_LockAudio();

	nOldVolume = pMixer->nVolume;
	pMixer->nVolume = nVolume;

	SDL_UnlockAudio();

	return(nOldVolume);
}

/******************************************************************************/

unsigned char MXR_SetInputVolume(MXR_Mixer *pMixer, long nInput, unsigned char nVolume)
{
	unsigned char nOldVolume = 0;

	if (pMixer->pInput[nInput]) {	// 2018-09-25
		SDL_LockAudio();

		nOldVolume = pMixer->pInput[nInput]->nVolume;
		pMixer->pInput[nInput]->nVolume = nVolume;

		SDL_UnlockAudio();
	}

	return(nOldVolume);
}

/******************************************************************************/
