/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "SDL.h"
#include "gfx\gfx.h"

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif
#ifndef MODULE_BASE
#include "base\base.h"
#endif
#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#include "gfx\gfx.ph"

/************************************************/

char bFullscreen = 0;	// default, don't change
char bScale2x = 1;		// default, don't change

/************************************************/

#include "gfx\scale2x.h"
static FUNCTION_SCALE2X(scale2x, ubyte, 320, 200)

/************************************************/

#include "port\port.h"

#define GFX_BOARD_SIZE	(640*328)

SDL_Surface *SurfaceScreen = NULL;
static const char *TitleStr = "Der Clou!";
static ubyte *NCH4Buffer = NULL;

/************************************************/

int gfxInitSDL(void)
{
	if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
	{
		Uint16 w = 320, h = 200;
		Uint32 flags = SDL_SWSURFACE;
		if (bFullscreen)
		{
			flags |= SDL_FULLSCREEN;
			h += 40;
			bScale2x = 1;
		}
		if (bScale2x)
		{
			w <<= 1;
			h <<= 1;
		}
		//SDL_WM_SetIcon(SDL_LoadBMP("pictures\\icon.bmp"), NULL);
		SurfaceScreen = SDL_SetVideoMode(w, h, 8, flags);
		if (SurfaceScreen)
		{
			if (!bFullscreen) SDL_WM_SetCaption(TitleStr, TitleStr);
			GfxBoardBase = (ubyte *)MemAlloc(GFX_BOARD_SIZE);
			if (GfxBoardBase)
			{
				NCH4Buffer = (ubyte*)MemAlloc(GFX_PAGE_SIZE);
				return(1);
			}
		}
	}
	return(0);
}

void gfxDoneSDL(void)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	if (GfxBoardBase) MemFree(GfxBoardBase, GFX_BOARD_SIZE);
	if (NCH4Buffer) MemFree(NCH4Buffer, GFX_PAGE_SIZE);
}

void gfxUpdateSDL(struct RastPort *rp)
{
	ubyte *src, *dst;
	if ((rp != &RefreshRP) && (rp != &PrepareRP))
	{
		SDL_LockSurface(SurfaceScreen);

		src = GfxBoardBase;
		dst = SurfaceScreen->pixels;

		if (bFullscreen) dst += 640*40;

		if (GfxBase.uch_VideoMode == GFX_VIDEO_NCH4)
		{
			gfxNCH4PutNCH4ToMCGA(src + gfxNCH4GetCurrScrollOffset(), NCH4Buffer, 0, 0, 0, 0, 320, 128, 160, 320);
			gfxNCH4PutNCH4ToMCGA(src, NCH4Buffer+(320*128), 0, 0, 0, 0, 320, 72, 160, 320);
			src = NCH4Buffer;
		}

		if (bScale2x) scale2x(src, dst);
		else memcpy(dst, src, GFX_PAGE_SIZE);

		SDL_UnlockSurface(SurfaceScreen);

		SDL_UpdateRect(SurfaceScreen, 0,0,0,0);
	}
}

/********************************************************************
 * inits & dons
 */

void gfxInit(void)
{
	if (!gfxInitSDL())
	{
		return;
	}

	gfxSetDestRPForShow(NULL);

	/* diese RP mssen nur ein Bild maximaler Gr”áe aufnehmen k”nnen */
	/* in anderen Modulen wird vorausgesetzt, daá alle RastPorts gleich */
	/* groá sind und auch gleich groá wie die StdBuffer sind */
	/* StdBuffer = 61 * 1024 = 62464, XMS: 62400 */

	/* Ausnahme (nachtr„glich) : der RefreshRP ist nur 320 * 140 Pixel groá!! */

	gfxInitXMSRastPort(&StdRP0InXMS    , 320, 195);	/* CMAP muá auch Platz haben ! */
	gfxInitXMSRastPort(&StdRP1InXMS    , 320, 195);
	gfxInitXMSRastPort(&AnimRPInXMS    , 320, 195);
	gfxInitXMSRastPort(&AddRPInXMS     , 320, 195);
	gfxInitXMSRastPort(&LSObjectRPInXMS, 320, 195);

	gfxInitXMSRastPort(&LSFloorRPInXMS, 320, 32);

	/* der RefreshRP muá den ganzen Bildschirm aufnehmen k”nnen */
	gfxInitXMSRastPort(&RefreshRPInXMS, 320, 200);

	gfxInitRastPort(&RefreshRP, 0,   0, 320, 140,   0,   0, GFX_LOAD_BUFFER);
	gfxInitRastPort(&PrepareRP, 0,   0, 320, 192,   0,   0, GFX_DECR_BUFFER);
	gfxInitRastPort(&LowerRP  , 0,   0, 320, 140,   0, 191, GfxBoardBase);
	gfxInitRastPort(&MenuRP   , 0, 140, 320,  50, 191, 255, GfxBoardBase + 320*140);

	gfxInitRastPort(&NCH4ScrollRP, 0,   0, 640, 256,   0, 191, (void *)((ulong)GfxBoardBase + (ulong)GFX_NCH4_SCROLLOFFSET));
	gfxInitRastPort(&NCH4MenuRP  , 0, 128, 320,  72, 191, 255, GfxBoardBase);
	gfxInitRastPort(&NCH4UpperRP, 0,   0, 320,  60,   0, 191, (void *)((ulong)GfxBoardBase + (ulong)GFX_NCH4_SCROLLOFFSET));
	// TopEdge von diesem RP muss 0 sein, da der Offset schon bei der Adresse
	// der BitMap richtig uebergeben wird

	l_wrp = &LowerRP;
	u_wrp = &LowerRP;
	m_wrp = &MenuRP;

	bubbleFont = gfxOpenFont(GFX_BUBBLE_FONT_NAME, 4, 8, 32, 255, 320, 24);
	menuFont   = gfxOpenFont(GFX_MENU_FONT_NAME, 5, 9, 32, 255, 320, 36);

	gfxSetFont(l_wrp, bubbleFont);
	gfxSetFont(u_wrp, bubbleFont);
	gfxSetFont(m_wrp, menuFont);

	gfxSetFont(&NCH4ScrollRP, bubbleFont);
	gfxSetFont(&NCH4MenuRP, menuFont);
	gfxSetFont(&NCH4UpperRP, bubbleFont);

	gfxInitCollList();
	gfxInitPictList();

	gfxSetVideoMode(GFX_VIDEO_MCGA); // nach RastPorts !!
}

void gfxDone(void)
{
	if (PictureList)
	{
		RemoveList(PictureList);
		PictureList = NULL;
	}

	if (CollectionList)
	{
		RemoveList(CollectionList);
		CollectionList = NULL;
	}

	gfxCloseFont(bubbleFont);
	gfxCloseFont(menuFont);

	gfxDoneXMSRastPort(&RefreshRPInXMS);
	gfxDoneXMSRastPort(&LSFloorRPInXMS);
	gfxDoneXMSRastPort(&LSObjectRPInXMS);

	gfxDoneXMSRastPort(&StdRP0InXMS);
	gfxDoneXMSRastPort(&StdRP1InXMS);
	gfxDoneXMSRastPort(&AnimRPInXMS);
	gfxDoneXMSRastPort(&AddRPInXMS);

	gfxSetVideoMode(GFX_VIDEO_TEXT);

	gfxDoneSDL();
}

void gfxSetDestRPForShow(struct RastPort *rp)
{
	GfxBase.p_DestRPForShow = rp;
}

void gfxSetVideoMode(ubyte uch_NewMode)
{
	GfxBase.uch_VideoMode = uch_NewMode;

	memset(GfxBoardBase, 0, GFX_BOARD_SIZE);

	switch (uch_NewMode)
	{
		case GFX_VIDEO_MCGA:
			LowerRP.uch_VideoMode = GFX_VIDEO_MCGA;
			MenuRP.uch_VideoMode = GFX_VIDEO_MCGA;
			l_wrp = &LowerRP;
			u_wrp = &LowerRP;
			m_wrp = &MenuRP;
			gfxSetRGB(NULL, 0, 0, 64, 48);
			gfxSetRGB(NULL, 255, 255, 255, 255); // Maus
		break;
		case GFX_VIDEO_NCH4:
			gfxNCH4Init();
			l_wrp = &NCH4ScrollRP;
			u_wrp = &NCH4UpperRP;
			m_wrp = &NCH4MenuRP;
			l_wrp->uch_VideoMode = GFX_VIDEO_NCH4;
			u_wrp->uch_VideoMode = GFX_VIDEO_NCH4;
			m_wrp->uch_VideoMode = GFX_VIDEO_NCH4;
			gfxSetRGB(NULL, 0, 0, 64, 48);
			memset(NCH4Buffer, 0, GFX_PAGE_SIZE);
		break;
		case GFX_VIDEO_TEXT:
			/* not available */
		break;
	}
}

/*
 * sollte nach jedem Scrolling (vor jedem Darstellen einer
 * Bubble) aufgerufen werden
 */
void gfxCorrectUpperRPBitmap(void)
{
	NCH4UpperRP.p_BitMap = GfxBoardBase + gfxNCH4GetCurrScrollOffset();
}

/********************************************************************
 * lists
 */

static void gfxInitCollList(void)
{
	char pathname[TXT_KEY_LENGTH];
	LIST *tempList = CreateList(0);
	NODE *n;

	CollectionList = CreateList(0);

	dskBuildPathName(TEXT_DIRECTORY, COLL_LIST_TXT, pathname);
	ReadList(tempList, 0, pathname, 0);

	for (n = (NODE *) LIST_HEAD(tempList); NODE_SUCC(n); n = (NODE *) NODE_SUCC(n))
	{
		struct Collection *coll;

		coll = CreateNode(CollectionList, sizeof(struct Collection), txtGetKey(2, NODE_NAME(n)));

		coll->us_CollId     = (uword)  txtGetKeyAsULONG(1, NODE_NAME(n));

		coll->puch_Filename = NODE_NAME(coll);
		coll->p_Prepared    = 0;

		coll->us_TotalWidth  = (uword) txtGetKeyAsULONG(3, NODE_NAME(n));
		coll->us_TotalHeight = (uword) txtGetKeyAsULONG(4, NODE_NAME(n));

		coll->uch_ColorRangeStart = (uword) txtGetKeyAsULONG(5, NODE_NAME(n));
		coll->uch_ColorRangeEnd   = (uword) txtGetKeyAsULONG(6, NODE_NAME(n));

		#ifdef THECLOU_PROFIDISK
		#ifdef THECLOU_CDROM_VERSION
		coll->fromDisk = (ubyte)txtGetKeyAsULONG(7, NODE_NAME(n));
		#endif
		#endif
	}

	RemoveList(tempList);
}

static void gfxInitPictList(void)
{
	char pathname[TXT_KEY_LENGTH];
	LIST *tempList = CreateList(0);
	NODE *n;

	PictureList = CreateList(0);

	dskBuildPathName(TEXT_DIRECTORY, PICT_LIST_TXT, pathname);
	ReadList(tempList, 0, pathname, 0);

	for (n = (NODE *) LIST_HEAD(tempList); NODE_SUCC(n); n = (NODE *) NODE_SUCC(n))
	{
		struct Picture *pict;

		pict = CreateNode(PictureList, sizeof(struct Picture), NULL);

		pict->us_PictId = (uword) txtGetKeyAsULONG(1, NODE_NAME(n));
		pict->us_CollId = (uword) txtGetKeyAsULONG(2, NODE_NAME(n));

		pict->us_XOffset = (uword) txtGetKeyAsULONG(3, NODE_NAME(n));
		pict->us_YOffset = (uword) txtGetKeyAsULONG(4, NODE_NAME(n));

		pict->us_Width  = (uword) txtGetKeyAsULONG(5, NODE_NAME(n));
		pict->us_Height = (uword) txtGetKeyAsULONG(6, NODE_NAME(n));

		pict->us_DestX = (uword) txtGetKeyAsULONG(7, NODE_NAME(n));
		pict->us_DestY = (uword) txtGetKeyAsULONG(8, NODE_NAME(n));
	}

	RemoveList(tempList);
}

/********************************************************************
 * Rastports...
 */

static void gfxInitXMSRastPort(struct XMSRastPort *rp, uword us_Width, uword us_Height)
{
	rp->us_Width  = us_Width;
	rp->us_Height = us_Height;
	rp->p_MemHandle = MemAlloc((ulong)rp->us_Width * (ulong)rp->us_Height);
	rp->us_CollId = GFX_NO_COLL_IN_XMS;
}

static void gfxDoneXMSRastPort(struct XMSRastPort *rp)
{
	MemFree(rp->p_MemHandle, (ulong)rp->us_Width * (ulong)rp->us_Height);
}

static void gfxInitRastPort(struct RastPort *rp, uword us_LeftEdge, uword us_TopEdge, uword us_Width, uword us_Height, ubyte uch_ColorStart, ubyte uch_ColorEnd, void *p_BitMap)
{
	rp->us_LeftEdge = us_LeftEdge;
	rp->us_TopEdge  = us_TopEdge;
	rp->us_Width    = us_Width;
	rp->us_Height   = us_Height;
	rp->us_DrawMode = GFX_JAM_1;
	rp->uch_FrontPenAbs   = uch_ColorStart + 1;
	rp->uch_BackPenAbs    = uch_ColorStart;
	rp->uch_OutlinePenAbs = uch_ColorStart + 1;
	rp->uch_ColorStart    = uch_ColorStart;
	rp->uch_ColorEnd      = uch_ColorEnd;
	rp->uch_VideoMode = GFX_VIDEO_MCGA;
	gfxMoveCursor(rp, 0, 0);
	rp->p_BitMap = p_BitMap;
	rp->p_Font   = 0;
}

/*******************************************************************
 * FONTS
 * gfxOpenFont
 * gfxCloseFont
 */

static struct Font *gfxOpenFont(ubyte *puch_FileName, uword us_Width, uword us_Height, ubyte uch_FirstChar, ubyte uch_LastChar, uword us_TotalWidth, uword us_TotalHeight)
{
	char Result[TXT_KEY_LENGTH];
	struct Font *font = MemAlloc(sizeof(struct Font));
	char *p;
	long i, size;

	font->us_Width  = us_Width;
	font->us_Height = us_Height;

	font->uch_FirstChar = uch_FirstChar;
	font->uch_LastChar  = uch_LastChar;

	font->us_TotalWidth  = us_TotalWidth;
	font->us_TotalHeight = us_TotalHeight;

	size = (long) font->us_TotalWidth * font->us_TotalHeight; // MUá NICHT IMMER STIMMEN !!!

	font->p_BitMap = MemAlloc(size);

	dskBuildPathName(PICTURE_DIRECTORY, puch_FileName, Result);
	dskLoad(Result, GFX_LOAD_BUFFER, 0);

	/* Buffer l”schen! */
	memset(font->p_BitMap, 0, size);

	gfxILBMToRAW(GFX_LOAD_BUFFER, font->p_BitMap);

	return font;
}

static void gfxCloseFont(struct Font *font)
{
	if (font)
	{
		if (font->p_BitMap)
		{
			MemFree(font->p_BitMap, font->us_TotalWidth * font->us_TotalHeight);
			font->p_BitMap = 0;
		}

		MemFree(font, sizeof(struct Font));
		font = NULL;
	}
}

/*******************************************************************
 * gfxMoveCursor (amiga function: Move)
 * gfxSetPens    (amiga defines:   SetAPen, SetBPen, SetOPen)
 * gfxRectFill   (amiga function: RectFill)
 * gfxTextLength (amiga function: TextLength)
 * gfxSetDrMd    (amiga define:   SetDrMd)
 * gfxDraw	     (amiga function: Draw)
 * gfxReadPixel  (amiga function: ReadPixel)
 */

void gfxDraw(struct RastPort *rp, uword us_X, uword us_Y)
{
	uword dex, dey, sx, sy, i;
	ubyte col;

	if (( us_X < rp->us_Width ) && ( us_Y < rp->us_Height ))
	{
		col  = rp->uch_FrontPenAbs;

		sx = min((int)us_X, (int)rp->us_CursorXPosAbs);
		sy = min((int)us_Y, (int)rp->us_CursorYPosAbs);

		dex = abs((int)rp->us_CursorXPosAbs - (int)us_X) + 1;
		dey = abs((int)rp->us_CursorYPosAbs - (int)us_Y) + 1;

		if (dey == 1)
		{
			for (i = sx; i < dex; i++) ((ubyte*)rp->p_BitMap)[i] = col;
		}
		else if (dex == 1)
		{
			for (i = sy; i < dey; i++) ((ubyte*)rp->p_BitMap)[i*320+sx] = col;
		}

		rp->us_CursorXPosAbs = us_X;
		rp->us_CursorYPosAbs = us_Y;
	}

	gfxUpdateSDL(rp);
}

ubyte gfxReadPixel(struct RastPort *rp, uword us_X, uword us_Y)
{
	ubyte col;

	if (rp->uch_VideoMode == GFX_VIDEO_MCGA)
		col = gfxMCGAReadPixel(rp, us_X, us_Y);

	if (rp->uch_VideoMode == GFX_VIDEO_NCH4)
		col = gfxNCH4ReadPixel(rp, us_X, us_Y);

	return col;
}

void gfxMoveCursor(struct RastPort *rp, uword us_X, uword us_Y)
{
	rp->us_CursorXPosAbs = us_X;
	rp->us_CursorYPosAbs = us_Y;
}

void gfxSetPens(struct RastPort *rp, ubyte uch_Front, ubyte uch_Back, ubyte uch_Out)
{
	if (uch_Front != GFX_SAME_PEN)
		rp->uch_FrontPenAbs   = uch_Front;

	if (uch_Back != GFX_SAME_PEN)
		rp->uch_BackPenAbs    = uch_Back;

	if (uch_Out != GFX_SAME_PEN)
		rp->uch_OutlinePenAbs = uch_Out;
}

// Minimalgroesse: 3 * 3 Pixel !
void gfxRectFill(struct RastPort *rp, uword us_SX, uword us_SY, uword us_EX, uword us_EY)
{
	if (rp->uch_VideoMode == GFX_VIDEO_MCGA)
		gfxMCGARectFill(rp, us_SX, us_SY, us_EX, us_EY);

	if (rp->uch_VideoMode == GFX_VIDEO_NCH4)
		gfxNCH4RectFill(rp, us_SX, us_SY, us_EX, us_EY);

	gfxUpdateSDL(rp);
}

void gfxSetDrMd(struct RastPort *rp, uword us_DrawMode)
{
	rp->us_DrawMode = us_DrawMode;
}

void gfxSetFont(struct RastPort *rp, struct Font *font)
{
	rp->p_Font = font;
}

/* berechnet die L„nge eines Textes in Pixel */
long gfxTextLength(struct RastPort *rp, ubyte *puch_Text, uword us_CharCount)
{
	long length = (long) strlen(puch_Text);
	return (length * (rp->p_Font->us_Width));
}

/*******************************************************************
 * access & calc functions
 */

struct Collection *gfxGetCollection(uword us_CollId)
{
	return GetNthNode(CollectionList, (ulong) (us_CollId - 1));
}

struct Picture *gfxGetPicture(uword us_PictId)
{
	return GetNthNode(PictureList, (ulong) (us_PictId - 1));
}

// die Kollektion muá einen Schritt davor hergerichtet (PrepareColl)
// worden sein
void gfxGetColorTable(uword us_CollId, ubyte *puch_ColorTable)
{
	long i;
	ubyte *p = (ubyte *) ((ulong) GFX_DECR_BUFFER + (ulong) GFX_CMAP_OFFSET);

	for (i = 0; i < GFX_COLORTABLE_SIZE; i++)
		puch_ColorTable[i] = p[i];
}

static long gfxGetRealDestY(struct RastPort *rp, long destY)
{
	if (rp->uch_VideoMode == GFX_VIDEO_NCH4)
	{
		// dieser RastPort vereingt im NCH4-mode sowohl den Upper als auch
		// den "normalen" Menurp
		// beim MenuRp muá die TopEdge abgezogen werden, beim UpperRp darf
		// dies nicht geschehen

		if (destY >= m_wrp->us_TopEdge)
			destY -= m_wrp->us_TopEdge;
	}

	if (rp->uch_VideoMode == GFX_VIDEO_MCGA)
		destY -= rp->us_TopEdge;

	return destY;
}

/*******************************************************************
 * collections...
 */

/* nach dieser Funktion befindet sich im PrepareRP die entpackte Collection */
struct RastPort *gfxPrepareColl(uword us_CollId)
{
	struct Collection *coll = gfxGetCollection(us_CollId);

	if (!coll->p_Prepared)
	{
		char Result[TXT_KEY_LENGTH];
		void *cmap = (void *) ((ulong) GFX_DECR_BUFFER + (ulong) GFX_CMAP_OFFSET);

		/* Dateiname erstellen */
		dskBuildPathName(PICTURE_DIRECTORY, coll->puch_Filename, Result);

		#ifdef THECLOU_PROFIDISK
		#ifdef THECLOU_CDROM_VERSION
		if (coll->fromDisk)
			sprintf(Result, "%s\\%s", PICTURE_DIRECTORY, coll->puch_Filename);
		#endif
		#endif

		/* Collection laden */
		dskLoad(Result, GFX_LOAD_BUFFER, DSK_DUMMY_DISK_ID);

		tcClearStdBuffer(GFX_DECR_BUFFER);
		/* Collection entpacken */
		/* der Decr Buffer entspricht dem PrepareRP */
		gfxILBMToRAW(GFX_LOAD_BUFFER, GFX_DECR_BUFFER);

		gfxGetCMAP(GFX_LOAD_BUFFER, cmap);

		/* coll->p_Prepared wird nicht mit dem PrepareRP initialisert, da */
		/* es sonst zu Inkonsistenzen kommen k”nnte. Collections im PrepareRP */
		/* werden als nicht vorbereitet betrachtet, da der PrepareRP st„ndig */
		/* durch andere Bilder berschrieben wird */
		coll->p_Prepared = 0;
	}
	else
	{
		/* vom aktuellen RP(muá XMS sein) in den PrepareRP kopieren */
		/* coll->p_Prepared darf nicht initialisert werden! */
		/* siehe oben! */
		memcpy(PrepareRP.p_BitMap, coll->p_Prepared->p_MemHandle, coll->p_Prepared->us_Width * coll->p_Prepared->us_Height);
	}

	return &PrepareRP;
}

void gfxUnPrepareColl(uword us_CollId)
{
	// dummy function
}

void gfxCopyCollFromXMS(uword us_CollId, void *p_Buffer)
{
	struct Collection *coll = gfxGetCollection(us_CollId);

	if (coll->p_Prepared)
		memcpy(p_Buffer, coll->p_Prepared->p_MemHandle, coll->p_Prepared->us_Width * coll->p_Prepared->us_Height);
}

void gfxCopyCollToXMS(uword us_CollId, struct XMSRastPort *rp)
{
	struct Collection *coll = gfxGetCollection(us_CollId);
	long size = rp->us_Width * rp->us_Height;

	// wenn sich in diesem XMS RastPort ein anderes Bild befindet so wird dieses
	// nun aus dem XMS RastPort "entfernt"
	if ((rp->us_CollId != GFX_NO_COLL_IN_XMS) && (us_CollId != rp->us_CollId))
	{
		struct Collection *oldColl = gfxGetCollection(rp->us_CollId);
		oldColl->p_Prepared = 0;
	}

	gfxPrepareColl(us_CollId);

	memcpy(rp->p_MemHandle, GFX_DECR_BUFFER, size);

	// in der neuen Collection wird der XMS RastPort eingetragen
	coll->p_Prepared = rp;

	rp->us_CollId = us_CollId;
}

/*******************************************************************
 * print functions
 */

void gfxSetRect(uword us_X, uword us_Width)
{
	GlobalPrintRect.us_X     = us_X;
	GlobalPrintRect.us_Width = us_Width;
}

void gfxPrintExact(struct RastPort *rp, ubyte *puch_Text, uword us_X, uword us_Y)
{
	if (rp->uch_VideoMode == GFX_VIDEO_MCGA)
		gfxMCGAPrintExact(rp, puch_Text, us_X, us_Y);

	if (rp->uch_VideoMode == GFX_VIDEO_NCH4)
		gfxNCH4PrintExact(rp, puch_Text, us_X, us_Y);

	gfxUpdateSDL(rp);
}

void gfxPrint(struct RastPort *rp, ubyte *puch_Text, uword us_Y, ulong ul_Mode)
{
	uword x = GlobalPrintRect.us_X;
	uword y = us_Y;
	uword l = gfxTextLength(rp, puch_Text, strlen(puch_Text));

	if (ul_Mode & GFX_PRINT_RIGHT)
		x += GlobalPrintRect.us_Width - l;

	if (ul_Mode & GFX_PRINT_CENTER)
	{
		if (l <= GlobalPrintRect.us_Width)
			x += (GlobalPrintRect.us_Width - l) / 2;
	}

	if (ul_Mode & GFX_PRINT_SHADOW)
	{
		ubyte APen = rp->uch_FrontPenAbs;
		rp->uch_FrontPenAbs = rp->uch_BackPenAbs;
		gfxPrintExact(rp, puch_Text, x + 1, y + 1);
		rp->uch_FrontPenAbs = APen;
	}

	gfxPrintExact(rp, puch_Text, x, y);
}

/*******************************************************************
 * refresh...
 */

/* kopiert aktuelles Bild in den RefreshRPInXMS */
void gfxPrepareRefresh(void)
{
	ubyte *mem = (ubyte*)RefreshRPInXMS.p_MemHandle;

	switch (GfxBase.uch_VideoMode)
	{
		case GFX_VIDEO_MCGA:
			memcpy(mem, GfxBoardBase, 320*200);
		break;
		case GFX_VIDEO_NCH4:
			gfxNCH4PutNCH4ToMCGA(GfxBoardBase + gfxNCH4GetCurrScrollOffset(), mem, 0, 0, 0, 0, 320, 128, 160, 320);
			gfxNCH4PutNCH4ToMCGA(GfxBoardBase, mem+(320*128), 0, 0, 0, 0, 320, 72, 160, 320);
		break;
	}
}

/* kopiert aktuellen Inhalt des RefreshRPInXMS in den Bildschirmspeicher */
void gfxRefresh(void)
{
	ubyte *mem = (ubyte*)RefreshRPInXMS.p_MemHandle;

	switch (GfxBase.uch_VideoMode)
	{
		case GFX_VIDEO_NCH4:
			gfxNCH4PutMCGAToNCH4(mem, GfxBoardBase + gfxNCH4GetCurrScrollOffset(), 0, 0, 0, 0, 320, 128, 320, 160);
			gfxNCH4PutMCGAToNCH4(mem+(320*128), GfxBoardBase, 0, 0, 0, 0, 320, 72, 320, 160);
		break;
		case GFX_VIDEO_MCGA:
		default:
			memcpy(GfxBoardBase, mem, 320*200);
		break;
	}

	gfxUpdateSDL(NULL);
}

/*******************************************************************
 * presentation
 */

void gfxBlit(struct RastPort *srp, uword us_SourceX, uword us_SourceY,
				 struct RastPort *drp, uword us_DestX, uword us_DestY,
				 uword us_Width, uword us_Height, ulong ul_BlitMode)
{
	gfxWaitTOF();

	if ((srp->uch_VideoMode == GFX_VIDEO_MCGA) && (drp->uch_VideoMode == GFX_VIDEO_MCGA))
	{
		gfxMCGABlit(srp, us_SourceX, us_SourceY, drp, us_DestX, us_DestY,
				us_Width, us_Height, ul_BlitMode & (GFX_ONE_STEP|GFX_OVERLAY));
	}

	if ((srp->uch_VideoMode == GFX_VIDEO_MCGA) && (drp->uch_VideoMode == GFX_VIDEO_NCH4))
	{
		if (ul_BlitMode & GFX_ONE_STEP)
			gfxNCH4PutMCGAToNCH4(srp->p_BitMap, drp->p_BitMap, us_SourceX, us_SourceY,
				us_DestX, us_DestY,us_Width, us_Height, 320, 160);

		if (ul_BlitMode & GFX_OVERLAY)
			gfxNCH4OLMCGAToNCH4(srp->p_BitMap, drp->p_BitMap, us_SourceX, us_SourceY,
				us_DestX, us_DestY,us_Width, us_Height, 320, 160);
	}

	gfxUpdateSDL(drp);
}

struct RastPort *gfxGetDestRP(long l_DestX, long l_DestY)
{
	struct RastPort *rp = NULL;

	if (GfxBase.uch_VideoMode == GFX_VIDEO_MCGA)
	{
		rp = u_wrp;

		if (l_DestY >= rp->us_TopEdge + rp->us_Height)
		{
			rp = l_wrp;

			if (l_DestY >= rp->us_TopEdge + rp->us_Height)
				rp = m_wrp;
		}
	}


	if (GfxBase.uch_VideoMode == GFX_VIDEO_NCH4)
	{
		rp = l_wrp;

		//if ((l_DestY < 60) || (l_DestY >= 140))
		//	rp = m_wrp;

		if (l_DestY < 60)
			rp = u_wrp;

		if (l_DestY >= 140)
			rp = m_wrp;
	}

	return rp;
}

void wfr()
// fr Bildschirmsync verwenden!!!
{
	gfxUpdateSDL(NULL);
}

void wfd(void)
// sperrt im Gegensatz zu wfr die Interrupts nicht
// wenn die Musik l„uft kann es daher vorkommen, daá
// dieses wfd einmal aussetzt
// !!! NICHT fr Bildschirmsync verwenden!!
{
	wfr();
}

void gfxClearArea(struct RastPort *rp)
{
	ubyte uch_APen, uch_BPen, uch_OPen;

	uch_APen = rp->uch_FrontPenAbs;
	uch_BPen = rp->uch_BackPenAbs;
	uch_OPen = rp->uch_OutlinePenAbs;

	gfxSetPens(rp, 0, 0, 0);
	gfxRectFill(rp, 0, 0, rp->us_Width - 1, rp->us_Height - 1);
	gfxSetPens(rp, uch_APen, uch_BPen, uch_OPen);
}

void gfxSetRGB(struct RastPort *rp, ubyte uch_ColNr, ubyte uch_Red, ubyte uch_Green, ubyte uch_Blue)
{
	SDL_Color color;
	color.r = uch_Red;
	color.g = uch_Green;
	color.b = uch_Blue;
	SDL_SetColors(SurfaceScreen, &color, uch_ColNr, 1);
}

void gfxSetPalette(struct RastPort *rp, ubyte uch_ColStart, ubyte uch_ColEnd, ubyte *p_ColorTable)
{
	SDL_Color colors[256];
	uword us_NumColors = (uword)(uch_ColEnd - uch_ColStart) + 1;
	for (uword i = 0; i < us_NumColors; i++)
	{
		colors[i].r = p_ColorTable[((uword)uch_ColStart+i)*3  ];
		colors[i].g = p_ColorTable[((uword)uch_ColStart+i)*3+1];
		colors[i].b = p_ColorTable[((uword)uch_ColStart+i)*3+2];
	}
	SDL_SetColors(SurfaceScreen, colors, uch_ColStart, us_NumColors);
}

void gfxSetColorRange(ubyte uch_ColorStart, ubyte uch_ColorEnd)
{
	GlobalColorRange.uch_Start = uch_ColorStart;
	GlobalColorRange.uch_End   = uch_ColorEnd;
}

void gfxGetColorTableFromReg(char *puch_Colortable)
{
	SDL_LockSurface(SurfaceScreen);

	SDL_Palette *palette;
	palette = SurfaceScreen->format->palette;
	if (palette)
	{
		SDL_Color *colors;
		colors = palette->colors;

		ubyte *table = (ubyte*)puch_Colortable;

		for (uword t = 256; t--;)
		{
			*table++ = colors->r;
			*table++ = colors->g;
			*table++ = colors->b;
			colors++;
		}
	}

	SDL_UnlockSurface(SurfaceScreen);
}

#define interpol(i,f,d)	(ubyte)((float)(i) + ((float)(f) - (float)(i)) * (float)(d))

void gfxChangeColors(struct RastPort *rp, long l_Delay, ulong ul_Mode, char *colorTable)
{
	ubyte cols[768];
	ubyte cols2[768];
	uword st,en,st3,en3;
	uword i;
	ubyte r,g,b;
	float e,f;

	if (rp)
	{
		st = rp->uch_ColorStart;
		en = rp->uch_ColorEnd;
	}
	else
	{
		st = GlobalColorRange.uch_Start;
		en = GlobalColorRange.uch_End;
	}

	st3 = st * 3;
	en3 = en * 3;

	l_Delay = max(l_Delay, 1);	// l_Delay may not be zero!

	f = 0.0f;
	e = 1.0f / (float)(l_Delay+1);

	switch (ul_Mode)
	{
		case GFX_FADE_OUT:		// fade to 0,64,48
			gfxGetColorTableFromReg(cols);
			do
			{
				for (i = st3; i <= en3; i += 3)
				{
					cols2[i  ] = interpol(cols[i  ],  0, f);
					cols2[i+1] = interpol(cols[i+1], 64, f);
					cols2[i+2] = interpol(cols[i+2], 48, f);
				}
				f += e;
				gfxSetPalette(rp, st, en, cols2);
				gfxUpdateSDL(rp);
				SDL_Delay(10);
			} while (--l_Delay);
			for (i = st3; i <= en3; i += 3)
			{
				cols2[i  ] = 0;
				cols2[i+1] = 64;
				cols2[i+2] = 48;
			}
			gfxSetPalette(rp, st, en, cols2);
			gfxUpdateSDL(rp);
		break;
		case GFX_BLEND_UP:		// fade to colorTable
			do
			{
				for (i = st3; i <= en3; i += 3)
				{
					cols[i  ] = interpol( 0, ((ubyte*)colorTable)[i  ], f);
					cols[i+1] = interpol(64, ((ubyte*)colorTable)[i+1], f);
					cols[i+2] = interpol(48, ((ubyte*)colorTable)[i+2], f);
				}
				f += e;
				gfxSetPalette(rp, st, en, cols);
				gfxUpdateSDL(rp);
				SDL_Delay(10);
			} while (--l_Delay);
			gfxSetPalette(rp, st, en, (ubyte*)colorTable);
			gfxUpdateSDL(rp);
		break;
	}
}

void gfxShow(uword us_PictId, ulong ul_Mode, long l_Delay, long l_XPos, long l_YPos)
{
	struct Picture    *pict = gfxGetPicture(us_PictId);
	struct Collection *coll = gfxGetCollection(pict->us_CollId);
	struct RastPort   *destRP;
	char   *colorTable = (char *) PrepareRP.p_BitMap + GFX_CMAP_OFFSET;
	long   destX = pict->us_DestX;
	long   destY = pict->us_DestY;

	if (l_XPos != -1)
		destX = l_XPos;

	if (l_YPos != -1)
		destY = l_YPos;

	if (GfxBase.p_DestRPForShow)
		destRP = GfxBase.p_DestRPForShow;
	else
		destRP = gfxGetDestRP(destX, destY);

	destY = gfxGetRealDestY(destRP, destY);

	gfxPrepareColl(pict->us_CollId);

	if (!(ul_Mode & GFX_NO_REFRESH))
		gfxPrepareRefresh();

	if (ul_Mode & GFX_CLEAR_FIRST)
		gfxClearArea(destRP);

	if (ul_Mode & GFX_FADE_OUT)
	{
		gfxSetColorRange(coll->uch_ColorRangeStart, coll->uch_ColorRangeEnd);
		gfxChangeColors(NULL, l_Delay, GFX_FADE_OUT, 0);
	}

	if (!l_Delay)
	{
		if (ul_Mode & GFX_BLEND_UP)
		{
			gfxSetColorRange(coll->uch_ColorRangeStart, coll->uch_ColorRangeEnd);
			gfxChangeColors(NULL , l_Delay, GFX_BLEND_UP, colorTable);
		}
	}

	if (ul_Mode & GFX_OVERLAY)
		gfxBlit(&PrepareRP, pict->us_XOffset, pict->us_YOffset, destRP,
				  destX, destY, pict->us_Width, pict->us_Height, ul_Mode);

	if (ul_Mode & GFX_ONE_STEP)
		gfxBlit(&PrepareRP, pict->us_XOffset, pict->us_YOffset, destRP,
				  destX, destY, pict->us_Width, pict->us_Height, ul_Mode);

	if (l_Delay)
	{
		if (ul_Mode & GFX_BLEND_UP)
		{
			gfxSetColorRange(coll->uch_ColorRangeStart, coll->uch_ColorRangeEnd);
			gfxChangeColors(NULL , l_Delay, GFX_BLEND_UP, colorTable);
		}
	}

	gfxSetDestRPForShow(NULL);
}

/*******************************************************************
 * misc
 */


long gfxGetILBMSize(struct Collection *coll)
{
	uword w = coll->us_TotalWidth;
	uword h = coll->us_TotalHeight;
	long size;

	w = ((w + 15) / 16) * 16;	// aufs Word aufrunden

	size = w * h;

	return size;
}

static void gfxGetCMAP(ubyte *p_Data, ubyte *p_Dest)
{
	long i;

	// CMAP Chunk suchen
	while ((p_Data[0] != 'C') || (p_Data[1] != 'M') || (p_Data[2] != 'A') || (p_Data[3] != 'P')) p_Data++;

	p_Data += 4;	// CMAP Chunk ueberspringen
	p_Data += 4;	// Size of CMAP Chunk ueberspringen

	for (i = 0; i < GFX_COLORTABLE_SIZE; i++)
	{
		p_Dest[i] = p_Data[i];
	}
}

static void MakeMCGA(uword b, ubyte *pic, uword PlSt, short c)
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

void gfxILBMToRAW(ubyte *p_Source, ubyte *p_Dest)
{
	uword IFFplanes = 0, IFFwidth = 0, IFFheight = 0;
	ubyte *fp, *pic, *pic1;
	int	s, t, b, x;
	uword a, y;
	uword flag;

	pic = p_Dest;
	fp = p_Source;

	fp += 8;

	if ((fp[0] != 'I') || (fp[1] != 'L') || (fp[2] != 'B') || (fp[3] != 'M')) return;

	fp += 12;

	IFFwidth  = (uword)fp[1] + ((uword)fp[0] << 8);
	IFFheight = ((uword)fp[2] << 8) + (uword)fp[3];
	IFFplanes = (uword)fp[8];
	flag = (uword)fp[10];

	if (((long)IFFwidth * (long)IFFheight) > GFX_DECR_BUFFER_SIZE) IFFheight = 192;

	while ((fp[0] != 'B') || (fp[1] != 'O') || (fp[2] != 'D') || (fp[3] != 'Y')) fp++;

	fp += 8;

	if (flag)
	{
		pic1 = pic;
		for (t = 1; t <= IFFheight; t++)
		{
			for (s = 1; s <= IFFplanes; s++)
			{
				pic = pic1;			/* Anfang der aktuellen Zeile */
				b = ((IFFwidth + 15) & 0xfff0);
				do
				{
					a = *fp;		/* Kommando (wiederholen oder uebernehmen */
					fp++;			/* naechstes Zeichen */
					if (a > 128)	/* Zeichen wiederholen */
					{
						a = 257 - a;
						y = *fp;
						fp++;
						for (x = 1; x <= a; x++)
						{
							MakeMCGA(y, pic, (1 << (s - 1)), b);
							pic += 8;
							b -= 8;
						}
					}
					else			/* Zeichen uebernehmen */
					{
						for (x = 0; x <= a; x++)
						{
							y = *fp;
							fp++;
							MakeMCGA(y, pic, (1 << (s - 1)), b);
							pic += 8;
							b -= 8;
						}
					}
				}
				while (b > 0);
			}
			pic1 += 320;
		}
	}
	else
	{
		pic1 = pic;
		for (t = 1; t <= IFFheight; t++)
		{
			for (s = 1; s <= IFFplanes; s++)
			{
				pic = pic1;			/* Anfang der aktuellen Zeile */
				b = ((IFFwidth + 15) >> 4) << 4;
				do
				{
					y = *fp;
					fp++;
					MakeMCGA(y, pic, (1 << (s - 1)),b);
					pic += 8;
					b -= 8;
				}
				while (b > 0);
			}
			pic1 += 320;
		}
	}
}
