/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "list\list.h"
#include "port\port.h"

void *CreateList(uword size)
{
	LIST *list = NULL;

	if (!size)
		size = sizeof(LIST);

	if (size >= sizeof(LIST))
	{
		if (list = (LIST *)MemAlloc(size))
		{
			LIST_HEAD(list) = (NODE *)&LIST_TAIL(list);
			LIST_TAIL(list) = NULL;
			LIST_TPRED(list)= (NODE *)&LIST_HEAD(list);
			LIST_SIZE(list) = size;
		}
	}

	return (void *)list;
}

void RemoveList(void *list)
{
	RemoveNode(list, NULL);
	FreeList(list);
}

void FreeList(void *list)
{
	MemFree(list, LIST_SIZE(list));
}

void *AddNode(void *list, void *node, void *predNode)
{
	if (!predNode)
		predNode = list;

	NODE_SUCC(node) = NODE_SUCC(predNode);
	NODE_PRED(node) =(NODE*) predNode;

	NODE_PRED(NODE_SUCC(predNode))=(NODE*) node;
	NODE_SUCC(predNode)           =(NODE*) node;

	return node;
}

void *AddTailNode(void *list, void *node)
{
	return AddNode(list, node, LIST_TPRED(list));
}

void *AddHeadNode(void *list, void *node)
{
	return AddNode(list, node, list);
}

void *RemNode(void *node)
{
	NODE_SUCC(NODE_PRED(node)) = NODE_SUCC(node);
	NODE_PRED(NODE_SUCC(node)) = NODE_PRED(node);

	// just to be certain
	NODE_SUCC(node) = NULL;
	NODE_PRED(node) = NULL;

	return node;
}

void *RemHeadNode(void *list)
{
	if (!LIST_EMPTY(list))
		return RemNode(LIST_HEAD(list));
	else
	    return NULL;
}

void *RemTailNode(void *list)
{
	if (!LIST_EMPTY(list))
		return RemNode(LIST_TPRED(list));
	else
	    return NULL;
}

void *CreateNode(void *list, uword size, char *name)
{
	NODE *node   = NULL;
	uword len    = 0;

	if (!size)
	{
		size = sizeof(NODE);
	}

	if (name)
	{
		len = strlen(name) + 1;
	}

	if (size >= sizeof(NODE))
	{
		if (node = (NODE *)MemAlloc(size + len))
		{
			NODE_SUCC(node) = NULL;
			NODE_PRED(node) = NULL;
			NODE_SIZE(node) = size;

			if (name)
			{
				NODE_NAME(node) = &((char *)node)[size];
				strcpy(NODE_NAME(node), name);
			}
			else
			{
				NODE_NAME(node) = NULL;
			}

			if (list)
			{
				AddTailNode(list, node);
			}
		}
	}
	return((void *)node);
}

void RemoveNode(void *list, char *name)
{
	NODE *node;

	if (name)
	{
		if (node = (NODE*)GetNode(list, name))
		{
			RemNode(node);
			FreeNode(node);
		}
	}
	else
	{
		if(!LIST_EMPTY(list))
		{
			while (node = (NODE*)RemTailNode(list))
				FreeNode(node);
		}
	}
}

void FreeNode(void *node)
{
	uword size;
	size = NODE_SIZE(node);

	if (NODE_NAME(node))
		size += strlen(NODE_NAME(node)) + 1;

	MemFree(node, size);
}

void *GetNode(void *list, char *name)
{
	NODE *node;

	for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
	{
		if (!strcmp(NODE_NAME(node), name))
		{
			return(node);
		}
	}
	return(NULL);
}

void *GetNthNode(void *list, ulong n)
{
	NODE *node;
	ulong i = 0;

	for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
	{
		if (i == n) return(node);
		i++;
	}
	return(NULL);
}

ulong GetNrOfNodes(void *list)
{
	NODE *node = NULL;
	ulong i = 0;

	for (i = 0, node = LIST_HEAD(list); NODE_SUCC(node); i++, node = NODE_SUCC(node));

	return i;
}

ulong GetNodeNrByAddr(void *list, void *node)
{
	NODE *s;
	ulong i;

	for (s = LIST_HEAD(list), i = 0; NODE_SUCC(s) && (s != node); s = NODE_SUCC(s), i++);

	return i;
}

ulong GetNodeNr(void *list, char *name)
{
	return GetNodeNrByAddr(list, GetNode(list, name));
}

void foreach(void *list, void (*processNode)(void *))
{
	NODE *node;

	for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
		processNode (node);
}

void Link(void *list, void *node, void *predNode)
{
	if (predNode)
		AddNode(list, node, predNode);
	else
	    AddTailNode(list, node);
}

void *UnLinkByAddr(void *list, void *node, void **predNode)
{
	if (predNode)
		*predNode = NODE_PRED(node);

	return RemNode(node);
}

void *UnLink(void *list, char *name, void **predNode)
{
	return UnLinkByAddr(list, GetNode(list, name), predNode);
}

void ReplaceNodeByAddr(void *list, void *node, void *newNode)
{
	void *predNode;

	if (node = UnLinkByAddr (list, node, &predNode))
	{
		Link(list, newNode, predNode);
		FreeNode (node);
	}
}

void ReplaceNode(void *list, char *name, void *newNode)
{
	ReplaceNodeByAddr(list, GetNode(list, name), newNode);
}

ulong ReadList(void *list, uword size, char *fileName, uword diskId)
{
	FILE *fh;
	ulong i = 0;
	char buffer[256];

	if (fh = dskOpen(fileName, "r", diskId))
	{
		while (dskGets(buffer, 255, fh))
		{
			if (buffer[0] == ';')
				continue;	// overread comments

			if (!buffer[0])
				continue;	// skip empty line

			if (!CreateNode(list, size, buffer))
			{
				RemoveNode(list, NULL);
				dskClose(fh);
				return 0L;
			}

			i++;
		}

		dskClose(fh);
	}

	return i;
}

void WriteList(void *list, char *fileName, uword diskId)
{
	FILE *fh = NULL;
	NODE *node  = NULL;

	if (fh = dskOpen(fileName, "w", diskId))
	{
		for (node = LIST_HEAD(list); NODE_SUCC(node); node = NODE_SUCC(node))
			fprintf(fh, "%s\n", NODE_NAME(node));

		fclose(fh);
	}
}
