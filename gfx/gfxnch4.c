/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "gfx\gfx.h"

static int ScrX = 0, ScrY = 0;

ulong gfxNCH4GetCurrScrollOffset(void)
{
	return(GFX_NCH4_SCROLLOFFSET + ScrX + (ScrY * 640));
}

void gfxNCH4PrintExact(struct RastPort *rp, char *puch_Text, uword us_X, uword us_Y)
{
	ubyte *DestPtr = &((ubyte*)rp->p_BitMap)[us_X + us_Y * 640];
	ubyte *FontPtr = (ubyte*)rp->p_Font->p_BitMap;
	uword CharsPerLine = 320 / rp->p_Font->us_Width;
	uword Length = strlen(puch_Text);

	ubyte *DestPtr1;
	ubyte *FontPtr1;
	uword t, h1, h2, i, j;
	ubyte c;

	for (t = 0; t < Length; t++)
	{
		c = puch_Text[t] - rp->p_Font->uch_FirstChar;
		h1 = (c / CharsPerLine) * 320 * rp->p_Font->us_Height;
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
			DestPtr1 += 640;
			FontPtr1 += 320;
		}
	}
}

void gfxNCH4RectFill(struct RastPort *rp, uword us_SX, uword us_SY, uword us_EX, uword us_EY)
{
	uword us_W, us_H;
	ubyte *bm;
	uword i, j;

	if ( !((us_SY >= rp->us_Height) && (us_EY >= rp->us_Height)) ||
		  ((us_SX >= rp->us_Width) && (us_EX >= rp->us_Width)) )
	{
		if (us_SX > us_EX)
		{
			us_SX = us_SX ^ us_EX;
			us_EX = us_SX ^ us_EX;
			us_SX = us_SX ^ us_EX;
		}

		if (us_SY > us_EY)
		{
			us_SY = us_SY ^ us_EY;
			us_EY = us_SY ^ us_EY;
			us_SY = us_SY ^ us_EY;
		}

		if (us_EX >= rp->us_Width)
			us_EX = rp->us_Width;

		if (us_EY >= rp->us_Height)
			us_EY = rp->us_Height;

		us_W = us_EX - us_SX + 1;
		us_H = us_EY - us_SY + 1;
	}

	bm = &((ubyte*)rp->p_BitMap)[us_SX + us_SY * 640];
	for (j = 0; j < us_H; j++)
	{
		for (i = 0; i < us_W; i++)
		{
			if (!i || !j || i==us_W-1 || j==us_W-1) bm[i] = rp->uch_OutlinePenAbs;
			else bm[i] = rp->uch_FrontPenAbs;
		}
		bm += 640;
	}
}

ubyte gfxNCH4ReadPixel(struct RastPort *rp, uword us_X, uword us_Y)
{
	if ((us_X < rp->us_Width) && (us_Y < rp->us_Height))
	{
		return(((ubyte*)rp->p_BitMap)[us_X + us_Y * 640]);
	}
	return(0);
}

void gfxNCH4SetViewPort(uword x, uword y)
{
	ScrX = x;
	ScrY = y;
	if (ScrX < 0) ScrX = 0;
	if (ScrX > 319) ScrX = 319;
	if (ScrY < 0) ScrY = 0;
	if (ScrY > 127) ScrY = 127;
	gfxCorrectUpperRPBitmap();
}

void gfxNCH4Scroll(int x, int y)
{
	ScrX += x;
	ScrY += y;
	gfxNCH4SetViewPort(ScrX, ScrY);
}

void gfxNCH4SetSplit(uword line)
{
}

void gfxNCH4Init()
{
	gfxNCH4SetViewPort(0, 0);
}

/* Put 320 to 640 */
void gfxNCH4PutMCGAToNCH4(void *sp, void *dp, uword us_SourceX, uword us_SourceY,
						  uword us_DestX, uword us_DestY,
						  uword us_Width, uword us_Height, uword sw, uword dw)
{
	unsigned int x;
	ubyte *sbm = (ubyte*)sp;
	ubyte *dbm = (ubyte*)dp;

	dw <<= 2;

	sbm += (ulong)us_SourceX + (ulong)us_SourceY * (ulong)sw;
	dbm += (ulong)us_DestX + (ulong)us_DestY * (ulong)dw;

	while (us_Height--)
	{
		for (x = us_Width; x--;)
		{
			dbm[x] = sbm[x];
		}
		dbm += dw;
		sbm += sw;
	}
}

/* Put Mask 320 to 640 */
void gfxNCH4PutMCGAToNCH4Mask(void *sp, void *dp, uword us_SourceX, uword us_SourceY,
						  uword us_DestX, uword us_DestY,
						  uword us_Width, uword us_Height, uword sw, uword dw)
{
	unsigned int x;
	ubyte *sbm = (ubyte*)sp;
	ubyte *dbm = (ubyte*)dp;

	dw <<= 2;

	sbm += (ulong)us_SourceX + (ulong)us_SourceY * (ulong)sw;
	dbm += (ulong)us_DestX + (ulong)us_DestY * (ulong)dw;

	while (us_Height--)
	{
		for (x = us_Width; x--;)
		{
			dbm[x] = sbm[x] | (dbm[x] & 0x40);
		}
		dbm += dw;
		sbm += sw;
	}
}

/* Overlay Mask 320 to 640 */
void gfxNCH4OLMCGAToNCH4Mask(void *sp, void *dp, uword us_SourceX, uword us_SourceY,
						  uword us_DestX, uword us_DestY,
						  uword us_Width, uword us_Height, uword sw, uword dw)
{
	unsigned int x;
	ubyte *sbm = (ubyte*)sp;
	ubyte *dbm = (ubyte*)dp;

	dw <<= 2;

	sbm += (ulong)us_SourceX + (ulong)us_SourceY * (ulong)sw;
	dbm += (ulong)us_DestX + (ulong)us_DestY * (ulong)dw;

	while (us_Height--)
	{
		for (x = us_Width; x--;)
		{
			if (sbm[x]) dbm[x] = sbm[x] | (dbm[x] & 0x40);
		}
		dbm += dw;
		sbm += sw;
	}
}

/* Overlay 320 to 640 */
void gfxNCH4OLMCGAToNCH4(void *sp, void *dp, uword us_SourceX, uword us_SourceY,
						  uword us_DestX, uword us_DestY,
						  uword us_Width, uword us_Height, uword sw, uword dw)
{
	unsigned int x;
	ubyte *sbm = (ubyte*)sp;
	ubyte *dbm = (ubyte*)dp;

	dw <<= 2;

	sbm += (ulong)us_SourceX + (ulong)us_SourceY * (ulong)sw;
	dbm += (ulong)us_DestX + (ulong)us_DestY * (ulong)dw;

	while (us_Height--)
	{
		for (x = us_Width; x--;)
		{
			if (sbm[x]) dbm[x] = sbm[x];
		}
		dbm += dw;
		sbm += sw;
	}
}

/* Or 320 to 640 */
void gfxNCH4OrMCGAToNCH4(void *sp, void *dp, uword us_SourceX, uword us_SourceY,
						  uword us_DestX, uword us_DestY,
						  uword us_Width, uword us_Height, uword sw, uword dw)
{
	unsigned int x;
	ubyte *sbm = (ubyte*)sp;
	ubyte *dbm = (ubyte*)dp;

	dw <<= 2;

	sbm += (ulong)us_SourceX + (ulong)us_SourceY * (ulong)sw;
	dbm += (ulong)us_DestX + (ulong)us_DestY * (ulong)dw;

	while (us_Height--)
	{
		for (x = us_Width; x--;)
		{
			if (sbm[x]) dbm[x] |= sbm[x];
		}
		dbm += dw;
		sbm += sw;
	}
}

/* And 320 to 640 */
void gfxNCH4AndMCGAToNCH4(void *sp, void *dp, uword us_SourceX, uword us_SourceY,
						  uword us_DestX, uword us_DestY,
						  uword us_Width, uword us_Height, uword sw, uword dw)
{
	unsigned int x;
	ubyte *sbm = (ubyte*)sp;
	ubyte *dbm = (ubyte*)dp;

	dw <<= 2;

	sbm += (ulong)us_SourceX + (ulong)us_SourceY * (ulong)sw;
	dbm += (ulong)us_DestX + (ulong)us_DestY * (ulong)dw;

	while (us_Height--)
	{
		for (x = us_Width; x--;)
		{
			if (sbm[x]) dbm[x] &= ~sbm[x];
		}
		dbm += dw;
		sbm += sw;
	}
}

/* Put 640 to 320 */
void gfxNCH4PutNCH4ToMCGA(void *sp, void *dp, uword us_SourceX, uword us_SourceY,
						  uword us_DestX, uword us_DestY,
						  uword us_Width, uword us_Height, uword sw, uword dw)
{
	unsigned int x;
	ubyte *sbm = (ubyte*)sp;
	ubyte *dbm = (ubyte*)dp;

	sw <<= 2;

	sbm += (ulong)us_SourceX + (ulong)us_SourceY * (ulong)sw;
	dbm += (ulong)us_DestX + (ulong)us_DestY * (ulong)dw;

	while (us_Height--)
	{
		for (x = us_Width; x--;)
		{
			dbm[x] = sbm[x];
		}
		dbm += dw;
		sbm += sw;
	}
}
