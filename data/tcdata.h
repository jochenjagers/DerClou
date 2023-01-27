/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
/* modified 2014-07-01 by byteranger*/

#ifndef MODULE_TCDATA
#define MODULE_TCDATA

/*
 * Data header file created with dc
 * Database (offset, size) : tcMain (0, 10000)
 */

/*
 * Make sure enums are 16bit
 */
#define TCenum(e)	unsigned short

#define DB_tcMain_OFFSET        0
#define DB_tcMain_SIZE          10000

#define Const_tcTALK_ABILITY       4
#define Const_tcIN_PROGRESS_BIT       5
#define Const_tcLOCK_UNLOCK_BIT       2
#define Const_tcTALK_JOB_OFFER       0
#define Const_tcON_OFF       6
#define Const_tcCAR_PICT_OFFSET       24
#define Const_tcCHAINED_TO_ALARM       1
#define Const_tcHORIZ_VERT_BIT       0
#define Const_tcTAKE_BIT       3
#define Const_tcALARM_PICT_OFFSET       25
#define Const_tcTALK_PRISON_BEFORE       2
#define Const_tcACCESS_BIT       4
#define Const_tcTALK_YOUR_JOB       1
#define Const_tcOPEN_CLOSE_BIT       1
#define Const_tcNO_CHAINING       0
#define Const_tcTOOL_OPENS       1
#define Const_tcCHAINED_TO_POWER       2
#define Const_tcSYMBOL_PICT_SIZE       16
#define Const_tcPOWER_PICT_OFFSET       26

#define Relation_joined_by      8
#define joined_byP(l,r,p,c)     AskP   (dbGetObject (l), 8, dbGetObject (r), p, c)
#define joined_by(l,r)          AskP   (dbGetObject (l), 8, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define joined_byAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 8, BuildObjectList); }
#define joined_bySetP(l,r,p)    SetP   (dbGetObject (l), 8, dbGetObject (r), p)
#define joined_bySet(l,r)       SetP   (dbGetObject (l), 8, dbGetObject (r), NO_PARAMETER)
#define joined_byUnSet(l,r)     UnSet  (dbGetObject (l), 8, dbGetObject (r))
#define joined_byGet(l,r)       GetP   (dbGetObject (l), 8, dbGetObject (r))

#define Relation_hurt      12
#define hurtP(l,r,p,c)     AskP   (dbGetObject (l), 12, dbGetObject (r), p, c)
#define hurt(l,r)          AskP   (dbGetObject (l), 12, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hurtAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 12, BuildObjectList); }
#define hurtSetP(l,r,p)    SetP   (dbGetObject (l), 12, dbGetObject (r), p)
#define hurtSet(l,r)       SetP   (dbGetObject (l), 12, dbGetObject (r), NO_PARAMETER)
#define hurtUnSet(l,r)     UnSet  (dbGetObject (l), 12, dbGetObject (r))
#define hurtGet(l,r)       GetP   (dbGetObject (l), 12, dbGetObject (r))

#define Relation_remember      18
#define rememberP(l,r,p,c)     AskP   (dbGetObject (l), 18, dbGetObject (r), p, c)
#define remember(l,r)          AskP   (dbGetObject (l), 18, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define rememberAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 18, BuildObjectList); }
#define rememberSetP(l,r,p)    SetP   (dbGetObject (l), 18, dbGetObject (r), p)
#define rememberSet(l,r)       SetP   (dbGetObject (l), 18, dbGetObject (r), NO_PARAMETER)
#define rememberUnSet(l,r)     UnSet  (dbGetObject (l), 18, dbGetObject (r))
#define rememberGet(l,r)       GetP   (dbGetObject (l), 18, dbGetObject (r))

#define Relation_has      4
#define hasP(l,r,p,c)     AskP   (dbGetObject (l), 4, dbGetObject (r), p, c)
#define has(l,r)          AskP   (dbGetObject (l), 4, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hasAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 4, BuildObjectList); }
#define hasSetP(l,r,p)    SetP   (dbGetObject (l), 4, dbGetObject (r), p)
#define hasSet(l,r)       SetP   (dbGetObject (l), 4, dbGetObject (r), NO_PARAMETER)
#define hasUnSet(l,r)     UnSet  (dbGetObject (l), 4, dbGetObject (r))
#define hasGet(l,r)       GetP   (dbGetObject (l), 4, dbGetObject (r))

#define Relation_taxi      16
#define taxiP(l,r,p,c)     AskP   (dbGetObject (l), 16, dbGetObject (r), p, c)
#define taxi(l,r)          AskP   (dbGetObject (l), 16, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define taxiAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 16, BuildObjectList); }
#define taxiSetP(l,r,p)    SetP   (dbGetObject (l), 16, dbGetObject (r), p)
#define taxiSet(l,r)       SetP   (dbGetObject (l), 16, dbGetObject (r), NO_PARAMETER)
#define taxiUnSet(l,r)     UnSet  (dbGetObject (l), 16, dbGetObject (r))
#define taxiGet(l,r)       GetP   (dbGetObject (l), 16, dbGetObject (r))

#define Relation_learned      17
#define learnedP(l,r,p,c)     AskP   (dbGetObject (l), 17, dbGetObject (r), p, c)
#define learned(l,r)          AskP   (dbGetObject (l), 17, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define learnedAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 17, BuildObjectList); }
#define learnedSetP(l,r,p)    SetP   (dbGetObject (l), 17, dbGetObject (r), p)
#define learnedSet(l,r)       SetP   (dbGetObject (l), 17, dbGetObject (r), NO_PARAMETER)
#define learnedUnSet(l,r)     UnSet  (dbGetObject (l), 17, dbGetObject (r))
#define learnedGet(l,r)       GetP   (dbGetObject (l), 17, dbGetObject (r))

#define Relation_sound      13
#define soundP(l,r,p,c)     AskP   (dbGetObject (l), 13, dbGetObject (r), p, c)
#define sound(l,r)          AskP   (dbGetObject (l), 13, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define soundAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 13, BuildObjectList); }
#define soundSetP(l,r,p)    SetP   (dbGetObject (l), 13, dbGetObject (r), p)
#define soundSet(l,r)       SetP   (dbGetObject (l), 13, dbGetObject (r), NO_PARAMETER)
#define soundUnSet(l,r)     UnSet  (dbGetObject (l), 13, dbGetObject (r))
#define soundGet(l,r)       GetP   (dbGetObject (l), 13, dbGetObject (r))

#define Relation_ClockTimer      2
#define ClockTimerP(l,r,p,c)     AskP   (dbGetObject (l), 2, dbGetObject (r), p, c)
#define ClockTimer(l,r)          AskP   (dbGetObject (l), 2, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define ClockTimerAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 2, BuildObjectList); }
#define ClockTimerSetP(l,r,p)    SetP   (dbGetObject (l), 2, dbGetObject (r), p)
#define ClockTimerSet(l,r)       SetP   (dbGetObject (l), 2, dbGetObject (r), NO_PARAMETER)
#define ClockTimerUnSet(l,r)     UnSet  (dbGetObject (l), 2, dbGetObject (r))
#define ClockTimerGet(l,r)       GetP   (dbGetObject (l), 2, dbGetObject (r))

#define Relation_PersonWorksHere      20
#define PersonWorksHereP(l,r,p,c)     AskP   (dbGetObject (l), 20, dbGetObject (r), p, c)
#define PersonWorksHere(l,r)          AskP   (dbGetObject (l), 20, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define PersonWorksHereAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 20, BuildObjectList); }
#define PersonWorksHereSetP(l,r,p)    SetP   (dbGetObject (l), 20, dbGetObject (r), p)
#define PersonWorksHereSet(l,r)       SetP   (dbGetObject (l), 20, dbGetObject (r), NO_PARAMETER)
#define PersonWorksHereUnSet(l,r)     UnSet  (dbGetObject (l), 20, dbGetObject (r))
#define PersonWorksHereGet(l,r)       GetP   (dbGetObject (l), 20, dbGetObject (r))

#define Relation_hasClock      1
#define hasClockP(l,r,p,c)     AskP   (dbGetObject (l), 1, dbGetObject (r), p, c)
#define hasClock(l,r)          AskP   (dbGetObject (l), 1, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hasClockAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 1, BuildObjectList); }
#define hasClockSetP(l,r,p)    SetP   (dbGetObject (l), 1, dbGetObject (r), p)
#define hasClockSet(l,r)       SetP   (dbGetObject (l), 1, dbGetObject (r), NO_PARAMETER)
#define hasClockUnSet(l,r)     UnSet  (dbGetObject (l), 1, dbGetObject (r))
#define hasClockGet(l,r)       GetP   (dbGetObject (l), 1, dbGetObject (r))

#define Relation_toolRequires      15
#define toolRequiresP(l,r,p,c)     AskP   (dbGetObject (l), 15, dbGetObject (r), p, c)
#define toolRequires(l,r)          AskP   (dbGetObject (l), 15, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define toolRequiresAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 15, BuildObjectList); }
#define toolRequiresSetP(l,r,p)    SetP   (dbGetObject (l), 15, dbGetObject (r), p)
#define toolRequiresSet(l,r)       SetP   (dbGetObject (l), 15, dbGetObject (r), NO_PARAMETER)
#define toolRequiresUnSet(l,r)     UnSet  (dbGetObject (l), 15, dbGetObject (r))
#define toolRequiresGet(l,r)       GetP   (dbGetObject (l), 15, dbGetObject (r))

#define Relation_knows      5
#define knowsP(l,r,p,c)     AskP   (dbGetObject (l), 5, dbGetObject (r), p, c)
#define knows(l,r)          AskP   (dbGetObject (l), 5, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define knowsAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 5, BuildObjectList); }
#define knowsSetP(l,r,p)    SetP   (dbGetObject (l), 5, dbGetObject (r), p)
#define knowsSet(l,r)       SetP   (dbGetObject (l), 5, dbGetObject (r), NO_PARAMETER)
#define knowsUnSet(l,r)     UnSet  (dbGetObject (l), 5, dbGetObject (r))
#define knowsGet(l,r)       GetP   (dbGetObject (l), 5, dbGetObject (r))

#define Relation_hasLootBag      19
#define hasLootBagP(l,r,p,c)     AskP   (dbGetObject (l), 19, dbGetObject (r), p, c)
#define hasLootBag(l,r)          AskP   (dbGetObject (l), 19, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define hasLootBagAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 19, BuildObjectList); }
#define hasLootBagSetP(l,r,p)    SetP   (dbGetObject (l), 19, dbGetObject (r), p)
#define hasLootBagSet(l,r)       SetP   (dbGetObject (l), 19, dbGetObject (r), NO_PARAMETER)
#define hasLootBagUnSet(l,r)     UnSet  (dbGetObject (l), 19, dbGetObject (r))
#define hasLootBagGet(l,r)       GetP   (dbGetObject (l), 19, dbGetObject (r))

#define Relation_likes_to_be      6
#define likes_to_beP(l,r,p,c)     AskP   (dbGetObject (l), 6, dbGetObject (r), p, c)
#define likes_to_be(l,r)          AskP   (dbGetObject (l), 6, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define likes_to_beAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 6, BuildObjectList); }
#define likes_to_beSetP(l,r,p)    SetP   (dbGetObject (l), 6, dbGetObject (r), p)
#define likes_to_beSet(l,r)       SetP   (dbGetObject (l), 6, dbGetObject (r), NO_PARAMETER)
#define likes_to_beUnSet(l,r)     UnSet  (dbGetObject (l), 6, dbGetObject (r))
#define likes_to_beGet(l,r)       GetP   (dbGetObject (l), 6, dbGetObject (r))

#define Relation_join      7
#define joinP(l,r,p,c)     AskP   (dbGetObject (l), 7, dbGetObject (r), p, c)
#define join(l,r)          AskP   (dbGetObject (l), 7, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define joinAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 7, BuildObjectList); }
#define joinSetP(l,r,p)    SetP   (dbGetObject (l), 7, dbGetObject (r), p)
#define joinSet(l,r)       SetP   (dbGetObject (l), 7, dbGetObject (r), NO_PARAMETER)
#define joinUnSet(l,r)     UnSet  (dbGetObject (l), 7, dbGetObject (r))
#define joinGet(l,r)       GetP   (dbGetObject (l), 7, dbGetObject (r))

#define Relation_opens      14
#define opensP(l,r,p,c)     AskP   (dbGetObject (l), 14, dbGetObject (r), p, c)
#define opens(l,r)          AskP   (dbGetObject (l), 14, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define opensAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 14, BuildObjectList); }
#define opensSetP(l,r,p)    SetP   (dbGetObject (l), 14, dbGetObject (r), p)
#define opensSet(l,r)       SetP   (dbGetObject (l), 14, dbGetObject (r), NO_PARAMETER)
#define opensUnSet(l,r)     UnSet  (dbGetObject (l), 14, dbGetObject (r))
#define opensGet(l,r)       GetP   (dbGetObject (l), 14, dbGetObject (r))

#define Relation_livesIn      10
#define livesInP(l,r,p,c)     AskP   (dbGetObject (l), 10, dbGetObject (r), p, c)
#define livesIn(l,r)          AskP   (dbGetObject (l), 10, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define livesInAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 10, BuildObjectList); }
#define livesInSetP(l,r,p)    SetP   (dbGetObject (l), 10, dbGetObject (r), p)
#define livesInSet(l,r)       SetP   (dbGetObject (l), 10, dbGetObject (r), NO_PARAMETER)
#define livesInUnSet(l,r)     UnSet  (dbGetObject (l), 10, dbGetObject (r))
#define livesInGet(l,r)       GetP   (dbGetObject (l), 10, dbGetObject (r))

#define Relation_break      11
#define breakP(l,r,p,c)     AskP   (dbGetObject (l), 11, dbGetObject (r), p, c)
#define break(l,r)          AskP   (dbGetObject (l), 11, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define breakAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 11, BuildObjectList); }
#define breakSetP(l,r,p)    SetP   (dbGetObject (l), 11, dbGetObject (r), p)
#define breakSet(l,r)       SetP   (dbGetObject (l), 11, dbGetObject (r), NO_PARAMETER)
#define breakUnSet(l,r)     UnSet  (dbGetObject (l), 11, dbGetObject (r))
#define breakGet(l,r)       GetP   (dbGetObject (l), 11, dbGetObject (r))

#define Relation_StairConnects      3
#define StairConnectsP(l,r,p,c)     AskP   (dbGetObject (l), 3, dbGetObject (r), p, c)
#define StairConnects(l,r)          AskP   (dbGetObject (l), 3, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define StairConnectsAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 3, BuildObjectList); }
#define StairConnectsSetP(l,r,p)    SetP   (dbGetObject (l), 3, dbGetObject (r), p)
#define StairConnectsSet(l,r)       SetP   (dbGetObject (l), 3, dbGetObject (r), NO_PARAMETER)
#define StairConnectsUnSet(l,r)     UnSet  (dbGetObject (l), 3, dbGetObject (r))
#define StairConnectsGet(l,r)       GetP   (dbGetObject (l), 3, dbGetObject (r))

#define Relation_uses      9
#define usesP(l,r,p,c)     AskP   (dbGetObject (l), 9, dbGetObject (r), p, c)
#define uses(l,r)          AskP   (dbGetObject (l), 9, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define usesAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 9, BuildObjectList); }
#define usesSetP(l,r,p)    SetP   (dbGetObject (l), 9, dbGetObject (r), p)
#define usesSet(l,r)       SetP   (dbGetObject (l), 9, dbGetObject (r), NO_PARAMETER)
#define usesUnSet(l,r)     UnSet  (dbGetObject (l), 9, dbGetObject (r))
#define usesGet(l,r)       GetP   (dbGetObject (l), 9, dbGetObject (r))


#define Location_Chiswick_Out                127L
//#define Ability_Kampf                179L
#define Item_Tresen                9223L
#define Car_Fiat_634_N_1943                27L
#define Location_Kenwood_Out                121L
#define Location_National_Galery_Out                135L
#define Item_Holztuer                9201L
#define Car_Pontiac_Streamliner_1944                11L
#define Person_Jiri_Poulin                9833L
#define Location_Aunt_Emmas_Shop                105L
#define Item_Beichtstuhl                9273L
#define Person_Neil_Grey                9815L
#define Location_British_Museum                130L
#define Item_Hottentotten_Figur                9293L
#define Loot_Kronjuwelen                9633L
#define Car_Pontiac_Streamliner_1946                12L
#define Person_Ken_Addison                9803L
#define Location_Lisson_Grove                155L
#define Item_Tresorraum                9207L
#define Item_Regal_mit_Chemikalien                9286L
#define Item_Zuggelaender                9308L
#define Car_Pontiac_Streamliner_1949                13L
#define Person_John_O_Keef                9841L
//#define Location_Cars_Vans_Office                100L
//#define London_London_1                30L
#define Location_Villa_Out                119L
#define Location_Aunt_Emmas_Shop_Out                106L
#define Location_British_Museum_Out                131L
#define Item_Lokomotive_Tuer                295L
#define Person_Frank_Meier                9804L
#define Item_Sofa                9218L
#define Tool_Bohrmaschine                9512L
#define Tool_Maske                9508L
#define Tool_Elektroset                9520L
#define Item_Bett                9227L
#define Car_Fiat_634_N_1936                26L
//#define Location_Fat_Mans_Pub                99L
#define LSOLock_Kombinationsschloss                9102L
#define Item_Kerzen                9279L
#define Tool_Schutzanzug                9521L
//#define Player_Player_1                29L
#define Location_Westminster_Abbey                162L
#define Tool_Chloroform                9509L
#define Tool_Batterie                9518L
#define Location_Osterly                124L
#define Item_Hantel                9267L
#define Item_Sockel                9261L
#define Tool_Stechkarte                9528L
#define Item_Mauer                9208L
#define Loot_Koenig                9643L
#define Person_Marc_Smith                9832L
#define Location_Bulstrode_Chemestry_Ltd_Out                172L
#define Location_Post_Train                166L
#define Item_Treppe                9238L
#define Item_Absperrung                9292L
#define Item_Palette_mit_Kisten                9305L
#define Tool_Brecheisen                9510L
//#define Ability_Sprengstoff                175L
#define Item_Beutesack                9270L
#define Tool_Schneidbrenner                9515L
#define Loot_Unterhose                9647L
#define Person_Mark_Hart                9816L
#define Person_Prof_Marcus_Green                9852L
#define Location_Pink_Villa_Out                112L
#define Item_Busch                9235L
#define Tool_Handschuhe                9506L
#define Loot_Hummer                9611L
#define Loot_Geschirr                9621L
#define Person_Matt_Stuvysunt                9801L
#define Person_Samuel_Rosenblatt                9830L
#define Loot_Gebeine                9618L
#define Item_Strauch                9232L
#define Tool_Hand                9501L
#define Loot_Saurierknochen                9628L
#define Person_Richard_Doil                9845L
#define Item_Saeule                9230L
#define Location_Southhampton                156L
#define Item_Palette_mit_Schachteln                9291L
#define Item_Batman_Figur                9294L
#define Location_Tower_Out                141L
#define Car_Jaguar_XK_1950                10L
#define Tool_Dietrich                9502L
#define Loot_Jack                9642L
#define Person_Lucas_Grull                9831L
#define Item_Nachtkaestchen                9260L
#define Tool_Strickleiter                9526L
#define Car_Morris_Minor_1948                17L
#define Location_Ham                122L
#define Location_Bulstrode_Chemestry_Ltd                171L
#define Tool_Hammer                9503L
#define Loot_Cartier_Uhr                9612L
#define Loot_Dokument                9636L
#define Loot_eine_Statue                9605L
#define Loot_Dicker                9640L
//#define Location_Highgate_Out                116L
#define Location_Osterly_Out                125L
#define Person_Pere_Ubu                9858L
#define Item_Schrein                9278L
#define Loot_Sanskrit_Buch                9613L
#define Item_Grabhuegel                9258L
#define Item_Bild                9265L
#define Location_Garage                144L
#define Loot_Wache_                9644L
#define Location_Westminster_Abbey_Out                163L
#define Item_Lokomotive_seitlich                293L
#define Tool_Elektrohammer                9523L
#define Loot_Autogramm                9614L
#define Person_Albert_Liet                9811L
#define Location_Downing_Street                164L
//#define Ability_Elektronik                177L
#define Loot_Opiumpfeife                9637L
#define Person_Al_Mel                9835L
#define Item_Alarmanlage_X3                9211L
#define Item_Gelaender                9284L
#define Item_Versuchstisch                9287L
#define Loot_Bernstein                9630L
//#define Location_Holland_Street                95L
#define Location_Watling_Street                97L
//#define Location_Parker                146L
#define Person_Frank_De_Silva                9839L
#define Item_Flugzeugheck                9244L
#define Loot_Muenzesammlung                9626L
#define Person_Dean_Esters                9851L
#define Car_Fiat_Topolino_1936                14L
#define Item_Lichtschranke                9263L
#define Loot_ein_historisches_Kunstobjekt                9607L
#define Tool_Dynamit                9522L
#define Person_Thomas_Groul                9819L
#define Location_Villa                118L
#define Location_Sotherbys                128L
#define CompleteLoot_MattsLoot                34L
#define Person_Herbert_Briggs                9805L
#define Item_Fasz                9289L
#define Tool_Schuhe                9507L
#define Item_Hecke                9257L
#define Item_Mikrophon                9264L
#define Item_Vase                9268L
#define Location_Pink_Villa                111L
#define Person_Red_Stanson                9847L
#define Item_Kanone                9226L
#define Car_Cadillac_Club_1952                5L
#define Person_Jack_Motart                9836L
#define Item_Gebuesch                9251L
#define Car_Fiat_Topolino_1940                15L
#define Loot_Buddha                9622L
#define Person_Serge_Fontane                9813L
#define Tool_Sauerstofflanze                9516L
#define Car_Fiat_Topolino_1942                16L
#define Car_Rover_75_1949                20L
#define Item_Statue                9233L
#define Loot_Jadepferd                9631L
#define Person_Mohammed_Abdula                9812L
#define Loot_Magna_Charta                9625L
#define Location_Police_Office                153L
#define Item_Stahltuer                9202L
#define Item_Buecherregal                9241L
#define Person_Paul_O_Conner                9802L
#define Location_Jeweller_Out                108L
#define Location_Antique_Shop_Out                110L
#define Item_Kranz                9237L
#define Item_Steinmauer                9253L
#define Item_Grab                9259L
//#define London_Escape                32L
#define LSOLock_Zylinderschloss                9103L
#define Item_leere_Palette                9290L
#define Loot_Ring_des_Abtes                9634L
#define Loot_Insektensammlung                9629L
#define Item_Kirchenkreuz                9283L
#define Person_Jim_Danner                9817L
#define Location_Antique_Shop                109L
//#define Location_Walrus                143L
#define Location_Baker_Street                158L
#define Item_Wache                9272L
#define Item_Alarmanlage_Top                9212L
#define Item_Spiegel                9231L
#define Item_Tanne                9255L
#define Location_Natural_Museum                132L
#define Person_Dan_Stanford                9821L
#define Location_Natural_Museum_Out                133L
#define Item_Regal                9242L
#define Loot_Gebrauchsartikel                9608L
#define Item_Sessel                9216L
#define Person_Thomas_Smith                9843L
//#define Location_Maloya                147L
#define Person_Rosi_Lambert                9854L
#define Person_Peter_Brook                9829L
#define Location_Kenwood_House                120L
//#define Location_Nirvana                151L
#define Item_Klavier                9221L
#define Item_Wache_Figur                9299L
#define Car_Rover_75_1950                21L
#define Loot_Batman                9639L
#define Loot_Geld                9602L
#define Loot_Stein_von_Rosetta                9624L
#define Car_Rover_75_1952                22L
#define Item_Elch                9225L
#define Loot_Kuriositaet                9606L
#define Location_Jeweller                107L
#define Loot_Beauty_Case                9615L
#define LSObject_Beutesack_1                9701L
//#define Location_Policestation                102L
//#define Ability_Aufpassen                178L
#define Item_Heiligenstatue                9281L
#define Tool_Kernbohrer                9505L
#define LSObject_Beutesack_2                9702L
#define LSObject_Beutesack_3                9703L
#define Location_Old_Peoples_Home_Out                114L
#define LSObject_Beutesack_4                9704L
#define Location_Tower                140L
#define Item_Kasse                9262L
#define LSObject_Beutesack_5                9705L
#define Item_Kamin                9229L
#define Item_Dinosaurier_                9248L
#define Item_Dinosaurier__                9249L
#define Tool_Axt                9504L
#define Loot_Victorianische_Vase                9632L
#define LSObject_Beutesack_6                9706L
#define Location_Grave                117L
//#define Ability_Schloesser                174L
//#define Ability_Safes                176L
#define Item_Kuehlschrank                9224L
#define LSObject_Beutesack_7                9707L
#define Location_Fat_Man_Out                150L
#define Loot_ein_Bild                9600L
#define Loot_Formel                9646L
#define LSObject_Beutesack_8                9708L
#define Item_Panzerschrank                9205L
#define Item_verzierte_Saeule                9285L
#define Person_Frank_Maloya                9814L
#define Item_Miss_World_1952                9300L
#define Location_Tobacconist                103L
#define Loot_Unbekannter_                9641L
#define Item_Opferkassa                9277L
#define Loot_Goldring                9627L
#define Item_Lokomotive_oben                292L
#define Person_Miles_Chickenwing                9823L
#define Person_Sid_Palmer                9850L
#define Tool_Schloszstecher                9514L
#define Person_Kevin_Smith                9837L
#define Location_Baker_Street_Out                159L
#define Location_National_Galery                134L
#define Location_Dowing_Street_Out                165L
#define Item_Scheinwerfer                9269L
#define Item_Postsack                9306L
#define Location_Sotherbys_Out                129L
#define Item_Gemaelde                9206L
#define Loot_Gold                9601L
#define Location_Madame_Tussaud_Out                161L
#define Person_Tony_Allen                9806L
#define Location_Buckingham_Palace                169L
#define Item_Gruft                9254L
#define Location_Victoria_Out                137L
#define Location_Kaserne                157L
#define Item_Dicker_Man                9295L
#define Loot_eine_Vase                9609L
#define Person_Justin_White                9840L
#define Location_Lambeth_Road                149L
#define Loot_Zigarren                9610L
//#define CompleteLoot_LastLoot                35L
#define Item_Laubbaum                9252L
//#define London_Jail                31L
#define Location_Victoria                136L
//#define Location_Hotel                142L
#define Item_Kerzenstaender                9282L
#define Item_Pult                9215L
#define Item_Leiter                9307L
//#define Person_Old_Matt                36L
#define Person_Tina_Olavson                9856L
#define Location_Tate_Gallery_Out                168L
#define Item_Schreibtisch                9240L
#define Item_Flugzeug_                9245L
#define Item_Speisetisch                9303L
#define Tool_Stromgenerator                9519L
#define Item_Koenigs_Figur                9298L
#define Item_Sitzbank                9302L
#define Tool_Funkgeraet                9527L
#define Location_Old_Peoples_Home                113L
#define Tool_Bohrwinde                9513L
#define Car_Ford_Model_T__1926                23L
#define Item_Ausstellungskasten                9243L
#define Item_Beefeater                9220L
#define Person_Mike_Seeger                9810L
#define Item_Wertschrank                9204L
#define Location_Bank_Out                139L
#define Person_Helen_Parker                9820L
#define Location_Bank                138L
#define Item_Mauertor                9250L
#define Item_Stechuhr                9266L
#define Item_Lokomotive_Kabine                294L
#define Tool_Winkelschleifer                9511L
#define Car_Opel_Olympia_1953                6L
#define Person_Miguel_Garcia                9809L
//#define Ability_Autos                173L
#define Item_Kroehnungsstuhl                9276L
#define Item_Unbekannter                9296L
#define Item_groszer_Koffer                9301L
#define Item_Lokomotive_vorne_rechts                290L
#define Location_Cars_Vans_Out                96L
#define Person_Mary_Bolton                9824L
#define LSOLock_Buntbartschloss                9101L
#define Loot_Sparstrumpf                9616L
#define Person_Mike_Kahn                9838L
#define Tool_Glasschneider                9524L
#define Loot_Bibel                9619L
#define Item_Kreuz                9236L
#define Person_John_Gludo                9822L
#define Item_Steuerkasten                9213L
#define Item_Scheinwerfer1                9271L
#define Item_Fichte                9256L
#define Loot_Juwelen                9603L
#define Tool_Stethoskop                9517L
#define Loot_Hottentotten                9638L
#define Person_Margrete_Briggs                9828L
#define Item_Stahlschrank                9228L
#define Location_Chiswick                126L
#define Item_Badewanne                9222L
#define Loot_Miss_World                9645L
#define Person_Ben_Riggley                9844L
#define Person_Phil_Ciggy                9857L
#define Person_Luthmilla_Nervesaw                9842L
#define Loot_Bronzestatue                9620L
#define Item_Billardtisch                9217L
#define Item_Alarmanlage_Z3                9210L
#define Item_Kasten                9203L
#define Item_Mauerecke                9209L
#define Car_Standard_Vanguard_1950                7L
#define Car_Standard_Vanguard_1951                8L
#define Item_Kirchenbank                9274L
#define Tool_Fusz                9525L
#define Car_Jeep_1940                1L
#define Car_Standard_Vanguard_1953                9L
//#define Environment_TheClou                28L
#define Item_Kiste                9288L
#define Item_Lokomotive_vorne_links                291L
#define Loot_Koffer                9635L
#define Person_Melanie_Morgan                9853L
#define Item_Holzkiste                9304L
#define Car_Jeep_1945                2L
#define Person_Rod_Masterson                9855L
#define Location_Highgate                115L
#define Car_Bentley_Continental_Typ_R_1952                24L
#define Location_Tate_Gallery                167L
#define Car_Bentley_Continental_Typ_R_1953                25L
#define Item_Skelett                9247L
#define Item_WC                9214L
//#define Evidence_Evidence_1                33L
#define Person_Eric_Pooly                9825L
#define Item_Jack_the_Ripper_Figur                9297L
#define Loot_Delikatessen                9604L
#define Location_Hotel_Room                98L
#define Person_Sabien_Pardo                9846L
#define Person_Elisabeth_Schmitt                9827L
#define Location_Trainstation                152L
#define Person_Tom_Cooler                9849L
#define Person_Robert_Bull                9818L
//#define Location_Tools_Shop                101L
#define Item_Flugzeug__                9246L
#define Loot_Toilettetasche                9617L
#define Person_Margret_Allen                9807L
#define Item_Weihwasserbecken                9280L
#define Car_Morris_Minor_1950                18L
#define Person_Mathew_Black                9808L
#define Location_Tobacconist_Out                104L
#define Location_Monastery                154L
#define Person_Pater_James                9848L
#define Car_Morris_Minor_1953                19L
#define Person_Garry_Stevenson                9834L
#define Location_Parking                145L
//#define Location_Pooly                148L
#define Item_Fenster                9234L
#define Car_Triumph_Roadstar_1947                3L
#define Location_Madame_Tussaud                160L
#define Item_Tisch                9219L
#define Item_Altar                9275L
#define Loot_Chinavase                9623L
#define Car_Triumph_Roadstar_1949                4L
#define Person_Prof_Emil_Schmitt                9826L
#define Location_Ham_Out                123L
#define Location_Buckingham_Palace_Out                170L
#define Loot_Stone_of_Cone                9648L


enum LootNameE 
{
	Kein_Name, 
	Eine_Zigarrenkiste, 
	Ein_Tiefgekuehlter_Hummer, 
	Eine_Cartier_Uhr, 
	Ein_Sanskrit_Buch, 
	Ein_Autogramm_von_Oscar_Wilde,
	Ein_Beauty_Case, 
	Ein_Sparstrumpf, 
	Eine_Toilettetasche, 
	Die_Gebeine_von_Karl_Marx, 
	Eine_Bibel, Eine_Bronzestatue, 
	Ein_Silbergeschirr, 
	Eine_Buddhastatue,
	Eine_Chinavase, 
	Der_Stein_von_Rosetta, 
	Die_Magna_Charta, 
	Eine_Muenzsammlung, 
	Ein_Goldring, 
	Ein_Saurierknochen, 
	Eine_Insektensammlung, 
	Ein_Bernstein,
	Das_Jadepferd, 
	Eine_Victorianische_Vase, 
	Die_Kronjuwelen, 
	Der_Ring_des_Abtes, 
	Ein_Koffer, 
	Ein_Dokument, 

	//** Profidisk *************
	Sherlock_Holmes_Opiumpfeife,
	Eine_Hottentotten_Figur, 
	Eine_Batman_Figur, 
	Ein_Dicker_Man, 
	Ein_Unbekannter, 
	Eine_Jack_the_Ripper_Figur, 
	Eine_Koenigs_Figur, 
	Eine_Wache_Figur,
	Eine_Miss_World_1952_Figur, 
	Eine_Formel, 
	Die_Unterhose_der_Queen, 
	Der_Stone_of_Cone
};
enum SexE 
{
	Maennlich, 
	Weiblich
};
enum ItemE 
{
	Holztuer, 
	Stahltuer, 
	Ausstellungskasten, 
	Kasse, 
	Kasten, 
	Schreibtisch, 
	Nachtkaestchen, 
	Wertschrank, 
	Stahlschrank, 
	Panzerschrank, 
	Tresorraum,
	Mauer, 
	Mauerecke, 
	Steinmauer, 
	Mauertor, 
	Zaun, 
	Zauntor, 
	Saeule, 
	Fenster, 
	Alarmanlage_Z3, 
	Alarmanlage_X3, 
	Alarmanlage_Top, 
	Mikrophon, 
	Stechuhr, 
	Scheinwerfer,
	Lichtschranke, 
	Steuerkasten, 
	Grab, 
	Grabhuegel, 
	Kreuz, 
	Kranz, 
	Gruft, 
	Treppe, 
	Sockel, 
	WC, 
	Sessel, 
	Pult, 
	Statue, 
	Beutesack, 
	Bett, 
	Billardtisch, 
	Sofa, 
	Spiegel,
	Tisch, 
	Kamin, 
	Badewanne, 
	Tresen, 
	Gemaelde, 
	Klavier, 
	Buecherregal, 
	Skelett, 
	Dinosaurier, 
	Elch, 
	Kanone, 
	Beefeater, 
	Busch, 
	Strauch, 
	Gebuesch, 
	Laubbaum,
	Nadelbaum, 
	Hecke, 
	Hantel, 
	Regal, 
	Flugzeugheck, 
	linker_Flugzeugfluegel, 
	rechter_Flugzeugfluegel, 
	Bild, 
	Vase, 
	Kuehlschrank, 
	Wache, 

	//** Profidisk *************
	Beichtstuhl, 
	Kirchenbank,
	Altar, 
	Kroehnungsstuhl, 
	Opferkassa, 
	Schrein, 
	Kerzen, 
	Weihwasserbecken, 
	Heiligenstatue, 
	Kerzenstaender, 
	Kirchenkreuz, 
	Gelaender, 
	verzierte_Saeule,
	Regal_mit_Chemikalien, 
	Versuchstisch, 
	Kiste, 
	Fasz, 
	leere_Palette, 
	Palette_mit_Schachteln, 
	Absperrung, 
	Hottentotten_Figur, 
	Batman_Figur, 
	Dicker_Man,
	Unbekannter, 
	Jack_the_Ripper_Figur, 
	Koenigs_Figur, 
	Wache_Figur,
	Miss_World_1952, 
	groszer_Koffer, 
	Sitzbank, 
	Speisetisch, 
	Holzkiste, 
	Palette_mit_Kisten, 
	Postsack, Leiter, 
	Zuggelaender, 
	Lokomotive_vorne_rechts,
	Lokomotive_vorne_links, 
	Lokomotive_oben, 
	Lokomotive_seitlich, 
	Lokomotive_Kabine, 
	Lokomotive_Tuer
};
enum ColorE 
{
	Grau, 
	Gruen, 
	Tuerkis, 
	Violett, 
	Beige, 
	Silber, 
	Rot, 
	Hellgruen, 
	Blau, 
	Gelb
};
enum AbilityE 
{
	Autos, 
	Schloesser, 
	Sprengstoff, 
	Safes, 
	Elektronik, 
	Aufpassen, 
	Kampf
};
enum LockE 
{
	Buntbartschloss, 
	Kombinationsschloss, 
	Zylinderschloss
};
enum LandE 
{
	GB, 
	USA, 
	Italien, 
	Deutschland
};
enum JobE 
{
	Detektiv, 
	Sprengmeister, 
	Schlosser, 
	Hehler, 
	Haendler, 
	Taxifahrer, 
	Mechaniker, 
	Elektriker, 
	Sekretaerin, 
	Anwalt, 
	Unbekannt, 
	Profiboxer,
	Angestellte, 
	Soldat, 
	Student, 
	Literat, 
	Professor, 
	Hausfrau, 
	Politiker, 
	Abenteurer, 
	Agent, 
	Musiker, 
	Pilot, 
	Kellner, 
	Rezeptionist, 
	Polizeichef, 
	Polizist,
	Moench, 
	Inspektor, 
	Pizzafahrer, 
	Kellner_und_Student, 

	//** Profidisk *************
	Arbeiter, 
	Butler, 
	Krankenschwester
};
enum LootE 
{
	Ein_Bild, 
	Gold, Geld, 
	Juwelen, 
	Delikatessen, 
	Eine_Statue, 
	Eine_Kuriositaet, 
	Ein_historisches_Kunstobjekt, 
	Gebrauchsartikel, 
	Eine_Vase
};


#define Object_Item              9905L
#define Object_Item_Size         sizeof(struct _Item)
struct _Item
{
   TCenum(ItemE) Type;
   unsigned short OffsetFact;
   unsigned short HExactXOffset;
   unsigned short HExactYOffset;
   unsigned short HExactWidth;
   unsigned short HExactHeight;
   unsigned short VExactXOffset;
   unsigned short VExactYOffset;
   unsigned short VExactWidth;
   unsigned short VExactHeight;
   unsigned char Size;
   unsigned char ColorNr;

};
typedef struct _Item *Item;

#define Object_London              9908L
#define Object_London_Size         sizeof(struct _London)
struct _London
{
   unsigned char Useless;

};
typedef struct _London *London;

#define Object_Evidence              9909L
#define Object_Evidence_Size         sizeof(struct _Evidence)
struct _Evidence
{
   unsigned long pers;
   unsigned char Recognition;
   unsigned char WalkTrail;
   unsigned char WaitTrail;
   unsigned char WorkTrail;
   unsigned char KillTrail;
   unsigned char CallTrail;
   unsigned char PaperTrail;
   unsigned char FotoTrail;

};
typedef struct _Evidence *Evidence;

#define Object_Location              9903L
#define Object_Location_Size         sizeof(struct _Location)
struct _Location
{
   unsigned long LocationNr;
   unsigned short OpenFromMinute;
   unsigned short OpenToMinute;

};
typedef struct _Location *Location;

#define Object_Person              9900L
#define Object_Person_Size         sizeof(struct _Person)
struct _Person
{
   unsigned short PictID;
   TCenum(JobE) Job;
   TCenum(SexE) Sex;
   char Age;
   unsigned char Health;
   unsigned char Mood;
   unsigned char Intelligence;
   unsigned char Strength;
   unsigned char Stamina;
   unsigned char Loyality;
   unsigned char Skill;
   unsigned char Known;
   unsigned char Popularity;
   unsigned char Avarice;
   unsigned char Panic;
   unsigned char KnownToPolice;
   unsigned long TalkBits;
   unsigned char TalkFileID;
   unsigned char OldHealth;
};
typedef struct _Person *Person;

#define Object_Car              9902L
#define Object_Car_Size         sizeof(struct _Car)
struct _Car
{
   unsigned short PictID;
   TCenum(LandE) Land;
   unsigned long Value;
   unsigned short YearOfConstruction;
   TCenum(ColorE) ColorIndex;
   unsigned char Strike;
   unsigned long Capacity;
   unsigned char PS;
   unsigned char Speed;
   unsigned char State;
   unsigned char MotorState;
   unsigned char BodyWorkState;
   unsigned char TyreState;
   unsigned char PlacesInCar;

};
typedef struct _Car *Car;

#define Object_Tool              9906L
#define Object_Tool_Size         sizeof(struct _Tool)
struct _Tool
{
   unsigned short PictID;
   unsigned long Value;
   unsigned char Danger;
   unsigned char Volume;
   unsigned char Effect;

};
typedef struct _Tool *Tool;

#define Object_CompleteLoot              9911L
#define Object_CompleteLoot_Size         sizeof(struct _CompleteLoot)
struct _CompleteLoot
{
   unsigned long Bild;
   unsigned long Gold;
   unsigned long Geld;
   unsigned long Juwelen;
   unsigned long Delikates;
   unsigned long Statue;
   unsigned long Kuriositaet;
   unsigned long HistKunst;
   unsigned long GebrauchsArt;
   unsigned long Vase;
   unsigned long TotalWeight;
   unsigned long TotalVolume;

};
typedef struct _CompleteLoot *CompleteLoot;

#define Object_Ability              9904L
#define Object_Ability_Size         sizeof(struct _Ability)
struct _Ability
{
   TCenum(AbilityE) Name;
   unsigned long Use;
};
typedef struct _Ability *Ability;

#define Object_Environment              9907L
#define Object_Environment_Size         sizeof(struct _Environment)
struct _Environment
{
   unsigned char MattHasHotelRoom;
   unsigned char MattHasIdentityCard;
   unsigned char WithOrWithoutYou;
   unsigned char MattIsInLove;
   unsigned char SouthhamptonHappened;
   unsigned char Present;
   unsigned char FirstTimeInSouth;
   unsigned char PostzugDone;

};
typedef struct _Environment *Environment;

#define Object_Player              9901L
#define Object_Player_Size         sizeof(struct _Player)
struct _Player
{
   unsigned long Money;
   unsigned long StolenMoney;
   unsigned long MyStolenMoney;
   unsigned char NrOfBurglaries;
   unsigned char JobOfferCount;
   unsigned char MattsPart;
   unsigned long CurrScene;
   unsigned long CurrDay;
   unsigned long CurrMinute;
   unsigned long CurrLocation;
};
typedef struct _Player *Player;

#define Object_LSOLock              9912L
#define Object_LSOLock_Size         sizeof(struct _LSOLock)
struct _LSOLock
{
   TCenum(LockE) Type;

};
typedef struct _LSOLock *LSOLock;

#define Object_LSObject              9913L
#define Object_LSObject_Size         sizeof(struct _LSObject)
struct _LSObject
{
   unsigned short us_OffsetFact;
   unsigned short us_DestX;
   unsigned short us_DestY;
   unsigned char uch_ExactX;
   unsigned char uch_ExactY;
   unsigned char uch_ExactX1;
   unsigned char uch_ExactY1;
   unsigned char uch_Size;
   unsigned char uch_Visible;
   unsigned char uch_Chained;
   unsigned long ul_Status;
   unsigned long Type;
};
typedef struct _LSObject *LSObject;

#define Object_Loot              9910L
#define Object_Loot_Size         sizeof(struct _Loot)
struct _Loot
{
   TCenum(LootE) Type;
   TCenum(LootNameE) Name;
   unsigned long Volume;
   unsigned short Weight;
   unsigned short PictID;

};
typedef struct _Loot *Loot;

/*
 * Data header file created with dc
 * (c) 1993 by ...and avoid panic by
 *
 * Database (offset, size) : tcBuild (500000, 10000)
 */

#define DB_tcBuild_OFFSET        500000
#define DB_tcBuild_SIZE          10000


#define Relation_isGuardedby      500003
#define isGuardedbyP(l,r,p,c)     AskP   (dbGetObject (l), 500003, dbGetObject (r), p, c)
#define isGuardedby(l,r)          AskP   (dbGetObject (l), 500003, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define isGuardedbyAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500003, BuildObjectList); }
#define isGuardedbySetP(l,r,p)    SetP   (dbGetObject (l), 500003, dbGetObject (r), p)
#define isGuardedbySet(l,r)       SetP   (dbGetObject (l), 500003, dbGetObject (r), NO_PARAMETER)
#define isGuardedbyUnSet(l,r)     UnSet  (dbGetObject (l), 500003, dbGetObject (r))
#define isGuardedbyGet(l,r)       GetP   (dbGetObject (l), 500003, dbGetObject (r))

#define Relation_startsWith      500001
#define startsWithP(l,r,p,c)     AskP   (dbGetObject (l), 500001, dbGetObject (r), p, c)
#define startsWith(l,r)          AskP   (dbGetObject (l), 500001, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define startsWithAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500001, BuildObjectList); }
#define startsWithSetP(l,r,p)    SetP   (dbGetObject (l), 500001, dbGetObject (r), p)
#define startsWithSet(l,r)       SetP   (dbGetObject (l), 500001, dbGetObject (r), NO_PARAMETER)
#define startsWithUnSet(l,r)     UnSet  (dbGetObject (l), 500001, dbGetObject (r))
#define startsWithGet(l,r)       GetP   (dbGetObject (l), 500001, dbGetObject (r))

#define Relation_consistsOf      500002
#define consistsOfP(l,r,p,c)     AskP   (dbGetObject (l), 500002, dbGetObject (r), p, c)
#define consistsOf(l,r)          AskP   (dbGetObject (l), 500002, dbGetObject (r), NO_PARAMETER, CMP_NO)
#define consistsOfAll(l,f,t)     { SetObjectListAttr (f,t); AskAll (dbGetObject (l), 500002, BuildObjectList); }
#define consistsOfSetP(l,r,p)    SetP   (dbGetObject (l), 500002, dbGetObject (r), p)
#define consistsOfSet(l,r)       SetP   (dbGetObject (l), 500002, dbGetObject (r), NO_PARAMETER)
#define consistsOfUnSet(l,r)     UnSet  (dbGetObject (l), 500002, dbGetObject (r))
#define consistsOfGet(l,r)       GetP   (dbGetObject (l), 500002, dbGetObject (r))


#define Building_Suterbys                509012L
#define Building_British_Museum                509013L
#define LSArea_TobaEta0                508001L
#define LSArea_ToweEta0                508041L
#define LSArea_ToweEta1                508042L
#define LSArea_TateEta0                508111L
#define Building_Tante_Emma_Laden                509001L
#define Building_Westminster_Abbey                509103L
#define LSArea_ToweEta2                508043L
#define LSArea_SothEta0                508028L
#define LSArea_VictEta0                508037L
#define LSArea_ChisEta0                508025L
#define Building_Bulstrode_Chemestry_Ltd                509108L
#define LSArea_DownEta0                508107L
#define Building_Bank_of_England                509017L
#define Building_Grab_von_Karl_Marx                509006L
#define Building_Villa                509007L
#define LSArea_KenwEta0                508016L
#define Building_Pink_Villa                509004L
#define LSArea_HamHEta0                508019L
#define Building_Seniorenheim                509005L
#define Building_Baker_Street                509101L
#define LSArea_NatiEta0                508035L
#define Building_Natural_Museum                509014L
#define Building_Downing_Street_10                509104L
#define LSArea_BuckEta0                508113L
#define LSArea_BuckEta1                508114L
#define Building_Kenwood_House                509008L
#define LSArea_TussEta0                508103L
#define LSArea_VillEta0                508014L
#define LSArea_VillEta1                508015L
#define Police_Waechter_1                508500L
#define LSArea_AntiEta0                508007L
#define Police_Waechter_2                508501L
#define Police_Waechter_3                508502L
#define Building_Old_Curiosity_Shop                509003L
#define Police_Waechter_4                508503L
#define Building_Chiswick_House                509011L
#define LSArea_BritEta0                508031L
#define LSArea_BritEta1                508032L
#define LSArea_BulCEta0                508115L
#define LSArea_BulCEta1                508116L
#define LSArea_JuweEta0                508005L
#define LSArea_SeniEta0                508011L
#define Building_Ham_House                509009L
#define Building_National_Galery                509015L
#define LSArea_NatuEta0                508033L
#define LSArea_WestEta0                508105L
#define Building_Buckingham_Palace                509107L
#define Building_Postzug                509105L
#define LSArea_BakeEta0                508101L
#define LSArea_BakeEta1                508102L
#define Building_Victoria_und_Albert_Museum                509016L
#define Building_Starford_Kaserne                509019L
#define Building_Tower_of_London                509018L
#define Building_Juwelier                509002L
#define LSArea_MarxEta0                508013L
#define LSArea_AuntEta0                508003L
#define LSArea_KaseEta0                508044L
#define LSArea_KaseEta1                508045L
#define LSArea_KaseEta2                508046L
#define LSArea_TranEta0                508009L
#define LSArea_TranEta1                508010L
#define Building_Tate_Gallery                509106L
#define LSArea_TraiEta0                508109L
#define Building_Osterly_Park_House                509010L
#define LSArea_OsteEta0                508022L
#define Building_Kiosk                509000L
#define Building_Madame_Tussaud                509102L
#define LSArea_BankEta0                508039L


enum RouteE {Innenstadt, Stadt, Landstrasse, Autobahn};




#define Object_LSRoom              509993L
#define Object_LSRoom_Size         sizeof(struct _LSRoom)
struct _LSRoom
{
   unsigned short us_LeftEdge;
   unsigned short us_TopEdge;
   unsigned short us_Width;
   unsigned short us_Height;

};
typedef struct _LSRoom *LSRoom;

#define Object_Building              509990L
#define Object_Building_Size         sizeof(struct _Building)
struct _Building
{
   unsigned long LocationNr;
   unsigned short PoliceTime;
   unsigned short GTime;
   unsigned char Exactlyness;
   unsigned char GRate;
   unsigned char Strike;
   unsigned long Values;
   TCenum(RouteE) EscapeRoute;
   unsigned char EscapeRouteLength;
   unsigned char RadioGuarding;
   unsigned char MaxVolume;
   unsigned char GuardStrength;
   unsigned short CarXPos;
   unsigned short CarYPos;
   unsigned char DiskId;

};
typedef struct _Building *Building;

#define Object_Police              509991L
#define Object_Police_Size         sizeof(struct _Police)
struct _Police
{
   unsigned short PictID;
   unsigned char LivingID;

};
typedef struct _Police *Police;

#define Object_LSArea              509992L
#define Object_LSArea_Size         sizeof(struct _LSArea)
struct _LSArea
{
   unsigned short us_Coll16ID;
   unsigned short us_Coll32ID;
   unsigned short us_Coll48ID;
   unsigned short us_PlanColl16ID;
   unsigned short us_PlanColl32ID;
   unsigned short us_PlanColl48ID;
   unsigned short us_FloorCollID;
   unsigned short us_PlanFloorCollID;
   unsigned short us_Width;
   unsigned short us_Height;
   unsigned long ul_ObjectBaseNr;
   unsigned char uch_Darkness;
   unsigned short us_StartX0;
   unsigned short us_StartX1;
   unsigned short us_StartX2;
   unsigned short us_StartX3;
   unsigned short us_StartX4;
   unsigned short us_StartX5;
   unsigned short us_StartY0;
   unsigned short us_StartY1;
   unsigned short us_StartY2;
   unsigned short us_StartY3;
   unsigned short us_StartY4;
   unsigned short us_StartY5;
};
typedef struct _LSArea *LSArea;


/* Now follow the double-assigned constants.
depending on the version they must be assigned different, this is done via macros */

typedef enum {  //27 overlaps
	_Ability_Kampf,
	_Location_Cars_Vans_Office,
	_London_London_1,
	_Location_Fat_Mans_Pub,
	_Player_Player_1,
	_Ability_Sprengstoff,
	_Location_Highgate_Out,
	_Ability_Elektronik,
	_Location_Holland_Street,
	_Location_Parker,
	_London_Escape,
	_Location_Walrus,
	_Location_Maloya,
	_Location_Nirvana,
	_Location_Policestation,
	_Ability_Aufpassen,
	_Ability_Schloesser,
    _Ability_Safes,
	_CompleteLoot_LastLoot,
	_London_Jail,
	_Location_Hotel,
	_Person_Old_Matt,
	_Ability_Autos,
	_Environment_TheClou,
    _Evidence_Evidence_1,
    _Location_Tools_Shop,
    _Location_Pooly   
} DoubleAssignValueE;

ulong getValue(DoubleAssignValueE val);

#define Ability_Kampf        getValue(_Ability_Kampf)
#define Location_Cars_Vans_Office        getValue(_Location_Cars_Vans_Office)
#define London_London_1        getValue(_London_London_1)
#define Location_Fat_Mans_Pub        getValue(_Location_Fat_Mans_Pub)
#define Player_Player_1        getValue(_Player_Player_1)
#define Ability_Sprengstoff        getValue(_Ability_Sprengstoff)
#define Location_Highgate_Out        getValue(_Location_Highgate_Out)
#define Ability_Elektronik        getValue(_Ability_Elektronik)
#define Location_Holland_Street        getValue(_Location_Holland_Street)
#define Location_Parker        getValue(_Location_Parker)
#define London_Escape        getValue(_London_Escape)
#define Location_Walrus        getValue(_Location_Walrus)
#define Location_Maloya        getValue(_Location_Maloya)
#define Location_Nirvana        getValue(_Location_Nirvana)
#define Location_Policestation        getValue(_Location_Policestation)
#define Ability_Aufpassen        getValue(_Ability_Aufpassen)
#define Ability_Schloesser        getValue(_Ability_Schloesser)
#define Ability_Safes        getValue(_Ability_Safes)
#define CompleteLoot_LastLoot        getValue(_CompleteLoot_LastLoot)
#define London_Jail        getValue(_London_Jail)
#define Location_Hotel        getValue(_Location_Hotel)
#define Person_Old_Matt        getValue(_Person_Old_Matt)
#define Ability_Autos        getValue(_Ability_Autos)
#define Environment_TheClou        getValue(_Environment_TheClou)
#define Evidence_Evidence_1        getValue(_Evidence_Evidence_1)
#define Location_Tools_Shop        getValue(_Location_Tools_Shop)
#define Location_Pooly        getValue(_Location_Pooly)
                

#endif   /* MODULE_TCDATA */
