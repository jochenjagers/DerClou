/*	_________  _______
   / ___/ __ \/ __/ _ \		 Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#define FILE_SYSTEM_ID		"SYS "		/* SYStem plan start */
#define FILE_HANDLER_ID		"HAND"		/* HANDler x needed	 */
#define FILE_ACTION_LIST_ID "ACLI"		/* ACtionLIst for handler x started */
#define FILE_ACTION_ID		"ACTI"		/* ACTIon */

#define SYS_MAX_MEMORY_SIZE	  1024L*25L

#include "planing\system.h"
#include "port\port.h"

ulong sysUsedMem = 0L;

ulong sizeofAction [] = {
	0L, /* first index is not used yet */
	(ulong)sizeof(struct ActionGo),
	0L,
	(ulong)sizeof(struct ActionSignal),
	(ulong)sizeof(struct ActionWaitSignal),
	(ulong)sizeof(struct ActionUse),
	(ulong)sizeof(struct ActionTake),
	(ulong)sizeof(struct ActionDrop),
	(ulong)sizeof(struct ActionOpen),
	(ulong)sizeof(struct ActionClose),
	(ulong)sizeof(struct ActionControl)
};

ulong plGetUsedMem(void)
	{
	return sysUsedMem;
	}

struct Handler *FindHandler(struct System *sys, ulong id)
{
   struct Handler *h;

   if (sys)
   {
	  for (h = (struct Handler *)LIST_HEAD(sys->Handlers); NODE_SUCC(h); h = (struct Handler *)NODE_SUCC(h))
	  {
		 if (h->Id == id)
			return h;
	  }
   }

   return NULL;
}

struct System *InitSystem(void)
{
   struct System *sys = (struct System *)MemAlloc(sizeof(struct System));

   if (sys)
   {
	  sys->Handlers = (LIST*)CreateList(0L);
	  sys->Signals	= (LIST*)CreateList(0L);

	  sys->ActivHandler = NULL;

	  if (!sys->Handlers || !sys->Signals)
	  {
		 CloseSystem(sys);
		 sys = NULL;
	  }
   }

   return sys;
}

void CloseSystem(struct System *sys)
{
   if (sys)
   {
	  RemoveList(sys->Handlers);
	  RemoveList(sys->Signals);

	  MemFree(sys, sizeof(struct System));
   }
}

void SetActivHandler(struct System *sys, ulong id)
{
   struct Handler *h;

   if (h = FindHandler(sys, id))
	  sys->ActivHandler = (NODE *)h;
   else
	  sys->ActivHandler = NULL;
}

void SaveSystem(FILE *fh, struct System *sys)
{
   struct Handler *h;

   if (fh)
   {
	  fprintf(fh, FILE_SYSTEM_ID "\n");

	  for (h = (struct Handler *)LIST_HEAD(sys->Handlers); NODE_SUCC(h); h = (struct Handler *)NODE_SUCC(h))
		 fprintf(fh, FILE_HANDLER_ID "\n%ld\n", h->Id);
   }
}

LIST *LoadSystem(FILE *fh, struct System *sys)
{
   LIST *l = txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_1");
	ubyte  foundAll = 1, knowsSomebody = 1, handlerNr = 0;
	char buffer[32];

	if (fh)
	{
		if (dskGets(buffer, 31, fh))
		{
			if (!strcmp(buffer, FILE_SYSTEM_ID))
			{
				while (dskGets(buffer, 31, fh))
				{
					if (strcmp(buffer, FILE_HANDLER_ID)) break;

					dskGets(buffer, 19, fh); /* reading handler id */

					if (atol(buffer) && !dbIsObject(atol(buffer), Object_Police))
					{
						handlerNr++;

						if (!FindHandler(sys, atol(buffer)))
						{
							if (knows(Person_Matt_Stuvysunt, atol (buffer)))
							{
								knowsSomebody++;
								dbAddObjectNode(l, atol(buffer), OLF_INCLUDE_NAME);
							}

							foundAll = 0;
						}
					}
				}
			}
		}
	}

	if (foundAll)
	{
		RemoveList(l);
		l = NULL;
	}
	else
	{
		LIST *extList = NULL;
		NODE *n;

		if (knowsSomebody == 1)
			extList = txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_3");
		else
		{
			if ((handlerNr - knowsSomebody) > 1)
				extList = txtGoKeyAndInsert(PLAN_TXT, "SYSTEM_GUYS_MISSING_2", (ulong)(handlerNr - knowsSomebody));
			else if (handlerNr - knowsSomebody)
				extList = txtGoKey(PLAN_TXT, "SYSTEM_GUYS_MISSING_4");
		}

		if (extList)
		{
			for (n = (NODE *)LIST_HEAD(extList); NODE_SUCC(n); n = (NODE *)NODE_SUCC(n))
				CreateNode(l, 0, NODE_NAME(n));

			RemoveList(extList);
		}
	}

	return l;
}

struct Handler *InitHandler(struct System *sys, ulong id, ulong flags)
{
   struct Handler *h = NULL;

   if (sys && !FindHandler(sys, id))
   {
	  if (h = (struct Handler *)CreateNode(sys->Handlers, sizeof (struct Handler), NULL))
	  {
		 h->Id			  = id;
		 h->Timer		  = 0L;
		 h->Flags		  = flags;
		 h->CurrentAction = NULL;

		 if (!(h->Actions = (LIST*)CreateList(0L)))
		 {
			RemNode(h);
			FreeNode(h);
			h = NULL;
		 }

		 sys->ActivHandler = (NODE *)h;
	  }
   }

   return h;
}

void CloseHandler(struct System *sys, ulong id)
{
   struct Handler *h;

   if (h = FindHandler(sys, id))
   {
	  CorrectMem(h->Actions);

	  if (h->Actions)
		 RemoveList(h->Actions);

	  RemNode(h);
	  FreeNode(h);
   }
}

struct Handler *ClearHandler(struct System *sys, ulong id)
{
   struct Handler *h;

   if (h = FindHandler(sys, id))
   {
	  CorrectMem(h->Actions);

	  if (h->Actions)
		 RemoveList(h->Actions);

	  if (!(h->Actions = (LIST*)CreateList(0L)))
	  {
		 RemNode(h);
		 FreeNode(h);
		 h = NULL;
	  }

	  h->Timer		   = 0L;
	  h->CurrentAction = NULL;
   }

   return h;
}

ubyte IsHandlerCleared(struct System *sys)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if (LIST_EMPTY(h->Actions))
		 return 1;
   }

   return 0;
}

void SaveHandler(FILE *fh, struct System *sys, ulong id)
{
   struct Handler *h;
   struct Action  *a;

   if (fh && sys && (h = FindHandler(sys, id)))
   {
	  fprintf(fh, FILE_ACTION_LIST_ID "\n%ld\n", id);

	  for (a = (struct Action *)LIST_HEAD(h->Actions); NODE_SUCC(a); a = (struct Action *)NODE_SUCC(a))
	  {
			fprintf(fh, FILE_ACTION_ID "\n%d\n%d\n", a->Type, a->TimeNeeded);

			switch (a->Type)
			{
				case ACTION_GO:
					fprintf(fh, "%d\n", (ulong)ActionData(a, struct ActionGo *)->Direction);
				break;

				case ACTION_USE:
				case ACTION_TAKE:
				case ACTION_DROP:
					fprintf(fh, "%ld\n%ld\n", ActionData(a, struct ActionUse *)->ToolId,
													  ActionData(a, struct ActionUse *)->ItemId);
				break;

			case ACTION_OPEN:
			case ACTION_CLOSE:
			case ACTION_CONTROL:
			case ACTION_SIGNAL:
			case ACTION_WAIT_SIGNAL:
			   fprintf(fh, "%ld\n", ActionData(a, struct ActionOpen *)->ItemId);
			break;
		 }
	  }
   }
}

ubyte LoadHandler(FILE *fh, struct System *sys, ulong id)
{
	struct Action  *a;
	ulong value;
	ulong time;
	char buffer[32];

	if (fh && sys && (FindHandler(sys, id))) {
		//rewind(fh);
		fseek(fh, 0, SEEK_SET);	// LucyG 2017-10-29

		while (!dskIsEOF(fh)) {
			dskGets(buffer, 31, fh);

			if (!strcmp(buffer, FILE_ACTION_LIST_ID)) {
				dskGets(buffer, 31, fh); /* reading handler id */

				if (id == atol(buffer)) {
					SetActivHandler(sys, id);

					while (dskGets(buffer, 31, fh)) {
						if (strcmp(buffer, FILE_ACTION_ID)) break;

						dskGets(buffer, 31, fh);
						value = atol(buffer);

						dskGets(buffer, 31, fh);
						time = atol(buffer);

						if (value) {
							a = InitAction(sys, value, 0L, 0L, time);

							switch (value) {
								case ACTION_GO:
									dskGets(buffer, 31, fh);
									value = atol(buffer);
									ActionData(a, struct ActionGo *)->Direction = value;
								break;
								case ACTION_USE:
								case ACTION_TAKE:
								case ACTION_DROP:
									dskGets(buffer, 31, fh);
									value = atol(buffer);
									ActionData(a, struct ActionUse *)->ToolId = value;

									dskGets(buffer, 31, fh);
									value = atol(buffer);
									ActionData(a, struct ActionUse *)->ItemId = value;
								break;
								case ACTION_OPEN:
								case ACTION_CLOSE:
								case ACTION_CONTROL:
								case ACTION_WAIT_SIGNAL:
								case ACTION_SIGNAL:
									dskGets(buffer, 31, fh);
									value = atol(buffer);
									ActionData(a, struct ActionOpen *)->ItemId = value;
								break;
							}
						} else {
							return 0;
						}
					}

					GoFirstAction(sys);
					return 1;
				}
			}
		}
	}
	return 0;
}

struct Action *InitAction(struct System *sys, uword type, ulong data1, ulong data2, ulong time)
{
   struct Handler *h;
   struct Action  *a = NULL;

	if (sys && (h = (struct Handler *)sys->ActivHandler))
	{
		if ((sysUsedMem + sizeof(struct Action) + sizeofAction[type]) <= SYS_MAX_MEMORY_SIZE)
	  {
		 sysUsedMem += sizeof(struct Action) + sizeofAction[type];

		 if (a = (struct Action *)CreateNode(h->Actions, sizeof(struct Action) + sizeofAction[type], NULL))
		 {
			a->Type		  = type;
			a->TimeNeeded = time;
			a->Timer	  = time;

			h->Timer		 += time;
			h->CurrentAction  = (NODE *)a;

			switch (type)
			{
			   case ACTION_GO:
				  ActionData(a, struct ActionGo *)->Direction = (uword)data1;
			   break;

			   case ACTION_USE:
			   case ACTION_TAKE:
			   case ACTION_DROP:
				  ActionData(a, struct ActionUse *)->ItemId = data1;
				  ActionData(a, struct ActionUse *)->ToolId = data2;
			   break;

			   case ACTION_SIGNAL:
			   case ACTION_WAIT_SIGNAL:
			   case ACTION_OPEN:
			   case ACTION_CLOSE:
			   case ACTION_CONTROL:
				  ActionData(a, struct ActionOpen *)->ItemId = data1;
			   break;
			}
		 }
	  }
   }

   return a;
}

struct Action *CurrentAction(struct System *sys)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
	  return (struct Action *)h->CurrentAction;

   return NULL;
}

struct Action *GoFirstAction(struct System *sys)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if (!LIST_EMPTY(h->Actions))
	  {
		 h->CurrentAction = (NODE *)LIST_HEAD(h->Actions);
		 ((struct Action *)h->CurrentAction)->Timer = 0;
		 h->Timer = 0;
	  }
	  else
		 h->CurrentAction = NULL;

	  return (struct Action *)h->CurrentAction;
   }

   return NULL;
}

struct Action *GoLastAction(struct System *sys)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if (!LIST_EMPTY(h->Actions))
	  {
		 h->CurrentAction = (NODE *)LIST_TPRED(h->Actions);
		 ((struct Action *)h->CurrentAction)->Timer = ((struct Action *)h->CurrentAction)->TimeNeeded;
		 h->Timer = GetMaxTimer(sys);
	  }
	  else
		 h->CurrentAction = NULL;

	  return (struct Action *)h->CurrentAction;
   }

   return NULL;
}

struct Action *NextAction(struct System *sys)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if ((struct Action *)h->CurrentAction)
	  {
		 if (((struct Action *)h->CurrentAction)->Timer == ((struct Action *)h->CurrentAction)->TimeNeeded)
		 {
			if (NODE_SUCC(NODE_SUCC(h->CurrentAction)))
			{
			   h->CurrentAction = (NODE *)NODE_SUCC(h->CurrentAction);

			   ((struct Action *)h->CurrentAction)->Timer = 1;
			   h->Timer++;
			}
			else
			{
			   if (h->Flags & SHF_AUTOREVERS)
			   {
				  h->CurrentAction = (NODE *)LIST_HEAD(h->Actions);
				  ((struct Action *)h->CurrentAction)->Timer = 1;
				  h->Timer++;
			   }
			   else
			   {
				  h->CurrentAction = (NODE *)LIST_TPRED(h->Actions);
				  ((struct Action *)h->CurrentAction)->Timer = ((struct Action *)h->CurrentAction)->TimeNeeded;
				  h->Timer = GetMaxTimer(sys);

				  return NULL;
			   }
			}
		 }
		 else
		 {
			((struct Action *)h->CurrentAction)->Timer++;
			h->Timer++;
		 }
	  }

	  return (struct Action *)h->CurrentAction;
   }

   return NULL;
}

struct Action *PrevAction(struct System *sys)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if ((struct Action *)h->CurrentAction)
	  {
		 if (((struct Action *)h->CurrentAction)->Timer == 1)
		 {
			if (NODE_PRED(NODE_PRED(h->CurrentAction)))
			{
			   h->CurrentAction = (NODE *)NODE_PRED(h->CurrentAction);

			   ((struct Action *)h->CurrentAction)->Timer = ((struct Action *)h->CurrentAction)->TimeNeeded;
			   h->Timer--;
			}
			else
			{
			   if ((h->Flags & SHF_AUTOREVERS) && h->Timer)
			   {
				  h->CurrentAction = (NODE *)LIST_TPRED(h->Actions);
				  ((struct Action *)h->CurrentAction)->Timer = ((struct Action *)h->CurrentAction)->TimeNeeded;
				  h->Timer--;
			   }
			   else
			   {
				  h->CurrentAction = (NODE *)LIST_HEAD(h->Actions);
				  ((struct Action *)h->CurrentAction)->Timer = 0;
				  h->Timer = 0;

				  return NULL;
			   }
			}
		 }
		 else
		 {
			((struct Action *)h->CurrentAction)->Timer--;
			h->Timer--;
		 }
	  }

	  return (struct Action *)h->CurrentAction;
   }

   return NULL;
}

ubyte ActionStarted(struct System *sys)
{
   struct Handler *h;
   struct Action  *a;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if (a = (struct Action *)h->CurrentAction)
	  {
		 if (a->Timer == 1)
			return 1;
	  }
   }

   return 0;
}

ubyte ActionEnded(struct System *sys)
{
   struct Handler *h;
   struct Action  *a;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if (a = (struct Action *)h->CurrentAction)
	  {
		 if (a->Timer == a->TimeNeeded)
			return 1;
	  }
   }

   return 0;
}

void RemLastAction(struct System *sys)
{
   struct Handler *h;
	NODE *n;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if (!LIST_EMPTY(h->Actions))
	  {
		 if (GetNrOfNodes(h->Actions) > 1)
		 {
			n = (NODE *)RemTailNode(h->Actions);
			sysUsedMem -= NODE_SIZE(n);
			FreeNode(n);

			h->Timer = GetMaxTimer(sys);
			h->CurrentAction = (NODE *)LIST_TPRED(h->Actions);
			((struct Action *)h->CurrentAction)->Timer = ((struct Action *)h->CurrentAction)->TimeNeeded;
		 }
		 else
			ClearHandler(sys, h->Id);
	  }
   }
}

void IgnoreAction(struct System *sys)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if ((struct Action *)h->CurrentAction)
	  {
		 if (NODE_SUCC(NODE_SUCC(h->CurrentAction)))
		 {
			h->CurrentAction = (NODE *)NODE_SUCC(h->CurrentAction);

			((struct Action *)h->CurrentAction)->Timer = 0;
		 }
		 else
		 {
			h->CurrentAction = (NODE *)LIST_TPRED(h->Actions);
			((struct Action *)h->CurrentAction)->Timer = ((struct Action *)h->CurrentAction)->TimeNeeded;
		 }
	  }
   }
}

struct plSignal *InitSignal(struct System *sys, ulong sender, ulong receiver)
{
   struct plSignal *s = NULL;

   if (sys)
   {
	  if (s = (struct plSignal *)CreateNode(sys->Signals, sizeof (struct plSignal), NULL))
	  {
		 s->SenderId   = sender;
		 s->ReceiverId = receiver;
	  }
   }

   return s;
}

void CloseSignal(struct plSignal *s)
{
   if (s)
   {
	  RemNode(s);
	  FreeNode(s);
   }
}

struct plSignal *IsSignal(struct System *sys, ulong sender, ulong receiver)
{
   struct plSignal *s;

   if (sys)
   {
	  for (s = (struct plSignal *)LIST_HEAD(sys->Signals); NODE_SUCC(s); s = (struct plSignal *)NODE_SUCC(s))
	  {
		 if ((s->SenderId == sender) && (s->ReceiverId == receiver))
			return s;
	  }
   }

   return NULL;
}

ulong CurrentTimer(struct System *sys)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
	  return h->Timer;

   return 0L;
}

void IncCurrentTimer(struct System *sys, ulong time, ubyte alsoTime)
{
   struct Handler *h;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  if (h->CurrentAction)
	  {
		 ((struct Action *)h->CurrentAction)->TimeNeeded += time;

		 if (alsoTime)
		 {
			((struct Action *)h->CurrentAction)->Timer += time;
			h->Timer								   += time;
		 }
	  }
   }
}

ulong GetMaxTimer(struct System *sys)
{
   struct Handler *h;
   struct Action  *a;
   ulong		   time = 0;

   if (sys && (h = (struct Handler *)sys->ActivHandler))
   {
	  for (a = (struct Action *)LIST_HEAD(h->Actions); NODE_SUCC(a); a = (struct Action *)NODE_SUCC(a))
		 time += a->TimeNeeded;
   }

   return time;
}

void ResetMem(void)
{
   sysUsedMem = 0L;
}

void CorrectMem(LIST *l)
{
   NODE *n;

   for (n = (NODE *)LIST_HEAD(l); NODE_SUCC(n); n = (NODE *)NODE_SUCC(n))
	  sysUsedMem -= NODE_SIZE(n);
}

