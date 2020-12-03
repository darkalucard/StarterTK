
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "class_db.h"
#include "db.h"
#include "malloc.h"
#include "core.h"
#include "db_mysql.h"
#include "metan.h"
#include "strlib.h"

DBMap *class_db;




struct class_data* classdb_search(int id) {
	static struct class_data *db=NULL;
	if (db && db->id == id)
		return db;

	db = uidb_get(class_db, id);
	if (db)
		return db;
	
	CALLOC(db, struct class_data, 1);
	uidb_put(class_db, id, db);
	db->id = id;
	db->chat=0;
	strcpy(db->rank0,"??");
	//strcpy(db->rank_name[0], "??");

	return db;
}

struct class_data* classdb_searchexist(int id) {
	struct class_data *db=NULL;
	
	db = uidb_get(class_db, id);
	return db;
}
int classdb_chat(int id) {
	struct class_data *db=NULL;
	
	db=classdb_search(id);
	return db->chat;
}
int classdb_icon(int id) {
	struct class_data *db=NULL;
	
	db=classdb_search(id);
	return db->icon;
}
char *classdb_name(int id,int a) {
	struct class_data *db=NULL;
	
	db = uidb_get(class_db, id);
	
	switch(a) {
	case 0:
	  return db->rank0;
	case 1:
	  return db->rank1;
	case 2:
	  return db->rank2;
	case 3:
	  return db->rank3;
	case 4:
	  return db->rank4;
	case 5:
	  return db->rank5;
	case 6:
	  return db->rank6;
	case 7:
	  return db->rank7;
	case 8:
	  return db->rank8;
	case 9:
	  return db->rank9;
	case 10:
	  return db->rank10;
	case 11:
	  return db->rank11;
	case 12:
	  return db->rank12;
	case 13:
	  return db->rank13;
	case 14:
	  return db->rank14;
	case 15:
	  return db->rank15;
	default:
	  return db->rank0;
	}
}

int classdb_path(int id) {
	struct class_data *db=NULL;
	
	db = uidb_get(class_db, id);
	
	if (db) {
		return db->path;
	} else {
		return 0;
	}
}

unsigned int classdb_level(int path,int lvl) {
	struct class_data *db=NULL;
	
	db=uidb_get(class_db,path);
	
	if (db) {
		return db->level[lvl];
	 } else {
		return 0;
	}
}


	
int classdb_read(const char *classdb_file) {

	struct class_data *db = NULL;
	int i, cls=0;
	int x;
	StringBuf buf;
	SqlStmt* stmt;
	struct class_data c;
	
	memset(&c,0,sizeof(c));
	
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	StringBuf_Init(&buf);
	StringBuf_AppendStr(&buf,"SELECT `PthId`, `PthType`, `PthChat`, `PthIcon`");
	for (i = 0; i < 16; i++)
		StringBuf_Printf(&buf, ", `PthMark%d`", i);
	
	StringBuf_AppendStr(&buf, " FROM `Paths`");
	
               //CALLOC(cdata[atoi(str[1])],struct class_data,1);
		if(SQL_ERROR == SqlStmt_Prepare(stmt,StringBuf_Value(&buf))
		|| SQL_ERROR == SqlStmt_Execute(stmt)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &c.id,0,NULL,NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_INT, &c.path, 0, NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_INT, &c.chat, 0, NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_INT, &c.icon, 0, NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_STRING, &c.rank0, sizeof(c.rank0), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_STRING, &c.rank1, sizeof(c.rank1), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_STRING, &c.rank2, sizeof(c.rank2), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 7, SQLDT_STRING, &c.rank3, sizeof(c.rank3), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 8, SQLDT_STRING, &c.rank4, sizeof(c.rank4), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 9, SQLDT_STRING, &c.rank5, sizeof(c.rank5), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 10, SQLDT_STRING, &c.rank6, sizeof(c.rank6), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 11, SQLDT_STRING, &c.rank7, sizeof(c.rank7), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 12, SQLDT_STRING, &c.rank8, sizeof(c.rank8), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 13, SQLDT_STRING, &c.rank9, sizeof(c.rank9), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 14, SQLDT_STRING, &c.rank10, sizeof(c.rank10), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 15, SQLDT_STRING, &c.rank11, sizeof(c.rank11), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 16, SQLDT_STRING, &c.rank12, sizeof(c.rank12), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 17, SQLDT_STRING, &c.rank13, sizeof(c.rank13), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 18, SQLDT_STRING, &c.rank14, sizeof(c.rank14), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 19, SQLDT_STRING, &c.rank15, sizeof(c.rank15), NULL, NULL)
		)
		{
			SqlStmt_ShowDebug(stmt);
			SqlStmt_Free(stmt);
			StringBuf_Destroy(&buf);
			return 0;
		}
		
		//sql_request("SELECT * FROM classdb");
		cls=SqlStmt_NumRows(stmt);
		for(i=0;i<cls && SQL_SUCCESS == SqlStmt_NextRow(stmt);i++) {
			db = classdb_search(c.id);
			memcpy(db,&c,sizeof(c));
		}

	SqlStmt_Free(stmt);
	StringBuf_Destroy(&buf);
	printf("Class db read done. %d classes loaded!\n", cls);
	return 0;
}
int leveldb_read(const char *leveldb_file) {
	FILE *fp;
	char line[1024];
	int lines=0;
	char *str[100], *p, *np;
	struct class_data *db;
	int i, cls=0;
	int x;
	fp = fopen(leveldb_file, "r");
	if (fp==NULL) {
		printf("DB_ERR: Can't read level db (%s).", leveldb_file);
		exit(1);
	}

	while (fgets(line,sizeof(line),fp)) {
		lines++;
		if (line[0] == '/' && line[1] == '/')
			continue;
		memset(str,0,sizeof(str));
		for(i=0,np=p=line;i<50 && p;i++) {
			str[i]=p;
			p=strchr(p,',');
			if(p){ *p++=0; np=p; }
		}
		
		cls++;
		db = classdb_search((unsigned int)strtoul(str[0], NULL, 10));
		
		for(x=1;x<50;x++) {
			db->level[x]=(unsigned int)strtoul(str[x], NULL, 10);
		}
	}

	fclose(fp);
	printf("Level db read done. %d class levels loaded!\n", cls);
	return 0;
}
static int classdb_final(void *key,void *data,va_list ap) {
	struct class_data *db;
	nullpo_ret(0, db=data);

	FREE(db);

	return 0;
}

int classdb_term() {
	if(class_db){
		//numdb_final(class_db,classdb_final);
		class_db=NULL;
	}
	return 0;
}

int classdb_init() {
	class_db = uidb_alloc(DB_OPT_BASE);
	classdb_read(CLASSDB_FILE);
	//leveldb_read(LEVELDB_FILE);
	return 0;
}

