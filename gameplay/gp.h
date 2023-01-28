/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_GAMEPLAY
#define MODULE_GAMEPLAY

#include "theclou.h"

#ifndef MODULE_BASE
#include "base/base.h"
#endif

#ifndef MODULE_MEMORY
#include "memory/memory.h"
#endif

#ifndef MODULE_LIST
#include "list/list.h"
#endif

#ifndef MODULE_DISK
#include "disk/disk.h"
#endif

#ifndef MODULE_GFX
#include "gfx/gfx.h"
#endif

#ifndef MODULE_ANIM
#include "anim/sysanim.h"
#endif

#ifndef MODULE_STORY
#include "story/story.h"
#endif

#ifndef MODULE_DATAAPPL
#include "data/dataappl.h"
#endif

#ifndef MODULE_RANDOM
#include "random/random.h"
#endif

#ifndef MODULE_INTERAC
#include "present/interac.h"
#endif

#define GO (1L << 0)
#define WAIT (1L << 1)
#define BUSINESS_TALK (1L << 2)
#define LOOK (1L << 3)
#define INVESTIGATE (1L << 4)
#define PLAN (1L << 5)
#define CALL_TAXI (1L << 6)
#define MAKE_CALL (1L << 7)
#define INFO (1L << 8)
#define SLEEP (1L << 9)

#define GP_ALL_CHOICES_ENABLED INT_MAX

#define CAN_ALWAYS_HAPPEN 65535L /* Szene kann UNENDLICH oft geschehen */

#define MINUTES_PER_DAY 1440L

#define GP_STORY_BEFORE 0
#define GP_STORY_TOWN 1
#define GP_STORY_PLAN 2

#define GP_STORY_OFF (1L << 0)
#define GP_DEBUG_ON (1L << 1)
#define GP_DEMO (1L << 2)
#define GP_LEVEL_DESIGN (1L << 3)
#define GP_GUARD_DESIGN (1L << 4)
#define GP_COORDINATES (1L << 5)
#define GP_DEBUG_PLAYER (1L << 6)
#define GP_FULL_ENV (1L << 7)
#define GP_MUSIC_OFF (1L << 8)
#define GP_NO_MUSIC_IN_PLANING (1L << 10)
#define GP_NO_SAMPLES (1L << 11)
#define GP_COLLISION_CHECKING_OFF (1L << 12)
#define GP_SHOW_ROOMS (1L << 13)
#define GP_MORE_MONEY (1L << 14)  // LucyG 2017-11-05

/* Zugriffsdefines */

#define SetMinute(zeit) (film->akt_Minute = (uint32_t)(zeit))
#define SetLocation(loc)                 \
    {                                    \
        film->alter_Ort = film->akt_Ort; \
        film->akt_Ort = (uint32_t)(loc); \
    }
#define SetDay(tag) (film->akt_Tag = (uint32_t)(tag))

#define GetDay (film->akt_Tag)
#define GetMinute (film->akt_Minute)
#define GetLocation (film->akt_Ort)
#define GetOldLocation (film->alter_Ort)

#define GetFromDay(x) ((x) >> 16)
#define GetToDay(x) (((x) << 16) >> 16)

struct Film
{
    uint32_t AmountOfScenes;

    struct Scene *act_scene;
    struct Scene *gameplay;

    LIST *loc_names; /* Liste aller Orte im Spiel */
                     /* OrtNr = Nr der Node in der */
                     /* Liste */
    uint32_t StartScene;
    uint32_t StartZeit; /* =Tag seit dem Jahr 0*/
    uint32_t StartOrt;

    uint32_t akt_Tag;
    uint32_t akt_Minute;
    uint32_t akt_Ort;
    uint32_t alter_Ort;

    uint32_t EnabledChoices;

    ubyte StoryIsRunning;
};

struct SceneArgs
{
    uint32_t Moeglichkeiten;
    intptr_t ReturnValue; /* wird AUCH (!) als Input verwendet,
                           * wenn als Output verwendet = EventNr der
                           * Nachfolgerszene
                           */
    ubyte Ueberschrieben; /*  0...direkter Nachfahre,
                           * >0......uberschriebene Methode
                           */
};

struct Scene
{
    intptr_t EventNr;

    void (*Init)(void);
    void (*Done)(void);

    struct Bedingungen *bed; /* damit das Ereignis eintritt */

    LIST *std_succ; /* Standardnachfolger TCEventNode */

    uint32_t Moeglichkeiten; /* siehe defines oben 			*/
    uint32_t Dauer;          /* Dauer dieser Szene in Sekunden	*/
    uword Anzahl;            /* wie oft sie geschehen kann		*/
    uword Geschehen;         /* wie oft sie SCHON geschehen ist */
    ubyte Probability;       /* mit der sie eintritt 	0-255	*/

    ubyte Padding[3];

    int32_t LocationNr; /* Ort, den diese Szene darstellt 	*/
                        /* == -1 falls Szene = StorySzene 	*/
                        /* ansonsten Nr des Ortes		*/
};

struct Bedingungen
{
    int32_t Ort; /* der erfüllt sein muß */

    LIST *events;   /* welche Events schon geschehen sein muessen */
    LIST *n_events; /* Events, die nicht geschehen sein dürfen */
};

struct TCEventNode
{
    NODE Node;

    uint32_t EventNr;
};

/* global functions */
extern void InitStory(char *story_filename);
extern void CloseStory(void);

extern uint32_t PlayStory(void);
extern void PatchStory(void);

extern void SetEnabledChoices(uint32_t ChoiceMask);

extern void StdDone(void);
extern void StdInit(void);

extern void RefreshCurrScene(void);
extern void SetCurrentScene(struct Scene *scene);

extern char *GetCurrLocName(void);

extern char *BuildDate(uint32_t days, char language, char *date);
extern char *BuildTime(uint32_t min, char language, char *time);

extern void FormatDigit(uint32_t digit, char *s);

extern struct Scene *GetCurrentScene(void);
extern struct Scene *GetLocScene(uint32_t locNr);
extern struct Scene *GetScene(uint32_t EventNr);

extern void AddVTime(uint32_t Zeit);

extern void LinkScenes(void); /* Init und Done in jeder Scene Struktur setzen */

extern struct SceneArgs SceneArgs;
extern struct Film *film;
extern uint32_t GamePlayMode;
extern ubyte RefreshMode;

#endif
