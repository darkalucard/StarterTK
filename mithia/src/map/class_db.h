
#ifndef _CLASSDB_H_
#define _CLASSDB_H_

#define CLASSDB_FILE "db/class_db.txt"
#define LEVELDB_FILE "db/level_db.txt"


struct class_data {
	char rank0[32];
	char rank1[32];
	char rank2[32];
	char rank3[32];
	char rank4[32];
	char rank5[32];
	char rank6[32];
	char rank7[32];
	char rank8[32];
	char rank9[32];
	char rank10[32];
	char rank11[32];
	char rank12[32];
	char rank13[32];
	char rank14[32];
	char rank15[32];
	unsigned short id;
	unsigned short path;
	unsigned int level[50];
	int chat;
	int icon;
};

struct class_data *cdata[20];
 
extern struct DBMap *class_db;

struct class_data* classdb_search(int);
struct class_data* classdb_searchexist(int);
unsigned int classdb_level(int,int);


char* classdb_name(int,int);
int classdb_path(int);
int classdb_chat(int);
int classdb_icon(int);

int classdb_read();
int classdb_term();
int classdb_init();

#endif
