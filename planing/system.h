/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_PLANING_SYSTEM
#define MODULE_PLANING_SYSTEM

#include <stdio.h>
#include "theclou.h"

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

#ifndef MODULE_ERROR
#include "error/error.h"
#endif

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_DATABASE
#include "data/database.h"
#endif

#ifndef MODULE_RELATION
#include "data/relation.h"
#endif

#ifndef MODULE_TCDATA
#include "data/tcdata.h"
#endif

#ifndef MODULE_TEXT
#include "text/text.h"
#endif

#ifndef MODULE_LANDSCAP
#include "landscap/landscap.h"
#endif

#ifndef MODULE_PLANING_PLANER
#include "planing/planer.h"
#endif


/* System main structure - like the kernel in an OS */
struct System
{
	LIST *Handlers;  /* Pointer to all started handlers */
	LIST *Signals;   /* Pointer to signal in the system */

	NODE *ActivHandler;
};

struct System *InitSystem(void);                          /* Initialize system for use */
void CloseSystem(struct System *sys);                    /* Close all system immedietly */
void SetActivHandler(struct System *sys, uint32_t id);

void  SaveSystem(FILE *fh, struct System *sys);
LIST *LoadSystem(FILE *fh, struct System *sys);


/* System Handler Flags */
#define SHF_NORMAL         0L
#define SHF_AUTOREVERS     1L<<0

/* Handler structure - like a task in an OS */
struct Handler
{
	NODE	Link;                   /* Link to next handler */

	uint32_t	Id;                     /* ID of handler (all handlers will be identified with their ID and
	                                 not through pointers, which will save global data) */

	uint32_t Timer;                  /* Handler time in seconds/3 */

	uint32_t Flags;                  /* Handler flags */

	LIST	*Actions;               /* Action table */
	NODE	*CurrentAction;         /* Current action */
};


struct Handler *InitHandler(struct System *sys, uint32_t id, uint32_t flags);    /* Initialize handler         */
void CloseHandler(struct System *sys, uint32_t id);                           /* Close Handler              */
struct Handler *ClearHandler(struct System *sys, uint32_t id);                /* Clear Handlers action list */
struct Handler *FindHandler(struct System *sys, uint32_t id);

ubyte IsHandlerCleared(struct System *sys);

void  SaveHandler(FILE *fh, struct System *sys, uint32_t id);
ubyte LoadHandler(FILE *fh, struct System *sys, uint32_t id);

uint32_t plGetUsedMem(void);

/* here we are at the real part - the actions */
#define ACTION_GO           1
#define ACTION_WAIT         2
#define ACTION_SIGNAL       3
#define ACTION_WAIT_SIGNAL  4
#define ACTION_USE          5
#define ACTION_TAKE         6
#define ACTION_DROP         7
#define ACTION_OPEN         8
#define ACTION_CLOSE	       9
#define ACTION_CONTROL      10

struct Action
{
   NODE  Link;

	uword Type;

	uword TimeNeeded;          /* times in seconds/3 */
	uword Timer;
	short pad;
};

/* Type : ACTION_GO
   Figure will go in one direction for x steps */
struct ActionGo
{
	uword Direction;
	short pad;
};

#define DIRECTION_NO    0
#define DIRECTION_LEFT  1
#define DIRECTION_RIGHT 2
#define DIRECTION_UP    4
#define DIRECTION_DOWN  8


/* Type : ACTION_WAIT
   Figure waits for x seconds
   Does not need an extended data structure */

/* Type : ACTION_SIGNAL
   Figure sends out a signal of a special type to a receiver */
struct ActionSignal
{
   uint32_t ReceiverId;
};

/* Type : ACTION_WAIT_SIGNAL
   Figure waits until it receives a signal of a special type from a special sender */
struct ActionWaitSignal
{
	uint32_t SenderId;
};

/* Type : ACTION_USE */
struct ActionUse
{
   uint32_t ItemId;
   uint32_t ToolId;
};

/* Type : ACTION_TAKE */
struct ActionTake
{
   uint32_t ItemId;
   uint32_t LootId;
};

/* Type : ACTION_DROP */
struct ActionDrop
{
   uint32_t ItemId;
   uint32_t LootId;
};

/* Type : ACTION_OPEN */
struct ActionOpen
{
   uint32_t ItemId;
};

/* Type : ACTION_CLOSE */
struct ActionClose
{
   uint32_t ItemId;
};

/* Type : ACTION_CONTROL */
struct ActionControl
{
   uint32_t ItemId;
};

/* LucyG 2017-10-29 : this macro looks suspicious */
#define ActionData(ac,type)      ((type)(ac+1))

struct Action *InitAction(struct System *sys, uword type, uint32_t data1, uint32_t data2, uint32_t time);
struct Action *CurrentAction(struct System *sys);
struct Action *GoFirstAction(struct System *sys);
struct Action *GoLastAction(struct System *sys);
struct Action *NextAction(struct System *sys);
struct Action *PrevAction(struct System *sys);
ubyte ActionStarted(struct System *sys);
ubyte ActionEnded(struct System *sys);
void RemLastAction(struct System *sys);
void IgnoreAction(struct System *sys);


/* Signal structure - to make it possible to communicate between handlers, our small attempt of an message system */
#define SIGNAL_HURRY_UP    1  /* come on, get on */
#define SIGNAL_DONE        2  /* well, my work is done */
#define SIGNAL_ESCAPE      3  /* f..., police is coming, let's go */

struct plSignal
{
	NODE  Link;

	uint32_t	SenderId;
	uint32_t	ReceiverId;
};

struct plSignal *InitSignal(struct System *sys, uint32_t sender, uint32_t receiver);
void CloseSignal(struct plSignal *sig);
struct plSignal *IsSignal(struct System *sys, uint32_t sender, uint32_t receiver);


uint32_t CurrentTimer(struct System *sys);
void  IncCurrentTimer(struct System *sys, uint32_t time, ubyte alsoTime);
uint32_t GetMaxTimer(struct System *sys);

void ResetMem(void);
void CorrectMem(LIST *l);

#endif
