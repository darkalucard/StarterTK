
#ifndef _SAVE_H_
#define _SAVE_H_

#define USE_DB_MYSQL

#include "mmo.h"

extern int char_fd;

extern char save_id[];
extern char save_pw[];

extern char sql_id[];
extern char sql_pw[];
extern char sql_ip[];
extern char sql_db[];
extern int sql_port;

extern struct Sql* sql_handle;

int config_read(const char*);

void do_term(void);

#endif
