/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "SDL.h"
#include "inphdl\inphdl.h"

#include "inphdl\arrow1x_xpm.c"
#include "inphdl\arrow2x_xpm.c"

#include "sound\newsound.h"
#include "sound\fx.h"

static SDL_Cursor *cursor[2];

static ubyte cursorInit;
ubyte initMouseCursor(void);
void removeMouseCursor(void);

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
};

struct IHandler IHandler;

static SDL_Cursor *init_system_cursor(char *image[])
{
  int i, row, col;
  Uint8 data[4*32];
  Uint8 mask[4*32];
  int hot_x, hot_y;

  i = -1;
  for (row=0; row<32; ++row) {
    for (col=0; col<32; ++col) {
      if (col % 8) {
        data[i] <<= 1;
        mask[i] <<= 1;
      } else {
        ++i;
        data[i] = mask[i] = 0;
      }
      switch (image[4+row][col]) {
        case 'X':
          data[i] |= 0x01;
          mask[i] |= 0x01;
          break;
        case '.':
          mask[i] |= 0x01;
          break;
        case ' ':
          break;
      }
    }
  }
  sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
  return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

void setMouseCursor(ubyte fact)
{
	if (cursorInit)
	{
		switch (fact)
		{
		case 1:
		case 2:
			SDL_SetCursor(cursor[0]);
			break;

		case 3:
		case 4:
			SDL_SetCursor(cursor[1]);
			break;
		}
		SDL_ShowCursor(1);
	}
}

ubyte initMouseCursor(void)
{
	if (!cursorInit)
	{
		cursor[0] = init_system_cursor(arrow1x);

		if (!cursor[0])
		{
			Log("SDL_CreateCursor -> %s", SDL_GetError());
			return 0;
		}

		cursor[1] = init_system_cursor(arrow2x);

		if (!cursor[1])
		{
			Log("SDL_CreateCursor -> %s", SDL_GetError());
			return 0;
		}
	}
	return 1;
}

void removeMouseCursor(void)
{
	if (cursorInit)
	{
		SDL_ShowCursor(0);
		SDL_FreeCursor(cursor[0]);
		SDL_FreeCursor(cursor[1]);

	}
}

/* 2014-01-10 LucyG: Joystick */
static SDL_Joystick *inpJoystick = NULL;

static void inpInitJoystick(void)
{
	int numJoys, i;

	inpJoystick = NULL;
	if (!Config.UseJoystick) {
		return;
	}

	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK)) {
		Log("Failed to init joystick subsystem.");
		Config.UseJoystick = 0;
		return;
	}

	numJoys = SDL_NumJoysticks();
	if (numJoys <= 0) {
		Log("No joystick detected.");
		Config.UseJoystick = 0;
		return;
	}

	if (Config.UseJoystick <= numJoys) {
		inpJoystick = SDL_JoystickOpen(Config.UseJoystick - 1);
		if (inpJoystick) {
			SDL_JoystickEventState(SDL_ENABLE);
			return;
		}
	}

	Log("Failed to open joystick #%d. Please try a different one.", Config.UseJoystick);
	for (i = 0; i < numJoys; i++) {
		Log("#%d = \"%s\"", i+1, SDL_JoystickName(i));
	}
	Config.UseJoystick = 0;
}

static void inpQuitJoystick(void)
{
	if (inpJoystick) {
		SDL_JoystickEventState(SDL_IGNORE);
		SDL_JoystickClose(inpJoystick);
		inpJoystick = NULL;
	}
}

void inpOpenAllInputDevs(void)
{

	inpSetKeyRepeat((1<<5) | 10);

	IHandler.uch_EscStatus = 1;
	IHandler.uch_FunctionKeyStatus = 1;
	IHandler.uch_MouseStatus = 1;

	//2014-07
	cursorInit = initMouseCursor();
	setMouseCursor(gfxScalingFactor);

	if (Config.UseJoystick) {
		inpInitJoystick();
	}

}

static void inpDoPseudoMultiTasking(void)
{
	// 2014-06-30 LucyG : rewritten
	static unsigned long timePrev = 0;
	unsigned long timeNow;
	unsigned long timePassed;
	timeNow = SDL_GetTicks();
	if (!timePrev) {
		timePrev = timeNow;
	}
	timePassed = timeNow - timePrev;
	if (timePassed >= INP_TICKS_TO_MS(1)) {
		timePrev = timeNow;

		sndDoFading(); // 2014-07-17 LucyG

		animator();
	}
}

void inpCloseAllInputDevs(void)
{
	// remove mouse cursor
	removeMouseCursor();

	if (Config.UseJoystick) {
		inpQuitJoystick();
	}
}

void inpDelay(long l_Ticks)
{
	// 2014-07-03 LucyG : rewritten
	unsigned long timePrev;
	timePrev = SDL_GetTicks();
	l_Ticks = INP_TICKS_TO_MS(l_Ticks);
	while ((SDL_GetTicks() - timePrev) < l_Ticks) {
		wfr();
		inpDoPseudoMultiTasking();
	}
}

void inpGetMouseXY(struct RastPort *p_RP, uword *p_X, uword *p_Y)
{
	*p_X = IHandler.us_MouseX - p_RP->us_LeftEdge;
	*p_Y = IHandler.us_MouseY - p_RP->us_TopEdge;
}

uword inpGetMouseY(struct RastPort *p_RP)
{
	return(IHandler.us_MouseY) - p_RP->us_TopEdge;
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

long inpWaitFor(long l_Mask)
{
	long action = 0;
	ulong WaitTime = 0;

	unsigned long timePrev = SDL_GetTicks();

	SDL_Event event;
	SDLKey sym = (SDLKey)0;

	if (IHandler.uch_EscStatus && !(l_Mask & INP_NO_ESC)) l_Mask |= INP_ESC;
	if (IHandler.uch_FunctionKeyStatus)	l_Mask |= INP_FUNCTION_KEY;

	while (!action)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					sym = event.key.keysym.sym;
					if ((l_Mask & INP_LEFT) && (sym == SDLK_LEFT)) {
						action |= INP_KEYBOARD|INP_LEFT;
					} else if ((l_Mask & INP_RIGHT) && (sym == SDLK_RIGHT)) {
						action |= INP_KEYBOARD|INP_RIGHT;
					}
					if ((l_Mask & INP_UP) && (sym == SDLK_UP)) {
						action |= INP_KEYBOARD|INP_UP;
					} else if ((l_Mask & INP_DOWN) && (sym == SDLK_DOWN)) {
						action |= INP_KEYBOARD|INP_DOWN;
					}
				break;
				case SDL_KEYUP:
					sym = event.key.keysym.sym;
					if ((l_Mask & (INP_LBUTTONP | INP_LBUTTONR)) && ((sym == SDLK_SPACE) || (sym == SDLK_RETURN))) {
						action |= INP_KEYBOARD|INP_LBUTTONP;
					}
					if (IHandler.uch_EscStatus && (l_Mask & INP_ESC) && (sym == SDLK_ESCAPE)) {
						action |= INP_KEYBOARD|INP_ESC;
					}
					if (IHandler.uch_FunctionKeyStatus && (l_Mask & INP_FUNCTION_KEY) && ((sym >= SDLK_F1) && (sym <= SDLK_F11))) {
						action |= INP_KEYBOARD|INP_FUNCTION_KEY;
					}
					switch (sym) {
						case SDLK_F12: {
							gfxScreenshot();	// 2014-07-13 LucyG : guess what F12 does
						} break;
						case SDLK_INSERT: {
							Config.MusicVolume += 25;
							if (Config.MusicVolume > SND_MAX_VOLUME) {
								Config.MusicVolume = SND_MAX_VOLUME;
							}
						} break;
						case SDLK_DELETE: {
							Config.MusicVolume -= 25;
							if (Config.MusicVolume < 0) {
								Config.MusicVolume = 0;
							}
						} break;
						case SDLK_HOME: {
							Config.SfxVolume += 25;
							if (Config.SfxVolume > SND_MAX_VOLUME) {
								Config.SfxVolume = SND_MAX_VOLUME;
							}
						} break;
						case SDLK_END: {
							Config.SfxVolume -= 25;
							if (Config.SfxVolume < 0) {
								Config.SfxVolume = 0;
							}
						} break;
						case SDLK_PAGEUP: {
							Config.VoiceVolume += 25;
							if (Config.VoiceVolume > SND_MAX_VOLUME) {
								Config.VoiceVolume = SND_MAX_VOLUME;
							}
						} break;
						case SDLK_PAGEDOWN: {
							Config.VoiceVolume -= 25;
							if (Config.VoiceVolume < 0) {
								Config.VoiceVolume = 0;
							}
						} break;
					}
				break;
				case SDL_MOUSEMOTION:
					event.motion.x = (event.motion.x - gfxScalingOffsetX) / gfxScalingFactor;
					event.motion.y = (event.motion.y - gfxScalingOffsetY) / gfxScalingFactor;
					if ((l_Mask & INP_LEFT) && (event.motion.x < IHandler.us_MouseX)) {
						action |= INP_MOUSE|INP_LEFT;
					} else if ((l_Mask & INP_RIGHT) && (event.motion.x > IHandler.us_MouseX)) {
						action |= INP_MOUSE|INP_RIGHT;
					}
					if ((l_Mask & INP_UP) && (event.motion.y < IHandler.us_MouseY)) {
						action |= INP_MOUSE|INP_UP;
					} else if ((l_Mask & INP_DOWN) && (event.motion.y > IHandler.us_MouseY)) {
						action |= INP_MOUSE|INP_DOWN;
					}
					IHandler.us_MouseX = event.motion.x;
					IHandler.us_MouseY = event.motion.y;					
				break;
				case SDL_MOUSEBUTTONDOWN:
					if ((l_Mask & INP_LBUTTONP) && (event.button.button == SDL_BUTTON_LEFT)) {
						action |= INP_MOUSE|INP_LBUTTONP;
					}
					if ((l_Mask & INP_RBUTTONP) && (event.button.button == SDL_BUTTON_RIGHT)) {
						action |= INP_MOUSE|INP_RBUTTONP;
					}
					IHandler.us_MouseX = (event.button.x - gfxScalingOffsetX) / gfxScalingFactor;
					IHandler.us_MouseY = (event.button.y - gfxScalingOffsetY) / gfxScalingFactor;				
				break;
				case SDL_MOUSEBUTTONUP:
					if ((l_Mask & INP_LBUTTONR) && (event.button.button == SDL_BUTTON_LEFT)) {
						action |= INP_MOUSE|INP_LBUTTONR;
					}
					if ((l_Mask & INP_RBUTTONR) && (event.button.button == SDL_BUTTON_RIGHT)) {
						action |= INP_MOUSE|INP_RBUTTONR;
					}
					IHandler.us_MouseX = (event.button.x - gfxScalingOffsetX) / gfxScalingFactor;
					IHandler.us_MouseY = (event.button.y - gfxScalingOffsetY) / gfxScalingFactor;	

					if (event.button.button == SDL_BUTTON_WHEELUP)
					{
						action |= INP_MOUSEWHEEL | INP_UP;
					}
					if (event.button.button == SDL_BUTTON_WHEELDOWN)
					{
						action |= INP_MOUSEWHEEL | INP_DOWN;
					}
				break;
				case SDL_JOYAXISMOTION:	/* 2015-01-10 LucyG: emulate cursor keys with joystick */
					if (event.jaxis.axis == 0) {	// X axis
						if ((l_Mask & INP_LEFT) && (event.jaxis.value < -10000)) {
							action |= INP_KEYBOARD|INP_LEFT;
						} else if ((l_Mask & INP_RIGHT) && (event.jaxis.value > 10000)) {
							action |= INP_KEYBOARD|INP_RIGHT;
						}
					} else if (event.jaxis.axis == 1) {	// Y axis
						if ((l_Mask & INP_UP) && (event.jaxis.value < -10000)) {
							action |= INP_KEYBOARD|INP_UP;
						} else if ((l_Mask & INP_DOWN) && (event.jaxis.value > 10000)) {
							action |= INP_KEYBOARD|INP_DOWN;
						}
					}
				break;
				case SDL_JOYBUTTONUP:	/* 2015-01-10 LucyG: emulate RETURN/SPACE with (any) joystick button */
					if (l_Mask & (INP_LBUTTONP | INP_LBUTTONR)) {
						action |= INP_KEYBOARD|INP_LBUTTONP;
					}
				break;
				case SDL_QUIT:
					if (l_Mask & INP_QUIT) {
						action |= INP_QUIT;
					}
				break;
			}
		}
		inpDoPseudoMultiTasking();
		wfd();

		WaitTime = SDL_GetTicks() - timePrev;
		if ((l_Mask & INP_TIME) && (WaitTime >= INP_TICKS_TO_MS(IHandler.ul_WaitTicks))) action |= INP_TIME;
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
	//int x = (((rate & 0xe0) >> 5) + 1) << 7;
	//int y = ((rate & 0x1f) + 1) << 1;
	int x = (((rate & 0xe0) >> 5) + 1) << 8;
	int y = ((rate & 0x1f) + 1) << 4;
	SDL_EnableKeyRepeat(x, y);
}
