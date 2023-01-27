/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "planing\main.h"


struct System *plSys = NULL;

char txtTooLoud[20];
char txtTimer[20];
char txtWeight[20];
char txtSeconds[20];


/* System functions */
void plInit (void)
{
   LIST *l;

   /* Get texts */
   l = txtGoKey(PLAN_TXT, "TXT_TOO_LOUD");
   sprintf(txtTooLoud, "%s", NODE_NAME(LIST_HEAD(l)));
   RemoveList(l);

   l = txtGoKey(PLAN_TXT, "TXT_TIMER");
   sprintf(txtTimer, "%s", NODE_NAME(LIST_HEAD(l)));
   RemoveList(l);

   l = txtGoKey(PLAN_TXT, "TXT_WEIGHT");
   sprintf(txtWeight, "%s", NODE_NAME(LIST_HEAD(l)));
   RemoveList(l);

   l = txtGoKey(PLAN_TXT, "TXT_SECONDS");
   sprintf(txtSeconds, "%s", NODE_NAME(LIST_HEAD(l)));
   RemoveList(l);

   plSys = InitSystem();
}

void plDone(void)
{
   CloseSystem(plSys);
}

