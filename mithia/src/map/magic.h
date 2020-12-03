
#ifndef _MAGIC_H_
#define _MAGIC_H_

//#define MAGICDB_FILE "db/magic_db.txt"

struct magic_data {
	int id, type;
	char name[32], yname[32], question[64];
	char script[64];
	char script2[64];
	char script3[64];
	unsigned char dispell, aether, mute;
	char class;
};

extern struct DBMap* magic_db;

struct magic_data* magicdb_search(int);
struct magic_data* magicdb_searchexist(int);
struct magic_data* magicdb_searchname(const char*);

char* magicdb_yname(int);
int magicdb_id(char *str);
char* magicdb_name(int);
char* magicdb_script(int);
char* magicdb_script2(int);
char* magicdb_script3(int);
char* magicdb_question(int);
int magicdb_dispel(int);
int magicdb_aether(int);
int magicdb_mute(int);
int magicdb_type(int);

int magicdb_read();
int magicdb_term();
int magicdb_init();

#endif
