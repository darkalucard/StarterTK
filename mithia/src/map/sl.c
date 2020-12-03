// TODO: pcl functions like dialog and menu should accept an npc parameter!
#include "sl.h"
#include <stdio.h>
#include <lauxlib.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "clif.h"
#include <assert.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include "pc.h"
#include "mob.h"
#include "itemdb.h"
#include "recipedb.h"
#include "map.h"
#include "socket.h"
#include "clan_db.h"
#include "mmo.h"
#include "malloc.h"
#include "db_mysql.h"
#include "crypt.h"
#include "npc.h"
#include "command.h"

#define sl_redtext(text) "\033[31;1m" text "\033[0m"
#define sl_err(text) printf(sl_redtext("Lua error:") " %s\n", text)
#define sl_err_print(state) sl_err(lua_tostring(state, -1))
// accessors should return 1 if the attribute was handled; otherwise an error will be raised
typedef int (*typel_getattrfunc)(lua_State *, void * /* self */, char * /* attrname */);
typedef int (*typel_setattrfunc)(lua_State *, void * /* self */, char * /*attrname */);
typedef int (*typel_initfunc)(lua_State *, void * /* self */, int /* dataref */, void * /* param */);
typedef struct typel_class_ {
	// an luaL_ref reference to the prototype table for this class. the prototype table holds
	// the functions for this class.
	int protoref;
	char *name;
	typel_getattrfunc getattr;
	typel_setattrfunc setattr;
	typel_initfunc init;
	// remember when implementing the ctor that the first argument will always be the class
	// however, luaL_argerror uses normal indices; as in 1 still refers to the first real argument
	lua_CFunction ctor;
} typel_class;
typedef struct typel_inst_ {
	typel_class *type;
	int dataref;
	void *self;
} typel_inst;
int errors;
int typel_mtindex(lua_State *);
int typel_mtnewindex(lua_State *);
int typel_mtgc(lua_State *);
int typel_mtref;
// TODO: actually use this!
#define sl_ensureasync(state) \
	if(state == sl_gstate) { luaL_error(state, "this function must be called from a coroutine; " \
		"that is, the calling method must be marked with async()"); }

// TODO: do we even need this anymore?
#define typel_err_invalidattr(state, type, attrname) \
	luaL_error(state, "'%s' object has no attribute '%s'", type->name, attrname)
void typel_staticinit();
void typel_staticdestroy(); // TODO: this should unref typel_mtref?
typedef int (*typel_func)(lua_State *, void * /* self */);
#define typel_topointer(state, index) (((typel_inst *)lua_touserdata(state, index))->self)
#define typel_check(state, index, type) (((typel_inst *)lua_touserdata(state, index))->type == type)
int typel_boundfunc(lua_State *);
void typel_pushinst(lua_State *, typel_class *, void *, void *);
typel_class typel_new(char *, lua_CFunction);
void typel_extendproto(typel_class *, char *, typel_func);
#define sl_memberarg(x) (x + 1)
#define sl_memberself 1

int bll_getattr(lua_State *, struct block_list *, char *attrname);
int bll_setattr(lua_State *, struct block_list *, char *attrname);
void bll_pushinst(lua_State *, struct block_list *, void *);

#define pcl_pushinst(state, sd) typel_pushinst(state, &pcl_type, sd, 0)
typel_class pcl_type;
void pcl_staticinit();
#define regl_pushinst(state, sd) typel_pushinst(state, &regl_type, sd, 0)
typel_class regl_type;
void regl_staticinit();
#define npcintregl_pushinst(state, sd) typel_pushinst(state, &npcintregl_type, sd, 0)
typel_class npcintregl_type;
void npcintregl_staticinit();
#define questregl_pushinst(state, sd) typel_pushinst(state, &questregl_type, sd, 0)
typel_class questregl_type;
void questregl_staticinit();
#define npcregl_pushinst(state, nd) typel_pushinst(state, &npcregl_type, nd, 0)
typel_class npcregl_type;
void npcregl_staticinit();
#define mobregl_pushinst(state, mob) typel_pushinst(state, &mobregl_type, mob, 0)
typel_class mobregl_type;
void mobregl_staticinit();
#define fll_pushinst(state,fl) typel_pushinst(state, &fll_type, fl, 0)
typel_class fll_type;
void fll_staticinit();
#define mapregl_pushinst(state,sd) typel_pushinst(state,&mapregl_type,sd,0)
typel_class mapregl_type;
void mapregl_staticinit();
#define gameregl_pushinst(state, sd) typel_pushinst(state, &gameregl_type,sd, 0)
typel_class gameregl_type;
void gameregl_staticinit();
#define acctregl_pushinst(state, sd) typel_pushinst(state, &acctregl_type, sd, 0)
typel_class acctregl_type;
void acctregl_staticinit();
#define mobl_pushinst(state, mob) typel_pushinst(state, &mobl_type, mob, 0)
typel_class mobl_type;
void mobl_staticinit();
#define npcl_pushinst(state, nd) typel_pushinst(state, &npcl_type, nd, 0)
typel_class npcl_type;
void npcl_staticinit();
//#define cnpcl_pushinst(state, cnd) typel_pushinst(state, &cnpcl_type, cnd, 0)
//typel_class cnpcl_type;
//void cnpcl_staticinit();
#define iteml_pushinst(state, item) typel_pushinst(state, &iteml_type, item, 0)
typel_class iteml_type;
void iteml_staticinit();
#define recipel_pushinst(state, recipe) typel_pushinst(state, &recipel_type, recipe, 0)
typel_class recipel_type;
void recipel_staticinit();
#define biteml_pushinst(state, bitem) typel_pushinst(state, &biteml_type, bitem, 0)
typel_class biteml_type;
void biteml_staticinit();
#define parcell_pushinst(state, parcel) typel_pushinst(state, &parcell_type, parcel, 0)
typel_class parcell_type;
void parcell_staticinit();
#define SL_EXPOSE_ENUM(val) lua_pushnumber(sl_gstate, val); lua_setglobal(sl_gstate, #val)
#define sl_pushref(state, ref) lua_rawgeti(state, LUA_REGISTRYINDEX, ref)
void sl_checkargs(lua_State *, char *fmt);
int sl_loaddir(char *, lua_State *);
int sl_async(lua_State *);
int sl_async_done(lua_State *);
void sl_async_freeco(USER *);
void sl_async_resume(USER *, int);
int bll_spawn(lua_State *state, struct block_list* bl) {
	sl_checkargs(state,"nnnn");
	int i = 0;
	int mob = lua_tonumber(state, sl_memberarg(1));
	int x = lua_tonumber(state, sl_memberarg(2));
	int y = lua_tonumber(state, sl_memberarg(3));
	unsigned int amount = lua_tonumber(state, sl_memberarg(4));
	int m = lua_tonumber(state,sl_memberarg(5));
	unsigned int* spawnedmobs = NULL;
	lua_newtable(state);

	if(m != 0) {
		spawnedmobs = mobspawn_onetime(mob, m, x, y, amount, 0, 0, 0);
		
		for (i = 1; i <= amount; i ++) {
			bll_pushinst(state, map_id2bl(spawnedmobs[i - 1]), 0);
			lua_rawseti(state, -2, i);
		}
	} else {
		spawnedmobs = mobspawn_onetime(mob, bl->m, x, y, amount, 0, 0, 0);
		
		for (i = 1; i <= amount; i ++) {
			bll_pushinst(state, map_id2bl(spawnedmobs[i - 1]), 0);
			lua_rawseti(state, -2, i);
		}
	}
	
	FREE(spawnedmobs);
	return 1;
}
int bll_getobjects_helper(struct block_list *bl, va_list ap) {
	lua_State *state = va_arg(ap, lua_State *);
	MOB* mob=NULL;
	USER* ptr=NULL;
	int* def=va_arg(ap,int*);
	nullpo_ret(0,bl);
	if(bl->type==BL_MOB) {
		mob=(MOB*)bl;
		if(mob->state==MOB_DEAD) {
			return 0;
		}
	}
	if(bl->type == BL_PC) {
        ptr = (USER*)bl;
		if(ptr&&(ptr->optFlags&optFlag_stealth))
			return 0;
	}

	def[0]++;
	bll_pushinst(state, bl, 0);
	lua_rawseti(state, -2, def[0]);
}
int bll_getobjects(lua_State *state, void *self, int area) {
	sl_checkargs(state, "n");
	struct block_list *bl=(struct block_list*)self;
	int def[1];
	def[0]=0;
/*#define NUMBER_FIELD(name) \
	lua_getfield(state, sl_memberself, #name); \
	int name = lua_tonumber(state, -1); \
	lua_pop(state, 1)

	NUMBER_FIELD(m);
	NUMBER_FIELD(x);
	NUMBER_FIELD(y);
#undef NUMBER_FIELD*/

	lua_newtable(state);
	int type = lua_tonumber(state, sl_memberarg(1));
	map_foreachinarea(bll_getobjects_helper, bl->m, bl->x, bl->y, area, type, state,def);
	return 1;
}
int bll_getobjects_cell(lua_State *state, void *self) {
	sl_checkargs(state, "nnnn");
	int m = lua_tonumber(state, sl_memberarg(1)),
	    x = lua_tonumber(state, sl_memberarg(2)),
	    y = lua_tonumber(state, sl_memberarg(3)),
	    type = lua_tonumber(state, sl_memberarg(4));
	int def[1];
	def[0]=0;
	lua_newtable(state);
	map_foreachincell(bll_getobjects_helper, m, x, y, type, state,def);
	return 1;
}
int bll_getobjects_cell_traps(lua_State *state, void *self) {
	sl_checkargs(state, "nnnn");
	int m = lua_tonumber(state, sl_memberarg(1)),
	    x = lua_tonumber(state, sl_memberarg(2)),
	    y = lua_tonumber(state, sl_memberarg(3)),
	    type = lua_tonumber(state, sl_memberarg(4));
	int def[1];
	def[0]=0;
	lua_newtable(state);
	map_foreachincellwithtraps(bll_getobjects_helper, m, x, y, type, state,def);
	return 1;
}
int bll_getobjects_area(lua_State *state, void *self) {
	return bll_getobjects(state, self, AREA);
}
int bll_getobjects_samemap(lua_State *state, void *self) {
	return bll_getobjects(state, self, SAMEMAP);
}
int bll_getobjects_map(lua_State *state, void *self) {
	sl_checkargs(state, "nn");
	struct block_list *bl=(struct block_list*)self;
	int def[1];
	def[0]=0;

	lua_newtable(state);
	int m = lua_tonumber(state, sl_memberarg(1));
	int type = lua_tonumber(state, sl_memberarg(2));
	map_foreachinarea(bll_getobjects_helper, m, bl->x, bl->y, SAMEMAP, type, state,def);
	return 1;
}
int bll_getblock(lua_State *state, void *self) {
	sl_checkargs(state, "n");
	struct block_list *bl = map_id2bl(lua_tonumber(state, sl_memberarg(1)));
	
	if (bl) {
		bll_pushinst(state, bl, 0);
		return 1;
	} else {
		lua_pushnil(state);
		return 1;
	}
}
int bll_sendanimxy(lua_State *state, void *self) {
	sl_checkargs(state, "nnn");
	struct block_list *bl = (struct block_list*)self;
	int anim=lua_tonumber(state,sl_memberarg(1));
	int x=lua_tonumber(state,sl_memberarg(2));
	int y=lua_tonumber(state,sl_memberarg(3));
	int times = lua_tonumber(state, sl_memberarg(4));

	map_foreachinarea(clif_sendanimation_xy, bl->m, bl->x, bl->y, AREA, BL_PC, anim, times, x, y);
	return 0;
}
int bll_sendanim(lua_State *state, void *self) {
	sl_checkargs(state, "n");
	struct block_list *bl = (struct block_list*)self;
	int anim = lua_tonumber(state, sl_memberarg(1));
	int times = lua_tonumber(state, sl_memberarg(2));

	map_foreachinarea(clif_sendanimation, bl->m, bl->x, bl->y, AREA, BL_PC, anim, bl, times);
	return 0;
}

int sl_sendanimationxy(USER *sd, ...) {
	va_list ap;

	va_start (ap, sd);
	clif_sendanimation_xy(&sd->bl, ap);
	va_end (ap);
	return 0;
}


int sl_sendanimation(USER *sd, ...) {
	va_list ap;

	va_start (ap, sd);
	clif_sendanimation(&sd->bl, ap);
	va_end (ap);
	return 0;
}

int bll_selfanimationxy(lua_State *state, struct block_list *bl) {
	USER *sd = NULL;
	int anim = lua_tonumber(state, sl_memberarg(2));
	int x = lua_tonumber(state, sl_memberarg(3));
	int y = lua_tonumber(state, sl_memberarg(4));
	int times = lua_tonumber(state, sl_memberarg(5));
	
	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "nnnn");
		sd = map_id2sd(lua_tonumber(state, sl_memberarg(1)));
	} else {
		sl_checkargs(state, "snnn");
		sd = map_name2sd(lua_tostring(state, sl_memberarg(1)));
	}
	
	sl_sendanimationxy(&sd->bl, anim, times, x, y);
	return 0;
}

int bll_selfanimation(lua_State *state, struct block_list *bl) {
	USER *sd = NULL;
	int anim = lua_tonumber(state, sl_memberarg(2));
	int times = lua_tonumber(state, sl_memberarg(3));
	
	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "nn");
		sd = map_id2sd(lua_tonumber(state, sl_memberarg(1)));
	} else {
		sl_checkargs(state, "sn");
		sd = map_name2sd(lua_tostring(state, sl_memberarg(1)));
	}
	
	sl_sendanimation(&sd->bl, anim, bl, times);
	return 0;
}

int bll_sendaction(lua_State *state,void *self) {
	sl_checkargs(state,"nn");
	struct block_list *bl=(struct block_list*)self;
	int action=lua_tonumber(state,sl_memberarg(1));
	int time=lua_tonumber(state,sl_memberarg(2));
	
	clif_sendaction(bl,action,time,0);
	return 0;
}
int bll_throw(lua_State *state,void *self) {
	sl_checkargs(state,"nnnnn");
	int x,y,icon,color,action;
	char buf[255];
	x=lua_tonumber(state,sl_memberarg(1));
	y=lua_tonumber(state,sl_memberarg(2));
	icon=lua_tonumber(state,sl_memberarg(3));
	color=lua_tonumber(state,sl_memberarg(4));
	action=lua_tonumber(state,sl_memberarg(5));
	struct block_list* bl=(struct block_list*)self;

	WBUFB(buf,0)=0xAA;
	WBUFW(buf,1)=SWAP16(0x1B);
	WBUFB(buf,3)=0x16;
	WBUFB(buf,4)=0x03;
	WBUFL(buf,5)=SWAP32(bl->id);
	WBUFW(buf,9)=SWAP16(icon+49152);
	WBUFB(buf,11)=color;
	WBUFL(buf,12)=0; //0 to not produce an image on ground.
	WBUFW(buf,16)=SWAP16(bl->x);
	WBUFW(buf,18)=SWAP16(bl->y);
	WBUFW(buf,20)=SWAP16(x);
	WBUFW(buf,22)=SWAP16(y);
	WBUFL(buf,24)=0;
	WBUFB(buf,28)=action;
	WBUFB(buf,29)=0x00;
	//crypt(WBUFP(buf,0));
	clif_send(buf,30,bl,SAMEAREA);
	return 0;
}
int bll_getusers(lua_State *state, void *self) {
	USER *tsd = NULL;
	struct socket_data* p;
	//struct block_list *bl= (struct block_list*)self;
	int i;
	lua_newtable(state);
	for(i=0;i<fd_max;i++) {
		p=session[i];
		if(p && (tsd=p->session_data)) {
			bll_pushinst(state, &tsd->bl, 0);
			lua_rawseti(state, -2, lua_objlen(state, -2) + 1);
		}
	}
	return 1;
}
int getobjectsmap(lua_State *state) {
	int m=lua_tonumber(state,1);
	int type=lua_tonumber(state,2);
	int x;
	MOB *te;
	//lua_newtable(state);
	//printf("Map: %d\n",type);
	//map_foreachinarea(bll_getobjects_helper, m, 1, 1, SAMEMAP, type, state);
	/*for(x=1;x<map[m].block_mob_count+;x++) {
		te=map[m].block_mob[x];
		bll_pushinst(state,&te->bl,0);
		lua_rawseti(state,-2,x+1);
	}*/
	return 0;
}
void sl_checkargs(lua_State *state, char *fmt) {

	int narg;
	for(narg = 1; fmt[narg - 1]; ++narg) {
		int expectedtype = LUA_TNIL, gottype = lua_type(state, sl_memberarg(narg));
		switch(fmt[narg - 1]) {
		case 'n': expectedtype = LUA_TNUMBER; break;
		case 'b': expectedtype = LUA_TBOOLEAN; break;
		case 't': expectedtype = LUA_TTABLE; break;
		case 's': expectedtype = LUA_TSTRING; break;
		case 'f': expectedtype = LUA_TFUNCTION; break;
		case 'u': expectedtype = LUA_TUSERDATA; break;
		}
		if(gottype != expectedtype) {
			char message[512];
			message[0] = 0;
			sprintf(message, "%s expected, got %s",
				lua_typename(state, expectedtype), lua_typename(state, gottype));
			luaL_argerror(state, sl_memberarg(narg), message);
		}
	}
}
int sl_exec_redirectio(lua_State *state) {
	USER *sd = lua_touserdata(state, lua_upvalueindex(1));
	char *msg = lua_tostring(state, 1);
	int msglen = lua_objlen(state, 1);
	clif_sendwisp(sd, "lua", msg, msglen);
	return 0;
}
int sl_luasize(USER* sd) {
	char buf[255];

	sprintf(buf,"LUA is using %u KB",lua_gc(sl_gstate,LUA_GCCOUNT,0));
	clif_sendminitext(sd,buf,0);
	return 0;
}

int sl_updatepeople(struct block_list *bl, va_list ap) {
	USER *sd = NULL;
	int x = 0;
	int y = 0;
	
	nullpo_ret(0, sd = (USER*)bl);
	
	if (sd->bl.id >= MOB_START_NUM) return 0;
	
	if (sd->bl.x - 9 < 0) x = 9;
	else if (sd->bl.x - 9 + 19 >= map[sd->bl.m].xs) x = map[sd->bl.m].xs - 10;
	else x = sd->bl.x;
	
	if (sd->bl.y - 8 < 0) y = 8;
	else if (sd->bl.y - 8 + 17 >= map[sd->bl.m].ys) y = map[sd->bl.m].ys - 9;
	else y = sd->bl.y;
	
	clif_sendmapdata(sd, sd->bl.m, x - 9, y - 8, 19, 17, 0);
	return 0;
}
int setpostcolor(lua_State *state) {
	sl_checkargs(state, "nn");
	int board=lua_tonumber(state, 1);
	int post=lua_tonumber(state, 2);
	int color=lua_tonumber(state, 3);
	map_changepostcolor(board,post,color);
	return 0;
}
int setobject(lua_State *state) {
	sl_checkargs(state,"nnn");
	int m=lua_tonumber(state, 1);
	int x=lua_tonumber(state, 2);
	int y=lua_tonumber(state, 3);
	int newnum=lua_tonumber(state, 4);

	if(map_isloaded(m)) {
		map[m].obj[x+y*map[m].xs]=newnum;
		map_foreachinarea(sl_updatepeople,m,x,y,AREA,BL_PC);
	}

	return 0;
}

int getobject(lua_State *state) {
	sl_checkargs(state,"nn");
	int m=lua_tonumber(state, 1);
	int x=lua_tonumber(state, 2);
	int y=lua_tonumber(state, 3);

	if(map_isloaded(m)) {
		lua_pushnumber(state,map[m].obj[x+y*map[m].xs]);
	}

	return 1;
}

int settile(lua_State *state) {
	sl_checkargs(state, "nnn");
	int m = lua_tonumber(state, 1);
	int x = lua_tonumber(state, 2);
	int y = lua_tonumber(state, 3);
	int newnum = lua_tonumber(state, 4);

	if (map_isloaded(m)) {
		map[m].tile[x + y * map[m].xs] = newnum;
		map_foreachinarea(sl_updatepeople,m,x,y,AREA,BL_PC);
	}
	
	return 0;
}

int gettile(lua_State *state) {
	sl_checkargs(state, "nn");
	int m = lua_tonumber(state, 1);
	int x = lua_tonumber(state, 2);
	int y = lua_tonumber(state, 3);
	
	if (map_isloaded(m)) {
		lua_pushnumber(state, map[m].tile[x + y * map[m].xs]);
	}
	
	return 1;
}

int setpass(lua_State *state) {
	sl_checkargs(state,"nnn");
	int m=lua_tonumber(state, 1);
	int x=lua_tonumber(state, 2);
	int y=lua_tonumber(state, 3);
	int newnum=lua_tonumber(state, 4);
	if(map_isloaded(m)) {
		map[m].pass[x+y*map[m].xs]=newnum;
		map_foreachinarea(sl_updatepeople,m,x,y,AREA,BL_PC);
	}
	return 0;
}
int getpass(lua_State *state) {
	sl_checkargs(state,"nn");
	int m=lua_tonumber(state, 1);
	int x=lua_tonumber(state, 2);
	int y=lua_tonumber(state, 3);
	if(map_isloaded(m)) {
		lua_pushnumber(state,map[m].pass[x+y*map[m].xs]);
	}
	return 1;
}

int setmap(lua_State *state) {
	sl_checkargs(state, "s");
	USER* tmpsd = NULL;
	unsigned short buff;
	unsigned int pos = 0;
	int i;
	int m = lua_tonumber(state, 1);
	char* title = lua_tostring(state, 3);
	int bgm = lua_tonumber(state, 4);
	int pvp = lua_tonumber(state, 5);
	int spell = lua_tonumber(state, 6);
	int light = lua_tonumber(state, 7);
	int weather = lua_tonumber(state, 8);
	int sweeptime = lua_tonumber(state, 9);
	int cantalk = lua_tonumber(state, 10);
	int show_ghosts = lua_tonumber(state, 11);
	int region = lua_tonumber(state, 12);
	int indoor = lua_tonumber(state, 13);
	int warpout = lua_tonumber(state, 14);
	int bind = lua_tonumber(state, 15);
	FILE* fp = fopen(lua_tostring(state, 2), "rb");
	int blockcount = map[m].bxs * map[m].bys;
	
	if (fp == NULL) {
		printf("MAP_ERR: Map file not found (%s).\n", lua_tostring(state, 2));
		return -1;
	}

	if (title != NULL) strcpy(map[m].title, title);
	map[m].bgm = bgm;
	map[m].pvp = pvp;
	map[m].spell = spell;
	map[m].light = light;
	map[m].weather = weather;
	map[m].sweeptime = sweeptime;
	map[m].cantalk = cantalk;
	map[m].show_ghosts = show_ghosts;
	map[m].region = region;
	map[m].indoor = indoor;
	map[m].warpout = warpout;
	map[m].bind = bind;
	fread(&buff, 2, 1, fp);
	map[m].xs = SWAP16(buff);
	fread(&buff, 2, 1, fp);
	map[m].ys = SWAP16(buff);

	if (map_isloaded(m)) {
		REALLOC(map[m].tile, unsigned short, map[m].xs * map[m].ys);
		REALLOC(map[m].obj, unsigned short, map[m].xs * map[m].ys);
		REALLOC(map[m].map, unsigned char, map[m].xs * map[m].ys);
		REALLOC(map[m].pass, unsigned short, map[m].xs * map[m].ys);
	} else {
		CALLOC(map[m].tile, unsigned short, map[m].xs * map[m].ys);
		CALLOC(map[m].obj, unsigned short, map[m].xs * map[m].ys);
		CALLOC(map[m].map, unsigned char, map[m].xs * map[m].ys);
		CALLOC(map[m].pass, unsigned short, map[m].xs * map[m].ys);
	}
	
	map[m].bxs = (map[m].xs + BLOCK_SIZE - 1) / BLOCK_SIZE;
	map[m].bys = (map[m].ys + BLOCK_SIZE - 1) / BLOCK_SIZE;
	
	if (map_isloaded(m)) {
		FREE(map[m].warp);
		CALLOC(map[m].warp, struct warp_list*, map[m].bxs*map[m].bys);
		REALLOC(map[m].block, struct block_list*, map[m].bxs*map[m].bys);
		REALLOC(map[m].block_mob, struct block_list*, map[m].bxs*map[m].bys);
		
		if (map[m].bxs * map[m].bys > blockcount) {
			for (i = blockcount; i < map[m].bxs * map[m].bys; i++) {
				map[m].block[i] = NULL;
				map[m].block_mob[i] = NULL;
			}
		}
	} else {
		CALLOC(map[m].warp, struct warp_list*, map[m].bxs*map[m].bys);
		CALLOC(map[m].block, struct block_list*, map[m].bxs*map[m].bys);
		CALLOC(map[m].block_mob, struct block_list*, map[m].bxs*map[m].bys);
		CALLOC(map[m].registry, struct global_reg, 1000);
	}
	
	while(!feof(fp)) {
		fread(&buff, 2, 1, fp);
		map[m].tile[pos] = SWAP16(buff);
		fread(&buff, 2, 1, fp);
		map[m].pass[pos] = SWAP16(buff);
		fread(&buff,2,1,fp);
		map[m].obj[pos] = SWAP16(buff);
		
		pos++;
		if (pos>=map[m].xs*map[m].ys)
			break;
	}
	
	pos = 0;
	fclose(fp);
	map_loadregistry(m);
	map_foreachinarea(sl_updatepeople,m,0,0,SAMEMAP,BL_PC);
	return 0;
}

void sl_exec(USER *sd, char *str) {
#define sl_exec_err() \
	char *msg = lua_tostring(sl_gstate, -1); \
	int msglen = lua_objlen(sl_gstate, -1); \
	clif_sendmsg(sd,0, msg, msglen)

	if(luaL_loadstring(sl_gstate, str) != 0) {
		sl_exec_err();
		lua_pop(sl_gstate, 1);
		return;
	}
	lua_newtable(sl_gstate); // the environment table
	lua_pushlightuserdata(sl_gstate, sd);
	lua_pushcclosure(sl_gstate, sl_exec_redirectio, 1);
	lua_setfield(sl_gstate, -2, "print"); // set the new print
	lua_newtable(sl_gstate); // the metatable for the environment
	lua_pushvalue(sl_gstate, LUA_GLOBALSINDEX);
	lua_setfield(sl_gstate, -2, "__index"); // set __index = _G
	lua_setmetatable(sl_gstate, -2);
	lua_setfenv(sl_gstate, -2);
	if(lua_pcall(sl_gstate, 0, 0, 0) != 0) {
		sl_exec_err();
		lua_pop(sl_gstate, 1); // pop the error string
		return;
	}
#undef sl_exec_err
}

int getofflineid(lua_State *state) {
	int id;
	char name[16];
	SqlStmt* stmt;

	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt==NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if (lua_isnumber(state, 1)) {
		int charid = lua_tonumber(state, 1);
		if(SQL_ERROR==SqlStmt_Prepare(stmt,"SELECT `ChaName` FROM `Character` WHERE `ChaId` = '%u'",charid)
		|| SQL_ERROR==SqlStmt_Execute(stmt)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,0,SQLDT_STRING,&name,sizeof(name),NULL,NULL)) {
			SqlStmt_ShowDebug(stmt);
			SqlStmt_Free(stmt);
			return 0;
		}

		if(SQL_SUCCESS==SqlStmt_NextRow(stmt)) {
			lua_pushstring(state, name);
			return 1;
		} else {
			lua_pushboolean(state, 0);
			return 1;
		}
	} else {
		char* name=lua_tostring(state, 1);
		if(SQL_ERROR==SqlStmt_Prepare(stmt,"SELECT `ChaId` FROM `Character` WHERE `ChaName` = '%s'",name)
		|| SQL_ERROR==SqlStmt_Execute(stmt)
		|| SQL_ERROR==SqlStmt_BindColumn(stmt,0,SQLDT_UINT,&id,0,NULL,NULL)) {
			SqlStmt_ShowDebug(stmt);
			SqlStmt_Free(stmt);
			return 0;
		}

		if(SQL_SUCCESS==SqlStmt_NextRow(stmt)) {
			lua_pushnumber(state, id);
			return 1;
		} else {
			lua_pushboolean(state, 0);
			return 1;
		}
	}
}
int func_panic(lua_State *state) {

	sl_err_print(state);
	lua_pop(state,1); //pop Error String
	return 0;

}
int sl_throw(struct block_list* bl, va_list ap) {
	USER* sd = NULL;

	nullpo_ret(0, sd = (USER*)bl);

	char* buf=va_arg(ap,char*);
	int len=va_arg(ap,int);
	
	if (!session[sd->fd])
	{
		session[sd->fd]->eof = 8;
		return 0;
	}

	WFIFOHEAD(sd->fd,len);
	memcpy(WFIFOP(sd->fd,0),buf,len);
	WFIFOSET(sd->fd,encrypt(sd->fd));
	return 0;
}
int throw(lua_State *state) {
	sl_checkargs(state,"nnnnnnnn");
	int x,y,icon,color,action,m;
	int x2,y2;
	char buf[255];
	int id=0;
	id=lua_tonumber(state, 1);
	m=lua_tonumber(state, 2);
	x=lua_tonumber(state, 3);
	y=lua_tonumber(state, 4);
	x2=lua_tonumber(state, 5);
	y2=lua_tonumber(state, 6);
	icon=lua_tonumber(state, 7);
	color=lua_tonumber(state, 8);
	action=lua_tonumber(state, 9);
	//struct block_list* bl=(struct block_list*)self;

	WBUFB(buf,0)=0xAA;
	WBUFW(buf,1)=SWAP16(0x1B);
	WBUFB(buf,3)=0x16;
	WBUFB(buf,4)=0x03;
	WBUFL(buf,5)=SWAP32(id);
	WBUFW(buf,9)=SWAP16(icon+49152);
	WBUFB(buf,11)=color;
	WBUFL(buf,12)=0; //0 to not produce an image on ground.
	WBUFW(buf,16)=SWAP16(x);
	WBUFW(buf,18)=SWAP16(x);
	WBUFW(buf,20)=SWAP16(x2);
	WBUFW(buf,22)=SWAP16(y2);
	WBUFL(buf,24)=0;
	WBUFB(buf,28)=action;
	WBUFB(buf,29)=0x00;
	//crypt(WBUFP(buf,0));
	map_foreachinarea(sl_throw,m,x,y,SAMEAREA,BL_PC,buf,30);
	//clif_send(buf,30,bl,AREA);
	return 0;
}

int msleep(lua_State *state){
	long msecs = lua_tointeger(state, 1);
	usleep(1000*msecs);
	return 0;
}

int getweather(lua_State* state) {
	int x;
	unsigned char mapregion = lua_tonumber(state, 1);
	unsigned char mapindoor = lua_tonumber(state, 2);
	
	for (x = 0; x < 65535; x++) {
		if(map[x].region == mapregion && map[x].indoor == mapindoor) {
			lua_pushnumber(state, map[x].weather);
			return 1;
		}
	}
	return 0;
}

int setweather(lua_State* state) {
	USER* tmpsd;
	int x, i, timer;
	unsigned int t = time(NULL);
	unsigned char mapregion = lua_tonumber(state, 1);
	unsigned char mapindoor = lua_tonumber(state, 2);
	unsigned char mapweather = lua_tonumber(state, 3);
	
	for (x = 0; x < 65535; x++) {
		if (map_isloaded(x)) {
			timer = map_readglobalreg(x, "artificial_weather_timer");
			if (timer <= t && timer > 0) {
				map_setglobalreg(x, "artificial_weather_timer", 0);
				timer = 0;
			}
			if (map[x].region == mapregion && map[x].indoor == mapindoor && timer == 0) {
				map[x].weather = mapweather;
				for (i = 1; i < fd_max; i++) {
					if (session[i] && (tmpsd = (USER*)session[i]->session_data) && !session[i]->eof) {
						if (tmpsd->bl.m == x) {
							clif_sendweather(tmpsd);
						}
					}
				}
			}
		}
	}
	return 0;
}

int getweatherm(lua_State* state) {
	int mapnum = lua_tonumber(state, 1);
	
	lua_pushnumber(state, map[mapnum].weather);
	return 1;
}

int setweatherm(lua_State* state) {
	USER* tmpsd;
	int i, timer;
	unsigned int t = time(NULL);
	int mapnum = lua_tonumber(state, 1);
	unsigned char mapweather = lua_tonumber(state, 2);
	
	if (map_isloaded(mapnum)) {
		timer = map_readglobalreg(mapnum, "artificial_weather_timer");
		if (timer <= t && timer > 0) {
			map_setglobalreg(mapnum, "artificial_weather_timer", 0);
			timer = 0;
		}
				
		if (timer == 0) {
			map[mapnum].weather = mapweather;
			for (i = 1; i < fd_max; i++) {
				if (session[i] && (tmpsd = (USER*)session[i]->session_data) && !session[i]->eof) {
					if (tmpsd->bl.m == mapnum) {
						clif_sendweather(tmpsd);
					}
				}
			}
		}
	}
	return 0;
}

int curyear(lua_State *state) {
	lua_pushnumber(state,cur_year);
	return 1;
}

int curseason(lua_State* state) {
	lua_pushnumber(state,cur_season);
	return 1;
}

int curday(lua_State* state) {
	lua_pushnumber(state,cur_day);
	return 1;
}

int curtime(lua_State* state) {
	lua_pushnumber(state,cur_time);
	return 1;
}

int realday(lua_State* state) {
	lua_pushnumber(state,getDay());
	return 1;
}

int realhour(lua_State* state) {
	lua_pushnumber(state,getHour());
	return 1;
}

int realminute(lua_State* state) {
	lua_pushnumber(state,getMinute());
	return 1;
}

int realsecond(lua_State* state) {
	lua_pushnumber(state,getSecond());
	return 1;
}

int broadcast(lua_State *state) {
	sl_checkargs(state,"s");

	clif_broadcast(lua_tostring(state, 2),lua_tonumber(state, 1));
	return 0;
}

int checkonline(lua_State *state) {
	int numonline, id;
	char *name = NULL;
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if(stmt==NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if (lua_isnumber(state, 1)) {
		if (lua_tonumber(state, 1) > 0) {
			id = lua_tonumber(state, 1);
			
			if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ChaId` FROM `Character` WHERE `ChaOnline` = '1' AND `ChaId` = '%u'", id)
			 || SQL_ERROR == SqlStmt_Execute(stmt)) {
				SqlStmt_ShowDebug(stmt);
				SqlStmt_Free(stmt);
				return 0;
			}
		}
	} else {
		name = lua_tostring(state, 1);
	
		if(strcmpi(name, "") == 0) {
			if(SQL_ERROR==SqlStmt_Prepare(stmt,"SELECT `ChaId` FROM `Character` WHERE `ChaOnline` = '1' AND `ChaGMLevel` = '0'")
			|| SQL_ERROR==SqlStmt_Execute(stmt)) {
				SqlStmt_ShowDebug(stmt);
				SqlStmt_Free(stmt);
				return 0;
			}
		} else {
			if(SQL_ERROR==SqlStmt_Prepare(stmt,"Select `ChaId` FROM `Character` WHERE `ChaOnline` = '1' AND `ChaName` = '%s'", strlwr(name))
			|| SQL_ERROR==SqlStmt_Execute(stmt)) {
				SqlStmt_ShowDebug(stmt);
				SqlStmt_Free(stmt);
				return 0;
			}
		}
	}
	
	numonline = SqlStmt_NumRows(stmt);
	SqlStmt_Free(stmt);
	lua_pushnumber(state, numonline);
	return 1;
}

int luareload(lua_State* state) {
	lua_newtable(state);
	errors = 0;
	command_reload(NULL, NULL, state);
	return 1;
}

int addchatlog(lua_State* state) {
	int id, type, map;
	char msg[255];
	char escape[255];
	
	sl_checkargs(state, "s");
	id = lua_tonumber(state, 1);
	sprintf(msg, "%s", lua_tostring(state, 2));
	
	Sql_EscapeString(sql_handle,escape,msg);
	
	if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `SayLogs` (`SayChaId`, `SayMessage`, `SayType`) VALUES ('%u', '%s', '%s')",
		id, escape, "LuaAdded")) {
		SqlStmt_ShowDebug(sql_handle);
	}
}

int luatimems(lua_State* state) {
	struct timeb tmb;
	
	ftime(&tmb);
	lua_pushnumber(state, tmb.millitm);
	return 1;
}

int getwarp(lua_State* state) {
	sl_checkargs(state, "nn");
	int m = lua_tonumber(state, 1);
	int x = lua_tonumber(state, 2);
	int y = lua_tonumber(state, 3);
	struct warp_list* i = NULL;
	
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= map[m].xs) x = map[m].xs - 1;
	if (y >= map[m].ys) y = map[m].ys - 1;
	
	if (map_isloaded(m)) {
		for (i = map[m].warp[x / BLOCK_SIZE + (y / BLOCK_SIZE) * map[m].bxs]; i; i = i->next) {
			if (i->x == x && i->y == y) {
				lua_pushboolean(state, 1);
				return 1;
			}
		}
	}
		
	lua_pushboolean(state, 0);
	return 1;
}

int setwarps(lua_State* state) {
	FILE *fp;
	char line[1024];
	char *str[10];
	int lines = 0;
	int x;
	int mx, my, mm, tm, tx, ty;
	struct warp_list* war = NULL;
	sl_checkargs(state, "s");
	int m = lua_tonumber(state, 1);
	
	if ((fp = fopen(lua_tostring(state, 2), "r")) == NULL) {
		printf("WARP_ERR: WARP file not found (%s).\n", lua_tostring(state, 2));
		lua_pushboolean(state, 0);
		return 1;
	}
	
	while (fgets(line, sizeof(line), fp)) {
		lines++;
		if ((line[0] == '/' && line[1] == '/') || line[0] == '\n') continue;
			
		memset(str,0,sizeof(str));
		
		if (sscanf(line, "%u,%u,%u,%u,%u,%u", &mm, &mx, &my, &tm, &tx, &ty) != 6) continue;
		
		CALLOC(war, struct warp_list, 1);
		
		if (!map_isloaded(mm + m) || !map_isloaded(tm + m)) continue;
		if (tm > x) x = tm;
		war->x = mx;
		war->y = my;
		war->tm = tm + m;
		war->tx = tx;
		war->ty = ty;
		war->next = map[mm + m].warp[(mx / BLOCK_SIZE) + (my / BLOCK_SIZE) * map[mm + m].bxs];
		
		if (war->next) war->next->prev = war;
		
		map[mm + m].warp[(mx / BLOCK_SIZE) + (my / BLOCK_SIZE) * map[mm + m].bxs] = war;
	}
	
	fclose(fp);
	lua_pushnumber(state, x + 1);
	return 1;
}

int savemap(lua_State* state) {
	int m = lua_tonumber(state, 1);
	char* mapFile = lua_tostring(state, 2);
	unsigned short* val;
	int x, y;
	FILE* fp;
	
	if (mapFile) {
		fp = fopen(mapFile, "wb");
		
		if (fp) {
			val = SWAP16(map[m].xs);
			fwrite(&val, 2, 1, fp);
			val = SWAP16(map[m].ys);
			fwrite(&val, 2, 1, fp);
			
			for (y = 0; y < map[m].ys; y++) {
				for (x = 0; x < map[m].xs; x++) {
					val = SWAP16(map[m].tile[(y * map[m].xs) + x]);
					fwrite(&val, 2, 1, fp);
					val = SWAP16(map[m].pass[(y * map[m].xs) + x]);
					fwrite(&val, 2, 1, fp);
					val = SWAP16(map[m].obj[(y * map[m].xs) + x]);
					fwrite(&val, 2, 1, fp);
				}
			}
			
			fclose(fp);
		} else {
			lua_pushboolean(state, 0);
			return 1;
		}
	} else {
		lua_pushboolean(state, 0);
		return 1;
	}
	
	lua_pushboolean(state, 1);
	return 1;
}

void sl_init() {
	int i,count=0;
	sl_gstate = lua_open();
	luaL_openlibs(sl_gstate);

	//lua_gc(sl_gstate,LUA_GCSTOP,0); //TEMPORARILY IN..THIS MAKES YIELD WORK
	typel_staticinit();
	pcl_staticinit();
	regl_staticinit();
	npcintregl_staticinit();
	questregl_staticinit();
	npcregl_staticinit();
	mobregl_staticinit();
	mapregl_staticinit();
	gameregl_staticinit();
	acctregl_staticinit();
	mobl_staticinit();
	npcl_staticinit();
	iteml_staticinit();
	recipel_staticinit();
	biteml_staticinit();
	parcell_staticinit();
	fll_staticinit();

	{
		SL_EXPOSE_ENUM(MOB_ALIVE);
		SL_EXPOSE_ENUM(MOB_DEAD);
		SL_EXPOSE_ENUM(MOB_HIT);
		SL_EXPOSE_ENUM(MOB_AGGRESSIVE);
		SL_EXPOSE_ENUM(MOB_STATIONARY);
		SL_EXPOSE_ENUM(MOB_NORMAL);
	}
	{
		SL_EXPOSE_ENUM(BL_PC);
		SL_EXPOSE_ENUM(BL_NPC);
		SL_EXPOSE_ENUM(BL_ITEM);
		SL_EXPOSE_ENUM(BL_MOB);
		SL_EXPOSE_ENUM(BL_NUL);
		SL_EXPOSE_ENUM(BL_ALL);
	}
	{
		SL_EXPOSE_ENUM(EQ_WEAP);
		SL_EXPOSE_ENUM(EQ_ARMOR);
		SL_EXPOSE_ENUM(EQ_SHIELD);
		SL_EXPOSE_ENUM(EQ_HELM);
		SL_EXPOSE_ENUM(EQ_RIGHT);
		SL_EXPOSE_ENUM(EQ_LEFT);
		SL_EXPOSE_ENUM(EQ_SUBLEFT);
		SL_EXPOSE_ENUM(EQ_SUBRIGHT);
		SL_EXPOSE_ENUM(EQ_FACEACC);
		SL_EXPOSE_ENUM(EQ_CROWN);
		SL_EXPOSE_ENUM(EQ_MANTLE);
		SL_EXPOSE_ENUM(EQ_NECKLACE);
		SL_EXPOSE_ENUM(EQ_BOOTS);
		SL_EXPOSE_ENUM(EQ_COAT);
		SL_EXPOSE_ENUM(EQ_FACEACCTWO);
	}
	{
		SL_EXPOSE_ENUM(BASE_MGW);
		SL_EXPOSE_ENUM(BASE_ACMR);
		SL_EXPOSE_ENUM(BASE_VITAMANA);
		SL_EXPOSE_ENUM(BASE_REGEN);
		SL_EXPOSE_ENUM(BASE_VMREGEN);
		SL_EXPOSE_ENUM(BASE_MINMAXDAM);
		SL_EXPOSE_ENUM(BASE_PD);
		SL_EXPOSE_ENUM(BASE_PROT);
		SL_EXPOSE_ENUM(BASE_SPEED);
		SL_EXPOSE_ENUM(BASE_WEAPARMOR);
	}
	{
		SL_EXPOSE_ENUM(WSUN);
		SL_EXPOSE_ENUM(WRAIN);
		SL_EXPOSE_ENUM(WSNOW);
		SL_EXPOSE_ENUM(WBIRD);
	}
	{
		SL_EXPOSE_ENUM(F1_NPC);
	}
	
	//lua_newtable(sl_gstate);
	/*for(i=0;i<65535;i++) {
		if(map_isloaded(i)) {
			lua_pushnumber(sl_gstate,i);
			lua_rawseti(sl_gstate,-2,count+1);
			count++;
		}
	}
	*/
	//lua_setglobal(sl_gstate,"maps");

	lua_register(sl_gstate, "_async", sl_async);
	lua_register(sl_gstate, "_asyncDone", sl_async_done);

	//lua_pushcclosure(sl_gstate,getobjectsmap,1);
	lua_register(sl_gstate,"getOfflineID",getofflineid);
	lua_register(sl_gstate,"getObjectsMap",getobjectsmap);
	lua_register(sl_gstate,"getObject",getobject);
	lua_register(sl_gstate,"setObject",setobject);
	lua_register(sl_gstate,"getTile",gettile);
	lua_register(sl_gstate,"setTile",settile);
	lua_register(sl_gstate,"setPass",setpass);
	lua_register(sl_gstate,"getPass",getpass);
	lua_register(sl_gstate,"setMap",setmap);
	lua_register(sl_gstate,"setPostColor",setpostcolor);
	lua_register(sl_gstate,"throw",throw);
	lua_register(sl_gstate,"msleep",msleep);
	lua_register(sl_gstate,"getWeather",getweather);
	lua_register(sl_gstate,"setWeather",setweather);
	lua_register(sl_gstate,"getWeatherM",getweatherm);
	lua_register(sl_gstate,"setWeatherM",setweatherm);
	lua_register(sl_gstate,"curYear",curyear);
	lua_register(sl_gstate,"curSeason",curseason);
	lua_register(sl_gstate,"curDay",curday);
	lua_register(sl_gstate,"curTime",curtime);
	lua_register(sl_gstate,"realDay",realday);
	lua_register(sl_gstate,"realHour",realhour);
	lua_register(sl_gstate,"realMinute",realminute);
	lua_register(sl_gstate,"realSecond",realsecond);
	lua_register(sl_gstate,"broadcast",broadcast);
	lua_register(sl_gstate,"checkOnline",checkonline);
	lua_register(sl_gstate,"luaReload",luareload);
	lua_register(sl_gstate,"addChatLog",addchatlog);
	lua_register(sl_gstate,"timeMS",luatimems);
	lua_register(sl_gstate,"getWarp",getwarp);
	lua_register(sl_gstate,"setWarps",setwarps);
	lua_register(sl_gstate,"saveMap",savemap);
	lua_atpanic(sl_gstate,func_panic);
	// make sure to load the sys.lua file before anything else!
	if(luaL_dofile(sl_gstate, "../mithialua/Developers/sys.lua")) {
		sl_err_print(sl_gstate);
		return;
	}

	sl_reload(NULL);
}
int sl_loaddir(char *dirpath, lua_State *state) {
	DIR *dir = opendir(dirpath);
	struct dirent *dir_entry;
	char path[512];
	char dirpath_tail = dirpath[strlen(dirpath) - 1];
	int dirpath_hasslash = dirpath_tail == '/' || dirpath_tail == '\\';
	while(0 != (dir_entry = readdir(dir))) {
		char *filename = dir_entry->d_name;
		int filename_len = strlen(filename);
		// ignore special files
		if(!strcmp(filename, ".") || !strcmp(filename, "..") ||
			!strcmp(filename, ".svn") || !strcmp(filename, "sys.lua")) continue;
		strcpy(path, dirpath);
		if(!dirpath_hasslash) strcat(path, "/");
		strcat(path, dir_entry->d_name);
		struct stat dir_entry_stat;
		stat(path, &dir_entry_stat);
		if(S_ISDIR(dir_entry_stat.st_mode)) {
			sl_loaddir(path, state);
		} else if(S_ISREG(dir_entry_stat.st_mode)) {
			// only load files with the extension ".lua"
			if(filename_len < 4) continue;
			if(strcmp(&filename[filename_len - 4], ".lua")) continue;
			if(luaL_dofile(sl_gstate, path)) {
				errors++;
				
				if (state != NULL) {
					lua_pushstring(state, lua_tostring(sl_gstate, -1));
					lua_rawseti(state, -2, errors);
				}
					
				sl_err_print(sl_gstate);
			}
		}
	}
	closedir(dir);
	
	return errors;
}
int sl_reload(lua_State* state) {
	int errors = 0;
	
	errors += sl_loaddir("../mithialua/Accepted/", state);
	errors += sl_loaddir("../mithialua/Developers/", state);
	
	return errors;
}
void sl_fixmem() {
	lua_gc(sl_gstate,LUA_GCCOLLECT,0);
	return 0;
}
int sl_async_done(lua_State *state) {
	// free the coroutine
	USER *sd = typel_topointer(state, 1);
	sl_async_freeco(sd);
}
void sl_async_freeco(USER *sd) {
	if(!sd->coref) return 0;
	luaL_unref(sl_gstate, LUA_REGISTRYINDEX, sd->coref);
	sd->coref = 0;
}
void sl_async_resume(USER *sd, int nargs) {
	unsigned int coref;
	USER *nsd = NULL;
	if(!sd->coref) {
		nsd=map_id2sd(sd->coref_container);
		if(!nsd) return 0;

		coref=nsd->coref;
	} else {
		coref=sd->coref;
	}

	lua_State *costate;
	sl_pushref(sl_gstate, coref);
	if(lua_isthread(sl_gstate,-1)) {
		costate = lua_tothread(sl_gstate, -1);
		lua_pop(sl_gstate, 1); // pop the thread
	//printf("%d\n",lua_status(costate));
		lua_xmove(sl_gstate, costate, nargs);
		if(lua_resume(costate, nargs) == LUA_ERRRUN) {
			char *err = lua_tostring(costate, -1);
			lua_pop(costate, 1);
			sl_async_freeco(sd);
			sl_err(err);
		}
	}

}
int sl_async(lua_State *state) {
	// first argument should be a player object, second argument should be a function to call
	USER *sd = typel_topointer(state, 1);
	if(!sd) return 0;

	if(sd->coref) { clif_sendminitext(sd,"You are busy.",0); return 0; };

	lua_State *costate = lua_newthread(state);
	sd->coref = luaL_ref(state, LUA_REGISTRYINDEX);
	lua_pushvalue(state, 2); // push a copy of the function
	lua_xmove(state, costate, 1); // move the function into the coroutine state
	if(lua_resume(costate, 0) == LUA_ERRRUN) {
		lua_xmove(costate, state, 1);
		sl_async_freeco(sd);
		return lua_error(state);
	}
	return 0;
}
int sl_doscript_stackargs(char *root, char *method, int nargs) {
	int argsindex = lua_gettop(sl_gstate) - (nargs - 1), errhandler, i;
	lua_getglobal(sl_gstate, "_errhandler");
	errhandler = lua_gettop(sl_gstate);
	lua_getglobal(sl_gstate, root);
	if(lua_isnil(sl_gstate, -1)) {
		lua_pop(sl_gstate, 2 + nargs); // pop _errhandler, the table, and the args
		return 0;
	}
	if (lua_istable(sl_gstate, -1)) {
		lua_getfield(sl_gstate, -1, method);
		lua_replace(sl_gstate, -2); // essentially remove the table leaving only the field
	}
	if(lua_isnil(sl_gstate, -1)) {
		lua_pop(sl_gstate, 2 + nargs); // pop _errhandler, the field, and the args
		return 0;
	}
	// push copies of the arguments onto the stack
	for(i = 0; i < nargs; i++)
		lua_pushvalue(sl_gstate, argsindex + i);
	if(lua_pcall(sl_gstate, nargs, 0, errhandler) != 0) {
		//sl_err_print(sl_gstate);
		lua_pop(sl_gstate, 1); // pop the error string
	}
	lua_pop(sl_gstate, nargs + 1); // pop the original arguments, and _errhandler
	return 1;
}
int sl_doscript_blargs(char *root, char *method, int nargs, ...) {

	int i;
	if(!lua_checkstack(sl_gstate,100)) {
		printf("Lua - Min stack requirements not met.\n");

		return 0;
	}
	va_list argp;
	va_start(argp, nargs);
	for(i = 0; i < nargs; i++) {
		struct block_list *bl = va_arg(argp, struct block_list *);
		bll_pushinst(sl_gstate, bl, 0);

	}
	va_end(argp);

	return sl_doscript_stackargs(root, method, nargs);
}
void sl_resumemenu(unsigned int selection, USER *sd) {

	/*if(sd->coroutine) {
		if(lua_status(sd->coroutine)==LUA_YIELD) {
			lua_pushnumber(sd->coroutine, selection);
			lua_resume(sd->coroutine, 1);
		}
	}*/
	lua_pushnumber(sl_gstate, selection);
	sl_async_resume(sd, 1);

}
void sl_resumemenuseq(unsigned int selection, int choice, USER *sd) {
	if(selection==1) { //Quit
		sl_async_freeco(sd);
	}
	if(selection==2) { //Selected(Next)
		lua_pushnumber(sl_gstate,choice);
		sl_async_resume(sd,1);
	}
}
void sl_resumedialog(unsigned int choice, USER *sd) {
	/*if(sd->coroutine) {
		if(lua_status(sd->coroutine)==LUA_YIELD) {
		if(choice == 0) lua_pushstring(sd->coroutine, "previous");
		else if(choice == 1) lua_pushstring(sd->coroutine, "quit");
		else if(choice == 2) lua_pushstring(sd->coroutine, "next");
		else lua_pushnil(sd->coroutine);
		lua_resume(sd->coroutine, 1);
		}
	}*/
	if(choice == 0) { lua_pushstring(sl_gstate, "previous"); }
	//else if(choice == 1) { lua_pushstring(sl_gstate, "quit"); }
	else if(choice == 1) { sl_async_freeco(sd); return; }
	else if(choice == 2) { lua_pushstring(sl_gstate, "next"); }
	else { lua_pushstring(sl_gstate,"quit"); }
	sl_async_resume(sd, 1);
}

void sl_resumebuy(unsigned int choice, USER *sd) {
	/*if(sd->coroutine) {

		if((lua_status(sd->coroutine)==LUA_YIELD)) {
		lua_pushnumber(sd->coroutine,choice);
		lua_resume(sd->coroutine,1);
		} else {

			//sl_err_print(sd->coroutine);
		}
	}*/
	lua_pushnumber(sl_gstate, choice);
	sl_async_resume(sd, 1);
}


void sl_resumesell(unsigned int choice, USER *sd) {
	/*if(sd->coroutine) {
		if(lua_status(sd->coroutine)==LUA_YIELD) {
		lua_pushnumber(sd->coroutine,choice);
		lua_resume(sd->coroutine,1);
		}
	}*/
	lua_pushnumber(sl_gstate, choice);
	sl_async_resume(sd, 1);
}
void sl_resumeinput(char* output, char* output2, USER *sd) {
	/*if(sd->coroutine) {
		if(lua_status(sd->coroutine)==LUA_YIELD) {
		//lua_pushstring(sd->coroutine,output);
		lua_pushstring(sd->coroutine,output2);
		lua_resume(sd->coroutine,1);
		}
	}*/
	lua_pushstring(sl_gstate, output2);
	sl_async_resume(sd, 1);
}

void sl_resume(char* output,USER *sd) {
	/*if(sd->coroutine) {
		if(lua_status(sd->coroutine)==LUA_YIELD) {
		lua_pushstring(sd->coroutine,output);
		lua_resume(sd->coroutine,1);
		}
	}*/
	lua_pushstring(sl_gstate, output);
	sl_async_resume(sd, 1);
}

int iteml_getattr(lua_State *, struct item_data *, char *);
int iteml_ctor(lua_State *);
void iteml_staticinit() {
	iteml_type = typel_new("Item", iteml_ctor);
	iteml_type.getattr = iteml_getattr;
}
int iteml_ctor(lua_State *state) {
	struct item_data *item = 0;
	if(lua_isnumber(state, 2))
		item = itemdb_search(lua_tonumber(state, 2));
	else if(lua_isstring(state, 2))
		item = itemdb_searchname(lua_tostring(state, 2));
	else luaL_argerror(state, 1, "expected string or number");
	if(!item) {
		lua_pushnil(state);
		return 1;
		//luaL_error(state, "could not find item in the database");
	}
	iteml_pushinst(state, item);
	return 1;
}
int iteml_getattr(lua_State *state, struct item_data *item, char *attrname) {
	if(!strcmp(attrname, "vita")) lua_pushnumber(state, item->vita);
	else if(!strcmp(attrname, "mana")) lua_pushnumber(state, item->mana);
	else if(!strcmp(attrname, "dam")) lua_pushnumber(state, item->dam);
	else if(!strcmp(attrname, "price")) lua_pushnumber(state,item->price);
	else if(!strcmp(attrname, "sell")) lua_pushnumber(state,item->sell);
	else if(!strcmp(attrname, "name")) lua_pushstring(state,item->name);
	else if(!strcmp(attrname, "yname")) lua_pushstring(state,item->yname);
	else if(!strcmp(attrname, "id")) lua_pushnumber(state,item->id);
	else if(!strcmp(attrname, "maxAmount")) lua_pushnumber(state,item->max_amount);
	else if(!strcmp(attrname, "maxDura")) lua_pushnumber(state,item->dura);
	else if(!strcmp(attrname, "type")) lua_pushnumber(state,item->type);
	else if(!strcmp(attrname, "depositable")) lua_pushboolean(state,item->depositable);
	else if(!strcmp(attrname, "exchangeable")) lua_pushboolean(state,item->exchangeable);
	else if(!strcmp(attrname, "droppable")) lua_pushboolean(state,item->droppable);
	else if(!strcmp(attrname, "sound")) lua_pushnumber(state,item->sound);
	else if(!strcmp(attrname, "minDmg")) lua_pushnumber(state,item->min_dam);
	else if(!strcmp(attrname, "maxDmg")) lua_pushnumber(state,item->max_dam);
	else if(!strcmp(attrname, "attackSpeed")) lua_pushnumber(state, item->attack_speed);
	else if(!strcmp(attrname, "wisdom")) lua_pushnumber(state, item->wisdom);
	else if(!strcmp(attrname, "thrown")) lua_pushboolean(state, item->thrown);
	else if(!strcmp(attrname, "con")) lua_pushnumber(state, item->con);
	else if(!strcmp(attrname, "event")) lua_pushboolean(state, item->event);
	else if(!strcmp(attrname, "level")) lua_pushnumber(state, item->level);
	else if(!strcmp(attrname, "might")) lua_pushnumber(state, item->might);
	else if(!strcmp(attrname, "grace")) lua_pushnumber(state, item->grace);
	else if(!strcmp(attrname, "will")) lua_pushnumber(state, item->will);
	else if(!strcmp(attrname, "sex")) lua_pushnumber(state, item->sex);
	else if(!strcmp(attrname, "ac")) lua_pushnumber(state, item->ac);
	else if(!strcmp(attrname, "hit")) lua_pushnumber(state, item->hit);
	//else if(!strcmp(attrname, "rank")) lua_pushstring(state, classdb_name(classdb_path(item->class), item->rank));
	else if(!strcmp(attrname, "reqVita")) lua_pushnumber(state, item->req_vita);
	else if(!strcmp(attrname, "reqMana")) lua_pushnumber(state, item->req_mana);
	else if(!strcmp(attrname, "unique")) lua_pushnumber(state, item->unique);
	else if(!strcmp(attrname, "regen")) lua_pushnumber(state, item->healing);
	else if(!strcmp(attrname, "ethereal")) lua_pushboolean(state, item->ethereal);
	else if(!strcmp(attrname, "dodge")) lua_pushnumber(state, item->dodge);
	else if(!strcmp(attrname, "parry")) lua_pushnumber(state, item->parry);
	else if(!strcmp(attrname, "block")) lua_pushnumber(state, item->block);
	else if(!strcmp(attrname, "soundHit")) lua_pushnumber(state, item->sound_hit);
	else if(!strcmp(attrname, "class")) lua_pushnumber(state, item->class);
	else if(!strcmp(attrname, "className")) lua_pushstring(state, classdb_name(item->class, item->rank));
	else if(!strcmp(attrname, "protection")) lua_pushnumber(state, item->protection);
	else if(!strcmp(attrname, "reqMight")) lua_pushnumber(state, item->might_req);	
	else if(!strcmp(attrname, "reqGrace")) lua_pushnumber(state, item->grace_req);
	else if(!strcmp(attrname, "reqWill")) lua_pushnumber(state, item->will_req);
	else if(!strcmp(attrname, "resist")) lua_pushnumber(state, item->resist);
	else if(!strcmp(attrname, "vRegen")) lua_pushnumber(state, item->vregen);
	else if(!strcmp(attrname, "mRegen")) lua_pushnumber(state, item->mregen);
	else if(!strcmp(attrname, "physDeduct")) lua_pushnumber(state, item->physdeduct);
	else if(!strcmp(attrname, "time")) lua_pushnumber(state, item->time);
	else if(!strcmp(attrname, "look")) lua_pushnumber(state, item->look);
	else if(!strcmp(attrname, "lookC")) lua_pushnumber(state, item->look_color);
	else if(!strcmp(attrname, "icon")) lua_pushnumber(state, item->icon);
	else if(!strcmp(attrname, "iconC")) lua_pushnumber(state, item->icon_color);
	else return 0; // the attribute was not handled
	return 1;
}

int biteml_getattr(lua_State *, struct item *, char *);
int biteml_setattr(lua_State *, struct item *, char *);
void biteml_staticinit() {
	biteml_type = typel_new("BoundItem", 0);
	biteml_type.getattr = biteml_getattr;
	biteml_type.setattr = biteml_setattr;
}
int biteml_setattr(lua_State *state, struct item *bitem, char *attrname) {
	if(!strcmp(attrname,"id")) bitem->id=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"amount")) bitem->amount=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"dura")) bitem->dura=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"owner")) bitem->owner_id=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"realName")) strcpy(bitem->real_name,lua_tostring(state,-1));
	else if(!strcmp(attrname,"timeLeft")) bitem->timeleft=lua_tonumber(state,-1);
	else if(!strcmp(attrname, "repairCheck")) bitem->repair = lua_tonumber(state, -1);
	return 1;
}
int biteml_getattr(lua_State *state, struct item *bitem, char *attrname) {
	if(!strcmp(attrname, "amount")) lua_pushnumber(state, bitem->amount);
	else if(!strcmp(attrname, "dura")) lua_pushnumber(state, bitem->dura);
	else if(!strcmp(attrname, "owner")) lua_pushnumber(state,bitem->owner_id);
	else if(!strcmp(attrname, "realName")) lua_pushstring(state,bitem->real_name);
	else if(!strcmp(attrname, "timeLeft")) lua_pushnumber(state,bitem->timeleft);
	else if(!strcmp(attrname, "repairCheck")) lua_pushnumber(state, bitem->repair);
	else {
	struct item_data *item = itemdb_search(bitem->id); // NOTE: this may be slow...?
		assert(item);
		return iteml_getattr(state, item, attrname);
	}
	return 1;
}

int recipel_getattr(lua_State *, struct recipe_data *, char *);
int recipel_ctor(lua_State *);
void recipel_staticinit() {
	recipel_type = typel_new("Recipe", recipel_ctor);
	recipel_type.getattr = recipel_getattr;
}
int recipel_ctor(lua_State *state) {
	struct recipe_data *recipe = NULL;
	if(lua_isnumber(state, 2))
		recipe = recipedb_search(lua_tonumber(state, 2));
	else if(lua_isstring(state, 2))
		recipe = recipedb_searchname(lua_tostring(state, 2));
	else luaL_argerror(state, 1, "expected string or number");
	if(!recipe) {
		lua_pushnil(state);
		return 1;
		//luaL_error(state, "could not find item in the database");
	}
	recipel_pushinst(state, recipe);
	return 1;
}
int recipel_getattr(lua_State *state, struct recipe_data *recipe, char *attrname) {
	if (!strcmp(attrname, "id")) lua_pushnumber(state, recipe->id);
	else if (!strcmp(attrname, "identifier")) lua_pushstring(state, recipe->identifier);
	else if (!strcmp(attrname, "description")) lua_pushstring(state, recipe->description);
	else if (!strcmp(attrname, "critIdentifier")) lua_pushstring(state, recipe->critIdentifier);
	else if (!strcmp(attrname, "critDescription")) lua_pushstring(state, recipe->critDescription);
	else if (!strcmp(attrname, "craftTime")) lua_pushnumber(state, recipe->craftTime);
	else if (!strcmp(attrname, "successRate")) lua_pushnumber(state, recipe->successRate);
	else if (!strcmp(attrname, "skillAdvance")) lua_pushnumber(state, recipe->skillAdvance);
	else if (!strcmp(attrname, "critRate")) lua_pushnumber(state, recipe->critRate);
	else if (!strcmp(attrname, "bonus")) lua_pushnumber(state, recipe->bonus);
	else if (!strcmp(attrname, "skillRequired")) lua_pushnumber(state, recipe->skillRequired);
	else if (!strcmp(attrname, "tokensRequired")) lua_pushnumber(state, recipe->tokensRequired);
	else if (!strcmp(attrname, "materials"))
	{
		int i = 0;
		lua_newtable(state);
		
		for (i = 0; i < 10; i++)
		{
			lua_pushnumber(state, recipe->materials[i]);
			lua_rawseti(state, -2, i + 1);
		}
	}
	else if (!strcmp(attrname, "superiorMaterials"))
	{
		int i = 0;
		lua_newtable(state);
		
		for (i = 0; i < 2; i++)
		{
			lua_pushnumber(state, recipe->superiorMaterials[i]);
			lua_rawseti(state, -2, i + 1);
		}
	}
	else return 0; // the attribute was not handled
	return 1;
}

int parcell_getattr(lua_State *, struct parcel *, char *);
void parcell_staticinit() {
	parcell_type = typel_new("Parcel", 0);
	parcell_type.getattr = parcell_getattr;
}
int parcell_getattr(lua_State *state, struct parcel *parcel, char *attrname) {
	if(!strcmp(attrname, "id")) lua_pushnumber(state, parcel->data.id);
	else if(!strcmp(attrname, "amount")) lua_pushnumber(state, parcel->data.amount);
	else if(!strcmp(attrname, "dura")) lua_pushnumber(state, parcel->data.dura);
	else if(!strcmp(attrname, "owner")) lua_pushnumber(state,parcel->data.owner_id);
	else if(!strcmp(attrname, "realName")) lua_pushstring(state,parcel->data.real_name);
	else if(!strcmp(attrname, "timeLeft")) lua_pushnumber(state,parcel->data.timeleft);
	else if(!strcmp(attrname, "sender")) lua_pushnumber(state,parcel->sender);
	else if(!strcmp(attrname, "pos")) lua_pushnumber(state, parcel->pos);
	else if(!strcmp(attrname, "npcFlag")) lua_pushnumber(state, parcel->npcflag);
	else {
	struct item_data *item = itemdb_search(parcel->data.id); // NOTE: this may be slow...?
		assert(item);
		return iteml_getattr(state, item, attrname);
	}
	return 1;
}

int regl_getattr(lua_State *, USER *, char *);
int regl_setattr(lua_State *, USER *, char *);
struct global_reg *regl_findreg(USER *, char *);
void regl_staticinit() {
	regl_type = typel_new("Registry", 0);
	regl_type.getattr = regl_getattr;
	regl_type.setattr = regl_setattr;
}
int regl_getattr(lua_State *state, USER *sd, char *attrname) {
	lua_pushnumber(state, pc_readglobalreg(sd, attrname));
	return 1;
}
int regl_setattr(lua_State *state, USER *sd, char *attrname) {
	pc_setglobalreg(sd, attrname, lua_tonumber(state, -1));
	return 1;
}
int npcintregl_getattr(lua_State*, USER*, char*);
int npcintregl_setattr(lua_State*, USER*, char*);
void npcintregl_staticinit() {
	npcintregl_type = typel_new("NpcInt", 0);
	npcintregl_type.getattr = npcintregl_getattr;
	npcintregl_type.setattr = npcintregl_setattr;
}
int npcintregl_getattr(lua_State *state, USER *sd, char *attrname) {
	lua_pushnumber(state, pc_readnpcintreg(sd, attrname));
	return 1;
}
int npcintregl_setattr(lua_State *state, USER *sd, char *attrname) {
	pc_setnpcintreg(sd, attrname, lua_tonumber(state, -1));
	return 1;
}
int questregl_getattr(lua_State*, USER*, char*);
int questregl_setattr(lua_State*, USER*, char*);
void questregl_staticinit() {
	questregl_type = typel_new("Quest", 0);
	questregl_type.getattr = questregl_getattr;
	questregl_type.setattr = questregl_setattr;
}
int questregl_getattr(lua_State *state, USER *sd, char *attrname) {
	lua_pushnumber(state, pc_readquestreg(sd, attrname));
	return 1;
}
int questregl_setattr(lua_State *state, USER *sd, char *attrname) {
	pc_setquestreg(sd, attrname, lua_tonumber(state, -1));
	return 1;
}
void typel_staticinit() {
	lua_newtable(sl_gstate); // the global metatable
	lua_pushcfunction(sl_gstate, typel_mtindex);
	lua_setfield(sl_gstate, -2, "__index");
	lua_pushcfunction(sl_gstate, typel_mtnewindex);
	lua_setfield(sl_gstate, -2, "__newindex");
	lua_pushcfunction(sl_gstate, typel_mtgc);
	lua_setfield(sl_gstate, -2, "__gc");
	typel_mtref = luaL_ref(sl_gstate, LUA_REGISTRYINDEX);
	printf("old pause: %d\n", lua_gc(sl_gstate, LUA_GCSETPAUSE, 100));
	printf("old mult: %d\n", lua_gc(sl_gstate, LUA_GCSETSTEPMUL, 1000));
}
typel_class typel_new(char *name, lua_CFunction ctor) {
	typel_class type;
	memset(&type, 0, sizeof(typel_class));
	// TODO: expose the type object, not the prototype, to lua
	lua_newtable(sl_gstate); // the prototype table
	if(ctor) {
		lua_newtable(sl_gstate);
		lua_pushcfunction(sl_gstate, ctor);
		lua_setfield(sl_gstate, -2, "__call");
		lua_setmetatable(sl_gstate, -2);
		type.ctor = ctor;
	}
	type.protoref = luaL_ref(sl_gstate, LUA_REGISTRYINDEX);
	sl_pushref(sl_gstate, type.protoref); // expose the prototype table to lua
	lua_setglobal(sl_gstate, name);
	type.name = name;
	return type;
}
int typel_mtgc(lua_State *state) {


	typel_inst *inst = lua_touserdata(state, 1);
	struct block_list *bl=(struct block_list*)inst->self;
	luaL_unref(state, LUA_REGISTRYINDEX, inst->dataref);

	//FREE(inst->type);
	//FREE(inst);
	return 0;
}
void typel_pushinst(lua_State *state, typel_class *type, void *self, void *param) {
	typel_inst *inst = lua_newuserdata(state, sizeof(typel_inst));
	inst->self = self;
	inst->type = type;
	sl_pushref(state, typel_mtref); // set the metatable
	lua_setmetatable(state, -2);
	lua_newtable(state); // create a new data table and store a ref
	inst->dataref = luaL_ref(state, LUA_REGISTRYINDEX);
	if(type->init && inst->self) { type->init(state, inst->self, inst->dataref, param); }
	//else { luaL_error(state,"dataref no longer exists"); }
}
void typel_extendproto(typel_class *type, char *name, typel_func func) {
	sl_pushref(sl_gstate, type->protoref);
	lua_pushlightuserdata(sl_gstate, func);
	lua_pushcclosure(sl_gstate, typel_boundfunc, 1);
	lua_setfield(sl_gstate, -2, name);
	lua_pop(sl_gstate, 1); // pop the prototype table
}
int typel_boundfunc(lua_State *state) {
	typel_func wrapped = lua_touserdata(state, lua_upvalueindex(1));
	typel_inst *inst = lua_touserdata(state, 1); // the first parameter is the userdata object

	if(inst && inst->self) {
		return wrapped(state, inst->self);
	}
}
int typel_mtindex(lua_State *state) {
	typel_inst *inst = lua_touserdata(state, 1);
	typel_class *type = inst->type;
	char *attrname = lua_tostring(state, 2);
	int result = 0;
	if(!inst->self) { lua_pushnil(state); return 1; }
	if(type->getattr)
		result = type->getattr(state, inst->self, attrname);
	if(!result) {
		// the attribute was not handled by the getattr method
		// next, try the prototype table
		sl_pushref(state, type->protoref);
		lua_getfield(state, -1, attrname);
		if(lua_isnil(state, -1)) {
			lua_pop(state, 2); // clean up the stack
			// then try the data table associated with the instance
			sl_pushref(state, inst->dataref);
			lua_getfield(state, -1, attrname);
			if(lua_isnil(state, -1)) {
				// there is no such attribute; push nil
				lua_pop(state, 1);
				lua_pushnil(state);
			}
		}
		lua_replace(state, -2);
	}
	return 1;
}
int typel_mtnewindex(lua_State *state) {
	typel_inst *inst = lua_touserdata(state, 1);
	typel_class *type = inst->type;
	char *attrname = lua_tostring(state, 2);
	if(!inst->self) { return 0; }
	int result = 0;
	if(type->setattr)
		result = type->setattr(state, inst->self, attrname);
	if(!result) { /* TODO: error */ }

	return 0;
}



int bll_deliddb(lua_State *state, struct block_list *bl) {
	map_deliddb(bl);
	return 0;
}

int bll_playsound(lua_State *state, struct block_list *bl) {
	sl_checkargs(state, "n");
	int sound=lua_tonumber(state,sl_memberarg(1));
	
	clif_playsound(bl,sound);
	return 0;
}
int bll_talk(lua_State *state, struct block_list *bl) {
	sl_checkargs(state, "ns");
	int type=lua_tonumber(state,sl_memberarg(1));
	char *msg = lua_tostring(state, sl_memberarg(2));

	map_foreachinarea(clif_speak, bl->m, bl->x, bl->y, AREA, BL_PC, msg, bl,type);
	return 0;
}
int bll_talkcolor(lua_State *state, struct block_list *bl) {
	sl_checkargs(state, "ns");
	int color=lua_tonumber(state,sl_memberarg(1));
	char *msg = lua_tostring(state, sl_memberarg(2));
	USER *tsd = NULL;
	
	if (lua_isnumber(state,sl_memberarg(3))) {
		sl_checkargs(state, "nsn");
		tsd = map_id2sd(lua_tonumber(state, sl_memberarg(3)));
	} else {
		sl_checkargs(state, "nss");
		tsd = map_name2sd(lua_tostring(state, sl_memberarg(3)));
	}

	if (lua_tonumber(state,sl_memberarg(3)) == 0) {
		//map_foreachinarea(clif_sendmsg, bl->m, bl->x, bl->y, AREA, BL_PC, msg
	} else if (!tsd) {
		return 0;
	} else {
		clif_sendmsg(tsd, color, msg, strlen(msg));
	}
	return 0;
}
int bll_permspawn(lua_State *state, struct block_list *bl) {
	return 0;
}

int bll_sendside(lua_State *state, struct block_list* bl) {
	clif_sendside(bl);
	return 0;
}
int bll_delete(lua_State *state, struct block_list* bl) {
	nullpo_ret(0, bl);
	
	if (bl->type == BL_PC) {
		return 0;
	}
	
	map_delblock(bl);
	map_deliddb(bl);
	
	if (bl->id > 0) {
		clif_lookgone(bl);
		FREE(bl);
	}
	
	return 0;
}
int bll_addnpc(lua_State *state, struct block_list* bl) {
	sl_checkargs(state, "snnn");
	struct npc_data *nd=NULL;
	int timer = 0;
	int duration = 0;
	int owner = 0;
	
	timer = lua_tonumber(state, sl_memberarg(5));
	duration = lua_tonumber(state, sl_memberarg(6));
	owner = lua_tonumber(state, sl_memberarg(7));
	CALLOC(nd,struct npc_data,1);
	strcpy(nd->name, lua_tostring(state,sl_memberarg(1)));
	if (lua_isstring(state, sl_memberarg(8))) {
		strcpy(nd->npc_name, lua_tostring(state, sl_memberarg(8)));
	} else {
		strcpy(nd->npc_name, "nothing");
	}
	nd->bl.type=BL_NPC;
	nd->bl.subtype=2;
	nd->bl.m=lua_tonumber(state,sl_memberarg(2));
	nd->bl.x=lua_tonumber(state,sl_memberarg(3));
	nd->bl.y=lua_tonumber(state,sl_memberarg(4));
	nd->bl.graphic_id=0;
	nd->bl.graphic_color=0;
	nd->bl.id = npc_get_new_npctempid();
	nd->bl.next = NULL;
	nd->bl.prev = NULL;
	nd->actiontime = timer;
	nd->duration = duration;
	nd->owner = owner;
	map_addblock(&nd->bl);
	map_addiddb(&nd->bl);
	
	sl_doscript_blargs(nd->name, "on_spawn", 1, &nd->bl);
	return 0;
}
int bll_dropitem(lua_State* state, struct block_list* bl) {
	sl_checkargs(state, "nn");
	unsigned int id;
	unsigned int amount;
	USER* sd=NULL;
	//printf("Gettop=%d\n",lua_gettop(state));
	if(lua_gettop(state)==4)
		sd=map_id2sd(lua_tonumber(state,4));

	id=lua_tonumber(state,2);
	amount=lua_tonumber(state,3);


	mobdb_dropitem(bl->id, id,amount,bl->m,bl->x,bl->y,sd);
	return 0;
}
int bll_dropitemxy(lua_State* state, struct block_list* bl) {
	sl_checkargs(state, "nnnnn");
	unsigned int id;
	unsigned int amount;
	int map;
	int x;
	int y;
	USER* sd=NULL;
	//printf("Gettop=%d\n",lua_gettop(state));
	if (lua_gettop(state) == 7)
		sd = map_id2sd(lua_tonumber(state, 7));
	
	id = lua_tonumber(state, 2);
	amount = lua_tonumber(state, 3);
	map = lua_tonumber(state, 4);
	x = lua_tonumber(state, 5);
	y = lua_tonumber(state, 6);
	
	mobdb_dropitem(bl->id, id, amount, map, x, y, sd);
	return 0;
}
int bll_handle_mob(struct block_list* bl, va_list ap) {
	MOB* mob=(MOB*)bl;

	if(mob && mob->state==MOB_DEAD && mob->onetime==0) {
		mob_respawn(mob);
	}
	return 0;
}
int bll_respawn(lua_State* state, struct block_list* bl) {
	int m=bl->m;

	map_foreachinarea(bll_handle_mob,m,1,1,SAMEMAP,BL_MOB);
	return 0;
}
int bll_objectcanmove(lua_State* state, struct block_list* bl) {
	sl_checkargs(state, "nnn");
	int x=lua_tonumber(state,sl_memberarg(1));
	int y=lua_tonumber(state,sl_memberarg(2));
	int side=lua_tonumber(state,sl_memberarg(3));
	int canmove=clif_object_canmove(bl->m,x,y,side);

	if(canmove) {
	lua_pushboolean(state,0);
	} else {
	lua_pushboolean(state,1);
	}
	return 1;
}
int bll_objectcanmovefrom(lua_State* state, struct block_list* bl) {
	sl_checkargs(state, "nnn");
	int x=lua_tonumber(state,sl_memberarg(1));
	int y=lua_tonumber(state,sl_memberarg(2));
	int side=lua_tonumber(state,sl_memberarg(3));
	int canmove=clif_object_canmove_from(bl->m,x,y,side);

	if(canmove) {
	lua_pushboolean(state,0);
	} else {
	lua_pushboolean(state,1);
	}
	return 1;
}
int bll_repeatanimation(lua_State* state, struct block_list* bl) {
	sl_checkargs(state, "nn");
	//int x;
	//MOB* mob=NULL;
	//USER* t=NULL;
	//int id=magicdb_id(lua_tostring(state,sl_memberarg(1)));
	int anim=lua_tonumber(state,sl_memberarg(1));
	int duration=lua_tonumber(state,sl_memberarg(2));
	//if(bl->type==BL_MOB) mob=(MOB*)bl;
	//else if(bl->type==BL_PC) t=(USER*)bl;
	//else return 0;
	if(duration > 0) {
		duration /= 1000;
	}
	//if(mob) {
	//	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
	//		if(mob->da[x].duration && mob->da[x].id==id) {
	//			mob->da[x].animation=anim;
				map_foreachinarea(clif_sendanimation,bl->m,bl->x,bl->y,AREA,BL_PC,anim,bl,duration);
	//			return 0;
	//		}
	//	}
	//} else if(t) {
	//	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
	//		if(t->status.dura_aether[x].duration && t->status.dura_aether[x].id==id) {
	//			t->status.dura_aether[x].animation=anim;
	//			map_foreachinarea(clif_sendanimation,bl->m,bl->x,bl->y,AREA,BL_PC,anim,bl,duration);
	//			return 0;
	//		}
	//	}
	//}
	return 0;
}


int bll_sendparcel(lua_State *state, struct block_list *bl) {
	sl_checkargs(state, "nnn");
	int x;
	int pos = -1;
	int newest = -1;
	char escape[255];
	int receiver = lua_tonumber(state, sl_memberarg(1));
	int sender = lua_tonumber(state, sl_memberarg(2));
	unsigned int item = lua_tonumber(state, sl_memberarg(3));
	unsigned int amount = lua_tonumber(state, sl_memberarg(4));
	int owner = lua_tonumber(state, sl_memberarg(5));
	char* engrave = lua_tostring(state, sl_memberarg(6));
	char npcflag = lua_tonumber(state, sl_memberarg(7));
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ParPosition` FROM `Parcels` WHERE `ParChaIdDestination` = '%u'", receiver)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &pos, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (SqlStmt_NumRows(stmt) > 0) {
		for (x = 0; x < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); x++) {
			if (pos > newest) {
				newest = pos;
			}
		}
	}
	
	newest += 1;
	SqlStmt_Free(stmt);
	Sql_EscapeString(sql_handle, escape, engrave);
	
	if (SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `Parcels` (`ParChaIdDestination`, `ParSender`, `ParItmId`, `ParAmount`, `ParChaIdOwner`, `ParEngrave`, `ParPosition`, `ParNpc`) VALUES ('%u', '%u', '%u', '%u', '%u', '%s', '%d', '%d')",
	receiver, sender, item, amount, owner, escape, newest, npcflag)) {
		Sql_ShowDebug(sql_handle);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `SendParcelLogs` (`SpcSender`, `SpcMapId`, `SpcX`, `SpcY`, `SpcItmId`, `SpcAmount`, `SpcChaIdDestination`, `SpcNpc`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u', '%u', '%d')",
	sender, bl->m, bl->x, bl->y, item, amount, receiver, npcflag)) {
		Sql_ShowDebug(sql_handle);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	lua_pushboolean(state, 1);
	return 1;
}

int bll_updatestate(lua_State* state, struct block_list* bl) {
	USER* sd = NULL;
	MOB* mob = NULL;
	NPC* npc = NULL;
	
	if (bl->type == BL_PC) {
		sd = (USER*)bl;
		map_foreachinarea(clif_updatestate, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, sd);
	} else if (bl->type == BL_MOB) {
		mob = (MOB*)bl;
		
		if (mob->data->subtype == 1) {
			map_foreachinarea(clif_cmoblook_sub, mob->bl.m, mob->bl.x, mob->bl.y, AREA, BL_PC, LOOK_SEND, mob);
		} else {
			map_foreachinarea(clif_object_look_sub2, mob->bl.m, mob->bl.x, mob->bl.y, AREA, BL_PC, LOOK_SEND, mob);
		}
	} else if (bl->type == BL_NPC) {
		npc = (NPC*)bl;
		
		if (npc->npctype == 1) {
			map_foreachinarea(clif_cnpclook_sub, npc->bl.m, npc->bl.x, npc->bl.y, AREA, BL_PC, LOOK_SEND, npc);
		} else {
			map_foreachinarea(clif_object_look_sub2, npc->bl.m, npc->bl.x, npc->bl.y, AREA, BL_PC, LOOK_SEND, npc);
		}
	} else {
		lua_pushboolean(state, 0);
		return 0;
	}
	
	lua_pushboolean(state, 1);
	return 1;
}

int bll_removesprite(lua_State* state, struct block_list* bl) {
	clif_lookgone(bl);
	return 1;
}

int bll_getfriends(lua_State *state, struct block_list *bl) {
	int i, id, numFriends;
	USER *sd = (USER *)bl;
	USER *tsd = NULL;
	lua_newtable(state);
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if (stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	if (bl->type != BL_PC)
		return 0;
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `FndChaId` FROM `Friends` JOIN `Character` ON `FndChaId` = `ChaId` AND `ChaOnline` = '1' WHERE `FndChaName1` = '%s' OR `FndChaName2` = '%s' OR `FndChaName3` = '%s' OR `FndChaName4` = '%s'"
		"OR `FndChaName5` = '%s' OR `FndChaName6` = '%s' OR `FndChaName7` = '%s' OR `FndChaName8` = '%s' OR `FndChaName9` = '%s' OR `FndChaName10` = '%s' OR `FndChaName11` = '%s' OR `FndChaName12` = '%s' OR `FndChaName13` = '%s'"
		"OR `FndChaName14` = '%s' OR `FndChaName15` = '%s' OR `FndChaName16` = '%s' OR `FndChaName17` = '%s' OR `FndChaName18` = '%s' OR `FndChaName19` = '%s' OR `FndChaName20` = '%s'", sd->status.name, sd->status.name, sd->status.name,
		sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name, sd->status.name,
		sd->status.name, sd->status.name, sd->status.name)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0 ,SQLDT_UINT, &id, 0, NULL, NULL))
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	numFriends = SqlStmt_NumRows(stmt);
	
	for (i = 1; i <= numFriends && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
	{
		tsd = map_id2sd(id);
		
		if (tsd)
		{
			bll_pushinst(state, &tsd->bl, 0);
			lua_rawseti(state, -2, i);
		}
	}
	
	SqlStmt_Free(stmt);
	return 1;
}

void bll_extendproto(typel_class *class) {
	typel_extendproto(class, "getObjectsInCell", bll_getobjects_cell);
	typel_extendproto(class, "getObjectsInCellWithTraps", bll_getobjects_cell_traps);
	typel_extendproto(class, "getObjectsInArea", bll_getobjects_area);
	typel_extendproto(class, "getObjectsInSameMap", bll_getobjects_samemap);
	typel_extendproto(class, "getObjectsInMap", bll_getobjects_map);
	typel_extendproto(class, "getBlock", bll_getblock);
	//typel_extendproto(class, "checkOnline", bll_checkonline);
	typel_extendproto(class, "sendAnimation",bll_sendanim);
	typel_extendproto(class, "sendAnimationXY",bll_sendanimxy);
	typel_extendproto(class, "getUsers",bll_getusers);
	typel_extendproto(class, "delFromIDDB",bll_deliddb);
	typel_extendproto(class, "playSound",bll_playsound);
	typel_extendproto(class, "sendAction",bll_sendaction);
	typel_extendproto(class, "talk",bll_talk);
	typel_extendproto(class, "msg",bll_talkcolor);
	typel_extendproto(class, "spawn",bll_spawn);
	typel_extendproto(class, "addPermanentSpawn",bll_permspawn);
	typel_extendproto(class, "sendSide",bll_sendside);
	typel_extendproto(class, "delete",bll_delete);
	typel_extendproto(class, "addNPC",bll_addnpc);
	typel_extendproto(class, "dropItem",bll_dropitem);
	typel_extendproto(class, "dropItemXY",bll_dropitemxy);
	typel_extendproto(class,"respawn",bll_respawn);
	typel_extendproto(class,"objectCanMove",bll_objectcanmove);
	typel_extendproto(class,"objectCanMoveFrom",bll_objectcanmovefrom);
	typel_extendproto(class,"repeatAnimation",bll_repeatanimation);
	typel_extendproto(class,"throw",bll_throw);
	typel_extendproto(class,"selfAnimation",bll_selfanimation);
	typel_extendproto(class,"selfAnimationXY",bll_selfanimationxy);
	typel_extendproto(class,"sendParcel",bll_sendparcel);
	typel_extendproto(class,"updateState", bll_updatestate);
	typel_extendproto(class,"removeSprite", bll_removesprite);
	typel_extendproto(class,"getFriends", bll_getfriends);

}
void bll_pushinst(lua_State *state, struct block_list *bl, void *param) {
	if(!bl) {
		lua_pushnil(state);
		return;
	}
	if(bl->type == BL_PC) {
		USER *sd = map_id2sd(bl->id);
		typel_pushinst(state, &pcl_type, sd, param);
	} else if(bl->type == BL_MOB) {
		MOB *mob = (MOB *)bl;
		typel_pushinst(state, &mobl_type, mob, param);
	} else if(bl->type == BL_NPC) {
		struct npc_data *nd = (struct npc_data *)bl;
		typel_pushinst(state, &npcl_type, nd, param);
	} else if(bl->type == BL_ITEM) {
		FLOORITEM *fl = (FLOORITEM*)bl;
		typel_pushinst(state, &fll_type, fl, param);
	} else lua_pushnil(state); // TODO: should this be an error?
}

int bll_getattr(lua_State *state, struct block_list *bl, char *attrname) {
	if(!bl) return 0;
	if(!strcmp(attrname, "x")) lua_pushnumber(state, bl->x);
	else if(!strcmp(attrname, "y")) lua_pushnumber(state, bl->y);
	else if(!strcmp(attrname, "m")) lua_pushnumber(state, bl->m);
	else if(!strcmp(attrname, "xmax")) lua_pushnumber(state, map[bl->m].xs - 1);
	else if(!strcmp(attrname, "ymax")) lua_pushnumber(state, map[bl->m].ys - 1);
	else if(!strcmp(attrname, "blType")) lua_pushnumber(state,bl->type);
	else if(!strcmp(attrname,"ID")) lua_pushnumber(state,bl->id);
	else if(!strcmp(attrname,"mapTitle")) lua_pushstring(state,map[bl->m].title);
	else if(!strcmp(attrname,"bgm")) lua_pushnumber(state,map[bl->m].bgm);
	else if(!strcmp(attrname,"pvp")) lua_pushnumber(state,map[bl->m].pvp);
	else if(!strcmp(attrname,"spell")) lua_pushnumber(state,map[bl->m].spell);
	else if(!strcmp(attrname,"light")) lua_pushnumber(state,map[bl->m].light);
	else if(!strcmp(attrname,"weather")) lua_pushnumber(state,map[bl->m].weather);
	else if(!strcmp(attrname,"sweepTime")) lua_pushnumber(state,map[bl->m].sweeptime);
	else if(!strcmp(attrname,"canTalk")) lua_pushnumber(state,map[bl->m].cantalk);
	else if(!strcmp(attrname,"showGhosts")) lua_pushnumber(state,map[bl->m].show_ghosts);
	else if(!strcmp(attrname,"region")) lua_pushnumber(state,map[bl->m].region);
	else if(!strcmp(attrname,"indoor")) lua_pushnumber(state,map[bl->m].indoor);
	else if(!strcmp(attrname,"warpOut")) lua_pushnumber(state,map[bl->m].warpout);
	else if(!strcmp(attrname,"bind")) lua_pushnumber(state,map[bl->m].bind);
	else return 0; // the attribute was not handled
	return 1;
}
int bll_setattr(lua_State *state, struct block_list *bl, char*attrname) {
	if(!bl) return 0;
	if(!strcmp(attrname,"mapTitle")) strcpy(map[bl->m].title, lua_tostring(state, -1));
	else if(!strcmp(attrname,"bgm")) map[bl->m].bgm=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"pvp")) map[bl->m].pvp=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"spell")) map[bl->m].spell=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"light")) map[bl->m].light=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"weather")) map[bl->m].weather=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"sweepTime")) map[bl->m].sweeptime=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"canTalk")) map[bl->m].cantalk=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"showGhosts")) map[bl->m].show_ghosts=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"region")) map[bl->m].region=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"indoor")) map[bl->m].indoor=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"warpOut")) map[bl->m].warpout=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"bind")) map[bl->m].bind=lua_tonumber(state,-1);
	else return 0;
	return 1;
}
int fll_getattr(lua_State *, FLOORITEM *, char *);
int fll_setattr(lua_State *, FLOORITEM *, char *);
int fll_move(lua_State *, FLOORITEM *);
int fll_init(lua_State *, FLOORITEM *, int, void *);
int fll_ctor(lua_State *);
int fll_getlooters(lua_State*, FLOORITEM*);

void fll_staticinit() {
	fll_type = typel_new("FloorItem", fll_ctor);
	fll_type.getattr = fll_getattr;
	fll_type.setattr = fll_setattr;
	fll_type.init = fll_init;
	
	typel_extendproto(&fll_type, "getLooters", fll_getlooters);
	/*typel_extendproto(&mobl_type, "attack", mobl_attack);
	typel_extendproto(&mobl_type, "addHealth", mobl_addhealth);
	typel_extendproto(&mobl_type, "move",mobl_move);
	typel_extendproto(&mobl_type,"setDuration",mobl_setduration);
	typel_extendproto(&mobl_type,"moveIntent",mobl_moveintent);
	typel_extendproto(&mobl_type, "hasDuration",mobl_hasduration);
	typel_extendproto(&mobl_type,"removeHealth",mobl_removehealth);
	typel_extendproto(&mobl_type,"flushDuration",mobl_flushduration);
	typel_extendproto(&mobl_type,"durationAmount",mobl_durationamount);
	*/
	bll_extendproto(&fll_type);
}
int fll_ctor(lua_State *state) {
	FLOORITEM *fl = NULL;
	if(lua_isnumber(state, 2)) {
		unsigned int id = lua_tonumber(state, 2);
		fl = map_id2fl(id);
		if(!fl) {
			return luaL_error(state, "invalid floor item id (%d)", id);
		}
	} else {
		luaL_argerror(state, 1, "expected number");
	}
	fll_pushinst(state, fl);
	return 1;
}
int fll_init(lua_State *state, FLOORITEM *fl, int dataref, void *param) {
	sl_pushref(state, dataref);

	//biteml_pushinst(state,&fl->data);
	//lua_setfield(state,-2,"data");

	lua_pop(state, 1); // pop the data table
}
int fll_getattr(lua_State *state, FLOORITEM *fl, char* attrname) {
	if(bll_getattr(state, &fl->bl, attrname)) return 1;
	if(!strcmp(attrname, "id")) lua_pushnumber(state, fl->data.id);
	else if(!strcmp(attrname, "amount")) lua_pushnumber(state, fl->data.amount);
	else if(!strcmp(attrname, "lastAmount")) lua_pushnumber(state, fl->lastamount);
	else {
		struct item_data *item = itemdb_search(fl->data.id); // NOTE: this may be slow...?
		assert(item);
		return iteml_getattr(state, item, attrname);
	}
	return 1;
}
int fll_setattr(lua_State *state, FLOORITEM *fl, char* attrname) {
	if(bll_setattr(state,&fl->bl,attrname)) return 1;
	if(!strcmp(attrname, "amount")) fl->data.amount = lua_tonumber(state, -1);
	else return 0;
	return 1;
}
int fll_getlooters(lua_State* state, FLOORITEM* fl) {
	int i = 0;
	lua_newtable(state);
	
	for (i = 0; i < MAX_GROUP_MEMBERS; i++) {
		lua_pushnumber(state, fl->canloot[i]);
		lua_rawseti(state, -2, i + 1);
	}
					
	return 1;
}

int npcl_ctor(lua_State *);
int npcl_init(lua_State *, NPC *, int, void *);
int npcl_getattr(lua_State *, NPC *, char *);
int npcl_setattr(lua_State *, NPC *, char *);
int npcregl_getattr(lua_State *, NPC *, char *);
int npcregl_setattr(lua_State *, NPC *, char *);
int npcl_move(lua_State *, NPC *);
int npcl_warp(lua_State *, NPC *);
int npcl_getequippeditem(lua_State*, NPC*);

void npcl_staticinit() {
	npcl_type = typel_new("NPC", npcl_ctor);
	npcl_type.init = npcl_init;
	npcl_type.getattr = npcl_getattr;
	npcl_type.setattr = npcl_setattr;
	typel_extendproto(&npcl_type, "move", npcl_move);
	typel_extendproto(&npcl_type, "warp", npcl_warp);
	typel_extendproto(&npcl_type, "getEquippedItem", npcl_getequippeditem);
	bll_extendproto(&npcl_type);
}

void npcregl_staticinit() {
	npcregl_type = typel_new("Registry", 0);
	npcregl_type.getattr = npcregl_getattr;
	npcregl_type.setattr = npcregl_setattr;
}

int npcl_ctor(lua_State *state) {
	NPC *nd = NULL;
	if (lua_isnumber(state, 2)) {
		unsigned int id = lua_tonumber(state, 2);
		nd = map_id2npc(id);
		
		if (!nd) {
			lua_pushnil(state);
			return 1; //luaL_error(state, "invalid NPC id (%d)", id);
		}
	} else if (lua_isstring(state, 2)) {
		nd = map_name2npc(lua_tostring(state, 2));
		
		if (!nd) {
			lua_pushnil(state);
			return 1; //luaL_error(state, "invalid NPC id (%d)", id);
		}
	} else {
		luaL_argerror(state, 1, "expected number");
	}
	
	npcl_pushinst(state, nd);
	return 1;
}

int npcl_init(lua_State *state, NPC *nd, int dataref, void *param) {
	sl_pushref(state, dataref);

	npcregl_pushinst(state, nd);
	lua_setfield(state, -2, "registry");
	
	mapregl_pushinst(state, nd);
	lua_setfield(state,-2, "mapRegistry");

	gameregl_pushinst(state, nd);
	lua_setfield(state,-2, "gameRegistry");

	lua_pop(state, 1); // pop the data table
	return 0;
}

int npcl_getattr(lua_State *state, NPC *nd, char *attrname) {
	if(bll_getattr(state, &nd->bl, attrname)) return 1;
	if(!strcmp(attrname, "id")) lua_pushnumber(state, nd->id);
	else if(!strcmp(attrname, "look")) lua_pushnumber(state, nd->bl.graphic_id);
	else if(!strcmp(attrname, "lookColor")) lua_pushnumber(state, nd->bl.graphic_color);
	else if(!strcmp(attrname, "name")) lua_pushstring(state, nd->npc_name);
	else if(!strcmp(attrname, "yname")) lua_pushstring(state, nd->name);
	else if(!strcmp(attrname, "subType")) lua_pushnumber(state, nd->bl.subtype);
	else if(!strcmp(attrname, "npcType")) lua_pushnumber(state, nd->npctype);
	else if(!strcmp(attrname, "side")) lua_pushnumber(state, nd->side);
	else if(!strcmp(attrname, "state")) lua_pushnumber(state, nd->state);
	else if(!strcmp(attrname, "sex")) lua_pushnumber(state, nd->sex);
	else if(!strcmp(attrname, "face")) lua_pushnumber(state, nd->face);
	else if(!strcmp(attrname, "faceColor")) lua_pushnumber(state, nd->face_color);
	else if(!strcmp(attrname, "hair")) lua_pushnumber(state, nd->hair);
	else if(!strcmp(attrname, "hairColor")) lua_pushnumber(state, nd->hair_color);
	else if(!strcmp(attrname, "skinColor")) lua_pushnumber(state, nd->skin_color);
	else if(!strcmp(attrname, "armorColor")) lua_pushnumber(state, nd->armor_color);
	else if(!strcmp(attrname, "lastAction")) lua_pushnumber(state, nd->lastaction);
	else if(!strcmp(attrname, "actionTime")) lua_pushnumber(state, nd->actiontime);
	else if(!strcmp(attrname, "duration")) lua_pushnumber(state, nd->duration);
	else if(!strcmp(attrname, "owner")) lua_pushnumber(state, nd->owner);
	else if(!strcmp(attrname, "startM")) lua_pushnumber(state, nd->startm);
	else if(!strcmp(attrname, "startX")) lua_pushnumber(state, nd->startx);
	else if(!strcmp(attrname, "startY")) lua_pushnumber(state, nd->starty);
	else if(!strcmp(attrname, "gfxFace")) lua_pushnumber(state, nd->gfx.face);
	else if(!strcmp(attrname, "gfxHair")) lua_pushnumber(state, nd->gfx.hair);
	else if(!strcmp(attrname, "gfxHairC")) lua_pushnumber(state, nd->gfx.chair);
	else if(!strcmp(attrname, "gfxFaceC")) lua_pushnumber(state, nd->gfx.cface);
	else if(!strcmp(attrname, "gfxSkinC")) lua_pushnumber(state, nd->gfx.cskin);
	else if(!strcmp(attrname, "gfxDye")) lua_pushnumber(state, nd->gfx.dye);
	else if(!strcmp(attrname, "gfxWeap")) lua_pushnumber(state, nd->gfx.weapon);
	else if(!strcmp(attrname, "gfxWeapC")) lua_pushnumber(state, nd->gfx.cweapon);
	else if(!strcmp(attrname, "gfxArmor")) lua_pushnumber(state, nd->gfx.armor);
	else if(!strcmp(attrname, "gfxArmorC")) lua_pushnumber(state, nd->gfx.carmor);
	else if(!strcmp(attrname, "gfxShield")) lua_pushnumber(state, nd->gfx.shield);
	else if(!strcmp(attrname, "gfxShiedlC")) lua_pushnumber(state, nd->gfx.cshield);
	else if(!strcmp(attrname, "gfxHelm")) lua_pushnumber(state, nd->gfx.helm);
	else if(!strcmp(attrname, "gfxHelmC")) lua_pushnumber(state, nd->gfx.chelm);
	else if(!strcmp(attrname, "gfxCape")) lua_pushnumber(state, nd->gfx.mantle);
	else if(!strcmp(attrname, "gfxCapeC")) lua_pushnumber(state, nd->gfx.cmantle);
	else if(!strcmp(attrname, "gfxCrown")) lua_pushnumber(state, nd->gfx.crown);
	else if(!strcmp(attrname, "gfxCrownC")) lua_pushnumber(state, nd->gfx.ccrown);
	else if(!strcmp(attrname, "gfxFaceA")) lua_pushnumber(state, nd->gfx.faceAcc);
	else if(!strcmp(attrname, "gfxFaceAC")) lua_pushnumber(state, nd->gfx.cfaceAcc);
	else if(!strcmp(attrname, "gfxFaceAT")) lua_pushnumber(state, nd->gfx.faceAccT);
	else if(!strcmp(attrname, "gfxFaceATC")) lua_pushnumber(state, nd->gfx.cfaceAccT);
	else if(!strcmp(attrname, "gfxBoots")) lua_pushnumber(state, nd->gfx.boots);
	else if(!strcmp(attrname, "gfxBootsC")) lua_pushnumber(state, nd->gfx.cboots);
	else if(!strcmp(attrname, "gfxNeck")) lua_pushnumber(state, nd->gfx.necklace);
	else if(!strcmp(attrname, "gfxNeckC")) lua_pushnumber(state, nd->gfx.cnecklace);
	else if(!strcmp(attrname, "gfxName")) lua_pushstring(state, nd->gfx.name);
	else if(!strcmp(attrname, "gfxClone")) lua_pushnumber(state, nd->clone);
	else return 0;
	return 1;
}

int npcl_setattr(lua_State *state, NPC *nd, char *attrname) {
	if (bll_setattr(state, &nd->bl, attrname)) return 1;
	if (!strcmp(attrname, "side")) nd->side = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "subType")) nd->bl.subtype = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "look")) nd->bl.graphic_id = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "lookColor")) nd->bl.graphic_color = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "state")) nd->state = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "sex")) nd->sex = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "face")) nd->face = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "faceColor")) nd->face_color = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "hair")) nd->hair = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "hairColor")) nd->hair_color = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "skinColor")) nd->skin_color = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "armorColor")) nd->armor_color = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "lastAction")) nd->lastaction = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "actionTime")) nd->actiontime = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "duration")) nd->duration = lua_tonumber(state, -1);
	else if(!strcmp(attrname, "owner")) nd->owner = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFace")) nd->gfx.face = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHair")) nd->gfx.hair = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHairC")) nd->gfx.chair = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceC")) nd->gfx.cface = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxSkinC")) nd->gfx.cskin = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxDye")) nd->gfx.dye = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxWeap")) nd->gfx.weapon = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxWeapC")) nd->gfx.cweapon = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxArmor")) nd->gfx.armor = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxArmorC")) nd->gfx.carmor = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxShield")) nd->gfx.shield = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxShieldC")) nd->gfx.cshield = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHelm")) nd->gfx.helm = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHelmC")) nd->gfx.chelm = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCape")) nd->gfx.mantle = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCapeC")) nd->gfx.cmantle = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCrown")) nd->gfx.crown = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCrownC")) nd->gfx.ccrown = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceA")) nd->gfx.faceAcc = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceAC")) nd->gfx.cfaceAcc = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceAT")) nd->gfx.faceAccT = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceATC")) nd->gfx.cfaceAccT = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxBoots")) nd->gfx.boots = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxBootsC")) nd->gfx.cboots = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxNeck")) nd->gfx.necklace = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxNeckC")) nd->gfx.cnecklace = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxName")) strcpy(nd->gfx.name, lua_tostring(state, -1));
	else if(!strcmp(attrname,"gfxClone")) nd->clone = lua_tonumber(state, -1);
	else return 0;
	return 1;
}

int npcregl_getattr(lua_State *state, NPC *nd, char *attrname) {
	lua_pushnumber(state, npc_readglobalreg(nd, attrname));
	return 1;
}

int npcregl_setattr(lua_State *state, NPC *nd, char *attrname) {
	npc_setglobalreg(nd, attrname, lua_tonumber(state, -1));
	return 0;
}

int npcl_move(lua_State *state, NPC *nd) {
	lua_pushnumber(state, npc_move(nd));
	return 1;
}

int npcl_warp(lua_State* state, NPC *nd) {
	sl_checkargs(state, "nnn");
	unsigned short m, x, y;
	
	m = lua_tonumber(state, sl_memberarg(1));
	x = lua_tonumber(state, sl_memberarg(2));
	y = lua_tonumber(state, sl_memberarg(3));
	
	npc_warp(nd, m, x, y);
	return 0;
}

int npcl_getequippeditem(lua_State* state, NPC* nd) {
	sl_checkargs(state,"n");
	int num = lua_tonumber(state, sl_memberarg(1));
	lua_newtable(state);
	
	if (nd->equip[num].id != 0) {
		lua_pushnumber(state, nd->equip[num].id);
		lua_rawseti(state, -2, 1);
		lua_pushnumber(state, nd->equip[num].custom);
		lua_rawseti(state, -2, 2);
	} else {
		lua_pushnil(state);
	}
	
	return 1;
}

int mobl_attack(lua_State *, MOB *);
int mobl_addhealth(lua_State *, MOB *);
int mobl_getattr(lua_State *, MOB *, char *);
int mobl_setattr(lua_State *, MOB *, char *);
int mobl_move(lua_State *, MOB *);
int mobl_init(lua_State *, MOB *, int, void *);
int mobl_ctor(lua_State *);
int mobl_setduration(lua_State *,MOB*);
int mobl_moveintent(lua_State*,MOB*);
int mobl_hasduration(lua_State*, MOB*);
int mobl_hasdurationid(lua_State*, MOB*); //hasduration with id check
int mobl_getduration(lua_State*, MOB*);
int mobl_getdurationid(lua_State*, MOB*);
int mobl_removehealth(lua_State*,MOB*);
int mobl_flushduration(lua_State*,MOB*);
int mobl_flushdurationnouncast(lua_State*,MOB*);
int mobl_durationamount(lua_State*,MOB*);
int mobl_checkthreat(lua_State*, MOB *);
int mobl_sendhealth(lua_State*, MOB*);
int mobl_warp(lua_State*, MOB*);
int mobl_moveghost(lua_State*, MOB*);
int mobl_callbase(lua_State*, MOB*);
int mobl_checkmove(lua_State*, MOB*);
int mobl_setinddmg(lua_State*, MOB*);
int mobl_setgrpdmg(lua_State*, MOB*);
int mobl_getinddmg(lua_State*, MOB*);
int mobl_getgrpdmg(lua_State*, MOB*);
int mobl_getequippeditem(lua_State*, MOB*);
int mobl_calcstat(lua_State*, MOB*);
int mobregl_getattr(lua_State*, MOB*, char*);
int mobregl_setattr(lua_State*, MOB*, char*);

void mobl_staticinit() {
	mobl_type = typel_new("Mob", mobl_ctor);
	mobl_type.getattr = mobl_getattr;
	mobl_type.setattr = mobl_setattr;
	mobregl_type.getattr = mobregl_getattr;
	mobregl_type.setattr = mobregl_setattr;
	mobl_type.init = mobl_init;
	typel_extendproto(&mobl_type,"attack", mobl_attack);
	typel_extendproto(&mobl_type,"addHealth", mobl_addhealth);
	typel_extendproto(&mobl_type,"move",mobl_move);
	typel_extendproto(&mobl_type,"setDuration",mobl_setduration);
	typel_extendproto(&mobl_type,"moveIntent",mobl_moveintent);
	typel_extendproto(&mobl_type,"hasDuration",mobl_hasduration);
	typel_extendproto(&mobl_type,"hasDurationID",mobl_hasdurationid); //hasduration with id check
	typel_extendproto(&mobl_type,"getDuration",mobl_getduration);
	typel_extendproto(&mobl_type,"getDurationID",mobl_getdurationid);
	typel_extendproto(&mobl_type,"removeHealth",mobl_removehealth);
	typel_extendproto(&mobl_type,"flushDuration",mobl_flushduration);
	typel_extendproto(&mobl_type,"flushDurationNoUncast",mobl_flushdurationnouncast);
	typel_extendproto(&mobl_type,"durationAmount",mobl_durationamount);
	typel_extendproto(&mobl_type,"checkThreat",mobl_checkthreat);
	typel_extendproto(&mobl_type,"sendHealth",mobl_sendhealth);
	typel_extendproto(&mobl_type,"warp",mobl_warp);
	typel_extendproto(&mobl_type,"moveGhost",mobl_moveghost);
	typel_extendproto(&mobl_type,"callBase",mobl_callbase);
	typel_extendproto(&mobl_type,"checkMove",mobl_checkmove);
	typel_extendproto(&mobl_type,"setIndDmg",mobl_setinddmg);
	typel_extendproto(&mobl_type,"setGrpDmg",mobl_setgrpdmg);
	typel_extendproto(&mobl_type,"getIndDmg",mobl_getinddmg);
	typel_extendproto(&mobl_type,"getGrpDmg",mobl_getgrpdmg);
	typel_extendproto(&mobl_type,"getEquippedItem",mobl_getequippeditem);
	typel_extendproto(&mobl_type,"calcStat",mobl_calcstat);

	bll_extendproto(&mobl_type);
}
void mobregl_staticinit() {
	mobregl_type = typel_new("Registry", 0);
	mobregl_type.getattr = mobregl_getattr;
	mobregl_type.setattr = mobregl_setattr;
}
int mobl_durationamount(lua_State* state,MOB* mob) {
	sl_checkargs(state, "n");
	int x,id;
	id=magicdb_id(lua_tostring(state,sl_memberarg(1)));

	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
		if(mob->da[x].id==id) {
			if(mob->da[x].duration>0) {
				lua_pushnumber(state,mob->da[x].duration);
				return 1;
			}
		}
	}
	lua_pushnumber(state,0);
	return 1;
}

int mobl_ctor(lua_State *state) {
	MOB *mob = NULL;
	if(lua_isnumber(state, 2)) {
		unsigned int id = lua_tonumber(state, 2);
		mob = map_id2mob(id);
		if(!mob) {
			lua_pushnil(state);
			return 1; //luaL_error(state, "invalid mob id (%d)", id);
		}
	} else {
		luaL_argerror(state, 1, "expected number");
	}
	mobl_pushinst(state, mob);
	return 1;
}
int mobl_removehealth(lua_State *state, MOB *mob) {
	sl_checkargs(state, "n");
	int damage = lua_tonumber(state, sl_memberarg(1));
	int caster = lua_tonumber(state,sl_memberarg(2));
	struct block_list *bl = NULL;
	USER *tsd = NULL;
	MOB *tmob = NULL;
	
	if(caster > 0) {
		bl = map_id2bl(caster);
		mob->attacker = caster;
	} else {
		bl = map_id2bl(mob->attacker);
	}
	
	if (bl != NULL) {
		if(bl->type == BL_PC) {
			tsd=(USER*)bl;
			tsd->damage = damage;
			tsd->critchance = 0;
		} else if(bl->type == BL_MOB) {
			tmob = (MOB*)bl;
			tmob->damage = damage;
			tmob->critchance = 0;
		}
	} else {
		mob->damage = damage;
		mob->critchance = 0;
	}
	
	if(mob->state!=MOB_DEAD) {
		clif_send_mob_healthscript(mob, damage, 0);
	}
	return 0;
}

int mobl_moveintent(lua_State *state, MOB *mob) {
	struct block_list* bl=map_id2bl(lua_tonumber(state, sl_memberarg(1)));
	nullpo_ret(0,bl);
	lua_pushnumber(state,move_mob_intent(mob,bl));
	return 1;
}

int mobl_setduration(lua_State *state,MOB *mob) {
	sl_checkargs(state, "sn");
	struct block_list *bl = NULL;
	int id, time, x, caster_id, recast, alreadycast, mid;
	
	alreadycast = 0;
	id = magicdb_id(lua_tostring(state, sl_memberarg(1)));
	time = lua_tonumber(state, sl_memberarg(2));
	if (time < 1000 && time > 0) time = 1000;
	caster_id = lua_tonumber(state, sl_memberarg(3)); //set duration for specific caster, 0 for universal
	recast = lua_tonumber(state, sl_memberarg(4)); //should the spell recast?

	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
		if(mob->da[x].id == id && mob->da[x].caster_id == caster_id && mob->da[x].duration > 0) {
			alreadycast = 1;
		}
	}
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
		//check for existing spell first
		mid = mob->da[x].id;
		if (mid == id && time <= 0 && mob->da[x].caster_id == caster_id && alreadycast == 1) {
			mob->da[x].duration = 0;
			mob->da[x].id = 0;
			mob->da[x].caster_id = 0;
			map_foreachinarea(clif_sendanimation,mob->bl.m,mob->bl.x,mob->bl.y,AREA,BL_PC,mob->da[x].animation,&mob->bl,-1);
			mob->da[x].animation = 0;
			
			if (mob->da[x].caster_id != mob->bl.id) {
				bl = map_id2bl(mob->da[x].caster_id);
			}

			if(bl != NULL) {
				sl_doscript_blargs(magicdb_yname(mid), "uncast", 2, &mob->bl, bl);
			} else {
				sl_doscript_blargs(magicdb_yname(mid), "uncast", 1, &mob->bl);
			}
			
			return 0;
		} else if (mob->da[x].id == id && mob->da[x].caster_id == caster_id && (mob->da[x].duration > time || recast == 1) && alreadycast == 1) {
			mob->da[x].duration = time;
			return 0;
		} else if (mob->da[x].id == 0 && mob->da[x].duration == 0 && time != 0 && alreadycast != 1) {
			//if (mob->da[x].caster_id != caster_id && caster_id > 0) {
			//	mob->da[x].id = id;
			//	mob->da[x].duration=time;
			//	mob->da[x].caster_id=caster_id; //for specific caster
			//} else if (caster_id == 0) {
			mob->da[x].id = id;
			mob->da[x].duration = time;
			mob->da[x].caster_id = caster_id;
			//}
			
			return 0;
		}
	}
	
	printf("I didn't set duration... id: %u, time: %u, caster: %u, recast: %u, alreadycast: %u\n", id, time, caster_id, recast, alreadycast);
	return 0;
}
/*int mobl_flushduration(lua_State *state, MOB *mob) {
	mob_flushmagic(mob);
	return 0;
}*/

int mobl_flushduration(lua_State* state, MOB* mob) {
	struct block_list *bl = NULL;
	int x;
	int id;
	int dis = lua_tonumber(state, sl_memberarg(1));
	int minid = lua_tonumber(state, sl_memberarg(2));
	int maxid = lua_tonumber(state, sl_memberarg(3));
	char flush = 0;

	if (maxid < minid) {
		maxid = minid;
	}
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
	    id = mob->da[x].id;
		
		if (magicdb_dispel(id) > dis) continue;
		
		if (minid <= 0) {
			flush = 1;
		} else if (minid > 0 && maxid <= 0) {
			if (id == minid) {
				flush = 1;
			} else {
				flush = 0;
			}
		} else {
			if (id >= minid && id <= maxid) {
				flush = 1;
			} else {
				flush = 0;
			}
		}
		
		if (flush == 1) {
			mob->da[x].duration = 0;
			map_foreachinarea(clif_sendanimation, mob->bl.m, mob->bl.x, mob->bl.y, AREA, BL_PC, mob->da[x].animation, &mob->bl, -1);
			mob->da[x].animation = 0;
			mob->da[x].id = 0;
			bl = map_id2bl(mob->da[x].caster_id);
			mob->da[x].caster_id = 0;
			
			if (bl != NULL) {
				sl_doscript_blargs(magicdb_yname(id), "uncast", 2, &mob->bl, bl);
			} else {
				sl_doscript_blargs(magicdb_yname(id), "uncast", 1, &mob->bl);
			}
		}
	}
	
	return 0;
}

int mobl_flushdurationnouncast(lua_State* state, MOB* mob) {
	int x;
	int id;
	int dis = lua_tonumber(state, sl_memberarg(1));
	int minid = lua_tonumber(state, sl_memberarg(2));
	int maxid = lua_tonumber(state, sl_memberarg(3));
	char flush = 0;

	if (maxid < minid) {
		maxid = minid;
	}
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
	    id = mob->da[x].id;
		
		if (magicdb_dispel(id) > dis) continue;
		
		if (minid <= 0) {
			flush = 1;
		} else if (minid > 0 && maxid <= 0) {
			if (id == minid) {
				flush = 1;
			}
		} else {
			if (id >= minid && id <= maxid) {
				flush = 1;
			}
		}
		if (flush == 1) {
			mob->da[x].duration = 0;
			mob->da[x].caster_id = 0;
			map_foreachinarea(clif_sendanimation, mob->bl.m, mob->bl.x, mob->bl.y, AREA, BL_PC, mob->da[x].animation, &mob->bl, -1);
			mob->da[x].animation = 0;
			mob->da[x].id = 0;
		}
	}
	
	return 0;
}

int mobl_hasduration(lua_State *state, MOB* mob) {
	sl_checkargs(state, "s");
	int x,id;
	
	id=magicdb_id(lua_tostring(state,sl_memberarg(1)));
	
	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
		if(mob->da[x].id == id) {
			if(mob->da[x].duration>0) {
				lua_pushboolean(state,1);
				return 1;
			}
		}
	}
	lua_pushboolean(state,0);
	return 1;
}

int mobl_hasdurationid(lua_State *state, MOB* mob) {
	sl_checkargs(state, "sn");
	int x,id,caster_id;
	
	caster_id=0; //zero out

	id=magicdb_id(lua_tostring(state,sl_memberarg(1)));
	caster_id=lua_tonumber(state,sl_memberarg(2)); //check duration from specific caster

	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
		if(mob->da[x].id==id && mob->da[x].caster_id == caster_id) { //add check for appropriate caster
			if(mob->da[x].duration>0) {
				lua_pushboolean(state,1);
				return 1;
			}
		}
	}
	lua_pushboolean(state,0);
	return 1;
}

int mobl_getduration(lua_State* state, MOB* mob) {
	sl_checkargs(state, "s");
	int i, id;

	id = magicdb_id(lua_tostring(state, sl_memberarg(1)));
	
	for (i = 0; i < MAX_MAGIC_TIMERS; i++) {
		if (mob->da[i].id == id) {
			if (mob->da[i].duration > 0) {
				lua_pushnumber(state, mob->da[i].duration);
				return 1;
			}
		}
	}
	
	lua_pushnumber(state,0);
	return 1;
}

int mobl_getdurationid(lua_State* state, MOB* mob) {
	sl_checkargs(state, "sn");
	int i, id, caster_id;
	
	caster_id = 0;
	id = magicdb_id(lua_tostring(state, sl_memberarg(1)));
	caster_id = lua_tonumber(state, sl_memberarg(2));
	
	for (i = 0; i < MAX_MAGIC_TIMERS; i++) {
		if (mob->da[i].id == id && mob->da[i].caster_id == caster_id) {
			if (mob->da[i].duration > 0) {
				lua_pushnumber(state, mob->da[i].duration);
				return 1;
			}
		}
	}
	
	lua_pushnumber(state, 0);
	return 1;
}

int mobl_sendhealth(lua_State *state, MOB *mob) {
	sl_checkargs(state, "nn");
	int damage;
	float dmg = lua_tonumber(state, sl_memberarg(1));
	int critical = lua_tonumber(state, sl_memberarg(2));

	if (dmg > 0) {
		damage = (int)(dmg + 0.5f);
	} else if (dmg < 0) {
		damage = (int)(dmg - 0.5f);
	} else {
		damage = 0;
	}
	
	if(critical == 1) {
		critical = 33;
	} else if (critical == 2) {
		critical = 255;
	}

	clif_send_mob_healthscript(mob, damage, critical);
}

int mobl_init(lua_State *state, MOB *mob, int dataref, void *param) {
	sl_pushref(state, dataref);

	mobregl_pushinst(state, mob);
	lua_setfield(state, -2, "registry");
	
	mapregl_pushinst(state,mob);
	lua_setfield(state,-2,"mapRegistry");

	gameregl_pushinst(state,mob);
	lua_setfield(state,-2,"gameRegistry");

	if(mob->target && param == 0) {
		// the param is used to stop infinite recursion
		/*struct block_list *bl = map_id2bl(mob->target);
		if(bl) {
			bll_pushinst(state, bl, 1);
			lua_setfield(state, -2, "target");
		}*/
	}
	lua_pop(state, 1); // pop the data table
}

int mobl_getattr(lua_State *state, MOB *mob, char *attrname) {
	if(bll_getattr(state, &mob->bl, attrname)) return 1;
	if(!strcmp(attrname,"state")) lua_pushnumber(state,mob->state);
	else if(!strcmp(attrname,"startX")) lua_pushnumber(state,mob->startx);
	else if(!strcmp(attrname,"startY")) lua_pushnumber(state,mob->starty);
	else if(!strcmp(attrname,"startM")) lua_pushnumber(state,mob->startm);
	else if(!strcmp(attrname,"mobID")) lua_pushnumber(state,mob->mobid);
	else if(!strcmp(attrname,"id")) lua_pushnumber(state,mob->id);
	else if(!strcmp(attrname,"behavior")) lua_pushnumber(state,mob->data->type);
	else if(!strcmp(attrname,"aiType")) lua_pushnumber(state,mob->data->subtype);
	else if(!strcmp(attrname,"mobType")) lua_pushnumber(state,mob->data->mobtype);
	else if(!strcmp(attrname,"side")) lua_pushnumber(state,mob->side);
	//else if(!strcmp(attrname,"blType")) lua_pushnumber(state,mob->bl.type);
	else if(!strcmp(attrname,"name")) lua_pushstring(state,mob->data->name);
	else if(!strcmp(attrname,"yname")) lua_pushstring(state, mob->data->yname);
	else if(!strcmp(attrname,"experience")) lua_pushnumber(state, mob->data->exp);
	else if(!strcmp(attrname,"paralyzed")) lua_pushboolean(state,mob->paralyzed);
	else if(!strcmp(attrname,"blind")) lua_pushboolean(state,mob->blind);
	else if(!strcmp(attrname,"hit")) lua_pushnumber(state, mob->hit);
	else if(!strcmp(attrname,"baseHit")) lua_pushnumber(state,mob->data->hit);
	else if(!strcmp(attrname,"miss")) lua_pushnumber(state,mob->miss);
	else if(!strcmp(attrname,"baseMiss")) lua_pushnumber(state,mob->data->miss);
	else if(!strcmp(attrname,"pierce")) lua_pushnumber(state,mob->pierce);
	else if(!strcmp(attrname,"basePierce")) lua_pushnumber(state,mob->data->pierce);
	else if(!strcmp(attrname,"level")) lua_pushnumber(state,mob->data->level);
	else if(!strcmp(attrname,"tier")) lua_pushnumber(state,mob->data->tier);
	else if(!strcmp(attrname,"mark")) lua_pushnumber(state,mob->data->mark);
	else if(!strcmp(attrname,"minDam")) lua_pushnumber(state, mob->mindam);
	else if(!strcmp(attrname,"baseMinDam")) lua_pushnumber(state,mob->data->mindam);
	else if(!strcmp(attrname,"maxDam")) lua_pushnumber(state, mob->maxdam);
	else if(!strcmp(attrname,"baseMaxDam")) lua_pushnumber(state,mob->data->maxdam);
	else if(!strcmp(attrname,"might")) lua_pushnumber(state, mob->might);
	else if(!strcmp(attrname,"baseMight")) lua_pushnumber(state,mob->data->might);
	else if(!strcmp(attrname,"grace")) lua_pushnumber(state, mob->grace);
	else if(!strcmp(attrname,"baseGrace")) lua_pushnumber(state,mob->data->grace);
	else if(!strcmp(attrname,"will")) lua_pushnumber(state, mob->will);
	else if(!strcmp(attrname,"baseWill")) lua_pushnumber(state,mob->data->will);
	else if(!strcmp(attrname,"health")) lua_pushnumber(state,mob->current_vita);
	else if(!strcmp(attrname,"maxHealth")) lua_pushnumber(state, mob->maxvita);
	else if(!strcmp(attrname,"baseHealth")) lua_pushnumber(state, mob->data->vita);
	else if(!strcmp(attrname,"lastHealth")) lua_pushnumber(state,mob->lastvita);
	else if(!strcmp(attrname,"magic")) lua_pushnumber(state,mob->current_mana);
	else if(!strcmp(attrname,"maxMagic")) lua_pushnumber(state, mob->maxmana);
	else if(!strcmp(attrname,"baseMagic")) lua_pushnumber(state, mob->data->mana);
	else if(!strcmp(attrname,"armor")) lua_pushnumber(state,mob->ac);
	else if(!strcmp(attrname,"baseArmor")) lua_pushnumber(state,mob->data->baseac);
	else if(!strcmp(attrname,"attacker")) lua_pushnumber(state,mob->attacker);
	else if(!strcmp(attrname,"confused")) lua_pushboolean(state,mob->confused);
	else if(!strcmp(attrname,"owner")) lua_pushnumber(state,mob->owner);
	else if(!strcmp(attrname,"sleep")) lua_pushnumber(state,mob->sleep);
	else if(!strcmp(attrname,"target")) lua_pushnumber(state,mob->target);
	else if(!strcmp(attrname,"confuseTarget")) lua_pushnumber(state,mob->confused_target);
	else if(!strcmp(attrname,"deduction")) lua_pushnumber(state,mob->deduction);
	else if(!strcmp(attrname,"sound")) lua_pushnumber(state,mob->data->sound);
	else if(!strcmp(attrname,"damage")) lua_pushnumber(state,mob->damage);
	else if(!strcmp(attrname,"crit")) lua_pushnumber(state,mob->crit);
	else if(!strcmp(attrname,"critChance")) lua_pushnumber(state,mob->critchance);
	else if(!strcmp(attrname,"critMult")) lua_pushnumber(state, mob->critmult);
	else if(!strcmp(attrname,"rangeTarget")) lua_pushnumber(state,mob->rangeTarget);
	else if(!strcmp(attrname,"newMove")) lua_pushnumber(state,mob->newmove);
	else if(!strcmp(attrname,"baseMove")) lua_pushnumber(state,mob->data->movetime);
	else if(!strcmp(attrname,"newAttack")) lua_pushnumber(state,mob->newatk);
	else if(!strcmp(attrname,"baseAttack")) lua_pushnumber(state,mob->data->atktime);
	else if(!strcmp(attrname,"spawnTime")) lua_pushnumber(state, mob->data->spawntime);
	else if(!strcmp(attrname,"snare")) lua_pushboolean(state,mob->snare);
	else if(!strcmp(attrname,"lastAction")) lua_pushnumber(state,mob->lastaction);
	else if(!strcmp(attrname,"summon")) lua_pushboolean(state,mob->summon);
	else if(!strcmp(attrname,"dodge")) lua_pushnumber(state, mob->dodge);
	else if(!strcmp(attrname,"baseDodge")) lua_pushnumber(state,mob->data->dodge);
	else if(!strcmp(attrname,"parry")) lua_pushnumber(state, mob->parry);
	else if(!strcmp(attrname,"baseParry")) lua_pushnumber(state,mob->data->parry);
	else if(!strcmp(attrname,"block")) lua_pushnumber(state, mob->block);
	else if(!strcmp(attrname,"baseBlock")) lua_pushnumber(state,mob->data->block);
	else if(!strcmp(attrname,"protection")) lua_pushnumber(state, mob->protection);
	else if(!strcmp(attrname,"baseProtection")) lua_pushnumber(state, mob->data->protection);
	else if(!strcmp(attrname,"resist")) lua_pushnumber(state, mob->resist);
	else if(!strcmp(attrname,"baseResist")) lua_pushnumber(state, mob->data->resist);
	else if(!strcmp(attrname,"retDist")) lua_pushnumber(state, mob->data->retdist);
	else if(!strcmp(attrname,"returning")) lua_pushboolean(state, mob->returning);
	else if(!strcmp(attrname,"bossType")) lua_pushnumber(state, mob->data->bosstype);
	else if(!strcmp(attrname,"race")) lua_pushnumber(state, mob->data->race);
	else if(!strcmp(attrname,"undead")) lua_pushboolean(state, mob->data->undead);
	else if(!strcmp(attrname,"dmgShield")) lua_pushnumber(state, mob->dmgshield);
	else if(!strcmp(attrname,"physDeduct")) lua_pushnumber(state, mob->physdeduct);
	else if(!strcmp(attrname,"basePhysDeduct")) lua_pushnumber(state, mob->data->physdeduct);
	else if(!strcmp(attrname,"dmgDealt")) lua_pushnumber(state, mob->dmgdealt);
	else if(!strcmp(attrname,"dmgTaken")) lua_pushnumber(state, mob->dmgtaken);
	else if(!strcmp(attrname,"seeInvis")) lua_pushnumber(state, mob->data->seeinvis);
	else if(!strcmp(attrname,"look")) lua_pushnumber(state, mob->look);
	else if(!strcmp(attrname,"lookColor")) lua_pushnumber(state, mob->look_color);
	else if(!strcmp(attrname,"charState")) lua_pushnumber(state, mob->charstate);
	else if(!strcmp(attrname,"invis")) lua_pushnumber(state, mob->invis);
	else if(!strcmp(attrname, "gfxFace")) lua_pushnumber(state, mob->gfx.face);
	else if(!strcmp(attrname, "gfxHair")) lua_pushnumber(state, mob->gfx.hair);
	else if(!strcmp(attrname, "gfxHairC")) lua_pushnumber(state, mob->gfx.chair);
	else if(!strcmp(attrname, "gfxFaceC")) lua_pushnumber(state, mob->gfx.cface);
	else if(!strcmp(attrname, "gfxSkinC")) lua_pushnumber(state, mob->gfx.cskin);
	else if(!strcmp(attrname, "gfxDye")) lua_pushnumber(state, mob->gfx.dye);
	else if(!strcmp(attrname, "gfxWeap")) lua_pushnumber(state, mob->gfx.weapon);
	else if(!strcmp(attrname, "gfxWeapC")) lua_pushnumber(state, mob->gfx.cweapon);
	else if(!strcmp(attrname, "gfxArmor")) lua_pushnumber(state, mob->gfx.armor);
	else if(!strcmp(attrname, "gfxArmorC")) lua_pushnumber(state, mob->gfx.carmor);
	else if(!strcmp(attrname, "gfxShield")) lua_pushnumber(state, mob->gfx.shield);
	else if(!strcmp(attrname, "gfxShiedlC")) lua_pushnumber(state, mob->gfx.cshield);
	else if(!strcmp(attrname, "gfxHelm")) lua_pushnumber(state, mob->gfx.helm);
	else if(!strcmp(attrname, "gfxHelmC")) lua_pushnumber(state, mob->gfx.chelm);
	else if(!strcmp(attrname, "gfxCape")) lua_pushnumber(state, mob->gfx.mantle);
	else if(!strcmp(attrname, "gfxCapeC")) lua_pushnumber(state, mob->gfx.cmantle);
	else if(!strcmp(attrname, "gfxCrown")) lua_pushnumber(state, mob->gfx.crown);
	else if(!strcmp(attrname, "gfxCrownC")) lua_pushnumber(state, mob->gfx.ccrown);
	else if(!strcmp(attrname, "gfxFaceA")) lua_pushnumber(state, mob->gfx.faceAcc);
	else if(!strcmp(attrname, "gfxFaceAC")) lua_pushnumber(state, mob->gfx.cfaceAcc);
	else if(!strcmp(attrname, "gfxFaceAT")) lua_pushnumber(state, mob->gfx.faceAccT);
	else if(!strcmp(attrname, "gfxFaceATC")) lua_pushnumber(state, mob->gfx.cfaceAccT);
	else if(!strcmp(attrname, "gfxBoots")) lua_pushnumber(state, mob->gfx.boots);
	else if(!strcmp(attrname, "gfxBootsC")) lua_pushnumber(state, mob->gfx.cboots);
	else if(!strcmp(attrname, "gfxNeck")) lua_pushnumber(state, mob->gfx.necklace);
	else if(!strcmp(attrname, "gfxNeckC")) lua_pushnumber(state, mob->gfx.cnecklace);
	else if(!strcmp(attrname, "gfxName")) lua_pushstring(state, mob->gfx.name);
	else if(!strcmp(attrname, "gfxClone")) lua_pushnumber(state, mob->clone);
	else if(!strcmp(attrname, "lastDeath")) lua_pushnumber(state, mob->last_death);
	//else if(!strcmp(attrname,"graphic"
	else return 0;

	return 1;

}
int mobl_setattr(lua_State *state, MOB *mob, char* attrname) {
	if(bll_setattr(state,&mob->bl,attrname)) return 1;
	if(!strcmp(attrname,"side")) mob->side=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"time")) mob->time=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"paralyzed")) mob->paralyzed=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"blind")) mob->blind=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"hit")) mob->hit = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"minDam")) mob->mindam = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"maxDam")) mob->maxdam = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"might")) mob->might = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"grace")) mob->grace = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"will")) mob->will = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"health")) mob->current_vita=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"maxHealth")) mob->maxvita = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"magic")) mob->current_mana=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"maxMagic")) mob->maxmana = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"lastHealth")) mob->lastvita=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"armor")) mob->ac=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"miss")) mob->miss=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"pierce")) mob->pierce=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"attacker")) mob->attacker=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"confused")) mob->confused=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"owner")) mob->owner=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"sleep")) mob->sleep=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"target")) mob->target=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"confuseTarget")) mob->confused_target=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"deduction")) mob->deduction=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"state")) mob->state=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"rangeTarget")) mob->rangeTarget=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"newMove")) mob->newmove=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"newAttack")) mob->newatk=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"snare")) mob->snare=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"lastAction")) mob->lastaction=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"crit")) mob->crit=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"critChance")) mob->critchance=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"critMult")) mob->critmult=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"damage")) mob->damage=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"summon")) mob->summon=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"dodge")) mob->dodge = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"parry")) mob->parry = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"block")) mob->block = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"protection")) mob->protection = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"resist")) mob->resist=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"returning")) mob->returning=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"dmgShield")) mob->dmgshield=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"physDeduct")) mob->physdeduct = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"dmgDealt")) mob->dmgdealt=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"dmgTaken")) mob->dmgtaken=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"look")) mob->look = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"lookColor")) mob->look_color = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFace"))mob->gfx.face=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHair"))mob->gfx.hair=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHairC"))mob->gfx.chair=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceC"))mob->gfx.cface=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxSkinC"))mob->gfx.cskin=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxDye"))mob->gfx.dye=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxWeap"))mob->gfx.weapon=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxWeapC"))mob->gfx.cweapon=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxArmor"))mob->gfx.armor=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxArmorC"))mob->gfx.carmor=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxShield"))mob->gfx.shield=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxShieldC"))mob->gfx.cshield=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHelm"))mob->gfx.helm=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHelmC"))mob->gfx.chelm=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCape"))mob->gfx.mantle=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCapeC"))mob->gfx.cmantle=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCrown"))mob->gfx.crown=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCrownC"))mob->gfx.ccrown=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceA"))mob->gfx.faceAcc=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceAC"))mob->gfx.cfaceAcc=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceAT"))mob->gfx.faceAccT=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceATC"))mob->gfx.cfaceAccT=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxBoots"))mob->gfx.boots=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxBootsC"))mob->gfx.cboots=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxNeck"))mob->gfx.necklace=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxNeckC"))mob->gfx.cnecklace=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxName"))strcpy(mob->gfx.name, lua_tostring(state, -1));
	else if(!strcmp(attrname,"gfxClone"))mob->clone=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"charState"))mob->charstate=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"invis"))mob->invis=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"lastDeath"))mob->last_death=lua_tonumber(state, -1);
	else return 0;

	return 1;
}

int mobl_attack(lua_State *state, MOB *mob) {
	sl_checkargs(state, "n");
	int id = lua_tonumber(state, sl_memberarg(1));
	mob_attack(mob, id);
	return 0;
}
int mobl_addhealth(lua_State *state, MOB *mob) {
	sl_checkargs(state, "n");
	int damage = lua_tonumber(state, sl_memberarg(1));
	struct block_list *bl = map_id2bl(mob->attacker);
	
	if (bl != NULL) {
		if (damage > 0) {
			if(mob->data->subtype==0) {
				sl_doscript_blargs("mob_ai_basic", "on_healed", 2, &mob->bl, bl);
			} else if(mob->data->subtype == 1) {
				sl_doscript_blargs("mob_ai_normal", "on_healed", 2, &mob->bl, bl);
			} else if(mob->data->subtype == 2) {
				sl_doscript_blargs("mob_ai_hard", "on_healed", 2, &mob->bl, bl);
			} else if(mob->data->subtype == 3) {
				sl_doscript_blargs("mob_ai_boss", "on_healed", 2, &mob->bl, bl);
			} else if(mob->data->subtype == 4) {
				sl_doscript_blargs(mob->data->yname, "on_healed", 2, &mob->bl, bl);
			} else if(mob->data->subtype == 5) {
				sl_doscript_blargs("mob_ai_ghost", "on_healed", 2, &mob->bl, bl);
			}
		}
	} else {
		if (damage > 0) {
			if(mob->data->subtype==0) {
				sl_doscript_blargs("mob_ai_basic", "on_healed", 1, &mob->bl);
			} else if(mob->data->subtype == 1) {
				sl_doscript_blargs("mob_ai_normal", "on_healed", 1, &mob->bl);
			} else if(mob->data->subtype == 2) {
				sl_doscript_blargs("mob_ai_hard", "on_healed", 1, &mob->bl);
			} else if(mob->data->subtype == 3) {
				sl_doscript_blargs("mob_ai_boss", "on_healed", 1, &mob->bl);
			} else if(mob->data->subtype == 4) {
				sl_doscript_blargs(mob->data->yname, "on_healed", 1, &mob->bl);
			} else if(mob->data->subtype == 5) {
				sl_doscript_blargs("mob_ai_ghost", "on_healed", 1, &mob->bl);
			}
		}
	}
	
	clif_send_mob_healthscript(mob, -damage, 0);
	return 0;
}

int mobl_move(lua_State *state, MOB *mob) {
	lua_pushboolean(state,move_mob(mob));
	return 1;
}
//check threat
int mobl_checkthreat(lua_State* state, MOB *mob) {
	sl_checkargs(state, "n");
	unsigned int id;
	int x;
	USER *tsd = NULL;

	id = lua_tonumber(state, sl_memberarg(1));

	tsd = map_id2sd(id);
	for(x=0;x<MAX_THREATCOUNT;x++) {
		if(mob->threat[x].user == tsd->bl.id) {
			lua_pushnumber(state,mob->threat[x].amount);
			return 1;
		}
	}

	lua_pushnumber(state,0);
	return 1;
}

int mobl_warp(lua_State* state, MOB *mob) {
	sl_checkargs(state, "nnn");
	unsigned short m, x, y;
	
	m = lua_tonumber(state, sl_memberarg(1));
	x = lua_tonumber(state, sl_memberarg(2));
	y = lua_tonumber(state, sl_memberarg(3));
	
	mob_warp(mob, m, x, y);
	return 0;
}

int mobl_moveghost(lua_State* state, MOB* mob) {
	lua_pushnumber(state, moveghost_mob(mob));
	return 1;
}

int mobl_callbase(lua_State* state, MOB* mob) {
	sl_checkargs(state, "s");
	char* script = lua_tostring(state, sl_memberarg(1));
	struct block_list* bl = map_id2bl(mob->attacker);
	
	if (bl != NULL) {
		sl_doscript_blargs(mob->data->yname, script, 2, &mob->bl, bl);
		lua_pushboolean(state, 1);
		return 1;
	} else {
		sl_doscript_blargs(mob->data->yname, script, 2, &mob->bl, &mob->bl);
		lua_pushboolean(state, 1);
		return 1;
	}
	
	lua_pushboolean(state, 0);
	return 1;
}

int mobl_checkmove(lua_State* state, MOB* mob) {
	char direction;
	short backx;
	short backy;
	short dx;
	short dy;
	unsigned short m;
	char c = 0;
	struct warp_list* i = NULL;
	
	m = mob->bl.m;
	backx = mob->bl.x;
	backy = mob->bl.y;
	dx = backx;
	dy = backy;
	direction = mob->side;
	
	switch(direction) {
		case 0:
			dy -= 1;
			break;
		case 1:
			dx += 1;
			break;
		case 2:
			dy += 1;
			break;
		case 3:
			dx -= 1;
			break;
	}

	if (dx >= map[m].xs) dx = map[m].xs - 1;
	if (dy >= map[m].ys) dy = map[m].ys - 1;
	for (i = map[m].warp[dx / BLOCK_SIZE + (dy / BLOCK_SIZE) * map[m].bxs]; i && !c; i = i->next) {
		if (i->x == dx && i->y == dy) {
			c = 1;
			lua_pushboolean(state, 0);
			return 1;
		}
	}

	map_foreachincell(mob_move, m, dx, dy, BL_MOB, mob);
	map_foreachincell(mob_move, m, dx, dy, BL_PC, mob);
	map_foreachincell(mob_move, m, dx, dy, BL_NPC, mob);
	
	if (clif_object_canmove(m, dx, dy, direction)) {
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (clif_object_canmove_from(m, backx, backy, direction)) {
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if(map_canmove(m, dx, dy) == 1 || mob->canmove == 1) {
		lua_pushboolean(state, 0);
		return 1;
	}
	
	lua_pushboolean(state, 1);
	return 1;
}

int mobl_getgrpdmg(lua_State* state, MOB* mob) {
	int x, y;
	lua_newtable(state);
	
	for (x = 0, y = 1; x < MAX_THREATCOUNT; x++, y += 2) {
		if (mob->dmggrptable[x][0] > 0) {
			lua_pushnumber(state, mob->dmggrptable[x][0]);
			lua_rawseti(state, -2, y);
			lua_pushnumber(state, mob->dmggrptable[x][1]);
			lua_rawseti(state, -2, y + 1);
		}
	}
	
	return 1;
}

int mobl_getinddmg(lua_State* state, MOB* mob) {
	int x, y;
	lua_newtable(state);
	
	for (x = 0, y = 1; x < MAX_THREATCOUNT; x++, y += 2) {
		if (mob->dmgindtable[x][0] > 0) {
			lua_pushnumber(state, mob->dmgindtable[x][0]);
			lua_rawseti(state, -2, y);
			lua_pushnumber(state, mob->dmgindtable[x][1]);
			lua_rawseti(state, -2, y + 1);
		}
	}
	
	return 1;
}

int mobl_setgrpdmg(lua_State* state, MOB* mob) {
	int x;
	USER* sd = map_id2sd(lua_tonumber(state, sl_memberarg(1)));
	float dmg = lua_tonumber(state, sl_memberarg(2));
	
	if (sd == NULL) {
		lua_pushboolean(state, 0);
		return 1;
	}

	for (x = 0; x < MAX_THREATCOUNT; x++) {
		if (mob->dmggrptable[x][0] == sd->groupid || mob->dmggrptable[x][0] == 0) {
			mob->dmggrptable[x][0] = sd->groupid;
			mob->dmggrptable[x][1] += dmg;
			lua_pushboolean(state, 1);
			return 1;
		}
	}
	
	lua_pushboolean(state, 0);
	return 1;
}

int mobl_setinddmg(lua_State* state, MOB* mob) {
	int x;
	USER* sd = map_id2sd(lua_tonumber(state, sl_memberarg(1)));
	float dmg = lua_tonumber(state, sl_memberarg(2));
	
	if (sd == NULL) {
		lua_pushboolean(state, 0);
		return 1;
	}

	for (x = 0; x < MAX_THREATCOUNT; x++) {
		if (mob->dmgindtable[x][0] == sd->status.id || mob->dmgindtable[x][0] == 0) {
			mob->dmgindtable[x][0] = sd->status.id;
			mob->dmgindtable[x][1] += dmg;
			lua_pushboolean(state, 1);
			return 1;
		}
	}
	
	lua_pushboolean(state, 0);
	return 1;
}

int mobl_getequippeditem(lua_State* state, MOB* mob) {
	sl_checkargs(state,"n");
	int num = lua_tonumber(state, sl_memberarg(1));
	lua_newtable(state);
	
	if (mob->data->equip[num].id != 0) {
		lua_pushnumber(state, mob->data->equip[num].id);
		lua_rawseti(state, -2, 1);
		lua_pushnumber(state, mob->data->equip[num].custom);
		lua_rawseti(state, -2, 2);
	} else {
		lua_pushnil(state);
	}
	
	return 1;
}

int mobl_calcstat(lua_State* state, MOB* mob) {
	mob_calcstat(mob);
	return 0;
}

int mobregl_getattr(lua_State* state, MOB* mob, char* attrname) {
	lua_pushnumber(state, mob_readglobalreg(mob, attrname));
}

int mobregl_setattr(lua_State* state, MOB* mob, char* attrname) {
	mob_setglobalreg(mob, attrname, lua_tonumber(state, -1));
}

int pcl_addhealth(lua_State *, USER *);
int pcl_removehealth(lua_State *, USER *);
int pcl_resurrect(lua_State *, USER *);
int pcl_warp(lua_State *, USER *);
int pcl_useitem(lua_State *, USER *);
int pcl_level(lua_State *, USER *);
int pcl_sendminitext(lua_State*,USER *);
int pcl_setduration(lua_State*,USER *);
int pcl_dialog(lua_State *, USER *);
int pcl_getattr(lua_State *, USER *, char *);
int pcl_setattr(lua_State *, USER *, char *);
int pcl_setaether(lua_State *, USER *);
int pcl_hasaether(lua_State *, USER *);
int pcl_getaether(lua_State *, USER *);
int pcl_getallaethers(lua_State*, USER*);
int pcl_hasduration(lua_State *, USER *);
int pcl_hasdurationid(lua_State *, USER *);
int pcl_getduration(lua_State *, USER *);
int pcl_getdurationid(lua_State *, USER *);
int pcl_getalldurations(lua_State *, USER *);
int pcl_addlegend(lua_State *, USER *);
int pcl_haslegend(lua_State *, USER *);
int pcl_getlegend(lua_State *, USER *);
int pcl_buy(lua_State *, USER *);
int pcl_input(lua_State *, USER *);
int pcl_sell(lua_State *,USER *);
int pcl_sell2(lua_State *,USER *);
int pcl_sendstatus(lua_State*,USER *);
int pcl_calcstat(lua_State*,USER*);
int pcl_getinventoryitem(lua_State*,USER *);
int pcl_getequippeditem(lua_State*,USER *);
int pcl_removelegendbyname(lua_State*,USER *);
int pcl_removelegendbycolor(lua_State*,USER *);
int pcl_menu(lua_State *, void*);
int pcl_showhealth(lua_State *, USER *);
int pcl_addclan(lua_State *, USER *);
int pcl_ctor(lua_State *);
int pcl_talkself(lua_State *, USER *);
int pcl_hasequipped(lua_State*, USER *);
int pcl_killcount(lua_State*, USER *);
int pcl_killrank(lua_State*,USER *);
int pcl_flushkills(lua_State*, USER *);
int pcl_removeinventoryitem(lua_State*, USER*);
int pcl_removeitemslot(lua_State*, USER*);
int pcl_removeitemdura(lua_State*, USER*);
int pcl_additem(lua_State*,USER*);
int pcl_init(lua_State *, USER *, int, void *);
int pcl_showboard(lua_State *, USER*);
int pcl_showpost(lua_State*, USER*);
int pcl_hasitem(lua_State*, USER*);
int pcl_hasitemdura(lua_State*, USER*);
int pcl_addspell(lua_State*, USER*);
int pcl_removespell(lua_State*, USER*);
int pcl_mapselect(lua_State*, USER*);
int pcl_hasspell(lua_State*, USER*);
int pcl_getspells(lua_State*, USER*); //for remove spells
int pcl_getspellname(lua_State*, USER*); //for remove spells
int pcl_getspellyname(lua_State*, USER*); //for remove spells
int pcl_flushduration(lua_State*,USER*);
int pcl_flushdurationnouncast(lua_State*,USER*);
//int pcl_flushdurationrange(lua_State *, USER *);
int pcl_flushaether(lua_State *, USER *);
int pcl_hasspace(lua_State*,USER*);
int pcl_deductarmor(lua_State*,USER*);
int pcl_updateinv(lua_State*,USER*);
int pcl_durationamount(lua_State*,USER*);
int pcl_addguide(lua_State*,USER*);
int pcl_delguide(lua_State*,USER*);
int pcl_popup(lua_State*,USER*);
int pcl_menuseq(lua_State*,USER*);
int pcl_powerboard(lua_State*, USER*);
int pcl_lock(lua_State*, USER*);
int pcl_unlock(lua_State*, USER*);
int pcl_swing(lua_State*, USER*);
int pcl_swingtarget(lua_State*, USER*);
int pcl_checkbankitems(lua_State*, USER*);
int pcl_checkbankamounts(lua_State*, USER*);
int pcl_checkbankowners(lua_State*, USER*);
int pcl_checkbankengraves(lua_State*, USER*);
int pcl_bankdeposit(lua_State*, USER*);
int pcl_bankwithdraw(lua_State*, USER*);
int pcl_checkbankitemamount(lua_State*, USER*);
int pcl_givexp(lua_State*, USER*);
int pcl_givexpgroup(lua_State *, USER *);
int pcl_addthreat(lua_State*, USER*); //threat table
int pcl_setthreat(lua_State*, USER*); //threat table
int pcl_addthreatgeneral(lua_State*, USER*); //threat table
int pcl_getclanitems(lua_State*, USER*);
int pcl_getclanamounts(lua_State*, USER*);
int pcl_clanbankdeposit(lua_State*, USER*);
int pcl_clanbankwithdraw(lua_State*, USER*);
int pcl_checkclanitemamounts(lua_State*, USER*);
int pcl_speak(lua_State*, USER*);
int pcl_freeasync(lua_State*, USER*);
int pcl_sendhealth(lua_State*, USER*);
int pcl_sendmail(lua_State*, USER*);
int pcl_pickup(lua_State*, USER*);
int pcl_equip(lua_State*, USER*);
int pcl_takeoff(lua_State*, USER*);
int pcl_die(lua_State*, USER*);
int pcl_throwitem(lua_State*, USER*);
int pcl_refresh(lua_State*, USER*);
int pcl_move(lua_State*, USER*);
int pcl_respawn(lua_State*, USER*);
int pcl_deductdura(lua_State*, USER*);
int pcl_deductdurainv(lua_State*, USER*);
int pcl_setpk(lua_State*, USER*);
int pcl_getpk(lua_State*, USER*);
int pcl_broadcast(lua_State*, USER*);
int pcl_getcreationitems(lua_State*, USER*);
int pcl_getcreationamounts(lua_State*, USER*);
int pcl_getparcel(lua_State*, USER*);
int pcl_getparcellist(lua_State*, USER*);
int pcl_removeparcel(lua_State*, USER*);
int pcl_expireitem(lua_State*, USER*);
int pcl_logbuysell(lua_State*, USER*);
int pcl_settimevalues(lua_State*, USER*);
int pcl_gettimevalues(lua_State*, USER*);
int pcl_lookat(lua_State*, USER*);
int pcl_getunknownspells(lua_State*, USER*);
int pcl_status(lua_State*, USER*);
int pcl_testpacket(lua_State*, USER*);
int pcl_getcasterid(lua_State*, USER*);
int pcl_changeview(lua_State*, USER*);
int pcl_settimer(lua_State*, USER*);
int pcl_addtime(lua_State*, USER*);
int pcl_removetime(lua_State*, USER*);

void pcl_staticinit() {
	pcl_type = typel_new("Player", pcl_ctor);
	pcl_type.getattr = pcl_getattr;
	pcl_type.setattr = pcl_setattr;
	pcl_type.init = pcl_init;
	typel_extendproto(&pcl_type, "freeAsync", pcl_freeasync);
	typel_extendproto(&pcl_type, "showHealth",pcl_showhealth);
	typel_extendproto(&pcl_type, "addHealth", pcl_addhealth);
	typel_extendproto(&pcl_type, "removeHealth", pcl_removehealth);
	typel_extendproto(&pcl_type, "sendHealth", pcl_sendhealth);
	typel_extendproto(&pcl_type, "die", pcl_die);
	typel_extendproto(&pcl_type, "resurrect", pcl_resurrect);
	typel_extendproto(&pcl_type, "setTimeValues", pcl_settimevalues);
	typel_extendproto(&pcl_type, "getTimeValues", pcl_gettimevalues);
	typel_extendproto(&pcl_type, "setPK", pcl_setpk);
	typel_extendproto(&pcl_type, "getPK", pcl_getpk);
	typel_extendproto(&pcl_type, "status", pcl_status);
	typel_extendproto(&pcl_type, "sendStatus",pcl_sendstatus);
	typel_extendproto(&pcl_type, "calcStat",pcl_calcstat);
	typel_extendproto(&pcl_type, "lookAt", pcl_lookat);
    typel_extendproto(&pcl_type, "giveXP",pcl_givexp);
	typel_extendproto(&pcl_type, "giveXPGroup", pcl_givexpgroup);
	typel_extendproto(&pcl_type, "sendMinitext",pcl_sendminitext);
	typel_extendproto(&pcl_type, "speak", pcl_speak);
	typel_extendproto(&pcl_type, "talkSelf", pcl_talkself);
	typel_extendproto(&pcl_type, "broadcast", pcl_broadcast);
	typel_extendproto(&pcl_type, "sendMail", pcl_sendmail);
	typel_extendproto(&pcl_type, "showBoard",pcl_showboard);
	typel_extendproto(&pcl_type, "showPost", pcl_showpost);
	typel_extendproto(&pcl_type, "refresh",pcl_refresh);
	typel_extendproto(&pcl_type, "popUp",pcl_popup);
	typel_extendproto(&pcl_type, "input",pcl_input);
	typel_extendproto(&pcl_type, "dialog", pcl_dialog);
	typel_extendproto(&pcl_type, "menu", pcl_menu);
	typel_extendproto(&pcl_type, "menuSeq",pcl_menuseq);
	typel_extendproto(&pcl_type, "buy",pcl_buy);
	typel_extendproto(&pcl_type, "sell",pcl_sell);
	typel_extendproto(&pcl_type, "sell2",pcl_sell2);
	typel_extendproto(&pcl_type, "logBuySell",pcl_logbuysell);
	typel_extendproto(&pcl_type, "setDuration",pcl_setduration);
	typel_extendproto(&pcl_type, "hasDuration",pcl_hasduration);
	typel_extendproto(&pcl_type, "hasDurationID",pcl_hasdurationid);
	typel_extendproto(&pcl_type, "getDuration",pcl_getduration);
	typel_extendproto(&pcl_type, "getDurationID",pcl_getdurationid);
	typel_extendproto(&pcl_type, "getAllDurations", pcl_getalldurations);
	typel_extendproto(&pcl_type, "flushDuration",pcl_flushduration);
	//typel_extendproto(&pcl_type, "flushDurationRange",pcl_flushdurationrange);
	typel_extendproto(&pcl_type, "flushDurationNoUncast", pcl_flushdurationnouncast);
	typel_extendproto(&pcl_type, "getCasterID", pcl_getcasterid);
	typel_extendproto(&pcl_type, "setAether",pcl_setaether);
	typel_extendproto(&pcl_type, "hasAether",pcl_hasaether); //check for aether
	typel_extendproto(&pcl_type, "getAether",pcl_getaether);
	typel_extendproto(&pcl_type, "getAllAethers", pcl_getallaethers);
	typel_extendproto(&pcl_type, "flushAether",pcl_flushaether);
	typel_extendproto(&pcl_type, "hasSpace",pcl_hasspace);
	typel_extendproto(&pcl_type, "addItem",pcl_additem);
	typel_extendproto(&pcl_type, "getInventoryItem",pcl_getinventoryitem);
	typel_extendproto(&pcl_type, "hasItem",pcl_hasitem);
	typel_extendproto(&pcl_type, "hasItemDura",pcl_hasitemdura); //has item with full dura
	typel_extendproto(&pcl_type, "removeItem",pcl_removeinventoryitem);
	typel_extendproto(&pcl_type, "removeItemSlot",pcl_removeitemslot);
	typel_extendproto(&pcl_type, "removeItemDura",pcl_removeitemdura); //remove items with full dura
	typel_extendproto(&pcl_type, "getEquippedItem",pcl_getequippeditem);
	typel_extendproto(&pcl_type, "hasEquipped",pcl_hasequipped);
	typel_extendproto(&pcl_type, "pickUp", pcl_pickup);
	typel_extendproto(&pcl_type, "equip", pcl_equip);
	typel_extendproto(&pcl_type, "takeOff", pcl_takeoff);
	typel_extendproto(&pcl_type, "throwItem",pcl_throwitem);
	typel_extendproto(&pcl_type, "updateInv",pcl_updateinv);
	typel_extendproto(&pcl_type, "addLegend",pcl_addlegend);
	typel_extendproto(&pcl_type, "hasLegend",pcl_haslegend);
	typel_extendproto(&pcl_type, "getLegend",pcl_getlegend);
	typel_extendproto(&pcl_type, "removeLegendbyName",pcl_removelegendbyname);
	typel_extendproto(&pcl_type, "removeLegendbyColor",pcl_removelegendbycolor);
	typel_extendproto(&pcl_type, "warp",pcl_warp);
	typel_extendproto(&pcl_type, "move",pcl_move);
	typel_extendproto(&pcl_type, "respawn",pcl_respawn);
	typel_extendproto(&pcl_type, "addClan", pcl_addclan);
	typel_extendproto(&pcl_type, "killCount",pcl_killcount);
	typel_extendproto(&pcl_type, "killRank",pcl_killrank);
	typel_extendproto(&pcl_type, "flushKills",pcl_flushkills);
	typel_extendproto(&pcl_type, "getUnknownSpells", pcl_getunknownspells);
	typel_extendproto(&pcl_type, "addSpell",pcl_addspell);
	typel_extendproto(&pcl_type, "removeSpell",pcl_removespell);
	typel_extendproto(&pcl_type, "hasSpell",pcl_hasspell);
	typel_extendproto(&pcl_type, "getSpells",pcl_getspells); //for remove spell
	typel_extendproto(&pcl_type, "getSpellName",pcl_getspellname); //for remove spell
	typel_extendproto(&pcl_type, "getSpellYName",pcl_getspellyname); //for remove spell
	typel_extendproto(&pcl_type, "mapSelection",pcl_mapselect);
	typel_extendproto(&pcl_type, "deductArmor",pcl_deductarmor);
	typel_extendproto(&pcl_type, "deductDura", pcl_deductdura);
	typel_extendproto(&pcl_type, "deductDuraInv", pcl_deductdurainv);
	typel_extendproto(&pcl_type, "durationAmount",pcl_durationamount);
	typel_extendproto(&pcl_type, "addGuide",pcl_addguide);
	typel_extendproto(&pcl_type, "delGuide",pcl_delguide);
	typel_extendproto(&pcl_type, "powerBoard",pcl_powerboard);
	typel_extendproto(&pcl_type, "lock",pcl_lock);
	typel_extendproto(&pcl_type, "unlock",pcl_unlock);
	typel_extendproto(&pcl_type, "swing",pcl_swing);
	typel_extendproto(&pcl_type, "swingTarget", pcl_swingtarget);
	typel_extendproto(&pcl_type, "checkBankItems",pcl_checkbankitems);
	typel_extendproto(&pcl_type, "checkBankAmounts",pcl_checkbankamounts);
	typel_extendproto(&pcl_type, "checkBankOwners",pcl_checkbankowners);
	typel_extendproto(&pcl_type, "checkBankEngraves",pcl_checkbankengraves);
	typel_extendproto(&pcl_type, "bankDeposit",pcl_bankdeposit);
	typel_extendproto(&pcl_type, "bankWithdraw",pcl_bankwithdraw);
	typel_extendproto(&pcl_type, "bankCheckAmount",pcl_checkbankitemamount);
	typel_extendproto(&pcl_type, "addThreat",pcl_addthreat); //threat table
	typel_extendproto(&pcl_type, "setThreat",pcl_setthreat); //threat table
	typel_extendproto(&pcl_type, "addThreatGeneral", pcl_addthreatgeneral); //threat table
	typel_extendproto(&pcl_type, "getClanItems", pcl_getclanitems);
	typel_extendproto(&pcl_type, "getClanAmounts", pcl_getclanamounts);
	typel_extendproto(&pcl_type, "clanBankDeposit", pcl_clanbankdeposit);
	typel_extendproto(&pcl_type, "clanBankWithdraw", pcl_clanbankwithdraw);
	typel_extendproto(&pcl_type, "checkClanItemAmounts", pcl_checkclanitemamounts);
	typel_extendproto(&pcl_type, "getCreationItems", pcl_getcreationitems);
	typel_extendproto(&pcl_type, "getCreationAmounts", pcl_getcreationamounts);
	typel_extendproto(&pcl_type, "getParcel", pcl_getparcel);
	typel_extendproto(&pcl_type, "getParcelList", pcl_getparcellist);
	typel_extendproto(&pcl_type, "removeParcel", pcl_removeparcel);
	typel_extendproto(&pcl_type, "expireItem", pcl_expireitem);
	typel_extendproto(&pcl_type, "testPacket", pcl_testpacket);
	typel_extendproto(&pcl_type, "changeView", pcl_changeview);
	typel_extendproto(&pcl_type, "setTimer", pcl_settimer);
	typel_extendproto(&pcl_type, "addTime", pcl_addtime);
	typel_extendproto(&pcl_type, "removeTime", pcl_removetime);
	bll_extendproto(&pcl_type);
}
int pcl_swing(lua_State *state, USER* sd) {
	clif_parseattack(sd);
	return 0;
}
int pcl_swingtarget(lua_State *state, USER* sd) {
	struct block_list* bl = ((struct block_list*)typel_topointer(state, sl_memberarg(1)));
	
	nullpo_ret(0, bl);
	
	if (bl->type == BL_MOB) {
		clif_mob_damage(sd, typel_topointer(state, sl_memberarg(1)));
	} else if (bl->type == BL_PC) {
		pc_damage(sd, typel_topointer(state, sl_memberarg(1)));
	}
	
	return 0;
}
int pcl_lock(lua_State *state, USER* sd) {
	clif_blockmovement(sd,0);
	return 0;
}
int pcl_unlock(lua_State *state, USER* sd) {
	clif_blockmovement(sd,1);
	return 0;
}
int pcl_menuseq(lua_State *state, USER* sd) {
	sl_checkargs(state, "stt");
	int previous = 0, next = 0;
	int size = lua_objlen(state, sl_memberarg(2));
	char *menuopts[size + 1];
	
	lua_pushnil(state);
	
	while(lua_next(state, sl_memberarg(2)) != 0) {
		// key is at -2, value at -1
		int index = lua_tonumber(state, -2);
		//if(index>0 && index<size) {
		menuopts[index] = lua_tostring(state, -1);
		//}
		lua_pop(state, 1);

	}
	
	lua_pushnil(state);
	
	while (lua_next(state, sl_memberarg(2)) != 0) {
		// key is at -2, value is at -1
		if(!strcmp(lua_tostring(state, -1), "previous")) previous = 1;
		else if(!strcmp(lua_tostring(state, -1), "next")) next = 1;
		lua_pop(state, 1);
	}
	
	char *topic = lua_tostring(state, sl_memberarg(1));
	clif_scriptmenuseq(sd, sd->last_click, topic, menuopts, size, previous, next);
	//sd->coroutine = state; TEMP
	return lua_yield(state, 0);
}
int pcl_popup(lua_State *state, USER *sd) {
	sl_checkargs(state,"s");

	clif_popup(sd,lua_tostring(state, sl_memberarg(1)));
	return 0;
}
int pcl_addguide(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int id=guidedb_id(lua_tostring(state,sl_memberarg(1)));
	int x;

	for(x=0;x<256;x++) {
		if(sd->status.guide[x]==id) return 0;
		if(sd->status.guide[x]==0) {
			sd->status.guide[x]=id;
			clif_sendguidespecific(sd,id);
			return 0;
		}
	}
	return 0;
}
int pcl_delguide(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int id=guidedb_id(lua_tostring(state,sl_memberarg(1)));
	int x;
	for(x=0;x<256;x++) {
		if(sd->status.guide[x]==id) {
			sd->status.guide[x]=0;
			return 0;
		}
	}
	return 0;
}
int pcl_durationamount(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int x,id;
	id=magicdb_id(lua_tostring(state,sl_memberarg(1)));

	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
		if(sd->status.dura_aether[x].id==id) {
			if(sd->status.dura_aether[x].duration>0) {
				lua_pushnumber(state,sd->status.dura_aether[x].duration);
				return 1;
			}
		}
	}
	lua_pushnumber(state,0);
	return 1;
}
int pcl_updateinv(lua_State *state, USER *sd) {
	pc_loaditem(sd);
	return 0;
}
int pcl_deductarmor(lua_State *state, USER *sd) {
	sl_checkargs(state, "n");
	clif_deductarmor(sd, lua_tonumber(state,sl_memberarg(1)));
	return 0;
}

int pcl_flushduration(lua_State *state, USER *sd) {
	struct block_list *bl = NULL;
	int x;
	int id;
	int dis = lua_tonumber(state, sl_memberarg(1));
	int minid = lua_tonumber(state, sl_memberarg(2));
	int maxid = lua_tonumber(state, sl_memberarg(3));
	char flush = 0;

	if (maxid < minid) {
		maxid = minid;
	}
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
	    id = sd->status.dura_aether[x].id;
		
		if (magicdb_dispel(id) > dis) continue;
		
		if (minid <= 0) {
			flush = 1;
		} else if (minid > 0 && maxid <= 0) {
			if (id == minid) {
				flush = 1;
			} else {
				flush = 0;
			}
		} else {
			if (id >= minid && id <= maxid) {
				flush = 1;
			} else {
				flush = 0;
			}
		}
		
		if (flush == 1) {
			clif_send_duration(sd, id, 0, map_id2sd(sd->status.dura_aether[x].caster_id));
			sd->status.dura_aether[x].duration = 0;
			map_foreachinarea(clif_sendanimation, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, sd->status.dura_aether[x].animation, &sd->bl, -1);
			sd->status.dura_aether[x].animation = 0;
			bl = map_id2bl(sd->status.dura_aether[x].caster_id);
			sd->status.dura_aether[x].caster_id = 0;
			
			if(sd->status.dura_aether[x].aether == 0) {
				sd->status.dura_aether[x].id = 0;
			}
			
			if (bl != NULL) {
				sl_doscript_blargs(magicdb_yname(id), "uncast", 2, &sd->bl, bl);
			} else {
				sl_doscript_blargs(magicdb_yname(id), "uncast", 1, &sd->bl);
			}
		}
	}
	
	return 0;
}

int pcl_flushdurationnouncast(lua_State *state, USER *sd) {
	int x;
	int id;
	int dis = lua_tonumber(state, sl_memberarg(1));
	int minid = lua_tonumber(state, sl_memberarg(2));
	int maxid = lua_tonumber(state, sl_memberarg(3));
	char flush = 0;

	if (maxid < minid) {
		maxid = minid;
	}
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
	    id = sd->status.dura_aether[x].id;
		
		if (magicdb_dispel(id) > dis) continue;
		
		if (minid <= 0) {
			flush = 1;
		} else if (minid > 0 && maxid <= 0) {
			if (id == minid) {
				flush = 1;
			} else {
				flush = 0;
			}
		} else {
			if(id >= minid && id <= maxid) {
				flush = 1;
			} else {
				flush = 0;
			}
		}
		
		if (flush == 1) {
			clif_send_duration(sd, id, 0, map_id2sd(sd->status.dura_aether[x].caster_id));
			sd->status.dura_aether[x].duration = 0;
			sd->status.dura_aether[x].caster_id = 0;
			map_foreachinarea(clif_sendanimation, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, sd->status.dura_aether[x].animation, &sd->bl, -1);
			sd->status.dura_aether[x].animation = 0;
			
			if(sd->status.dura_aether[x].aether == 0) {
				sd->status.dura_aether[x].id = 0;
			}
		}
	}
	
	return 0;
}

/*int pcl_flushduration(lua_State *state, USER *sd) {
	struct block_list *bl = NULL;
	int x;
	int id;
	
	for(x = 0; x < MAX_MAGIC_TIMERS; x++) {
		id = sd->status.dura_aether[x].id;
		
		if(id > 0) {
			sd->status.dura_aether[x].duration = 0;
			clif_send_duration(sd,sd->status.dura_aether[x].id,0);
			sd->status.dura_aether[x].caster_id = 0;
			map_foreachinarea(clif_sendanimation, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, sd->status.dura_aether[x].animation, &sd->bl, -1);
			sd->status.dura_aether[x].animation = 0;
			
			if(sd->status.dura_aether[x].aether == 0) {
				sd->status.dura_aether[x].id = 0;
			}
			
			if(sd->status.dura_aether[x].caster_id != sd->bl.id) {
				bl = map_id2bl(sd->status.dura_aether[x].caster_id);
			}
			
			if(bl) {
				sl_doscript_blargs(magicdb_yname(id), "uncast", 2, &sd->bl, bl);
			} else {
				sl_doscript_blargs(magicdb_yname(id), "uncast", 1, &sd->bl);
			}
		}
	}
	
	return 0;
}*/

int pcl_flushaether(lua_State *state, USER *sd) {
	int x;
	int id;
	int dis = lua_tonumber(state, sl_memberarg(1));
	int minid = lua_tonumber(state, sl_memberarg(2));
	int maxid = lua_tonumber(state, sl_memberarg(3));
	char flush = 0;

	if (maxid < minid) {
		maxid = minid;
	}
	
	for(x = 0; x < MAX_MAGIC_TIMERS; x++) {
		id = sd->status.dura_aether[x].id;
		
		if (magicdb_aether(id) > dis) continue;
		
		if (minid <= 0) {
			flush = 1;
		} else if (minid > 0 && maxid <= 0) {
			if (id == minid) {
				flush = 1;
			}
		} else {
			if(id >= minid && id <= maxid) {
				flush = 1;
			}
		}
		if (flush == 1) {
			clif_send_aether(sd, id, 0);
			sd->status.dura_aether[x].aether = 0;
			
			if(sd->status.dura_aether[x].duration == 0) {
				sd->status.dura_aether[x].id = 0;
			}
		}
	}
	
	return 0;
}

int pcl_killcount(lua_State *state, USER *sd) {
	int x;
	int id;

	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "n");
		id = lua_tonumber(state,sl_memberarg(1));
	} else {
		sl_checkargs(state, "s");
		id=mobdb_id(lua_tostring(state,sl_memberarg(1)));
	}
	for(x=0;x<10;x++) {
		if(sd->status.quest[x].mob_id==id) {
			lua_pushnumber(state,sd->status.quest[x].amount);
			return 1;
		}
	}

	lua_pushnumber(state,0);
	return 1;
}
int pcl_killrank(lua_State *state, USER *sd) {
	int x;
	int id;

	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "n");
		id = lua_tonumber(state,sl_memberarg(1));
	} else {
		sl_checkargs(state, "s");
		id=mobdb_id(lua_tostring(state,sl_memberarg(1)));
	}
	for(x=0;x<10;x++) {
		if(sd->status.quest[x].mob_id==id) {
			lua_pushnumber(state,x+1);
			return 1;
		}
	}

	lua_pushnumber(state,0);
	return 1;
}
int pcl_flushkills(lua_State *state, USER *sd) {
	int x;
	int flushid = 0;
	
	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "n");
		flushid = lua_tonumber(state,sl_memberarg(1));
	} else {
		sl_checkargs(state, "s");
		flushid=mobdb_id(lua_tostring(state,sl_memberarg(1)));
	}
	
	if(flushid == 0) {
		for(x=0;x<10;x++) {
			sd->status.quest[x].mob_id=0;
			sd->status.quest[x].amount=0;
		}
	} else {
		for(x=0;x<10;x++) {
			if(sd->status.quest[x].mob_id == flushid) {
				sd->status.quest[x].mob_id = 0;
				sd->status.quest[x].amount = 0;
			}
		}
	}

	return 0;
}

int pcl_showhealth(lua_State *state, USER *sd) {
	sl_checkargs(state,"nn");
	int type=lua_tonumber(state,sl_memberarg(1));
	int damage=lua_tonumber(state,sl_memberarg(2));
	//printf("Damage: %d\n",damage);
	clif_send_pc_health(sd,damage,type);
	return 0;
}
int pcl_addclan(lua_State *state, USER *sd) {
	struct clan_data* clan;
	sl_checkargs(state,"s");
	char* c_name=lua_tostring(state,sl_memberarg(1));
	clan=clandb_searchname(c_name);

	if(clan) {
		lua_pushboolean(state,0);
	} else {
		clandb_add(sd,c_name);
		lua_pushboolean(state,1);
	}
	return 1;
}
int pcl_hasequipped(lua_State *state, USER *sd) {
	sl_checkargs(state,"t");
    int found=0;
	int count=0;
	int id;
	int x;
	lua_pushnil(state);

	while (lua_next(state, sl_memberarg(1)) != 0) {
		id=0;
		// key is at -2, value is at -1
		if(lua_isstring(state,-1)) {
			id=itemdb_id(lua_tostring(state,-1));
		} else {
			id=lua_tonumber(state,-1);
		}
		count++;

		for(x=0;x<15;x++) {
			if((sd->status.equip[x].id==id) && (id>0)) {
				found++;
				break;
			}
		}

		lua_pop(state, 1);
	}

	if(found==count) {
		lua_pushboolean(state,1);
	} else {
		lua_pushboolean(state,0);
	}
	return 1;
}
int pcl_ctor(lua_State *state) {
	USER *sd = NULL;
	if(lua_isnumber(state, 2)) {
		unsigned int id = lua_tonumber(state, 2);
		sd = map_id2sd(id);
		if(!sd) {
			lua_pushnil(state);
			return 1; //luaL_error(state, "invalid player id (%d)", id);
		}
	} else if(lua_isstring(state, 2)) {
		char *name = lua_tostring(state, 2);
		sd = map_name2sd(name);
		if(!sd) {
			lua_pushnil(state);
			return 1; //luaL_error(state, "invalid player name (%s)", name);
		}
	} else {
		luaL_argerror(state, 1, "expected string or number");
	}
	pcl_pushinst(state, sd);
	return 1;
}
int pcl_setattr(lua_State *state, USER *sd, char *attrname) {
	if(bll_setattr(state,&sd->bl,attrname)) return 1;
	if(!strcmp(attrname,"npcGraphic")) sd->npc_g = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"npcColor")) sd->npc_gc = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"lastClick")) sd->last_click = lua_tonumber(state, -1);
	else if(!strcmp(attrname,"title")) strcpy(sd->status.title,lua_tostring(state,-1));
	else if(!strcmp(attrname,"clanTitle")) strcpy(sd->status.clan_title,lua_tostring(state,-1));
	else if(!strcmp(attrname,"maxHealth")) sd->max_hp = lua_tonumber(state,-1);
	else if(!strcmp(attrname,"maxMagic")) sd->max_mp = lua_tonumber(state,-1);
	else if(!strcmp(attrname,"polearm")) sd->polearm=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"rage")) sd->rage=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"clan")) sd->status.clan_id=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"gmLevel")) sd->status.gm_level=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"class")) sd->status.class=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"totem")) sd->status.totem=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"tier")) sd->status.tier=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"mark")) sd->status.mark=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"health")) sd->status.hp=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"magic")) sd->status.mp=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"baseHealth")) sd->status.basehp=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"baseMagic")) sd->status.basemp=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"lastHealth")) sd->lastvita=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"exp")) sd->status.exp=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"expSold")) sd->status.expsold=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"state")) sd->status.state=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"hair")) sd->status.hair=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"paralyzed")) sd->paralyzed=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"blind")) { sd->blind=lua_toboolean(state,-1); clif_sendstatus(sd,NULL); }
	else if(!strcmp(attrname,"drunk")) { sd->drunk=lua_tonumber(state,-1); clif_sendstatus(sd,NULL); }
	else if(!strcmp(attrname,"sex")) { sd->status.sex=lua_tonumber(state,-1); }
	else if(!strcmp(attrname,"uflags")) { sd->uFlags^=lua_tointeger(state,-1); }
	else if(!strcmp(attrname,"backstab")) sd->backstab=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"flank")) sd->flank=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"armor")) sd->armor=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"ac")) sd->ac=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"dam")) sd->dam=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"hit")) sd->hit=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"miss")) sd->miss=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"pierce")) sd->pierce=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"might")) sd->might=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"will")) sd->will=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"grace")) sd->grace=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"boardDel")) sd->board_candel=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"boardWrite")) sd->board_canwrite=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"boardShow")) sd->boardshow=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"side")) sd->status.side=lua_tonumber(state,-1);
    else if(!strcmp(attrname,"sleep")) sd->sleep= lua_tonumber(state,-1);
	else if(!strcmp(attrname,"attackSpeed")) sd->attack_speed=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"speech")) strcpy(sd->speech, lua_tostring(state,-1));
	else if(!strcmp(attrname,"enchant")) sd->enchanted=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"money")) sd->status.money=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"settings")) sd->status.settingFlags^=lua_tointeger(state,-1);
	else if(!strcmp(attrname,"confused")) sd->confused=lua_toboolean(state,-1);
	else if(!strcmp(attrname,"skinColor")) sd->status.skin_color=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"target")) sd->target=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"deduction")) sd->deduction=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"hairColor")) sd->status.hair_color=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"face")) sd->status.face=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"faceColor")) sd->status.face_color=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"speed")) sd->speed=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"armorColor")) sd->status.armor_color=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"spouse")) sd->status.spouse=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"disguise")) sd->disguise=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"disguiseColor")) sd->disguise_color=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"attacker")) sd->attacker=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"invis")) sd->invis=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"coContainer")) sd->coref_container=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"level")) sd->status.level=lua_tonumber(state,-1);
	else if(!strcmp(attrname,"rangeTarget")) sd->rangeTarget=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"maxSlots"))sd->status.maxslots=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"bankMoney"))sd->status.bankmoney=lua_tonumber(state, -1);
	//else if(!strcmp(attrname,"threat"))sd->threat=lua_tonumber(state, -1); deprecated
	else if(!strcmp(attrname,"snare"))sd->snare=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"silence"))sd->silence=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"extendHit"))sd->extendhit=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"afk"))sd->afk=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"afkMessage"))strcpy(sd->afkmessage,lua_tostring(state, -1));
	else if(!strcmp(attrname,"optFlags"))sd->optFlags^=lua_tointeger(state, -1);
	else if(!strcmp(attrname,"regen"))sd->regen=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"vRegen"))sd->vregen=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"mRegen"))sd->mregen=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"talkType"))sd->talktype=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"f1Name"))strcpy(sd->status.f1name,lua_tostring(state, -1));
	else if(!strcmp(attrname,"crit"))sd->crit=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"critChance"))sd->critchance=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"critMult"))sd->critmult=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"damage"))sd->damage=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"parry"))sd->parry=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"block"))sd->block=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"dodge"))sd->dodge=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"spotTraps"))sd->spottraps=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"fury"))sd->fury=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"maxInv"))sd->status.maxinv=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"faceAccessoryTwo"))sd->status.equip[EQ_FACEACCTWO].id=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"faceAccessoryTwoColor"))sd->status.equip[EQ_FACEACCTWO].custom=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFace"))sd->gfx.face=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHair"))sd->gfx.hair=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHairC"))sd->gfx.chair=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceC"))sd->gfx.cface=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxSkinC"))sd->gfx.cskin=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxDye"))sd->gfx.dye=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxWeap"))sd->gfx.weapon=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxWeapC"))sd->gfx.cweapon=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxArmor"))sd->gfx.armor=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxArmorC"))sd->gfx.carmor=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxShield"))sd->gfx.shield=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxShieldC"))sd->gfx.cshield=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHelm"))sd->gfx.helm=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxHelmC"))sd->gfx.chelm=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCape"))sd->gfx.mantle=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCapeC"))sd->gfx.cmantle=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCrown"))sd->gfx.crown=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxCrownC"))sd->gfx.ccrown=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceA"))sd->gfx.faceAcc=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceAC"))sd->gfx.cfaceAcc=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceAT"))sd->gfx.faceAccT=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxFaceATC"))sd->gfx.cfaceAccT=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxBoots"))sd->gfx.boots=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxBootsC"))sd->gfx.cboots=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxNeck"))sd->gfx.necklace=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxNeckC"))sd->gfx.cnecklace=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"gfxName"))strcpy(sd->gfx.name, lua_tostring(state, -1));
	else if(!strcmp(attrname,"gfxClone"))sd->clone=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"PK"))sd->status.pk=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"killedBy"))sd->status.killedby=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"protection"))sd->protection=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"resist"))sd->resist=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"physDeduct"))sd->physdeduct=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"dmgShield")) {sd->dmgshield=lua_tonumber(state, -1); if (sd->dmgshield <= 0) clif_send_duration(sd, 0, 0, NULL);}
	else if(!strcmp(attrname,"dmgDealt"))sd->dmgdealt=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"dmgTaken"))sd->dmgtaken=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"mute"))sd->status.mute=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"wisdom"))sd->wisdom=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"con"))sd->con=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"luaExec"))sd->luaexec=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"deathFlag"))sd->deathflag=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"selfBar"))sd->selfbar=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"groupBars"))sd->groupbars=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"mobBars"))sd->mobbars=lua_toboolean(state, -1);
	else if(!strcmp(attrname,"bindMap"))sd->bindmap=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"bindX"))sd->bindx=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"bindY"))sd->bindy=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"heroShow"))sd->status.heroes=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"ambushTimer"))sd->ambushtimer=lua_tonumber(state, -1);
	else if(!strcmp(attrname,"dialogType"))sd->dialogtype=lua_tonumber(state, -1);
	else return 0;
	return 1;
}
int pcl_dialog(lua_State *state, USER *sd) {
	sl_checkargs(state, "st");
	int previous = 0, next = 0;
	lua_pushnil(state);
	
	while (lua_next(state, sl_memberarg(2)) != 0) {
		// key is at -2, value is at -1
		if(!strcmp(lua_tostring(state, -1), "previous")) previous = 1;
		else if(!strcmp(lua_tostring(state, -1), "next")) next = 1;
		lua_pop(state, 1);
	}

	char *msg = lua_tostring(state, sl_memberarg(1));

	//sd->coroutine = state; TEMP
	clif_scriptmes(sd, sd->last_click, msg, previous, next);
	return lua_yield(state, 0);
}
int pcl_init(lua_State *state, USER *sd, int dataref, void *param) {
	//USER *tsd;
	int userc=0;
	//if(!sd) {
		//luaL_error(state,"cannot do that.");
		//sl_err_print(state);
	//	lua_pop(state,1);
	//	return 0;
	//}

	sl_pushref(state, dataref);
	regl_pushinst(state, sd);
	lua_setfield(state, -2, "registry");
	
	questregl_pushinst(state, sd);
	lua_setfield(state, -2, "quest");
	
	npcintregl_pushinst(state, sd);
	lua_setfield(state, -2, "npc");

	acctregl_pushinst(state, sd);
	lua_setfield(state, -2, "accountRegistry");
	
	mapregl_pushinst(state,sd);
	lua_setfield(state,-2,"mapRegistry");

	gameregl_pushinst(state,sd);
	lua_setfield(state,-2,"gameRegistry");
	/*if(sd->target) {
		if(sd->target == sd->bl.id) {
			lua_pushvalue(state, -2); // push a copy of the player object
			lua_setfield(state, -2, "target");
		} else if(param == 0) {
			// the param is used to stop infinite recursion
			struct block_list *bl = map_id2bl(sd->target);
			if(bl) {
				bll_pushinst(state, bl, 1);
				lua_setfield(state, -2, "target");
			}
		}
	}*/
	lua_newtable(state);
	
	if (sd->group_count > 0) {
		int i;
		
		for(i = 0; i < sd->group_count; i++) {
			lua_pushnumber(state, groups[sd->groupid][i]);
			lua_rawseti(state, -2, i + 1);
		}
	} else {
		lua_pushnumber(state, sd->status.id);
		lua_rawseti(state, -2, 1);
	}
		
	lua_setfield(state, -2, "group");

	//lua_newtable(state);
	//map_foreachinarea(sl_mob_look,sd->bl.m,sd->bl.x,sd->bl.y,SAMEMAP,BL_MOB,sd);
	//lua_setfield(state,-2,"mobs");

	/*int i;
	lua_newtable(state);
	for(i=0;i<26;i++) {
		lua_pushnumber(state, sd->status.inventory[i].id);
		lua_rawseti(state, -2, i + 1);
	}
	lua_setfield(state,-2,"inventory");
*/
	lua_pop(state, 1); // pop the data table
}

int sl_mob_look(struct block_list *bl, va_list ap) {
	return 0;
}
int pcl_getattr(lua_State *state, USER *sd, char *attrname) {
	if(bll_getattr(state, &sd->bl, attrname)) return 1;
	if(!strcmp(attrname,"id")) lua_pushnumber(state, sd->status.id);
	else if(!strcmp(attrname, "groupID")) lua_pushnumber(state, sd->groupid);
	else if(!strcmp(attrname, "health")) lua_pushnumber(state, sd->status.hp);
	else if(!strcmp(attrname, "magic")) lua_pushnumber(state, sd->status.mp);
	else if(!strcmp(attrname, "level")) lua_pushnumber(state, sd->status.level);
	else if(!strcmp(attrname, "name")) lua_pushstring(state, sd->status.name);
	else if(!strcmp(attrname, "title")) lua_pushstring(state, sd->status.title);
	else if(!strcmp(attrname,"maxHealth")) lua_pushnumber(state,sd->max_hp);
	else if(!strcmp(attrname,"maxMagic")) lua_pushnumber(state,sd->max_mp);
	else if(!strcmp(attrname,"lastHealth")) lua_pushnumber(state,sd->lastvita);
	else if(!strcmp(attrname,"rage")) lua_pushnumber(state,sd->rage);
//		if(sd->rage) {
//			lua_pushnumber(state,sd->rage);
//		} else {
//			lua_pushnumber(state,1);
//		}
//	}
	else if(!strcmp(attrname,"clanTitle")) lua_pushstring(state,sd->status.clan_title);
	else if(!strcmp(attrname,"polearm")) lua_pushnumber(state,sd->polearm);
	else if(!strcmp(attrname,"clan")) lua_pushnumber(state,sd->status.clan_id);
	else if(!strcmp(attrname,"clanName")) lua_pushstring(state, clandb_name(sd->status.clan_id));
	else if(!strcmp(attrname,"gmLevel")) lua_pushnumber(state,sd->status.gm_level);
	else if(!strcmp(attrname,"lastClick")) lua_pushnumber(state, sd->last_click);
	else if(!strcmp(attrname,"class")) lua_pushnumber(state,sd->status.class);
	else if(!strcmp(attrname,"baseClass")) lua_pushnumber(state,classdb_path(sd->status.class));
	else if(!strcmp(attrname,"totem")) lua_pushnumber(state, sd->status.totem);
	else if(!strcmp(attrname,"tier")) lua_pushnumber(state,sd->status.tier);
	else if(!strcmp(attrname,"mark")) lua_pushnumber(state,sd->status.mark);
	else if(!strcmp(attrname,"baseHealth")) lua_pushnumber(state,sd->status.basehp);
	else if(!strcmp(attrname,"baseMagic")) lua_pushnumber(state,sd->status.basemp);
	else if(!strcmp(attrname,"exp")) lua_pushnumber(state,sd->status.exp);
	else if(!strcmp(attrname,"expSold")) lua_pushnumber(state,sd->status.expsold);
	else if(!strcmp(attrname,"state")) lua_pushnumber(state,sd->status.state);
	else if(!strcmp(attrname,"face")) lua_pushnumber(state,sd->status.face);
	else if(!strcmp(attrname,"hair")) lua_pushnumber(state,sd->status.hair);
	else if(!strcmp(attrname,"hairColor")) lua_pushnumber(state, sd->status.hair_color);
	else if(!strcmp(attrname,"faceColor")) lua_pushnumber(state, sd->status.face_color);
	else if(!strcmp(attrname,"armorColor")) lua_pushnumber(state, sd->status.armor_color);
	else if(!strcmp(attrname,"skinColor")) lua_pushnumber(state, sd->status.skin_color);
	else if(!strcmp(attrname,"paralyzed")) lua_pushboolean(state,sd->paralyzed);
	else if(!strcmp(attrname,"blind")) lua_pushboolean(state,sd->blind);
	else if(!strcmp(attrname,"drunk")) lua_pushnumber(state,sd->drunk);
	else if(!strcmp(attrname,"board")) lua_pushnumber(state,sd->board);
	else if(!strcmp(attrname,"boardDel")) lua_pushnumber(state,sd->board_candel);
	else if(!strcmp(attrname,"boardWrite")) lua_pushnumber(state,sd->board_canwrite);
	else if(!strcmp(attrname,"boardShow")) lua_pushnumber(state,sd->boardshow);
	else if(!strcmp(attrname,"sex")) lua_pushnumber(state,sd->status.sex);
    else if(!strcmp(attrname,"ping")) lua_pushnumber(state,sd->msPing);
	else if(!strcmp(attrname,"pbColor")) lua_pushnumber(state,sd->pbColor);
	else if(!strcmp(attrname,"coRef")) lua_pushnumber(state,sd->coref);
	else if(!strcmp(attrname,"optFlags")) lua_pushnumber(state,sd->optFlags);
    else if(!strcmp(attrname,"uflags")) lua_pushnumber(state,sd->uFlags);
	else if(!strcmp(attrname,"settings")) lua_pushnumber(state,sd->status.settingFlags);
	else if(!strcmp(attrname,"side")) lua_pushnumber(state,sd->status.side);
	else if(!strcmp(attrname,"grace")) lua_pushnumber(state,sd->grace);
	else if(!strcmp(attrname,"might")) lua_pushnumber(state,sd->might);
	else if(!strcmp(attrname,"will")) lua_pushnumber(state,sd->will);
	else if(!strcmp(attrname,"backstab")) lua_pushboolean(state, sd->backstab);
	else if(!strcmp(attrname,"flank")) lua_pushboolean(state, sd->flank);
	else if(!strcmp(attrname,"armor")) lua_pushnumber(state, sd->armor);
	else if(!strcmp(attrname,"ac")) lua_pushnumber(state,sd->ac);
	else if(!strcmp(attrname,"dam")) lua_pushnumber(state,sd->dam);
	else if(!strcmp(attrname,"hit")) lua_pushnumber(state,sd->hit);
	else if(!strcmp(attrname,"miss")) lua_pushnumber(state,sd->miss);
	else if(!strcmp(attrname,"pierce")) lua_pushnumber(state,sd->pierce);
	else if(!strcmp(attrname,"sleep")) lua_pushnumber(state,sd->sleep);
	else if(!strcmp(attrname,"attackSpeed")) lua_pushnumber(state,sd->attack_speed);
	else if(!strcmp(attrname,"question")) lua_pushstring(state,sd->question);
	else if(!strcmp(attrname,"enchant")) lua_pushnumber(state,sd->enchanted);
	else if(!strcmp(attrname,"speech")) lua_pushstring(state,sd->speech);
	else if(!strcmp(attrname,"money")) lua_pushnumber(state,sd->status.money);
	else if(!strcmp(attrname,"confused")) lua_pushboolean(state,sd->confused);
	else if(!strcmp(attrname,"skinColor")) lua_pushnumber(state,sd->status.skin_color);
	else if(!strcmp(attrname,"target")) lua_pushnumber(state,sd->target);
	else if(!strcmp(attrname,"deduction")) lua_pushnumber(state,sd->deduction);
	else if(!strcmp(attrname,"speed")) lua_pushnumber(state,sd->speed);
	else if(!strcmp(attrname,"spouse")) lua_pushnumber(state,sd->status.spouse);
	else if(!strcmp(attrname,"disguise")) lua_pushnumber(state,sd->disguise);
	else if(!strcmp(attrname,"disguiseColor")) lua_pushnumber(state,sd->disguise_color);
	else if(!strcmp(attrname,"attacker")) lua_pushnumber(state,sd->attacker);
	else if(!strcmp(attrname,"invis")) lua_pushnumber(state,sd->invis);
	else if(!strcmp(attrname,"damage")) lua_pushnumber(state,sd->damage);
	else if(!strcmp(attrname,"crit")) lua_pushnumber(state, sd->crit);
	else if(!strcmp(attrname,"critChance")) lua_pushnumber(state, sd->critchance);
	else if(!strcmp(attrname,"critMult")) lua_pushnumber(state, sd->critmult);
	else if(!strcmp(attrname,"rangeTarget")) lua_pushnumber(state,sd->rangeTarget);
	else if(!strcmp(attrname,"maxSlots")) lua_pushnumber(state,sd->status.maxslots);
	else if(!strcmp(attrname,"bankMoney")) lua_pushnumber(state,sd->status.bankmoney);
	//else if(!strcmp(attrname,"threat")) lua_pushnumber(state,sd->threat); deprecated
	else if(!strcmp(attrname,"snare")) lua_pushboolean(state,sd->snare);
	else if(!strcmp(attrname,"silence")) lua_pushboolean(state,sd->silence);
	else if(!strcmp(attrname,"extendHit")) lua_pushboolean(state,sd->extendhit);
	else if(!strcmp(attrname,"afk")) lua_pushboolean(state,sd->afk);
	else if(!strcmp(attrname,"afkTime")) lua_pushnumber(state, sd->afktime);
	else if(!strcmp(attrname,"afkTimeTotal")) lua_pushnumber(state,sd->totalafktime);
	else if(!strcmp(attrname,"afkMessage")) lua_pushstring(state,sd->afkmessage);
	else if(!strcmp(attrname,"className")) lua_pushstring(state,classdb_name(sd->status.class, 0));
	else if(!strcmp(attrname,"classNameMark")) lua_pushstring(state,classdb_name(sd->status.class, sd->status.mark));
	else if(!strcmp(attrname,"regen")) lua_pushnumber(state,sd->regen);
	else if(!strcmp(attrname,"vRegen")) lua_pushnumber(state,sd->vregen);
	else if(!strcmp(attrname,"vRegenAmount")) {
		lua_pushnumber(state, (((((float)sd->max_hp * 0.01f) +
		((float)sd->max_hp * (((float)(sd->regen + sd->vregen) * 0.00025f) +
		((float)((sd->regen + sd->vregen) ^ 2) * 0.00000625f)) * 2.0f)) / 3.0f) * 5.0f));
	}
	else if(!strcmp(attrname,"mRegen")) lua_pushnumber(state,sd->mregen);
	else if(!strcmp(attrname,"mRegenAmount")) {
		lua_pushnumber(state, (((((float)sd->max_mp * .0085f) +
		((float)sd->max_mp * ((float)((sd->regen + sd->mregen) + (sd->will / 5)) +
		((float)((sd->regen + sd->mregen) ^ 2) * 0.00000625f)) * 0.00042f)) / 3.0f) * 5.0f));
	}
	else if(!strcmp(attrname,"minDam")) lua_pushnumber(state,sd->min_dam);
	else if(!strcmp(attrname,"maxDam")) lua_pushnumber(state,sd->max_dam);
	else if(!strcmp(attrname,"talkType")) lua_pushnumber(state,sd->talktype);
	else if(!strcmp(attrname,"f1Name")) lua_pushstring(state,sd->status.f1name);
	else if(!strcmp(attrname,"equipID")) lua_pushnumber(state, sd->equipid);
	else if(!strcmp(attrname,"takeOffID")) lua_pushnumber(state, sd->takeoffid);
	else if(!strcmp(attrname,"breakID")) lua_pushnumber(state, sd->breakid);
	else if(!strcmp(attrname,"invSlot")) lua_pushnumber(state, sd->invslot);
	else if(!strcmp(attrname,"pickUpType")) lua_pushnumber(state, sd->pickuptype);
	else if(!strcmp(attrname,"parry")) lua_pushnumber(state, sd->parry);
	else if(!strcmp(attrname,"block")) lua_pushnumber(state, sd->block);
	else if(!strcmp(attrname,"dodge")) lua_pushnumber(state, sd->dodge);
	else if(!strcmp(attrname,"spotTraps")) lua_pushboolean(state, sd->spottraps);
	else if(!strcmp(attrname,"fury")) lua_pushnumber(state, sd->fury);
	else if(!strcmp(attrname,"maxInv")) lua_pushnumber(state, sd->status.maxinv);
	else if(!strcmp(attrname,"faceAccessoryTwo")) lua_pushnumber(state, sd->status.equip[EQ_FACEACCTWO].id);
	else if(!strcmp(attrname,"faceAccessoryTwoColor")) lua_pushnumber(state, sd->status.equip[EQ_FACEACCTWO].custom);
	else if(!strcmp(attrname,"gfxFace")) lua_pushnumber(state, sd->gfx.face);
	else if(!strcmp(attrname,"gfxHair")) lua_pushnumber(state, sd->gfx.hair);
	else if(!strcmp(attrname,"gfxHairC")) lua_pushnumber(state, sd->gfx.chair);
	else if(!strcmp(attrname,"gfxFaceC")) lua_pushnumber(state, sd->gfx.cface);
	else if(!strcmp(attrname,"gfxSkinC")) lua_pushnumber(state, sd->gfx.cskin);
	else if(!strcmp(attrname,"gfxDye")) lua_pushnumber(state, sd->gfx.dye);
	else if(!strcmp(attrname,"gfxWeap"))lua_pushnumber(state, sd->gfx.weapon);
	else if(!strcmp(attrname,"gfxWeapC"))lua_pushnumber(state, sd->gfx.cweapon);
	else if(!strcmp(attrname,"gfxArmor"))lua_pushnumber(state, sd->gfx.armor);
	else if(!strcmp(attrname,"gfxArmorC"))lua_pushnumber(state, sd->gfx.carmor);
	else if(!strcmp(attrname,"gfxShield"))lua_pushnumber(state, sd->gfx.shield);
	else if(!strcmp(attrname,"gfxShieldC"))lua_pushnumber(state, sd->gfx.cshield);
	else if(!strcmp(attrname,"gfxHelm"))lua_pushnumber(state, sd->gfx.helm);
	else if(!strcmp(attrname,"gfxHelmC"))lua_pushnumber(state, sd->gfx.chelm);
	else if(!strcmp(attrname,"gfxCape"))lua_pushnumber(state, sd->gfx.mantle);
	else if(!strcmp(attrname,"gfxCapeC"))lua_pushnumber(state, sd->gfx.cmantle);
	else if(!strcmp(attrname,"gfxCrown"))lua_pushnumber(state, sd->gfx.crown);
	else if(!strcmp(attrname,"gfxCrownC"))lua_pushnumber(state, sd->gfx.ccrown);
	else if(!strcmp(attrname,"gfxFaceA"))lua_pushnumber(state, sd->gfx.faceAcc);
	else if(!strcmp(attrname,"gfxFaceAC"))lua_pushnumber(state, sd->gfx.cfaceAcc);
	else if(!strcmp(attrname,"gfxFaceAT"))lua_pushnumber(state, sd->gfx.faceAccT);
	else if(!strcmp(attrname,"gfxFaceATC"))lua_pushnumber(state, sd->gfx.cfaceAccT);
	else if(!strcmp(attrname,"gfxBoots"))lua_pushnumber(state, sd->gfx.boots);
	else if(!strcmp(attrname,"gfxBootsC"))lua_pushnumber(state, sd->gfx.cboots);
	else if(!strcmp(attrname,"gfxNeck"))lua_pushnumber(state, sd->gfx.necklace);
	else if(!strcmp(attrname,"gfxNeckC"))lua_pushnumber(state, sd->gfx.cnecklace);
	else if(!strcmp(attrname,"gfxName"))lua_pushstring(state, sd->gfx.name);
	else if(!strcmp(attrname,"gfxClone"))lua_pushnumber(state, sd->clone);
	else if(!strcmp(attrname,"PK"))lua_pushnumber(state, sd->status.pk);
	else if(!strcmp(attrname,"killedBy"))lua_pushnumber(state, sd->status.killedby);
	else if(!strcmp(attrname,"killsPK"))lua_pushnumber(state, sd->status.killspk);
	else if(!strcmp(attrname,"killsPVP"))lua_pushnumber(state, sd->killspvp);
	else if(!strcmp(attrname,"durationPK"))lua_pushnumber(state, sd->status.pkduration);
	else if(!strcmp(attrname,"protection"))lua_pushnumber(state, sd->protection);
	else if(!strcmp(attrname,"resist"))lua_pushnumber(state, sd->resist);
	else if(!strcmp(attrname,"physDeduct"))lua_pushnumber(state, sd->physdeduct);
	else if(!strcmp(attrname,"createCount"))lua_pushnumber(state, RFIFOB(sd->fd, 5));
	else if(!strcmp(attrname,"timerTick"))lua_pushnumber(state, sd->scripttick);
	else if(!strcmp(attrname,"dmgShield"))lua_pushnumber(state, sd->dmgshield);
	else if(!strcmp(attrname,"dmgDealt"))lua_pushnumber(state, sd->dmgdealt);
	else if(!strcmp(attrname,"dmgTaken"))lua_pushnumber(state, sd->dmgtaken);
	else if(!strcmp(attrname,"action"))lua_pushnumber(state, sd->action);
	else if(!strcmp(attrname,"mute"))lua_pushboolean(state, sd->status.mute);
	else if(!strcmp(attrname,"wisdom"))lua_pushnumber(state, sd->wisdom);
	else if(!strcmp(attrname,"con"))lua_pushnumber(state, sd->con);
	else if(!strcmp(attrname,"deathFlag"))lua_pushnumber(state, sd->deathflag);
	else if(!strcmp(attrname,"selfBar")) lua_pushboolean(state, sd->selfbar);
	else if(!strcmp(attrname,"groupBars")) lua_pushboolean(state, sd->groupbars);
	else if(!strcmp(attrname,"mobBars")) lua_pushboolean(state, sd->mobbars);
	else if(!strcmp(attrname,"displayTimeLeft")) lua_pushnumber(state, sd->disptimertick);
	else if(!strcmp(attrname,"bindMap")) lua_pushnumber(state, sd->bindmap);
	else if(!strcmp(attrname,"bindX")) lua_pushnumber(state, sd->bindx);
	else if(!strcmp(attrname,"bindY")) lua_pushnumber(state, sd->bindy);
	else if(!strcmp(attrname,"heroShow")) lua_pushnumber(state, sd->status.heroes);
	else if(!strcmp(attrname,"ambushTimer")) lua_pushnumber(state, sd->ambushtimer);
	else if(!strcmp(attrname,"dialogType")) lua_pushnumber(state, sd->dialogtype);
	else if(!strcmp(attrname,"mail")) lua_pushstring(state, sd->mail);
	else return 0; // the attribute wasn't handled
	return 1;
}
int pcl_useitem(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	char *itemname = lua_tostring(state, sl_memberarg(1));
	int i;
	for(i = 0; i < sd->status.maxinv; i++) {
		if(!strcmpi(itemdb_name(sd->status.inventory[i].id), itemname)) {
			pc_useitem(sd, i);
			return 0;
		}
	}
	return 0;
}
int pcl_warp(lua_State *state, USER *sd) {
	sl_checkargs(state, "nnn");
	int m = lua_tonumber(state, sl_memberarg(1)),
	    x = lua_tonumber(state, sl_memberarg(2)),
	    y = lua_tonumber(state, sl_memberarg(3));
	
	pc_warp(sd, m, x, y);
	return 0;
}
int pcl_resurrect(lua_State *state, USER *sd) {
	pc_res(sd);
	return 0;
}
int pcl_addhealth(lua_State *state, USER *sd) {
	sl_checkargs(state, "n");
	int damage = lua_tonumber(state, sl_memberarg(1));
	struct block_list *bl = map_id2bl(sd->attacker);

	if (bl != NULL) {
		if (damage > 0) sl_doscript_blargs("player_combat", "on_healed", 2, &sd->bl, bl);
	} else {
		if (damage > 0) sl_doscript_blargs("player_combat", "on_healed", 1, &sd->bl);
	}
	
	clif_send_pc_healthscript(sd, -damage, 0);
	clif_sendstatus(sd, SFLAG_HPMP);
	return 0;
}
int pcl_removehealth(lua_State *state, USER *sd) {
	sl_checkargs(state, "n");
	int damage = lua_tonumber(state, sl_memberarg(1));
	int caster = lua_tonumber(state,sl_memberarg(2));
	struct block_list *bl = NULL;
	USER *tsd = NULL;
	MOB *tmob = NULL;
	
	if(caster > 0) {
		bl = map_id2bl(caster);
		sd->attacker = caster;
	} else {
		bl = map_id2bl(sd->attacker);
	}
	
	if (bl != NULL) {
		if(bl->type == BL_PC) {
			tsd=(USER*)bl;
			tsd->damage = damage;
			tsd->critchance = 0;
		} else if(bl->type == BL_MOB) {
			tmob = (MOB*)bl;
			tmob->damage = damage;
			tmob->critchance = 0;
		}
	} else {
		sd->damage = damage;
		sd->critchance = 0;
	}
	
	if(sd->status.state != PC_DIE) {
		clif_send_pc_healthscript(sd, damage, 0);
		clif_sendstatus(sd, SFLAG_HPMP);
	}
	return 0;
}

int pcl_menu(lua_State *state, void *self) {
	USER *sd = (USER*)self;
	sl_checkargs(state, "st");
	int size = lua_objlen(state, sl_memberarg(2));
	char *menuopts[size + 1];
	lua_pushnil(state);
	
	while(lua_next(state, sl_memberarg(2)) != 0) {
		// key is at -2, value at -1
		int index = lua_tonumber(state, -2);
		//if(index<size) {
		menuopts[index] = lua_tostring(state, -1);
		//}
		lua_pop(state, 1);
	}
	
	char *topic = lua_tostring(state, sl_memberarg(1));
	clif_scriptmenu(sd, sd->last_click, topic, menuopts, size);
	//sd->coroutine = state; TEMP
	return lua_yield(state, 0);
}

int pcl_sendminitext(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	char* msg;

	msg=lua_tostring(state,sl_memberarg(1));

	clif_sendminitext(sd,msg,strlen(msg));
	return 0;
}



int pcl_setduration(lua_State *state, USER *sd) {
	sl_checkargs(state, "sn");
	struct block_list *bl = NULL;
	int id,time,x,caster_id,recast,alreadycast, mid;
	
	alreadycast = 0;
	id=magicdb_id(lua_tostring(state,sl_memberarg(1)));
	time=lua_tonumber(state,sl_memberarg(2));
	if (time < 1000 && time > 0) time = 1000;
	caster_id=lua_tonumber(state,sl_memberarg(3)); //sets duration from specific caster
	recast=lua_tonumber(state,sl_memberarg(4));
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
		if(sd->status.dura_aether[x].id == id && sd->status.dura_aether[x].caster_id == caster_id && sd->status.dura_aether[x].duration > 0) {
			alreadycast = 1;
			break;
		}
	}
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) { //MAX_DURATIMER
		mid = sd->status.dura_aether[x].id;
		if (mid == id && time <= 0 && sd->status.dura_aether[x].caster_id == caster_id && alreadycast == 1) {
			clif_send_duration(sd, id, time, map_id2sd(caster_id));
			sd->status.dura_aether[x].duration = 0;
			sd->status.dura_aether[x].caster_id = 0;
			map_foreachinarea(clif_sendanimation, sd->bl.m, sd->bl.x, sd->bl.y ,AREA, BL_PC, sd->status.dura_aether[x].animation, &sd->bl, -1);
			sd->status.dura_aether[x].animation = 0;
			
			if(sd->status.dura_aether[x].aether == 0) {
				sd->status.dura_aether[x].id = 0;
			}
			
			if(sd->status.dura_aether[x].caster_id != sd->bl.id) {
				bl = map_id2bl(sd->status.dura_aether[x].caster_id);
			}
			
			if(bl != NULL) {
				sl_doscript_blargs(magicdb_yname(mid), "uncast", 2, &sd->bl, bl);
			} else {
				sl_doscript_blargs(magicdb_yname(mid), "uncast", 1, &sd->bl);
			}
			
			return 0;
		} else if (sd->status.dura_aether[x].id == id && sd->status.dura_aether[x].caster_id == caster_id && sd->status.dura_aether[x].aether > 0 && sd->status.dura_aether[x].duration <= 0) {
			sd->status.dura_aether[x].duration = time;
			clif_send_duration(sd, id, time / 1000, map_id2sd(caster_id));
			return 0;
		} else if (sd->status.dura_aether[x].id == id && sd->status.dura_aether[x].caster_id == caster_id && (sd->status.dura_aether[x].duration > time || recast == 1) && alreadycast == 1) {
			sd->status.dura_aether[x].duration = time;
			clif_send_duration(sd, id, time / 1000, map_id2sd(caster_id));
			return 0;
		} else if(sd->status.dura_aether[x].id == 0 && sd->status.dura_aether[x].duration == 0 && time != 0 && alreadycast != 1) {
			//if (sd->status.dura_aether[x].caster_id != caster_id && caster_id > 0) {
			//	sd->status.dura_aether[x].id = id;
			//	sd->status.dura_aether[x].duration = time;
			//	sd->status.dura_aether[x].caster_id = caster_id;
			//	clif_send_duration(sd, id, time / 1000);
			//} else if (caster_id == 0) {
			sd->status.dura_aether[x].id = id;
			sd->status.dura_aether[x].duration = time;
			sd->status.dura_aether[x].caster_id = caster_id;
			clif_send_duration(sd, id, time / 1000, map_id2sd(caster_id));
			//}

			return 0;
		}
	}
	
	printf("I didn't set duration... id: %u, time: %u, caster: %u, recast: %u, alreadycast: %u\n", id, time, caster_id, recast, alreadycast);
	return 0;
}

int pcl_setaether(lua_State *state, USER *sd) {
	sl_checkargs(state, "sn");
	int id;
	int time;
	int x;
	int alreadycast;

	id = magicdb_id(lua_tostring(state, sl_memberarg(1)));
	time = lua_tonumber(state, sl_memberarg(2));
	if (time < 1000 && time > 0) time = 1000;
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
		if (sd->status.dura_aether[x].id == id) {
			alreadycast = 1;
		}
	}

	for(x = 0; x < MAX_MAGIC_TIMERS; x++) {
		if(sd->status.dura_aether[x].id == id && time <= 0) {
			clif_send_aether(sd, id, time);
			
			if (sd->status.dura_aether[x].duration == 0) {
				sd->status.dura_aether[x].id = 0;
			}
			
			sd->status.dura_aether[x].aether = 0;
			return 0;
		}
		else if (sd->status.dura_aether[x].id == id && (sd->status.dura_aether[x].aether > time || sd->status.dura_aether[x].duration > 0)) {
			sd->status.dura_aether[x].aether = time;
            clif_send_aether(sd, id, time / 1000);
			return 0;
		}
		else if (sd->status.dura_aether[x].id == 0 && sd->status.dura_aether[x].aether == 0 && time != 0 && alreadycast != 1) {
			sd->status.dura_aether[x].id = id;
			sd->status.dura_aether[x].aether = time;
			clif_send_aether(sd, id, time / 1000);
			return 0;
		}
	}
	return 0;
}
int pcl_hasaether(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int x,id;

	id=magicdb_id(lua_tostring(state,sl_memberarg(1)));

	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
		if(sd->status.dura_aether[x].id==id) {
			if(sd->status.dura_aether[x].aether>0) {
				lua_pushboolean(state,1);
				return 1;
			}
		}
	}
	lua_pushboolean(state,0);
	return 1;
}
int pcl_getaether(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int x, id;
	
	id = magicdb_id(lua_tostring(state, sl_memberarg(1)));
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
		if (sd->status.dura_aether[x].id == id) {
			if (sd->status.dura_aether[x].aether > 0) {
				lua_pushnumber(state, sd->status.dura_aether[x].aether);
				return 1;
			}
		}
	}
	
	lua_pushnumber(state, 0);
	return 1;
}
int pcl_getallaethers(lua_State* state, USER* sd) {
	int i, x;
	lua_newtable(state);
	
	for (i = 0, x = 1; i < MAX_MAGIC_TIMERS; i++, x += 2) {
		if (sd->status.dura_aether[i].id > 0 && sd->status.dura_aether[i].aether > 0) {
			lua_pushnumber(state, sd->status.dura_aether[i].aether);
			lua_rawseti(state, -2, x);
			lua_pushstring(state, magicdb_yname(sd->status.dura_aether[i].id));
			lua_rawseti(state, -2, x + 1);
		}
	}
	
	return 1;
}
int pcl_hasduration(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int x,id;

	id=magicdb_id(lua_tostring(state,sl_memberarg(1)));
	
	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
		if(sd->status.dura_aether[x].id==id) {
			if(sd->status.dura_aether[x].duration>0) {
				lua_pushboolean(state,1);
				return 1;
			}
		}
	}
	lua_pushboolean(state,0);
	return 1;
}
int pcl_hasdurationid(lua_State *state, USER *sd) {
	sl_checkargs(state, "sn");
	int x,id,caster_id;

	caster_id = 0;

	id=magicdb_id(lua_tostring(state,sl_memberarg(1)));
	caster_id=lua_tonumber(state,sl_memberarg(2)); //checks for duration from specific caster

	for(x=0;x<MAX_MAGIC_TIMERS;x++) {
		if(sd->status.dura_aether[x].id==id && sd->status.dura_aether[x].caster_id == caster_id) {
			if(sd->status.dura_aether[x].duration>0) {
				lua_pushboolean(state,1);
				return 1;
			}
		}
	}
	lua_pushboolean(state,0);
	return 1;
}
int pcl_getduration(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int i, id;

	id = magicdb_id(lua_tostring(state, sl_memberarg(1)));
	
	for (i = 0; i < MAX_MAGIC_TIMERS; i++) {
		if (sd->status.dura_aether[i].id == id) {
			if (sd->status.dura_aether[i].duration > 0) {
				lua_pushnumber(state, sd->status.dura_aether[i].duration);
				return 1;
			}
		}
	}
	
	lua_pushnumber(state,0);
	return 1;
}
int pcl_getdurationid(lua_State *state, USER *sd) {
	sl_checkargs(state, "sn");
	int i, id, caster_id;
	
	caster_id = 0;
	id = magicdb_id(lua_tostring(state, sl_memberarg(1)));
	caster_id = lua_tonumber(state, sl_memberarg(2));
	
	for (i = 0; i < MAX_MAGIC_TIMERS; i++) {
		if (sd->status.dura_aether[i].id == id && sd->status.dura_aether[i].caster_id == caster_id) {
			if (sd->status.dura_aether[i].duration > 0) {
				lua_pushnumber(state, sd->status.dura_aether[i].duration);
				return 1;
			}
		}
	}
	
	lua_pushnumber(state, 0);
	return 1;
}
int pcl_getalldurations(lua_State* state, USER* sd) {
	int i, x;
	lua_newtable(state);
	
	for (i = 0, x = 1; i < MAX_MAGIC_TIMERS; i++, x++) {
		if (sd->status.dura_aether[i].id > 0 && sd->status.dura_aether[i].duration > 0) {
			lua_pushstring(state, magicdb_yname(sd->status.dura_aether[i].id));
			lua_rawseti(state, -2, x);
		}
	}
	
	return 1;
}
int pcl_addlegend(lua_State *state, USER *sd) {
	sl_checkargs(state,"ssnn");
	int icon,color,x;
	int found=0;
	
	icon=lua_tonumber(state,sl_memberarg(3));
	color=lua_tonumber(state,sl_memberarg(4));



	for(x=0;x<MAX_LEGENDS;x++) {
		if(strcmpi(sd->status.legends[x].name,"") == 0 && strcmpi(sd->status.legends[x+1].name,"") == 0) {
			strcpy(sd->status.legends[x].text,lua_tostring(state,sl_memberarg(1)));
			strcpy(sd->status.legends[x].name,lua_tostring(state,sl_memberarg(2)));
			sd->status.legends[x].icon = icon;
			sd->status.legends[x].color = color;
			return 0;
		}
	}
	//old methodology.
	/*for(x=0;x<200;x++) {
		if(strlen(sd->status.legends[x].text)<1) {
			strcpy(sd->status.legends[x].text,lua_tostring(state,sl_memberarg(1)));
			strcpy(sd->status.legends[x].name,lua_tostring(state,sl_memberarg(2)));
			sd->status.legends[x].icon=icon;
			sd->status.legends[x].color=color;
			return 0;
		}
		if(!strcmpi(sd->status.legends[x].name,lua_tostring(state,sl_memberarg(2)))) {
			strcpy(sd->status.legends[x].text,lua_tostring(state,sl_memberarg(1)));
			sd->status.legends[x].icon=icon;
			sd->status.legends[x].color=color;
			return 0;
		}
	}*/
	return 0;
}
int pcl_haslegend(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int i;
	char* legend = lua_tostring(state, sl_memberarg(1));
	
	for (i = 0; i < MAX_LEGENDS; i++) {
		if (!strcmp(sd->status.legends[i].name, legend)) {
			lua_pushboolean(state, 1);
			return 1;
		}
	}
	
	lua_pushboolean(state, 0);
	return 1;
}
int pcl_getlegend(lua_State *state, USER *sd) {
	sl_checkargs(state, "s");
	int i;
	char* legend = lua_tostring(state, sl_memberarg(1));
	
	for (i = 0; i < MAX_LEGENDS; i++) {
		if (!strcmp(sd->status.legends[i].name, legend)) {
			lua_pushstring(state, sd->status.legends[i].text);
			return 1;
		}
	}
	
	lua_pushnil(state);
	return 1;
}
int pcl_buy(lua_State *state, USER *sd) {
	sl_checkargs(state,"stt");
	int x;
	unsigned int amount=lua_objlen(state,sl_memberarg(2));
	struct item_data *item[amount+1];
	int var2[amount+1];
	char* dialog;

	lua_pushnil(state);
	while(lua_next(state, sl_memberarg(2)) != 0) {
		// key is at -2, value at -1
		int index = lua_tonumber(state, -2);

			item[index-1] = itemdb_search(itemdb_id(lua_tostring(state, -1)));
		//var2[index-1]=1;
		lua_pop(state, 1);
	}

	lua_pushnil(state);
	while(lua_next(state, sl_memberarg(3)) != 0) {
		int index=lua_tonumber(state,-2);
		var2[index-1]=lua_tonumber(state,-1);
		lua_pop(state,1);
	}
	
	lua_pushnil(state);
	while(lua_next(state, sl_memberarg(4)) != 0) {
		int index = lua_tonumber(state, -2);
		
		strcpy(item[index - 1]->real_name, lua_tostring(state, -1));
		lua_pop(state, 1);
	}
	
	lua_pushnil(state);
	while(lua_next(state, sl_memberarg(5)) != 0) {
		int index = lua_tonumber(state, -2);
		
		strcpy(item[index - 1]->buytext, lua_tostring(state, -1));
		lua_pop(state, 1);
	}

	dialog=lua_tostring(state,sl_memberarg(1));

	clif_buydialog(sd, sd->last_click, dialog, item, var2, amount);

	// sd->coroutine=state;//sd->coroutine=state; TEMP

	return lua_yield(state,0);

}

int pcl_mapselect(lua_State *state, USER *sd) {
	sl_checkargs(state,"st");
	int index=0;
	int key=0;
	char* wm = NULL;
	int map_x0[255];
	int map_y0[255];
	char* map_name[255];
	int map_id[255];
	int map_x1[255];
	int map_y1[255];

	wm=lua_tostring(state,sl_memberarg(1));

	lua_pushnil(state);
	while(lua_next(state,sl_memberarg(2)) !=0) {
		if(key==6) {
			key=0;
			index++;
		}

		switch(key) {
		case 0:
			map_x0[index]=lua_tonumber(state,-1);
			break;
		case 1:
			map_y0[index]=lua_tonumber(state,-1);
			break;
		case 2:
			map_name[index]=lua_tostring(state,-1);
			break;
		case 3:
			map_id[index]=lua_tonumber(state,-1);
			break;
		case 4:
			map_x1[index]=lua_tonumber(state,-1);
			break;
		case 5:
			map_y1[index]=lua_tonumber(state,-1);
			break;
		}
		key++;
		lua_pop(state,1);
	}

	clif_mapselect(sd,wm,map_x0,map_y0,map_name,map_id,map_x1,map_y1,index);

	return 0;
}
int pcl_bank(lua_State *state, USER *sd) {
	sl_checkargs(state,"s");
	int x;

	//for(x=0;x<sd->status.bank
}
int pcl_input(lua_State *state, USER *sd) {
	sl_checkargs(state,"s");
	int count=lua_gettop(state);
	char* dialog=lua_tostring(state,sl_memberarg(1));

	lua_pushnil(state);

	//if(count==2) {
		//clif_input(sd,0,dialog,itemdb_name(lua_tonumber(state,2)));
	//} else {

	clif_input(sd,sd->last_click,dialog,"");


	// sd->coroutine=state; TEMP

	return lua_yield(state,0);
}

int pcl_sell(lua_State *state, USER *sd) {
	sl_checkargs(state,"st");
	char* dialog=lua_tostring(state,sl_memberarg(1));
	unsigned int amount=lua_objlen(state,sl_memberarg(2));
	int item[60];
	int taken[26];
	int count=0;
	int lastcount=0;
	unsigned int temp;
	int x,y;
	lua_pushnil(state);
	while(lua_next(state,sl_memberarg(2))!=0) {
		int index=lua_tonumber(state,-2);
		temp=lua_tonumber(state,-1);

		for(x=0;x<sd->status.maxinv;x++) {
			lastcount=0;
			if(sd->status.inventory[x].id==temp) {

				item[count]=x;
				//lastcount=count;
				count++;

			}
		}
		//printf("%d\n",count);
		//if(count>lastcount) item[count]=0;
		lua_pop(state,1);
	}
	// sd->coroutine=state; TEMP
	clif_selldialog(sd, sd->last_click, dialog, item, count);
	return lua_yield(state,0);
}
int pcl_sell2(lua_State *state, USER *sd) {
	sl_checkargs(state,"st");
	char* dialog=lua_tostring(state,sl_memberarg(1));
	unsigned int amount=lua_objlen(state,sl_memberarg(2));
	int item[60];
	int taken[26];
	int count=0;
	int lastcount=0;
	char* temp = NULL;
	int x,y;
	lua_pushnil(state);

	while(lua_next(state,sl_memberarg(2))!=0) {
		int index=lua_tonumber(state,-2);
		temp=lua_tostring(state,-1);


		item[count]=(unsigned int)strtoul(temp, NULL, 10);

					count++;

				//lastcount=count;




		//printf("%d\n",count);
		//if(count>lastcount) item[count]=0;
		lua_pop(state,1);
	}
	// sd->coroutine=state; TEMP
	clif_selldialog(sd, sd->last_click, dialog, item, count);
	return lua_yield(state,0);
}
int pcl_sendstatus(lua_State *state, USER *sd) {
	pc_requestmp(sd);
	clif_sendstatus(sd,SFLAG_FULLSTATS|SFLAG_HPMP|SFLAG_XPMONEY);
	return 0;
}
int pcl_calcstat(lua_State *state, USER *sd) {
	pc_calcstat(sd);
	return 0;
}
int pcl_getinventoryitem(lua_State *state, USER *sd) {
	sl_checkargs(state,"n");
	int num=lua_tonumber(state,sl_memberarg(1));
	if(sd->status.inventory[num].id != 0)
		biteml_pushinst(state, &sd->status.inventory[num]);
	else lua_pushnil(state);
	return 1;
}
int pcl_addinventoryitem(lua_State *state, USER *sd) {
	return 1;
}
int pcl_getequippeditem(lua_State *state, USER *sd) {
	sl_checkargs(state,"n");
	int num=lua_tonumber(state,sl_memberarg(1));
	if(sd->status.equip[num].id != 0)
		biteml_pushinst(state, &sd->status.equip[num]);
	else lua_pushnil(state);
	return 1;
}
int pcl_removelegendbyname(lua_State *state, USER *sd) {
	sl_checkargs(state,"s");
	char* find=lua_tostring(state,sl_memberarg(1));
	int x;

	for(x=0;x<MAX_LEGENDS;x++) {
		if(strcmpi(sd->status.legends[x].name,find) == 0) {
			strcpy(sd->status.legends[x].text,"");
			strcpy(sd->status.legends[x].name,"");
			sd->status.legends[x].icon=0;
			sd->status.legends[x].color=0;
		}
	}
	for(x=0;x<MAX_LEGENDS;x++) {
		if(strcmpi(sd->status.legends[x].name,"") == 0 && strcmpi(sd->status.legends[x+1].name,"") != 0) {
			strcpy(sd->status.legends[x].text,sd->status.legends[x+1].text);
			strcpy(sd->status.legends[x+1].text,"");
			strcpy(sd->status.legends[x].name,sd->status.legends[x+1].name);
			strcpy(sd->status.legends[x+1].name,"");
			sd->status.legends[x].icon = sd->status.legends[x+1].icon;
			sd->status.legends[x+1].icon = 0;
			sd->status.legends[x].color = sd->status.legends[x+1].color;
			sd->status.legends[x+1].color =0;
		}
	}
	return 0;
}

int pcl_removelegendbycolor(lua_State *state, USER *sd) {
	sl_checkargs(state,"n");
	int find=lua_tonumber(state,sl_memberarg(1));
	int x,count=0;

	for(x=0;x<MAX_LEGENDS;x++) {
		if(sd->status.legends[x].color==find) {
			count++;
		}
		if(x+count<MAX_LEGENDS) {
			strcpy(sd->status.legends[x].text,sd->status.legends[x+count].text);
			strcpy(sd->status.legends[x].name,sd->status.legends[x+count].name);
			sd->status.legends[x].icon=sd->status.legends[x+count].icon;
			sd->status.legends[x].color=sd->status.legends[x+count].color;
		}
	}
	for(x=0;x<MAX_LEGENDS;x++) {
		if(strcmpi(sd->status.legends[x].name,"") == 0 && strcmpi(sd->status.legends[x+1].name,"") != 0) {
			strcpy(sd->status.legends[x].text,sd->status.legends[x+1].text);
			strcpy(sd->status.legends[x+1].text,"");
			strcpy(sd->status.legends[x].name,sd->status.legends[x+1].name);
			strcpy(sd->status.legends[x+1].name,"");
			sd->status.legends[x].icon = sd->status.legends[x+1].icon;
			sd->status.legends[x+1].icon = 0;
			sd->status.legends[x].color = sd->status.legends[x+1].color;
			sd->status.legends[x+1].color =0;
		}
	}
	return 0;
}

int pcl_removeinventoryitem(lua_State *state, USER *sd) {
	unsigned int id = 0, amount = 0, owner = 0,max = 0;
	int x, count = 0, type = 0;
	char *engrave = NULL;

	if (lua_isnumber(state, sl_memberarg(1)))
	{
		sl_checkargs(state, "nn");
		id = lua_tonumber(state, sl_memberarg(1));
	}
	else
	{
		sl_checkargs(state,"sn");
		id = itemdb_id(lua_tostring(state, sl_memberarg(1)));
	}
	
	amount = lua_tonumber(state,sl_memberarg(2));
	type = lua_tonumber(state, sl_memberarg(3));
	owner = lua_tonumber(state, sl_memberarg(4));
	engrave = lua_tostring(state, sl_memberarg(5));
	max = amount;
	
	if (!engrave)
	{
		CALLOC(engrave, char, 64);
	}
	
	for (x = 0; x < sd->status.maxinv; x++)
	{
		if (sd->status.inventory[x].id == id && sd->status.inventory[x].amount < itemdb_maxamount(id) && sd->status.inventory[x].owner_id == owner && !(strcmpi(sd->status.inventory[x].real_name, engrave)))
		{
			if (sd->status.inventory[x].amount < amount && sd->status.inventory[x].amount > 0)
			{
				amount -= sd->status.inventory[x].amount;
				pc_delitem(sd, x, sd->status.inventory[x].amount, type);
				
				if (amount == 0)
				{
					lua_pushboolean(state, 1);
					return 1;
				}
			}
			else if (sd->status.inventory[x].amount >= amount)
			{
				pc_delitem(sd, x, amount, type);
				lua_pushboolean(state, 1);
				return 1;
			}
		}
	}

	for (x = 0; x < sd->status.maxinv; x++)
	{
		if (sd->status.inventory[x].id == id && sd->status.inventory[x].owner_id == owner && !(strcmpi(sd->status.inventory[x].real_name, engrave)))
		{
			if (sd->status.inventory[x].amount < amount && sd->status.inventory[x].amount > 0)
			{
				amount -= sd->status.inventory[x].amount;
				pc_delitem(sd, x, sd->status.inventory[x].amount, type);
				
				if (amount == 0)
				{
					lua_pushboolean(state, 1);
					return 1;
				}
			}
			else if (sd->status.inventory[x].amount >= amount)
			{
				pc_delitem(sd, x, amount, type);
				lua_pushboolean(state, 1);
				return 1;
			}
		}
	}

	if (engrave)
	{
		engrave = NULL;
	}
	else
	{
		FREE(engrave);
	}
	
	lua_pushboolean(state, 0);
	return 1;
}
int pcl_removeitemslot(lua_State *state, USER *sd) {
	sl_checkargs(state, "nn");
	int slot,amount,max,type;

	slot=lua_tonumber(state,sl_memberarg(1));
	amount=lua_tonumber(state,sl_memberarg(2));
	type=lua_tonumber(state,sl_memberarg(3));
	max=amount;

	if(sd->status.inventory[slot].id > 0) {
		if((sd->status.inventory[slot].amount<amount) && (sd->status.inventory[slot].amount>0)) {
				//count+=sd->status.inventory[x].amount;
				amount-=sd->status.inventory[slot].amount;
				if (type == 1) {
					pc_delitem(sd,slot,sd->status.inventory[slot].amount,10);
				} else if (type == 2) {
					pc_delitem(sd,slot,sd->status.inventory[slot].amount,9);
				} else {				
					pc_delitem(sd,slot,sd->status.inventory[slot].amount,11);
				}
				
				if(amount==0) {
					lua_pushboolean(state,1);
					return 1;
				}

		} else if(sd->status.inventory[slot].amount>=amount){
				//count+=amount;
				if (type == 1) {
					pc_delitem(sd,slot,amount,10);
				} else if (type == 2) {
					pc_delitem(sd,slot,amount,9);
				} else {				
					pc_delitem(sd,slot,amount,11);
				}
				
				lua_pushboolean(state,1);
				return 1;
		}
	}


	lua_pushboolean(state,0);
	return 1;
}

int pcl_removeitemdura(lua_State *state, USER *sd) {
	unsigned int id=0;
	unsigned int amount=0,max=0;
	int x;
	int count=0;
	int type=0;

	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "nn");
		id = lua_tonumber(state, sl_memberarg(1));
	} else {
		sl_checkargs(state,"sn");
		id=itemdb_id(lua_tostring(state,sl_memberarg(1)));
	}
	amount=lua_tonumber(state,sl_memberarg(2));
	type=lua_tonumber(state,sl_memberarg(3));
	max=amount;

	for(x=0;x<sd->status.maxinv;x++) {
		if(sd->status.inventory[x].id==id) {
			if((sd->status.inventory[x].amount<amount) && (sd->status.inventory[x].amount>0) && (sd->status.inventory[x].dura==itemdb_dura(sd->status.inventory[x].id))) {
				//count+=sd->status.inventory[x].amount;
				amount-=sd->status.inventory[x].amount;
				if (type == 1) {
					pc_delitem(sd,x,sd->status.inventory[x].amount,10);
				} else if (type == 2) {
					pc_delitem(sd,x,sd->status.inventory[x].amount,9);
				} else {				
					pc_delitem(sd,x,sd->status.inventory[x].amount,11);
				}
				
				if(amount==0) {
					lua_pushboolean(state,1);
					return 1;
				}

			} else if(sd->status.inventory[x].amount>=amount && (sd->status.inventory[x].dura==itemdb_dura(sd->status.inventory[x].id))){
				//count+=amount;
				if (type == 1) {
					pc_delitem(sd,x,amount,10);
				} else if (type == 2) {
					pc_delitem(sd,x,amount,9);
				} else {				
					pc_delitem(sd,x,amount,11);
				}
				
				lua_pushboolean(state,1);
				return 1;
			}
		}
	}


	lua_pushboolean(state,0);
	return 1;
}

int pcl_additem(lua_State *state, USER* sd) {
	unsigned int id=0;
	unsigned int amount=0;
	int dura=0;
	int owner=0;
	struct item *fl;
	char *engrave = NULL;
	
	CALLOC(fl,struct item,1);
	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "nn");
		id = lua_tonumber(state, sl_memberarg(1));
	} else {
		sl_checkargs(state, "sn");
		id=itemdb_id(lua_tostring(state,sl_memberarg(1)));
	}
	amount=lua_tonumber(state,sl_memberarg(2));
	dura=lua_tonumber(state,sl_memberarg(3));
	owner=lua_tonumber(state,sl_memberarg(4));
	engrave=lua_tostring(state,sl_memberarg(5));

	fl->id=id;
	fl->amount=amount;

	if(owner) {
		fl->owner_id=owner;
	} else {
		fl->owner_id=0;
	}
	if(engrave) {
		strcpy(fl->real_name,engrave);
	} else {
		strcpy(fl->real_name,"");
	}
	if(dura) {
		fl->dura=dura;
	} else {
		fl->dura=itemdb_dura(id);
	}

	//map_additem(&fl->bl);
	if(!pc_additem(sd,fl)) {
		lua_pushboolean(state,1);
	} else {
		lua_pushboolean(state,0);
	}
	FREE(fl);
	return 1;

}

int pcl_showboard(lua_State *state, USER* sd) {
	sl_checkargs(state,"n");
	sd->bcount=0;
	sd->board_popup=1;
	boards_showposts(sd,lua_tonumber(state,sl_memberarg(1)));
	return 0;
}

int pcl_showpost(lua_State *state, USER* sd) {
	sl_checkargs(state, "nn");
	sd->bcount = 0;
	sd->board_popup = 1;
	boards_readpost(sd, lua_tonumber(state, sl_memberarg(1)), lua_tonumber(state, sl_memberarg(2)));
	return 0;
}

int pcl_hasitem(lua_State *state, USER* sd) {
	unsigned int id = 0, amount = 0, leftover = 0, owner = 0;
	int x, count = 0;
	char *engrave = NULL;

	if (lua_isnumber(state, sl_memberarg(1)))
	{
		sl_checkargs(state, "nn");
		id = lua_tonumber(state, sl_memberarg(1));
	}
	else
	{
		sl_checkargs(state, "sn");
		id = itemdb_id(lua_tostring(state,sl_memberarg(1)));
	}
	amount=abs(lua_tonumber(state,sl_memberarg(2)));
	owner = lua_tonumber(state, sl_memberarg(3));
	engrave = lua_tostring(state, sl_memberarg(4));
	leftover = amount;
	
	if (!engrave)
	{
		CALLOC(engrave, char, 64);
	}

	for (x = 0; x < sd->status.maxinv; x++)
	{
		if (sd->status.inventory[x].id == id && sd->status.inventory[x].owner_id == owner && !(strcmpi(sd->status.inventory[x].real_name, engrave)))
		{
			if (sd->status.inventory[x].amount < amount && sd->status.inventory[x].amount > 0)
			{
				amount -= sd->status.inventory[x].amount;
				
				if (amount == 0)
				{
					lua_pushboolean(state, 1);
					return 1;
				}
			}
			else if(sd->status.inventory[x].amount>=amount)
			{
				lua_pushboolean(state, 1);
				return 1;
			}
		}
	}

	if (engrave)
	{
		engrave = NULL;
	}
	else
	{
		FREE(engrave);
	}
	
	leftover -= amount;
	lua_pushnumber(state, leftover);
	return 1;
}
int pcl_hasitemdura(lua_State *state, USER* sd) {
	unsigned int id=0;
	unsigned int amount=0,leftover=0;
	int x;
	int count=0;
	
	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "nn");
		id = lua_tonumber(state, sl_memberarg(1));
	} else {
		sl_checkargs(state, "sn");
		id=itemdb_id(lua_tostring(state,sl_memberarg(1)));
	}
	amount=abs(lua_tonumber(state,sl_memberarg(2)));
	leftover=amount;

	for(x=0;x<sd->status.maxinv;x++) {
		if(sd->status.inventory[x].id==id) {
			if((sd->status.inventory[x].amount<amount) && (sd->status.inventory[x].amount>0) && (sd->status.inventory[x].dura==itemdb_dura(sd->status.inventory[x].id))) {
				amount-=sd->status.inventory[x].amount;
				//pc_delitem(sd,x,sd->status.inventory[x].amount,itemdb_type(sd->status.inventory[x].id));
				if(amount==0) {
					lua_pushboolean(state,1);
					return 1;
				}

			} else if(sd->status.inventory[x].amount>=amount && (sd->status.inventory[x].dura==itemdb_dura(sd->status.inventory[x].id))){
				//count+=amount;
				//pc_delitem(sd,x,count,itemdb_type(sd->status.inventory[x].id));
				lua_pushboolean(state,1);
				return 1;
			}
		}
	}

	leftover-=amount;
	lua_pushnumber(state,leftover);
	return 1;
}

int pcl_addspell(lua_State* state, USER *sd) {
	sl_checkargs(state,"s");
	int spell=magicdb_id(lua_tostring(state,sl_memberarg(1)));
	//char name[32];
	int x;
	//if(sscanf(line,"%d",&spell)<1)
	//	return -1;
	for(x=0;x<52;x++) {
		if(sd->status.skill[x]==spell) {
			return 0;
		}
	}
	for(x=0;x<52;x++) {
		if(sd->status.skill[x]<=0) { //vacant spot
			sl_doscript_blargs(magicdb_yname(spell), "on_learn", 1, &sd->bl);
			sd->status.skill[x]=spell;
			pc_loadmagic(sd);
			break;
		}
		if(sd->status.skill[x]==spell) {
			break;
		}
	}

	return 0;

}
int pcl_hasspell(lua_State* state, USER *sd) {
	sl_checkargs(state,"s");
	int spell=magicdb_id(lua_tostring(state,sl_memberarg(1)));
	//char name[32];
	int x;
	//if(sscanf(line,"%d",&spell)<1)
	//	return -1;

	for(x=0;x<52;x++) {
		if(sd->status.skill[x]==spell) {
			lua_pushboolean(state,1);
			return 1;
		}
	}

	lua_pushboolean(state,0);
	return 1;
}
int pcl_getspells(lua_State* state, USER *sd) {
	int i, x;
	lua_newtable(state);
	
	for (x = 0, i = 1; x < MAX_SPELLS; x++) {
		if (sd->status.skill[x]) {
			lua_pushnumber(state, sd->status.skill[x]);
			lua_rawseti(state, -2, i);
			i++;
		}
	}
	
	return 1;
}
int pcl_getspellname(lua_State* state, USER *sd) {
	int i, x;
	lua_newtable(state);

	for (x = 0, i = 1; x < MAX_SPELLS; x++) {
		if (sd->status.skill[x]) {
			lua_pushstring(state, magicdb_name(sd->status.skill[x]));
			lua_rawseti(state, -2, i);
			i++;
		}
	}

	return 1;
}
int pcl_getspellyname(lua_State* state, USER* sd) {
	int i, x;
	lua_newtable(state);
	
	for (x = 0, i = 1; x < MAX_SPELLS; x++) {
		if (sd->status.skill[x]) {
			lua_pushstring(state, magicdb_yname(sd->status.skill[x]));
			lua_rawseti(state, -2, i);
			i++;
		}
	}
	
	return 1;
}
int pcl_hasspace(lua_State* state, USER *sd) {
	unsigned int id = 0;
	int a,x;
	char *engrave = NULL;
	char engraved = 0;
	
	if (lua_isnumber(state, sl_memberarg(1))) {
		sl_checkargs(state, "nn");
		id = lua_tonumber(state, sl_memberarg(1));
	} else {
		sl_checkargs(state, "sn");
		id=itemdb_id(lua_tostring(state,sl_memberarg(1)));
	}
	int amount=lua_tonumber(state,sl_memberarg(2));
	int owner=lua_tonumber(state,sl_memberarg(3));
	
	engrave = lua_tostring(state,sl_memberarg(4));

	if (!engrave)
	{
		CALLOC(engrave, char, 64);
	}

	if(pc_isinvenspace(sd,id, owner, engrave)>=sd->status.maxinv) {
		lua_pushboolean(state, 0);
		return 1;
	}
	
	for (x = 0; x < sd->status.maxinv; x++) {
		if (!engrave) {
			engraved = -1;
		} else {
			engraved = strcmpi(sd->status.inventory[x].real_name, engrave);
		}
		
		if (sd->status.inventory[x].id == id && sd->status.inventory[x].owner_id == owner && engraved == 0) {
			if (sd->status.inventory[x].amount < itemdb_maxamount(id)) {
				amount -= (itemdb_maxamount(id) - sd->status.inventory[x].amount);
			}
		}
		
		if (sd->status.inventory[x].id == id && sd->status.inventory[x].owner_id == owner && engraved != 0) {
			if(sd->status.inventory[x].amount < itemdb_maxamount(id)) {
				amount -= (itemdb_maxamount(id) - sd->status.inventory[x].amount);
			}
		}
		
		if (sd->status.inventory[x].id <= 0) {
			if (amount <= itemdb_maxamount(id)) {
				amount = 0;
			} else {
				amount -= itemdb_maxamount(id);
			}
		}
		
		if (amount <= 0) {
			lua_pushboolean(state, 1);
			return 1;
		}
	}
		
	if (engrave)
	{
		engrave = NULL;
	}
	else
	{
		FREE(engrave);
	}
	
	lua_pushboolean(state,0);
	return 1;

}
int pcl_removespell(lua_State* state, USER *sd) {
	int spell;
	int x;

	if (lua_isnumber(state, sl_memberarg(1))) {
		spell = lua_tonumber(state, sl_memberarg(1));
	} else if (lua_isstring(state, sl_memberarg(1))) {
		spell = magicdb_id(lua_tostring(state, sl_memberarg(1)));
	} else {
		lua_pushnil(state);
		return 0;
	}
	
	for (x = 0; x < MAX_SPELLS; x++) {
		if (sd->status.skill[x] == spell) {
			sl_doscript_blargs(magicdb_yname(spell), "on_forget", 1, &sd->bl);
			clif_removespell(sd, x);
			sd->status.skill[x] = 0;
			return 0;
		}
	}

	return 0;
}
int pcl_powerboard(lua_State* state, USER *sd) {
	clif_sendpowerboard(sd);
}
//check bank items
int pcl_checkbankitems(lua_State* state, USER *sd) {
	int i;
	lua_newtable(state);
	
	for (i = 0; i < MAX_BANK_SLOTS; i++) {
		if (sd->status.banks[i].item_id > 0) {
			lua_pushnumber(state, sd->status.banks[i].item_id);
			lua_rawseti(state, -2, i + 1);
		} else {
			lua_pushnumber(state, 0);
			lua_rawseti(state, -2, i + 1);
		}
	}
	
	return 1;
}
//check bank amounts
int pcl_checkbankamounts(lua_State* state, USER *sd) {
	int i;
	lua_newtable(state);
	
	for (i = 0; i < MAX_BANK_SLOTS; i++) {
		if(sd->status.banks[i].amount > 0) {
			lua_pushnumber(state, sd->status.banks[i].amount);
			lua_rawseti(state, -2, i + 1);
		} else {
			lua_pushnumber(state, 0);
			lua_rawseti(state, -2, i + 1);
		}
	}
	
	return 1;
}
//check bank owners
int pcl_checkbankowners(lua_State* state, USER *sd) {
	int i;
	lua_newtable(state);

	for (i = 0; i < MAX_BANK_SLOTS; i++) {
		if (sd->status.banks[i].owner_id > 0) {
			lua_pushnumber(state, sd->status.banks[i].owner_id);
			lua_rawseti(state, -2, i + 1);
		} else {
			lua_pushnumber(state, 0);
			lua_rawseti(state, -2, i + 1);
		}
	}
	
	return 1;
}
//check bank engraves
int pcl_checkbankengraves(lua_State* state, USER *sd) {
	int i;
	lua_newtable(state);

	for (i = 0; i < MAX_BANK_SLOTS; i++) {
		if (strcmpi(sd->status.banks[i].real_name, "") != 0){
			lua_pushstring(state, sd->status.banks[i].real_name);
			lua_rawseti(state, -2, i + 1);
		} else {
			lua_pushstring(state, "");
			lua_rawseti(state, -2, i + 1);
		}
	}
	
	return 1;
}
//deposit item
int pcl_bankdeposit(lua_State* state, USER *sd) {
	sl_checkargs(state, "nn");
	int x, deposit, amount, owner;
	unsigned int item;
	char *engrave = NULL;
	
	deposit = -1;
	item = lua_tonumber(state, sl_memberarg(1));
	amount = lua_tonumber(state, sl_memberarg(2));
	owner = lua_tonumber(state, sl_memberarg(3));
	engrave = lua_tostring(state, sl_memberarg(4));
	
	for (x = 0; x < MAX_BANK_SLOTS;x++) {
		if (sd->status.banks[x].item_id == item && sd->status.banks[x].owner_id == owner && !strcmpi(sd->status.banks[x].real_name, engrave)) {
			deposit = x;
			break;
		}
	}

	if (deposit != -1) {
		sd->status.banks[deposit].amount += amount;
	} else {
		for (x = 0; x < MAX_BANK_SLOTS; x++) {
			if (sd->status.banks[x].item_id == 0) {
				sd->status.banks[x].item_id = item;
				sd->status.banks[x].amount = amount;
				sd->status.banks[x].owner_id = owner;
				strcpy(sd->status.banks[x].real_name, engrave);				
				lua_pushboolean(state, 1);
				return 1;
			}
		}
	}
	
	lua_pushboolean(state, 0);
	return 0;
}
//withdraw item
int pcl_bankwithdraw(lua_State* state, USER *sd) {
	sl_checkargs(state, "nn");
	int x, deposit, amount, owner;
	unsigned int item;
	char *engrave=NULL;

	deposit = -1;
	item = lua_tonumber(state, sl_memberarg(1));
	amount = lua_tonumber(state, sl_memberarg(2));
	owner = lua_tonumber(state, sl_memberarg(3));
	engrave = lua_tostring(state, sl_memberarg(4));
	
	for (x = 0; x < MAX_BANK_SLOTS; x++) {
		if (sd->status.banks[x].item_id == item && sd->status.banks[x].owner_id == owner && !strcmpi(sd->status.banks[x].real_name, engrave)) {
			deposit = x;
			break;
		}
	}

	if (deposit != -1) {
		if ((sd->status.banks[deposit].amount - amount) < 0) {
			lua_pushboolean(state, 0);
			return 1;
		} else {
			sd->status.banks[deposit].amount -= amount;
			
			if (sd->status.banks[deposit].amount <= 0) {
				sd->status.banks[deposit].item_id = 0;
				sd->status.banks[deposit].amount = 0;
				sd->status.banks[deposit].owner_id = 0;
				strcpy(sd->status.banks[deposit].real_name, "");
				
				for (x = 0; x < MAX_BANK_SLOTS; x++) {
					if (sd->status.banks[x].item_id == 0 && x < MAX_BANK_SLOTS - 1) {
						sd->status.banks[x].item_id = sd->status.banks[x + 1].item_id;
						sd->status.banks[x + 1].item_id = 0;
						sd->status.banks[x].amount = sd->status.banks[x + 1].amount;
						sd->status.banks[x + 1].amount = 0;
						sd->status.banks[x].owner_id = sd->status.banks[x + 1].owner_id;
						sd->status.banks[x + 1].owner_id = 0;
						strcpy(sd->status.banks[x].real_name, sd->status.banks[x + 1].real_name);
						strcpy(sd->status.banks[x + 1].real_name, "");
					}
				}
				
				lua_pushboolean(state, 1);
				return 1;
			}
		}
	}
	
	lua_pushboolean(state, 0);
	return 1;
}
//check amount in bank
int pcl_checkbankitemamount(lua_State* state, USER *sd) {
	sl_checkargs(state, "n");
	int x, deposit, amount, owner;
	unsigned int item;
	char *engrave = NULL;

	deposit = -1;
	item = lua_tonumber(state, sl_memberarg(1));
	owner = lua_tonumber(state, sl_memberarg(2));
	engrave = lua_tostring(state, sl_memberarg(3));

	for (x = 0; x < MAX_BANK_SLOTS; x++) {
		if (sd->status.banks[x].item_id == item && sd->status.banks[x].owner_id == owner && !strcmpi(sd->status.banks[x].real_name, engrave)) {
			deposit = x;
			break;
		}
	}

	if (deposit != -1) {
		lua_pushnumber(state, sd->status.banks[deposit].amount);
		return 1;
	} else {
		lua_pushboolean(state,0);
		return 1;
	}
}
//move givexp to core to allow calling of pc_givexp
int pcl_givexp(lua_State* state, USER *sd) {
	sl_checkargs(state, "n");
	pc_givexp(sd, lua_tonumber(state, sl_memberarg(1)),xp_rate);
}

int pcl_givexpgroup(lua_State* state, USER *sd) {
	sl_checkargs(state, "n");
	clif_groupexp(sd, lua_tonumber(state, sl_memberarg(1)));
}
//threat table adding
int pcl_addthreat(lua_State* state, USER *sd) {
	sl_checkargs(state, "nn");
	unsigned int id,amount;
	int x;
	MOB *tmob = NULL;
	
	id = lua_tonumber(state, sl_memberarg(1));
	amount = lua_tonumber(state, sl_memberarg(2));
	tmob = (MOB*)map_id2mob(id);
	nullpo_ret(0, tmob);
	tmob->lastaction = time(NULL);
	
	for(x=0;x<MAX_THREATCOUNT && tmob != NULL;x++) {
		if(tmob->threat[x].user == sd->bl.id) {
			tmob->threat[x].amount += amount;
			return 0;
		} else if(tmob->threat[x].user == 0) {
			tmob->threat[x].user = sd->bl.id;
			tmob->threat[x].amount = amount;
			return 0;
		}
	}

	return 0;
}

int pcl_setthreat(lua_State* state, USER *sd) {
	sl_checkargs(state, "nn");
	unsigned int id,amount;
	int x;
	MOB *tmob = NULL;

	id = lua_tonumber(state, sl_memberarg(1));
	amount = lua_tonumber(state, sl_memberarg(2));
	tmob = (MOB*)map_id2mob(id);
	nullpo_ret(0, tmob);
	tmob->lastaction = time(NULL);
	
	for(x=0;x<MAX_THREATCOUNT && tmob != NULL;x++) {
		if(tmob->threat[x].user == sd->bl.id) {
			tmob->threat[x].amount = amount;
			return 0;
		} else if(tmob->threat[x].user == 0) {
			tmob->threat[x].user = sd->bl.id;
			tmob->threat[x].amount = amount;
			return 0;
		}
	}

	return 0;
}

int pcl_addthreatgeneral(lua_State* state, USER *sd) {
	sl_checkargs(state, "n");
	unsigned int amount;
	int i;
	
	amount = lua_tonumber(state, sl_memberarg(1));

	//threat stuff
	int bx, by, nx, ny, x0, x1, y0, y1, m, x, y;
	int nAreaSizeX = AREAX_SIZE, nAreaSizeY = AREAY_SIZE;
	struct block_list *bl = NULL;
	MOB *tmob = NULL;
	
	m = sd->bl.m;
	x = sd->bl.x;
	y = sd->bl.y;
	nx = map[m].xs - x;
	ny = map[m].ys - y;

	if (nx < 18) nAreaSizeX = nAreaSizeX * 2;
	if (ny < 16) nAreaSizeY = nAreaSizeY * 2;
	if (x < 18) nAreaSizeX = nAreaSizeX * 2;
	if (y < 16) nAreaSizeY = nAreaSizeY * 2;

	x0 = x - nAreaSizeX;
	x1 = x + nAreaSizeX;
	y0 = y - nAreaSizeY;
	y1 = y + nAreaSizeY;

	if (x0 < 0) x0 = 0;
	if (y0 < 0) y0 = 0;
	if (x1 >= map[m].xs) x1 = map[m].xs - 1;
	if (y1 >= map[m].ys) y1 = map[m].ys - 1;

	for (by = y0 / BLOCK_SIZE; by <= y1 / BLOCK_SIZE; by++) {
		for (bx = x0 / BLOCK_SIZE; bx <= x1 / BLOCK_SIZE; bx++) {
			for (bl = map[m].block_mob[bx + by * map[m].bxs]; bl; bl = bl->next ) {
				if (bl->type == BL_MOB && bl->x >= x0 && bl->x <= x1 && bl->y >= y0 && bl->y <= y1) {
					tmob = (MOB*)bl;
					if (!tmob) continue;
					tmob->lastaction = time(NULL);
					
					for (i = 0; i < MAX_THREATCOUNT && tmob != NULL; i++) {
						if (tmob->threat[i].user == sd->bl.id) {
							tmob->threat[i].amount += amount;
						} else if (tmob->threat[i].user == 0) {
							tmob->threat[i].user = sd->bl.id;
							tmob->threat[i].amount = amount;
						}
					}
				}
			}
		}
	}
	
	return 0;
}

int pcl_getclanitems(lua_State* state, USER *sd) {
	sl_checkargs(state, "nn");
	int x,id;
	struct clan_data *clan = NULL;

	id=lua_tonumber(state, sl_memberarg(1));
	x=(lua_tonumber(state, sl_memberarg(2))) - 1;

	clan = clandb_search(id);
	if(clan->clanbanks[x].item > 0) {
		lua_pushnumber(state,clan->clanbanks[x].item);
		return 1;
	}
	else {
		lua_pushboolean(state,0);
		return 1;
	}
	return 0;
}

int pcl_getclanamounts(lua_State* state, USER *sd) {
	sl_checkargs(state, "nn");
	int x,id;
	struct clan_data *clan = NULL;

	id=lua_tonumber(state, sl_memberarg(1));
	x=(lua_tonumber(state, sl_memberarg(2))) - 1;

	clan = clandb_search(id);
	if(clan->clanbanks[x].amount > 0) {
		lua_pushnumber(state,clan->clanbanks[x].amount);
		return 1;
	}
	else {
		lua_pushboolean(state,0);
		return 1;
	}
	return 0;
}

int pcl_clanbankdeposit(lua_State* state, USER *sd) {
	sl_checkargs(state, "nnn");
	int x, deposit, item, amount, id;
	struct clan_data *clan = NULL;

	deposit = -1;
	id=lua_tonumber(state, sl_memberarg(1));
	item=lua_tonumber(state, sl_memberarg(2));
	amount=lua_tonumber(state, sl_memberarg(3));
	
	clan = clandb_search(id);
	for (x=0;x<clan->maxslots;x++) {
		if (clan->clanbanks[x].item == item) {
			deposit = x;
			break;
		}
	}
	if (deposit != -1) {
		//if ((clan->clanbanks[deposit].amount + amount) > clan->maxperslot) {
		//	lua_pushboolean(state, 1);
		//	return 1;
		//} else {
			clan->clanbanks[deposit].amount += amount;
			map_saveclanbanks(id);
		//}
	} else {
		for (x=0;x<clan->maxslots;x++) {
			if (clan->clanbanks[x].item == 0) {
				clan->clanbanks[x].item = item;
				clan->clanbanks[x].amount = amount;
				map_saveclanbanks(id);
				break;
			}
		}
		lua_pushboolean(state, 0);
		return 1;
	}
	return 0;
}

int pcl_clanbankwithdraw(lua_State* state, USER *sd) {
	sl_checkargs(state, "nnn");
	int x, deposit, item, amount, id, i;
	struct clan_data *clan = NULL;

	deposit = -1;
	id=lua_tonumber(state, sl_memberarg(1));
	item=lua_tonumber(state, sl_memberarg(2));
	amount=lua_tonumber(state, sl_memberarg(3));
	
	clan = clandb_search(id);
	for (x=0;x<clan->maxslots;x++) {
		if (clan->clanbanks[x].item == item) {
			deposit = x;
			break;
		}
	}

	if (deposit != -1) {
		if ((clan->clanbanks[deposit].amount - amount) < 0) {
			lua_pushboolean(state, 1);
			return 1;
		} else {
			clan->clanbanks[deposit].amount -= amount;
			if (clan->clanbanks[deposit].amount <= 0) {
				clan->clanbanks[deposit].item = 0;
				clan->clanbanks[deposit].amount = 0;
				for(i=0;i<clan->maxslots;i++) {
					if(clan->clanbanks[x].item == 0) {
						clan->clanbanks[x].item = clan->clanbanks[x+1].item;
						clan->clanbanks[x+1].item = 0;
						clan->clanbanks[x].amount = clan->clanbanks[x+1].amount;
						clan->clanbanks[x+1].amount = 0;
					}
				}
				map_saveclanbanks(id);
			}
		}
	}
	return 0;
}

int pcl_checkclanitemamounts(lua_State* state, USER *sd) {
	sl_checkargs(state, "nn");
	int x, deposit, item, amount, id;
	struct clan_data *clan = NULL;

	deposit = -1;
	id = lua_tonumber(state, sl_memberarg(1));
	item = lua_tonumber(state, sl_memberarg(2));
	
	clan = clandb_search(id);
	for (x=0;x<clan->maxslots;x++) {
		if (clan->clanbanks[x].item == item) {
			deposit = x;
			break;
		}
	}

	if (deposit != -1) {
		lua_pushnumber(state,clan->clanbanks[deposit].amount);
		return 1;
	} else {
		lua_pushboolean(state,0);
		return 1;
	}
	return 0;
}

int pcl_speak(lua_State *state, USER *sd) {
	sl_checkargs(state, "sn");
	char *msg = lua_tostring(state, sl_memberarg(1));
	int msglen = strlen(msg);
	int type = lua_tonumber(state, sl_memberarg(2));

	clif_sendscriptsay(sd, msg, msglen, type);
	return 0;
}

int pcl_talkself(lua_State *state, USER *sd) {
	sl_checkargs(state, "ns");
	char *buf;
	int type = lua_tonumber(state, sl_memberarg(1));
	char *msg = lua_tostring(state, sl_memberarg(2));
	unsigned int id = lua_tonumber(state, sl_memberarg(3));
	int msglen = strlen(msg);
	struct block_list *bl = map_id2bl(id);
	USER *tsd = NULL;
	MOB *mob = NULL;
	NPC *npc = NULL;
	
	if (bl != NULL)
	{
		if (bl->type == BL_PC)
			tsd = (USER *)bl;
		else if (bl->type == BL_MOB)
			mob = (MOB *)bl;
		else if (bl->type == BL_NPC)
			npc = (NPC *)bl;
		else
			return 0;
	}
	
	WFIFOHEAD(sd->fd, msglen + 13);
	CALLOC(buf, char, 16 + msglen);
	WBUFB(buf, 0) = 0xAA;
	WBUFW(buf, 1) = SWAP16(10 + msglen);
	WBUFB(buf, 3) = 0x0D;
	WBUFB(buf, 5) = type;
	
	if (id == 0 || bl == NULL)
		WBUFL(buf, 6) = SWAP32(sd->status.id);
	else
		WBUFL(buf, 6) = SWAP32(id);
	
	WBUFB(buf, 10) = msglen + 2;
	
	memcpy(WBUFP(buf, 11), msg, msglen);
	
	clif_send(buf, 16 + msglen, &sd->bl, SELF);
}

int pcl_freeasync(lua_State *state, USER *sd) {
	sl_async_freeco(sd);
	return 0;
}

int pcl_sendhealth(lua_State *state, USER *sd) {
	sl_checkargs(state, "nn");
	int damage;
	float dmg = lua_tonumber(state, sl_memberarg(1));
	int critical = lua_tonumber(state, sl_memberarg(2));
	
	if (dmg > 0) {
		damage = (int)(dmg + 0.5f);
	} else if (dmg < 0) {
		damage = (int)(dmg - 0.5f);
	} else {
		damage = 0;
	}
	
	if(critical == 1) {
		critical = 33;
	} else if (critical == 2) {
		critical = 255;
	}

	clif_send_pc_healthscript(sd, damage, critical);
}

int pcl_sendmail(lua_State *state, USER *sd) {
	sl_checkargs(state, "sss");
	char *to_user = lua_tostring(state, sl_memberarg(1));
	char *topic = lua_tostring(state, sl_memberarg(2));
	char *message = lua_tostring(state, sl_memberarg(3));
	
	nmail_sendmail(sd, to_user, topic, message);
	return 0;
}

int pcl_pickup(lua_State *state, USER *sd) {
	sl_checkargs(state, "n");
	unsigned int id = lua_tonumber(state, sl_memberarg(1));
	
	pc_getitemscript(sd, id);
	return 0;
}

int pcl_equip(lua_State *state, USER *sd) {
	pc_equipscript(sd);
	return 0;
}

int pcl_takeoff(lua_State *state, USER *sd) {
	pc_unequipscript(sd);
	return 0;
}

int pcl_die(lua_State *state, USER *sd) {
	pc_diescript(sd);
	return 0;
}

int pcl_throwitem(lua_State *state, USER *sd) {
	clif_throwitem_script(sd);
	return 0;
}

int pcl_refresh(lua_State *state, USER *sd) {
	pc_setpos(sd, sd->bl.m, sd->bl.x, sd->bl.y);
	clif_refreshnoclick(sd);
	return 0;
}

int pcl_move(lua_State *state, USER *sd) {
	//sl_checkargs(state, "n");
	char speed = lua_tonumber(state, sl_memberarg(1));
	lua_pushnumber(state, clif_noparsewalk(sd, speed));
	return 1;
}

int pcl_respawn(lua_State *state, USER *sd) {
	clif_spawn(sd);
	return 0;
}

int pcl_deductdura(lua_State *state, USER *sd) {
	sl_checkargs(state, "nn");
	int equip = lua_tonumber(state, sl_memberarg(1));
	unsigned int amount = lua_tonumber(state, sl_memberarg(2));
	
	clif_deductdura(sd, equip, amount);
	return 0;
}

int pcl_deductdurainv(lua_State *state, USER *sd) {
	sl_checkargs(state, "nn");
	int slot = lua_tonumber(state, sl_memberarg(1));
	unsigned int amount = lua_tonumber(state, sl_memberarg(2));
	
	sd->status.inventory[slot].dura -= amount;
	return 0;
}

int pcl_setpk(lua_State *state, USER *sd) {
	sl_checkargs(state, "n");
	int id = lua_tonumber(state, sl_memberarg(1));
	int exist = -1;
	int x;
	
	for (x = 0; x < 20; x++) {
		if (sd->pvp[x][0] == id) {
			exist = x;
			break;
		}
	}
	
	if (exist != -1) {
		sd->pvp[exist][1] = time(NULL);
	} else {
		for (x = 0; x < 20; x++) {
			if (!sd->pvp[x][0]) {
				sd->pvp[x][0] = id;
				sd->pvp[x][1] = time(NULL);
				clif_getchararea(sd);
				break;
			} else if (x == 19 && sd->pvp[x][0]) {
				lua_pushboolean(state, 0);
				return 1;
			}
		}
	}
	
	lua_pushboolean(state, 1);
	return 1;
}

int pcl_getpk(lua_State *state, USER *sd) {
	sl_checkargs(state, "n");
	int id = lua_tonumber(state, sl_memberarg(1));
	int x;
	
	for (x = 0; x < 20; x++) {
		if (sd->pvp[x][0] == id) {
			lua_pushboolean(state, 1);
			return 1;
		}
	}

	lua_pushboolean(state, 0);
	return 1;
}

int pcl_broadcast(lua_State *state, USER *sd) {
	sl_checkargs(state, "ns");
	int m = lua_tonumber(state, sl_memberarg(1));
	char* msg = lua_tostring(state, sl_memberarg(2));
	
	clif_broadcastsd(msg, m, sd);
	return 0;
}

int pcl_getcreationitems(lua_State *state, USER *sd) {
	sl_checkargs(state, "n");
	int len = lua_tonumber(state, sl_memberarg(1));
	int curitem;
	
	curitem = RFIFOB(sd->fd,len) - 1;
	
	if (sd->status.inventory[curitem].id) {
		lua_pushnumber(state, sd->status.inventory[curitem].id);
		return 1;
	}
	
	return 0;
}

int pcl_getcreationamounts(lua_State *state, USER *sd) {
	sl_checkargs(state, "nn");
	int len = lua_tonumber(state, sl_memberarg(1));
	int item = lua_tonumber(state, sl_memberarg(2));
	
	if (itemdb_type(item) < 3 || itemdb_type(item) > 17) {
		lua_pushnumber(state, RFIFOB(sd->fd,len));
		return 1;
	} else {
		lua_pushnumber(state, 1);
		return 1;
	}
	
	return 0;
}

int pcl_getparcel(lua_State *state, USER *sd) {
	struct parcel *item;
	unsigned int id, amount, owner_id, sender;
	int pos;
	char real_name[64];
	char npcflag;
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ParItmId`, `ParAmount`, `ParChaIdOwner`, `ParEngrave`, `ParSender`, `ParPosition`, `ParNpc` FROM `Parcels` WHERE `ParChaIdDestination` = '%u'", sd->status.id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_UINT, &amount, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_UINT, &owner_id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_STRING, &real_name, sizeof(real_name), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_UINT, &sender, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_INT, &pos, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_CHAR, &npcflag, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		CALLOC(item, struct parcel, 1);
		item->data.id = id;
		item->data.amount = amount;
		item->data.owner_id = owner_id;
		memcpy(item->data.real_name, real_name, sizeof(real_name));
		if (npcflag == 0) {
			item->sender = sender;
		} else {
			item->sender = sender + NPC_START_NUM - 2;
		}
		item->pos = pos;
		item->npcflag = npcflag;
		item->data.dura = itemdb_dura(id);
	} else {
		clif_sendminitext(sd, "You have no pending parcels.", strlen("You have no pending parcels."));
		lua_pushboolean(state, 0);
		return 1;
	}
	
	SqlStmt_Free(stmt);
	parcell_pushinst(state, item);
	return 1;
}

int pcl_getparcellist(lua_State *state, USER *sd) {
	struct parcel *item;
	unsigned int id, amount, owner_id, sender;
	int i, x, pos;
	char real_name[64];
	char npcflag;
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ParItmId`, `ParAmount`, `ParChaIdOwner`, `ParEngrave`, `ParSender`, `ParPosition`, `ParNpc` FROM `Parcels` WHERE `ParChaIdDestination` = '%u'", sd->status.id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_UINT, &amount, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_UINT, &owner_id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_STRING, &real_name, sizeof(real_name), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_UINT, &sender, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_INT, &pos, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_CHAR, &npcflag, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	lua_newtable(state);
	for (x = 0, i = 1; x < floor(sd->status.maxslots / 4) && SQL_SUCCESS == SqlStmt_NextRow(stmt); x++) {
		CALLOC(item, struct parcel, 1);
		item->data.id = id;
		item->data.amount = amount;
		item->data.owner_id = owner_id;
		memcpy(item->data.real_name, real_name, sizeof(real_name));
		if (npcflag == 0) {
			item->sender = sender;
		} else {
			item->sender = sender + NPC_START_NUM - 2;
		}
		item->pos = pos;
		item->npcflag = npcflag;
		item->data.dura = itemdb_dura(id);
		parcell_pushinst(state, item);
		lua_rawseti(state, -2, i);
		i++;
	}
	
	SqlStmt_Free(stmt);
	//lua_pushboolean(state, 1);
	return 1;
}

int pcl_removeparcel(lua_State *state, USER *sd) {
	sl_checkargs(state, "nnnn");
	int sender = lua_tonumber(state, sl_memberarg(1));
	unsigned int item = lua_tonumber(state, sl_memberarg(2));
	unsigned int amount = lua_tonumber(state, sl_memberarg(3));
	int pos = lua_tonumber(state, sl_memberarg(4));
	int owner = lua_tonumber(state, sl_memberarg(5));
	char* engrave = lua_tostring(state, sl_memberarg(6));
	char npcflag = lua_tonumber(state, sl_memberarg(7));
	sender -= NPC_START_NUM + 1;
	
	if (SQL_ERROR == Sql_Query(sql_handle, "DELETE FROM `Parcels` WHERE `ParChaIdDestination` = '%u' AND `ParPosition` = '%d'", sd->status.id, pos)) {
		SqlStmt_ShowDebug(sql_handle);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `ReceiveParcelLogs` (`RpcChaId`, `RpcMapId`, `RpcX`, `RpcY`, `RpcItmId`, `RpcAmount`, `RpcSender`, `RpcNpc`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u', '%u', '%d')",
	sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, item, amount, sender, npcflag)) {
		SqlStmt_ShowDebug(sql_handle);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	lua_pushboolean(state, 1);
	return 1;
}

int pcl_expireitem(lua_State* state, USER* sd) {
	int x, eqdel;
	unsigned int t = time(NULL);
	char msg[255];
	
	for (x = 0; x < sd->status.maxinv; x++) {
		if (sd->status.inventory[x].timeleft > 0 && sd->status.inventory[x].timeleft < t && itemdb_time(sd->status.inventory[x].id)) {
			sprintf(msg, "Your %s has expired! Please visit the cash shop to purchase another.", itemdb_name(sd->status.inventory[x].id));
			pc_delitem(sd, x, 1, 8);
			clif_sendminitext(sd, msg, 0);
		}
	}
	
	for (x = 0; x < sd->status.maxinv; x++) {
		if (!sd->status.inventory[x].id) {
			eqdel = x;
			break;
		}
	}
	
	for (x = 0; x < 14; x++) {
		if (sd->status.equip[x].timeleft > 0 && sd->status.equip[x].timeleft < t && itemdb_time(sd->status.equip[x].id)) {
			sprintf(msg, "Your %s has expired! Please visit the cash shop to purchase another.", itemdb_name(sd->status.equip[x].id));
			pc_unequip(sd, x);
			pc_delitem(sd, eqdel, 1, 8);
			clif_sendminitext(sd, msg, 0);
		}
	}
	
	return 0;
}

int pcl_logbuysell(lua_State* state, USER* sd) {
	sl_checkargs(state, "nnnn");
	unsigned int item = lua_tonumber(state, sl_memberarg(1));
	unsigned int amount = lua_tonumber(state, sl_memberarg(2));
	unsigned int money = lua_tonumber(state, sl_memberarg(3));
	char buysell = lua_tonumber(state, sl_memberarg(4));
	
	if (!buysell) {
		if (SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `SellLogs` (`SelChaId`, `SelMapId`, `SelX`, `SelY`, `SelItmId`, `SelAmount`, `SelPrice`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u', '%u')",
		sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, item, amount, money)) {
			SqlStmt_ShowDebug(sql_handle);
			lua_pushboolean(state, 0);
			return 1;
		}
	} else {
		if (SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `BuyLogs` (`BuyChaId`, `BuyMapId`, `BuyX`, `BuyY`, `BuyItmId`, `BuyAmount`, `BuyPrice`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u', '%u')",
		sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, item, amount, money)) {
			SqlStmt_ShowDebug(sql_handle);
			lua_pushboolean(state, 0);
			return 1;
		}
	}
	
	lua_pushboolean(state, 1);
	return 1;
}

int pcl_settimevalues(lua_State* state, USER* sd) {
	sl_checkargs(state, "n");
	int i;
	unsigned int newval = lua_tonumber(state, sl_memberarg(1));
	
	for (i = (sizeof(sd->timevalues) / sizeof(sd->timevalues[0])) - 1; i >= 0; i--) {
		if (i > 0) {
			sd->timevalues[i] = sd->timevalues[i - 1];
		} else {
			sd->timevalues[i] = newval;
		}
	}
}

int pcl_gettimevalues(lua_State* state, USER* sd) {
	int i, x;
	
	lua_newtable(state);
	for (x = 0, i = 1; x < (sizeof(sd->timevalues) / sizeof(sd->timevalues[0])); x++) {
		lua_pushnumber(state, sd->timevalues[x]);
		lua_rawseti(state, -2, i);
		i++;
	}
	return 1;
}

int pcl_lookat(lua_State* state, USER* sd) {
	sl_checkargs(state, "n");
	struct block_list* bl = map_id2bl(lua_tonumber(state, sl_memberarg(1)));
	
	clif_parselookat_scriptsub(sd, bl);
	return 0;
}

int pcl_getunknownspells(lua_State* state, USER* sd) {
	int i, j, x;
	unsigned int id;
	unsigned int idlist[255];
	char found = 0;
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	lua_newtable(state);
	
	memset(idlist, 0, sizeof(int) * 255);
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `SplId` FROM `Spells` WHERE `SplPthId` IN ('-1', '0', '%d', '%d') AND `SplActive` = '1'", sd->status.class, classdb_path(sd->status.class))
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		lua_pushboolean(state, 0);
		return 1;
	}
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++) {
		switch (id) {
			case 0:
			case 100:
			case 200:
			case 300:
			case 400:
			case 500:
			case 1000:
			case 1500:
			case 2000:
			case 2500:
			case 3000:
			case 3500:
			case 4000:
			case 4500:
			case 5000:
			case 5500:
			case 7000:
			case 10000:
				continue;
			default:
				for (j = 0; j < MAX_SPELLS; j++) {
					if (sd->status.skill[j] == id) {
						found = 1;
						break;
					}
				}
		
				if (found == 0) {
					for (x = 0; x < 255; x++) {
						if (idlist[x] == 0) {
							idlist[x] = id;
							break;
						}
					}
				} else {
					found = 0;
					continue;
				}
				
				break;
		}
	}
	
	SqlStmt_Free(stmt);
	
	for (i = 0, x = 1; i < 255; i++, x += 2) {
		if (idlist[i] > 0) {
			lua_pushstring(state, magicdb_name(idlist[i]));
			lua_rawseti(state, -2, x);
			lua_pushstring(state, magicdb_yname(idlist[i]));
			lua_rawseti(state, -2, x + 1);
		} else {
			break;
		}
	}
	
	return 1;
}

int pcl_status(lua_State* state, USER* sd) {
	clif_mystaytus(sd);
	return 1;
}

int pcl_testpacket(lua_State* state, USER* sd) {
	sl_checkargs(state, "t");
	int i = 0;
	int index = 0;
	
	lua_pushnil(state);
	
	if (!session[sd->fd])
	{
		session[sd->fd]->eof = 8;
		return 0;
	}
	
	while(lua_next(state, sl_memberarg(1)) != 0) {
		// key is at -2, value at -1
		i = lua_tonumber(state, -2);

		if (i == 2) {
			WFIFOHEAD(sd->fd, lua_tonumber(state, -1) + 6);
		}
		
		if (lua_isnumber(state, -1)) {
			if (lua_tonumber(state, -1) <= 255) {
				WFIFOB(sd->fd, i - 1 + index) = lua_tonumber(state, -1);
			} else if (lua_tonumber(state, -1) > 255 && lua_tonumber(state, -1) <= 65535) {
				WFIFOW(sd->fd, i - 1 + index) = SWAP16((int)lua_tonumber(state, -1));
				index += 1;
			} else {
				WFIFOL(sd->fd, i - 1 + index) = SWAP32((int)lua_tonumber(state, -1));
				index += 3;
			}
		} else {
			strcpy(WFIFOP(sd->fd, i - 1 + index), lua_tostring(state, -1));
			index += strlen(lua_tostring(state, -1)) - 1;
		}
		
		lua_pop(state, 1);
	}
	
	WFIFOSET(sd->fd, encrypt(sd->fd));
	return 1;
}

int pcl_getcasterid(lua_State* state, USER* sd) {
	sl_checkargs(state, "s");
	int spell = magicdb_id(lua_tostring(state,sl_memberarg(1)));
	int i, x;
	lua_newtable(state);
	
	for (i = 0, x = 1; i < MAX_MAGIC_TIMERS; i++) {
		if (sd->status.dura_aether[i].id == spell) {
			lua_pushnumber(state, sd->status.dura_aether[i].caster_id);
			lua_rawseti(state, -2, x);
			x++;
		}
	}
	
	return 1;
}

int pcl_changeview(lua_State* state, USER* sd) {
	sl_checkargs(state, "nn");
	int x = lua_tonumber(state, sl_memberarg(1));
	int y = lua_tonumber(state, sl_memberarg(2));
	
	clif_sendxychange(sd, x, y);
	clif_mob_look_start(sd);
	map_foreachinarea(clif_object_look_sub,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_ALL,LOOK_GET,sd);
	clif_mob_look_close(sd);
	clif_destroyold(sd);
	map_foreachinarea(clif_charlook_sub,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,LOOK_SEND,sd);
	map_foreachinarea(clif_charlook_sub,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,LOOK_GET,sd);
	map_foreachinarea(clif_cnpclook_sub,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_NPC,LOOK_GET,sd);
	map_foreachinarea(clif_cmoblook_sub,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_MOB,LOOK_GET,sd);
	return 1;
}

int pcl_settimer(lua_State* state, USER* sd) {
	sl_checkargs(state, "nn");
	char type = lua_tonumber(state, sl_memberarg(1));
	unsigned int length = lua_tonumber(state, sl_memberarg(2));
	
	if (type == 1 || type == 2) {
		if ((long)length > 4294967295) {
			sd->disptimertick = 4294967295;
		} else if ((long)length < 0) {
			lua_pushboolean(state, 0);
			return 0;
		} else {
			sd->disptimertick = length;
		}
	} else if ((type != 1 && type != 2) || sd->disptimer > 0){
		lua_pushboolean(state, 0);
		return 0;
	}
	
	sd->disptimertype = type;
	sd->disptimer = timer_insert(1000, 1000, pc_disptimertick, sd->bl.id, 0);
	clif_send_timer(sd, type, length);
	lua_pushboolean(state, 1);
	return 0;
}

int pcl_addtime(lua_State* state, USER* sd) {
	sl_checkargs(state, "n");
	unsigned int addition = lua_tonumber(state, sl_memberarg(1));
	
	if (sd->disptimertype == 2) {
		if ((long)(sd->disptimertick + addition) > 4294967295) {
			sd->disptimertick = 4294967295;
		} else {
			sd->disptimertick += addition;
		}
		
		clif_send_timer(sd, sd->disptimertype, sd->disptimertick);
	} else {
		lua_pushboolean(state, 0);
		return 0;
	}
	
	lua_pushboolean(state, 1);
	return 0;
}

int pcl_removetime(lua_State* state, USER* sd) {
	sl_checkargs(state, "n");
	unsigned int addition = lua_tonumber(state, sl_memberarg(1));
	
	if (sd->disptimertype == 2) {
		if ((long)(sd->disptimertick - addition) < 0) {
			sd->disptimertick = 0;
		} else {
			sd->disptimertick -= addition;
		}
		
		clif_send_timer(sd, sd->disptimertype, sd->disptimertick);
	} else {
		lua_pushboolean(state, 0);
		return 0;
	}
	
	lua_pushboolean(state, 1);
	return 0;
}

int mapregl_getattr(lua_State*,USER*,char*);
int mapregl_setattr(lua_State*,USER*,char*);


void mapregl_staticinit() {
	mapregl_type=typel_new("Mapregistry",0);
	mapregl_type.getattr=mapregl_getattr;
	mapregl_type.setattr=mapregl_setattr;
}

int mapregl_setattr(lua_State* state,USER* sd,char* attrname) {
	map_setglobalreg(sd->bl.m,attrname,lua_tonumber(state,-1));
	return 0;

}

int mapregl_getattr(lua_State* state,USER* sd,char* attrname) {
	lua_pushnumber(state,map_readglobalreg(sd->bl.m,attrname));
	return 1;
}

//game registries
int gameregl_setattr(lua_State* state,USER* sd, char* attrname) {
	map_setglobalgamereg(attrname,lua_tonumber(state,-1));
	return 0;
}

int gameregl_getattr(lua_State* state,USER *sd, char* attrname) {
	lua_pushnumber(state,map_readglobalgamereg(attrname));
	return 1;
}

void gameregl_staticinit() {
	gameregl_type=typel_new("Gameregistry",0);
	gameregl_type.getattr=gameregl_getattr;
	gameregl_type.setattr=gameregl_setattr;
}

//account registries
int acctregl_setattr(lua_State* state, USER* sd, char* attrname) {
	pc_setacctreg(sd, attrname, lua_tonumber(state, -1));
	return 0;
}

int acctregl_getattr(lua_State* state, USER* sd, char* attrname) {
	lua_pushnumber(state, pc_readacctreg(sd, attrname));
	return 1;
}

void acctregl_staticinit() {
	acctregl_type = typel_new("Accountregistry", 0);
	acctregl_type.getattr = acctregl_getattr;
	acctregl_type.setattr = acctregl_setattr;
}