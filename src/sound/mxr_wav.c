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
#include "mxr.h"

/******************************************************************************/

typedef struct
{
    uint32_t dwChunkID;
    uint32_t dwChunkSize;
} WAVChunk;

typedef struct
{
    unsigned short wFormatTag;
    unsigned short wChannels;
    uint32_t dwSamplesPerSec;
    uint32_t dwAvgBytesPerSec;
    unsigned short wBlockAlign;
    unsigned short wBitsPerSample;
} WAVFmtChunk;

typedef struct
{
    FILE *pFile;
    WAVFmtChunk fmt;
    uint32_t dwNumSamples;
    uint32_t dwDataOffset;
    uint32_t dwDataSize;
    uint32_t dwSampleSize;
    uint32_t dwPosition;
} WAVFile;

#define makefourcc(ch0, ch1, ch2, ch3)                                                \
    ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) | \
     ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24))

/******************************************************************************/

static int32_t WAVOpen(WAVFile *wav, const char *pszFileName)
{
    WAVChunk chk;

    wav->pFile = fopen(pszFileName, "rb");
    if (!wav->pFile)
    {
        return (0);
    }

    if (fread(&chk, 1, sizeof(WAVChunk), wav->pFile) != sizeof(WAVChunk)) goto Lgetout;
    if (chk.dwChunkID != makefourcc('R', 'I', 'F', 'F')) goto Lgetout;
    if (fread(&chk.dwChunkID, 1, sizeof(int32_t), wav->pFile) != sizeof(int32_t)) goto Lgetout;
    if (chk.dwChunkID != makefourcc('W', 'A', 'V', 'E')) goto Lgetout;
    if (fread(&chk, 1, sizeof(WAVChunk), wav->pFile) != sizeof(WAVChunk)) goto Lgetout;
    if (chk.dwChunkID != makefourcc('f', 'm', 't', ' ')) goto Lgetout;
    if (chk.dwChunkSize < sizeof(WAVFmtChunk)) goto Lgetout;
    if (fread(&wav->fmt, 1, sizeof(WAVFmtChunk), wav->pFile) != sizeof(WAVFmtChunk)) goto Lgetout;
    if (chk.dwChunkSize > sizeof(WAVFmtChunk))
    {
        fseek(wav->pFile, chk.dwChunkSize - sizeof(WAVFmtChunk), SEEK_CUR);
    }
    if (fread(&chk, 1, sizeof(WAVChunk), wav->pFile) != sizeof(WAVChunk)) goto Lgetout;
    if (chk.dwChunkID != makefourcc('d', 'a', 't', 'a')) goto Lgetout;
    wav->dwSampleSize = (wav->fmt.wChannels * wav->fmt.wBitsPerSample) >> 3;
    if (!wav->dwSampleSize) goto Lgetout;
    wav->dwDataSize = chk.dwChunkSize;
    if (!wav->dwDataSize) goto Lgetout;
    wav->dwDataOffset = ftell(wav->pFile);
    wav->dwNumSamples = wav->dwDataSize / wav->dwSampleSize;
    if (!wav->dwNumSamples) goto Lgetout;
    wav->dwPosition = 0;

    return (1);

Lgetout:
    fclose(wav->pFile);
    wav->pFile = NULL;
    return (0);
}

/******************************************************************************/

typedef struct
{
    MXR_Input mxrInput;
    WAVFile wavFile;
} MXR_InputWAV;

static uint32_t MXR_ProcessInputWAV(MXR_InputWAV *pInput, void *pStream, uint32_t nNumSamples)
{
    int32_t nSamplesRemain = 0;
    int32_t nSamplesRead = 0;
    nSamplesRemain = (pInput->wavFile.dwDataSize - pInput->wavFile.dwPosition) / pInput->mxrInput.fmt.nSampleSize;
    if (nNumSamples > nSamplesRemain)
    {
        nNumSamples = nSamplesRemain;
    }
    if (nNumSamples > 0)
    {
        nSamplesRead = fread(pStream, pInput->mxrInput.fmt.nSampleSize, nNumSamples, pInput->wavFile.pFile);
        if (nSamplesRead > 0)
        {
            pInput->wavFile.dwPosition += (nSamplesRead * pInput->mxrInput.fmt.nSampleSize);
            return (nSamplesRead);
        }
    }
    return (0);  // MXR will destroy this input
}

static void MXR_DestroyInputWAV(MXR_InputWAV *pInput)
{
    if (pInput->wavFile.pFile)
    {
        fclose(pInput->wavFile.pFile);
    }
    MXR_MemFree(pInput, sizeof(MXR_InputWAV));
}

MXR_Input *MXR_CreateInputWAV(const char *pszFileName)
{
    MXR_InputWAV *pInput = NULL;
    pInput = (MXR_InputWAV *)MXR_MemAlloc(sizeof(MXR_InputWAV));
    if (!pInput)
    {
        return (NULL);
    }
    pInput->mxrInput.pProcess = (MXR_ProcessInputFunc)MXR_ProcessInputWAV;
    pInput->mxrInput.pDestroy = (MXR_DestroyInputFunc)MXR_DestroyInputWAV;
    pInput->mxrInput.nVolume = 128;

    if (!WAVOpen(&pInput->wavFile, pszFileName))
    {
        MXR_MemFree(pInput, sizeof(MXR_InputWAV));
        return (NULL);
    }

    pInput->mxrInput.fmt.nSamplesPerSec = pInput->wavFile.fmt.dwSamplesPerSec;
    pInput->mxrInput.fmt.nBitsPerChannel = pInput->wavFile.fmt.wBitsPerSample;
    pInput->mxrInput.fmt.nNumChannels = pInput->wavFile.fmt.wChannels;
    pInput->mxrInput.fmt.nSampleSize = (pInput->mxrInput.fmt.nNumChannels * pInput->mxrInput.fmt.nBitsPerChannel) >> 3;

    return ((MXR_Input *)pInput);
}

/******************************************************************************/
