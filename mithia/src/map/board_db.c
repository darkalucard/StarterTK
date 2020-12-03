
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board_db.h"
#include "map.h"
#include "db.h"
#include "malloc.h"
#include "core.h"
#include "db_mysql.h"
#include "strlib.h"

DBMap *board_db;
DBMap *bn_db;




struct board_data* boarddb_search(int id) {
	static struct board_data *db=NULL;
	if (db && db->id == id)
		return db;

	db = uidb_get(board_db, id);
	if (db)
		return db;
	
	CALLOC(db, struct board_data, 1);
	uidb_put(board_db, id, db);
	db->id = id;
	strcpy(db->name, "??");
	db->level=100;
	db->gmlevel=100;
	db->path=0;
	db->clan=0;

	return db;
}
struct bn_data* bn_search(int id) {
	static struct bn_data *db=NULL;
	if (db && db->id == id)
		return db;

	db = uidb_get(bn_db, id);
	if (db)
		return db;
	
	CALLOC(db, struct bn_data, 1);
	uidb_put(bn_db, id, db);
	db->id = id;
	strcpy(db->name, "??");
	return db;
}
struct bn_data* bn_searchexist(int id) {
	struct bn_data *db=NULL;
	db=uidb_get(bn_db,id);
	return db;
}

struct board_data* boarddb_searchexist(int id) {
	struct board_data *db=NULL;
	db = uidb_get(board_db, id);
	return db;
}


char *boarddb_name(int id) {
	struct board_data *db=NULL;
	db = boarddb_search(id);
	return db->name;
}

int boarddb_path(int id) {
	struct board_data *db=NULL;
	db = boarddb_search(id);
	
	return db->path;
}

int boarddb_level(int id) {
	struct board_data *db=NULL;
	db=boarddb_search(id);
	
	return db->level;
}

int boarddb_special(int id) {
	struct board_data *db=NULL;
	db=boarddb_search(id);
	return db->special;
}

int boarddb_gmlevel(int id) {
	struct board_data *db=NULL;
	db=boarddb_search(id);
	return db->gmlevel;
}

int boarddb_clan(int id) {
	struct board_data *db=NULL;
	db=boarddb_search(id);
	return db->clan;
}
char* bn_name(int id) {
	struct bn_data *db=NULL;
	db=bn_search(id);
	return db->name;
}

char boarddb_script(int id) {
	struct board_data *db = NULL;
	db = boarddb_search(id);
	return db->script;
}

char* boarddb_yname(int id) {
	struct board_data *db = NULL;
	db = boarddb_search(id);
	return db->yname;
}

int boarddb_sort(int id) {
	struct board_data *db = NULL;
	db = boarddb_search(id);
	return db->sort;
}

int boarddb_read() {
	
	struct board_data *db;
	int i, cls=0;
	int x;
	struct board_data b;
	SqlStmt* stmt;
	
	memset(&b,0,sizeof(b));
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `BnmId`, `BnmDescription`, `BnmLevel`, `BnmGMLevel`, `BnmPthId`, `BnmClnId`, `BnmScripted`, `BnmIdentifier`, `BnmSortOrder` FROM `BoardNames`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &b.id,0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_STRING, &b.name,sizeof(b.name),NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_INT, &b.level, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_INT, &b.gmlevel, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_INT, &b.path, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_INT, &b.clan, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_UCHAR, &b.script, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 7, SQLDT_STRING, &b.yname, sizeof(b.yname), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 8, SQLDT_INT, &b.sort, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	i=SqlStmt_NumRows(stmt);
	for(x=0;x<i && SQL_SUCCESS == SqlStmt_NextRow(stmt);x++) {
		
		db = boarddb_search(b.id);
		memcpy(db,&b,sizeof(b));
		
	}
	//sql_free_row();
	SqlStmt_Free(stmt);
	
	printf("Board db read done. %d boards loaded!\n", i);
	return 0;
}
int bn_read() {
	struct bn_data* db;
	int i;
	int x;
	
	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT BtlId, BtlDescription FROM BoardTitles"))
		{
			Sql_ShowDebug(sql_handle);
			return 0;
		}
		
	i=Sql_NumRows(sql_handle);
	for(x=0;x<i && SQL_SUCCESS == Sql_NextRow(sql_handle);x++) {
		char* data;
		size_t len;
		Sql_GetData(sql_handle, 0, &data,0); db=bn_search((unsigned int)strtoul(data, NULL, 10));
		Sql_GetData(sql_handle, 1, &data,&len); memcpy(db->name,data,len);
		//strcpy(db->name,sql_get_str(1));
		printf(" Board ID: %d - %s\n",db->id,db->name);
	}
	Sql_FreeResult(sql_handle);
	//sql_free_row();
	return 0;
}
static int boarddb_final(void *key,void *data,va_list ap) {
	struct board_data *db;
	nullpo_ret(0, db=data);

	FREE(db);

	return 0;
}

int boarddb_term() {
	if (board_db){
		//numdb_final(board_db,boarddb_final);
		db_destroy(board_db);
	}
	
	if (bn_db) {
		db_destroy(bn_db);
	}
	
	return 0;
}

int boarddb_init() {
	board_db = uidb_alloc(DB_OPT_BASE);
	bn_db=uidb_alloc(DB_OPT_BASE);
	boarddb_read();
	bn_read();
	return 0;
}

