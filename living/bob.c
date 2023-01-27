/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "living\bob.h"
#include "memory\memory.h"
#include "gfx\gfx.h"

#define MaxBob 		32

struct ObjStruct
{
	void *SPtr;		// source bitmap
	void *DPtr;		// dest. bitmap
	void *RPtr;		// rescue bitmap [w*h]
	int	x,y;
	int	w,h;		// width, height
	int	frame;
	int	vis;
	int sx,sy;
	int ox,oy;
	int DrawMe;		// internal
} Bob[MaxBob];

void BobInitLists(void)
{
	uword t;
	for (t = 0; t < MaxBob; t++)
	{
		Bob[t].x = -1;
		Bob[t].y = -1;
	}
}

// Liefert das Handle des Bobings....im Fehlerfall: -1
uword BobInit(void *BitMap, uword Width, uword Height)
{
	uword ID = 0;

	while (ID < MaxBob)
	{
		if (Bob[ID].x == -1) break;
		ID++;
	}

	if (ID == MaxBob)
	{
		#ifdef THECLOU_DEBUG
		Log("Too many bobs! Increase 'MaxBob'.");
		#endif
		return(-1);
	}

	Bob[ID].SPtr = BitMap;
	Bob[ID].DPtr = l_wrp->p_BitMap;

	if ((Bob[ID].RPtr = MemAlloc(Width * Height)) == NULL) return(-1);

	Bob[ID].w = Width;
	Bob[ID].h = Height;

	Bob[ID].x = Bob[ID].y = 0;
	Bob[ID].sx = Bob[ID].sy = 0;
	Bob[ID].ox = Bob[ID].oy = 0;

	Bob[ID].vis = 0;
	Bob[ID].frame = 0;

	Bob[ID].DrawMe = 1;
	return(ID);
}

static void BobCheckVis(uword ID)
{
	uword s, t;

	for (t = ID; t < MaxBob; t++)
	{
		if (Bob[t].DrawMe != 1) continue;

		// moeglicherweise (wenn es zu Fehlern kommt),
		// muss "t+1" durch "ID+1" ersetzt werden
		for (s = t + 1; s < MaxBob; s++)
		{
			if ((Bob[s].DrawMe == 1) || (Bob[s].vis != 1)) continue;

			if ((Bob[s].x >= Bob[t].x) &&
				(Bob[s].y >= Bob[t].y) &&
				(Bob[s].x <= (Bob[t].x + Bob[t].w)) &&
				(Bob[s].y <= (Bob[t].y + Bob[t].h)))
			{
				Bob[s].DrawMe = 1;
				continue;
			}

			if (((Bob[s].x + Bob[s].w) >= Bob[t].x) &&
				(Bob[s].y >= Bob[t].y) &&
				((Bob[s].x + Bob[s].w) <= (Bob[t].x + Bob[t].w)) &&
			    (Bob[s].y <= (Bob[t].y + Bob[t].h)))
			{
				Bob[s].DrawMe = 1;
				continue;
			}

			if ((Bob[s].x >= Bob[t].x) &&
			    ((Bob[s].y+Bob[s].h) >= Bob[t].y) &&
			    (Bob[s].x <= (Bob[t].x + Bob[t].w)) &&
			    ((Bob[s].y + Bob[s].h) <= (Bob[t].y + Bob[t].h)))
			{
				Bob[s].DrawMe = 1;
				continue;
			}

			if (((Bob[s].x + Bob[s].w) >= Bob[t].x) &&
				((Bob[s].y + Bob[s].h) >= Bob[t].y) &&
				((Bob[s].x + Bob[s].w) <= (Bob[t].x + Bob[t].w)) &&
				((Bob[s].y + Bob[s].h) <= (Bob[t].y + Bob[t].h)))
			{
				Bob[s].DrawMe = 1;
				continue;
			}

			if ((Bob[s].x >= Bob[t].ox) &&
			    (Bob[s].y >= Bob[t].oy) &&
			    (Bob[s].x <= (Bob[t].ox + Bob[t].w)) &&
			    (Bob[s].y <= (Bob[t].oy + Bob[t].h)))
			{
				Bob[s].DrawMe = 1;
				continue;
			}

			if (((Bob[s].x + Bob[s].w) >= Bob[t].ox) &&
			    (Bob[s].y >= Bob[t].oy) &&
			    ((Bob[s].x + Bob[s].w) <= (Bob[t].ox + Bob[t].w)) &&
			    (Bob[s].y <= (Bob[t].oy + Bob[t].h)))
			{
				Bob[s].DrawMe = 1;
				continue;
			}

			if ((Bob[s].x >= Bob[t].ox) &&
			    ((Bob[s].y + Bob[s].h) >= Bob[t].oy) &&
			    (Bob[s].x <= (Bob[t].ox + Bob[t].w)) &&
			    ((Bob[s].y + Bob[s].h) <= (Bob[t].oy + Bob[t].h)))
			{
				Bob[s].DrawMe = 1;
				continue;
			}

			if (((Bob[s].x + Bob[s].w) >= Bob[t].ox) &&
			    ((Bob[s].y + Bob[s].h) >= Bob[t].oy) &&
			    ((Bob[s].x + Bob[s].w) <= (Bob[t].ox + Bob[t].w)) &&
			    ((Bob[s].y + Bob[s].h) <= (Bob[t].oy + Bob[t].h)))
			{
				Bob[s].DrawMe = 1;
				continue;
			}
		}
	}
}

static void BobRefresh(uword ID)
{
	int t;	// MUSS signed sein!

	BobCheckVis(ID);

	t = MaxBob - 1;
	while (t >= ID)
	{
		if (Bob[t].DrawMe == 1)
		{
			if ((Bob[t].vis == 1) || (Bob[t].vis == 2))
			{
				gfxNCH4PutMCGAToNCH4(Bob[t].RPtr, Bob[t].DPtr,
					0, 0,
					Bob[t].ox, Bob[t].oy,
					Bob[t].w, Bob[t].h,
					Bob[t].w, 160);

				Bob[t].ox = Bob[t].x;
				Bob[t].oy = Bob[t].y;
			}
			Bob[t].vis &= 1;
		}
		t--;
	}

	for (t = ID; t < MaxBob; t++)
	{
		if ((Bob[t].vis == 1) && (Bob[t].DrawMe == 1))
		{
			gfxNCH4PutNCH4ToMCGA(Bob[t].DPtr, Bob[t].RPtr,
				Bob[t].x, Bob[t].y,
				0, 0,
				Bob[t].w, Bob[t].h,
				160, Bob[t].w);

			gfxNCH4OLMCGAToNCH4(Bob[t].SPtr, Bob[t].DPtr,
				Bob[t].sx, Bob[t].sy,
				Bob[t].x, Bob[t].y,
				Bob[t].w, Bob[t].h,
				320, 160);

			Bob[t].DrawMe = 0;
		}
	}
}

void BobDone(uword ID)
{
	Bob[ID].vis = 0;
	Bob[ID].DrawMe = 1;
	BobRefresh(ID);
	MemFree(Bob[ID].RPtr, Bob[ID].w * Bob[ID].h);
}

void BobVis(uword ID)
{
	Bob[ID].DrawMe = 1;
	if (Bob[ID].vis != 1)
	{
		Bob[ID].vis = 3;
	}
	BobRefresh(ID);
}

void BobInVis(uword ID)
{
	Bob[ID].DrawMe = 1;
	if (Bob[ID].vis != 0)
	{
		Bob[ID].vis = 2;
	}
	BobRefresh(ID);
}

void BobSet(uword ID, uword x, uword y , uword xs, uword ys)
{
	Bob[ID].ox = Bob[ID].x;
	Bob[ID].oy = Bob[ID].y;
	Bob[ID].sx = xs;
	Bob[ID].sy = ys;
	Bob[ID].x = x;
	Bob[ID].y = y;
	Bob[ID].DrawMe = 1;
	if (Bob[ID].vis == 1)
	{
		BobRefresh(ID);
	}
}

void BobSetDarkness(ubyte b)
{

	#ifdef THECLOU_DEBUG
	Log("%s|%s(%d) not implemented", __FILE__, __func__, b);
	#endif
}
