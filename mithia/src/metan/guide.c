#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "guide.h"
#include "db.h"
#include "malloc.h"
#include "core.h"
#ifdef OMGNEWBZ
struct dbt *guide_db;

int guidedb_searchname_sub(void *key,void *data,va_list ap) {
	struct guide_data *item=(struct guide_data *)data,**dst;
	char *str;
	str=va_arg(ap,char *);
	dst=va_arg(ap,struct guide_data**);
	if(strcmpi(item->yname,str)==0)
		*dst=item;
	return 0;
}

struct guide_data* guidedb_searchname(const char *str) {
	struct guide_data *item=NULL;
	numdb_foreach(guide_db,guidedb_searchname_sub,str,&item);
	return item;
}

struct guide_data* guidedb_search(int id) {
	static struct guide_data *db=NULL;
	if (db && db->id == id)
		return db;

	db = numdb_search(guide_db, id);
	if (db)
		return db;
	
	CALLOC(db, struct guide_data, 1);
	numdb_insert(guide_db, id, db);
	db->id = id;
	//db->type = ITM_ETC;
	//db->max_amount = 1;
	strcpy(db->name, "??");
	strcpy(db->yname,"??");
	return db;
}

struct guide_data* guidedb_searchexist(int id) {
	struct guide_data *db=NULL;
	db = numdb_search(guide_db, id);
	return db;
}

int guidedb_id(char *str) {
	struct guide_data *db=NULL;
	db=guidedb_searchname(str);
	if (db)
		return db->id;

	if ((unsigned int)strtoul(str, NULL, 10)>0) {
		db=guidedb_searchexist((unsigned int)strtoul(str, NULL, 10));
		if (db)
			return db->id;
	}
	return 0;
}

char* guidedb_name(int id) {
	struct guide_data *db;
	db = guidedb_search(id);
	return db->name;
}

char* guidedb_yname(int id) {
	struct guide_data *db;
	db=guidedb_search(id);
	return db->yname;
}
int guidedb_read(const char* file) {
	FILE *fp;
	char line[1024];
	int line_count=0;
	char name[64];
	char yname[64];
	int id;
	struct guide_data* db;
	int thing;
	fp=fopen(file,"r");
	if(fp==NULL) {
		printf("Can't load Guide\n");
		return 0;
	}
	
	while (fgets(line,sizeof(line),fp)) {
		if(line[0]=='/' && line[1]=='/')
			continue;
		
		thing=sscanf(line,"%d,%[^,],%[^\n]",&id,name,yname);
		
		
		
		if(thing==3) {
			db=guidedb_search(id);
			strcpy(db->name,name);
			strcpy(db->yname,yname);
			line_count++;
			
		}
	}
	fclose(fp);
	printf("%d Guides Loaded\n",line_count);
	return line_count;
}
static int guidedb_final(void *key,void *data,va_list ap) {
	struct guide_data *db;
	nullpo_ret(0, db=data);

	//FREE(db->script);
	FREE(db);

	return 0;
}

int guidedb_term() {
	if(guide_db){
		numdb_final(guide_db,guidedb_final);
		guide_db=NULL;
	}
	return 0;
}

int guidedb_init() {
	guide_db = numdb_init();
	
	return guidedb_read(GUIDEDB_FILE);
}
#endif
