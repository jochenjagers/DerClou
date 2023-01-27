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
#include <stdio.h>

#include "mxr.h"

/******************************************************************************/

typedef struct
{
    char sMagic[20];
    unsigned short wDataOffset;
    unsigned short wVersion;
    unsigned short wVersionCheck;
} VOCHeader;
enum
{
    SIZEOF_VOCHeader = 26
};

typedef struct
{
    unsigned char nBlockType;
    unsigned char nSize[3];
    unsigned char nSampleRate;
    unsigned char nCompression;
} VOCDataHeader;
enum
{
    SIZEOF_VOCDataHeader = 6
};

typedef struct
{
    FILE *pFile;
    uint32_t dwNumSamples;
    uint32_t dwDataOffset;
    uint32_t dwDataSize;
    uint32_t dwSampleSize;
    uint32_t dwPosition;
    uint32_t dwSamplesPerSec;
    unsigned short wChannels;
    unsigned short wBitsPerSample;
} VOCFile;

/******************************************************************************/

static int32_t VOCOpen(VOCFile *voc, const char *pszFileName)
{
    VOCHeader hdr;
    VOCDataHeader dat;

    voc->pFile = fopen(pszFileName, "rb");
    if (!voc->pFile)
    {
        return (0);
    }

    if (fread(&hdr, 1, SIZEOF_VOCHeader, voc->pFile) != SIZEOF_VOCHeader) goto Lgetout;
    if (memcmp(hdr.sMagic, "Creative Voice File\x1a", 20)) goto Lgetout;
    fseek(voc->pFile, hdr.wDataOffset, SEEK_SET);
    if (fread(&dat, 1, SIZEOF_VOCDataHeader, voc->pFile) != SIZEOF_VOCDataHeader) goto Lgetout;
    if (dat.nBlockType != 1) goto Lgetout;
    if (dat.nCompression != 0) goto Lgetout;

    voc->dwDataSize = (dat.nSize[0] | (dat.nSize[1] << 8) | (dat.nSize[2] << 16)) - 2;
    voc->dwDataOffset = ftell(voc->pFile);
    voc->dwSampleSize = 1;
    voc->dwNumSamples = voc->dwDataSize / voc->dwSampleSize;
    voc->dwPosition = 0;

    voc->dwSamplesPerSec = 1000000L / (256L - dat.nSampleRate);
    voc->wChannels = 1;
    voc->wBitsPerSample = 8;

    return (1);

Lgetout:
    fclose(voc->pFile);
    voc->pFile = NULL;
    return (0);
}

/******************************************************************************/

typedef struct
{
    MXR_Input mxrInput;
    VOCFile vocFile;
} MXR_InputVOC;

static uint32_t MXR_ProcessInputVOC(MXR_InputVOC *pInput, void *pStream, uint32_t nNumSamples)
{
    int32_t nSamplesRemain = 0, nSamplesRead = 0;
    nSamplesRemain = (pInput->vocFile.dwDataSize - pInput->vocFile.dwPosition) / pInput->mxrInput.fmt.nSampleSize;
    if (nNumSamples > nSamplesRemain)
    {
        nNumSamples = nSamplesRemain;
    }
    if (nNumSamples > 0)
    {
        nSamplesRead = fread(pStream, pInput->mxrInput.fmt.nSampleSize, nNumSamples, pInput->vocFile.pFile);
        if (nSamplesRead > 0)
        {
            pInput->vocFile.dwPosition += (nSamplesRead * pInput->mxrInput.fmt.nSampleSize);
            return (nSamplesRead);
        }
    }
    return (0);  // MXR will destroy this input
}

static void MXR_DestroyInputVOC(MXR_InputVOC *pInput)
{
    if (pInput->vocFile.pFile)
    {
        fclose(pInput->vocFile.pFile);
    }
    MXR_MemFree(pInput, sizeof(MXR_InputVOC));
}

MXR_Input *MXR_CreateInputVOC(const char *pszFileName)
{
    MXR_InputVOC *pInput = NULL;
    pInput = (MXR_InputVOC *)MXR_MemAlloc(sizeof(MXR_InputVOC));
    if (!pInput)
    {
        return (NULL);
    }
    pInput->mxrInput.pProcess = (MXR_ProcessInputFunc)MXR_ProcessInputVOC;
    pInput->mxrInput.pDestroy = (MXR_DestroyInputFunc)MXR_DestroyInputVOC;
    pInput->mxrInput.nVolume = 128;

    if (!VOCOpen(&pInput->vocFile, pszFileName))
    {
        MXR_MemFree(pInput, sizeof(MXR_InputVOC));
        return (NULL);
    }

    pInput->mxrInput.fmt.nSamplesPerSec = pInput->vocFile.dwSamplesPerSec;
    pInput->mxrInput.fmt.nBitsPerChannel = pInput->vocFile.wBitsPerSample;
    pInput->mxrInput.fmt.nNumChannels = pInput->vocFile.wChannels;
    pInput->mxrInput.fmt.nSampleSize = (pInput->mxrInput.fmt.nNumChannels * pInput->mxrInput.fmt.nBitsPerChannel) >> 3;

    return ((MXR_Input *)pInput);
}

/******************************************************************************/
