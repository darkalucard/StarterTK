
#ifndef _SAVE_DB_H_
#define _SAVE_DB_H_

#include "mmo.h"

void save_db_init();
void save_db_term();
int mmo_char_todb(struct mmo_charstatus*);
#endif
