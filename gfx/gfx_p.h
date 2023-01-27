/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

#define GFX_PAGE_SIZE					(64000L)		/* 320*200 = 64000 Bytes gross */

#define GFX_LOAD_BUFFER					StdBuffer0
#define GFX_DECR_BUFFER					StdBuffer1

#define GFX_DECR_BUFFER_SIZE			STD_BUFFER1_SIZE

#define GFX_BUBBLE_FONT_NAME			((char*)"bubble.fnt")
#define GFX_MENU_FONT_NAME				((char*)"menu.fnt")

#define GFX_CMAP_OFFSET					61440	// -> Maximalgroesse 320 * 192

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

	ubyte	uch_VideoMode;
	};

struct RastPort LowerRP;
struct RastPort MenuRP;

struct RastPort NCH4MenuRP;
struct RastPort NCH4ScrollRP;
struct RastPort NCH4UpperRP;

struct RastPort RefreshRP;
/* wird verwendet fÅr Double Buffering wÑhrend der Vorbereitung */
struct RastPort PrepareRP;
/* Da man von XMS nur ganze Blîcke kopieren kann (ohne Modulo)    */
/* mÅssen einige Grafiken, bevor sie ins Video RAM kopiert werden */
/* hier an dieser Stelle gepuffert werden. */
/* ist als extern definfiert, da Animmodul darauf zugreifen mu· */


struct XMSRastPort StdRP0InXMS;
/* wird benutzt fÅr Objekte, die immer im Speicher sind */
struct XMSRastPort StdRP1InXMS;
/* wird benutzt fÅr Objekte, die immer im Speicher sind */
struct XMSRastPort RefreshRPInXMS;
/* Bild, das im Moment gerade gezeigt wird (fÅr Refresh) */
struct XMSRastPort AnimRPInXMS;
/* beinhaltet die Animphasen des gerade aktuellen Bildes */
/* um Inkonsistenzen zu vermeiden, wird jedesmal, bevor eine */
/* Animphase gezeigt wird, das gesamte Bild in den PrepareRp */
/* kopiert, der benîtigte Ausschnitt wird dann von dort */
/* ins Video-RAM kopiert */
/* ist als extern definfiert, da Animmodul darauf zugreifen mu· */
struct XMSRastPort AddRPInXMS;
/* in diesem Rastport befinden sich diverse zusÑtzliche Grafiken */
/* die in das Bild hinzugefÅgt werden (Gesichter, Werkzeuge)     */
/* je nach Situation... 													  */
/* dieser RP wird je nach Situation ausgewechselt					  */
struct XMSRastPort LSFloorRPInXMS;
/* in diesem RastPort befindet sich wÑhrend der Planungs-Einbruchs */
/* phasen das Bild der Bîden */
struct XMSRastPort LSObjectRPInXMS;
/* in diesem RastPort befinden sich einige Objekte wÑhrend der Planung */

static ubyte *GfxBoardBase = NULL;

struct PrintRect 			GlobalPrintRect;
struct ColorRange			GlobalColorRange;

struct GfxBase				GfxBase;

LIST	*CollectionList = NULL;
LIST  *PictureList = NULL;

struct RastPort *l_wrp;
struct RastPort *m_wrp;
struct RastPort *u_wrp;

struct Font *bubbleFont;
struct Font *menuFont;

static void gfxInitXMSRastPort(struct XMSRastPort *rp, uword us_Width, uword us_Height);
static void gfxDoneXMSRastPort(struct XMSRastPort *rp);

static void gfxInitRastPort(struct RastPort *rp, uword us_LeftEdge, uword us_TopEdge, uword us_Width, uword us_Height, ubyte uch_ColorStart, ubyte uch_ColorEnd, void *BitMap);

static void gfxInitCollList(void);
static void gfxInitPictList(void);

static void gfxGetCMAP(ubyte *p_Data, ubyte *p_Dest);

static struct Font *gfxOpenFont(char *puch_FileName, uword us_Width, uword us_Height, ubyte uch_FirstChar, ubyte uch_LastChar, uword us_TotalWidth, uword us_TotalHeight);
static void gfxCloseFont(struct Font *font);

