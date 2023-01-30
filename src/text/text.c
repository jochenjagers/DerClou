/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/

// public header(s)
#include "text/text.h"

// private header(s)
#include "port/port.h"
#include "text/text_e.h"
#include "text/text_p.h"

// private globals declaration
char *txtLanguageMark[TXT_LANG_LAST + 1] = {"E", "D", "F", "S"};
struct TextControl *txtBase = NULL;
char keyBuffer[TXT_KEY_LENGTH];

// private functions
static char *txtGetLine(struct Text *txt, ubyte lineNr)
{
    ubyte i = 0;
    char *line = NULL;

    if (txt && txt->txt_LastMark && lineNr)
    {
        line = txt->txt_LastMark;
        i = 0;

        while (i < lineNr)
        {
            if (!line)
            {  // 2015-01-04 LucyG : just in case...
                break;
            }

            if (*line == TXT_CHAR_EOF)
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
                line++;  // overread second EOS
                i++;

                if (*line == TXT_CHAR_EOF)
                {
                    line = NULL;
                    break;
                }

                // overread comments
                while (*(line + 1) == TXT_CHAR_REMARK)
                {
                    while (*(++line) != TXT_CHAR_EOS)
                        ;
                    line++;  // overread second EOS;
                }
            }

            line++;
        }

        // 2015-01-04 LucyG : crashed on executing Chiswick House plan
        //                    because line was NULL
        if (!line)
        {
            Log("%s|%s: line %d = NULL", __FILE__, __func__, lineNr);
        }
        else if ((*line == TXT_CHAR_EOF) || (*line == TXT_CHAR_MARK))
        {
            line = NULL;
        }

        if (line >= ((char *)txt->txt_Handle + txt->txt_Size)) line = NULL;
    }

    return line;
}

//  public functions - TEXT
void txtInit(ubyte lang)
{
    char txtListPath[DSK_PATHNAMELENGTH];
    struct Text *txt = NULL;
    uword i = 0;

    if ((txtBase = (struct TextControl *)MemAlloc(sizeof(struct TextControl))))
    {
        txtBase->tc_Texts = (LIST *)CreateList(0);
        txtBase->tc_Language = lang;

        dskBuildPathName(TEXT_DIRECTORY, TXT_LIST, txtListPath);

        if (ReadList(txtBase->tc_Texts, sizeof(struct Text), txtListPath, 0))
        {
            for (i = 0; i < GetNrOfNodes(txtBase->tc_Texts); i++)
            {
                txt = (struct Text *)GetNthNode(txtBase->tc_Texts, i);

                txt->txt_Handle = NULL;
                txt->txt_LastMark = NULL;
                txt->txt_Size = 0;

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
        uword i = 0;

        for (i = 0; i < GetNrOfNodes(txtBase->tc_Texts); i++) txtUnLoad(i);

        RemoveList(txtBase->tc_Texts);

        MemFree(txtBase, sizeof(struct TextControl));
    }
}

void txtLoad(uint32_t textId)
{
    struct Text *txt = (struct Text *)GetNthNode(txtBase->tc_Texts, textId);

    if (txt)
    {
        if (!txt->txt_Handle)
        {
            char txtPath[DSK_PATHNAMELENGTH];
            ubyte *mem = NULL;

            dskBuildPathName(TEXT_DIRECTORY, NODE_NAME(txt), txtPath);
            strcat(txtPath, txtLanguageMark[txtBase->tc_Language]);
            strcat(txtPath, TXT_SUFFIX);

            txt->txt_Size = dskFileLength(txtPath);

            if ((txt->txt_Handle = MemAlloc(txt->txt_Size)))
            {
                dskLoad(txtPath, txt->txt_Handle);  // loading text into buffer

                // correcting text
                for (mem = (ubyte *)txt->txt_Handle; mem < ((ubyte *)txt->txt_Handle + txt->txt_Size); mem++)
                {
                    *mem ^= TXT_XOR_VALUE;
                    if (*mem == 10 || *mem == 13) *mem = TXT_CHAR_EOS;
                }
            }
            else
            {
                NewErrorMsg(No_Mem, __FILE__, __func__, ERR_TXT_NO_XMS);
            }
        }
    }
}

void txtUnLoad(uint32_t textId)
{
    struct Text *txt = (struct Text *)GetNthNode(txtBase->tc_Texts, textId);

    if (txt)
    {
        if (txt->txt_Handle)
        {
            MemFree(txt->txt_Handle, txt->txt_Size);

            txt->txt_Handle = NULL;
            txt->txt_LastMark = NULL;
            txt->txt_Size = 0;
        }
    }
}

void txtPrepare(uint32_t textId)
{
    struct Text *txt = (struct Text *)GetNthNode(txtBase->tc_Texts, textId);

    if (txt)
    {
        txt->txt_LastMark = (char *)txt->txt_Handle;
    }
}

void txtUnPrepare(uint32_t textId)
{
    struct Text *txt = (struct Text *)GetNthNode(txtBase->tc_Texts, textId);

    if (txt) txt->txt_LastMark = NULL;
}

void txtReset(uint32_t textId)
{
    struct Text *txt = (struct Text *)GetNthNode(txtBase->tc_Texts, textId);

    if (txt) txt->txt_LastMark = (char *)txt->txt_Handle;
}

// public functions - KEY
char *txtGetKey(uword keyNr, char *key)
{
    uword i = 0;

    if (key)
    {
        for (i = 1; i < keyNr; i++)
        {
            while (*key && (*key != TXT_CHAR_KEY_SEPERATOR)) key++;

            if (!*key)
                break;
            else
                key++;
        }

        if (*key)
        {
            while (isspace(*key)) key++;

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

uint32_t txtGetKeyAsULONG(uword keyNr, char *key)
{
    char *t = txtGetKey(keyNr, key);
    if (!t) return (0);
    return ((uint32_t)atol(t));
}

/* 2015-01-10 LucyG: the Czech localization crashed when asking Dan Stanford about */
/* Marc Smith and selecting the second option (... k?). There is a problem with */
/* "Dan Stanford_kÜiv]mi obchody" getting truncated to "Dan Stanford_k". */

/* This is really just a workaround at the rear end of the problem. If something */
/* like that should happen again with other strings, we should trace to the source */
/* of the problem (data file, some "strange" char converted to 0, whatever). */

/* Same as txtGoKey, but uses strncmp instead of strcmp. */
static LIST *txtGoKeyN(uint32_t textId, char *key)
{
    LIST *txtList = NULL;
    char *LastMark = NULL;
    struct Text *txt = NULL;
    ubyte found = 0;
    ubyte i = 0;
    char mark[TXT_KEY_LENGTH];
    char *line = NULL;

    txt = (struct Text *)GetNthNode(txtBase->tc_Texts, textId);
    if (txt)
    {
        if (!key && txt->txt_LastMark)
        {
            LastMark = txt->txt_LastMark;
        }
        txtPrepare(textId);
        if (!key && LastMark)
        {
            txt->txt_LastMark = LastMark + 1;
        }

        for (; txt->txt_LastMark < ((char *)txt->txt_Handle + txt->txt_Size); txt->txt_LastMark++)
        {
            if (*txt->txt_LastMark == TXT_CHAR_MARK)
            {
                found = 1;
                if (key)
                {
                    strcpy(mark, txt->txt_LastMark + 1);
                    if (strncmp(key, mark, strlen(key)) != 0)
                    {
                        found = 0;
                    }
                    else
                    {
                        /* Got it! This should be the truncated string */
                        Log("%s|%s: \"%s\" == \"%s\"", __FILE__, __func__, key, mark);
                    }
                }
                if (found)
                {
                    i = 1;
                    txtList = (LIST *)CreateList(0);
                    while ((line = txtGetLine(txt, i++)))
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
        Log("%s|%s: Cannot find text (%d, \"%s\"). Giving up :(", __FILE__, __func__, textId, key ? key : "NULL");
        NewErrorMsg(Disk_Defect, __FILE__, __func__, ERR_TXT_NO_KEY);
    }

    return txtList;
}

LIST *txtGoKey(uint32_t textId, char *key)
{
    LIST *txtList = NULL;
    char *LastMark = NULL;
    struct Text *txt = NULL;
    ubyte found = 0;
    ubyte i = 0;
    char mark[TXT_KEY_LENGTH];
    char *line = NULL;

    txt = (struct Text *)GetNthNode(txtBase->tc_Texts, textId);
    if (txt)
    {
        // MOD: 08-04-94 hg
        // wenn kein key angegeben wurde, soll der nächste genommen werden
        // -> in LastMark wird die letzte Position zwischengespeichert, da
        // txt->LastMark in txtPrepare neu gesetzt wird!

        // Vorsicht auf Spezialfall: kein Key, Text noch nie benutzt !!

        if (!key && txt->txt_LastMark)
        {
            LastMark = txt->txt_LastMark;
        }

        txtPrepare(textId);

        // Erklärung zu +1: LastMark zeigt auf den letzten Schlüssel
        // -> ohne "+1" würde wieder der selbe Schlüssel zurückgegeben werden
        if (!key && LastMark)
        {
            txt->txt_LastMark = LastMark + 1;
        }

        for (; txt->txt_LastMark < ((char *)txt->txt_Handle + txt->txt_Size); txt->txt_LastMark++)
        {
            if (*txt->txt_LastMark == TXT_CHAR_MARK)
            {
                found = 1;

                if (key)
                {
                    strcpy(mark, txt->txt_LastMark + 1);

                    if (strcmp(key, mark) != 0)
                    {
                        found = 0;
                    }
                }

                if (found)
                {
                    i = 1;

                    txtList = (LIST *)CreateList(0);

                    while ((line = txtGetLine(txt, i++)))
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
        Log("%s|%s: Cannot find text (%d, \"%s\"). Using alternative search.", __FILE__, __func__, textId,
            key ? key : "NULL");
        return (txtGoKeyN(textId, key));
    }

    return txtList;
}

LIST *txtGoKeyAndInsert(uint32_t textId, char *key, ...)
{
    va_list argument;
    LIST *txtList = (LIST *)CreateList(0L);
    LIST *originList = NULL;
    NODE *node = NULL;

    va_start(argument, key);

    originList = txtGoKey(textId, key);

    for (node = LIST_HEAD(originList); NODE_SUCC(node); node = NODE_SUCC(node))
    {
        ubyte i = 0;
        char originLine[256];
        char txtLine[256];

        strcpy(originLine, NODE_NAME(node));
        strcpy(txtLine, NODE_NAME(node));

        for (i = 2; i < strlen(originLine); i++)
        {
            if (originLine[i - 2] == '%')
            {
                sprintf(txtLine, originLine, va_arg(argument, uint32_t));
                i = strlen(originLine) + 1;
            }
        }

        CreateNode(txtList, 0, txtLine);
    }

    RemoveList(originList);

    return txtList;
}

ubyte txtKeyExists(uint32_t textId, char *key)
{
    ubyte found = 0;
    struct Text *txt = (struct Text *)GetNthNode(txtBase->tc_Texts, textId);
    char mark[TXT_KEY_LENGTH];

    if (txt && key)
    {
        txtPrepare(textId);

        // after txtPrepare txt_LastMark points to TXT_BUFFER_PREPARE in every case
        for (; txt->txt_LastMark < ((char *)txt->txt_Handle + txt->txt_Size); txt->txt_LastMark++)
        {
            if (*txt->txt_LastMark == TXT_CHAR_MARK)
            {
                strcpy(mark, txt->txt_LastMark + 1);

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

uint32_t txtCountKey(char *key)
{
    uint32_t i = strlen(key);
    uint32_t j = 0;
    uint32_t k = 0;
    for (j = 0, k = 0; j < i; j++)
    {
        if (key[j] == TXT_CHAR_KEY_SEPERATOR) k++;
    }
    return k + 1;
}

// functions - STRING
char *txtGetNthString(uint32_t textId, char *key, uint32_t nth, char *dest)
{
    LIST *txtList = txtGoKey(textId, key);
    char *src = NULL;
    if ((src = (char *)GetNthNode(txtList, nth)))
        strcpy(dest, NODE_NAME(src));
    else
        dest[0] = '\0';
    RemoveList(txtList);
    return dest;
}

void txtPutCharacter(LIST *list, uword pos, ubyte c)
{
    NODE *node = NULL;
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
        if (lang > TXT_LANG_LAST)
            txtBase->tc_Language = TXT_LANG_GERMAN;
        else
            txtBase->tc_Language = lang;
    }
}

ubyte txtGetLanguage(void)
{
    if (txtBase) return (txtBase->tc_Language);
    return TXT_LANG_GERMAN;
}

char *txtGetTemporary(void)
{
    static ubyte i = 0;
    static char s[16][TXT_KEY_LENGTH];
    char *r = NULL;
    r = &s[i][0];
    i = (i + 1) & 15;
    memset(r, 0, TXT_KEY_LENGTH);
    return r;
}
