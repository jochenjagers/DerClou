/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#include "data\relation.h"
#include "port\port.h"

static RELATION            nrOfRelations    = 1;
static struct relationDef *relationsDefBase = NULL;

int   (*CompareKey)(KEY, KEY) = NULL;
KEY   (*EncodeKey) (char *)   = NULL;
char *(*DecodeKey) (KEY)      = NULL;


struct relationDef *FindRelation (RELATION id)
{
	struct relationDef *rd = relationsDefBase;

	while (rd)
	{
		if (rd->rd_id == id)
			return rd;

		rd = rd->rd_next;
	}

	return NULL;
}

RELATION AddRelation(RELATION id)
{
	struct relationDef *rd;

	if (!FindRelation (id))
	{
		if (rd = (struct relationDef *)MemAlloc(sizeof(struct relationDef)))
		{
			rd->rd_next           = relationsDefBase;
			relationsDefBase      = rd;

			rd->rd_id             = id;
			nrOfRelations++;

			rd->rd_relationsTable = NULL;

			return rd->rd_id;
		}
	}

	return 0;
}

RELATION CloneRelation(RELATION id, RELATION cloneId)
{
	struct relationDef *rd;

	if (rd = FindRelation (id))
	{
		if (AddRelation(cloneId) || FindRelation(cloneId))
		{
			struct relation *r = rd->rd_relationsTable;

			while (r)
			{
				SetP(r->r_leftKey, cloneId, r->r_rightKey, r->r_parameter);
				r = r->r_next;
			}

			return cloneId;
		}
	}

	return 0;
}

RELATION RemRelation(RELATION id)
{
	struct relationDef *rd = relationsDefBase,
	*h  = NULL;

	while (rd)
	{
		if (rd->rd_id == id)
		{
			if (h)
				h->rd_next = rd->rd_next;
			else
			    relationsDefBase = rd->rd_next;

			while (rd->rd_relationsTable)
			{
				struct relation *h = rd->rd_relationsTable->r_next;

				MemFree(rd->rd_relationsTable, sizeof(struct relation));
				rd->rd_relationsTable = h;
			}

			MemFree(rd, sizeof(struct relationDef));

			nrOfRelations--;

			return id;
		}

		h  = rd;
		rd = rd->rd_next;
	}

	return 0;
}

RELATION SetP (KEY leftKey, RELATION id, KEY rightKey, PARAMETER parameter)
{
	struct relationDef *rd = FindRelation (id);

	if (rd && CompareKey)
	{
		struct relation *r = rd->rd_relationsTable;

		while (r)
		{
			if (CompareKey (r->r_leftKey, leftKey) && CompareKey (r->r_rightKey, rightKey))
			{
				if (parameter != NO_PARAMETER)
					r->r_parameter = parameter;
				return id;
			}

			r = r->r_next;
		}

		if (r = (struct relation *)MemAlloc(sizeof(struct relation)))
		{
			r->r_next = rd->rd_relationsTable;
			rd->rd_relationsTable = r;

			r->r_leftKey   = leftKey;
			r->r_rightKey  = rightKey;
			r->r_parameter = parameter;

			return id;
		}
	}

	return 0;
}

RELATION UnSet (KEY leftKey, RELATION id, KEY rightKey)
{
	struct relationDef *rd = FindRelation (id);

	if (rd && CompareKey)
	{
		struct relation *r = rd->rd_relationsTable;
		struct relation *h = NULL;

		while (r)
		{
			if (CompareKey (r->r_leftKey, leftKey) && CompareKey (r->r_rightKey, rightKey))
			{
				if (h)
					h->r_next = r->r_next;
				else
				    rd->rd_relationsTable = r->r_next;

				MemFree(r, sizeof(struct relation));

				return id;
			}

			h = r;
			r = r->r_next;
		}
	}

	return 0;
}

PARAMETER GetP (KEY leftKey, RELATION id, KEY rightKey)
{
	struct relationDef *rd = FindRelation (id);

	if (rd && CompareKey)
	{
		struct relation *r = rd->rd_relationsTable;

		while (r)
		{
			if (CompareKey (r->r_leftKey, leftKey) && CompareKey (r->r_rightKey, rightKey))
				return r->r_parameter;

			r = r->r_next;
		}
	}

	return NO_PARAMETER;
}

RELATION AskP (KEY leftKey, RELATION id, KEY rightKey, PARAMETER  parameter, COMPARSION comparsion)
{
	struct relationDef *rd = FindRelation (id);

	if (rd && CompareKey)
	{
		struct relation *r = rd->rd_relationsTable;

		while (r)
		{
			if (CompareKey (r->r_leftKey, leftKey) && CompareKey (r->r_rightKey, rightKey))
			{
				if (comparsion && (parameter != NO_PARAMETER))
				{
					if (comparsion & CMP_EQUAL)
					{
						if (comparsion & CMP_HIGHER)
							return (r->r_parameter >= parameter ? id : 0);
						else if (comparsion & CMP_LOWER)
							return (r->r_parameter <= parameter ? id : 0);

						return (r->r_parameter == parameter ? id : 0);
					}
					else if (comparsion & CMP_HIGHER)
						return (r->r_parameter > parameter ? id : 0);
					else if (comparsion & CMP_LOWER)
						return (r->r_parameter < parameter ? id : 0);
					else if (comparsion & CMP_NOT_EQUAL)
						return (r->r_parameter != parameter ? id : 0);

					return 0;
				}

				return id;
			}

			r = r->r_next;
		}
	}

	return 0;
}

void AskAll (KEY leftKey, RELATION id, void (*UseKey)(void *))
{
	struct relationDef *rd = FindRelation (id);

	if (rd && CompareKey)
	{
		struct relation *r = rd->rd_relationsTable;

		while (r)
		{
			if (CompareKey (r->r_leftKey, leftKey))
			{
				if (UseKey)
					UseKey (r->r_rightKey);
			}

			r = r->r_next;
		}
	}
}

void UnSetAll (KEY key, void (*UseKey)(KEY))
{
	struct relationDef *rd = relationsDefBase;

	while (rd)
	{
		struct relation *r = rd->rd_relationsTable;
		struct relation *h = NULL;

		while (r)
		{
			if (CompareKey (r->r_leftKey, key) || CompareKey (r->r_rightKey, key))
			{
				if (UseKey)
					UseKey (key);

				if (h)
					h->r_next = r->r_next;
				else
				    rd->rd_relationsTable = r->r_next;

				MemFree(r, sizeof(struct relation));
			}

			h = r;
			r = r->r_next;
		}

		rd = rd->rd_next;
	}
}

int SaveRelations (char *file, unsigned long offset, unsigned long size, uword disk_id)
{
	struct relationDef *rd = relationsDefBase;
	struct relation    *r;
	FILE *fh;
	char left   [256];
	char right  [256];

	if (rd && DecodeKey)
	{
		if (fh = dskOpen (file, "w", disk_id))
		{
			fprintf (fh, "%s\n", REL_FILE_MARK);

			while (rd)
			{
				if (rd->rd_id > offset)
				{
					if (size &&  (rd->rd_id > offset+size))
					{
						rd = rd->rd_next;
						continue;
					}

					fprintf (fh, "%s\n", REL_TABLE_MARK);

					fprintf (fh, "%ld\n", rd->rd_id);

					if (r = rd->rd_relationsTable)
					{
						while (r)
						{
							strcpy (left,  DecodeKey (r->r_leftKey));
							strcpy (right, DecodeKey (r->r_rightKey));
							fprintf (fh, "%s\n%s\n%ld\n", left, right, r->r_parameter);
							r = r->r_next;
						}
					}
				}

				rd = rd->rd_next;
			}

			dskClose (fh);

			return 1;
		}
	}

	return 0;
}

int LoadRelations(char *file, uword disk_id)
{
	RELATION  rd;
	PARAMETER parameter;
	char buffer [256];
	char left   [256];
	char right  [256];
	ubyte goOn;
	int   size = 255;
	FILE *fh = NULL;

	strcpy (buffer,"");
	strcpy (left,"");
	strcpy (right,"");

	if (EncodeKey)
	{
		if (fh = dskOpen (file, "r", disk_id))
		{
			dskGets(buffer, size, fh);

			buffer [strlen(REL_FILE_MARK)] = '\0';

			if (!strcmp (buffer, REL_FILE_MARK))
			{
				dskGets(buffer, size, fh);

				buffer [strlen (REL_TABLE_MARK)] = '\0';

				while (!dskIsEOF(fh))
				{
					while (strcmp (buffer, REL_TABLE_MARK) == 0)
					{
						dskGets (buffer, size, fh);

						rd = atol(buffer);

						goOn = 0;
						if (FindRelation (rd))
							goOn = 1;
						else
						{
							if (AddRelation (rd))
								goOn = 1;
						}

						if (goOn)
						{
							while (!dskIsEOF(fh))
							{
								if (dskGets (left, size, fh))
								{
									if (!strcmp (left, REL_TABLE_MARK))
									{
										strcpy (buffer, left);
										break;
									}

									dskGets (right,size, fh);

									dskGets (buffer,size, fh);

									parameter = atol(buffer);

									if (!SetP (EncodeKey (left), rd, EncodeKey (right), parameter))
									{
										dskClose (fh);
										return 0;
									}
								}
							}
						}
						else
						    {
							dskClose (fh);
							return 0;
						}
					}
				}

				dskClose (fh);
				return 1;
			}

			dskClose (fh);
		}
	}

	return 0;
}

void RemRelations (unsigned long offset, unsigned long size)
{
	struct relationDef *rd = relationsDefBase, *help;

	while (rd)
	{
		if (rd->rd_id > offset)
		{
			if (size &&  (rd->rd_id > offset+size))
			{
				rd = rd->rd_next;
				continue;
			}

			help = rd->rd_next;
			RemRelation (rd->rd_id);
			rd = help;
		}
		else
			rd = rd->rd_next;
	}
}
