/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

#define BAR_WIDTH         	 255

struct PresentControl
     {
     struct RastPort *p_CurrRP;

     uword     us_BarWidth;
     uword     us_BarHeight;

     ubyte     uch_FCol;
	  ubyte     uch_BCol;
	  ubyte		uch_TCol;
     };

struct presentationInfo
	{
	NODE link;

	char  extendedText[70];
	ulong extendedNr;
	ulong maxNr;

	ubyte presentHow;
	};




