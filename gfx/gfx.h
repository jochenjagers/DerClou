/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_GFX
#define MODULE_GFX

#include "theclou.h"

#ifndef MODULE_LIST
#include "list\list.h"
#endif

extern int gfxScalingOffsetX;
extern int gfxScalingOffsetY;
extern int gfxScalingFactor;

extern void gfxScreenshot(void);
extern ubyte *gfxGetGfxBoardBase(void);
extern void gfxInvalidate(void);
extern void gfxSetDarkness(ubyte value);

#define GFX_NO_MEM_HANDLE	((uword) -1)

/* defines fÅr gfxPrint */
#define GFX_PRINT_CENTER		1<<0
#define GFX_PRINT_RIGHT       1<<1
#define GFX_PRINT_LEFT        1<<2

#define GFX_PRINT_SHADOW		1<<3

/* defines fÅr SetDrMd */
#define GFX_JAM_1				0	/* Hintergrund wird gleichbelassen */
#define GFX_JAM_2				1	/* Hintergrund wird mit BackPen Åbermalt */

/* defines fÅr Show */
#define GFX_NO_REFRESH				   1
#define GFX_BLEND_UP					   2
#define GFX_FADE_OUT					   4
#define GFX_CLEAR_FIRST			    	8
#define GFX_OVERLAY       		     16
#define GFX_ONE_STEP		  		     32	/* "plain" blit */
#define GFX_DONT_SHOW_FIRST_PIC    64

/* defines fÅr gfxSetPens */
#define GFX_SAME_PEN						((ubyte)255)

#define GFX_NO_COLL_IN_XMS				((uword) -1)

#define GFX_COLORTABLE_SIZE			768	/* 256 Farben * 3 Bytes */
#define GFX_SIZE_OF_COLLECTION(c) 	(gfxGetILBMSize(c) + GFX_COLORTABLE_SIZE)

#define GFX_VIDEO_MCGA					1
#define GFX_VIDEO_NCH4					2
#define GFX_VIDEO_TEXT					3

struct XMSRastPort
{
	uword us_Width;
	uword	us_Height;
	void* p_MemHandle;
	uword	us_CollId;	/* Collection, die sich gerade hier befindet! */
};

struct RastPort	/* change rp definitions in gfx.ph */
{
	uword	us_LeftEdge;
	uword	us_TopEdge;
	uword	us_Width;
	uword	us_Height;
	uword us_DrawMode;
	ubyte	uch_VideoMode;
	ubyte uch_FrontPenAbs;	 /* entspricht dem Farbregister */
	ubyte uch_BackPenAbs;	 /* ebenfalls absolut und nicht relativ zum */
	ubyte uch_OutlinePenAbs; /* Registerstart */
	uword	us_CursorXPosAbs;
	uword us_CursorYPosAbs;
	ubyte	uch_ColorStart;
	ubyte	uch_ColorEnd;
	void	      *p_BitMap;
	struct Font *p_Font;
};

struct Collection
{
	NODE Link;

	uword	us_CollId;
	ubyte uch_ColorRangeStart;
	ubyte uch_ColorRangeEnd;

	ubyte	*puch_Filename;

	struct XMSRastPort *p_Prepared;	/* falls Collection im Speicher -> != 0*/

	void	*p_ColorTable;		// stimmt nur eingeschrÑnkt!!!! (nur solange der Buffer
									// durch nichts anderes verÑndert wurde!

	uword	us_TotalWidth;
	uword	us_TotalHeight;

	ubyte fromDisk;
};

struct Picture
{
	NODE  Link;

	uword us_PictId;
	uword us_CollId;	/* in welcher Collection sich dieses Bild befindet */

	uword us_XOffset;	/* innerhalb der Collection */
	uword us_YOffset;

	uword us_Width;
	uword us_Height;

	uword us_DestX;
	uword us_DestY;
};

struct Font
{
	void *p_BitMap;

	uword us_Width;      	/* of one character */
	uword us_Height;			/* of one character */

	uword us_TotalWidth;
	uword us_TotalHeight;

	ubyte uch_FirstChar;
	ubyte uch_LastChar;
};

extern ubyte gfxPaletteGlobal[256*4];

extern struct RastPort *l_wrp;
extern struct RastPort *u_wrp;
extern struct RastPort *m_wrp;

extern struct RastPort PrepareRP;
extern struct RastPort RefreshRP;

extern struct XMSRastPort StdRP0InXMS;
extern struct XMSRastPort StdRP1InXMS;
extern struct XMSRastPort AnimRPInXMS;
extern struct XMSRastPort RefreshRPInXMS;
extern struct XMSRastPort AddRPInXMS;
extern struct XMSRastPort LSFloorRPInXMS;
extern struct XMSRastPort LSObjectRPInXMS;

extern struct Font *bubbleFont;
extern struct Font *menuFont;

extern void gfxUpdateSDL(struct RastPort *rp);
extern const char *gfxPointerName(void *ptr);

extern void gfxInit(void);
extern void gfxDone(void);

extern void gfxSetVideoMode(ubyte uch_NewMode);

extern void wfd(void);
extern void wfr(void);

extern long gfxGetILBMSize(struct Collection *coll);

extern void gfxCorrectUpperRPBitmap(void);

extern void gfxSetRGB(struct RastPort *rp, ubyte uch_ColNr,ubyte uch_Red,ubyte uch_Green,ubyte uch_Blue);
extern void gfxSetPalette24(struct RastPort *rp, ubyte uch_ColStart, ubyte uch_ColEnd, ubyte *p_ColorTable24);
extern void gfxSetPalette32(struct RastPort *rp, ubyte uch_ColStart, ubyte uch_ColEnd, ubyte *p_ColorTable32);

extern void gfxBlit(struct RastPort *srp, uword us_SourceX, uword us_SourceY,
				 struct RastPort *drp, uword us_DestX, uword us_DestY,
				 uword us_Width, uword us_Height, ulong ul_BlitMode);

extern void gfxMoveCursor(struct RastPort *rp, uword us_X, uword us_Y);
extern void gfxSetPens(struct RastPort *rp, ubyte uch_Front, ubyte uch_Back, ubyte uch_Out);
extern void gfxRectFill(struct RastPort *rp, uword us_SX, uword us_SY, uword us_EX, uword us_EY);
extern void gfxDraw(struct RastPort *rp, uword us_X, uword us_Y);
extern void gfxSetDrMd(struct RastPort *rp, uword us_DrawMode);
extern void gfxSetFont(struct RastPort *rp, struct Font *font);

extern void gfxPrepareRefresh(void);
extern void gfxRefresh(void);

extern void gfxClearArea(struct RastPort *rp);
extern void gfxSetRect(uword us_X, uword us_Width);

extern long gfxTextLength(struct RastPort *rp, char *puch_Text, uword us_CharCount);
extern ubyte gfxReadPixel(struct RastPort *rp, uword us_X, uword us_Y);

extern void gfxPrint(struct RastPort *rp, char *puch_Text, uword us_Y, ulong ul_Mode);
extern void gfxPrintExact(struct RastPort *rp, char *puch_Text, uword us_X, uword us_Y);

extern void gfxSetColorRange(ubyte uch_ColorStart, ubyte uch_ColorEnd);

extern void gfxChangeColors(struct RastPort *rp, long l_Delay, ulong ul_Mode, ubyte *colorTable);
extern void gfxShow(uword us_PictId, ulong ul_Mode, long l_Delay, long l_XPos, long l_YPos);

extern void gfxSetDestRPForShow(struct RastPort *rp);

extern void gfxCopyCollToXMS(uword us_CollId, struct XMSRastPort *rp);
extern void gfxCopyCollFromXMS(uword us_CollId, void *p_Buffer);

extern void gfxGetColorTableFromReg(ubyte *puch_Colortable);
extern void gfxGetColorTable(uword us_CollId, ubyte *puch_ColorTable);

extern struct RastPort *gfxPrepareColl(uword us_CollId);
extern void gfxUnPrepareColl(uword us_CollId);

extern struct Collection *gfxGetCollection(uword us_CollId);
extern struct Picture *gfxGetPicture(uword us_PictId);

extern void gfxILBMToRAW(ubyte *p_Source, ubyte *p_Dest);

/* gfxmcga.h */
#ifndef MODULE_GFXMCGA
#define MODULE_GFXMCGA

#ifndef MODULE_INPHDL
#include "inphdl\inphdl.h"
#endif

extern void gfxMCGAPrintExact(struct RastPort *rp, char *puch_Text, uword us_X, uword us_Y);
extern void gfxMCGARectFill(struct RastPort *rp, uword us_SX, uword us_SY, uword us_EX, uword us_EY);
extern ubyte gfxMCGAReadPixel(struct RastPort *rp, uword us_X, uword us_Y);

extern void gfxMCGABlit(struct RastPort *srp, uword us_SourceX, uword us_SourceY,
				 struct RastPort *drp, uword us_DestX, uword us_DestY,
				 uword us_Width, uword us_Height, ulong ul_BlitMode);
#endif

/* gfxnch4.h */
#ifndef MODULE_GFXNCH4
#define MODULE_GFXNCH4

#define GFX_NCH4_SCROLLOFFSET 	(640 * 72)  // Speicher , den die fixe Anzeige belegt

extern ulong gfxNCH4GetCurrScrollOffset(void);

extern void  gfxNCH4SetViewPort(uword x, uword y);
extern void  gfxNCH4Scroll(int x,int y);
extern void  gfxNCH4SetSplit(uword line);
extern void  gfxNCH4Init(void);
extern void  gfxNCH4RectFill(struct RastPort *rp, uword x1, uword y1, uword x2, uword y2);
extern void  gfxNCH4PrintExact(struct RastPort *rp, char *puch_Text, uword us_X, uword us_Y);
extern ubyte gfxNCH4ReadPixel(struct RastPort *rp, uword x, uword y);

extern void gfxNCH4OLMCGAToNCH4Mask(void *sp, void *dp, uword x1, uword y1, uword x2, uword y2, uword w, uword h, uword sw, uword dw);
extern void gfxNCH4PutMCGAToNCH4Mask(void *sp, void *dp, uword x1, uword y1, uword x2, uword y2, uword w, uword h, uword sw, uword dw);
extern void gfxNCH4PutMCGAToNCH4(void *sp, void *dp, uword x1, uword y1, uword x2, uword y2, uword w, uword h, uword sw, uword dw);
extern void gfxNCH4OLMCGAToNCH4(void *sp, void *dp, uword x1, uword y1, uword x2, uword y2, uword w, uword h, uword sw, uword dw);
extern void gfxNCH4OrMCGAToNCH4(void *sp,void *dp, uword x1, uword y1, uword x2, uword y2, uword w, uword h, uword sw, uword dw);
extern void gfxNCH4AndMCGAToNCH4(void *sp,void *dp, uword x1, uword y1, uword x2, uword y2, uword w, uword h, uword sw, uword dw);
extern void gfxNCH4PutNCH4ToMCGA(void *sp,void *dp, uword x1, uword y1, uword x2, uword y2, uword w, uword h, uword sw, uword dw);

#endif

#endif
