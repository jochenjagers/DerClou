/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

/*
 * SetFullEnvironment (Cheats!)
 */

static void SetFullAbilities(void)
{
	hasSetP(Person_Matt_Stuvysunt,  Ability_Elektronik, 251);
	hasSetP(Person_Matt_Stuvysunt,  Ability_Schloesser, 210);
	hasSetP(Person_Matt_Stuvysunt,  Ability_Aufpassen,  180);
	hasSetP(Person_Matt_Stuvysunt,  Ability_Safes, 180);	// 2015-01-04 LucyG: was Ability_Schloesser

	hasSetP(Person_Marc_Smith,      Ability_Autos,      255);
	hasSetP(Person_Mohammed_Abdula, Ability_Kampf,      255);
}

static void SetFullCars(void)
{
	hasSet(Person_Matt_Stuvysunt, Car_Triumph_Roadstar_1947);
	hasSet(Person_Matt_Stuvysunt, Car_Triumph_Roadstar_1949);
	hasSet(Person_Matt_Stuvysunt, Car_Cadillac_Club_1952);
	hasSet(Person_Matt_Stuvysunt, Car_Opel_Olympia_1953);
	hasSet(Person_Matt_Stuvysunt, Car_Standard_Vanguard_1950);
	hasSet(Person_Matt_Stuvysunt, Car_Standard_Vanguard_1951);
	hasSet(Person_Matt_Stuvysunt, Car_Standard_Vanguard_1953);
	hasSet(Person_Matt_Stuvysunt, Car_Jaguar_XK_1950);
	hasSet(Person_Matt_Stuvysunt, Car_Pontiac_Streamliner_1944);
	hasSet(Person_Matt_Stuvysunt, Car_Pontiac_Streamliner_1946);
	hasSet(Person_Matt_Stuvysunt, Car_Pontiac_Streamliner_1949);
	hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1936);
	hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1940);
	hasSet(Person_Matt_Stuvysunt, Car_Fiat_Topolino_1942);
	hasSet(Person_Matt_Stuvysunt, Car_Morris_Minor_1948);
	hasSet(Person_Matt_Stuvysunt, Car_Morris_Minor_1950);
	hasSet(Person_Matt_Stuvysunt, Car_Morris_Minor_1953);
	if (bProfidisk)
	{
		hasSet(Person_Matt_Stuvysunt, Car_Rover_75_1949);
		hasSet(Person_Matt_Stuvysunt, Car_Rover_75_1950);
		hasSet(Person_Matt_Stuvysunt, Car_Rover_75_1952);
		hasSet(Person_Matt_Stuvysunt, Car_Ford_Model_T__1926);
		hasSet(Person_Matt_Stuvysunt, Car_Bentley_Continental_Typ_R_1952);
		hasSet(Person_Matt_Stuvysunt, Car_Bentley_Continental_Typ_R_1953);
		hasSet(Person_Matt_Stuvysunt, Car_Fiat_634_N_1936);
		hasSet(Person_Matt_Stuvysunt, Car_Fiat_634_N_1943);
	}
}

static void SetFullTools(void)
{
	hasSet(Person_Matt_Stuvysunt, Tool_Dietrich);
	hasSet(Person_Matt_Stuvysunt, Tool_Hammer);
	hasSet(Person_Matt_Stuvysunt, Tool_Axt);
	hasSet(Person_Matt_Stuvysunt, Tool_Kernbohrer);
	hasSet(Person_Matt_Stuvysunt, Tool_Handschuhe);
	hasSet(Person_Matt_Stuvysunt, Tool_Schuhe);
	hasSet(Person_Matt_Stuvysunt, Tool_Maske);
	hasSet(Person_Matt_Stuvysunt, Tool_Chloroform);
	hasSet(Person_Matt_Stuvysunt, Tool_Brecheisen);
	hasSet(Person_Matt_Stuvysunt, Tool_Winkelschleifer);
	hasSet(Person_Matt_Stuvysunt, Tool_Bohrmaschine);
	hasSet(Person_Matt_Stuvysunt, Tool_Bohrwinde);
	hasSet(Person_Matt_Stuvysunt, Tool_Schloszstecher);
	hasSet(Person_Matt_Stuvysunt, Tool_Schneidbrenner);
	hasSet(Person_Matt_Stuvysunt, Tool_Sauerstofflanze);
	hasSet(Person_Matt_Stuvysunt, Tool_Stethoskop);
	hasSet(Person_Matt_Stuvysunt, Tool_Batterie);
	hasSet(Person_Matt_Stuvysunt, Tool_Stromgenerator);
	hasSet(Person_Matt_Stuvysunt, Tool_Elektroset);
	hasSet(Person_Matt_Stuvysunt, Tool_Schutzanzug);
	hasSet(Person_Matt_Stuvysunt, Tool_Dynamit);
	hasSet(Person_Matt_Stuvysunt, Tool_Elektrohammer);
	hasSet(Person_Matt_Stuvysunt, Tool_Glasschneider);
	hasSet(Person_Matt_Stuvysunt, Tool_Strickleiter);
	hasSet(Person_Matt_Stuvysunt, Tool_Funkgeraet);
}

static void SetFullJobOffers(void)
{
	joinSet(Person_Matt_Stuvysunt, Person_Paul_O_Conner);
	joinSet(Person_Matt_Stuvysunt, Person_Marc_Smith);
	joinSet(Person_Matt_Stuvysunt, Person_Frank_Meier);
	joinSet(Person_Matt_Stuvysunt, Person_Herbert_Briggs);
	joinSet(Person_Matt_Stuvysunt, Person_Tony_Allen);
	joinSet(Person_Matt_Stuvysunt, Person_Margret_Allen);
	joinSet(Person_Matt_Stuvysunt, Person_Mathew_Black);
	joinSet(Person_Matt_Stuvysunt, Person_Miguel_Garcia);
	joinSet(Person_Matt_Stuvysunt, Person_Mike_Seeger);
	joinSet(Person_Matt_Stuvysunt, Person_Albert_Liet);
	joinSet(Person_Matt_Stuvysunt, Person_Mohammed_Abdula);
	joinSet(Person_Matt_Stuvysunt, Person_Serge_Fontane);
	joinSet(Person_Matt_Stuvysunt, Person_Frank_Maloya);
	joinSet(Person_Matt_Stuvysunt, Person_Neil_Grey);
	joinSet(Person_Matt_Stuvysunt, Person_Mark_Hart);
	joinSet(Person_Matt_Stuvysunt, Person_Jim_Danner);
	joinSet(Person_Matt_Stuvysunt, Person_Robert_Bull);
	joinSet(Person_Matt_Stuvysunt, Person_Thomas_Groul);
	joinSet(Person_Matt_Stuvysunt, Person_Helen_Parker);
	joinSet(Person_Matt_Stuvysunt, Person_Dan_Stanford);
	joinSet(Person_Matt_Stuvysunt, Person_Mary_Bolton);
	joinSet(Person_Matt_Stuvysunt, Person_Eric_Pooly);
	joinSet(Person_Matt_Stuvysunt, Person_Prof_Emil_Schmitt);
	joinSet(Person_Matt_Stuvysunt, Person_Elisabeth_Schmitt);
	joinSet(Person_Matt_Stuvysunt, Person_Margrete_Briggs);
	joinSet(Person_Matt_Stuvysunt, Person_Peter_Brook);
	joinSet(Person_Matt_Stuvysunt, Person_Samuel_Rosenblatt);
	joinSet(Person_Matt_Stuvysunt, Person_Lucas_Grull);
	joinSet(Person_Matt_Stuvysunt, Person_Ken_Addison);
	joinSet(Person_Matt_Stuvysunt, Person_Jiri_Poulin);
	joinSet(Person_Matt_Stuvysunt, Person_Garry_Stevenson);
	joinSet(Person_Matt_Stuvysunt, Person_Al_Mel);
	joinSet(Person_Matt_Stuvysunt, Person_Jack_Motart);
	joinSet(Person_Matt_Stuvysunt, Person_Kevin_Smith);
	joinSet(Person_Matt_Stuvysunt, Person_Mike_Kahn);
	joinSet(Person_Matt_Stuvysunt, Person_Frank_De_Silva);
	joinSet(Person_Matt_Stuvysunt, Person_Justin_White);
	joinSet(Person_Matt_Stuvysunt, Person_John_O_Keef);
	joinSet(Person_Matt_Stuvysunt, Person_Luthmilla_Nervesaw);
	joinSet(Person_Matt_Stuvysunt, Person_Thomas_Smith);
	joinSet(Person_Matt_Stuvysunt, Person_Ben_Riggley);
	joinSet(Person_Matt_Stuvysunt, Person_Richard_Doil);
	joinSet(Person_Matt_Stuvysunt, Person_Pater_James);
	if (bProfidisk)
	{
		joinSet(Person_Matt_Stuvysunt, Person_Tom_Cooler);
		joinSet(Person_Matt_Stuvysunt, Person_Sid_Palmer);
		joinSet(Person_Matt_Stuvysunt, Person_Dean_Esters);
		joinSet(Person_Matt_Stuvysunt, Person_Prof_Marcus_Green);
		joinSet(Person_Matt_Stuvysunt, Person_Melanie_Morgan);
		joinSet(Person_Matt_Stuvysunt, Person_Rosi_Lambert);
		joinSet(Person_Matt_Stuvysunt, Person_Rod_Masterson);
		joinSet(Person_Matt_Stuvysunt, Person_Tina_Olavson);
		joinSet(Person_Matt_Stuvysunt, Person_Phil_Ciggy);
		joinSet(Person_Matt_Stuvysunt, Person_Pere_Ubu);
	}
}

static void SetFullInvestigations(void)
{
	hasSet(Person_Matt_Stuvysunt, Building_Kiosk);
	((Building)dbGetObject (Building_Kiosk))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Tante_Emma_Laden);
	((Building)dbGetObject (Building_Tante_Emma_Laden))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Juwelier);
	((Building)dbGetObject (Building_Juwelier))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Old_Curiosity_Shop);
	((Building)dbGetObject (Building_Old_Curiosity_Shop))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Pink_Villa);
	((Building)dbGetObject (Building_Pink_Villa))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Seniorenheim);
	((Building)dbGetObject (Building_Seniorenheim))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Grab_von_Karl_Marx);
	((Building)dbGetObject (Building_Grab_von_Karl_Marx))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Villa);
	((Building)dbGetObject (Building_Villa))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Kenwood_House);
	((Building)dbGetObject (Building_Kenwood_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Ham_House);
	((Building)dbGetObject (Building_Ham_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Osterly_Park_House);
	((Building)dbGetObject (Building_Osterly_Park_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Chiswick_House);
	((Building)dbGetObject (Building_Chiswick_House))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Suterbys);
	((Building)dbGetObject (Building_Suterbys))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_British_Museum);
	((Building)dbGetObject (Building_British_Museum))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Natural_Museum);
	((Building)dbGetObject (Building_Natural_Museum))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_National_Galery);
	((Building)dbGetObject (Building_National_Galery))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Victoria_und_Albert_Museum);
	((Building)dbGetObject (Building_Victoria_und_Albert_Museum))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Bank_of_England);
	((Building)dbGetObject (Building_Bank_of_England))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Tower_of_London);
	((Building)dbGetObject (Building_Tower_of_London))->Exactlyness = 255;
	hasSet(Person_Matt_Stuvysunt, Building_Starford_Kaserne);
	((Building)dbGetObject (Building_Starford_Kaserne))->Exactlyness = 255;
	if (bProfidisk)
	{
		hasSet(Person_Matt_Stuvysunt, Building_Baker_Street);
		((Building)dbGetObject (Building_Baker_Street))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Madame_Tussaud);
		((Building)dbGetObject (Building_Madame_Tussaud))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Westminster_Abbey);
		((Building)dbGetObject (Building_Westminster_Abbey))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Downing_Street_10);
		((Building)dbGetObject (Building_Downing_Street_10))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Postzug);
		((Building)dbGetObject (Building_Postzug))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Tate_Gallery);
		((Building)dbGetObject (Building_Tate_Gallery))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Buckingham_Palace);
		((Building)dbGetObject (Building_Buckingham_Palace))->Exactlyness = 255;
		hasSet(Person_Matt_Stuvysunt, Building_Bulstrode_Chemestry_Ltd);
		((Building)dbGetObject (Building_Bulstrode_Chemestry_Ltd))->Exactlyness = 255;
	}
}

static void SetFullEnvironment(void)
{
	SetFullAbilities();
	SetFullCars();
	SetFullTools();
	SetFullJobOffers();
	SetFullInvestigations();
}
