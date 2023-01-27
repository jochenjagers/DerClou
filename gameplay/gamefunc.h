/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_GAMEFUNC
#define MODULE_GAMEFUNC

#define SCENE_NODE			((ubyte)100)
#define EVENT_NODE         ((ubyte)101)
#define MAX_SCENES_AMOUNT	1L<<14	/* 14 + 18 = 32 Bits !!! */
#define MAX_EVENTS_AMOUNT	1L<<18	/* wichtig da story das ben”tigt ! */


struct StoryHeader
{
	ubyte StoryName[20];

	ulong EventCount;	/* Zaehler mit der h”chsten EventNr */
	ulong SceneCount;

	ulong AmountOfScenes;
	ulong AmountOfEvents;

	ulong StartZeit;
	ulong StartOrt;
	ulong StartSzene;
};

struct NewStory
{
	ubyte StoryName[20];

	struct StoryHeader *sh;

	struct List *scenes;	/* Liste von SceneNodes */
	struct List *events;

	ulong StartZeit;
	ulong StartOrt;
};

struct NewScene
{
	ulong	EventNr;
	ubyte	SceneName[20];

	long	Tag;				   /* der Tag an dem sie eintritt */
	long	MinZeitPunkt;		/* zeitlicher Bereich in dem 	*/
	long	MaxZeitPunkt;		/* sie eintritt 			*/
	long	Ort;					/* der erfllt sein muá 		*/

	ulong	AnzahlderEvents;
	ulong	AnzahlderN_Events;

	ulong	*events;				/* Nr. der Events, die erfllt sein mssen */
	ulong	*n_events;			/* Nr. der Events, die nicht erfllt sein mssen */

	ulong	AnzahlderNachfolger;
	ulong	*nachfolger;		/* Nr. der NachfolgerEvents */

	ulong	Moeglichkeiten;	/* siehe defines oben 			*/
	ulong	Dauer;			   /* Dauer dieser Szene in Minuten	*/
	uword	Anzahl;			   /* wie oft sie geschehen kann		*/
	uword	Geschehen;		   /* wie oft sie SCHON geschehen ist */
	ubyte	Possibility;		/* mit der sie eintritt 0-255	*/

	ubyte	Padding[3];		/* Alignment */

	ulong Sample;           /* Nummer des Samples */
	ulong Anim;             /* Nummer der Animation */
	long  NewOrt;           /* Ort der Scene */
};

struct SceneNode
{
	struct Node Link;
	struct NewScene ns;
};

struct NewEvent
{
	ulong EventNr;
	ubyte EventName[20];
};

struct EventNode
{
	struct Node Link;
	struct NewEvent ne;
};


/* Prototypes */
/* Story */
extern void NewStory    (ubyte *name, ulong StartZeit, ulong StartOrt);
extern void ChangeStory (ulong StartZeit, ulong StartOrt);
extern void RemoveStory (void);
extern void WriteStory  (ubyte *filename);
extern void ReadStory   (ubyte *filename);

/* Scenes */
extern void AddScene        (struct NewScene *ns);
extern void ChangeScene     (ubyte *name, struct NewScene *ns);
extern void RemoveScene     (ubyte *name);
extern void RemoveAllScenes (void);

/* Events */
extern void AddEvent        (struct NewEvent *ev);
extern void RemoveEvent     (ubyte *name);
extern void RemoveAllEvents (void);

/* Specials */
char *GetName (ulong EventNr);


extern struct NewStory *story;

#endif
