/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

#define ACTIV_POSS   3L
#define INACTIV_POSS 2L
#define GHOSTED      1L

#define BG_INACTIVE_COLOR		252
#define VG_INACTIVE_COLOR		253

#define TXT_MAX_MENU_COLUMNS     8
#define TXT_1ST_MENU_LINE_Y     24
#define TXT_2ND_MENU_LINE_Y     36

#define INT_BUBBLE_WIDTH		 200

// misc
ubyte CurrentBubbleType   = 3;
ubyte ExtBubbleActionInfo = 0;

uword ActivPersonPictID = MATT_PICTID;
uword MenuCoords[TXT_MAX_MENU_COLUMNS];

void (*MenuTimeOutFunc)(void);

// refresh
LIST *refreshMenu  = NULL;
ubyte refreshActiv = 0;
ulong refreshPoss  = 0;





