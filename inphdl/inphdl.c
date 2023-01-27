/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "SDL.h"
#include "inphdl\inphdl.h"

struct IHandler
{
	long	ul_XSensitivity;
	long	ul_YSensitivity;
	long	ul_WaitTicks;

	Uint16	us_MouseX;
	Uint16	us_MouseY;

	ubyte	uch_EscStatus;
	ubyte	uch_FunctionKeyStatus;
	ubyte	uch_MouseStatus;
	ubyte	uch_JoyExists;
};

struct IHandler IHandler;

void inpOpenAllInputDevs(void)
{
	IHandler.uch_JoyExists = 0;

	inpSetKeyRepeat((1<<5) | 10);

	IHandler.uch_EscStatus = 1;
	IHandler.uch_FunctionKeyStatus = 1;
	IHandler.uch_MouseStatus = 1;

	inpClearKbBuffer();
}

void inpDoPseudoMultiTasking(void)
{
	animator();
	SDL_Delay(10);
	//SDL_PumpEvents();
}

void inpCloseAllInputDevs(void)
{
}

void inpDelay(long l_Ticks)
{
	while (l_Ticks--)
	{
		gfxWaitTOF();
		inpDoPseudoMultiTasking();
	}
}

void inpGetMouseXY(struct RastPort *p_RP, uword *p_X, uword *p_Y)
{
	*p_X = IHandler.us_MouseX;
	*p_Y = IHandler.us_MouseY;
}

uword inpGetMouseY(struct RastPort *p_RP)
{
	return(IHandler.us_MouseY);
}

void inpSetWaitTicks(long l_Ticks)
{
	IHandler.ul_WaitTicks = l_Ticks;
}

void inpTurnESC(uword us_NewStatus)
{
	IHandler.uch_EscStatus = (ubyte) us_NewStatus;
}

void inpTurnFunctionKey(uword us_NewStatus)
{
	IHandler.uch_FunctionKeyStatus = (ubyte) us_NewStatus;
}

void inpTurnMouse(uword us_NewStatus)
{
	IHandler.uch_MouseStatus = (ubyte) us_NewStatus;
}

void inpClearKbBuffer(void)
{
}

long inpWaitFor(long l_Mask)
{
	long action = 0;
	ulong WaitTime = 0;

	SDL_Event event;
	SDLKey sym = 0;

	if (IHandler.uch_EscStatus && !(l_Mask & INP_NO_ESC)) l_Mask |= INP_ESC;
	if (IHandler.uch_FunctionKeyStatus)	l_Mask |= INP_FUNCTION_KEY;

	while (!action)
	{
		wfd();
		WaitTime++;
		if ((l_Mask & INP_TIME) && (WaitTime >= IHandler.ul_WaitTicks)) action |= INP_TIME;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					sym = event.key.keysym.sym;
					if ((l_Mask & INP_LEFT) && (sym == SDLK_LEFT)) action |= INP_KEYBOARD|INP_LEFT;
					else if ((l_Mask & INP_RIGHT) && (sym == SDLK_RIGHT)) action |= INP_KEYBOARD|INP_RIGHT;
					if ((l_Mask & INP_UP) && (sym == SDLK_UP)) action |= INP_KEYBOARD|INP_UP;
					else if ((l_Mask & INP_DOWN) && (sym == SDLK_DOWN)) action |= INP_KEYBOARD|INP_DOWN;
				break;
				case SDL_KEYUP:
					if (sym == event.key.keysym.sym)
					{
						if ((l_Mask & (INP_LBUTTONP | INP_LBUTTONR)) && ((sym == SDLK_SPACE) || (sym == SDLK_RETURN))) action |= INP_KEYBOARD|INP_LBUTTONP;
						if (IHandler.uch_EscStatus && (l_Mask & INP_ESC) && (sym == SDLK_ESCAPE)) action |= INP_KEYBOARD|INP_ESC;
						if (IHandler.uch_FunctionKeyStatus && (l_Mask & INP_FUNCTION_KEY) && ((sym >= SDLK_F1) && (sym <= SDLK_F12))) action |= INP_KEYBOARD|INP_FUNCTION_KEY;
					}
				break;
				#if 0
				/* mouse support is broken */
				case SDL_MOUSEMOTION:
					if ((l_Mask & INP_LEFT) && (event.motion.x < IHandler.us_MouseX)) action |= INP_MOUSE|INP_LEFT;
					else if ((l_Mask & INP_RIGHT) && (event.motion.x > IHandler.us_MouseX)) action |= INP_MOUSE|INP_RIGHT;
					if ((l_Mask & INP_UP) && (event.motion.y < IHandler.us_MouseY)) action |= INP_MOUSE|INP_UP;
					else if ((l_Mask & INP_DOWN) && (event.motion.y > IHandler.us_MouseY)) action |= INP_MOUSE|INP_DOWN;
					IHandler.us_MouseX = event.motion.x;
					IHandler.us_MouseY = event.motion.y;
				break;
				case SDL_MOUSEBUTTONDOWN:
					if ((l_Mask & INP_LBUTTONP) && (event.button.button == SDL_BUTTON_LEFT)) action |= INP_MOUSE|INP_LBUTTONP;
					if ((l_Mask & INP_RBUTTONP) && (event.button.button == SDL_BUTTON_RIGHT)) action |= INP_MOUSE|INP_RBUTTONP;
					IHandler.us_MouseX = event.button.x;
					IHandler.us_MouseY = event.button.y;
				break;
				case SDL_MOUSEBUTTONUP:
					if ((l_Mask & INP_LBUTTONR) && (event.button.button == SDL_BUTTON_LEFT)) action |= INP_MOUSE|INP_LBUTTONR;
					if ((l_Mask & INP_RBUTTONR) && (event.button.button == SDL_BUTTON_RIGHT)) action |= INP_MOUSE|INP_RBUTTONR;
					IHandler.us_MouseX = event.button.x;
					IHandler.us_MouseY = event.button.y;
				break;
				#endif
				/* currently ignored */
				case SDL_QUIT:
					if (l_Mask & INP_QUIT) action |= INP_QUIT;
				break;
			}
		}
		inpClearKbBuffer();
		inpDoPseudoMultiTasking();
	}

	return(action);
}

/*
 * rate = (x << 5) | y
 * x : Delay (0-3)
 * y : Repeat (0-31)
 */
void inpSetKeyRepeat(unsigned char rate)
{
	/* needs some experimenting... */
	int x = (((rate & 0xe0) >> 5) + 1) << 7;
	int y = ((rate & 0x1f) + 1) << 1;
	SDL_EnableKeyRepeat(x, y);
}
