/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "gfx/gfx.h"

#include <math.h>
#include <stdint.h>

#include "SDL.h"
#include "base/base.h"
#include "gfx/filter.h"
#include "memory/memory.h"
#include "port/port.h"
#include "text/text.h"

/************************************************/

/************************************************/

enum
{
    GFX_NCH4_BUFFER_SIZE = (320 * 200) /* 320*200 = 64000 Bytes gross */
};

#define GFX_BUBBLE_FONT_NAME ((char *)"bubble.fnt")
#define GFX_MENU_FONT_NAME ((char *)"menu.fnt")

enum
{
    GFX_CMAP_OFFSET = 61440  // -> Maximalgroesse 320 * 192
};

struct PrintRect
{
    uword us_X;
    uword us_Width;
};

struct ColorRange
{
    ubyte uch_Start;
    ubyte uch_End;
};

struct GfxBase
{
    struct RastPort *p_DestRPForShow;

    ubyte uch_VideoMode;
};

struct RastPort LowerRP;
struct RastPort MenuRP;

struct RastPort NCH4MenuRP;
struct RastPort NCH4ScrollRP;
struct RastPort NCH4UpperRP;

struct RastPort RefreshRP;
/* wird verwendet für Double Buffering während der Vorbereitung */
struct RastPort PrepareRP;
/* Da man von XMS nur ganze Blöcke kopieren kann (ohne Modulo)    */
/* müssen einige Grafiken, bevor sie ins Video RAM kopiert werden */
/* hier an dieser Stelle gepuffert werden. */
/* ist als extern definfiert, da Animmodul darauf zugreifen muß */

struct XMSRastPort StdRP0InXMS;
/* wird benutzt für Objekte, die immer im Speicher sind */
struct XMSRastPort StdRP1InXMS;
/* wird benutzt für Objekte, die immer im Speicher sind */
struct XMSRastPort RefreshRPInXMS;
/* Bild, das im Moment gerade gezeigt wird (für Refresh) */
struct XMSRastPort AnimRPInXMS;
/* beinhaltet die Animphasen des gerade aktuellen Bildes */
/* um Inkonsistenzen zu vermeiden, wird jedesmal, bevor eine */
/* Animphase gezeigt wird, das gesamte Bild in den PrepareRp */
/* kopiert, der benötigte Ausschnitt wird dann von dort */
/* ins Video-RAM kopiert */
/* ist als extern definfiert, da Animmodul darauf zugreifen muß */
struct XMSRastPort AddRPInXMS;
/* in diesem Rastport befinden sich diverse zusätzliche Grafiken */
/* die in das Bild hinzugefügt werden (Gesichter, Werkzeuge)     */
/* je nach Situation... */
/* dieser RP wird je nach Situation ausgewechselt					  */
struct XMSRastPort LSFloorRPInXMS;
/* in diesem RastPort befindet sich während der Planungs-Einbruchs */
/* phasen das Bild der Böden */
struct XMSRastPort LSObjectRPInXMS;
/* in diesem RastPort befinden sich einige Objekte während der Planung */

static SDL_Surface *pShadowSurface = NULL;   // 640x328 pixels
static SDL_Surface *pRefreshSurface = NULL;  // 320x140 pixels

struct PrintRect GlobalPrintRect;
struct ColorRange GlobalColorRange;

struct GfxBase GfxBase;

LIST *CollectionList = NULL;
LIST *PictureList = NULL;

struct RastPort *l_wrp;
struct RastPort *m_wrp;
struct RastPort *u_wrp;

struct Font *bubbleFont;
struct Font *menuFont;

static void gfxInitXMSRastPort(struct XMSRastPort *rp, uword us_Width, uword us_Height);
static void gfxDoneXMSRastPort(struct XMSRastPort *rp);

static void gfxInitRastPort(struct RastPort *rp, uword us_LeftEdge, uword us_TopEdge, uword us_Width, uword us_Height,
                            ubyte uch_ColorStart, ubyte uch_ColorEnd, void *BitMap);

static void gfxInitCollList(void);
static void gfxInitPictList(void);

static struct Font *gfxOpenFont(char *puch_FileName, uword us_Width, uword us_Height, ubyte uch_FirstChar,
                                ubyte uch_LastChar, uword us_TotalWidth, uword us_TotalHeight);
static void gfxCloseFont(struct Font *font);

/************************************************/

/************************************************/

static char bGfxInvalidate = 0;
static char bGfxPaletteChanged = 0;

int gfxScalingOffsetX = 0;
int gfxScalingOffsetY = 0;
int gfxScalingFactor = 1;

static gfxFilterFunc gfxFilter = gfxFilter_x1_Copy;

ubyte gfxPaletteGlobal[256 * 4];

/************************************************/

#define GFX_BOARD_SIZE (640 * 328)

SDL_Surface *SurfaceScreen = NULL;
static const char *TitleStr = COSP_TITLE;
static ubyte *NCH4Buffer = NULL;

/************************************************/

static void SetWindowIcon(char *icon)
{
    unsigned int colorkey = 0;
    SDL_Surface *image = NULL;

    image = SDL_LoadBMP(icon);

    if (image)
    {
        colorkey = SDL_MapRGB(image->format, 2, 5, 1);

        SDL_SetColorKey(image, SDL_SRCCOLORKEY, colorkey);

        SDL_WM_SetIcon(image, NULL);
    }
}

ubyte *gfxGetGfxBoardBase(void) { return (pShadowSurface->pixels); }

void gfxInvalidate(void) { bGfxInvalidate = 1; }

// 2014-07-13 LucyG : guess what this one does
void gfxScreenshot(void)
{
    static int screenshotsTaken = 0;
    char fileName[256];

    sprintf(fileName, "." DIR_SEPARATOR "screenshot%02d.bmp", screenshotsTaken + 1);
    if (!SDL_SaveBMP(SurfaceScreen, fileName))
    {
        screenshotsTaken++;
    }
}

void gfxScreenshotShadow(void)
{
    static int screenshotsShadowTaken = 0;
    char fileName[256];

    sprintf(fileName, "." DIR_SEPARATOR "shadow%02d.bmp", screenshotsShadowTaken + 1);
    if (!SDL_SaveBMP(pShadowSurface, fileName))
    {
        screenshotsShadowTaken++;
    }
}

/************************************************/

void gfxSetDarkness(ubyte value)
{
    ubyte *dp = NULL;
    int h = 0, i = 0;

    dp = (ubyte *)l_wrp->p_BitMap;

    if (value == LS_BRIGHTNESS)
    {
        h = LS_MAX_AREA_WIDTH;

        do
        {
            i = LS_MAX_AREA_HEIGHT;

            do
            {
                *dp++ |= 64;
            } while (--i);
        } while (--h);
    }
    else
    {
        h = LS_MAX_AREA_WIDTH;

        do
        {
            i = LS_MAX_AREA_HEIGHT;

            do
            {
                *dp++ &= ~64;
            } while (--i);
        } while (--h);
    }

    bGfxInvalidate = 1;
}

/************************************************/

int gfxInitSDL(void)
{
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO))
    {
        Uint32 flags = SDL_SWSURFACE;
        char iconPath[TXT_KEY_LENGTH];

        if (Config.gfxFullScreen)
        {
            flags |= SDL_FULLSCREEN;
        }

        gfxScalingFactor = min((Config.gfxScreenWidth / 320), (Config.gfxScreenHeight / 200));
        if (gfxScalingFactor > 4)
        {
            gfxScalingFactor = 4;
        }
        else if (gfxScalingFactor < 1)
        {
            Log("%s|%s: screen too small", __FILE__, __func__);
            gfxScalingFactor = 1;
            Config.gfxScreenWidth = 320;
            Config.gfxScreenHeight = 200;
        }

        switch (gfxScalingFactor)
        {
            case 2:
                if (Config.gfxScaleNx)
                {
                    gfxFilter = gfxFilter_x2_Scale2x;
                }
                else
                {
                    gfxFilter = gfxFilter_x2_Copy;
                }
                break;
            case 3:
                if (Config.gfxScaleNx)
                {
                    gfxFilter = gfxFilter_x3_Scale3x;
                }
                else
                {
                    gfxFilter = gfxFilter_x3_Copy;
                }
                break;
            case 4:
                if (Config.gfxScaleNx)
                {
                    gfxFilter = gfxFilter_x4_Scale4x;
                    gfxFilter_x4_Buffer = (unsigned char *)MemAlloc(640 * 400);
                    if (!gfxFilter_x4_Buffer)
                    {
                        gfxFilter = gfxFilter_x4_Copy;
                    }
                }
                else
                {
                    gfxFilter = gfxFilter_x4_Copy;
                }
                break;
            default:
                gfxFilter = gfxFilter_x1_Copy;
                break;
        }

        gfxScalingOffsetX = (Config.gfxScreenWidth - (320 * gfxScalingFactor)) / 2;
        gfxScalingOffsetY = (Config.gfxScreenHeight - (200 * gfxScalingFactor)) / 2;

        dskBuildPathName(PICTURE_DIRECTORY, "icon.bmp", iconPath);
        SetWindowIcon(iconPath);
        SurfaceScreen = SDL_SetVideoMode(Config.gfxScreenWidth, Config.gfxScreenHeight, 8, flags);
        if (SurfaceScreen)
        {
            SDL_WM_SetCaption(TitleStr, TitleStr);
            pShadowSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 640, 328, 8, 0, 0, 0, 0);
            if (pShadowSurface)
            {
                pRefreshSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, 320, 140, 8, 0, 0, 0, 0);
                if (pRefreshSurface)
                {
                    NCH4Buffer = (ubyte *)MemAlloc(GFX_NCH4_BUFFER_SIZE);
                    return (1);
                }
            }
        }
    }
    return (0);
}

void gfxDoneSDL(void)
{
    if (pRefreshSurface)
    {
        SDL_FreeSurface(pRefreshSurface);
        pRefreshSurface = NULL;
    }
    if (pShadowSurface)
    {
        SDL_FreeSurface(pShadowSurface);
        pShadowSurface = NULL;
    }
    if (NCH4Buffer) MemFree(NCH4Buffer, GFX_NCH4_BUFFER_SIZE);
    if (gfxFilter_x4_Buffer)
    {
        MemFree(gfxFilter_x4_Buffer, 640 * 400);
    }
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void gfxUpdateSDL(struct RastPort *rp)
{
    ubyte *src = NULL, *dst = NULL;
    int32_t dstpitch = 0;

    if ((rp != &RefreshRP) && (rp != &PrepareRP))
    {
        src = gfxGetGfxBoardBase();

        if (GfxBase.uch_VideoMode == GFX_VIDEO_NCH4)
        {
            gfxNCH4PutNCH4ToMCGA(src + gfxNCH4GetCurrScrollOffset(), NCH4Buffer, 0, 0, 0, 0, 320, 128, 160, 320);
            gfxNCH4PutNCH4ToMCGA(src, NCH4Buffer + (320 * 128), 0, 0, 0, 0, 320, 72, 160, 320);
            src = NCH4Buffer;
        }

        if (bGfxPaletteChanged)
        {
            SDL_SetPalette(SurfaceScreen, SDL_LOGPAL | SDL_PHYSPAL, (SDL_Color *)gfxPaletteGlobal, 0, 256);
            SDL_SetPalette(pShadowSurface, SDL_LOGPAL | SDL_PHYSPAL, (SDL_Color *)gfxPaletteGlobal, 0, 256);
            bGfxPaletteChanged = 0;
        }

        SDL_LockSurface(SurfaceScreen);

        dst = (ubyte *)SurfaceScreen->pixels;
        dstpitch = SurfaceScreen->pitch;

        if (gfxScalingOffsetX > 0)
        {
            dst += gfxScalingOffsetX;
        }
        if (gfxScalingOffsetY > 0)
        {
            dst += dstpitch * gfxScalingOffsetY;
        }

        gfxFilter(src, 320, 200, dst, dstpitch);

        SDL_UnlockSurface(SurfaceScreen);

        SDL_UpdateRect(SurfaceScreen, 0, 0, 0, 0);
    }
    bGfxInvalidate = 0;
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

    /* diese RP müssen nur ein Bild maximaler Groesse aufnehmen koennen */
    /* in anderen Modulen wird vorausgesetzt, dass alle RastPorts gleich */
    /* gross sind und auch gleich gross wie die StdBuffer sind */
    /* StdBuffer = 61 * 1024 = 62464, XMS: 62400 */

    /* Ausnahme (nachtraeglich) : der RefreshRP ist nur 320 * 140 Pixel gross!! */

    gfxInitXMSRastPort(&StdRP0InXMS, 320, 195); /* CMAP muss auch Platz haben ! */
    gfxInitXMSRastPort(&StdRP1InXMS, 320, 195);
    gfxInitXMSRastPort(&AnimRPInXMS, 320, 195);
    gfxInitXMSRastPort(&AddRPInXMS, 320, 195);
    gfxInitXMSRastPort(&LSObjectRPInXMS, 320, 195);

    gfxInitXMSRastPort(&LSFloorRPInXMS, 320, 32);

    /* der RefreshRP muss den ganzen Bildschirm aufnehmen koennen */
    gfxInitXMSRastPort(&RefreshRPInXMS, 320, 200);

    /*
    #define GFX_NCH4_SCROLLOFFSET 	(640 * 72)  // Speicher , den die fixe Anzeige belegt

    gfxInitRastPort(struct RastPort *, us_LeftEdge, us_TopEdge, us_Width, us_Height, uch_ColorStart, uch_ColorEnd,
    BitMap);
    */

    gfxInitRastPort(&RefreshRP, 0, 0, 320, 140, 0, 0, pRefreshSurface->pixels);
    gfxInitRastPort(&PrepareRP, 0, 0, 320, 192, 0, 0, StdBuffer1);

    gfxInitRastPort(&LowerRP, 0, 0, 320, 140, 0, 191, pShadowSurface->pixels);
    gfxInitRastPort(&MenuRP, 0, 140, 320, 50, 191, 255, (ubyte *)pShadowSurface->pixels + 320 * 140);

    gfxInitRastPort(&NCH4ScrollRP, 0, 0, 640, 256, 0, 191, (ubyte *)pShadowSurface->pixels + GFX_NCH4_SCROLLOFFSET);
    gfxInitRastPort(&NCH4MenuRP, 0, 128, 320, 72, 191, 255, pShadowSurface->pixels);
    gfxInitRastPort(&NCH4UpperRP, 0, 0, 320, 60, 0, 191,
                    (ubyte *)pShadowSurface->pixels + GFX_NCH4_SCROLLOFFSET);  // this one moves
    // TopEdge von diesem RP muss 0 sein, da der Offset schon bei der Adresse
    // der BitMap richtig uebergeben wird

    l_wrp = &LowerRP;
    u_wrp = &LowerRP;
    m_wrp = &MenuRP;

    bubbleFont = gfxOpenFont(GFX_BUBBLE_FONT_NAME, 4, 8, 32, 255, 320, 24);
    menuFont = gfxOpenFont(GFX_MENU_FONT_NAME, 5, 9, 32, 255, 320, 36);

    gfxSetFont(l_wrp, bubbleFont);
    gfxSetFont(u_wrp, bubbleFont);
    gfxSetFont(m_wrp, menuFont);

    gfxSetFont(&NCH4ScrollRP, bubbleFont);
    gfxSetFont(&NCH4MenuRP, menuFont);
    gfxSetFont(&NCH4UpperRP, bubbleFont);

    gfxInitCollList();
    gfxInitPictList();

    gfxSetVideoMode(GFX_VIDEO_MCGA);  // nach RastPorts !!

    bGfxInvalidate = 1;
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

void gfxSetDestRPForShow(struct RastPort *rp) { GfxBase.p_DestRPForShow = rp; }

void gfxSetVideoMode(ubyte uch_NewMode)
{
    GfxBase.uch_VideoMode = uch_NewMode;

    SDL_FillRect(pShadowSurface, NULL, 0);

    switch (uch_NewMode)
    {
        case GFX_VIDEO_MCGA:
            LowerRP.uch_VideoMode = GFX_VIDEO_MCGA;
            MenuRP.uch_VideoMode = GFX_VIDEO_MCGA;
            l_wrp = &LowerRP;
            u_wrp = &LowerRP;
            m_wrp = &MenuRP;
            gfxSetRGB(NULL, 0, 0, 64, 48);
            gfxSetRGB(NULL, 255, 255, 255, 255);  // Maus
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
            memset(NCH4Buffer, 0, GFX_NCH4_BUFFER_SIZE);
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
void gfxCorrectUpperRPBitmap(void) { NCH4UpperRP.p_BitMap = gfxGetGfxBoardBase() + gfxNCH4GetCurrScrollOffset(); }

/********************************************************************
 * lists
 */

static void gfxInitCollList(void)
{
    char pathname[TXT_KEY_LENGTH];
    LIST *tempList = (LIST *)CreateList(0);
    NODE *n = NULL;
    struct Collection *coll = NULL;

    CollectionList = (LIST *)CreateList(0);

    dskBuildPathName(TEXT_DIRECTORY, COLL_LIST_TXT, pathname);
    ReadList(tempList, 0, pathname, 0);

    for (n = (NODE *)LIST_HEAD(tempList); NODE_SUCC(n); n = (NODE *)NODE_SUCC(n))
    {
        coll = (struct Collection *)CreateNode(CollectionList, sizeof(struct Collection), txtGetKey(2, NODE_NAME(n)));

        coll->us_CollId = (uword)txtGetKeyAsULONG(1, NODE_NAME(n));

        coll->puch_Filename = (ubyte *)NODE_NAME(coll);
        coll->p_Prepared = 0;

        coll->us_TotalWidth = (uword)txtGetKeyAsULONG(3, NODE_NAME(n));
        coll->us_TotalHeight = (uword)txtGetKeyAsULONG(4, NODE_NAME(n));

        coll->uch_ColorRangeStart = (ubyte)txtGetKeyAsULONG(5, NODE_NAME(n));
        coll->uch_ColorRangeEnd = (ubyte)txtGetKeyAsULONG(6, NODE_NAME(n));

        if (bProfidisk)
        {
            // is nonessential,because all the data are read from the hard disk
            coll->fromDisk = (ubyte)txtGetKeyAsULONG(7, NODE_NAME(n));
        }
    }

    RemoveList(tempList);
}

static void gfxInitPictList(void)
{
    char pathname[TXT_KEY_LENGTH];
    LIST *tempList = (LIST *)CreateList(0);
    NODE *n = NULL;
    struct Picture *pict = NULL;

    PictureList = (LIST *)CreateList(0);

    dskBuildPathName(TEXT_DIRECTORY, PICT_LIST_TXT, pathname);
    ReadList(tempList, 0, pathname, 0);

    for (n = (NODE *)LIST_HEAD(tempList); NODE_SUCC(n); n = (NODE *)NODE_SUCC(n))
    {
        pict = (struct Picture *)CreateNode(PictureList, sizeof(struct Picture), NULL);

        pict->us_PictId = (uword)txtGetKeyAsULONG(1, NODE_NAME(n));
        pict->us_CollId = (uword)txtGetKeyAsULONG(2, NODE_NAME(n));

        pict->us_XOffset = (uword)txtGetKeyAsULONG(3, NODE_NAME(n));
        pict->us_YOffset = (uword)txtGetKeyAsULONG(4, NODE_NAME(n));

        pict->us_Width = (uword)txtGetKeyAsULONG(5, NODE_NAME(n));
        pict->us_Height = (uword)txtGetKeyAsULONG(6, NODE_NAME(n));

        pict->us_DestX = (uword)txtGetKeyAsULONG(7, NODE_NAME(n));
        pict->us_DestY = (uword)txtGetKeyAsULONG(8, NODE_NAME(n));
    }

    RemoveList(tempList);
}

/********************************************************************
 * Rastports...
 */

static void gfxInitXMSRastPort(struct XMSRastPort *rp, uword us_Width, uword us_Height)
{
    rp->pSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, us_Width, us_Height, 8, 0, 0, 0, 0);
    SDL_FillRect(rp->pSurface, NULL, 0);
    /*
    rp->us_Width  = us_Width;
    rp->us_Height = us_Height;
    rp->p_MemHandle = MemAlloc((uint32_t)rp->us_Width * (uint32_t)rp->us_Height);
    */
    rp->us_CollId = GFX_NO_COLL_IN_XMS;
}

static void gfxDoneXMSRastPort(struct XMSRastPort *rp)
{
    if (rp->pSurface)
    {
        SDL_FreeSurface(rp->pSurface);
    }
    /*
    if (rp->p_MemHandle) {
            MemFree(rp->p_MemHandle, (uint32_t)rp->us_Width * (uint32_t)rp->us_Height);
            rp->p_MemHandle = NULL;
    }
    */
}

static void gfxInitRastPort(struct RastPort *rp, uword us_LeftEdge, uword us_TopEdge, uword us_Width, uword us_Height,
                            ubyte uch_ColorStart, ubyte uch_ColorEnd, void *p_BitMap)
{
    rp->us_LeftEdge = us_LeftEdge;
    rp->us_TopEdge = us_TopEdge;
    rp->us_Width = us_Width;
    rp->us_Height = us_Height;
    rp->us_DrawMode = GFX_JAM_1;
    rp->uch_FrontPenAbs = uch_ColorStart + 1;
    rp->uch_BackPenAbs = uch_ColorStart;
    rp->uch_OutlinePenAbs = uch_ColorStart + 1;
    rp->uch_ColorStart = uch_ColorStart;
    rp->uch_ColorEnd = uch_ColorEnd;
    rp->uch_VideoMode = GFX_VIDEO_MCGA;
    gfxMoveCursor(rp, 0, 0);
    rp->p_BitMap = p_BitMap;
    rp->p_Font = 0;
}

/*******************************************************************
 * FONTS
 * gfxOpenFont
 * gfxCloseFont
 */

static struct Font *gfxOpenFont(char *puch_FileName, uword us_Width, uword us_Height, ubyte uch_FirstChar,
                                ubyte uch_LastChar, uword us_TotalWidth, uword us_TotalHeight)
{
    char Result[TXT_KEY_LENGTH];
    struct Font *font = (struct Font *)MemAlloc(sizeof(struct Font));

    font->us_Width = us_Width;
    font->us_Height = us_Height;

    font->uch_FirstChar = uch_FirstChar;
    font->uch_LastChar = uch_LastChar;

    dskBuildPathName(PICTURE_DIRECTORY, puch_FileName, Result);

    font->pSurface = gfxLoadImage(Result);

    return (font);
}

static void gfxCloseFont(struct Font *font)
{
    if (font)
    {
        if (font->pSurface)
        {
            SDL_FreeSurface(font->pSurface);
            font->pSurface = NULL;
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
    /* LucyG 2017-10-29 : this never worked at all! */
    /* Hell, it's a simple line drawing function :D */

    int sx = 0, sy = 0, dx = 0, dy = 0, i = 0;
    ubyte col = 0;
    ubyte *bm = NULL;

    if (!rp)
    {
        Log("%s|%s: RastPort is NULL", __FILE__, __func__);
        return;
    }

    if ((us_X >= 0) && (us_X < rp->us_Width) && (us_Y >= 0) && (us_Y < rp->us_Height))
    {
        col = rp->uch_FrontPenAbs;

        sx = min(us_X, rp->us_CursorXPosAbs);
        sy = min(us_Y, rp->us_CursorYPosAbs);
        dx = max(us_X, rp->us_CursorXPosAbs);
        dy = max(us_Y, rp->us_CursorYPosAbs);

        // the starting position
        bm = &((ubyte *)rp->p_BitMap)[sy * rp->us_Width + sx];

        if (sy == dy)
        {
            // horizontal
            for (i = sx; i <= dx; i++)
            {
                *bm++ = col;
            }
        }
        else if (sx == dx)
        {
            // vertical
            for (i = sy; i <= dy; i++)
            {
                *bm = col;
                bm += rp->us_Width;
            }
        }
        else
        {
            // diagonal (not used?)
            Log("%s|%s: diagonal line not implemented *****", __FILE__, __func__);
        }

        rp->us_CursorXPosAbs = us_X;
        rp->us_CursorYPosAbs = us_Y;
    }

    // gfxUpdateSDL(rp);
    bGfxInvalidate = 1;
}

ubyte gfxReadPixel(struct RastPort *rp, uword us_X, uword us_Y)
{
    ubyte col = 0;

    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return (0);
    }

    if (rp->uch_VideoMode == GFX_VIDEO_MCGA) col = gfxMCGAReadPixel(rp, us_X, us_Y);

    if (rp->uch_VideoMode == GFX_VIDEO_NCH4) col = gfxNCH4ReadPixel(rp, us_X, us_Y);

    return col;
}

void gfxMoveCursor(struct RastPort *rp, uword us_X, uword us_Y)
{
    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return;
    }
    rp->us_CursorXPosAbs = us_X;
    rp->us_CursorYPosAbs = us_Y;
}

void gfxSetPens(struct RastPort *rp, ubyte uch_Front, ubyte uch_Back, ubyte uch_Out)
{
    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return;
    }
    if (uch_Front != GFX_SAME_PEN) rp->uch_FrontPenAbs = uch_Front;

    if (uch_Back != GFX_SAME_PEN) rp->uch_BackPenAbs = uch_Back;

    if (uch_Out != GFX_SAME_PEN) rp->uch_OutlinePenAbs = uch_Out;
}

// minimum size: 3 * 3 pixel !
void gfxRectFill(struct RastPort *rp, uword us_SX, uword us_SY, uword us_EX, uword us_EY)
{
    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return;
    }
    if (rp->uch_VideoMode == GFX_VIDEO_MCGA)
    {
        gfxMCGARectFill(rp, us_SX, us_SY, us_EX, us_EY);
    }

    if (rp->uch_VideoMode == GFX_VIDEO_NCH4)
    {
        gfxNCH4RectFill(rp, us_SX, us_SY, us_EX, us_EY);
    }

    // gfxUpdateSDL(rp);
    bGfxInvalidate = 1;
}

void gfxSetDrMd(struct RastPort *rp, uword us_DrawMode)
{
    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return;
    }
    rp->us_DrawMode = us_DrawMode;
}

void gfxSetFont(struct RastPort *rp, struct Font *font)
{
    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return;
    }
    rp->p_Font = font;
}

/* calculates the length of a text in pixels */
int32_t gfxTextLength(struct RastPort *rp, char *puch_Text, uword us_CharCount)
{
    int32_t length = 0;
    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return (0);
    }
    length = (int32_t)strlen(puch_Text);
    return (length * (rp->p_Font->us_Width));
}

/*******************************************************************
 * access & calc functions
 */

struct Collection *gfxGetCollection(uword us_CollId)
{
    return (struct Collection *)GetNthNode(CollectionList, (uint32_t)(us_CollId - 1));
}

struct Picture *gfxGetPicture(uword us_PictId)
{
    return (struct Picture *)GetNthNode(PictureList, (uint32_t)(us_PictId - 1));
}

// The Collection must be prepared one step before (with gfxPrepareColl)
void gfxGetColorTable(uword us_CollId, ubyte *puch_ColorTable)
{
    int32_t i = 0;
    ubyte *p = (ubyte *)((intptr_t)StdBuffer1 + (intptr_t)GFX_CMAP_OFFSET);

    for (i = 0; i < GFX_COLORTABLE_SIZE; i++) puch_ColorTable[i] = p[i];
}

void gfxSetColorTable_hack(SDL_Surface *pSurface)
{
    int32_t i = 0;
    ubyte *p = (ubyte *)((intptr_t)StdBuffer1 + (intptr_t)GFX_CMAP_OFFSET);

    SDL_Color *pc = NULL;
    for (i = 0; i < 256; i++)
    {
        pc = &pSurface->format->palette->colors[i];
        *p++ = pc->r;
        *p++ = pc->g;
        *p++ = pc->b;
    }
}

static int32_t gfxGetRealDestY(struct RastPort *rp, int32_t destY)
{
    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return (0);
    }
    if (rp->uch_VideoMode == GFX_VIDEO_NCH4)
    {
        // dieser RastPort vereingt im NCH4-mode sowohl den Upper als auch
        // den "normalen" Menurp
        // beim MenuRp muss die TopEdge abgezogen werden, beim UpperRp darf
        // dies nicht geschehen

        if (destY >= m_wrp->us_TopEdge) destY -= m_wrp->us_TopEdge;
    }

    if (rp->uch_VideoMode == GFX_VIDEO_MCGA) destY -= rp->us_TopEdge;

    return destY;
}

/*******************************************************************
 * collections...
 */

/* nach dieser Funktion befindet sich im PrepareRP die entpackte Collection */
struct RastPort *gfxPrepareColl(uword us_CollId)
{
    struct Collection *coll = gfxGetCollection(us_CollId);
    if (!coll)
    {
        Log("gfxPrepareColl: coll %d is NULL", us_CollId);
    }
    else
    {
        if (!coll->p_Prepared)
        {
            char Result[TXT_KEY_LENGTH];

            /* Dateiname erstellen */
            dskBuildPathName(PICTURE_DIRECTORY, (char *)coll->puch_Filename, Result);

            /* load collection */
            SDL_Surface *pSurface = NULL;
            pSurface = gfxLoadImage(Result);

            gfxSetColorTable_hack(pSurface);

            /* Collection in den PrepareRP kopieren */
            // use temp surface to blit image to because src and dst of pixels must be the same size
            SDL_Surface *pTmpSurface =
                SDL_CreateRGBSurface(SDL_SWSURFACE, PrepareRP.us_Width, PrepareRP.us_Height, 8, 0, 0, 0, 0);
            SDL_SetPalette(pTmpSurface, SDL_LOGPAL | SDL_PHYSPAL, pSurface->format->palette->colors, 0,
                           pSurface->format->palette->ncolors);
            SDL_BlitSurface(pSurface, NULL, pTmpSurface, NULL);
            memcpy(PrepareRP.p_BitMap, pTmpSurface->pixels, pTmpSurface->w * pTmpSurface->h);
            SDL_FreeSurface(pTmpSurface);
            SDL_FreeSurface(pSurface);
        }
        else
        {
            /* vom aktuellen RP(muss XMS sein) in den PrepareRP kopieren */

            SDL_LockSurface(coll->p_Prepared->pSurface);

            memcpy(PrepareRP.p_BitMap, coll->p_Prepared->pSurface->pixels,
                   coll->p_Prepared->pSurface->w * coll->p_Prepared->pSurface->h); /* does not respect pitch! */

            SDL_UnlockSurface(coll->p_Prepared->pSurface);
        }
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
    if (!coll)
    {
        Log("gfxCopyCollFromXMS: coll %d is NULL", us_CollId);
    }
    else
    {
        if (coll->p_Prepared)
        {
            // memcpy(p_Buffer, coll->p_Prepared->p_MemHandle, coll->p_Prepared->us_Width *
            // coll->p_Prepared->us_Height);

            SDL_LockSurface(coll->p_Prepared->pSurface);

            memcpy(p_Buffer, coll->p_Prepared->pSurface->pixels,
                   coll->p_Prepared->pSurface->w * coll->p_Prepared->pSurface->h); /* does not respect pitch! */

            SDL_UnlockSurface(coll->p_Prepared->pSurface);
        }
    }
}

void gfxCopyCollToXMS(uword us_CollId, struct XMSRastPort *rp)
{
    struct Collection *oldColl = NULL;
    struct Collection *coll = gfxGetCollection(us_CollId);
    if (!coll)
    {
        Log("gfxCopyCollToXMS: coll %d is NULL", us_CollId);
    }

    // wenn sich in diesem XMS RastPort ein anderes Bild befindet so wird dieses
    // nun aus dem XMS RastPort "entfernt"
    if ((rp->us_CollId != GFX_NO_COLL_IN_XMS) && (us_CollId != rp->us_CollId))
    {
        oldColl = gfxGetCollection(rp->us_CollId);
        if (!oldColl)
        {
            Log("gfxCopyCollToXMS: oldColl %d is NULL", rp->us_CollId);
        }
        else
        {
            oldColl->p_Prepared = 0;
        }
    }

    gfxPrepareColl(us_CollId);

    // memcpy(rp->p_MemHandle, StdBuffer1, size);

    SDL_LockSurface(rp->pSurface);

    memcpy(rp->pSurface->pixels, StdBuffer1, rp->pSurface->w * rp->pSurface->h); /* does not respect pitch! */

    SDL_UnlockSurface(rp->pSurface);

    // in der neuen Collection wird der XMS RastPort eingetragen
    if (coll)
    {
        coll->p_Prepared = rp;
    }

    rp->us_CollId = us_CollId;
}

/*******************************************************************
 * print functions
 */

void gfxSetRect(uword us_X, uword us_Width)
{
    GlobalPrintRect.us_X = us_X;
    GlobalPrintRect.us_Width = us_Width;
}

void gfxPrintExact(struct RastPort *rp, char *puch_Text, uword us_X, uword us_Y)
{
    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return;
    }
    if (rp->uch_VideoMode == GFX_VIDEO_MCGA) gfxMCGAPrintExact(rp, puch_Text, us_X, us_Y);

    if (rp->uch_VideoMode == GFX_VIDEO_NCH4) gfxNCH4PrintExact(rp, puch_Text, us_X, us_Y);

    // gfxUpdateSDL(rp);
    bGfxInvalidate = 1;
}

void gfxPrint(struct RastPort *rp, char *puch_Text, uword us_Y, uint32_t ul_Mode)
{
    uword x = 0, y = 0, l = 0;

    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return;
    }
    x = GlobalPrintRect.us_X;
    y = us_Y;
    l = gfxTextLength(rp, puch_Text, strlen(puch_Text));

    if (ul_Mode & GFX_PRINT_RIGHT) x += GlobalPrintRect.us_Width - l;

    if (ul_Mode & GFX_PRINT_CENTER)
    {
        if (l <= GlobalPrintRect.us_Width) x += (GlobalPrintRect.us_Width - l) / 2;
    }

    if (ul_Mode & GFX_PRINT_SHADOW)
    {
        if (!Config.gfxNoFontShadow)
        {
            ubyte APen = rp->uch_FrontPenAbs;
            rp->uch_FrontPenAbs = rp->uch_BackPenAbs;
            gfxPrintExact(rp, puch_Text, x + 1, y + 1);
            rp->uch_FrontPenAbs = APen;
        }
    }

    gfxPrintExact(rp, puch_Text, x, y);
}

/*******************************************************************
 * refresh...
 */

/* kopiert aktuelles Bild in den RefreshRPInXMS */
void gfxPrepareRefresh(void)
{
    SDL_LockSurface(RefreshRPInXMS.pSurface);

    ubyte *mem = (ubyte *)RefreshRPInXMS.pSurface->pixels;
    if (!mem)
    {
        Log("%s: p_MemHandle is NULL", __func__);
        return;
    }

    switch (GfxBase.uch_VideoMode)
    {
        case GFX_VIDEO_MCGA:
            memcpy(mem, gfxGetGfxBoardBase(), 320 * 200);
            break;
        case GFX_VIDEO_NCH4:
            gfxNCH4PutNCH4ToMCGA(gfxGetGfxBoardBase() + gfxNCH4GetCurrScrollOffset(), mem, 0, 0, 0, 0, 320, 128, 160,
                                 320);
            gfxNCH4PutNCH4ToMCGA(gfxGetGfxBoardBase(), mem + (320 * 128), 0, 0, 0, 0, 320, 72, 160, 320);
            break;
    }

    SDL_UnlockSurface(RefreshRPInXMS.pSurface);
}

/* kopiert aktuellen Inhalt des RefreshRPInXMS in den Bildschirmspeicher */
void gfxRefresh(void)
{
    SDL_LockSurface(RefreshRPInXMS.pSurface);

    ubyte *mem = (ubyte *)RefreshRPInXMS.pSurface->pixels;
    if (!mem)
    {
        Log("%s: p_MemHandle is NULL", __func__);
        return;
    }

    switch (GfxBase.uch_VideoMode)
    {
        case GFX_VIDEO_NCH4:
            gfxNCH4PutMCGAToNCH4(mem, gfxGetGfxBoardBase() + gfxNCH4GetCurrScrollOffset(), 0, 0, 0, 0, 320, 128, 320,
                                 160);
            gfxNCH4PutMCGAToNCH4(mem + (320 * 128), gfxGetGfxBoardBase(), 0, 0, 0, 0, 320, 72, 320, 160);
            break;
        case GFX_VIDEO_MCGA:
        default:
            memcpy(gfxGetGfxBoardBase(), mem, 320 * 200);
            break;
    }

    SDL_UnlockSurface(RefreshRPInXMS.pSurface);

    gfxUpdateSDL(NULL);
}

/*******************************************************************
 * presentation
 */

void gfxBlit(struct RastPort *srp, uword us_SourceX, uword us_SourceY, struct RastPort *drp, uword us_DestX,
             uword us_DestY, uword us_Width, uword us_Height, uint32_t ul_BlitMode)
{
    if (!srp)
    {
        Log("%s: source RastPort is NULL", __func__);
        return;
    }
    if (!drp)
    {
        Log("%s: dest RastPort is NULL", __func__);
        return;
    }

    // gfxWaitTOF();

    if ((srp->uch_VideoMode == GFX_VIDEO_MCGA) && (drp->uch_VideoMode == GFX_VIDEO_MCGA))
    {
        gfxMCGABlit(srp, us_SourceX, us_SourceY, drp, us_DestX, us_DestY, us_Width, us_Height,
                    ul_BlitMode & (GFX_ONE_STEP | GFX_OVERLAY));
    }

    if ((srp->uch_VideoMode == GFX_VIDEO_MCGA) && (drp->uch_VideoMode == GFX_VIDEO_NCH4))
    {
        if (ul_BlitMode & GFX_ONE_STEP)
            gfxNCH4PutMCGAToNCH4(srp->p_BitMap, drp->p_BitMap, us_SourceX, us_SourceY, us_DestX, us_DestY, us_Width,
                                 us_Height, 320, 160);

        if (ul_BlitMode & GFX_OVERLAY)
            gfxNCH4OLMCGAToNCH4(srp->p_BitMap, drp->p_BitMap, us_SourceX, us_SourceY, us_DestX, us_DestY, us_Width,
                                us_Height, 320, 160);
    }

    // gfxUpdateSDL(rp);
    bGfxInvalidate = 1;
}

struct RastPort *gfxGetDestRP(int32_t l_DestX, int32_t l_DestY)
{
    struct RastPort *rp = NULL;

    if (GfxBase.uch_VideoMode == GFX_VIDEO_MCGA)
    {
        rp = u_wrp;

        if (l_DestY >= rp->us_TopEdge + rp->us_Height)
        {
            rp = l_wrp;

            if (l_DestY >= rp->us_TopEdge + rp->us_Height) rp = m_wrp;
        }
    }

    if (GfxBase.uch_VideoMode == GFX_VIDEO_NCH4)
    {
        rp = l_wrp;

        // if ((l_DestY < 60) || (l_DestY >= 140))
        //	rp = m_wrp;

        if (l_DestY < 60) rp = u_wrp;

        if (l_DestY >= 140) rp = m_wrp;
    }

    return rp;
}

void wfr()
// für Bildschirmsync verwenden!!!
{
    if (bGfxInvalidate || bGfxPaletteChanged)
    {
        gfxUpdateSDL(NULL);
    }
    else
    {
        SDL_Delay(1);
    }
}

void wfd(void)
// sperrt im Gegensatz zu wfr die Interrupts nicht
// wenn die Musik laeuft kann es daher vorkommen, dass
// dieses wfd einmal aussetzt
// !!! NICHT für Bildschirmsync verwenden!!
{
    wfr();
}

void gfxClearArea(struct RastPort *rp)
{
    ubyte uch_APen = 0, uch_BPen = 0, uch_OPen = 0;

    if (!rp)
    {
        Log("%s: RastPort is NULL", __func__);
        return;
    }

    uch_APen = rp->uch_FrontPenAbs;
    uch_BPen = rp->uch_BackPenAbs;
    uch_OPen = rp->uch_OutlinePenAbs;

    gfxSetPens(rp, 0, 0, 0);
    gfxRectFill(rp, 0, 0, rp->us_Width - 1, rp->us_Height - 1);
    gfxSetPens(rp, uch_APen, uch_BPen, uch_OPen);
}

void gfxSetRGB(struct RastPort *rp, ubyte uch_ColNr, ubyte uch_Red, ubyte uch_Green, ubyte uch_Blue)
{
    uint32_t n4 = (uint32_t)uch_ColNr << 2;
    gfxPaletteGlobal[n4] = uch_Red;
    gfxPaletteGlobal[n4 + 1] = uch_Green;
    gfxPaletteGlobal[n4 + 2] = uch_Blue;
    gfxPaletteGlobal[n4 + 3] = 0;
    bGfxPaletteChanged = 1;
    bGfxInvalidate = 1;
}

void gfxSetPalette24(struct RastPort *rp, ubyte uch_ColStart, ubyte uch_ColEnd, ubyte *p_ColorTable24)
{
    int i = 0, i3 = 0, i4 = 0;
    for (i = uch_ColStart; i < uch_ColEnd; i++)
    {
        i3 = i * 3;
        i4 = i << 2;

        gfxPaletteGlobal[i4] = p_ColorTable24[i3];
        gfxPaletteGlobal[i4 + 1] = p_ColorTable24[i3 + 1];
        gfxPaletteGlobal[i4 + 2] = p_ColorTable24[i3 + 2];
        gfxPaletteGlobal[i4 + 3] = 0;
    }
    bGfxPaletteChanged = 1;
    bGfxInvalidate = 1;
}

void gfxSetPalette32(struct RastPort *rp, ubyte uch_ColStart, ubyte uch_ColEnd, ubyte *p_ColorTable32)
{
    int i = 0, i4 = 0;
    for (i = uch_ColStart; i < uch_ColEnd; i++)
    {
        i4 = i << 2;

        gfxPaletteGlobal[i4] = p_ColorTable32[i4];
        gfxPaletteGlobal[i4 + 1] = p_ColorTable32[i4 + 1];
        gfxPaletteGlobal[i4 + 2] = p_ColorTable32[i4 + 2];
        gfxPaletteGlobal[i4 + 3] = 0;
    }
    bGfxPaletteChanged = 1;
    bGfxInvalidate = 1;
}

void gfxSetColorRange(ubyte uch_ColorStart, ubyte uch_ColorEnd)
{
    GlobalColorRange.uch_Start = uch_ColorStart;
    GlobalColorRange.uch_End = uch_ColorEnd;
}

void gfxGetColorTableFromReg(ubyte *puch_Colortable)
{
    int i = 0, i3 = 0, i4 = 0;
    for (i = 0; i < 256; i++)
    {
        i3 = i * 3;
        i4 = i << 2;
        puch_Colortable[i3] = gfxPaletteGlobal[i4];
        puch_Colortable[i3 + 1] = gfxPaletteGlobal[i4 + 1];
        puch_Colortable[i3 + 2] = gfxPaletteGlobal[i4 + 2];
    }
}

#define interpol(i, f, d) (ubyte)((float)(i) + ((float)(f) - (float)(i)) * (float)(d))

void gfxChangeColors(struct RastPort *rp, int32_t l_Delay, uint32_t ul_Mode, ubyte *colorTable)
{
    ubyte cols[768];
    ubyte cols2[768];
    uword st = 0, en = 0, st3 = 0, en3 = 0;
    uword i = 0;
    float e = NAN, f = NAN;

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

    l_Delay = max(l_Delay, 1);  // l_Delay may not be zero!

    inpSetWaitTicks(l_Delay);

    f = 0.0f;
    e = 1.0f / (float)(l_Delay + 1);

    switch (ul_Mode)
    {
        case GFX_FADE_OUT:  // fade to 0,64,48
            gfxGetColorTableFromReg(cols);
            do
            {
                for (i = st3; i <= en3; i += 3)
                {
                    cols2[i] = interpol(cols[i], 0, f);
                    cols2[i + 1] = interpol(cols[i + 1], 64, f);
                    cols2[i + 2] = interpol(cols[i + 2], 48, f);
                }
                f += e;
                gfxSetPalette24(rp, st, en, cols2);
                inpWaitFor(INP_TIME);
            } while (--l_Delay);
            for (i = st3; i <= en3; i += 3)
            {
                cols2[i] = 0;
                cols2[i + 1] = 64;
                cols2[i + 2] = 48;
            }
            gfxSetPalette24(rp, st, en, cols2);
            inpWaitFor(INP_TIME);
            break;
        case GFX_BLEND_UP:  // fade to colorTable
            do
            {
                for (i = st3; i <= en3; i += 3)
                {
                    cols[i] = interpol(0, ((ubyte *)colorTable)[i], f);
                    cols[i + 1] = interpol(64, ((ubyte *)colorTable)[i + 1], f);
                    cols[i + 2] = interpol(48, ((ubyte *)colorTable)[i + 2], f);
                }
                f += e;
                gfxSetPalette24(rp, st, en, cols);
                inpWaitFor(INP_TIME);
            } while (--l_Delay);
            gfxSetPalette24(rp, st, en, (ubyte *)colorTable);
            inpWaitFor(INP_TIME);
            break;
    }
}

void gfxShow(uword us_PictId, uint32_t ul_Mode, int32_t l_Delay, int32_t l_XPos, int32_t l_YPos)
{
    struct Picture *pict = NULL;
    struct Collection *coll = NULL;
    struct RastPort *destRP = NULL;
    ubyte *colorTable = NULL;
    int32_t destX = 0;
    int32_t destY = 0;

    pict = gfxGetPicture(us_PictId);
    if (!pict)
    {
        Log("gfxShow: Picture %d is NULL", us_PictId);
        return;
    }
    coll = gfxGetCollection(pict->us_CollId);
    if (!coll)
    {
        Log("gfxShow: Picture %d Collection %d is NULL", us_PictId, pict->us_CollId);
    }
    colorTable = (ubyte *)PrepareRP.p_BitMap + GFX_CMAP_OFFSET;
    if (!colorTable)
    {
        Log("gfxShow: colorTable is NULL");
    }
    destX = pict->us_DestX;
    destY = pict->us_DestY;

    if (l_XPos != -1) destX = l_XPos;

    if (l_YPos != -1) destY = l_YPos;

    if (GfxBase.p_DestRPForShow)
        destRP = GfxBase.p_DestRPForShow;
    else
        destRP = gfxGetDestRP(destX, destY);

    destY = gfxGetRealDestY(destRP, destY);

    gfxPrepareColl(pict->us_CollId);

    if (!(ul_Mode & GFX_NO_REFRESH)) gfxPrepareRefresh();

    if (ul_Mode & GFX_CLEAR_FIRST) gfxClearArea(destRP);

    if (ul_Mode & GFX_FADE_OUT)
    {
        if (coll)
        {
            gfxSetColorRange(coll->uch_ColorRangeStart, coll->uch_ColorRangeEnd);
            gfxChangeColors(NULL, l_Delay, GFX_FADE_OUT, 0);
        }
    }

    if (!l_Delay)
    {
        if (ul_Mode & GFX_BLEND_UP)
        {
            if (coll && colorTable)
            {
                gfxSetColorRange(coll->uch_ColorRangeStart, coll->uch_ColorRangeEnd);
                gfxChangeColors(NULL, l_Delay, GFX_BLEND_UP, colorTable);
            }
        }
    }

    if (ul_Mode & GFX_OVERLAY)
        gfxBlit(&PrepareRP, pict->us_XOffset, pict->us_YOffset, destRP, destX, destY, pict->us_Width, pict->us_Height,
                ul_Mode);

    if (ul_Mode & GFX_ONE_STEP)
        gfxBlit(&PrepareRP, pict->us_XOffset, pict->us_YOffset, destRP, destX, destY, pict->us_Width, pict->us_Height,
                ul_Mode);

    if (l_Delay)
    {
        if (ul_Mode & GFX_BLEND_UP)
        {
            if (coll && colorTable)
            {
                gfxSetColorRange(coll->uch_ColorRangeStart, coll->uch_ColorRangeEnd);
                gfxChangeColors(NULL, l_Delay, GFX_BLEND_UP, colorTable);
            }
        }
    }

    gfxSetDestRPForShow(NULL);
}
