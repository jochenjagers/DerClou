/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "dialog\dialog.h"
#include "dialog\talkappl.h"
#include "sound\mxr.h"

#define DLG_NO_SPEECH	((ulong) -1)
ulong	StartFrame = DLG_NO_SPEECH;
ulong EndFrame = DLG_NO_SPEECH;

struct DynDlgNode
{
	NODE	Link;

	ubyte	KnownBefore;   /* wie gut Sie bekannt sein mÅssen */
	ubyte	KnownAfter;    /* wie gut Sie danach bekannt sind ! */

	uword	Padding;
};


LIST *PrepareQuestions(LIST *keyWords, ulong talkBits, ubyte textID)
{
	LIST *stdQuestionList, *questionList, *preparedList;
	NODE *n, *n2;
	char question[TXT_KEY_LENGTH] = {0};
	ulong r, i;
	char *name;

	preparedList = (LIST*)CreateList(0);
	stdQuestionList = txtGoKey(BUSINESS_TXT, "STD_QUEST");
	questionList = txtGoKey((ulong)textID, "QUESTIONS");

	for (n = (NODE*)LIST_HEAD(keyWords); NODE_SUCC((NODE*)n); n=(NODE*)NODE_SUCC(n))
	{
		r = CalcRandomNr(0L,6L);
		if (n2 = (NODE*)GetNthNode(questionList, r))
		{
			name = NODE_NAME(n2);
			sprintf(question, name, NODE_NAME((NODE*)n));
			CreateNode(preparedList, 0L, question);
		}
		#ifdef THECLOU_DEBUG
		else
		{
			Log("GetNthNode(questionList, %d) = NULL", r);
		}
		#endif
	}

	for (i = 0; i < 32; i++)
	{
		if (talkBits & (1 << i))
		{
			if (n2 = (NODE*)GetNthNode(stdQuestionList, i))
			{
				name = NODE_NAME(n2);
				strcpy(question, name);
				CreateNode(preparedList, 0L, question);
			}
			#ifdef THECLOU_DEBUG
			else
			{
				Log("GetNthNode(stdQuestionList, %d) = NULL", i);
			}
			#endif
		}
	}

	txtGetFirstLine(BUSINESS_TXT, "Bye_says_Matt", question);
	CreateNode(preparedList, 0L, question);

	RemoveList(stdQuestionList);
	RemoveList(questionList);

	return(preparedList);
}

LIST *ParseTalkText(LIST *origin, LIST *bubble, ubyte known)
{
	LIST *keyWords;
	NODE *n, *keyNode;
	char line[TXT_KEY_LENGTH] = {0};
	char key[TXT_KEY_LENGTH] = {0};
	char keyWord[TXT_KEY_LENGTH] = {0};
	ubyte line_pos,mem_pos,key_pos;
	char *mem;
	ubyte snr[10],snr1[10],nr,nr1;
	ulong i;
	ulong sl;

	keyWords = (LIST*)CreateList(0);

	for (n = (NODE*)LIST_HEAD(origin); NODE_SUCC(n); n = (NODE*)NODE_SUCC(n))
	{
		line_pos = key_pos = mem_pos = 0;

		mem = NODE_NAME(n);

		sl = strlen(mem);

		while (mem_pos < sl)
		{
			if (mem[mem_pos] != '[')
			{
				line[line_pos++] = mem[mem_pos++];
			}
			else
			{
				mem_pos++;    /* Klammer Åueberspringen ! */
				key_pos=0;

				while (mem[mem_pos] != ']')
				{
					key[key_pos++] = mem[mem_pos++];
				}

				key[key_pos++] = EOS;

				mem_pos++;    /* Klammer Åberspringen ! */

				for (i = 0; i < 3; i++)    /* Zahlen entfernen */
				{
					snr[i] = key[i];
					snr1[i] = key[key_pos - 4 + i];
				}

				for (i = 3; i < (strlen(key)-3); i++)     /* keyWord */
				{
					keyWord[i-3] = key[i];
				}

				/* umwandeln ! */
				snr[3] = EOS;
				snr1[3] = EOS;
				keyWord[strlen(key) - 6] = EOS;

				nr  = (ubyte)atol((char*)snr);
				nr1 = (ubyte)atol((char*)snr1);

				/* keyword einfÅgen */
				for (i = 0; i < strlen(keyWord); i++)
				{
					line[line_pos++] = keyWord[i];
				}

				if (known >= nr)
				{
					keyNode = (NODE*)CreateNode(keyWords, sizeof(struct DynDlgNode), keyWord);
					((struct DynDlgNode*)keyNode)->KnownBefore = nr;
					((struct DynDlgNode*)keyNode)->KnownAfter = nr1;
				}
			}
		}
		line[line_pos++]= EOS;
		CreateNode(bubble, 0L, line);
	}

	return(keyWords);
}

void DynamicTalk(ulong Person1ID, ulong Person2ID, ubyte TalkMode)
{
	char *Extension[4] = { "_UNKNOWN","_KNOWN","_FRIENDLY","_BUSINESS" };
	char *Standard = "STANDARD";
	ubyte known = 0;
	Person p1, p2;
	char key[TXT_KEY_LENGTH] = {0};
	char name[TXT_KEY_LENGTH] = {0};
	ubyte choice = 0, uch_Max = 1, i, uch_Quit, stdcount = 0, j, gencount = 0, textID;
	LIST *origin = NULL, *questions = NULL, *bubble, *keyWords;
	struct DynDlgNode *n;

	p1 = (Person)dbGetObject(Person1ID);
	p2 = (Person)dbGetObject(Person2ID);

	bubble = (LIST*)CreateList(0);

	tcChgPersPopularity(p1, 5);   /* Bekanntheit steigt sehr gering */

	/* je nach Bekanntheitsgrad wird Matt begrÅ·t ! */
	dbGetObjectName(Person2ID, name);
	strcpy(key, name);

	if (TalkMode & DLG_TALKMODE_BUSINESS)
	{
		knowsSet(Person1ID, Person2ID);
		known = 3;     /* Business */
	}
	else
	{
		if (!(knows(Person1ID,Person2ID)))
		{
			known = 0;
			knowsSet(Person1ID,Person2ID);
		}
		else
		{
		    known = 1;  /* MOD - kein "FRIENDLY mehr mîglich!" */
		}
	}

	strcat(key, Extension[known]);

	if (p2->TalkFileID)
	{
		textID = TALK_1_TXT;
	}
	else
	{
	    textID = TALK_0_TXT;
	}

	if (!(txtKeyExists(textID, key)))
	{
		strcpy(key,Standard);
		strcat(key,Extension[known]);
	}

	do
    {
		origin    = txtGoKey(textID, key);
		keyWords  = ParseTalkText(origin, bubble, p2->Known);
		questions = PrepareQuestions(keyWords, p2->TalkBits, textID);

		if (choice < (uch_Max - stdcount))
		{
			SetPictID(p2->PictID);
			Bubble(bubble, 0, 0L, 0L);
		}

		SetPictID(MATT_PICTID);
		choice = Bubble(questions, 0, 0L, 0L);

		//uch_Max = GetNrOfNodes(questions) - 1;
		uch_Max = GetNrOfNodes(questions);
		if (uch_Max) uch_Max--;
		uch_Quit = uch_Max;

		for (i = 0, stdcount = 0; i < 32; i++)   /* Std Fragen zÑhlen */
		{
			if (p2->TalkBits & (1 << i))
			{
				stdcount++;
			}
		}

		//gencount = uch_Max - stdcount;
		if (uch_Max > stdcount) gencount = uch_Max - stdcount;
		else gencount = 0;

		if (choice < gencount)
		{
			if (n = (struct DynDlgNode*)GetNthNode(keyWords, (ulong)choice))
			{
				strcpy(key, name);
				strcat(key, "_");
				strcat(key, NODE_NAME(n));

				if (n->KnownAfter > p2->Known)
				{
					p2->Known = n->KnownAfter;
				}
			}
			#ifdef THECLOU_DEBUG
			else
			{
				Log("GetNthNode(keyWords, %d) = NULL", choice);
			}
			#endif
		}

		if (choice >= gencount && choice < uch_Quit)
		{
			for (i = 0; i < 32; i++)   /*  start at the first set bit! */
			{
				if ((1<<i) & p2->TalkBits)
				{
					break;
				}
			}

			for (j = 0; (i < 32) && (j != (choice - gencount)); i++)
			{
				if ((1<<i) & p2->TalkBits)
				{
					j++;
				}
			}

			switch (i)
			{
				case 0:
					tcJobOffer (p2);
					tcChgPersPopularity(p1, 10);
				break;
				case 1:
					tcMyJobAnswer (p2);
				break;
				case 2:
					tcPrisonAnswer (p2);
				break;
				case 3:
					tcAbilityAnswer(Person2ID);
				break;
			}
		}

		RemoveList(keyWords);
		RemoveList(origin);
		RemoveList(questions);
		RemoveNode(bubble,NULL);
	} while (choice != uch_Quit);

	RemoveList(bubble);
}

void PlayFromCDROM(void)
{
	if ((StartFrame != DLG_NO_SPEECH) && (EndFrame != DLG_NO_SPEECH))
	{
		sndFading(16);
		CDROM_PlayAudioSequence(2, StartFrame, EndFrame);
	}
}

ubyte Say(ulong TextID,ubyte activ,uword person,char *text)
{
	LIST *bubble;
	ubyte choice;
	char keys[TXT_KEY_LENGTH] = {0};
	char wavName[256];

	if (!bCDRom) {
		bubble = txtGoKey(TextID,(char*)text);

		if (person != (uword)-1) {
			SetPictID(person);
		}

		choice = Bubble(bubble, activ, NULL, 0L);
		RemoveList(bubble);
	} else {
		bubble = txtGoKey(TextID,text);

		if (person != (uword) -1) {
			SetPictID(person);
		}

        //the voice output must be started from the procedure "Bubble"
        //because after the start of the voice output no access to the CDROM
        //may occur (The voice output would be otherwise interrupted)

		if (CDRomInstalled) {
			if (txtKeyExists(CDROM_TXT, text)) {
				txtGetFirstLine(CDROM_TXT, text, keys);

				StartFrame = (txtGetKeyAsULONG(1,keys) * 60L + txtGetKeyAsULONG(2, keys)) * 75L + txtGetKeyAsULONG(3, keys);
				EndFrame = (txtGetKeyAsULONG(4,keys) * 60L + txtGetKeyAsULONG(5, keys)) * 75L + txtGetKeyAsULONG(6, keys);

				choice = Bubble(bubble,activ,NULL,0L);
			} else {
				StartFrame = DLG_NO_SPEECH;
				EndFrame = DLG_NO_SPEECH;

				choice = Bubble(bubble,activ,NULL,0L);
			}

			CDROM_StopAudioTrack();
		} else {
			dskBuildPathName(AUDIO_DIRECTORY, text, wavName);
			strcat(wavName, ".wav");

			sndFading(16);

			MXR_SetInput(pAudioMixer, MXR_INPUT_VOICE, MXR_CreateInputWAV(wavName));
			MXR_SetInputVolume(pAudioMixer, MXR_INPUT_VOICE, Config.VoiceVolume);	// 2018-09-25
			
			choice = Bubble(bubble,activ,NULL,0L);
			
			MXR_SetInput(pAudioMixer, MXR_INPUT_VOICE, NULL);
		}
		sndFading(0);

		StartFrame = DLG_NO_SPEECH;
		EndFrame = DLG_NO_SPEECH;

		RemoveList(bubble);
	}

	return(choice);
}

ulong Talk(void)
{
	ulong succ_event_nr=0L,locNr,personID;
	LIST  *bubble;
	ubyte choice;
	char helloFriends[TXT_KEY_LENGTH] = {0};

	inpTurnESC (0);

	if (locNr= GetObjNrOfLocation(GetLocation))
	{
		hasAll(locNr, OLF_PRIVATE_LIST|OLF_INCLUDE_NAME|OLF_INSERT_STAR, Object_Person);
		bubble = ObjectListPrivate;

		if (!(LIST_EMPTY(bubble)))
		{
			inpTurnESC (1);

			txtGetFirstLine(BUSINESS_TXT, "NO_CHOICE", helloFriends);
			ExpandObjectList(bubble, helloFriends);

			if (ChoiceOk((choice = Bubble(bubble, 0, 0L, 0L)), GET_OUT, bubble))
			{
				personID = ((struct ObjectNode *)GetNthNode(bubble,(ulong)choice))->nr;

				inpTurnESC (0);

				if (PersonWorksHere(personID, locNr))
				{
					DynamicTalk(Person_Matt_Stuvysunt,personID, DLG_TALKMODE_BUSINESS);
				}
				else
				{
				    DynamicTalk(Person_Matt_Stuvysunt,personID, DLG_TALKMODE_STANDARD);
				}
			}
		}
		else
		{
		    Say(BUSINESS_TXT,0,MATT_PICTID,"NOBODY HERE");
		}

		RemoveList(bubble);
	}

	inpTurnESC (1);

	return(succ_event_nr);
}
