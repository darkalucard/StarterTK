
#ifndef _CHAR_H_
#define _CHAR_H_

#define USE_DB_MYSQL

#include "mmo.h"

struct char_login_data {
	char name[16];
	int map_server;
};

struct map_fifo_data {
	int fd;
	unsigned int ip, port;
	unsigned short map[MAX_MAP_PER_SERVER];
	unsigned int map_n;
};

extern struct mmo_charstatus *char_dat;
extern struct map_fifo_data map_fifo[];
extern int map_fifo_max;
extern int map_fifo_n;

extern int start_money;
extern struct point start_pos;

extern int login_fd;
extern char login_id[];
extern char login_pw[];
extern int save_fd;
extern char save_id[];
extern char save_pw[];
extern int char_fd;
extern char char_id[];
extern char char_pw[];

extern char sql_id[];
extern char sql_pw[];
extern char sql_ip[];
extern char sql_db[];
extern int sql_port;

extern struct Sql* sql_handle;
struct char_login_data* logindata_search(unsigned int);

//int logindata_del(unsigned int);
int logindata_change(unsigned int,int);
int mapfifo_from_mapid(int);

int config_read(const char*);

void do_term(void);

#endif
