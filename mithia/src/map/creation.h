#ifndef _CREATION_H
#define _CREATION_H

#define CREATEDB_FILE "db/createdb.txt"

struct creation_data {
	int id;
	int count;
	int amount;
	int item[10];
	int item_count[10];
	int item_created;
};

//int createdb_start(USER*);

extern struct DBMap *create_db;

#endif
