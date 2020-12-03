
#ifndef _LOGIF_H_
#define _LOGIF_H_
struct auth_node {
	unsigned int id;
	unsigned int ip;
	char name[32];
	int timer;
};

extern struct DBMap* auth_db;
int authdb_init();
int auth_check(char*,unsigned int);
int auth_delete(char*);
int auth_add(char*,unsigned int,unsigned int);
int check_connect_char(int,int);
int check_connect_save(int,int);
int intif_mmo_tosd(int, struct mmo_charstatus*);
int intif_parse(int);
int intif_quit(USER*);
int intif_load(int,int,char*);
int intif_save(USER*);
int intif_init();
int intif_timer(int,int);
int intif_wisp(char*,char*,unsigned char*,int);
int intif_savequit(USER*);
#endif
