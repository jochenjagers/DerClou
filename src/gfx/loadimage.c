/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ https://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "SDL.h"
#include "gfx/gfx.h"
#include "memory/memory.h"
// #include "base\base.h"
// #include "text\text.h"
#include "disk/disk.h"

typedef struct
{
    uword width;
    uword height;
    word xOrigin;
    word yOrigin;
    ubyte numPlanes;
    ubyte mask;
    ubyte compression;
    ubyte padding;
    uword transClr;
    ubyte xAspect;
    ubyte yAspect;
    uword pageWidth;
    uword pageHeight;
} ILBMHeader;

static const ubyte idFORM[] = {'F', 'O', 'R', 'M'};
static const ubyte idILBM[] = {'I', 'L', 'B', 'M'};
static const ubyte idBMHD[] = {'B', 'M', 'H', 'D'};
static const ubyte idBODY[] = {'B', 'O', 'D', 'Y'};
static const ubyte idCMAP[] = {'C', 'M', 'A', 'P'};

static inline uint32_t PeekL_BE(ubyte *p)
{
    return (((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3]);
}

static inline uword PeekW_BE(ubyte *p) { return (((uword)p[0] << 8) | (uword)p[1]); }

static uint32_t IFFFindChunk(ubyte *pFileBuffer, int32_t sizeOfForm, const ubyte *chkID)
{
    uint32_t currPos = 12;
    while ((currPos + 4) <= sizeOfForm)
    {
        if (!memcmp(&pFileBuffer[currPos], chkID, 4))
        {
            currPos += 4;
            return (currPos);
        }
        currPos += 2;
    }
    return (0);
}

static void MakeMCGA(ubyte b, ubyte *pic, ubyte PlSt, int c)
{
    if ((b & 0x80) && (c > 0)) pic[0] |= PlSt;
    if ((b & 0x40) && (c > 1)) pic[1] |= PlSt;
    if ((b & 0x20) && (c > 2)) pic[2] |= PlSt;
    if ((b & 0x10) && (c > 3)) pic[3] |= PlSt;
    if ((b & 0x08) && (c > 4)) pic[4] |= PlSt;
    if ((b & 0x04) && (c > 5)) pic[5] |= PlSt;
    if ((b & 0x02) && (c > 6)) pic[6] |= PlSt;
    if ((b & 0x01) && (c > 7)) pic[7] |= PlSt;
}

static void ILBMUncompressToSurface(ubyte *pBody, ILBMHeader *hdr, SDL_Surface *pSurface)
{
    ubyte *pPlane = NULL, *pLine = NULL;
    int x = 0, y = 0, plane = 0, breite = 0;
    uword a = 0;
    ubyte o = 0;

    pLine = (ubyte *)pSurface->pixels;
    for (y = 1; y <= hdr->height; y++)
    {
        for (plane = 0; plane < hdr->numPlanes; plane++)
        {
            pPlane = pLine;
            breite = (hdr->width + 15) & 0xFFF0;
            do
            {
                a = *pBody++; /* Kommando (wiederholen oder uebernehmen */
                if (a > 128)
                { /* Zeichen wiederholen */
                    a = 257 - a;
                    o = *pBody++;
                    for (x = 1; x <= a; x++)
                    {
                        MakeMCGA(o, pPlane, (1 << plane), breite);
                        pPlane += 8;
                        breite -= 8;
                    }
                }
                else
                { /* Zeichen uebernehmen */
                    for (x = 0; x <= a; x++)
                    {
                        o = *pBody++;
                        MakeMCGA(o, pPlane, (1 << plane), breite);
                        pPlane += 8;
                        breite -= 8;
                    }
                }
            } while (breite > 0);
        }
        pLine += pSurface->pitch;
    }
}

static void ILBMCopyToSurface(ubyte *pBody, ILBMHeader *hdr, SDL_Surface *pSurface)
{
    ubyte *pPlane = NULL, *pLine = NULL;
    int y = 0, plane = 0, breite = 0;
    ubyte o = 0;

    pLine = (ubyte *)pSurface->pixels;
    for (y = 1; y <= hdr->height; y++)
    {
        for (plane = 0; plane < hdr->numPlanes; plane++)
        {
            pPlane = pLine;
            breite = (hdr->width + 15) & 0xFFF0;
            do
            {
                o = *pBody++;
                MakeMCGA(o, pPlane, (1 << plane), breite);
                pPlane += 8;
                breite -= 8;
            } while (breite > 0);
        }
        pLine += pSurface->pitch;
    }
}

static SDL_Surface *gfxLoadILBM(ubyte *pFileBuffer, int32_t ulSizeOfFile, const char *puch_Pathname)
{
    int32_t sizeOfForm = PeekL_BE(&pFileBuffer[4]) + 8;
    if (sizeOfForm > ulSizeOfFile)
    {
        // file truncated!
        Log("ILBM truncated: %s\n", puch_Pathname);
        return (NULL);
    }
    uint32_t currPos = IFFFindChunk(pFileBuffer, sizeOfForm, idBMHD);
    if (!currPos)
    {
        Log("No BMHD found: %s\n", puch_Pathname);
        return (NULL);
    }

    int32_t chunkSize = PeekL_BE(&pFileBuffer[currPos]);
    currPos += 4;
    if (chunkSize < 20)
    {
        Log("Invalid BMHD: %s\n", puch_Pathname);
        return (NULL);
    }

    ILBMHeader hdr;
    hdr.width = PeekW_BE(&pFileBuffer[currPos]);
    currPos += 2;
    hdr.height = PeekW_BE(&pFileBuffer[currPos]);
    currPos += 2;
    hdr.xOrigin = PeekW_BE(&pFileBuffer[currPos]);
    currPos += 2;
    hdr.yOrigin = PeekW_BE(&pFileBuffer[currPos]);
    currPos += 2;
    hdr.numPlanes = pFileBuffer[currPos];
    currPos++;
    hdr.mask = pFileBuffer[currPos];
    currPos++;
    hdr.compression = pFileBuffer[currPos];
    currPos++;
    currPos++;  // padding
    hdr.transClr = PeekW_BE(&pFileBuffer[currPos]);
    currPos += 2;
    hdr.xAspect = pFileBuffer[currPos];
    currPos++;
    hdr.yAspect = pFileBuffer[currPos];
    currPos++;
    hdr.pageWidth = PeekW_BE(&pFileBuffer[currPos]);
    currPos += 2;
    hdr.pageHeight = PeekW_BE(&pFileBuffer[currPos]);
    currPos += 2;

    if (hdr.compression > 1)
    {
        Log("Unsupported compression: %s\n", puch_Pathname);
        return (NULL);
    }

    currPos = IFFFindChunk(pFileBuffer, sizeOfForm, idBODY);
    if (!currPos)
    {
        Log("No BODY found: %s\n", puch_Pathname);
        return (NULL);
    }
    chunkSize = PeekL_BE(&pFileBuffer[currPos]);
    currPos += 4;

    SDL_Surface *pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, hdr.width, hdr.height, 8, 0, 0, 0, 0);
    if (!pSurface)
    {
        Log("Failed to create surface: %s\n", puch_Pathname);
        return (NULL);
    }

    if (hdr.compression == 1)
    {
        ILBMUncompressToSurface(&pFileBuffer[currPos], &hdr, pSurface);
    }
    else
    {
        ILBMCopyToSurface(&pFileBuffer[currPos], &hdr, pSurface);
    }

    currPos = IFFFindChunk(pFileBuffer, sizeOfForm, idCMAP);
    if (currPos)
    {
        chunkSize = PeekL_BE(&pFileBuffer[currPos]);
        currPos += 4;
        int numColors = chunkSize / 3;
        if (numColors > 0)
        {
            if (numColors > 256) numColors = 256;

            int c = 0;
            SDL_Color *pc = NULL;
            for (c = 0; c < numColors; c++)
            {
                pc = &pSurface->format->palette->colors[c];
                pc->r = pFileBuffer[currPos];
                currPos++;
                pc->g = pFileBuffer[currPos];
                currPos++;
                pc->b = pFileBuffer[currPos];
                currPos++;
                pc->unused = 0;
            }
        }
    }
    return (pSurface);
}

SDL_Surface *gfxLoadImage(const char *puch_Pathname)
{
    FILE *p_File = NULL;
    int32_t ul_SizeOfFile = 0;
    ubyte *p_FileBuffer = NULL;
    SDL_Surface *pSurface = NULL;

    ul_SizeOfFile = dskFileLength(puch_Pathname);
    if (!ul_SizeOfFile)
    {
        Log("gfxLoadImage empty file: %s", puch_Pathname);
        return (NULL);
    }

    p_File = dskOpen(puch_Pathname, "rb", 0);
    if (!p_File)
    {
        Log("gfxLoadImage failed to open: %s", puch_Pathname);
        return (NULL);
    }

    p_FileBuffer = MemAlloc(ul_SizeOfFile);
    if (p_FileBuffer)
    {
        dskRead(p_File, p_FileBuffer, ul_SizeOfFile);
    }
    dskClose(p_File);

    if (!p_FileBuffer)
    {
        Log("gfxLoadImage out of memory: %s", puch_Pathname);
        return (NULL);
    }

    if (!memcmp(&p_FileBuffer[0], idFORM, 4) && !memcmp(&p_FileBuffer[8], idILBM, 4))
    {
        // IFF ILBM (standard)
        pSurface = gfxLoadILBM(p_FileBuffer, ul_SizeOfFile, puch_Pathname);
        if (!pSurface)
        {
            Log("gfxLoadImage pSurface = NULL: %s", puch_Pathname);
        }
    }
    else
    {
        Log("gfxLoadImage not an IFF file: %s", puch_Pathname);
    }

    MemFree(p_FileBuffer, ul_SizeOfFile);
    return (pSurface);
}
