
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "creation.h"
#include "map.h"
#include "db.h"
#include "malloc.h"
#include "core.h"
#include "script.h"
#include "socket.h"
#include "strlib.h"
#include "db_mysql.h"

DBMap *create_db;


struct creation_data* createdb_search(int id) {
	static struct creation_data *db=NULL;
	if (db && db->id == id)
		return db;

	db = uidb_get(create_db, id);
	if (db)
		return db;

	CALLOC(db, struct creation_data, 1);
	uidb_put(create_db, id, db);
	db->id = id;
	//strcpy(db->name, "??");

	return db;
}

struct creation_data* createdb_searchexist(int id) {
	struct creation_data *db=NULL;
	db = uidb_get(create_db, id);
	return db;
}



int createdb_read(const char *createdb_file) {
	FILE *fp;
	struct creation_data *db;
	int i, itm=0,x,id,count;
	SqlStmt* stmt=NULL;
	struct creation_data data;
	StringBuf buf;


	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	StringBuf_Init(&buf);

	StringBuf_AppendStr(&buf,"SELECT `id`,`item_created`,`item_count`");

	for(x=1;x<11;x++)
		StringBuf_Printf(&buf,",`item%d`",x);

	for(x=1;x<11;x++)
		StringBuf_Printf(&buf,",`amount%d`",x);

	StringBuf_AppendStr(&buf," FROM `createdb`");

	memset(&data,0,sizeof(data));

		if(SQL_ERROR == SqlStmt_Prepare(stmt,StringBuf_Value(&buf))
		|| SQL_ERROR == SqlStmt_Execute(stmt)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &id,0, NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_INT, &data.item_created,0, NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_INT, &data.amount,0, NULL, NULL)
		) {
			SqlStmt_ShowDebug(stmt);
			SqlStmt_Free(stmt);
			StringBuf_Destroy(&buf);
			return 0;
		}

			for(x=0;x<10;x++) {
				SqlStmt_BindColumn(stmt, x+3, SQLDT_INT, &data.item[x], 0,NULL, NULL);
				SqlStmt_BindColumn(stmt, x+13, SQLDT_INT, &data.item_count[x],0,NULL,NULL);
			}

		for(itm=0;SQL_SUCCESS == SqlStmt_NextRow(stmt);itm++) {

			db = createdb_search(id);
			db->item_created=data.item_created;
			db->amount=data.amount;
			db->count=0;
			for(x=0;x<10;x++) {
				db->item[x]=data.item[x];
				db->item_count[x]=data.item_count[x];
				if(db->item[x]>0)
					db->count++;

			}


		}

	SqlStmt_Free(stmt);
	StringBuf_Destroy(&buf);

	printf("Creation db read done. %u creations loaded!\n", itm);
	return 0;
}
int create_check_sub(void *key, void *data, va_list ap) {
	struct creation_data *db=(struct creation_data*)data;
	USER *sd=va_arg(ap,USER*);
	int item_c=va_arg(ap,int);
	int* item;
	int* item_amount;
	int items_found=0;
	int x,y;
	int items[10],dbitems[10];
	int items_amount[10],dbitems_amount[10];
	if(db->count!=item_c) return 0;
	if(sd->creation_works) return 0;
	item=va_arg(ap,int*);

	item_amount=va_arg(ap,int*);
	for(x=0;x<10;x++){
		items[x]=item[x];
		items_amount[x]=item_amount[x];
		dbitems[x]=db->item[x];
		dbitems_amount[x]=db->item_count[x];
	}

	for(y=0;y<item_c;y++) {
		for(x=0;x<item_c;x++) {
			if(dbitems[x]==items[y] && dbitems_amount[x]==items_amount[y]) {
				items[y]=0;
				items_amount[y]=0;
				dbitems[x]=-1;
				dbitems_amount[x]=-1;
				items_found++;
			}
		}
	}

	if(items_found==item_c) {
		sd->creation_works=1;
		sd->creation_item=db->item_created;
		sd->creation_itemamount=db->amount;
	}
	return 0;
}
int createdb_start(USER *sd) {
	int item_c=RFIFOB(sd->fd,5);
	int item[10],item_amount[10];
	int item_s[10];
	int len=6;
	int x;
	int curitem;
	struct item *fl;
	
	for(x=0;x<item_c;x++) {
		curitem=RFIFOB(sd->fd,len)-1;
		item_s[x]=curitem;
		item[x]=sd->status.inventory[curitem].id;

		if(itemdb_type(item[x])<3 || itemdb_type(item[x])>17) {
			item_amount[x]=RFIFOB(sd->fd,len+1);
			len+=2;
		} else {
			item_amount[x]=1;
			len+=1;
		}

	}
	sd->creation_works=0;
	sd->creation_item=0;
	sd->creation_itemamount=0;
	
	if(item_c) {
		create_db->foreach(create_db,create_check_sub,sd,item_c,item,item_amount);
	} else {
		return 0;
	}

	if(sd->creation_works) {

		for(x=0;x<item_c;x++) {
	//		printf("%d, %d\n",item_s[x],item_amount[x]);
			pc_delitem(sd,item_s[x],item_amount[x],25);
		}
		CALLOC(fl,struct item,1);
		fl->id=sd->creation_item;
		fl->dura=itemdb_dura(fl->id);
		fl->amount=sd->creation_itemamount;
		pc_additem(sd,fl);
		FREE(fl);
		clif_sendminitext(sd, "You were successful!",0);
	} else {
		sl_doscript_blargs("onCreation", NULL, 1, &sd->bl);
	}
	return 0;
}


static int createdb_final(void *key,void *data,va_list ap) {
	struct creation_data *db;
	nullpo_ret(0, db=data);


	FREE(db);

	return 0;
}

int createdb_term() {
	if(create_db){
		//numdb_final(create_db,createdb_final);
		db_destroy(create_db);
	}
	
	return 0;
}

int createdb_init() {
	create_db = uidb_alloc(DB_OPT_BASE);
	//createdb_read(CREATEDB_FILE);
	return 0;
}

