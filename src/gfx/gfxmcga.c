/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "gfx/gfx.h"

void gfxMCGABlit(struct RastPort *srp, uword us_SourceX, uword us_SourceY, struct RastPort *drp, uword us_DestX,
                 uword us_DestY, uword us_Width, uword us_Height, uint32_t ul_BlitMode)
{
    unsigned int x = 0;
    ubyte *sbm = (ubyte *)srp->p_BitMap;
    ubyte *dbm = (ubyte *)drp->p_BitMap;

    sbm += (uint32_t)us_SourceX + (uint32_t)us_SourceY * srp->us_Width;
    dbm += (uint32_t)us_DestX + (uint32_t)us_DestY * drp->us_Width;

    switch (ul_BlitMode)
    {
        case GFX_ONE_STEP: /* 1 zu 1 kopieren */
            while (us_Height--)
            {
                for (x = us_Width; x--;)
                {
                    dbm[x] = sbm[x];
                }
                dbm += drp->us_Width;
                sbm += srp->us_Width;
            }
            break;
        case GFX_OVERLAY: /* Farbe 0 transparent */
            while (us_Height--)
            {
                for (x = us_Width; x--;)
                {
                    if (sbm[x]) dbm[x] = sbm[x];
                }
                dbm += drp->us_Width;
                sbm += srp->us_Width;
            }
            break;
    }
}

void gfxMCGAPrintExact(struct RastPort *rp, char *puch_Text, uword us_X, uword us_Y)
{
    SDL_LockSurface(rp->p_Font->pSurface);

    ubyte *DestPtr = &((ubyte *)rp->p_BitMap)[us_X + us_Y * rp->us_Width];
    ubyte *FontPtr = (ubyte *)rp->p_Font->pSurface->pixels;
    uword CharsPerLine = rp->p_Font->pSurface->w / rp->p_Font->us_Width;
    uword Length = strlen(puch_Text);

    ubyte *DestPtr1 = NULL;
    ubyte *FontPtr1 = NULL;
    uword t = 0;
    uword h1 = 0;
    uword h2 = 0;
    uword i = 0;
    uword j = 0;
    ubyte c = 0;

    for (t = 0; t < Length; t++)
    {
        c = puch_Text[t] - rp->p_Font->uch_FirstChar;
        h1 = (c / CharsPerLine) * rp->p_Font->pSurface->pitch * rp->p_Font->us_Height;
        h2 = (c % CharsPerLine) * rp->p_Font->us_Width;
        FontPtr1 = &FontPtr[h1 + h2];
        DestPtr1 = &DestPtr[t * rp->p_Font->us_Width];

        for (j = 0; j < rp->p_Font->us_Height; j++)
        {
            for (i = 0; i < rp->p_Font->us_Width; i++)
            {
                if (FontPtr1[i])
                    DestPtr1[i] = rp->uch_FrontPenAbs;
                else if (rp->us_DrawMode == GFX_JAM_2)
                    DestPtr1[i] = rp->uch_BackPenAbs;
            }
            DestPtr1 += rp->us_Width;
            FontPtr1 += rp->p_Font->pSurface->pitch;
        }
    }

    SDL_UnlockSurface(rp->p_Font->pSurface);
}

void gfxMCGARectFill(struct RastPort *rp, uword us_X, uword us_Y, uword us_X1, uword us_Y1)
{
    uword us_W = (us_X1 - us_X) + 1;
    uword us_H = (us_Y1 - us_Y) + 1;

    // us_X += rp->us_LeftEdge;
    // us_Y += rp->us_TopEdge;
    ubyte *bm = &((ubyte *)rp->p_BitMap)[us_X + us_Y * rp->us_Width];
    uword i = 0;
    uword j = 0;
    for (j = 0; j < us_H; j++)
    {
        for (i = 0; i < us_W; i++)
        {
            if (!i || !j || i == us_W - 1 || j == us_W - 1)
                bm[i] = rp->uch_OutlinePenAbs;
            else
                bm[i] = rp->uch_FrontPenAbs;
        }
        bm += rp->us_Width;
    }
}

ubyte gfxMCGAReadPixel(struct RastPort *rp, uword us_X, uword us_Y)
{
    if ((us_X < rp->us_Width) && (us_Y < rp->us_Height))
    {
        // us_X += rp->us_LeftEdge;
        // us_Y += rp->us_TopEdge;
        return (((ubyte *)rp->p_BitMap)[us_X + us_Y * rp->us_Width]);
    }
    return 0;
}
