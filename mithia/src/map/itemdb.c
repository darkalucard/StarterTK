
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "itemdb.h"
#include "map.h"
#include "db.h"
#include "malloc.h"
#include "core.h"
#include "script.h"
#include "db_mysql.h"

DBMap* item_db;
//DBMap* custom_db;

int itemdb_searchname_sub(DBKey* key,void *data,va_list ap) {
	struct item_data *item=(struct item_data *)data,**dst;
	char *str;
	
	str=va_arg(ap,char *);
	dst=va_arg(ap,struct item_data**);
	nullpo_ret(0, str);
	
	if(strcmpi(item->yname,str)==0) {
		*dst=item;
	} else if(strcmpi(item->name,str)==0) {
		*dst=item;
	}
	return 0;
}

struct item_data* itemdb_searchname(const char *str) {
	struct item_data *item=NULL;
	item_db->foreach(item_db,itemdb_searchname_sub,str,&item);
	return item;
}

struct item_data* itemdb_search(unsigned int id) {
	static struct item_data *db=NULL;
	if (db && db->id == id)
		return db;
	
	db = uidb_get(item_db, id);
	if (db)
		return db;
	
	CALLOC(db, struct item_data, 1);
	uidb_put(item_db, id, db);
	db->id = id;
	db->type = ITM_ETC;
	db->max_amount = 1;
	strcpy(db->name, "??");
	strcpy(db->text, "??");
	strcpy(db->buytext, "??");
	return db;
}

	
struct item_data* itemdb_searchexist(unsigned int id) {
	struct item_data *db=NULL;
	db = uidb_get(item_db, id);
	return db;
}
int itemdb_thrown(unsigned int id) {
	struct item_data *db=NULL;
	db=itemdb_search(id);
	return db->thrown;
}
unsigned int itemdb_id(char *str) {
	struct item_data *db=NULL;
	db=itemdb_searchname(str);
	if (db)
		return db->id;

	if ((unsigned int)strtoul(str, NULL, 10)>0) {
		db=itemdb_searchexist((unsigned int)strtoul(str, NULL, 10));
		if (db)
			return db->id;
	}
	return 0;
}

char* itemdb_name(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->name;
}

char* itemdb_yname(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->yname;
}
int itemdb_attackspeed(unsigned int id) {
	struct item_data* db;
	db=itemdb_search(id);
	return db->attack_speed;
}
int itemdb_event(unsigned int id) {
	struct item_data* db;
	db=itemdb_search(id);
	return db->event;
}
int itemdb_wisdom(unsigned int id) {
	struct item_data* db;
	db=itemdb_search(id);
	return db->wisdom;
}
int itemdb_con(unsigned int id) {
	struct item_data* db;
	db=itemdb_search(id);
	return db->con;
}

int itemdb_maxamount(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->max_amount;
}

int itemdb_sound(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->sound;
}

int itemdb_dura(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->dura;
}
unsigned int itemdb_soundhit(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->sound_hit;
}
int itemdb_look(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->look;
}

int itemdb_lookcolor(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->look_color;
}
int itemdb_mindam(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->min_dam;
}
int itemdb_maxdam(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->max_dam;
}

int itemdb_icon(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->icon;
}

int itemdb_iconcolor(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->icon_color;
}

int itemdb_type(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->type;
}

int itemdb_level(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->level;
}

int itemdb_might(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->might;
}

int itemdb_mightreq(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->might_req;
}

int itemdb_grace(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->grace;
}

int itemdb_gracereq(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->grace_req;
}

int itemdb_will(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->will;
}

int itemdb_willreq(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->will_req;
}

int itemdb_sex(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->sex;
}
int itemdb_exchangeable(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->exchangeable;
}
int itemdb_droppable(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->droppable;
}
int itemdb_depositable(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->depositable;
}

char* itemdb_script(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->script;
}

int itemdb_vita(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->vita;
		
}

int itemdb_mana(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->mana;
}

int itemdb_ac(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->ac;
}

int itemdb_dam(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->dam;
}
int itemdb_hit(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->hit;
}
char* itemdb_unequipscript(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->unequip_script;
}
char* itemdb_equipscript(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->equip_script;
}

int itemdb_rank(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->rank;
}

int itemdb_class(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->class;
}

int itemdb_price(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->price;
}

char* itemdb_text(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->text;
}

char* itemdb_buytext(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->buytext;
}

unsigned int itemdb_reqvita(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->req_vita;
}
unsigned int itemdb_reqmana(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->req_mana;
}
int itemdb_unique(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->unique;
}
int itemdb_healing(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->healing;
}
int itemdb_ethereal(unsigned int id) {
	struct item_data *db;
	db=itemdb_search(id);
	return db->ethereal;
}
int itemdb_dodge(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->dodge;
}
int itemdb_parry(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->parry;
}
int itemdb_block(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->block;
}

int itemdb_protection(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->protection;
}

int itemdb_resist(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->resist;
}

int itemdb_vregen(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->vregen;
}

int itemdb_mregen(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->mregen;
}

int itemdb_physdeduct(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->physdeduct;
}

int itemdb_time(unsigned int id) {
	struct item_data *db;
	db = itemdb_search(id);
	return db->time;
}

int itemdb_read(const char *itemdb_file) {
	struct item_data *db;
	unsigned int i, itm=0;
	struct item_data item;
	SqlStmt* stmt;
	
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	memset(&item,0,sizeof(item));
		
		if(SQL_ERROR==SqlStmt_Prepare(stmt,"SELECT `ItmId`, `ItmDescription`, `ItmIdentifier`, `ItmMark`, `ItmType`, `ItmBuyPrice`, `ItmSellPrice`, `ItmMaximumAmount`, `ItmPthId`, `ItmSex`, `ItmLevel`, `ItmLook`, "
			"`ItmLookColor`, `ItmIcon`, `ItmIconColor`, `ItmSound`, `ItmDurability`, `ItmMight`, `ItmWill`, `ItmGrace`, `ItmArmor`, `ItmHit`, `ItmDam`, `ItmVita`, `ItmMana`, `ItmProtection`, `ItmMinimumDamage`, "
			"`ItmMaximumDamage`, `ItmText`, `ItmBuyText`, `ItmExchangeable`, `ItmDepositable`, `ItmDroppable`, `ItmWisdom`, `ItmCon`, `ItmAttackSpeed`, `ItmEvent`, `ItmVitaRequired`, `ItmManaRequired`, "
			"`ItmThrown`, `ItmUniqueAmount`, `ItmIndestructible`, `ItmTimer` FROM `Items`")
		|| SQL_ERROR==SqlStmt_Execute(stmt)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,0,SQLDT_UINT,&item.id,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,1,SQLDT_STRING,&item.name,sizeof(item.name),NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,2,SQLDT_STRING,&item.yname,sizeof(item.yname),NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,3,SQLDT_INT,&item.rank,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,4,SQLDT_UCHAR,&item.type,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,5,SQLDT_INT,&item.price,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,6,SQLDT_INT,&item.sell,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,7,SQLDT_INT,&item.max_amount,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,8,SQLDT_UCHAR,&item.class,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,9,SQLDT_UCHAR,&item.sex,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,10,SQLDT_UCHAR,&item.level,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,11,SQLDT_INT,&item.look,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,12,SQLDT_INT,&item.look_color,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,13,SQLDT_INT,&item.icon,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,14,SQLDT_UCHAR,&item.icon_color,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,15,SQLDT_UINT,&item.sound,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,16,SQLDT_INT,&item.dura,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,17,SQLDT_INT,&item.might,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,18,SQLDT_INT,&item.will,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,19,SQLDT_INT,&item.grace,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,20,SQLDT_INT,&item.ac,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,21,SQLDT_INT,&item.hit,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,22,SQLDT_INT,&item.dam,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,23,SQLDT_INT,&item.vita,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,24,SQLDT_INT,&item.mana,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,25,SQLDT_INT,&item.protection,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,26,SQLDT_UINT,&item.min_dam,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,27,SQLDT_UINT,&item.max_dam,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,28,SQLDT_STRING,&item.text,sizeof(item.text),NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,29,SQLDT_STRING,&item.buytext,sizeof(item.buytext),NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,30,SQLDT_INT,&item.exchangeable,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,31,SQLDT_INT,&item.depositable,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,32,SQLDT_INT,&item.droppable,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,33,SQLDT_INT,&item.wisdom,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,34,SQLDT_INT,&item.con,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,35,SQLDT_INT,&item.attack_speed,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,36,SQLDT_UINT,&item.event,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,37,SQLDT_UINT,&item.req_vita,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,38,SQLDT_UINT,&item.req_mana,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,39,SQLDT_INT,&item.thrown,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,40,SQLDT_INT,&item.unique,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,41,SQLDT_UCHAR,&item.ethereal,0,NULL,NULL)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,42,SQLDT_UINT,&item.time,0,NULL,NULL)
		)
			{
				SqlStmt_ShowDebug(stmt);
				SqlStmt_Free(stmt);
				return 0;
			}
			
		itm=SqlStmt_NumRows(stmt);
		for(i=0;i<itm && SQL_SUCCESS==SqlStmt_NextRow(stmt);i++) {
			db=itemdb_search(item.id);
			memcpy(db,&item,sizeof(item));
			db->icon+=49152;
		}
		
		SqlStmt_Free(stmt);
		
	printf("Item db read done. %d item loaded!\n", itm);
	return 0;
}

static int itemdb_final(void *key,void *data,va_list ap) {
	struct item_data *db;
	nullpo_ret(0, db=data);

	FREE(db->script);
	FREE(db);

	return 0;
}

int itemdb_term() {
	if(item_db){
		//numdb_final(item_db,itemdb_final);
		db_destroy(item_db);
	}
	return 0;
}

int itemdb_init() {
	item_db = uidb_alloc(DB_OPT_BASE);
	//custom_db = uidb_alloc(DB_OPT_BASE);
	itemdb_read(ITEMDB_FILE);
	//itemdb_readc();
	return 0;
}

