#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "magic.h"
#include "map.h"
#include "db.h"
#include "db_mysql.h"
#include "malloc.h"
#include "core.h"
#include "script.h"

DBMap* magic_db;

int magicdb_searchname_sub(void *key,void *data,va_list ap) {
	struct magic_data *item=(struct magic_data *)data,**dst;
	char *str;
	str=va_arg(ap,char *);
	dst=va_arg(ap,struct magic_data**);
	if(strcmpi(item->yname,str)==0)
		*dst=item;
	return 0;
}

struct magic_data* magicdb_searchname(const char *str) {
	struct magic_data *item=NULL;
	magic_db->foreach(magic_db,magicdb_searchname_sub,str,&item);
	return item;
}

struct magic_data* magicdb_search(int id) {
	static struct magic_data *db=NULL;
	if (db && db->id == id)
		return db;

	db = uidb_get(magic_db, id);
	if (db)
		return db;
	
	CALLOC(db, struct magic_data, 1);
	uidb_put(magic_db, id, db);
	db->id = id;
	//db->type = ITM_ETC;
	//db->max_amount = 1;
	strcpy(db->name, "??");

	return db;
}

struct magic_data* magicdb_searchexist(int id) {
	struct magic_data *db=NULL;
	db = uidb_get(magic_db, id);
	return db;
}

int magicdb_id(char *str) {
	struct magic_data *db=NULL;
	db=magicdb_searchname(str);
	if (db)
		return db->id;

	if ((unsigned int)strtoul(str, NULL, 10)>0) {
		db=magicdb_searchexist((unsigned int)strtoul(str, NULL, 10));
		if (db)
			return db->id;
	}
	return 0;
}

char* magicdb_name(int id) {
	struct magic_data *db;
	db = magicdb_search(id);
	return db->name;
}

char* magicdb_yname(int id) {
	struct magic_data *db;
	db=magicdb_search(id);
	return db->yname;
}


char* magicdb_question(int id) {
	struct magic_data *db;
	db=magicdb_search(id);
	return db->question;
}

int magicdb_type(int id) {
	struct magic_data *db;
	db=magicdb_search(id);
	return db->type;
}

int magicdb_dispel(int id) {
	struct magic_data *db;
	db=magicdb_search(id);
	return db->dispell;
}

int magicdb_aether(int id) {
	struct magic_data *db;
	db=magicdb_search(id);
	return db->aether;
}

int magicdb_mute(int id) {
	struct magic_data *db;
	db=magicdb_search(id);
	return db->mute;
}

/*
char* magicdb_script(int id) {
	struct magic_data *db;
	db = magicdb_search(id);
	
	return db->script;
}

char* magicdb_script2(int id) {
	struct magic_data *db;
	db=magicdb_search(id);
	return db->script2;
}

char* magicdb_script3(int id) {
	struct magic_data *db;
	db=magicdb_search(id);
	return db->script3;
}

*/	
int magicdb_read() {
	struct magic_data *db = NULL;
	int i, mag, id;
	unsigned char type, dispell, aether, mute;
	char class;
	char name[32], yname[32], question[64];
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `SplId`, `SplDescription`, `SplIdentifier`, `SplType`, `SplQuestion`, `SplDispel`, `SplAether`, `SplMute`, `SplPthId` FROM `Spells` WHERE `SplActive` = '1'")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_STRING, &name, sizeof(name), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_STRING, &yname,sizeof(yname), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_UCHAR, &type,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_STRING, &question,sizeof(question),NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_UCHAR, &dispell,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_UCHAR, &aether,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 7, SQLDT_UCHAR, &mute,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 8, SQLDT_CHAR, &class,0,NULL,NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	mag = SqlStmt_NumRows(stmt);
	
	for (i = 0; i < mag && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++) {
		db = magicdb_search(id);
		strcpy(db->name, name);
		strcpy(db->yname, yname);
		db->type = type;
		strcpy(db->question, question);
		db->dispell = dispell;
		db->aether = aether;
		db->mute = mute;
		db->class = class;
	}		
		
	SqlStmt_Free(stmt);
	printf("Magic db read done. %d magic loaded!\n", mag);
	return 0;
}

static int magicdb_final(void *key,void *data,va_list ap) {
	struct magic_data *db;
	nullpo_ret(0, db=data);

	//FREE(db->script);
	FREE(db);

	return 0;
}

int magicdb_term() {
	if(magic_db){
		//numdb_final(magic_db,magicdb_final);
		db_destroy(magic_db);
	}
	return 0;
}

int magicdb_init() {
	magic_db = uidb_alloc(DB_OPT_BASE);
	magicdb_read();
	return 0;
}
