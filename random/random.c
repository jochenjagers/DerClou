/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "random\random.h"

void rndInit(void)
{
	#ifndef __COSP__
	randomize();	/* Zufallszahl ueÅber Timer initialisieren */
	#else
	srand((unsigned int)time(NULL));
	#endif
}

ulong CalcRandomNr(ulong l_limit, ulong u_limit)
{
	ulong x = l_limit + (rand() % (u_limit - l_limit));

	return x;
}
