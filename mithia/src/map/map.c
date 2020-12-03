
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdarg.h>
#include <sys/time.h>
#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif
#include "../common/core.h"
#include "../common/socket.h"
#include "../common/timer.h"
#include "version.h"
#include "map.h"
#include "crypt.h"
#include "clif.h"
#include "intif.h"
#include "mmo.h"
#include "malloc.h"
#include "db.h"
#include "class_db.h"
#include "itemdb.h"
#include "recipedb.h"
#include "magic.h"
#include "pc.h"
#include "sl.h"
#include "npc.h"
#include "script.h"
#include "mob.h"
#include "board_db.h"
#include "clan_db.h"
#include "db_mysql.h"
#include "guide.h"
#include "zlib.h"
#include "strlib.h"

#include <netdb.h>
#ifndef _MAP_SERVER_
#define _MAP_SERVER_
#endif
char sql_id[32]="";
char sql_pw[32]="";
char sql_ip[32]="";
char sql_db[32]="";
int sql_port;
Sql* sql_handle=NULL;

DBMap* item_db;
DBMap* id_db;
DBMap* class_db;
DBMap* magic_db;
DBMap* board_db;
DBMap* clan_db;
DBMap* guide_db;
DBMap* mobsearch_db;
DBMap* mobid_db=NULL;
struct map_msg_data map_msg[MSG_MAX];
struct town_data towns[255];
int town_n=0;
int map_loadgameregistry();

//item id pool
char *object;
int object_n=0;

struct map_src_list{
	struct map_src_list *next;
	int id, pvp, spell;
	unsigned int sweeptime;
	char title[64];
	unsigned char cantalk, show_ghosts, region, indoor, warpout, bind;
	unsigned short bgm, light, weather;
	char *mapfile;
};
struct map_src_list *map_src_first = NULL;
struct map_src_list *map_src_last = NULL;

char map_ip_s[16];
char log_ip_s[16];

unsigned int map_ip;
unsigned int map_port = 2011;
unsigned int log_ip;
unsigned int log_port=2010;
unsigned int logip;
unsigned int logport=4000;
int log_fd;
int char_fd;
int map_fd;
unsigned int myip=0;
char char_id[32];
char char_pw[32];
char char_ip_s[16];
int char_ip;
int char_port;
int save_time = 5*60000;
int dump_save = 0;
int xp_rate;
int d_rate;
int map_max=0;
//unsigned int blcount_t=0;
int auth_n = 0;
struct userlist_data userlist;
struct map_data *map;
struct game_data gamereg;
int map_n = 0;
int oldHour;
int oldMinute;
int cronjobtimer;


#define BL_LIST_MAX 32768

struct block_list *bl_list[BL_LIST_MAX];

static struct block_list bl_head;
int bl_list_count = 0;

unsigned int u_time=0;
unsigned int r_time=0;

time_t gettickthing(void) {

	return time(NULL);
}
int command_input(char* val) {
	return 0;
}
int map_moveblock(struct block_list *bl, int x1, int y1)
{
	int x0 = bl->x, y0 = bl->y;
	//struct status_change *sc = NULL;
	if(x1>=map[bl->m].xs) x1=map[bl->m].xs-1;
	if(y1>=map[bl->m].ys) y1=map[bl->m].ys-1;
	int moveblock = ( x0/BLOCK_SIZE != x1/BLOCK_SIZE || y0/BLOCK_SIZE != y1/BLOCK_SIZE);
	//int moveblock=1;
	if (!bl->prev) {
		//Block not in map, just update coordinates, but do naught else.
		bl->x = x1;
		bl->y = y1;
		return 0;
	}

	//TODO: Perhaps some outs of bounds checking should be placed here?
	if (moveblock) map_delblock(bl);
	bl->x = x1;
	bl->y = y1;
	if (moveblock) map_addblock(bl);
	return 0;
}

int map_timerall(int n, int b) {
	USER *sd;
	int x;
	u_time+=250;
	r_time+=250;


	for(x=0;x<fd_max;x++) {
		if(session[x] && (sd=session[x]->session_data) && !session[x]->eof) {
			pc_timer(sd->bl.id,0);
			if(r_time>5000) {
				pc_regen(sd->bl.id,0);
			}


		}
	}




	if(r_time>5000) r_time=0;

}
int map_timerthing(int x, int b) {
	return 0;


}
int nmail_sendmessage(USER *sd, char* message, int other,int type) {
	int len=0;
	isactive(sd);
	
	if (!session[sd->fd])
	{
		session[sd->fd]->eof = 8;
		return 0;
	}
	
	WFIFOHEAD(sd->fd,65535+3);
	WFIFOB(sd->fd,0)=0xAA;
	WFIFOB(sd->fd,3)=0x31;
	WFIFOB(sd->fd,4)=0x03;
	WFIFOB(sd->fd,5)=other;
	WFIFOB(sd->fd,6)=type;
	len+=strlen(message);
	WFIFOB(sd->fd,7)=len;
	len++;
	strcpy(WFIFOP(sd->fd,8),message);
	WFIFOB(sd->fd,len+7)=7;
	WFIFOW(sd->fd,1)=SWAP16(len+5);

	WFIFOSET(sd->fd,encrypt(sd->fd));
	return 0;
}

struct block_list* map_id2bl(unsigned int id) {
	struct block_list *bl=NULL;
    bl = uidb_get(id_db, id);
	return bl;
}

void map_deliddb(struct block_list* bl) {
	uidb_remove(id_db, bl->id);
}

void map_addiddb(struct block_list *bl) {

	//if(bl->type==BL_MOB)
		//uidb_put(mobid_db,bl->id,bl);

	uidb_put(id_db, bl->id, bl);


}


void map_foreachmob(int (*func)(struct block_list* bl, va_list args), ...)
{
	DBIterator* iter;
	struct block_list* md;

	iter = db_iterator(mobid_db);
	for( md = (struct mob_data*)dbi_first(iter); dbi_exists(iter); md = (struct block_list*)dbi_next(iter) )
	{
		va_list args;
		int ret;

		va_start(args, func);
		ret = func(md, args);
		va_end(args);
		if( ret == -1 )
			break;// stop iterating
	}
	dbi_destroy(iter);
}

void map_initiddb() {
	id_db = uidb_alloc(DB_OPT_BASE);
	mobid_db=uidb_alloc(DB_OPT_BASE);
	//mobsearch_db=numdb_init();
}

int map_finaliddb(void *key,void *data,va_list ap) {
	struct block_list *db;
	struct npc_data *nd;
	nullpo_ret(0, db=data);

	map_delblock(db);
	switch (db->id) {
	case BL_NPC:
		nd=data;
		switch(db->subtype) {
		case FLOOR:
		case SCRIPT:
			//FREE(nd->script);
			break;
		}
	case BL_PC:
	case BL_ITEM:
		FREE(db);
		break;
	}
	return 0;
}

void map_termiddb() {
	if(id_db){
		//uidb_final(id_db,map_finaliddb);
		id_db=NULL;
	}
}

void map_clritem() {
	FREE(object);
	object_n = 0;
}

void map_delitem(unsigned int id) {
	struct block_list *bl;
	bl = map_id2bl(id);
	if (!bl)
		return;

	map_deliddb(bl);
	map_delblock(bl);
	FREE(bl);

	id -= FLOORITEM_START_NUM;
	if (id>=object_n || id<0)
		return;

	object[id] = 0;
}

void map_additem(struct block_list *bl) {
	unsigned int i;
	for (i=0;i<object_n;i++) {
		if (!object[i])
			break;
	}
	if (i>=MAX_FLOORITEM) {
		printf("MAP_ERR: Item reached max item capasity.\n");
		return;
	}

	if (i>=object_n) {
		if (object_n) {
			REALLOC(object, char, i+256);
		} else {
			CALLOC(object, char, 256);
		}
		object_n = i+256;
	}

	object[i] = 1;
	i += FLOORITEM_START_NUM;
	bl->id = i;
	bl->type = BL_ITEM;
	bl->prev = NULL;
	bl->next = NULL;
	map_addiddb(bl);
	map_addblock(bl);
}

int map_src_clear() {
	struct map_src_list *p=map_src_first;

	while(p) {
		struct map_src_list *p2=p;
		p=p->next;
		FREE(p2->mapfile);
		FREE(p2);
	}

	map_src_first=NULL;
	map_src_last=NULL;
	return 0;
}
int map_town_add(const char *r1) {
	char town_name[1024];
	//printf("%s\n",r1);
	if(sscanf(r1,"%[^\r\n]",town_name) !=1)
		return -1;
	memset(towns[town_n].name,0,32);
	strncpy(towns[town_n].name,town_name,32);
	//printf("%s\n",towns[town_n].name);
	town_n++;
	return 0;
}
int map_src_add(const char *r1) {
	int map_id,pvp,spell;
	unsigned int sweeptime;
	unsigned short map_bgm,light,weather;
	unsigned char cantalk,showghosts,region,indoor,warpout, bind;
	struct map_src_list *new;
	char map_title[1024], map_file[1024];
	if (sscanf(r1, "%d,%[^,],%d,%d,%d,%u,%u,%u,%u,%u,%u,%u,%u, %u,%s", &map_id, map_title, &map_bgm, &pvp, &spell, &light, &weather, &sweeptime, &cantalk, &showghosts, &region, &indoor, &warpout, &bind, map_file) < 13) {
		return -1;
	}
//[^,],[^\r\n]

	CALLOC(new, struct map_src_list, 1);
	CALLOC(new->mapfile, char, strlen(map_file) + 1);
	new->next = NULL;

	new->id = map_id;
	new->bgm = map_bgm;
	new->pvp = pvp;
	new->spell=spell;
	new->light=light;
	new->weather=weather;
	new->sweeptime=sweeptime;
	new->cantalk=cantalk;
	new->show_ghosts=showghosts;
	new->region=region;
	new->indoor=indoor;
	new->warpout=warpout;
	new->bind=bind;
	strncpy(new->title, map_title, 64);
	strncpy(new->mapfile, map_file, strlen(map_file));

	if (map_src_first == NULL)
		map_src_first = new;
	if (map_src_last)
		map_src_last->next = new;

	map_src_last = new;
	return 0;
}

USER* map_id2sd(unsigned int id) {
	int i;
	USER *sd=NULL;

	sd=(USER*)uidb_get(id_db,id);
	//nullpo_ret(0,sd=(USER*)uidb_get(id_db,id));
	return sd;
}
int isPlayerActive(USER* sd) {
	if(!sd) return 0;
	if(!sd->fd) return 0;
	if(!session[sd->fd]) { //This is an anomally, save, and exit.
		printf("Abnormal, Player exists but session does not(%s).  Attempting to recreate.\n",sd->status.name);
		//log_session(sd->fd,"Abnormal client FD(%d)\n");
		//create_session(sd->fd); //Recreate socket data.
		//session[sd->fd]->session_data=sd;
		//session[sd->fd]->eof=1;
		//clif_handle_disconnect(sd);
		//session_eof(sd->fd);
		//FREE(sd);
		//return 1;
		return 0;
	}
	return 1;
}
MOB* map_id2mob(unsigned int id) {
	int i;
	MOB *mob;
	struct block_list* bl;

	if (id < MOB_START_NUM) id += MOB_START_NUM - 1;
	bl=map_id2bl(id);
	if(bl) {
		if(bl->type==BL_MOB) {
			mob=(MOB*)bl;
			return mob;
		}
	}
	return NULL;
}

NPC* map_id2npc(unsigned int id) {
	int i;
	NPC *npc;
	struct block_list* bl;

	if (id < NPC_START_NUM)	id += NPC_START_NUM - 2;
	bl=map_id2bl(id);
	if(bl) {
		if(bl->type==BL_NPC) {
			npc=(NPC*)bl;
			return npc;
		}
	}
	return NULL;
}

NPC* map_name2npc(char *name) {
	unsigned int i;
	NPC *nd=NULL;
	
	for (i = NPC_START_NUM; i <= npc_id; i++) {
		nd = map_id2npc(i);
		
		if (nd && !strcmpi(nd->name, name)) {
			return nd;
		}
	}
	
	return NULL;
}

FLOORITEM* map_id2fl(unsigned int id) {
	int i;
	FLOORITEM *fl;
	struct block_list* bl;

	bl=map_id2bl(id);
	if(bl) {
		if(bl->type==BL_ITEM) {
			fl=(FLOORITEM*)bl;
			return fl;
		}
	}
	return NULL;
}

USER* map_name2sd(char *name) {
	int i;
	USER *sd=NULL;

	for(i=0;i<fd_max;i++){
		if(session[i] && (sd=session[i]->session_data)){
			if(strcmpi(name, sd->status.name) == 0)
				return sd;
		}
	}
	return NULL;
}

void map_termblock() {
	//FREE(bl_head);
}

void map_initblock() {
	//CALLOC(bl_head, struct block_list, 1);
}

/*int map_freeblock(void *bl) {
	if (!bl_free_lock) {
		FREE(bl);
	} else {
		if (bl_free_count >= BL_FREE_MAX) {
			printf("BL_ERR: Too many block free list! Block free count: %d\n", bl_free_count);
		} else
			bl_free[bl_free_count++] = bl;
	}
	return bl_free_lock;
}

int map_freeblock_lock() {
	return ++bl_free_lock;
}

int map_freeblock_unlock() {
	if(!--bl_free_lock) {
		int i;
		for(i=0;i<bl_free_count;i++)
			FREE(bl_free[i]);
		bl_free_count=0;
	}
	return bl_free_lock;
}
*/
int map_addblock(struct block_list *bl) {

	int m, x, y, pos;

	nullpo_ret(0, bl);
	if (bl->prev != NULL) {
		ShowError("map_addblock: bl->prev != NULL\n");
		return 1;
	}


	m = bl->m;
	x = bl->x;
	y = bl->y;
	if( !map_isloaded(m))
	{
		ShowError("map_addblock: invalid map id (%d), only %d are loaded.\n", m, map_n);
		return 1;
	}
	if( x < 0 || x >= map[m].xs || y < 0 || y >= map[m].ys )
	{
		ShowError("map_addblock: out-of-bounds coordinates map: %d -> (%d,%d) id: %u\n",m,x,y,bl->id);

		//ShowError("map_addblock: out-of-bounds coordinates (\"%s\",%d,%d), map is %dx%d\n", map[m].name, x, y, map[m].xs, map[m].ys);
		return 1;
	}

	pos = x/BLOCK_SIZE+(y/BLOCK_SIZE)*map[m].bxs;

	if (bl->type == BL_MOB) {
		bl->next = map[m].block_mob[pos];
		bl->prev = &bl_head;
		if (bl->next) bl->next->prev = bl;
		map[m].block_mob[pos] = bl;
	} else {
		bl->next = map[m].block[pos];
		bl->prev = &bl_head;
		if (bl->next) bl->next->prev = bl;
		map[m].block[pos] = bl;
	}

	if(bl->type==BL_PC)
		map[m].user++;

		if(bl->prev<0x100 || bl->next <0x100 ) {
			if(bl->next) {
				printf("Prev = %u : Next = %u\n",bl->prev,bl->next);
			}
		}
	return 0;
}

int map_delblock(struct block_list *bl) {
	int pos;
	nullpo_ret(0, bl);

	// ?にblocklistから?けている

	if (bl->prev == NULL) {
		if (bl->next != NULL) {
			// prevがNULLでnextがNULLでないのは有ってはならない
			ShowError("map_delblock error : bl->next!=NULL\n");
		}
		return 0;
	}

	pos = bl->x/BLOCK_SIZE+(bl->y/BLOCK_SIZE)*map[bl->m].bxs;

	if (bl->next)
		bl->next->prev = bl->prev;
	if (bl->prev == &bl_head) {
		// リストの頭なので、map[]のblock_listを更新する
		if (bl->type == BL_MOB) {
			map[bl->m].block_mob[pos] = bl->next;
		} else {
			map[bl->m].block[pos] = bl->next;
		}
	} else {
		bl->prev->next = bl->next;
	}

	if(bl->type==BL_PC)
		map[bl->m].user--;


	bl->next = NULL;
	bl->prev = NULL;

	return 0;
}

int map_foreachinarea(int (*func)(struct block_list*,va_list),int m,int x,int y,int area,int type,...) {
	int mx,my;
	int nAreaSizeX=AREAX_SIZE,nAreaSizeY=AREAY_SIZE;
	va_list ap;
	va_start(ap,type);

		int x0 = x - 9;
		int y0 = y - 8;
		int x1 = x + 9;
		int y1 = y + 8;

		if(x0 < 0){
		    x1 += -x0;
		    x0 = 0;
		    if(x1 >= map[m].xs)
                x1 = map[m].xs - 1;
		}
		if(y0 < 0){
		    y1 += -y0;
		    y0 = 0;
		    if(y1 >= map[m].ys)
                y1 = map[m].ys - 1;
		}
		if(x1 >= map[m].xs){
		    x0 -= x1 - map[m].xs + 1;
		    x1 = map[m].xs - 1;
		    if(x0 < 0)
                x0 = 0;
		}
		if(y1 >= map[m].ys){
		    y0 -= y1 - map[m].ys + 1;
		    y1 = map[m].ys - 1;
		    if(y0 < 0)
                y0 = 0;
		}

	switch(area) {
	case AREA:


		map_foreachinblockva(func,m,x-(nAreaSizeX+1),y-(nAreaSizeY+1),x+(nAreaSizeX+1),y+(nAreaSizeY+1),type,ap);
		break;
	case CORNER:
		if (map[m].xs > (nAreaSizeX*2+1) && map[m].ys > (nAreaSizeY*2+1)) {
			if (x < (nAreaSizeX*2+2) && x > nAreaSizeX)
				map_foreachinblockva(func,m,0,y-(nAreaSizeY+1),x-(nAreaSizeX+2),y+(nAreaSizeY+1),type,ap);
			if (y < (nAreaSizeY*2+2) && y > nAreaSizeY) {
				map_foreachinblockva(func,m,x-(nAreaSizeX+1),0,x+(nAreaSizeX+1),y-(nAreaSizeY+2),type,ap);
				if (x < (nAreaSizeX*2+2) && x > nAreaSizeX)
					map_foreachinblockva(func,m,0,0,x-(nAreaSizeX+2),y-(nAreaSizeY+2),type,ap);
				else if (x > map[m].xs-(nAreaSizeX*2+3) && x < map[m].xs-(nAreaSizeX+1))
					map_foreachinblockva(func,m,x+(nAreaSizeX+2),0,map[m].xs-1,y+(nAreaSizeY+2),type,ap);
			}
			if (x > map[m].xs-(nAreaSizeX*2+3) && x < map[m].xs-(nAreaSizeX+1))
				map_foreachinblockva(func,m,x+(nAreaSizeX+2),y-(nAreaSizeY+1),map[m].xs-1,y+(nAreaSizeY+1),type,ap);
			if (y > map[m].ys-(nAreaSizeY*2+3) && y < map[m].ys-(nAreaSizeY+1)) {
				map_foreachinblockva(func,m,x-(nAreaSizeX+1),y+(nAreaSizeY+2),x+(nAreaSizeX+1),map[m].ys-1,type,ap);
				if (x < (nAreaSizeX*2+2) && x > nAreaSizeX)
					map_foreachinblockva(func,m,0,y+(nAreaSizeY+2),x-(nAreaSizeX+2),map[m].ys-1,type,ap);
				else if (x > map[m].xs-(nAreaSizeX*2+3) && x < map[m].xs-(nAreaSizeX+1))
					map_foreachinblockva(func,m,x+(nAreaSizeX+2),y+(nAreaSizeY+2),map[m].xs-1,map[m].ys-1,type,ap);
			}
		}
		break;
	case SAMEAREA:
		/*if (x <= nAreaSizeX && y <= nAreaSizeY)
			map_foreachinblockva(func,m,0,0,(nAreaSizeX*2+2),(nAreaSizeY*2+2),type,ap);
		else if (x <= nAreaSizeX && y >= map[m].ys-(nAreaSizeY+1))
			map_foreachinblockva(func,m,0,map[m].ys-(nAreaSizeY*2+3),(nAreaSizeX*2+2),map[m].ys-1,type,ap);
		else if (x >= map[m].xs-(nAreaSizeX+1) && y <= nAreaSizeY)
			map_foreachinblockva(func,m,map[m].xs-(nAreaSizeX*2+3),0,map[m].xs-1,(nAreaSizeY*2+2),type,ap);
		else if (x >= map[m].xs-(nAreaSizeX+1) && y >= map[m].ys-(nAreaSizeY+1))
			map_foreachinblockva(func,m,map[m].xs-(nAreaSizeX*2+3),map[m].ys-(nAreaSizeY*2+3),map[m].xs-1,map[m].ys-1,type,ap);
		else
			map_foreachinblockva(func,m,x-(nAreaSizeX+1),y-(nAreaSizeY+1),x+(nAreaSizeX+1),y+(nAreaSizeY+1),type,ap);*/
        map_foreachinblockva(func,m,x0,y0,x1,y1,type,ap);
		break;
	case SAMEMAP:
		map_foreachinblockva(func,m,0,0,map[m].xs-1, map[m].ys-1,type,ap);
		break;
		}

	va_end(ap);
	return 0;
}


int map_foreachinblockva(int (*func)(struct block_list*,va_list),int m,int x0,int y0,int x1,int y1,int type,va_list ap) {
	int bx,by;
	int returnCount = 0;	//total sum of returned values of func() [Skotlex]
	struct block_list *bl = NULL;
	int blockcount = 0;
	int i;

	if (m < 0)
		return 0;

	if(!map_isloaded(m))
		return 0;

	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 >= map[m].xs) x1 = map[m].xs-1;
	if (y1 >= map[m].ys) y1 = map[m].ys-1;

	if ((type &~ BL_MOB))
		for (by = y0 / BLOCK_SIZE; by <= y1 / BLOCK_SIZE; by++)
			for (bx = x0 / BLOCK_SIZE; bx <= x1 / BLOCK_SIZE; bx++)
				for (bl = map[m].block[bx+by*map[m].bxs]; bl && blockcount<BL_LIST_MAX; bl = bl->next) {
					if ((bl->type & type) && bl->x>=x0 && bl->x<=x1 && bl->y>=y0 && bl->y<=y1 && blockcount<BL_LIST_MAX)
						bl_list[blockcount++]=bl;
				}
				
	if ((type & BL_MOB))
		for (by = y0 / BLOCK_SIZE; by <= y1 / BLOCK_SIZE; by++)
			for (bx = x0 / BLOCK_SIZE; bx <= x1 / BLOCK_SIZE; bx++)
				for (bl = map[m].block_mob[bx+by*map[m].bxs]; bl && blockcount<BL_LIST_MAX; bl = bl->next) {
					MOB* mob = (MOB*)bl;
					if(mob->state != MOB_DEAD && bl->x>=x0 && bl->x<=x1 && bl->y>=y0 && bl->y<=y1 && blockcount<BL_LIST_MAX)
						bl_list[blockcount++]=bl;
				}
						
	if(blockcount>=BL_LIST_MAX)
		printf("map_foreachinarea: block count too many!\n");


	for(i=0;i<blockcount;i++)
		if(bl_list[i]->prev)	// 有?かどうかチェック
			returnCount += func(bl_list[i], ap);



	//bl_list_count = blockcount;
	return returnCount;
}


int map_foreachinblock(int (*func)(struct block_list*,va_list),int m,int x0,int y0,int x1,int y1,int type,...) {
	va_list ap;
	va_start(ap, type);
	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 >= map[m].xs) x1 = map[m].xs-1;
	if (y1 >= map[m].ys) y1 = map[m].ys-1;

	map_foreachinblockva(func,m,x0,y0,x1,y1,type,ap);

	va_end(ap);
	return 0;
}

int map_foreachincell(int (*func)(struct block_list*,va_list),int m,int x,int y,int type,...) {
	int bx,by;
	int returnCount =0;  //total sum of returned values of func() [Skotlex]
	struct block_list *bl = NULL;
	int blockcount = 0;
	int i;

	if (x < 0 || y < 0 || x >= map[m].xs || y >= map[m].ys) return 0;

	by=y/BLOCK_SIZE;
	bx=x/BLOCK_SIZE;

	if ((type &~ BL_MOB))
		for (bl = map[m].block[bx+by*map[m].bxs]; bl && blockcount<BL_LIST_MAX; bl = bl->next) {
			if((bl->type & type) && bl->x==x && bl->y==y && blockcount<BL_LIST_MAX)
			{
				if (bl->type != BL_ITEM)
					bl_list[blockcount++]=bl;
				else
				{
					FLOORITEM *fl = (FLOORITEM *)bl;
					if (itemdb_type(fl->data.id) != ITM_TRAPS)
						bl_list[blockcount++]=bl;
				}
			}
		}

	if ((type & BL_MOB))
		for (bl = map[m].block_mob[bx+by*map[m].bxs]; bl && blockcount < BL_LIST_MAX; bl = bl->next) {
			MOB* mob = (MOB*)bl;
			if(mob->state != MOB_DEAD && bl->x==x && bl->y==y && blockcount<BL_LIST_MAX)
				bl_list[blockcount++]=bl;
		}

	if(blockcount>=BL_LIST_MAX)
		printf("Map_foreachincell: block count too many!\n");
		//ShowWarning("map_foreachincell: block count too many!\n");

	//map_freeblock_lock();	// メモリからの解放を禁止する

	for(i=0;i<blockcount;i++)
		if(bl_list[i]->prev)	// 有?かどうかチェック
		{
			va_list ap;
			va_start(ap, type);
			returnCount += func(bl_list[i], ap);
			va_end(ap);
		}

	//map_freeblock_unlock();	// 解放を許可する

	//bl_list_count = blockcount;
	return returnCount;
}

int map_foreachincellwithtraps(int (*func)(struct block_list*,va_list),int m,int x,int y,int type,...) {
	int bx,by;
	int returnCount =0;  //total sum of returned values of func() [Skotlex]
	struct block_list *bl = NULL;
	int blockcount = 0;
	int i;

	if (x < 0 || y < 0 || x >= map[m].xs || y >= map[m].ys) return 0;

	by=y/BLOCK_SIZE;
	bx=x/BLOCK_SIZE;

	if ((type &~ BL_MOB))
		for (bl = map[m].block[bx+by*map[m].bxs]; bl && blockcount<BL_LIST_MAX; bl = bl->next) {
			if((bl->type & type) && bl->x==x && bl->y==y && blockcount<BL_LIST_MAX)
			{
				bl_list[blockcount++]=bl;
			}
				
		}

	if ((type & BL_MOB))
		for (bl = map[m].block_mob[bx+by*map[m].bxs]; bl && blockcount<BL_LIST_MAX; bl = bl->next) {
			MOB* mob = (MOB*)bl;
			if (mob->state != MOB_DEAD && bl->x==x && bl->y==y && blockcount<BL_LIST_MAX)
				bl_list[blockcount++]=bl;
		}

	if(blockcount>=BL_LIST_MAX)
		printf("Map_foreachincell: block count too many!\n");
		//ShowWarning("map_foreachincell: block count too many!\n");

	//map_freeblock_lock();	// メモリからの解放を禁止する

	for(i=0;i<blockcount;i++)
		if(bl_list[i]->prev)	// 有?かどうかチェック
		{
			va_list ap;
			va_start(ap, type);
			returnCount += func(bl_list[i], ap);
			va_end(ap);
		}

	//map_freeblock_unlock();	// 解放を許可する

	//bl_list_count = blockcount;
	return returnCount;
}

struct block_list* map_firstincell(int m,int x,int y,int type) {
	int bx,by;
	int i,c;
	struct block_list *bl = NULL;

	if(m < 0)
		return 0;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= map[m].xs) x = map[m].xs-1;
	if (y >= map[m].ys) y = map[m].ys-1;

	by=y/BLOCK_SIZE;
	bx=x/BLOCK_SIZE;

	if ((type &~ BL_MOB))
			for (bl = map[m].block[bx+by*map[m].bxs]; bl /*&& bl_list_count<BL_LIST_MAX*/; bl = bl->next)
				if ((bl->type & type) && bl->x == x && bl->y == y)
				{
					if (bl->type != BL_ITEM)
						return bl;
					else
					{
						FLOORITEM *fl = (FLOORITEM *)bl;
						if (itemdb_type(fl->data.id) != ITM_TRAPS)
							return bl;
					}
				}

	if ((type & BL_MOB))
			for (bl = map[m].block_mob[bx+by*map[m].bxs]; bl /*&& bl_list_count<BL_LIST_MAX*/; bl = bl->next) {
				MOB* mob = (MOB*)bl;
				if (mob->state != MOB_DEAD && bl->x==x && bl->y==y)
					return bl;
			}

	return 0;
}

struct block_list* map_firstincellwithtraps(int m,int x,int y,int type) {
	int bx,by;
	int i,c;
	struct block_list *bl = NULL;

	if(m < 0)
		return 0;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= map[m].xs) x = map[m].xs-1;
	if (y >= map[m].ys) y = map[m].ys-1;

	by=y/BLOCK_SIZE;
	bx=x/BLOCK_SIZE;

	if ((type &~ BL_MOB))
			for (bl = map[m].block[bx+by*map[m].bxs]; bl /*&& bl_list_count<BL_LIST_MAX*/; bl = bl->next)
				if ((bl->type & type) && bl->x == x && bl->y == y)
				{
					return bl;
				}

	if ((type & BL_MOB))
			for (bl = map[m].block_mob[bx+by*map[m].bxs]; bl /*&& bl_list_count<BL_LIST_MAX*/; bl = bl->next) {
				MOB* mob = (MOB*)bl;
				if(mob->state != MOB_DEAD && bl->x==x && bl->y==y)
					return bl;
			}

	return 0;
}

int map_respawn(int (*func)(struct block_list*, va_list), int m, int type, ...) {
	va_list ap;
	
	va_start(ap, type);
	map_respawnmobs(func, m, type, ap);
	va_end(ap);
	return 0;
}
	

int map_respawnmobs(int (*func)(struct block_list*, va_list), int m, int type, va_list ap) {
	int x0, x1, y0, y1, bx, by;
	int returnCount = 0;	//total sum of returned values of func() [Skotlex]
	struct block_list *bl = NULL;
	int blockcount = 0;
	int i;

	if (m < 0)
		return 0;

	if (!map_isloaded(m))
		return 0;

	x0 = 0;
	y0 = 0;
	x1 = map[m].xs - 1;
	y1 = map[m].ys - 1;

	if ((type&BL_MOB))
		for (by = y0 / BLOCK_SIZE; by <= y1 / BLOCK_SIZE; by++)
			for (bx = x0 / BLOCK_SIZE; bx <= x1 / BLOCK_SIZE; bx++)
				for (bl = map[m].block_mob[bx + by * map[m].bxs]; bl && blockcount < BL_LIST_MAX; bl = bl->next) {
					MOB* mob = (MOB*)bl;
					
					if (bl->x >= x0 && bl->x <= x1 && bl->y >= y0 && bl->y <= y1 && blockcount < BL_LIST_MAX)
						bl_list[blockcount++] = bl;
				}
						
	if (blockcount >= BL_LIST_MAX)
		printf("map_foreachinarea: block count too many!\n");


	for (i = 0; i < blockcount; i++)
		if (bl_list[i]->prev)	// 有?かどうかチェック
			returnCount += func(bl_list[i], ap);

	return returnCount;
}

int map_loadregistry(int id) {
	int i;
	SqlStmt* stmt;
	struct global_reg reg;

	memset(&reg,0,sizeof(reg));

	map[id].registry_num=0;
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if( SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `MrgIdentifier`, `MrgValue` FROM `MapRegistry` WHERE `MrgMapId` = '%d'", id)
	||	SQL_ERROR == SqlStmt_Execute(stmt)
	|| 	SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_STRING, &reg.str, sizeof(reg.str),NULL,NULL)
	|| 	SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_INT, &reg.val, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	map[id].registry_num=SqlStmt_NumRows(stmt);

	for(i=0;i<map[id].registry_num && SQL_SUCCESS == SqlStmt_NextRow(stmt);i++)
		memcpy(&map[id].registry[i],&reg,sizeof(reg));

	SqlStmt_Free(stmt);

	return 0;
}
int map_lastdeath_mob(MOB* p) {

	if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Spawns` SET `SpnLastDeath`='%u' WHERE `SpnX`='%u' AND `SpnY`='%u' AND `SpnMapId`='%u' AND `SpnId`='%u'",p->last_death,p->startx,p->starty,p->bl.m,p->id))
	{
		Sql_ShowDebug(sql_handle);
	}

	return 0;
}

int map_registrysave(int m, int i) {
	struct global_reg* p=&map[m].registry[i];
	long long save_id=-1;
	SqlStmt* stmt;
	unsigned int reg_id;

	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	if( SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `MrgPosition` FROM `MapRegistry` WHERE `MrgMapId` = '%d' AND `MrgIdentifier`='%s'", m, p->str)
	||	SQL_ERROR == SqlStmt_Execute(stmt)
	|| 	SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &reg_id, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	if(SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		save_id = reg_id;
	}
	SqlStmt_Free(stmt);

	if(save_id != -1) {
		if(p->val == 0) {
			if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `MapRegistry` WHERE `MrgMapId` = '%u' AND `MrgIdentifier` = '%s'", m, p->str)) {
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		} else {
			if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `MapRegistry` SET `MrgIdentifier` = '%s', `MrgValue` = '%d' WHERE `MrgMapId` = '%u' AND `MrgPosition`='%d'", p->str, p->val, m, save_id)) {
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		}
	} else {
		if(p->val > 0) {
			if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `MapRegistry` (`MrgMapId`, `MrgIdentifier`, `MrgValue`, `MrgPosition`) VALUES ('%d', '%s', '%d', '%u')", m, p->str, p->val, i)) {
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		}
	}



	return 0;
}
/*int map_registrydelete(int m, int i) {
	struct global_reg* p=&map[m].registry[i];

	if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `mapreg` WHERE `map_id` = '%u' AND `name`='%s'",m,p->str))
		Sql_ShowDebug(sql_handle);

	return 0;
}*/

//map data (title,id,and sound) is readed from configuration file
int map_read() {//int id, const char *title, char bgm, int pvp, int spell, unsigned short light, unsigned short weather, unsigned int sweeptime, unsigned char cantalk, unsigned char showghosts, unsigned char region, unsigned char indoor, unsigned char warpout, const char *map_file) {
	unsigned short buff;
	unsigned int pos = 0;
	unsigned int i, id, sweeptime;
	unsigned short bgm;
	unsigned char pvp, spell, light, weather, cantalk, show_ghosts, region, indoor, warpout, bind;
	char title[64], mapfile[1024], mappath[1024];
	SqlStmt *stmt = SqlStmt_Malloc(sql_handle);
	FILE *fp;
	//struct map_src_list *i = NULL;
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `MapId`, `MapName`, `MapBGM`, `MapPvP`, `MapSpells`, `MapLight`, `MapWeather`, `MapSweepTime`, `MapChat`, `MapGhosts`, `MapRegion`, `MapIndoor`, `MapWarpout`, `MapBind`, `MapFile` FROM `Maps`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_STRING, &title, sizeof(title), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_USHORT, &bgm, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_UCHAR, &pvp, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_UCHAR, &spell, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_UCHAR, &light, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_UCHAR, &weather, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 7, SQLDT_UINT, &sweeptime, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 8, SQLDT_UCHAR, &cantalk, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 9, SQLDT_UCHAR, &show_ghosts, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 10, SQLDT_UCHAR, &region, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 11, SQLDT_UCHAR, &indoor, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 12, SQLDT_UCHAR, &warpout, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 13, SQLDT_UCHAR, &bind, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 14, SQLDT_STRING, &mapfile, sizeof(mapfile), NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	map_n = SqlStmt_NumRows(stmt);
	CALLOC(map, struct map_data, 65535);
		
	for (i = 0; i < map_n && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++) {
		sprintf(mappath, "../mithiamaps/Accepted/%s", mapfile);
		fp = fopen(mappath, "rb");
		
		if(fp == NULL) {
			printf("MAP_ERR: Map file not found (%s).\n", mappath);
			return -1;
		}
		
		memcpy(map[id].title, title, sizeof(title));
		map[id].bgm = bgm;
		map[id].pvp = pvp;
		map[id].spell = spell;
		map[id].light = light;
		map[id].weather = weather;
		map[id].sweeptime = sweeptime;
		map[id].cantalk = cantalk;
		map[id].show_ghosts = show_ghosts;
		map[id].region = region;
		map[id].indoor = indoor;
		map[id].warpout = warpout;
		map[id].bind = bind;
		fread(&buff, 2, 1, fp);
		map[id].xs = SWAP16(buff);
		fread(&buff, 2, 1, fp);
		map[id].ys = SWAP16(buff);
		CALLOC(map[id].tile, unsigned short, map[id].xs*map[id].ys);
		CALLOC(map[id].obj, unsigned short, map[id].xs*map[id].ys);
		CALLOC(map[id].map, unsigned char, map[id].xs*map[id].ys);
		CALLOC(map[id].pass, unsigned short, map[id].xs*map[id].ys);


		map[id].bxs = (map[id].xs+BLOCK_SIZE-1)/BLOCK_SIZE;
		map[id].bys = (map[id].ys+BLOCK_SIZE-1)/BLOCK_SIZE;
		CALLOC(map[id].warp,struct warp_list*,map[id].bxs*map[id].bys);
		CALLOC(map[id].block, struct block_list*, map[id].bxs*map[id].bys);
		//CALLOC(map[id].block_npc, struct block_list*, map[id].bxs*map[id].bys);
		//CALLOC(map[id].block_count, int, map[id].bxs*map[id].bys);
		//CALLOC(map[id].block_npc_count, int, map[id].bxs*map[id].bys);
		CALLOC(map[id].block_mob, struct block_list*,map[id].bxs*map[id].bys);
		//CALLOC(map[id].item_sweep,FLOORITEM*,10000);
		//CALLOC(map[id].block_mob_count, int, map[id].bxs*map[id].bys);
		CALLOC(map[id].registry,struct global_reg,1000);
		//map[id].block_mob_count=0;
		//map[id].item_sweep_count=0;
		//map[id].max_sweep_count=10000;
		while(!feof(fp)) {
			fread(&buff, 2, 1, fp);
			map[id].tile[pos] = SWAP16(buff);
			fread(&buff, 2, 1, fp);
			map[id].pass[pos] = SWAP16(buff);
			fread(&buff,2,1,fp);
			map[id].obj[pos] = SWAP16(buff);
			//map[id].pass[pos]=0;
			//all map section is walkable
			//map[id].map[pos] = 0;
			pos++;
			if (pos>=map[id].xs*map[id].ys)
				break;
		}
		pos = 0;
		fclose(fp);
		map_loadregistry(id);
	}
	SqlStmt_Free(stmt);
	printf("Map data file reading finished. %d map loaded!\n", map_n);
	//timer_insert(1800000,1800000,map_saveregistry, id, 0);
	//map_n++;
	return 0;
}

int map_reload() {
	USER* tmpsd = NULL;
	unsigned short buff;
	unsigned int pos = 0;
	unsigned int i, x, id, sweeptime, blockcount;
	unsigned short bgm;
	unsigned char pvp, spell, light, weather, cantalk, show_ghosts, region, indoor, warpout, bind;
	char title[64], mapfile[1024], mappath[1024];
	SqlStmt *stmt = SqlStmt_Malloc(sql_handle);
	FILE *fp;
	//struct map_src_list *i = NULL;
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `MapId`, `MapName`, `MapBGM`, `MapPvP`, `MapSpells`, `MapLight`, `MapWeather`, `MapSweepTime`, `MapChat`, `MapGhosts`, `MapRegion`, `MapIndoor`, `MapWarpout`, `MapBind`, `MapFile` FROM `Maps`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_STRING, &title, sizeof(title), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_USHORT, &bgm, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_UCHAR, &pvp, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_UCHAR, &spell, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_UCHAR, &light, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_UCHAR, &weather, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 7, SQLDT_UINT, &sweeptime, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 8, SQLDT_UCHAR, &cantalk, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 9, SQLDT_UCHAR, &show_ghosts, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 10, SQLDT_UCHAR, &region, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 11, SQLDT_UCHAR, &indoor, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 12, SQLDT_UCHAR, &warpout, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 13, SQLDT_UCHAR, &bind, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 14, SQLDT_STRING, &mapfile, sizeof(mapfile), NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	map_n = SqlStmt_NumRows(stmt);
		
	for (i = 0; i < map_n && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++) {
		sprintf(mappath, "../mithiamaps/Accepted/%s", mapfile);
		fp = fopen(mappath, "rb");
		blockcount = map[id].bxs * map[id].bys;
		
		if(fp == NULL) {
			printf("MAP_ERR: Map file not found (%s).\n", mappath);
			return -1;
		}

		memcpy(map[id].title, title, sizeof(title));
		if (map[id].spell != spell) map[id].spell = spell;
		if (map[id].light != light) map[id].light = light;
		if (map[id].weather != weather) map[id].weather = weather;
		if (map[id].sweeptime != sweeptime) map[id].sweeptime = sweeptime;
		if (map[id].cantalk != cantalk) map[id].cantalk = cantalk;
		if (map[id].show_ghosts != show_ghosts) map[id].show_ghosts = show_ghosts;
		if (map[id].region != region) map[id].region = region;
		if (map[id].indoor != indoor) map[id].indoor = indoor;
		if (map[id].warpout != warpout) map[id].warpout = warpout;
		if (map[id].bind != bind) map[id].bind = bind;
		
		fread(&buff, 2, 1, fp);
		map[id].xs = SWAP16(buff);
		fread(&buff, 2, 1, fp);
		map[id].ys = SWAP16(buff);
		
		if (map_isloaded(id)) {
			REALLOC(map[id].tile, unsigned short, map[id].xs*map[id].ys);
			REALLOC(map[id].obj, unsigned short, map[id].xs*map[id].ys);
			REALLOC(map[id].map, unsigned char, map[id].xs*map[id].ys);
			REALLOC(map[id].pass, unsigned short, map[id].xs*map[id].ys);
		} else {
			CALLOC(map[id].tile, unsigned short, map[id].xs*map[id].ys);
			CALLOC(map[id].obj, unsigned short, map[id].xs*map[id].ys);
			CALLOC(map[id].map, unsigned char, map[id].xs*map[id].ys);
			CALLOC(map[id].pass, unsigned short, map[id].xs*map[id].ys);
		}

		map[id].bxs = (map[id].xs+BLOCK_SIZE-1)/BLOCK_SIZE;
		map[id].bys = (map[id].ys+BLOCK_SIZE-1)/BLOCK_SIZE;

		if (map_isloaded(id)) {
			FREE(map[id].warp);
			CALLOC(map[id].warp, struct warp_list*, map[id].bxs * map[id].bys);
			REALLOC(map[id].block, struct block_list*, map[id].bxs * map[id].bys);
			REALLOC(map[id].block_mob, struct block_list*, map[id].bxs * map[id].bys);
		
			if (map[id].bxs * map[id].bys > blockcount) {
				for (x = blockcount; x < map[id].bxs * map[id].bys; x++) {
					map[id].block[x] = NULL;
					map[id].block_mob[x] = NULL;
				}
			}
		} else {
			CALLOC(map[id].warp, struct warp_list*, map[id].bxs * map[id].bys);
			CALLOC(map[id].block, struct block_list*, map[id].bxs * map[id].bys);
			CALLOC(map[id].block_mob, struct block_list*, map[id].bxs * map[id].bys);
			CALLOC(map[id].registry, struct global_reg, 1000);
		}
		
		while(!feof(fp)) {
			fread(&buff, 2, 1, fp);
			map[id].tile[pos] = SWAP16(buff);
			fread(&buff, 2, 1, fp);
			map[id].pass[pos] = SWAP16(buff);
			fread(&buff,2,1,fp);
			map[id].obj[pos] = SWAP16(buff);
			//map[id].pass[pos]=0;
			//all map section is walkable
			//map[id].map[pos] = 0;
			pos++;
			if (pos>=map[id].xs*map[id].ys)
				break;
		}
		
		pos = 0;
		fclose(fp);
		map_loadregistry(id);
		map_foreachinarea(sl_updatepeople,id,0,0,SAMEMAP,BL_PC);
	}
	
	SqlStmt_Free(stmt);
	printf("Map data file reading finished. %d map loaded!\n", map_n);
	//timer_insert(1800000,1800000,map_saveregistry, id, 0);
	//map_n++;
	return 0;
}

int map_setmapip(int id, unsigned int ip, unsigned short port) {
	if(id > MAX_MAP_PER_SERVER)
		return 1;

	map[id].ip = ip;
	map[id].port = port;
	return 0;
}

int lang_read(const char *cfg_file) {
	char line[1024], r1[1024], r2[1024];
	int line_num =0,i;
	FILE *fp;
	struct {
		char *name;
		int number;
	} map_msg_db[] = {
		{"MAP_WHISPFAIL", MAP_WHISPFAIL},
		{"MAP_ERRGHOST", MAP_ERRGHOST},
		{"MAP_ERRITMPATH",MAP_ERRITMPATH},
		{"MAP_ERRITMLEVEL", MAP_ERRITMLEVEL},
		{"MAP_ERRITMMIGHT", MAP_ERRITMMIGHT},
		{"MAP_ERRITMGRACE", MAP_ERRITMGRACE},
		{"MAP_ERRITMWILL", MAP_ERRITMWILL},
		{"MAP_ERRITMSEX", MAP_ERRITMSEX},
		{"MAP_ERRITMFULL", MAP_ERRITMFULL},
		{"MAP_ERRITMUNIQUE", MAP_ERRITMUNIQUE},
		{"MAP_ERRVITA",MAP_ERRVITA},
		{"MAP_ERRMANA",MAP_ERRMANA},
		{"MAP_EQHELM", MAP_EQHELM},
		{"MAP_EQWEAP", MAP_EQWEAP},
		{"MAP_EQARMOR", MAP_EQARMOR},
		{"MAP_EQSHIELD", MAP_EQSHIELD},
		{"MAP_EQLEFT", MAP_EQLEFT},
		{"MAP_EQRIGHT", MAP_EQRIGHT},
		{"MAP_EQSUBLEFT",MAP_EQSUBLEFT},
		{"MAP_EQSUBRIGHT",MAP_EQSUBRIGHT},
		{"MAP_EQFACEACC",MAP_EQFACEACC},
		{"MAP_EQCROWN",MAP_EQCROWN},
		{"MAP_EQMANTLE",MAP_EQMANTLE},
		{"MAP_EQNECKLACE",MAP_EQNECKLACE},
		{"MAP_EQBOOTS",MAP_EQBOOTS},
		{"MAP_EQCOAT",MAP_EQCOAT},
		{NULL, 0}
	};

	fp = fopen(cfg_file, "r");
	if (fp == NULL) {
		printf("CFG_ERR: Language file (%s) not found.\n", cfg_file);
		return 1;
	}

	while(fgets(line, sizeof(line), fp)) {
		line_num++;
		if (line[0] == '/' && line[1] == '/')
			continue;

		if (sscanf(line, "%[^:]: %[^\r\n]", r1, r2) == 2) {
			for(i=0;map_msg_db[i].name;i++) {
				if (strcmpi(map_msg_db[i].name, r1)==0) {
					strncpy(map_msg[map_msg_db[i].number].message, r2, 256);
					map_msg[map_msg_db[i].number].message[255] = '\0';
					map_msg[map_msg_db[i].number].len = strlen(r2);
					if (map_msg[map_msg_db[i].number].len > 255)
						map_msg[map_msg_db[i].number].len=255;
					break;
				}
			}
		}
	}
	fclose(fp);
	printf("Language File (%s) reading finished!\n", cfg_file);
	return 0;
}
unsigned int return_ip_n(char* addr) {
	struct hostent *he;
	struct in_addr a;
	he =gethostbyname(addr);
	if(he)
	{
		while(*he->h_addr_list)
		{
			bcopy(*he->h_addr_list++, (char*) &a, sizeof(a));
		}
	}

	return a.s_addr;
}

int config_read(const char *cfg_file) {
	char line[1024], r1[1024], r2[1024];
	int line_num = 0;
	FILE *fp;

	fp = fopen(cfg_file, "r");
	if (fp == NULL) {
		printf("CFG_ERR: Configuration file (%s) not found.\n", cfg_file);
		return 1;
	}

	while(fgets(line, sizeof(line), fp)) {
		line_num++;
		if (line[0] == '/' && line[1] == '/')
			continue;

		if (sscanf(line, "%[^:]: %[^\r\n]", r1, r2) == 2) {
			//MAP
			if(strcmpi(r1, "map_ip") == 0) {
				get_actual_ip(r2,map_ip_s);
				myip=return_ip_n(r2);
				map_ip_s[15] = '\0';
				map_ip = inet_addr(map_ip_s);

			} else if(strcmpi(r1, "map_port") == 0) {
				map_port = atoi(r2);
			} else if(strcmpi(r1,"loginip")==0) {
				get_actual_ip2(r2);
				log_ip_s[15]='\0';
				log_ip=inet_addr(log_ip_s);
			} else if(strcmpi(r1,"loginport")==0) {
				log_port=atoi(r2);
			//CHAR
			} else if(strcmpi(r1, "char_ip") == 0) {
				strncpy(char_ip_s, r2, 16);
				char_ip_s[15] = '\0';
				char_ip = inet_addr(char_ip_s);
			} else if(strcmpi(r1, "char_port") == 0) {
				char_port = atoi(r2);
			} else if(strcmpi(r1, "char_id") == 0) {
				strncpy(char_id, r2, 32);
				char_id[31] = '\0';
			} else if(strcmpi(r1, "char_pw") == 0) {
				strncpy(char_pw, r2, 32);
				char_pw[31] = '\0';
			//SAVE TIME
			} else if(strcmpi(r1, "save_time") == 0) {
				save_time = atoi(r2)*1000;
			//LOG & DUMP
			} else if(strcmpi(r1, "map_log") == 0) {
				set_logfile(r2);
			} else if(strcmpi(r1, "dump_log") == 0) {
				set_dmpfile(r2);
			} else if(strcmpi(r1, "dump_save") == 0) {
				dump_save = atoi(r2);
			//MAP & NPC SCRIPT
			//} else if(strcmpi(r1, "map") == 0) {
			//	if (map_src_add(r2)) {
			//		printf("CFG_ERR: Map Script Configuration parse error!\n");
			//		printf(" line %d: %s\n", line_num, line);
			//	}
			} else if(strcmpi(r1, "town") == 0) {
				if( map_town_add(r2)) {
					printf("CFG_ERR: Town Name Parse error!\n");
					printf(" line %d: %s\n", line_num, line);
				}
			} else if(strcmpi(r1, "npc") == 0) {
				npc_src_add(r2);
			} else if(strcmpi(r1, "warp") == 0) {
				npc_warp_add(r2);
            } else if(strcmpi(r1, "import") == 0) {
                script_config_read_sub(r2);
			} else if(strcmpi(r1, "xprate") == 0) {
				xp_rate=atoi(r2);
			} else if(strcmpi(r1,"droprate") == 0) {
				d_rate=atoi(r2);
			} else if(strcmpi(r1, "sql_ip") == 0) {
				strcpy(sql_ip,r2);
			} else if(strcmpi(r1, "sql_port") == 0) {
				sql_port=atoi(r2);
			} else if(strcmpi(r1, "sql_id") == 0) {
				strcpy(sql_id,r2);
			} else if(strcmpi(r1, "sql_pw") == 0) {
				strcpy(sql_pw,r2);
			} else if(strcmpi(r1, "sql_db") == 0) {
				strcpy(sql_db,r2);
			}
		}
	}
	fclose(fp);
	printf("Configuration File (%s) reading finished!\n", cfg_file);
	return 0;
}

void do_term(void) {
	int i;
	map_savechars(0, 0);
	map_clritem();
	map_termiddb();
	for(i=0;i<MAX_MAP_PER_SERVER;i++){
		FREE(map[i].tile);
		FREE(map[i].obj);
		FREE(map[i].map);
		FREE(map[i].block);
		//FREE(map[i].block_npc);
		//FREE(map[i].block_count);
		//FREE(map[i].block_npc_count);
		FREE(map[i].block_mob);
		//FREE(map[i].block_mob_count);
		FREE(map[i].warp);
	}
	map_termblock();
	itemdb_term();
	magicdb_term();
	classdb_term();
	//boarddb_term();
	//mobdb_term();
	//sql_close();

	printf("Mithia Map Server Shutdown.\n");
	add_log("Shutdown.\n");
}

void help_screen() {
	printf("HELP LIST\n");
	printf("---------\n");
	printf(" --conf [FILENAME]  : set config file\n");
	printf(" --inter [FILENAME] : set inter file\n");
	printf(" --lang [FILENAME]  : set lang file\n");
	exit(0);
}
int change_time_char(int none,int none2) {
	int i;
	USER *sd;
	cur_time++;

	if(cur_time==24) {
		cur_time=0;
		cur_day++;
		if (cur_day == 92) {
			cur_day = 1;
			cur_season++;
		
			if(cur_season==5) {
				cur_season=1;
				cur_year++;
			}
		}
	}

	for(i=0;i<fd_max;i++) {
		if(session[i] && (sd=session[i]->session_data)) {
			clif_sendtime(sd);
		}
	}

	if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Time` SET `TimHour` ='%d', `TimDay`='%d', `TimSeason`='%d', `TimYear`='%d'",cur_time,cur_day,cur_season,cur_year))
		Sql_ShowDebug(sql_handle);

	return 0;
}
int get_time_thing(void) {
	SqlStmt* stmt;
	int time,day,season,year;
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `TimHour`, `TimDay`, `TimSeason`, `TimYear` FROM `Time`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &time, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_INT, &day, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_INT, &season, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_INT, &year, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	if(SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		old_time=time;
		cur_time=time;
		cur_day=day;
		cur_season=season;
		cur_year=year;
	}

	SqlStmt_Free(stmt);

	return 0;
}
int uptime(void) {
	if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `UpTime` WHERE `UtmId` = '3'"))
		Sql_ShowDebug(sql_handle);

	if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `UpTime`(`UtmId`, `UtmValue`) VALUES('3', '%d')",gettickthing()))
		Sql_ShowDebug(sql_handle);


	return 0;
}
int keep_char_alive(int n,int a) {

	if(char_fd) {
		WFIFOHEAD(char_fd,3);
		WFIFOB(char_fd,0)=0xFF;
		WFIFOB(char_fd,1)=1;
		WFIFOSET(char_fd,2);
	}
	return 0;
}
int object_flag_init(void) {
	 int num;
    char nothing[8];
    short tile;
    char flag, count;
    int z=1;
	FILE *fi = fopen("SObj.tbl", "rb");
	//FILE *fo = fopen("C:\\sobjflags.tbl", "wb");
    fread(&num, 4, 1, fi);
	CALLOC(objectFlags,unsigned char,num+1);
    fread(&flag, 1, 1, fi);

    while(!feof(fi))
    {
        fread(&count, 1, 1, fi);
        for(; count != 0; count--)
        {
            fread(&tile, 2, 1, fi);
        }
        fread(&nothing, 5, 1, fi);
        fread(&flag, 1, 1, fi);
		objectFlags[z]=flag;
        //fwrite(&flag, 1, 1, fo);
		z++;
		}
		return 0;

}
int do_init(int argc, char **argv) {
	#ifdef LOGGING_ENABLED
		LOG_START(log_fd);
	#endif

	int i;
	char *CONF_FILE = "conf/map.conf";
	char *LANG_FILE = "conf/lang.conf";
	char *INTER_FILE = "conf/inter.conf";
	char *CHAR_FILE = "conf/char.conf";
	srand(gettick());
	set_logfile("log/map.log");
	set_dmpfile("log/map_dump.log");

	for (i = 1; i < argc ; i++) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "--h") == 0 || strcmp(argv[i], "--?") == 0 || strcmp(argv[i], "/?") == 0)
			help_screen();
		else if (strcmp(argv[i], "--conf") == 0)
			CONF_FILE=argv[i+1];
		else if (strcmp(argv[i],"--inter") == 0)
			INTER_FILE = argv[i+1];
		else if (strcmp(argv[i],"--lang") == 0)
			LANG_FILE = argv[i+1];
		//else if(strcmp(argv[i],"--log") == 0)
			//LOGGING_ENABLED=1;
	}

	config_read(CONF_FILE);
	config_read(INTER_FILE);
	config_read(CHAR_FILE);
	lang_read(LANG_FILE);
	set_termfunc(do_term);
	//CALLOC(userlist,struct userlist_data,1);
	//gcFixPrematureFrees();
	set_default_input(command_input);
	add_log("");
	add_log("Mithia Map Server Started.\n");
	sql_handle=Sql_Malloc();
	if(sql_handle == NULL)
	{
		Sql_ShowDebug(sql_handle);
		exit(EXIT_FAILURE);
	}
	if( SQL_ERROR == Sql_Connect(sql_handle, sql_id, sql_pw, sql_ip, (uint16)sql_port, sql_db) )
	{
		Sql_ShowDebug(sql_handle);
		Sql_Free(sql_handle);
		exit(EXIT_FAILURE);
	}
	
	if (SQL_ERROR == Sql_Query(sql_handle, "UPDATE `Character` SET `ChaOnline` = 0 WHERE `ChaOnline` = 1")) {
		Sql_ShowDebug(sql_handle);
	}

	//sql_init();
	uptime();
	guidedb_init();
	map_read();
	map_initblock();
	map_initiddb();
	//script_init();
	npc_init();
	warp_init();
	printf("Loaded warps!\n");
	itemdb_init();
	recipedb_init();
	mobdb_init();
	magicdb_init();
    classdb_init();
	clandb_init();
	boarddb_init();
	intif_init();
	createdb_init();
	object_flag_init();
	sl_init();
	map_loadgameregistry();
	//set_defaultaccept(clif_accept);
	set_defaultparse(clif_parse);
	set_defaulttimeout(clif_timeout);
	map_fd = make_listen_port(map_port);
	cur_time=12;
	cur_day=0;
	cur_year=1;
	get_time_thing();
	authdb_init();
	timer_insert(450000,450000,change_time_char,i,i);

	timer_insert(1000, 1000, check_connect_char, char_ip, char_port);
	
	cronjobtimer = timer_insert(1000, 1000, map_cronjob, 0, 0);
	timer_insert(100,100,npc_runtimers,0,0);
	//timer_insert(250,250,mob_timer_new,0,0);
	timer_insert(50,50,mob_timer_spawns,0,0);
	timer_insert(30000,30000,map_weather,0,0);
	timer_insert(300000, 300000, map_savechars, 0, 0);

	for (i = 0; i < MAX_GROUPS; i++) {
		memset(groups[i], 0, sizeof(unsigned int) * MAX_GROUP_MEMBERS);
	}
	printf("Mithia Map Server is \033[1;32mready\033[0m! Listening at %d.\n", map_port);

	add_log("Server Ready! Listening at %d.\n", map_port);
	#ifdef LOGGING_ENABLED
		LOG_STOP(log_fd);
	#endif
	return 0;
}


int map_canmove(int m, int x, int y) {
	//struct block_list *bl;
	//int c;
	int obj;
	int bx,by;
	int pass;
	obj=read_obj(m,x,y);
	pass=read_pass(m,x,y);
	//bx=x/BLOCK_SIZE;
	//by=y/BLOCK_SIZE;

	//bl=map[m].block[x+y*map[m].bxs];
	//c=map[m].block_count[bx+by*map[m].bxs];


	//if(obj)	return 1;

	if(pass){
	    USER *sd = map_id2sd(pass);
		if(!sd || !(sd->uFlags&uFlag_unphysical))
		return 1;
	}

	return 0;
}



int boards_delete(USER *sd, int board) {


	if(!sd) return 0;

	int post=SWAP16(RFIFOW(sd->fd,8));

	//Board(0) == NMail
	WFIFOHEAD(char_fd,28);
	WFIFOW(char_fd,0)=0x3008;
	WFIFOW(char_fd,2)=sd->fd; //FD Descriptor to send to when sending back.
	WFIFOW(char_fd,4)=sd->status.gm_level; //GM level
	WFIFOW(char_fd,6)=sd->board_candel; //Has ability to DELETE from here.
	WFIFOW(char_fd,8)=board; //Board deleting from(0 for NMail)
	WFIFOW(char_fd,10)=post;
	memcpy(WFIFOP(char_fd,12),sd->status.name,16);
	WFIFOSET(char_fd,28);




	return 0;
}

int boards_showposts(USER* sd, int board) {
	int len;
	struct board_posts *post = NULL;
	int num;
	int pos;
	int bcount=0;
	int x;
	char name_l[255];
	struct board_show_0 a;

	memset(&a,0,sizeof(struct board_show_0));

	if(!sd) return 0;
	
	sd->board_canwrite = 0;
	sd->board_candel = 0;

	if(board==0) {
		sd->board_canwrite = 1;
		sd->board_candel = 1;
	} else {
		sd->board=board;
		if (boarddb_script(board)) {
			sl_doscript_blargs(boarddb_yname(board), "check", 1, &sd->bl);
		} else {
			sd->board_canwrite = 1;
		}
		if(sd->status.gm_level==99) {
			sd->board_canwrite=1;
			sd->board_candel=1;
		}
	}

	if(sd->board_canwrite) a.flags |= BOARD_CAN_WRITE;
	if(sd->board_candel) a.flags |= BOARD_CAN_DEL;

	//Board(0) == NMail
	a.fd = sd->fd;
	a.bcount = sd->bcount;
	a.board = board;
	a.popup = sd->board_popup;
	memcpy(a.name, sd->status.name,16);

	///Need to add Level Check...Also must add "Hide Flag" for boards.
	WFIFOHEAD(char_fd,sizeof(struct board_show_0)+2);
	WFIFOW(char_fd,0)=0x3009; //NMail/Board Show
	memcpy(WFIFOP(char_fd,2),&a, sizeof(struct board_show_0));
	WFIFOSET(char_fd,sizeof(struct board_show_0)+2);
	return 0;


}

int boards_readpost(USER* sd, int board,int post) {
	struct boards_read_post_0 header;

	int len=0;
	int maxpost=0;
	int newpost=post;

	memset(&header,0,sizeof(header));

	if(board) {
		sd->board=board;
		if (boarddb_script(board)) {
			sl_doscript_blargs(boarddb_yname(board), "check", 1, &sd->bl);
		} else {
			sd->board_canwrite = 1;
		}
		if(sd->status.gm_level==99) {
			sd->board_canwrite=1;
			sd->board_candel=1;
		}
	}

		if(sd->board_canwrite) header.flags |= BOARD_CAN_WRITE;
		if(sd->board_candel) header.flags |= BOARD_CAN_DEL;

		header.board=board;
		header.fd = sd->fd;
		header.post = post;
		memcpy(header.name,sd->status.name,16);

		WFIFOHEAD(char_fd,sizeof(header)+2);
		WFIFOW(char_fd,0)=0x300A;
		memcpy(WFIFOP(char_fd,2),&header,sizeof(header));
		WFIFOSET(char_fd,sizeof(header)+2);

	return 0;
}

int boards_post(USER* sd, int board) {
	int nval=0;
	struct boards_post_0 header;
	//char post[4000]; //Max of Post is 4000
	//char topic[52];
	int topiclen,postlen;

	memset(&header,0,sizeof(header));

	StringBuf buf;



	if(!sd) return 0;

	//memset(topic,0,52);
	//memset(post,0,4000);
	header.fd=sd->fd;
	header.board=board;
	memcpy(header.name,sd->status.name,16);
	topiclen=RFIFOB(sd->fd,8);
	if(topiclen>52) {
		clif_Hacker(sd->status.name,"Board hacking: TOPIC HACK");
		return 0;
	}

	memcpy(header.topic,RFIFOP(sd->fd,9),topiclen);

	postlen=SWAP16(RFIFOW(sd->fd,topiclen+9));

	if(postlen>4000) {
		clif_Hacker(sd->status.name,"Board hacking: POST(BODY) HACK");

		return 0;
	}

	memcpy(header.post,RFIFOP(sd->fd,topiclen+11),postlen);

	if(!topiclen) {
		nmail_sendmessage(sd,"Post must contain subject.",6,0);
		return 0;
	}

	if(!postlen) {
		nmail_sendmessage(sd,"Post must contain a body.",6,0);
		return 0;
	}

	///Get Boardname
	StringBuf_Init(&buf);
	StringBuf_Printf(&buf,"boardname%d",board);
	header.nval=pc_readglobalreg(sd,StringBuf_Value(&buf));
	StringBuf_Destroy(&buf);
	///End Get Boardname

	if(sd->status.gm_level) header.nval=1;

	WFIFOHEAD(char_fd,sizeof(header)+2); // 4000(Body) + 52(topic) + 2(sfd) + 2(ID) + 2(board) + 1(null-terminator)*2
	WFIFOW(char_fd,0)=0x300C; //Boards POST
	memcpy(WFIFOP(char_fd,2),&header,sizeof(header));
	WFIFOSET(char_fd,sizeof(header)+2);
	return 0;



}

/* N-Mail shit */


int nmail_read(USER* sd, int post) {
	int len;
	//int hasm=sd->flags&;
	/*sql_request("SELECT fromuser,topic,post,month,day FROM nmail WHERE touser='%s' AND mail_id=%d",sd->status.name,post);

	if(sql_get_row()) {
		return -1;
	} else {

		WFIFOB(sd->fd,0)=0xAA;
		WFIFOB(sd->fd,3)=0x31;
		WFIFOB(sd->fd,4)=0x03;
		WFIFOB(sd->fd,5)=0x05;
		WFIFOB(sd->fd,6)=0x03;
		WFIFOB(sd->fd,7)=0x01;
		WFIFOW(sd->fd,8)=SWAP16(post);

		len=strlen(sql_get_str(0));
		WFIFOB(sd->fd,10)=len;
		len++;
		strcpy(WFIFOP(sd->fd,11),sql_get_str(0));
		WFIFOB(sd->fd,len+10)=sql_get_int(3);
		WFIFOB(sd->fd,len+11)=sql_get_int(4);
		len+=2;
		WFIFOB(sd->fd,len+10)=strlen(sql_get_str(1));
		strcpy(WFIFOP(sd->fd,len+11),sql_get_str(1));
		len+=strlen(sql_get_str(1))+1;
		WFIFOW(sd->fd,len+10)=SWAP16(strlen(sql_get_str(2)));
		strcpy(WFIFOP(sd->fd,len+11),sql_get_str(2));
		len+=strlen(sql_get_str(2))+1;
		WFIFOW(sd->fd,1)=SWAP16(len+7);
		crypt(WFIFOP(sd->fd,0));
		WFIFOSET(sd->fd,len+10);
		sql_free_row();
		sql_request("UPDATE nmail SET new=0 WHERE touser='%s' AND mail_id=%d",sd->status.name,post);
		sql_free_row();
	}


	sql_request("SELECT * FROM nmail WHERE new=1 AND touser='%s'",sd->status.name);
	if(sql_get_row()) {
		sd->hasmail=0;

	} else {
		sd->hasmail=1;
	}
	if(hasm!=sd->hasmail) {
			clif_sendstatus2(sd);
	}
	sql_free_row();*/
	return 0;
}

int nmail_luascript(USER* sd, int to, int topic, int msg) {
	char message[4000];
	char escape[4000];
	
	memset(message,0,4000);
	memcpy(message,RFIFOP(sd->fd,to+topic+12),msg);
	
	Sql_EscapeString(sql_handle, escape, message);
	
	if (SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `Mail` (`MalChaName`, `MalChaNameDestination`, `MalBody`) VALUES ('%d', 'Lua', '%s')", sd->status.id, escape)) {
		Sql_ShowDebug(sql_handle);
		return 0;
	}
	
	sl_exec(sd, message);
	return 0;
}
int nmail_write(USER* sd) {
	USER* tsd = NULL;
	char to_user[52];
	char topic[52];
	char message[4000];
	int topiclen,messagelen,tolen;


	int newid=0;
	nullpo_ret(0,sd);

	memset(topic,0,52);
	memset(message,0,4000);
	memset(to_user,0,52);

	tolen=RFIFOB(sd->fd,8);
	if(tolen>52) {
		clif_Hacker(sd->status.name,"NMAIL: To User");
		return 0;
	}
	topiclen=RFIFOB(sd->fd,tolen+9);
	if(topiclen>52) {
		clif_Hacker(sd->status.name,"NMAIL: Topic");
		return 0;
	}
	messagelen=SWAP16(RFIFOW(sd->fd,tolen+topiclen+10));
	if(messagelen>4000) {
		clif_Hacker(sd->status.name,"NMAIL: Message");
		return 0;
	}

	memcpy(to_user,RFIFOP(sd->fd,9),tolen);
	memcpy(topic,RFIFOP(sd->fd,tolen+10),topiclen);
	memcpy(message,RFIFOP(sd->fd,topiclen+tolen+12),messagelen);

	if(strcmpi(to_user,"lua")==0) {
		strcpy(sd->mail, message);
		sd->luaexec = 0;
		sl_doscript_blargs("canRunLuaMail", NULL, 1, &sd->bl);
		
		if(sd->status.gm_level==99 || sd->luaexec) { //it's a gm so do lua thing
			
			nmail_luascript(sd,tolen,topiclen,messagelen);
			nmail_sendmessage(sd,"LUA script ran!",6,1);
			return 0;
		}
	}

	if(!topiclen) {
		nmail_sendmessage(sd,"Mail must contain a subject.",6,0);
		return 0;
	}

	if(!messagelen) {
		nmail_sendmessage(sd,"Mail must contain a body.",6,0);
		return 0;
	}
	
	tsd = map_name2sd(to_user);

	/*if (tsd && clif_isignore(sd, tsd))*/ nmail_sendmail(sd, to_user, topic, message);
	return 0;
}

int nmail_sendmail(USER* sd, char* to_user, char* topic, char* message) {

	WFIFOHEAD(char_fd,4124); // 4000 + 52 + 52 + 16 + 2 +2
	WFIFOW(char_fd,0)=0x300D;
	WFIFOW(char_fd,2)=sd->fd;
	memcpy(WFIFOP(char_fd,4),sd->status.name,16);
	memcpy(WFIFOP(char_fd,20),to_user,52);
	memcpy(WFIFOP(char_fd,72),topic,52);
	memcpy(WFIFOP(char_fd,124),message,4000);

	WFIFOSET(char_fd,4124);


	return 0;
}



int get_actual_ip(char* addr) {
	struct hostent *he;
	struct in_addr a;

    he = gethostbyname (addr);
    if (he)
    {
        while (*he->h_addr_list)
        {
            bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));
			strncpy(map_ip_s,inet_ntoa(a),16);
            //printf("Listening on IP: %s\n", inet_ntoa(a));
        }
    }

    return 0;
}

int get_actual_ip2(char* addr) {
	struct hostent *he;
	struct in_addr a;

    he = gethostbyname (addr);
    if (he)
    {
        while (*he->h_addr_list)
        {
            bcopy(*he->h_addr_list++, (char *) &a, sizeof(a));
			strncpy(log_ip_s,inet_ntoa(a),16);
            printf("Login IP: %s\n", inet_ntoa(a));
        }
    }

    return 0;
}

int map_addmob(USER *sd, unsigned int id, int start, int end, unsigned int replace) {
	if(SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `Spawns` (`SpnMapId`, `SpnX`, `SpnY`, `SpnMobId`, `SpnLastDeath`, `SpnStartTime`, `SpnEndTime`, `SpnMobIdReplace`) VALUES(%d, %d, %d, %d, %d, %d, %d, %d)", sd->bl.m, sd->bl.x, sd->bl.y, id, 0, start, end, replace))
		Sql_ShowDebug(sql_handle);
	//sql_request("INSERT INTO spawndb VALUES(%d,%d,%d,%d,%d)",sd->bl.m,sd->bl.x,sd->bl.y,id,0);
	//sql_get_row();
	//sql_free_row();
	return 0;
}
int map_changepostcolor(int board, int post, int color) {
	if(SQL_ERROR == Sql_Query(sql_handle, "UPDATE `Boards` SET `BrdHighlighted` = '%d' WHERE `BrdBnmId` = '%d' AND `BrdPosition` = '%d'",color, board, post))
		Sql_ShowDebug(sql_handle);
	//sql_request("UPDATE boards SET color=%d WHERE board_id=%d AND post_id=%d",color,board,post);
	//sql_get_row();
	//sql_free_row();
	return 0;
}
int map_getpostcolor(int board, int post) {
	int x,color=0;
	SqlStmt* stmt;

	stmt = SqlStmt_Malloc(sql_handle);
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return -1;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `BrdHighlighted` FROM `Boards` WHERE `BrdBnmId` = '%d' AND `BrdPosition` = '%d'",board, post)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt,0,SQLDT_INT,&color,0,NULL,NULL)) {
		Sql_ShowDebug(sql_handle);
		return -1;
	}
	
	if (SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		x = color;
	}
	
	SqlStmt_Free(stmt);
	//sql_request("SELECT color FROM boards WHERE board_id=%d AND post_id=%d",board,post);
	//if(!sql_get_row()) {
	//	color=sql_get_int(0);
	//}
	//sql_free_row();

	return color;
}
char* map_id2name(unsigned int id) {
	unsigned char* buf;
	USER* sd;
	CALLOC(buf,unsigned char,255);
	memset(buf,0,255);
	sd=map_id2sd(id);
	if(!id) {
		strcpy(buf,"None");
		return buf;
	}
	if(sd) {
		strcpy(buf,sd->status.name);
		return buf;
	} else {
	//sql_request("SELECT name FROM hyul.character WHERE id=%d",id);
	}


	//if(sql_get_row()) {
		//strcpy(buf,"None");
	//} else {
		//strcpy(buf,sql_get_str(0));
	//}
	return buf;
}


void mmo_setonline(unsigned int id, int val) {
	int a, b, c, d, regid;
	char escape[255];
	USER* sd = map_id2sd(id);
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ChaActId` FROM `Character` WHERE `ChaId` = '%u'", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &regid, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	if (SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		a=b=c=d=session[sd->fd]->client_addr.sin_addr.s_addr;
		a &=0xff;
		b=(b>>8)&0xff;
		c=(c>>16)&0xff;
		d=(d>>24)&0xff;
		sprintf(escape, "%u.%u.%u.%u\0", a, b, c, d);
		sl_doscript_blargs("login", NULL, 1, &sd->bl);
	
		if (SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `LoginLogs` (`LgnChaId`, `LgnIp`, `LgnActId`) VALUES ('%u', '%s', '%u')",
				id, escape, regid)) {
			Sql_ShowDebug(sql_handle);
		}
	}
	
	if(SQL_ERROR == Sql_Query(sql_handle, "UPDATE `Character` SET `ChaOnline` = '%d' WHERE `ChaId` = '%u'", val, id))
		Sql_ShowDebug(sql_handle);
		
	SqlStmt_Free(stmt);
	//sql_request("UPDATE `character` SET `online`='%d' WHERE `id`='%u'", val, id);
}

char isActive(USER* sd) {
	if(!sd)
		return 0;

	if(!session[sd->fd])
		return 0;

	if(session[sd->fd]->eof)
		return 0;

	return 1;
}

int hasCoref(USER* sd) {

	USER* nsd=NULL;

	if(sd->coref) return 1;
	if(sd->coref_container) {
		nsd=map_id2sd(sd->coref_container);
		if(!nsd) return 0;
		return 1;
	}


	return 0;


}
int map_reset_timer(int v1,int v2) {

	static int reset;

	static int diff;
	int x;
	struct map_sessiondata* sd=NULL;
	char msg[255];
	if(!reset) reset=v1;

	reset-=v2;
	diff+=v2;
	if(reset<=250) {
		clif_broadcast("The gods are recasting the magic that sustains the land of Mithia.", -1);
	}
	if(reset<=0) {
		for(x=0;x<fd_max;x++) {
		if(session[x] && (sd=session[x]->session_data) && !session[x]->eof) {



					clif_handle_disconnect(sd);

	            	if(session[x]->func_parse){
		        	  session[x]->func_parse(x);
					}
	          	    RFIFOFLUSH(x);

					session[x]->eof=1;

			}
		}


         //Flush the stupid pipe for saving everyones stupid character

		server_shutdown=1;


		//EnableReset=1;
		//exit(0);

		//exit(0);
		return 1;
	}



	if(reset<=60000) { //Less than a minute remaining(gonna mass spell everyone)
		if(diff>=10000) {

			sprintf(msg,"Server resetting in %d seconds",reset/1000);
			//clif_broadcast("---------------------------------------------------",-1);
			clif_broadcast(msg,-1);
			//clif_broadcast("---------------------------------------------------",-1);
			diff=0;
		}
	} else if(reset <= 3600000) {
		if(diff>=300000) {
			sprintf(msg,"Server resetting in %d minutes",reset/60000);
			//clif_broadcast("---------------------------------------------------",-1);
			clif_broadcast(msg,-1);
			//clif_broadcast("---------------------------------------------------",-1);
			diff=0;
		}
	} else if (reset > 3600000) {
		if (diff >= 3600000) {
			sprintf(msg,"Server resetting in %d hours",reset/3600000);
			//clif_broadcast("---------------------------------------------------",-1);
			clif_broadcast(msg,-1);
			//clif_broadcast("---------------------------------------------------",-1);
			diff=0;
		}
	}


	return 0;
}
int map_setglobalreg(int m,char *reg, int val) {
	int i,exist;
	
	exist = -1;
	nullpo_ret(0,reg);
	if(!map_isloaded(m)) return 0;

	for(i=0;i<map[m].registry_num;i++) {
		if(strcmpi(map[m].registry[i].str,reg) == 0) {
			exist = i;
			break;
		}
	}
	//if registry exists, set value
	if(exist != -1) {
		if(val == 0) {
			map[m].registry[exist].val = val;
			map_registrysave(m, exist);
			strcpy(map[m].registry[exist].str,""); //empty registry
			return 0;
		} else {
			map[m].registry[exist].val = val;
			map_registrysave(m, exist);
			return 0;
		}
	} else {
		for(i=0;i<map[m].registry_num;i++) {
			if(strcmpi(map[m].registry[i].str,"") == 0) {
				strcpy(map[m].registry[i].str,reg);
				map[m].registry[i].val = val;
				map_registrysave(m, i);
				return 0;
			}
		}
		
		if (map[m].registry_num < 10000) {
			map[m].registry_num = map[m].registry_num + 1;
			i = map[m].registry_num - 1;
			strcpy(map[m].registry[i].str,reg);
			map[m].registry[i].val = val;
			map_registrysave(m, i);
			return 0;
		}
	}
				/*map_registrydelete(m,i);
				map[m].registry[i]=map[m].registry[map[m].registry_num-1];
				memset(&map[m].registry[map[m].registry_num-1], 0, sizeof(struct global_reg));
				map[m].registry_num--;

				break;
			}
		}
		return 0;
	}
	for(i=0;i<map[m].registry_num;i++){
		if(strcmp(map[m].registry[i].str,reg)==0){
			map[m].registry[i].val=val;
			map_registrysave(m,i);
			return 0;
		}
	}
	if(map[m].registry_num<10000){
		strcpy(map[m].registry[i].str,reg);
		map[m].registry[i].val=val;
		map[m].registry_num++;
		map_registrysave(m,i);
		return 0;
	}*/
	//printf("pc_setglobalreg : couldn't set %s\n", reg);

	return 0;
}

int map_readglobalreg(int m, char *reg) {
	int i,exist;

	exist = -1;
	if(!map_isloaded(m)) return 0;

	for(i=0;i<map[m].registry_num;i++) {
		if(strcmpi(map[m].registry[i].str,reg) == 0) {
			exist = i;
			break;
		}
	}
	if(exist != -1) {
		return map[m].registry[exist].val;
	} else {
		return 0;
	}
	return 0;
}
//Game registries
//loads game registries
int map_loadgameregistry() {
	SqlStmt* stmt;
	struct global_reg reg;
	int i;

	gamereg.registry_num = 0;
	CALLOC(gamereg.registry,struct global_reg,10000);
	memset(&reg, 0, sizeof(reg));

	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `GrgIdentifier`, `GrgValue` FROM `GameRegistry`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_STRING, &reg.str,sizeof(reg.str),NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_INT, &reg.val, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	gamereg.registry_num = SqlStmt_NumRows(stmt);
	
	for(i = 0; i < gamereg.registry_num && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++) {
		memcpy(&gamereg.registry[i], &reg, sizeof(reg));
	}

	printf("Loaded %d game registries\n", gamereg.registry_num);
	SqlStmt_Free(stmt);
	return 0;
}
//saves gameregistries
int map_savegameregistry(int i) {
	struct global_reg* p = &gamereg.registry[i];
	unsigned int save_id = 0;
	SqlStmt* stmt;
	unsigned int gameregid;
	
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `GrgId` FROM `GameRegistry` WHERE `GrgIdentifier` = '%s'", p->str)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &gameregid, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	if (SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		save_id = gameregid;
	}

	SqlStmt_Free(stmt);

	if(save_id) {
		if(p->val == 0) {
			if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `GameRegistry` WHERE `GrgIdentifier` = '%s'", p->str)) {
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		} else {
			if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `GameRegistry` SET `GrgIdentifier` = '%s', `GrgValue` = '%d' WHERE `GrgId` = '%d'", p->str, p->val, save_id)) {
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		}
	} else {
		if(p->val > 0) {
			if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `GameRegistry` (`GrgIdentifier`, `GrgValue`) VALUES ('%s', '%d')", p->str, p->val)) {
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		}
	}
	
	return 0;
}
//sets game registry
int map_setglobalgamereg(char *reg, int val) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, reg);

	//if registry exists, get number
	for(i = 0; i < gamereg.registry_num; i++) {
		if(!strcmpi(gamereg.registry[i].str, reg)) {
			exist = i;
			break;
		}
	}
	//if registry exists, set value
	if(exist != -1) {
		if(val == 0) {
			gamereg.registry[exist].val = val;
			map_savegameregistry(exist);
			strcpy(gamereg.registry[exist].str,""); //empty registry
			return 0;
		} else {
			gamereg.registry[exist].val = val;
			map_savegameregistry(exist);
			return 0;
		}
	} else {
		for(i = 0; i < gamereg.registry_num; i++) {
			if(!strcmpi(gamereg.registry[i].str,"")) {
				strcpy(gamereg.registry[i].str,reg);
				gamereg.registry[i].val = val;
				map_savegameregistry(i);
				return 0;
			}
		}
		
		if (gamereg.registry_num < 10000) {
			gamereg.registry_num = gamereg.registry_num + 1;
			i = gamereg.registry_num - 1;
			strcpy(gamereg.registry[i].str, reg);
			gamereg.registry[i].val = val;
			map_savegameregistry(i);
			return 0;
		}
	}
	
	return 0;
}
//reads game registry
int map_readglobalgamereg(char *reg) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, reg);
	
	for(i = 0; i < gamereg.registry_num; i++) {
		if(!strcmpi(gamereg.registry[i].str, reg)) {
			exist = i;
			break;
		}
	}

	if(exist != -1) {
		return gamereg.registry[exist].val;
	} else {
		return 0;
	}
	
	return 0;
}

int map_readclanbanks(int id) {
	int i,itemid,amount;
	SqlStmt* stmt;
	struct clan_bank cbank;
	struct clan_data *clan;

	memset(&cbank,0,sizeof(cbank));
	stmt = SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return -1;
	}

	clan = clandb_search(id);
	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `CbkItmId`, `CbkAmount` FROM `ClanBanks` WHERE `CbkClnId` = '%d' LIMIT %d", id, MAX_BANK_SLOTS)
	|| SQL_ERROR == SqlStmt_BindParam(stmt, 0, SQLDT_UINT, &id, 0)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &cbank.item, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &cbank.amount, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return -1;
	}

	for(i=0;i<MAX_BANK_SLOTS && SQL_SUCCESS == SqlStmt_NextRow(stmt);i++) {
		memcpy(&clan->clanbanks[i], &cbank, sizeof(cbank));
	}

	SqlStmt_Free(stmt);
	printf("Loaded bank slots for clan %s\n", clandb_name(id));
	return 0;
}

int map_saveclanbanks(int id) {
	SqlStmt* stmt;
	int save_id[1000];
	unsigned int item_id;
	int i;
	struct clan_data *clan;

	memset(save_id,0,1000*sizeof(unsigned int));
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	clan = clandb_search(id);
	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `CbkPosition` FROM `ClanBanks` WHERE `CbkClnId` = '%d'", id)
	|| SQL_ERROR == SqlStmt_BindParam(stmt, 0, SQLDT_UINT, &id, 0)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &item_id, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for(i=0;i<1000 && SQL_SUCCESS == SqlStmt_NextRow(stmt);i++) {
		save_id[i]=item_id;
	}

	for(i=0;i<1000;i++) {
		if(save_id[i]) {
			if(clan->clanbanks[i].amount == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `ClanBanks` WHERE `CbkPosition` = '%d'",save_id[i])) {
					Sql_ShowDebug(sql_handle);
					return 0;
				} 
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `ClanBanks` SET `CbkAmount` = '%d' WHERE `CbkPosition` = '%d'",clan->clanbanks[i].amount, save_id[i])) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		} else {
			if(clan->clanbanks[i].amount != 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `ClanBanks` (`CbkClnId`, `CbkItmId`, `CbkAmount`, `CbkPosition`) VALUES ('%d', '%d', '%d', '%d')", id, clan->clanbanks[i].item, clan->clanbanks[i].amount, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}

	SqlStmt_Free(stmt);
	
	return 0;
}

int map_weather(int id, int n) {
	if(old_time != cur_time) {
		old_time = cur_time;
		sl_doscript_blargs("mapWeather", NULL, 0);
	}
	
	return 0;
}

int map_cronjob(int none, int nonetoo) {
	unsigned int t = time(NULL);
	
	if (t % 60 == 0) {
		sl_doscript_blargs("cronJobMin", NULL, 0);
	}

	if (t % 3600 == 0) {
		sl_doscript_blargs("cronJobHour", NULL, 0);
	}
	
	if (t % 86400 == 0) {
		sl_doscript_blargs("cronJobDay", NULL, 0);
	}
	
	sl_doscript_blargs("cronJobSec", NULL, 0);
	return 0;
}

int map_savechars(int none, int nonetoo) {
	USER *sd = NULL;
	int x;
	
	for (x = 1; x < fd_max; x++) {
		if (session[x] && (sd = (USER*)session[x]->session_data) && !session[x]->eof) {
			intif_save(sd);
		}
	}
	return 0;
}