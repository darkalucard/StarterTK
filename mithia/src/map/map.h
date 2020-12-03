
#ifndef _MAP_H_
#define _MAP_H_

#include <stdarg.h>
#include "script.h"
#include "mmo.h"
#include <lua.h>

#define AUTH_FIFO_SIZE (4*1024)
#define BLOCK_SIZE 8
#define AREAX_SIZE 18
#define AREAY_SIZE 16

#define F1_NPC 4294967295
#define NPCT_START_NUM 3321225472
#define NPC_START_NUM 3221225472
#define MAX_NPC 100000000
#define FLOORITEM_START_NUM 2047483647
#define MAX_FLOORITEM 100000000
#define MOBOT_START_NUM 1173741823
#define MOB_START_NUM 1073741823
#define MAX_MOB 100000000
#define MAX_GROUPS 256
#define MAX_GROUP_MEMBERS 256

#define USER struct map_sessiondata
#define MOB struct mobspawn_data
#define FLOORITEM struct flooritem_data
#define NPC struct npc_data

enum { //Options flags(to work on later)
	optFlag_stealth = 32,
	optFlag_noclick = 64,
	optFlag_walkthrough = 128,
	optFlag_ghosts = 256
};


enum {
	WSUN = 0,
	WRAIN = 1,
	WSNOW = 2,
	WBIRD = 3
};

enum { ///User flags
	uFlag_none = 0,
	uFlag_silenced = 1,
	uFlag_canpk = 2,
	uFlag_canbepk = 3,
	uFlag_immortal = 8,
	uFlag_unphysical = 16,
	uFlag_eventhost = 32,
	uFlag_constable = 64,
	uFlag_archon = 128,
	uFlag_gm = 256
};


#define isactive(p) if(!isPlayerActive(p)) return 0



enum { BL_NUL   = 0x00,
	BL_PC    = 0x01,
	BL_MOB   = 0x02,
	BL_NPC   = 0x04,
	BL_ITEM   = 0x08,
	BL_ALL = 0x0E //MOB->NPC->ITEM
	};
enum { SCRIPT, FLOOR};
enum { PC_ALIVE, PC_DIE, PC_HORSE, PC_INVIS, PC_DISGUISE };
enum { SP_HP, SP_MP, SP_MHP, SP_MMP, SP_ZENY };
/// USED FOR  SENDSTATUS(packet 0x08)
enum {  // Status Flags
	SFLAG_UNKNOWN1 = 0x01,
	SFLAG_UNKNOWN2 = 0x02,
	SFLAG_UNKNOWN3 = 0x04,
	SFLAG_ALWAYSON = 0x08,
	SFLAG_XPMONEY = 0x10,
	SFLAG_HPMP = 0x20,
	SFLAG_FULLSTATS = 0x40,
	SFLAG_GMON = 0x80
};
enum {	
	MAP_WHISPFAIL,
	MAP_ERRGHOST,
	MAP_ERRITMLEVEL,
	MAP_ERRITMMIGHT,
	MAP_ERRITMGRACE,
	MAP_ERRITMWILL,
	MAP_ERRITMSEX,
	MAP_ERRITMFULL,
	MAP_ERRITMUNIQUE,
	MAP_ERRITMPATH,
	MAP_EQHELM,
	MAP_EQWEAP,
	MAP_EQARMOR,
	MAP_EQSHIELD,
	MAP_EQLEFT,
	MAP_EQRIGHT,
	MAP_EQSUBLEFT,
	MAP_EQSUBRIGHT,
	MAP_EQFACEACC,
	MAP_EQCROWN,
	MAP_EQMANTLE,
	MAP_EQNECKLACE,
	MAP_EQBOOTS,
	MAP_EQCOAT,
	MAP_ERRVITA,
	MAP_ERRMANA,
	MSG_MAX
};

///Packet Descriptors(for incoming data)
enum { 
	IN_NULL = 0,
};

///Packet Descripters(for outgoing data)
enum {
	OUT_NULL = 0,
	OUT_STATUS = 0x08,
};

extern struct map_msg_data {
	char message[256];
	int len;
} map_msg[MSG_MAX];

struct n_post {
    struct n_post *prev;
    unsigned int pos;
};

struct block_list {
	struct block_list *next, *prev;
	unsigned int id, bx, by, graphic_id, graphic_color;
	unsigned short m, x, y;
	unsigned char type, subtype;
};

struct gfxViewer
{
	unsigned short weapon, armor, helm, faceAcc, crown, shield, necklace, mantle, boots, faceAccT;
	unsigned char cweapon, carmor, chelm, cfaceAcc, ccrown, cshield, cnecklace, cmantle, cboots, cfaceAccT;
	unsigned char hair, chair, face, cface, cskin, dye;
	char toggle;
	char name[16];
};

struct mobdb_data {
    struct item equip[MAX_EQUIP];
	int vita, exp, type, subtype, look, look_color, hit, level, might, grace, will, movetime, atktime, spawntime, baseac;
	int sound, mana, physdeduct;
	unsigned int id, mindam, maxdam, drop_count[8], drop[8], drop_rate[8];
	char name[45], yname[45];
	char dodge, block, parry, retdist, bosstype, mobtype, state, race, undead, pierce, seeinvis, tier, mark, isnpc;
	short protection, resist, miss;
	unsigned short sex, face, face_color, hair, hair_color, armor_color, skin_color, startm, startx, starty;
};

struct threat_table {
	unsigned int user, amount;
};

struct mobspawn_data {
    struct block_list bl;
    struct skill_info da[MAX_MAGIC_TIMERS];
    struct mobdb_data *data;
	struct threat_table threat[MAX_THREATCOUNT];
	struct global_reg registry[MAX_GLOBALREG];
	struct gfxViewer gfx;
    unsigned short startm, startx, starty, bx, by, look;
 	short resist, miss, protection;
	unsigned int id, mobid, current_vita, current_mana, target, attacker, owner, confused_target, timer, last_death, rangeTarget, ranged, newmove, newatk;
	unsigned int lastvita, maxvita, maxmana, replace, mindam, maxdam;
    int ac, side, time, spawncheck, num, crit, critchance, critmult, snare, lastaction, hit, might, grace, will, physdeduct;
    unsigned char state, canmove, onetime, paralyzed, blind, confused, summon, returning, look_color;
	char pierce, clone, start, end, dodge, block, parry, charstate;
	float sleep, deduction, damage, dmgshield, invis;
	double dmgdealt, dmgtaken, maxdmg, dmgindtable[MAX_THREATCOUNT][2], dmggrptable[MAX_THREATCOUNT][2];
};

struct script_reg {
	int index, data;
};

struct script_regstr {
	int index;
	char data[256];
};


struct sd_ignorelist
{
  char name[100];
  struct sd_ignorelist *Next;
};

struct map_sessiondata {
	struct block_list bl;
	int fd;
	//mmo
	struct mmo_charstatus status;
	//status
	unsigned long ambushtimer;
	unsigned int max_hp, max_mp, attacker, rangeTarget, equipid, breakid, pvp[20][2], killspvp, timevalues[5], lastvita, groupid, disptimer, disptimertick;
	int might, will, grace, ac, armor, min_dam, max_dam, hit, dam, regen, vregen, mregen, speed, regentimer, pongtimer, disguise, disguise_color, backstab;
	int flank, polearm, tooclose, canmove, iswalking, paralyzed, blind, drunk, snare, silence, critchance, afk, afktime, totalafktime, afktimer, extendhit;
	int crit, duratimer, physdeduct, scripttimer, scripttick, secondduratimer, wisdom, bindx, bindy;
	short protection, resist, miss, attack_speed, con;
	float rage, enchanted, sleep, deduction, damage, invis, fury, critmult, dmgshield, vregenoverflow, mregenoverflow;
	double dmgdealt, dmgtaken;
	char afkmessage[80], mail[4000];
	char parry, block, dodge, takeoffid, attacked, pierce, boardshow, clone, action, luaexec, deathflag, selfbar, groupbars, mobbars, disptimertype, sendstatus_tick;
	char dialogtype;
	unsigned char confused, talktype, pickuptype, invslot, spottraps;
	unsigned short throwx, throwy, viewx, viewy, bindmap;
	char EncHash[0x401];
	//npc
	struct script_data *npc_stackbuf;
	int npc_id, npc_pos, npc_lastpos, npc_menu, npc_amount, npc_g, npc_gc, target, time, time2, lasttime, timer, npc_stack, npc_stackmax;
	char *npc_script,*npc_scriptroot;
	//registry
	struct script_reg *reg;
	struct script_regstr *regstr;
	struct n_post npcp;
	int reg_num, regstr_num;
	//group
	int bcount, group_count, group_on;
	unsigned int group_leader;//, group[MAX_GROUP];
	//exchange
	int exchange_on;
	struct {
		struct item item[52];
		int item_count, exchange_done, list_count;
		unsigned int gold, target;
	} exchange;
	struct {
		int menu_or_input;
	} state;
	// lua related fields
	unsigned int coref;
	unsigned int coref_container;
	//creation system stuff
	int creation_works, creation_item, creation_itemamount;
	//boards
	int board_candel, board_canwrite, board, board_popup, co_timer;
	char question[64], speech[255], profilepic_data[65535], profile_data[255];
	unsigned short profilepic_size;
	unsigned char profile_size;
	//mob
	int mob_len, mob_count, mob_item;
	int msPing, pbColor;   //Ping in miliseconds
   	unsigned int time_check, time_hash, last_click;
	int chat_timer, savetimer;
	struct gfxViewer gfx;
    struct sd_ignorelist  *IgnoreList;
	unsigned long optFlags, uFlags, LastPongStamp, LastPingTick, flags;
    unsigned char PrevSeed, NextSeed, LastWalk, loaded;
};

struct npc_data {
	struct block_list bl;
	struct item equip[MAX_EQUIP];
	struct global_reg registry[MAX_GLOBALREG];
	struct gfxViewer gfx;
	unsigned int id, actiontime, owner, duration, lastaction, time, duratime;
	char name[64], npc_name[64];
	char state, side, canmove, npctype, clone;
	unsigned short sex, face, face_color, hair, hair_color, armor_color, skin_color, startm, startx, starty;
};

struct auth_data {
	unsigned int id, ip;
	char name[16];
};

struct warp_list {
	int x, y, tm, tx, ty;
	struct warp_list* next;
	struct warp_list* prev;
};

struct map_data {
	char title[64];
	struct block_list **block;
	struct block_list **block_mob;
	struct warp_list **warp;
	struct global_reg *registry;
	int max_sweep_count, user, registry_num, id;
	unsigned short xs, ys, bxs, bys, port, bgm;
	unsigned char *map;
	unsigned short *tile, *obj, *pass;
	unsigned int ip, sweeptime;
	unsigned char pvp, spell, light, weather, cantalk, show_ghosts, region, indoor, warpout, bind;
};
struct town_data {
	char name[32];
};

struct userlist_data {
	unsigned int user_count, user[10000];
};

struct game_data {
	struct global_reg *registry;
	int registry_num;
};

#define map_isloaded(m) (map[m].registry)
#define read_map(m,x,y) (map[m].map[(x)+(y)*map[m].xs])
#define read_tile(m,x,y) (map[m].tile[(x)+(y)*map[m].xs])
#define read_obj(m,x,y) (map[m].obj[(x)+(y)*map[m].xs])
#define read_pass(m,x,y) (map[m].pass[(x)+(y)*map[m].xs])
#define OBJ_UP 1
#define OBJ_DOWN 2
#define OBJ_RIGHT 4
#define OBJ_LEFT 8

unsigned char* objectFlags;

struct flooritem_data {
	struct block_list bl;
	struct item data;
	unsigned int lastamount, timer, clepto_timer, canloot[MAX_GROUP_MEMBERS];
};

struct parcel {
	unsigned int sender;
	int pos, npcflag;
	struct item data;
};

extern struct town_data towns[];
extern int town_n;
extern struct map_data *map;
//extern struct map_data map[];
extern int map_n;
struct userlist_data userlist;
extern struct auth_data auth_fifo[];
extern int auth_n;

extern int char_fd;
extern char char_id[];
extern char char_pw[];
extern int save_fd;
extern char save_id[];
extern char save_pw[];
extern char sql_id[];
extern char sql_pw[];
extern char sql_ip[];
extern char sql_db[];
extern int sql_port;
extern struct Sql* sql_handle;
extern unsigned int map_ip;
extern unsigned int map_port;
extern unsigned int log_ip;
extern unsigned int log_port;
extern int save_time;
extern int dump_save;

struct map_sessiondata* map_id2sd(unsigned int);
struct map_sessiondata* map_name2sd(char*);
struct mobspawn_data* map_id2mob(unsigned int);
struct flooritem_data* map_id2fl(unsigned int);
struct npc_data* map_id2npc(unsigned int);
struct npc_chardata* map_id2cnpc(unsigned int);

void map_clritem();
void map_delitem(unsigned int);
void map_additem(struct block_list*);
//void Log_Add(const char*, const char*, ...);
void map_deliddb(struct block_list*);
void map_addiddb(struct block_list*);
int isPlayerActive(USER* sd);
int map_setmapip(int,unsigned int,unsigned short);
int map_freeblock(void*);
int map_freeblock_lock();
int map_freeblock_unlock();
int map_addblock(struct block_list*);
int map_delblock(struct block_list*);
int map_foreachincell(int (*)(struct block_list*,va_list),int,int,int,int,...);
int map_foreachincellwithtraps(int (*)(struct block_list*,va_list),int,int,int,int,...);
int map_foreachinarea(int (*)(struct block_list*,va_list),int,int,int,int,int,...);
int map_foreachinblock(int (*)(struct block_list*,va_list),int,int,int,int,int,int,...);
int map_foreachinblockva(int (*)(struct block_list*,va_list),int,int,int,int,int,int,va_list);
int map_foreachinarea_alive(int (*)(struct block_list*,va_list),int,int,int,int,int,...);
int map_foreachinarea_gm(int (*)(struct block_list*,va_list),int,int,int,int,int,...);
int map_respawn(int (*)(struct block_list*, va_list),int, int, ...);
int map_respawnmobs(int (*)(struct block_list*, va_list), int, int, va_list);

int map_foreachinblockva_alive(int (*)(struct block_list*,va_list),int,int,int,int,int,int,va_list);
int map_foreachinblockva_gm(int (*)(struct block_list*,va_list),int,int,int,int,int,int,va_list);

int map_canmove(int,int,int);
int map_addspawn(struct mobspawn_data*);
int map_removespawn(struct mobspawn_data*);
struct block_list* map_id2bl(unsigned int id);
int map_sweepadd(struct flooritem_data*);
int map_sweepdel(struct flooritem_data*);
int map_moveblock(struct block_list*,int,int);
struct block_list* map_firstincell(int,int,int,int);
struct block_list* map_firstincellwithtraps(int,int,int,int);
void map_foreachmob(int (*)(struct block_list*,va_list),...);
int map_read();//int,const char*,char,int,int,unsigned short,unsigned short,unsigned int,unsigned char,unsigned char,unsigned char,unsigned char,unsigned char,const char*);
int map_reload();
int map_src_add(const char*);
int map_town_add(const char*);
int map_src_clear();
int map_adduserlist(struct map_sessiondata*);
int map_deluserlist(struct map_sessiondata*);
int lang_read(const char*);
int config_read(const char*);
int map_changepostcolor(int,int,int);
int map_getpostcolor(int,int);
char* map_id2name(unsigned int);
void help_screen();
int mmo_char_fromdb(unsigned int, struct mmo_charstatus*,char*);
void do_term(void);
void mmo_setonline(unsigned int,int);
int boards_showposts(struct map_sessiondata*,int);
int boards_readpost(struct map_sessiondata*,int,int);
int boards_post(struct map_sessiondata*,int);
char isActive(struct map_sessiondata*);
int nmail_show(struct map_sessiondata*);
int nmail_read(struct map_sessiondata*,int);
int nmail_sendmail(struct map_sessiondata*,char*,char*,char*);
int map_addmob(struct map_sessiondata*,unsigned int,int,int,unsigned int);
int mmo_char_todb(struct mmo_charstatus*);
extern int xp_rate;
extern int d_rate;
extern int map_fd;
int map_reset_timer(int,int);
int old_time,cur_time,cur_year,cur_day,cur_season;
int hasCoref(USER*);
int map_setglobalreg(int,char*,int);
int map_readglobalreg(int,char*);
//int map_registrysave(struct global_reg*,int,const char*,int);
int map_registrysave(int,int);
int map_registrydelete(int,int);
int map_loadgameregistry();
int map_savegameregistry(int);
int map_setglobalgamereg(char*,int);
int map_readglobalgamereg(char*);
int map_readclanbanks(int);
int map_saveclanbanks(int);
int map_weather(int,int);
int map_cronjob(int,int);
int map_savechars(int,int);
#endif
