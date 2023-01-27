/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_ORGANISATION
#define MODULE_ORGANISATION

#include "theclou.h"

#ifndef MODULE_LIST
#include "list\list.h"
#endif

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_TEXT
#include "text\text.h"
#endif

#ifndef MODULE_GFX
#include "gfx\gfx.h"
#endif

#ifndef MODULE_BASE
#include "base\base.h"
#endif

/* Display Areas */

#define   ORG_DISP_ABILITIES   (1L<<0)
#define   ORG_DISP_TOOLS       (1L<<1)

struct Organisation
	  {
	  struct RastPort *rp;

	  ulong     CarID;
	  ulong     DriverID;
	  ulong     BuildingID;

	  ubyte     GuyCount;
	  ubyte     PlacesInCar;

	  char		pad[2];
	  };

extern struct Organisation Organisation;

extern ulong  tcOrganisation(void);
extern void	  tcResetOrganisation(void);

#endif
