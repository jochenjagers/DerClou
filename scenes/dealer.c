/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "scenes\scenes.h"

void tcDealerDlg(void)
	{
	ulong locNr = GetObjNrOfLocation(GetLocation);
	Person dealer = NULL;
	ubyte dealerNr, choice = 0;

	if (locNr == Location_Parker)
	{
		dealer = (Person)dbGetObject(Person_Helen_Parker); knowsSet(Person_Matt_Stuvysunt, Person_Helen_Parker); dealerNr = 2;
	}
	else if (locNr == Location_Maloya)
	{
		dealer = (Person)dbGetObject(Person_Frank_Maloya); knowsSet(Person_Matt_Stuvysunt, Person_Frank_Maloya); dealerNr = 0;
	}
	else if (locNr == Location_Pooly)
	{
		dealer = (Person)dbGetObject(Person_Eric_Pooly); knowsSet(Person_Matt_Stuvysunt, Person_Eric_Pooly); dealerNr = 1;
	}

	while((choice != 2) && (choice != GET_OUT))
		{
		choice = Say(BUSINESS_TXT, 0, MATT_PICTID, "DEALER_QUEST");

		switch(choice)
			{
			case 0:             /* womit ? */
				if (locNr == Location_Parker)
				{
					Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_PARKER");
				}
				else if (locNr ==  Location_Maloya)
				{
					Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_MALOYA"); 
				}
				else if (locNr == Location_Pooly)
				{
					Say(BUSINESS_TXT, 0, dealer->PictID, "DEALER_POOLY"); 
				}
				break;
			case 1:             /* offer */
				hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Loot);

				if	(LIST_EMPTY(ObjectList))
					{
					Say(BUSINESS_TXT,0,dealer->PictID,"NO_LOOT");
					AddVTime(17);
					}
				else
					tcDealerOffer(dealer, dealerNr);
				break;
			case 2:
			default:
				break;
			}
		}

	AddVTime(11);
	ShowTime(2);
	}

// 2014-06-29 LucyG : increase price variation for less predictable gameplay
static long moreRandom(long v)
{
	long r;
	r = CalcRandomNr(0, (v / 4) + 1);
	r -= (v / 8);
	v += r;
	return(min(v, 250));
}

void tcDealerOffer(Person dealer, ubyte which)
	{
	// 2014-06-29 LucyG : possible compiler bug fixed
	static const long Price[3*10] =    {  70, 150, 220,  90, 210, 110, 200,   0, 190,  80,   /* maloya */
										 120, 200, 180, 220,  79, 110,   0,   0, 110, 200,   /* pooly */
										 220,  66,   0, 110,   0, 220,   0, 212,  20, 130};  /* parker */
	CompleteLoot comp = (CompleteLoot)dbGetObject (CompleteLoot_LastLoot);

	RemoveList (tcMakeLootList(Person_Matt_Stuvysunt, Relation_has));

	if(comp->Bild)           tcDealerSays(dealer, 0, moreRandom(Price[which*10+0]));
	if(comp->Gold)           tcDealerSays(dealer, 1, moreRandom(Price[which*10+1]));
	if(comp->Geld)           tcDealerSays(dealer, 2, moreRandom(Price[which*10+2]));
	if(comp->Juwelen)        tcDealerSays(dealer, 3, moreRandom(Price[which*10+3]));
	if(comp->Delikates)      tcDealerSays(dealer, 4, moreRandom(Price[which*10+4]));
	if(comp->Statue)         tcDealerSays(dealer, 5, moreRandom(Price[which*10+5]));
	if(comp->Kuriositaet)    tcDealerSays(dealer, 6, moreRandom(Price[which*10+6]));
	if(comp->HistKunst)      tcDealerSays(dealer, 7, moreRandom(Price[which*10+7]));
	if(comp->GebrauchsArt)   tcDealerSays(dealer, 8, moreRandom(Price[which*10+8]));
	if(comp->Vase)           tcDealerSays(dealer, 9, moreRandom(Price[which*10+9]));
	}

void tcDealerSays(Person dealer, ubyte textNr, long perc)
	{
	LIST *lootNames   = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootE");
	LIST *specialLoot = txtGoKey(OBJECTS_ENUM_TXT, "enum_LootNameE");
	LIST *dealerText  = txtGoKey(BUSINESS_TXT, "DEALER_OFFER");
	LIST *dealerOffer = (LIST*)CreateList(0);
	char line[TXT_KEY_LENGTH], symp, i;
	struct ObjectNode *n;
	Person others[3];
	Player player = (Player)dbGetObject(Player_Player_1);

	others[0] = (Person)dbGetObject(Person_Frank_Maloya);
	others[1] = (Person)dbGetObject(Person_Eric_Pooly);
	others[2] = (Person)dbGetObject(Person_Helen_Parker);

	if (perc == 0)
		{
		sprintf(line, NODE_NAME(GetNthNode(dealerText, 4)), NODE_NAME(GetNthNode(lootNames, (ulong) textNr)));
		CreateNode (dealerOffer, 0L, line);

		CreateNode (dealerOffer, 0L, NODE_NAME(GetNthNode(dealerText, 5)));

		SetPictID (dealer->PictID);
		Bubble (dealerOffer, 0, 0L, 0L);
		}
	else
		{
		hasAll(Person_Matt_Stuvysunt, OLF_NORMAL, Object_Loot);
		perc = tcGetDealerPerc(dealer, perc);

		for (n = (struct ObjectNode *)LIST_HEAD(ObjectList); NODE_SUCC(n); n = (struct ObjectNode *) NODE_SUCC(n))
			{
			Loot loot = (Loot)OL_DATA(n);
			ulong price = hasGet(Person_Matt_Stuvysunt, OL_NR(n)), offer;

			offer = tcGetDealerOffer(price, perc);
			offer = max(offer, 1);

			RemoveNode(dealerOffer, NULL);

			if ((loot->Type) == textNr)
				{
				if (loot->Name)
					{
					symp = 10;

					sprintf(line, NODE_NAME(GetNthNode(dealerText, 2)), NODE_NAME(GetNthNode(specialLoot, (ulong) loot->Name)));
					CreateNode (dealerOffer, 0L, line);

					sprintf (line, NODE_NAME(GetNthNode(dealerText, 3)), offer);
					CreateNode (dealerOffer, 0L, line);
					}
				else
					{
					symp = 1;

					sprintf (line, NODE_NAME(GetNthNode(dealerText, 0)), NODE_NAME(GetNthNode(lootNames, (ulong)textNr)));
					CreateNode (dealerOffer, 0L, line);

					sprintf (line, NODE_NAME(GetNthNode(dealerText, 1)), price, offer);
					CreateNode (dealerOffer, 0L, line);
					}

				SetPictID (dealer->PictID);
				Bubble (dealerOffer, 0, 0L, 0L);

				if (!(Say (BUSINESS_TXT, 0, MATT_PICTID, "DEALER_ANSWER")))
					{
					long mattsMoney;

					hasUnSet(Person_Matt_Stuvysunt, OL_NR(n));

					if (GamePlayMode & GP_MORE_MONEY) {	// Lucy 2017-11-08 new cheat
						if (offer > 0) offer = (offer * 3) / 2;
					}

					mattsMoney = max(((offer * (player->MattsPart)) / 100), 1);

					tcAddDealerSymp(dealer, symp);
					tcAddPlayerMoney(mattsMoney);

					player->StolenMoney   += offer;
					player->MyStolenMoney += mattsMoney;

					for (i = 0; i < 3; i++)
						if(dealer != others[i])    tcAddDealerSymp((others[i]), (symp * (-1)));
					}
				}
			}
		}

	RemoveList (specialLoot);
	RemoveList (dealerOffer);
	RemoveList (dealerText);
	RemoveList (lootNames);
	}

LIST *tcMakeLootList(ulong containerID, ulong relID)
	{
	NODE *n;
	Loot loot;
	CompleteLoot comp = (CompleteLoot)dbGetObject(CompleteLoot_LastLoot);
	char data[TXT_KEY_LENGTH];
	ulong value;
	LIST *out = (LIST*)CreateList (0);
	LIST *loots;
	LIST *lootE     = txtGoKey(OBJECTS_ENUM_TXT,"enum_LootE");
	LIST *lootNameE = txtGoKey(OBJECTS_ENUM_TXT,"enum_LootNameE");

	/* Listen initialisieren */

	SetObjectListAttr (OLF_PRIVATE_LIST, Object_Loot);
	AskAll (dbGetObject(containerID), relID, BuildObjectList);
	loots = ObjectListPrivate;

	comp->Bild = comp->Gold = comp->Geld = comp->Juwelen = 0;
	comp->Delikates = comp->Statue = comp->Kuriositaet = 0;
	comp->HistKunst = comp->GebrauchsArt = comp->Vase = 0;

	comp->TotalWeight = comp->TotalVolume = 0;

	/* Liste durcharbeiten */

	if (!(LIST_EMPTY(loots)))
		{
		for (n = (NODE*) LIST_HEAD(loots); NODE_SUCC(n); n = (NODE*) NODE_SUCC(n))
			{
			if (OL_TYPE(n) == Object_Loot)
				{
				loot = (Loot)OL_DATA(n);

				value = GetP (dbGetObject(containerID), relID, loot);

				switch(loot->Type)
					{
					case Ein_Bild:      comp->Bild += value; break;
					case Gold:          comp->Gold += value; break;
					case Geld:          comp->Geld += value; break;
					case Juwelen:       comp->Juwelen += value; break;
					case Delikatessen:  comp->Delikates += value; break;
					case Eine_Statue:   comp->Statue += value; break;
					case Eine_Kuriositaet:             comp->Kuriositaet += value; break;
					case Eine_Vase:                    comp->Vase += value; break;
					case Ein_historisches_Kunstobjekt: comp->HistKunst += value; break;
					case Gebrauchsartikel:             comp->GebrauchsArt += value; break;
					default: break;
					}

				comp->TotalWeight += loot->Weight;
				comp->TotalVolume += loot->Volume;

				if(loot->Name)
					strcpy (data, NODE_NAME(GetNthNode (lootNameE, loot->Name)));
				else
					strcpy (data, NODE_NAME(GetNthNode (lootE, loot->Type)));

				CreateNode (out, 0L, data);

				sprintf (data, "%ld", value);
				CreateNode (out, 0L, data);

				sprintf (data, "%ld", loot->Volume);
				CreateNode (out, 0L, data);

				sprintf (data, "%ld", loot->Weight);
				CreateNode (out, 0L, data);
				}
			}
		}

	RemoveList(lootE);
	RemoveList(lootNameE);
	RemoveList(loots);

	return(out);
	}

