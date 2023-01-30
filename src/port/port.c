/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "port/port.h"

#include "SDL_endian.h"

/**
 * Convert little to native endian word.
 */
void EndianW(uword *pw)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    *pw = SDL_Swap16(*pw);
#endif
}

/**
 * Convert little to native endian int32_t.
 */
void EndianL(uint32_t *pl)
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    *pl = SDL_Swap32(*pl);
#endif
}
