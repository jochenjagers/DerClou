/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

// public header(s)
#include "text\text.h"

// private header(s)
#include "text\text.eh"
#include "text\text.ph"

#include "port\port.h"

// private globals declaration
char   *txtLanguageMark[TXT_LANG_LAST + 1] = {
	"E", "D", "F", "S" };
struct TextControl *txtBase = NULL;
char   keyBuffer[TXT_KEY_LENGTH];

// private functions
static char *txtGetLine(struct Text *txt, ubyte lineNr)
{
	ubyte i;
	char *line = NULL;

	if (txt && txt->txt_LastMark && lineNr)
	{
		line = txt->txt_LastMark;
		i = 0;

		while (i < lineNr)
		{
			if (*line == TXT_CHAR_EOF) // < ((char *)TXT_BUFFER_WORK + txt->txt_Size))
			{
				line = NULL;
				break;
			}

			if (i && (*line == TXT_CHAR_MARK))
			{
				line = NULL;
				break;
			}

			if (*line == TXT_CHAR_EOS)
			{
				line++;	// overread second EOS
				i++;

				if (*line == TXT_CHAR_EOF)
				{
					line = NULL;
					break;
				}

				// overread comments
				while (*(line+1) == TXT_CHAR_REMARK)
				{
					while (*(++line) != TXT_CHAR_EOS);
					line++;	// overread second EOS;
				}
			}

			line++;
		}

		if ((*line == TXT_CHAR_EOF) || (*line == TXT_CHAR_MARK))
			line = NULL;

		if (line >= ((char *)TXT_BUFFER_WORK + txt->txt_Size))
			line = NULL;
	}

	return(line);
}

//  public functions - TEXT
void txtInit(ubyte lang)
{
	char txtListPath[DSK_PATHNAMELENGTH];

	if (txtBase = MemAlloc(sizeof(struct TextControl)))
	{
		txtBase->tc_Texts     = CreateList(0);
		txtBase->tc_Language  = lang;

		dskBuildPathName(TEXT_DIRECTORY, TXT_LIST, txtListPath);

		if (ReadList(txtBase->tc_Texts, sizeof(struct Text), txtListPath, TXT_DISK_ID))
		{
			uword i;

			for (i = 0; i < GetNrOfNodes(txtBase->tc_Texts); i++)
			{
				struct Text *txt = GetNthNode(txtBase->tc_Texts, i);

				txt->txt_Handle   = NULL;
				txt->txt_LastMark = NULL;
				txt->txt_Size     = 0;

				txtLoad(i);
			}
		}
		else
		    NewErrorMsg(No_Mem, __FILE__, __func__, ERR_TXT_READING_LIST);
	}
	else
	    NewErrorMsg(No_Mem, __FILE__, __func__, ERR_TXT_FAILED_BASE);
}

void txtDone(void)
{
	if (txtBase)
	{
		uword i;

		for (i = 0; i < GetNrOfNodes(txtBase->tc_Texts); i++)
			txtUnLoad(i);

		RemoveList(txtBase->tc_Texts);

		MemFree(txtBase, sizeof(struct TextControl));
	}
}

void txtLoad (ulong textId)
{
	struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

	if (txt)
	{
		if (!txt->txt_Handle)
		{
			char txtPath[DSK_PATHNAMELENGTH];
			ubyte *mem;

			dskBuildPathName(TEXT_DIRECTORY, NODE_NAME(txt), txtPath);
			strcat(txtPath, txtLanguageMark[txtBase->tc_Language]);
			strcat(txtPath, TXT_SUFFIX);

			txt->txt_Size = dskFileLength(txtPath);

			// loading text into buffer
			dskLoad(txtPath, TXT_BUFFER_LOAD, TXT_DISK_ID);

			// correcting text
			for (mem = TXT_BUFFER_LOAD; mem < ((ubyte *)TXT_BUFFER_LOAD + txt->txt_Size); mem++)
			{
				*mem ^= TXT_XOR_VALUE;
				if (*mem == 10 || *mem == 13) *mem = TXT_CHAR_EOS;
			}

			// save text into xms
			if (txt->txt_Handle = MemAlloc(txt->txt_Size))
				xmsCopyUp(txt->txt_Handle, 0, TXT_BUFFER_LOAD, txt->txt_Size);
			else
			    NewErrorMsg(No_Mem, __FILE__, __func__, ERR_TXT_NO_XMS);
		}
	}
}

void txtUnLoad(ulong textId)
{
	struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

	if (txt)
	{
		if (txt->txt_Handle)
		{
			MemFree(txt->txt_Handle, txt->txt_Size);

			txt->txt_Handle   = NULL;
			txt->txt_LastMark = NULL;
			txt->txt_Size     = 0;
		}
	}
}

void txtPrepare(ulong textId)
{
	struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

	if (txt)
	{
		xmsCopyDown(txt->txt_Handle, 0, TXT_BUFFER_WORK, txt->txt_Size);
		txt->txt_LastMark = TXT_BUFFER_WORK;
	}
}

void txtUnPrepare(ulong textId)
{
	struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

	if (txt)
		txt->txt_LastMark = NULL;
}

void txtReset(ulong textId)
{
	struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

	if (txt)
		txt->txt_LastMark = TXT_BUFFER_WORK;
}

// public functions - KEY
char *txtGetKey(uword keyNr, char *key)
{
	uword i;

	if (key)
	{
		for (i = 1; i < keyNr; i++)
		{
			while (*key && (*key != TXT_CHAR_KEY_SEPERATOR))
				key++;

			if (!*key)
				break;
			else
			    key++;
		}

		if (*key)
		{
			while (isspace (*key))
				key++;

			if (*key)
			{
				for (i = 0; *key && (*key != TXT_CHAR_KEY_SEPERATOR) && (i < TXT_KEY_LENGTH); i++)
					keyBuffer[i] = *key++;

				keyBuffer[i] = TXT_CHAR_EOS;

				return keyBuffer;
			}
		}
	}

	return NULL;
}

ulong txtGetKeyAsULONG(uword keyNr, char *key)
{
	char *t = txtGetKey(keyNr, key);
	if (!t) return(0);
	return((ulong)atol(t));
}

LIST *txtGoKey(ulong textId, char *key)
{
	LIST *txtList = NULL;
	struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

	if (txt)
	{
		char *LastMark = NULL;

		// MOD: 08-04-94 hg
		// wenn kein key angegeben wurde, soll der n„chste genommen werden
		// -> in LastMark wird die letzte Position zwischengespeichert, da
		// txt->LastMark in txtPrepare neu gesetzt wird!

		// Vorsicht auf Spezialfall: kein Key, Text noch nie benutzt !!

		if (!key && txt->txt_LastMark)
		{
			LastMark = txt->txt_LastMark;
		}

		txtPrepare(textId);

		// Erkl„rung zu +1: LastMark zeigt auf den letzten Schlssel
		// -> ohne "+1" wrde wieder der selbe Schlssel zurckgegeben werden
		if (!key && LastMark)
		{
			txt->txt_LastMark = LastMark + 1;
		}

		for (; txt->txt_LastMark < ((char *)TXT_BUFFER_WORK + txt->txt_Size); txt->txt_LastMark++)
		{
			if (*txt->txt_LastMark == TXT_CHAR_MARK)
			{
				ubyte found = 1;

				if (key)
				{
					char mark[TXT_KEY_LENGTH];

					strcpy(mark, txt->txt_LastMark+1);

					if (strcmp(key, mark) != 0)
					{
						found = 0;
					}
				}

				if (found)
				{
					ubyte i = 1;
					char *line;

					txtList = CreateList(0);

					while (line = txtGetLine(txt, i++))
					{
						CreateNode(txtList, 0, line);
					}

					break;
				}
			}
		}
	}

	if (!txtList)
	{
		Log("Cannot find text %d", textId);
		NewErrorMsg(Disk_Defect, __FILE__, __func__, ERR_TXT_NO_KEY);
	}

	return txtList;
}

LIST *txtGoKeyAndInsert(ulong textId, char *key, ...)
{
	va_list argument;
	LIST *txtList = CreateList(0L), *originList = NULL;
	NODE *node;

	va_start(argument, key);

	originList = txtGoKey(textId, key);

	for (node = LIST_HEAD(originList); NODE_SUCC(node); node = NODE_SUCC(node))
	{
		ubyte i;
		char originLine[256],txtLine[256];

		strcpy(originLine, NODE_NAME(node));
		strcpy(txtLine, NODE_NAME(node));

		for (i = 2; i < strlen(originLine); i++)
		{
			if (originLine[i-2] == '%')
			{
				sprintf(txtLine, originLine, va_arg(argument, ulong));
				i = strlen(originLine)+1;
			}
		}

		CreateNode(txtList, 0, txtLine);
	}

	RemoveList(originList);

	return txtList;
}

ubyte txtKeyExists(ulong textId, char *key)
{
	ubyte found = 0;
	struct Text *txt = GetNthNode(txtBase->tc_Texts, textId);

	if (txt && key)
	{
		txtPrepare(textId);

		// after txtPrepare txt_LastMark points to TXT_BUFFER_PREPARE in every case
		for (; txt->txt_LastMark < ((char *)TXT_BUFFER_WORK + txt->txt_Size); txt->txt_LastMark++)
		{
			if (*txt->txt_LastMark == TXT_CHAR_MARK)
			{
				char mark[TXT_KEY_LENGTH];

				strcpy(mark, txt->txt_LastMark+1);

				if (strcmp(key, mark) == 0)
				{
					found = 1;
					break;
				}
			}
		}
	}

	return found;
}

ulong txtCountKey(char *key)
{
	ulong i = strlen(key), j, k;
	for (j = 0, k = 0; j < i; j++)
	{
		if (key[j] == TXT_CHAR_KEY_SEPERATOR) k++;
	}
	return(k + 1);
}

// functions - STRING
char *txtGetNthString(ulong textId, char *key, ulong nth, char *dest)
{
	LIST *txtList = txtGoKey(textId, key);
	char *src;
	if (src = GetNthNode(txtList, nth)) strcpy(dest, NODE_NAME(src));
	else dest[0] = '\0';
	RemoveList(txtList);
	return(dest);
}

void txtPutCharacter(LIST *list, uword pos, ubyte c)
{
	NODE *node;
	for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
	{
		NODE_NAME(node)[pos] = c;
	}
}

// public functions - LANGUAGE
void txtSetLanguage(ubyte lang)
{
	if (txtBase)
	{
		if (lang > TXT_LANG_LAST) txtBase->tc_Language = TXT_LANG_GERMAN;
		else txtBase->tc_Language = lang;
	}
}

ubyte txtGetLanguage(void)
{
	if (txtBase) return(txtBase->tc_Language);
	return(TXT_LANG_GERMAN);
}

ubyte *txtGetTemporary(void)
{
	static ubyte i = 0;
	static ubyte s[16][TXT_KEY_LENGTH];
	ubyte *r;
	r = &s[i][0];
	i = (i+1) & 15;
	memset(r, 0, TXT_KEY_LENGTH);
	return(r);
}
