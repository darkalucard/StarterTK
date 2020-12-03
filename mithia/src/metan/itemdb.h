
#ifndef _ITEM_H_
#define _ITEM_H_

#define ITEMDB_FILE "db/item_db.txt"

enum {
	EQ_WEAP,
	EQ_ARMOR,
	EQ_SHIELD,
	EQ_HELM,
	EQ_LEFT,
	EQ_RIGHT,
	EQ_SUBLEFT,
	EQ_SUBRIGHT,
	EQ_FACEACC,
	EQ_CROWN,
	EQ_MANTLE,
	EQ_NECKLACE,
	EQ_BOOTS,
	EQ_COAT
};

enum {
	ITM_EAT, //0
	ITM_USE, //1
	ITM_SMOKE, //2
	ITM_WEAP, //3
	ITM_ARMOR, //4
	ITM_SHIELD, //5
	ITM_HELM, //6
	ITM_LEFT, //7
	ITM_RIGHT, //8
	ITM_SUBLEFT, //9
	ITM_SUBRIGHT, //10
	ITM_FACEACC, //11
	ITM_CROWN, //12
	ITM_MANTLE, //13
	ITM_NECKLACE, //14
	ITM_BOOTS, //15
	ITM_COAT, //16
	ITM_HAND, //17
	ITM_ETC, //18
	ITM_USESPC, //19
	ITM_TRAPS //20
};

struct item_data {
	unsigned int id, sound, min_dam, max_dam, sound_hit, req_vita, req_mana, time;
	char name[64], yname[64], text[64], buytext[64], real_name[64];
	unsigned char type, class, sex, level, icon_color, ethereal;
	unsigned short icon;
	int price, sell, rank, max_amount, look, look_color, dura, might, will, grace, ac, dam, hit, vita, mana, protection, healing, event, wisdom, con, attack_speed;
	int might_req, grace_req, will_req, depositable, exchangeable, droppable, thrown, unique, dodge, parry, block, resist, vregen, mregen, physdeduct;
	char *script, *equip_script, *unequip_script;
};

extern struct DBMap *item_db;
//extern struct DBMap *custom_db;

struct item_data* itemdb_search(unsigned int);
struct item_data* itemdb_searchexist(unsigned int);
struct item_data* itemdb_searchname(const char*);
int itemdb_thrown(unsigned int);
unsigned int itemdb_id(char *str);
char* itemdb_name(unsigned int);
char* itemdb_yname(unsigned int);
int itemdb_attackspeed(unsigned int);
int itemdb_wisdom(unsigned int);
int itemdb_con(unsigned int);
int itemdb_event(unsigned int);
int itemdb_maxamount(unsigned int);
int itemdb_sound(unsigned int);
int itemdb_dura(unsigned int);
int itemdb_look(unsigned int);
int itemdb_lookcolor(unsigned int);
int itemdb_icon(unsigned int);
int itemdb_iconcolor(unsigned int);
int itemdb_type(unsigned int);
int itemdb_level(unsigned int);
int itemdb_might(unsigned int);
int itemdb_mightreq(unsigned int);
int itemdb_grace(unsigned int);
int itemdb_gracereq(unsigned int);
int itemdb_will(unsigned int);
int itemdb_willreq(unsigned int);
int itemdb_sex(unsigned int);
int itemdb_vita(unsigned int);
int itemdb_mana(unsigned int);
int itemdb_ac(unsigned int);
int itemdb_dam(unsigned int);
int itemdb_hit(unsigned int);
int itemdb_mindam(unsigned int);
int itemdb_maxdam(unsigned int);
int itemdb_rank(unsigned int);
int itemdb_exchangeable(unsigned int);
int itemdb_droppable(unsigned int);
int itemdb_depositable(unsigned int);
unsigned int itemdb_soundhit(unsigned int);
char* itemdb_script(unsigned int);
char* itemdb_unequipscript(unsigned int);
char* itemdb_equipscript(unsigned int);
char* itemdb_text(unsigned int);
char* itemdb_buytext(unsigned int);
int itemdb_read();
int itemdb_term();
int itemdb_init();
unsigned int itemdb_reqvita(unsigned int);
unsigned int itemdb_reqmana(unsigned int);
int itemdb_unique(unsigned int);
int itemdb_healing(unsigned int);
int itemdb_ethereal(unsigned int);
int itemdb_dodge(unsigned int);
int itemdb_block(unsigned int);
int itemdb_parry(unsigned int);
int itemdb_price(unsigned int);
int itemdb_class(unsigned int);
int itemdb_protection(unsigned int);
int itemdb_resist(unsigned int);
int itemdb_vregen(unsigned int);
int itemdb_mregen(unsigned int);
int itemdb_physdeduct(unsigned int);
int itemdb_time(unsigned int);

/*struct item_data* itemdb_searchc(int);
struct item_data* itemdb_searchexistc(int);
struct item_data* itemdb_searchnamec(const char*);
int itemdb_thrownc(int);
int itemdb_idc(char *str);
char* itemdb_namec(int);
char* itemdb_ynamec(int);
int itemdb_attackspeedc(int);
int itemdb_wisdomc(int);
int itemdb_conc(int);
int itemdb_eventc(int);
int itemdb_maxamountc(int);
int itemdb_soundc(int);
int itemdb_durac(int);
int itemdb_lookc(int);
int itemdb_lookcolorc(int);
int itemdb_iconc(int);
int itemdb_iconcolorc(int);
int itemdb_typec(int);
int itemdb_levelc(int);
int itemdb_mightc(int);
int itemdb_gracec(int);
int itemdb_willc(int);
int itemdb_sexc(int);
int itemdb_vitac(int);
int itemdb_manac(int);
int itemdb_acc(int);
int itemdb_damc(int);
int itemdb_hitc(int);
int itemdb_mindamc(int);
int itemdb_maxdamc(int);
int itemdb_rankc(int);
int itemdb_exchangeablec(int);
int itemdb_droppablec(int);
int itemdb_depositablec(int);
unsigned int itemdb_soundhitc(int);
char* itemdb_scriptc(int);
char* itemdb_unequipscriptc(int);
char* itemdb_equipscriptc(int);
char* itemdb_textc(int);
unsigned int itemdb_reqvitac(int);
unsigned int itemdb_reqmanac(int);
int itemdb_uniquec(int);
int itemdb_healingc(int);
int itemdb_etherealc(int);
int itemdb_readc();*/
#endif
