/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef THECLOU_MODULE
#define THECLOU_MODULE

#define __COSP__
#define THECLOU_VERSION	"0.1d"

/*
 * MemAlloc/MemFree logging to
 * help with memory leaking code
 */
//#define THECLOU_DEBUG_ALLOC

/*
 * verwendete Typen
 */

typedef unsigned char	ubyte;
typedef unsigned short	uword;
typedef short			word;
typedef unsigned long	ulong;

#define EOS 	 ((char)'\0')

// ADDITIONAL VERSION DEFINES
//#define THECLOU_CDROM_VERSION
#define THECLOU_PROFIDISK

// LANGUAGE DEFINES
#define THECLOU_VERSION_GERMAN
//#define THECLOU_VERSION_ENGLISH

// SPECIALS DEFINES
#define THECLOU_JOYSTICK_DISABLED
#define THECLOU_DEBUG

/*
 * Textdateien, die auch als Daten verwendet werden
 *
 */

#define LOCATIONS_TXT		"location.lst"
#define COLL_LIST_TXT		"coll.lst"
#define PICT_LIST_TXT		"pict.lst"
#define GAMES_LIST_TXT		"games.lst"
#define GAMES_ORIG_TXT		"origin.lst"

#define LIV_ANIM_TEMPLATE_LIST	"Template.lst"
#define LIV_LIVINGS_LIST		"livings.lst"

#define SND_SOUND_LIST			((ubyte*)"sounds.list")
#define SND_SOUND_DIRECTORY		((ubyte*)"sounds")

/*
 * Namen der anderen Dateien
 *
 * Diese Dateien befinden sich im Verzeichnis theclou:data
 *
 */

#define STORY_DAT 		"TCStory.pc"
#define STORY_DAT_DEMO	"TCStory.dem"

/*
 * Nummern der Textdateien in denen die Menues stehen
 *
 * siehe Texts.list im Verzeichnis Texts
 *
 */

#define MENU_TXT				0L
#define OBJECTS_TXT				1L
#define BUSINESS_TXT 			2L
#define HOUSEDESC_TXT			3L
#define THECLOU_TXT				4L
#define INVESTIGATIONS_TXT 		5L
#define CDROM_TXT 				6L
#define OBJECTS_ENUM_TXT		7L
#define ANIM_TXT				8L
#define PRESENT_TXT				9L
#define STORY_0_TXT				10L
#define PLAN_TXT				11L
#define TOOLS_TXT 				12L
#define LOOK_TXT				13L
#define ABILITY_TXT				14L
#define STORY_1_TXT				15L
#define TALK_0_TXT				16L
#define TALK_1_TXT				17L

/*
 * Disk Nr.
 *
 */

/*
 * Defines betreffend Disketten und Verzeichnisse
 *
 */

#define PICTURE_DIRECTORY	((ubyte*)"pictures")
#define TEXT_DIRECTORY		((ubyte*)"texts")
#define INCLUDE_DIRECTORY	((ubyte*)"Include")
#define DATA_DIRECTORY		((ubyte*)"data")
#define SOUND_DIRECTORY 	((ubyte*)"sounds")
#define SAMPLES_DIRECTORY	((ubyte*)"samples")

#define DATADISK			((ubyte*)"datadisk")

#define BUILD_DATA_NAME 	((ubyte*)"tcbuild")
#define MAIN_DATA_NAME		((ubyte*)"tcmain")
#define GAME_DATA_EXT		((ubyte*)".dat")
#define GAME_REL_EXT 		((ubyte*)".rel")
#define TAXI_LOC_EXT 		((ubyte*)".loc")

#define STORY_DATA_NAME 	((ubyte*)"tcstory")

/*
 * defines fÅr Error Modul
 *
 */

ulong ShowTheClouRequester(long error_class);	/* siehe Base.c */

/*
 * some defines ...
 */

#define MATT_PICTID		 ((uword)7)
#define OLD_MATT_PICTID  ((uword)125)
#define BIG_SHEET 		 ((uword)5)

#define BGD_LONDON		 ((uword)21)
#define BGD_PLANUNG		 ((uword)23)
#define BGD_EINBRUCH 	 ((uword)23)
#define BGD_CLEAR 		 ((uword)0)

#define RADIO_BUBBLE 	 ((uword)12)
#define SPEAK_BUBBLE 	 ((uword)12)
#define THINK_BUBBLE 	 ((uword)13)

#define GFX_COLL_PARKING ((uword)27)

#endif
