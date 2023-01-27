/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "data\datacalc.h"

long Round(long v,long p)     /* p...Stellen ! */
     {
     long i,z;

     for(i=0,z=1; i<p; i++,z*=10);

     if(v/z != 0)
          v = (v / z) * z;

     return(v);
     }

long CalcValue(long value, long min, long max, long fact, long perc)
     {
     perc = ((perc * fact) - (perc * 128)) / 127;

     value = value + (value * perc) / 100;

     if(value < min)     value = min;
     if(value > max)     value = max;

     return value;
     }

long ChangeAbs(long item, long value, long min, long max)
     {
     item+=value;

     if(item > max)     item = max;
     if(item < min)     item = min;

     return(item);
     }

long RoundF(float num)
{
    if (num < 0)
		return (long) (num - 0.5f);
	else
		return (long) (num + 0.5f);
}


