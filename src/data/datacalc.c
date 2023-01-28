/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "data/datacalc.h"

int32_t Round(int32_t v, int32_t p) /* p...Stellen ! */
{
    int32_t i = 0, z = 0;

    for (i = 0, z = 1; i < p; i++, z *= 10)
        ;

    if (v / z != 0) v = (v / z) * z;

    return (v);
}

int32_t CalcValue(int32_t value, int32_t min, int32_t max, int32_t fact, int32_t perc)
{
    perc = ((perc * fact) - (perc * 128)) / 127;

    value = value + (value * perc) / 100;

    if (value < min) value = min;
    if (value > max) value = max;

    return value;
}

int32_t ChangeAbs(int32_t item, int32_t value, int32_t min, int32_t max)
{
    item += value;

    if (item > max) item = max;
    if (item < min) item = min;

    return (item);
}

int32_t RoundF(float num)
{
    if (num < 0)
        return (int32_t)(num - 0.5f);
    else
        return (int32_t)(num + 0.5f);
}
