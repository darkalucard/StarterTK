
#ifndef _MOB_H_
#define _MOB_H_

#define MOBDB_FILE "db/mob_db.txt"
//#define MOBSPAWN_FILE "db/spawn_db.txt"
enum {MOB_ALIVE,MOB_DEAD,MOB_PARA,MOB_BLIND,MOB_HIT};
enum {MOB_NORMAL,MOB_AGGRESSIVE, MOB_STATIONARY};





extern unsigned int MOB_SPAWN_START;
extern unsigned int MOB_SPAWN_MAX;
extern unsigned int MOB_ONETIME_START;
extern unsigned int MOB_ONETIME_MAX;
extern unsigned int MIN_TIMER;
int mobdb_read(const char *);
int mobspawn_read(const char *);
struct mobdb_data* mobdb_search(unsigned int);
int mobdb_init();
struct mobdb_data* mobdb_searchexist(unsigned int);
struct mobdb_data* mobdb_searchname(const char*);
int mobdb_id(char*);
int mobdb_searchname_sub(void*,void*,va_list);
int mob_handle(int,int);
int mob_handle_sub(MOB*,va_list);
int mob_handle_magic(struct block_list*,va_list);
int move_mob(MOB*);
unsigned int* mobspawn_onetime(unsigned int,int,int,int,int,int,int,unsigned int);
int mobdb_itemrate(unsigned int,int);
int mobdb_drops(MOB*,USER*);
int mobdb_itemid(unsigned int,int);
int mobdb_itemamount(unsigned int,int);
int mob_calcstat(MOB*);
int mob_respawn(MOB*);
int mob_find_target(struct block_list*,va_list);
int move_mob_intent(MOB*,struct block_list* bl);
int mob_move2(MOB*,int,int,int);
int mob_attack(MOB*,int);
int mob_move(struct block_list*,va_list);
int mobdb_dropitem(unsigned int,unsigned int,int,int,int,int,USER*);
//int move_mob_intent(MOB*,USER*);
int mob_timer_new(int,int);
int mob_timer_spawns(int,int);
int move_mob(MOB*);
int moveghost_mob(MOB*);
int mob_flushmagic(MOB*);
int mob_duratimer(MOB*);
int mob_secondduratimer(MOB*);
int mob_warp(MOB*,int,int,int);
int mob_setglobalreg(MOB*,char*,int);
int mob_readglobalreg(MOB*,char*);

void onetime_addiddb(struct block_list*);
void onetime_deliddb(unsigned int);
//int free_onetime(unsigned int);
struct block_list* onetime_avail(unsigned int);
int free_session_add(int);
#endif
