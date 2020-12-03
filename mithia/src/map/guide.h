#ifndef _GUIDE_H_
#define _GUIDE_H_

#define GUIDEDB_FILE "db/guide_db.txt"

struct guide_data {
	int id;
	char name[64];
	char yname[64];
};

extern struct DBMap *guide_db;

struct guide_data* guidedb_search(int);
struct guide_data* guidedb_searchexist(int);
struct guide_data* guidedb_searchname(const char*);

char* guidedb_yname(int);
int guidedb_id(char *str);
char* guidedb_name(int);
int guidedb_init(void);

#endif

