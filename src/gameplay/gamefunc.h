/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_GAMEFUNC
#define MODULE_GAMEFUNC

#define SCENE_NODE ((ubyte)100)
#define EVENT_NODE ((ubyte)101)
#define MAX_SCENES_AMOUNT 1L << 14 /* 14 + 18 = 32 Bits !!! */
#define MAX_EVENTS_AMOUNT 1L << 18 /* wichtig da story das benötigt ! */

struct StoryHeader
{
    ubyte StoryName[20];

    uint32_t EventCount; /* Zaehler mit der höchsten EventNr */
    uint32_t SceneCount;

    uint32_t AmountOfScenes;
    uint32_t AmountOfEvents;

    uint32_t StartZeit;
    uint32_t StartOrt;
    uint32_t StartSzene;
};

struct NewStory
{
    ubyte StoryName[20];

    struct StoryHeader *sh;

    struct List *scenes; /* Liste von SceneNodes */
    struct List *events;

    uint32_t StartZeit;
    uint32_t StartOrt;
};

struct NewScene
{
    uint32_t EventNr;
    ubyte SceneName[20];

    int32_t Tag;          /* der Tag an dem sie eintritt */
    int32_t MinZeitPunkt; /* zeitlicher Bereich in dem 	*/
    int32_t MaxZeitPunkt; /* sie eintritt 			*/
    int32_t Ort;          /* der erfüllt sein muß 		*/

    uint32_t AnzahlderEvents;
    uint32_t AnzahlderN_Events;

    uint32_t *events;   /* Nr. der Events, die erfüllt sein müssen */
    uint32_t *n_events; /* Nr. der Events, die nicht erfüllt sein müssen */

    uint32_t AnzahlderNachfolger;
    uint32_t *nachfolger; /* Nr. der NachfolgerEvents */

    uint32_t Moeglichkeiten; /* siehe defines oben 			*/
    uint32_t Dauer;          /* Dauer dieser Szene in Minuten	*/
    uword Anzahl;            /* wie oft sie geschehen kann		*/
    uword Geschehen;         /* wie oft sie SCHON geschehen ist */
    ubyte Possibility;       /* mit der sie eintritt 0-255	*/

    ubyte Padding[3]; /* Alignment */

    uint32_t Sample; /* Nummer des Samples */
    uint32_t Anim;   /* Nummer der Animation */
    int32_t NewOrt;  /* Ort der Scene */
};

struct SceneNode
{
    struct Node Link;
    struct NewScene ns;
};

struct NewEvent
{
    uint32_t EventNr;
    ubyte EventName[20];
};

struct EventNode
{
    struct Node Link;
    struct NewEvent ne;
};

/* Prototypes */
/* Story */
extern void NewStory(ubyte *name, uint32_t StartZeit, uint32_t StartOrt);
extern void ChangeStory(uint32_t StartZeit, uint32_t StartOrt);
extern void RemoveStory(void);
extern void WriteStory(ubyte *filename);
extern void ReadStory(ubyte *filename);

/* Scenes */
extern void AddScene(struct NewScene *ns);
extern void ChangeScene(ubyte *name, struct NewScene *ns);
extern void RemoveScene(ubyte *name);
extern void RemoveAllScenes(void);

/* Events */
extern void AddEvent(struct NewEvent *ev);
extern void RemoveEvent(ubyte *name);
extern void RemoveAllEvents(void);

/* Specials */
char *GetName(uint32_t EventNr);

extern struct NewStory *story;

#endif
