/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "landscap\landscap.h"
#include "landscap\landscap_p.h"

uword DX,DY,PX,PY;

ubyte lsScrollLandScape(ubyte direction)
	{
	struct LandScape *l=ls;
	ubyte collis=0, back, speed = ls->uch_ScrollSpeed;

	if (direction != (ubyte)-1)
		collis = lsInitScrollLandScape(direction, LS_SCROLL_PREPARE);

	if (!collis)
		{
		lsScrollCorrectData(DX * speed,DY *speed);

		l->us_PersonXPos += (PX * speed);
		l->us_PersonYPos += (PY * speed);

		back =(ubyte) direction;
		}

	return back;
	}

ubyte lsInitScrollLandScape(ubyte direction, ubyte mode)
	{
	struct LandScape *l=ls;
	long dx=0,dy=0,px=0,py=0,tx=0,ty=0,speed=(long)l->uch_ScrollSpeed;
	ubyte collis=0;

	if (direction & LS_SCROLL_LEFT)
		{
		if ((l->us_WindowXPos >= speed) && (l->us_PersonXPos <= LS_CENTER_X))
			{
			dx=-1L;

			l->us_LivingXSpeed = (word) dx;
			l->us_LivingYSpeed = 0;
			l->uch_LivingAction = ANM_MOVE_LEFT;
			}
		else
			{
			if (l->us_PersonXPos > speed)
				{
				px = -1L;

				l->us_LivingXSpeed = (word) px;
				l->us_LivingYSpeed = 0;
				l->uch_LivingAction = ANM_MOVE_LEFT;
				}
			}
		}

	if (direction & LS_SCROLL_RIGHT)
		{
		if ((l->us_WindowXPos <= (LS_MAX_AREA_WIDTH-LS_VISIBLE_X_SIZE-speed))
			&& (l->us_PersonXPos >= LS_CENTER_X))
			{
			dx=1;

			l->us_LivingXSpeed = (word) dx;
			l->us_LivingYSpeed = 0;
			l->uch_LivingAction = ANM_MOVE_RIGHT;
			}
		else
			{
			if (l->us_PersonXPos < (LS_VISIBLE_X_SIZE- speed))
				{
				px = 1L;

				l->us_LivingXSpeed = (word) px;
				l->us_LivingYSpeed = 0;
				l->uch_LivingAction = ANM_MOVE_RIGHT;
				}
			}
		}

	if (direction & LS_SCROLL_UP)
		{
		if ((l->us_WindowYPos >= (speed)) && (l->us_PersonYPos<=LS_CENTER_Y))
			{
			dy=-1L;

			l->us_LivingXSpeed = 0;
			l->us_LivingYSpeed = (word) dy;
			l->uch_LivingAction = ANM_MOVE_UP;
			}
		else
			{
			if (l->us_PersonYPos > (speed))
				{
				py = -1L;

				l->us_LivingXSpeed = 0;
				l->us_LivingYSpeed = (word) py;
				l->uch_LivingAction = ANM_MOVE_UP;
				}
			}
		}

	if (direction & LS_SCROLL_DOWN)
		{
		if (l->us_WindowYPos <= (LS_MAX_AREA_HEIGHT-LS_VISIBLE_Y_SIZE- speed)
			&& (l->us_PersonYPos>=LS_CENTER_Y))
			{
			dy = 1L;

			l->us_LivingXSpeed = 0;
			l->us_LivingYSpeed = (word) dy;
			l->uch_LivingAction = ANM_MOVE_DOWN;
			}
		else
			{
			if (l->us_PersonYPos < (LS_VISIBLE_Y_SIZE- speed))
				{
				py = 1L;

				l->us_LivingXSpeed = 0;
				l->us_LivingYSpeed = (word) py;
				l->uch_LivingAction = ANM_MOVE_DOWN;
				}
			}
		}

	tx = livGetXPos((char*)l->uch_ActivLiving) + (px + dx) * speed;
	ty = livGetYPos((char*)l->uch_ActivLiving) + (py + dy) * speed;

	collis = lsIsCollision((uword)tx,(uword)ty, direction);

	if (mode & LS_SCROLL_PREPARE)
		{
		DX = dx; DY =dy; PX =px; PY = py;
		}

	return(collis);
	}

ubyte lsIsCollision(uword x, uword y, ubyte direction)
	{
	long i;
	ubyte collis = 0;
	ubyte color[14], collisDir = 0;

	for (i = 0; i < 14; i++)
		color[i] = 255;

	// Kollision ÅberprÅfen
	// Da sich das Maxi immer um 2 Pixel weiterbewegt, mÅssen Pixel
	// hintereinander abgefragt werden, au·erdem wird immer an jeder
	// Ecke einer Kollisionsseite kontrolliert

	switch(direction)
		{
		/*
		case LS_SCROLL_LEFT:
				color[0] = gfxNCH4ReadPixel(l_wrp, x - 2, y - 2);
				color[1] = gfxNCH4ReadPixel(l_wrp, x - 2, y + 0);
				color[2] = gfxNCH4ReadPixel(l_wrp, x - 2, y + 2);
				color[3] = gfxNCH4ReadPixel(l_wrp, x - 2, y + 4);
				color[4] = gfxNCH4ReadPixel(l_wrp, x - 2, y + 6);
				color[5] = gfxNCH4ReadPixel(l_wrp, x - 2, y + 8);
				color[6] = gfxNCH4ReadPixel(l_wrp, x - 2, y + 10);

				color[7] = gfxNCH4ReadPixel(l_wrp, x - 1, y - 2);
				color[8] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 0);
				color[9] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 2);
				color[10] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 4);
				color[11] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 6);
				color[12] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 8);
				color[13] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 10);

				collisDir |= LS_COLLIS_LEFT;
				break;
		case LS_SCROLL_RIGHT:
				color[0] = gfxNCH4ReadPixel(l_wrp, x + 16, y - 2);
				color[1] = gfxNCH4ReadPixel(l_wrp, x + 16, y + 0);
				color[2] = gfxNCH4ReadPixel(l_wrp, x + 16, y + 2);
				color[3] = gfxNCH4ReadPixel(l_wrp, x + 16, y + 4);
				color[4] = gfxNCH4ReadPixel(l_wrp, x + 16, y + 6);
				color[5] = gfxNCH4ReadPixel(l_wrp, x + 16, y + 8);
				color[6] = gfxNCH4ReadPixel(l_wrp, x + 16, y + 10);

				color[7] = gfxNCH4ReadPixel(l_wrp, x + 17, y - 2);
				color[8] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 0);
				color[9] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 2);
				color[10] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 4);
				color[11] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 6);
				color[12] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 8);
				color[13] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 10);

				collisDir |= LS_COLLIS_RIGHT;
				break;
		case LS_SCROLL_UP:
				color[0] = gfxNCH4ReadPixel(l_wrp, x + 1 , y - 3);
				color[1] = gfxNCH4ReadPixel(l_wrp, x + 2 , y - 3);
				color[2] = gfxNCH4ReadPixel(l_wrp, x + 4 , y - 3);
				color[3] = gfxNCH4ReadPixel(l_wrp, x + 6 , y - 3);
				color[4] = gfxNCH4ReadPixel(l_wrp, x + 9 , y - 3);
				color[5] = gfxNCH4ReadPixel(l_wrp, x + 12, y - 3);
				color[6] = gfxNCH4ReadPixel(l_wrp, x + 14, y - 3);

				color[7] = gfxNCH4ReadPixel(l_wrp, x + 1 , y - 4);
				color[8] = gfxNCH4ReadPixel(l_wrp, x + 2 , y - 4);
				color[9] = gfxNCH4ReadPixel(l_wrp, x + 4 , y - 4);
				color[10] = gfxNCH4ReadPixel(l_wrp, x + 6 , y - 4);
				color[11] = gfxNCH4ReadPixel(l_wrp, x + 9 , y - 4);
				color[12] = gfxNCH4ReadPixel(l_wrp, x + 12, y - 4);
				color[13] = gfxNCH4ReadPixel(l_wrp, x + 14, y - 4);
				collisDir |= LS_COLLIS_UP;
				break;
		case LS_SCROLL_DOWN:
				color[0] = gfxNCH4ReadPixel(l_wrp, x + 1 , y + 13);
				color[1] = gfxNCH4ReadPixel(l_wrp, x + 2 , y + 13);
				color[2] = gfxNCH4ReadPixel(l_wrp, x + 4 , y + 13);
				color[3] = gfxNCH4ReadPixel(l_wrp, x + 6 , y + 13);
				color[4] = gfxNCH4ReadPixel(l_wrp, x + 9 , y + 13);
				color[5] = gfxNCH4ReadPixel(l_wrp, x + 12, y + 13);
				color[6] = gfxNCH4ReadPixel(l_wrp, x + 14, y + 13);

				color[7] = gfxNCH4ReadPixel(l_wrp, x + 1 , y + 14);
				color[8] = gfxNCH4ReadPixel(l_wrp, x + 2 , y + 14);
				color[9] = gfxNCH4ReadPixel(l_wrp, x + 4 , y + 14);
				color[10] = gfxNCH4ReadPixel(l_wrp, x + 6 , y + 14);
				color[11] = gfxNCH4ReadPixel(l_wrp, x + 9 , y + 14);
				color[12] = gfxNCH4ReadPixel(l_wrp, x + 12, y + 14);
				color[13] = gfxNCH4ReadPixel(l_wrp, x + 14, y + 14);

				collisDir |= LS_COLLIS_DOWN;
				break;
		*/
		case LS_SCROLL_LEFT:
				color[0]  = gfxNCH4ReadPixel(l_wrp, x - 2, y - 2);
				color[1]  = gfxNCH4ReadPixel(l_wrp, x - 2, y + 0);
				color[2]  = gfxNCH4ReadPixel(l_wrp, x - 2, y + 2);
				color[3]  = gfxNCH4ReadPixel(l_wrp, x - 2, y + 4);
				color[4]  = gfxNCH4ReadPixel(l_wrp, x - 2, y + 6);
				color[5]  = gfxNCH4ReadPixel(l_wrp, x - 2, y + 8);
				color[6]  = gfxNCH4ReadPixel(l_wrp, x - 2, y + 10);

				color[7]  = gfxNCH4ReadPixel(l_wrp, x - 1, y - 2);
				color[8]  = gfxNCH4ReadPixel(l_wrp, x - 1, y + 0);
				color[9]  = gfxNCH4ReadPixel(l_wrp, x - 1, y + 2);
				color[10] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 4);
				color[11] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 6);
				color[12] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 8);
				color[13] = gfxNCH4ReadPixel(l_wrp, x - 1, y + 10);

				collisDir |= LS_COLLIS_LEFT;
				break;
		case LS_SCROLL_RIGHT:
				color[0]  = gfxNCH4ReadPixel(l_wrp, x + 16, y - 2);
				color[1]  = gfxNCH4ReadPixel(l_wrp, x + 16, y + 0);
				color[2]  = gfxNCH4ReadPixel(l_wrp, x + 16, y + 2);
				color[3]  = gfxNCH4ReadPixel(l_wrp, x + 16, y + 4);
				color[4]  = gfxNCH4ReadPixel(l_wrp, x + 16, y + 6);
				color[5]  = gfxNCH4ReadPixel(l_wrp, x + 16, y + 8);
				color[6]  = gfxNCH4ReadPixel(l_wrp, x + 16, y + 10);

				color[7]  = gfxNCH4ReadPixel(l_wrp, x + 17, y - 2);
				color[8]  = gfxNCH4ReadPixel(l_wrp, x + 17, y + 0);
				color[9]  = gfxNCH4ReadPixel(l_wrp, x + 17, y + 2);
				color[10] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 4);
				color[11] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 6);
				color[12] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 8);
				color[13] = gfxNCH4ReadPixel(l_wrp, x + 17, y + 10);

				collisDir |= LS_COLLIS_RIGHT;
				break;
		case LS_SCROLL_UP:
				color[0]  = gfxNCH4ReadPixel(l_wrp, x + 3 , y - 3);
				color[1]  = gfxNCH4ReadPixel(l_wrp, x + 3 , y - 3);
				color[2]  = gfxNCH4ReadPixel(l_wrp, x + 4 , y - 3);
				color[3]  = gfxNCH4ReadPixel(l_wrp, x + 6 , y - 3);
				color[4]  = gfxNCH4ReadPixel(l_wrp, x + 9 , y - 3);
				color[5]  = gfxNCH4ReadPixel(l_wrp, x + 11, y - 3);
				color[6]  = gfxNCH4ReadPixel(l_wrp, x + 11, y - 3);

				color[7]  = gfxNCH4ReadPixel(l_wrp, x + 3 , y - 4);
				color[8]  = gfxNCH4ReadPixel(l_wrp, x + 3 , y - 4);
				color[9]  = gfxNCH4ReadPixel(l_wrp, x + 4 , y - 4);
				color[10] = gfxNCH4ReadPixel(l_wrp, x + 6 , y - 4);
				color[11] = gfxNCH4ReadPixel(l_wrp, x + 9 , y - 4);
				color[12] = gfxNCH4ReadPixel(l_wrp, x + 11, y - 4);
				color[13] = gfxNCH4ReadPixel(l_wrp, x + 11, y - 4);

				collisDir |= LS_COLLIS_UP;
				break;
		case LS_SCROLL_DOWN:
				color[0]  = gfxNCH4ReadPixel(l_wrp, x + 3 , y + 13);
				color[1]  = gfxNCH4ReadPixel(l_wrp, x + 3 , y + 13);
				color[2]  = gfxNCH4ReadPixel(l_wrp, x + 4 , y + 13);
				color[3]  = gfxNCH4ReadPixel(l_wrp, x + 6 , y + 13);
				color[4]  = gfxNCH4ReadPixel(l_wrp, x + 9 , y + 13);
				color[5]  = gfxNCH4ReadPixel(l_wrp, x + 11, y + 13);
				color[6]  = gfxNCH4ReadPixel(l_wrp, x + 11, y + 13);

				color[7]  = gfxNCH4ReadPixel(l_wrp, x + 3 , y + 14);
				color[8]  = gfxNCH4ReadPixel(l_wrp, x + 3 , y + 14);
				color[9]  = gfxNCH4ReadPixel(l_wrp, x + 4 , y + 14);
				color[10] = gfxNCH4ReadPixel(l_wrp, x + 6 , y + 14);
				color[11] = gfxNCH4ReadPixel(l_wrp, x + 9 , y + 14);
				color[12] = gfxNCH4ReadPixel(l_wrp, x + 11, y + 14);
				color[13] = gfxNCH4ReadPixel(l_wrp, x + 11, y + 14);

				collisDir |= LS_COLLIS_DOWN;
				break;
		default:
				break;
		}

	// ab jetzt enthaelt color[i] einen Wert ungleich 0, wenn Kollision vorliegt
	for (i = 0; i < 14; i++) {
		if ((color[i] != LS_COLLIS_COLOR_0) && (color[i] != LS_COLLIS_COLOR_1) &&
		    (color[i] != LS_COLLIS_COLOR_2) && (color[i] != LS_COLLIS_COLOR_3)) {
			color[i] = 0;
		} else {
			/* 2014-06-27 templer
			there is no collision on background color
			This is because that in the query a value of 0 means no collision
			To fix this, color[i] must be set on color "0" to a value greater than zero
			*/
			color[i] = 2;
		}
	}
	for (i = 0; i < 14; i++) {
		if (color[i]) {
			collis = collisDir;
		}
	}
	// wenn die aeusseren kollidieren, die inneren aber nicht -> keine Kollision (TÅr!)
	if ((color[0] || color[1]) && (color[6] || color[5]) && !color[3])
		collis = 0;

	if ((color[7] || color[8]) && (color[13] || color[12]) && !color[10])
		collis = 0;

	// Raender ueÅberprueÅfen!
	if (x <= 2)
		collis |= LS_COLLIS_LEFT;
	if (x >= (LS_MAX_AREA_WIDTH - LS_PERSON_X_SIZE - 3))
		collis |=LS_COLLIS_RIGHT;

	if (y <= 2)
		collis |= LS_COLLIS_UP;
	if (y >= (LS_MAX_AREA_HEIGHT - LS_PERSON_Y_SIZE -3))
		collis |= LS_COLLIS_DOWN;

	if (GamePlayMode & GP_COLLISION_CHECKING_OFF)
		collis = 0;

	return collis;
	}
