/*  _________  _______
   / ___/ __ \/ __/ _ \      Der Clou!
  / /__/ /_/ /\ \/ ___/ Open Source Project
  \___/\____/___/_/ http://cosp.sourceforge.net
   Based on the original by neo Software GmbH
*/
#ifndef MODULE_RELATION
#define MODULE_RELATION

#ifndef __STDIO_H
#include <stdio.h>
#endif

#ifndef __STRING_H
#include <string.h>
#endif

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

#ifndef __LIMITS_H
#include <limits.h>
#endif

#include "theclou.h"

#ifndef MODULE_ERROR
#include "error\error.h"
#endif

#ifndef MODULE_MEMORY
#include "memory\memory.h"
#endif

#ifndef MODULE_DISK
#include "disk\disk.h"
#endif

#define RELATION           unsigned long
#define KEY                void *
#define PARAMETER          unsigned long
#define COMPARSION         unsigned char

#define CMP_NO             ((COMPARSION)0)
#define CMP_EQUAL          ((COMPARSION)(1 << 0))
#define CMP_NOT_EQUAL      ((COMPARSION)(1 << 1))
#define CMP_HIGHER         ((COMPARSION)(1 << 2))
#define CMP_LOWER          ((COMPARSION)(1 << 3))

#define REL_FILE_MARK      "RELF"
#define REL_TABLE_MARK     "RTAB"

#define NO_PARAMETER       (0xffffffff)


#define Set(leftKey,id,rightKey)  SetP (leftKey, id, rightKey, NO_PARAMETER)
#define Ask(leftKey,id,rightKey)  AskP (leftKey, id, rightKey, NO_PARAMETER, CMP_NO)


struct relation
{
   struct relation *r_next;

   KEY       r_leftKey;
   KEY       r_rightKey;
   PARAMETER r_parameter;
};

struct relationDef
{
   struct relationDef *rd_next;

   RELATION            rd_id;
   struct relation    *rd_relationsTable;
};


extern int   (*CompareKey)(KEY, KEY);
extern KEY   (*EncodeKey) (char *);
extern char *(*DecodeKey) (KEY);


RELATION  AddRelation  (RELATION);
RELATION  CloneRelation(RELATION, RELATION);
RELATION  RemRelation  (RELATION);

RELATION  SetP    (KEY, RELATION, KEY, PARAMETER);
RELATION  UnSet   (KEY, RELATION, KEY);
PARAMETER GetP    (KEY, RELATION, KEY);
RELATION  AskP    (KEY, RELATION, KEY, PARAMETER, COMPARSION);
void      AskAll  (KEY, RELATION, void (*)(void *));
void      UnSetAll(KEY, void (*)(KEY));

int  SaveRelations(char *, unsigned long, unsigned long, uword disk_id);
int  LoadRelations(char *, uword disk_id);
void RemRelations (unsigned long, unsigned long);

#endif
