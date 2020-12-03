
#ifndef _CHAR_DB_H_
#define _CHAR_DB_H_

#include "mmo.h"

void char_db_init();
void char_db_term();
int char_db_newchar(const char*,const char*,int,int,int,int,int,int,int);
int char_db_isnameused(const char*);
unsigned int char_db_idfromname(const char *name);
int char_db_setpass(const char*,const char*,const char*);
int char_db_mapfifofromlogin(const char*,const char*,unsigned int*);
int mmo_char_fromdb(unsigned int,struct mmo_charstatus*,char*);
int mmo_char_todb(struct mmo_charstatus*);
void mmo_setonline(unsigned int,int);
void mmo_setallonline(int);
int logindata_add(unsigned int,int,char*);
int logindata_del(unsigned int);
#endif
