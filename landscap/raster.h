/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_RASTER
#define MODULE_RASTER

#include "theclou.h"

#ifndef MODULE_DIALOG
#include "dialog\dialog.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap\landscap.h"
#endif

#define LS_RASTER_DISP_WIDTH       320
#define LS_RASTER_DISP_HEIGHT      140

#define LS_RASTER_X_SIZE           4    /* Raster is used for collision */
#define LS_RASTER_Y_SIZE           4    /* dedection, in pixel */


NODE 	  *lsGetSuccObject(NODE *start);
NODE 	  *lsGetPredObject(NODE *start);
void  	lsFadeRasterObject(ulong areaID, LSObject lso, ubyte status);
void  	lsShowAllConnections(ulong areaID, NODE *node, ubyte perc);
void  	lsShowRaster(ulong areaID, ubyte perc);

#endif
