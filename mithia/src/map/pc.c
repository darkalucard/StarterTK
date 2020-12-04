
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <math.h>
#include <lua.h>
#include <lauxlib.h>

#include "map.h"
#include "../common/mmo.h"
#include "../common/malloc.h"
#include "../common/rndm.h"
#include "script.h"
#include "clif.h"
#include "itemdb.h"
#include "pc.h"
#include "timer.h"
#include "sl.h"
#include "db_mysql.h"
#include "socket.h"
#include "mob.h"


struct timeval start;

//New function used for evaluating how many miliseconds the server program
//has been running for.


int pc_addtocurrent2(struct block_list *bl, va_list ap) {
	int *def = NULL;
	int id=0;
	USER *sd = NULL;
	FLOORITEM *fl = NULL;
	FLOORITEM *fl2 = NULL;
	int type=0;

	nullpo_ret(0,fl=(FLOORITEM*)bl);

	def=va_arg(ap,int*);
	id=va_arg(ap,int);
	nullpo_ret(0,fl2=va_arg(ap,FLOORITEM*));

	if(def[0]) return 0;

	if(fl->data.id==fl2->data.id && fl->data.owner_id == fl2->data.owner_id && !(strcmpi(fl->data.real_name, fl2->data.real_name))) {

		fl->data.amount+=fl2->data.amount;
		if(fl->timer) timer_remove(fl->timer);
		if(map[fl->bl.m].sweeptime) fl->timer=timer_insert(map[fl->bl.m].sweeptime,map[fl->bl.m].sweeptime,pc_item_timer,fl->bl.id,0);

		//fl->gone_tick=0;
		def[0]=1;
		return 0;
	}
	return 0;

}
int pc_dropitemfull(USER *sd, struct item* fl2) {
	FLOORITEM *fl;

	//if(sd->status.inventory[id].amount<=0) { clif_senddelitem(sd,id,1); return 0; }

	int def[2];

	CALLOC(fl, FLOORITEM, 1);

	fl->bl.m = sd->bl.m;
	fl->bl.x = sd->bl.x;
	fl->bl.y = sd->bl.y;
	//printf("%d\n",type);
	memcpy(&fl->data, fl2, sizeof(struct item));

	def[0]=0;

	//item check goes here(to see if there are previous items added
	//fl->data.dura=sd->status.inventory[id].dura;
	//if(!fl->data.custom) {
		if(fl->data.dura==itemdb_dura(fl->data.id)) {
			map_foreachincell(pc_addtocurrent2,fl->bl.m,fl->bl.x,fl->bl.y,BL_ITEM,def,fl->data.id,fl);
		}
	//} else {
	//	if(fl->data.dura==itemdb_durac(fl->data.custom)) {
	//		map_foreachincell(pc_addtocurrent2,fl->bl.m,fl->bl.x,fl->bl.y,BL_ITEM,def,fl->data.id,fl);
	//	}
	//}

	//sd->status.inventory[id].amount--;



	//map_foreachincell(pc_npc_drop,sd->bl.m,sd->bl.x,sd->bl.y,BL_NPC,fl,sd);

	if(!def[0]) {
		//map_sweepadd(fl);
		//fl->gone_tick=0;

		map_additem(&fl->bl);
		if(fl->timer) timer_remove(fl->timer);
		if(map[fl->bl.m].sweeptime) fl->timer=timer_insert(map[fl->bl.m].sweeptime,map[fl->bl.m].sweeptime,pc_item_timer,fl->bl.id,0);

		map_foreachinarea(clif_object_look_sub2,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,LOOK_SEND,&fl->bl);
	} else {
		FREE(fl);
	}
	return 0;
}


int pc_item_timer(int id, int none) {
	FLOORITEM* fl=NULL;

	nullpo_ret(1,fl=(FLOORITEM*)map_id2bl((unsigned int)id));

	clif_lookgone(&fl->bl);
	map_delitem(fl->bl.id);
	return 1;
}

int pc_isequip(USER *sd,int type){
	//if(!sd->status.equip[type].custom) {
		return sd->status.equip[type].id;
	//} else {
	//	return sd->status.equip[type].custom;
	//}
}
int pc_loadmagic(USER *sd) {
	int i;

	for(i=0;i<MAX_SPELLS;i++) {
		if(sd->status.skill[i]>0) {
			clif_sendmagic(sd,i);
		}
	}
	return 0;
}

int pc_savetimer(int id, int none) {
	USER* sd=map_id2sd((unsigned int)id);
	if(sd) {
		intif_save(sd);
	}
	return 0;
}
//afk timer!
int pc_afktimer(int id, int none) {
	USER* sd=map_id2sd((unsigned int)id);
	
	nullpo_ret(0, sd);
	sd->afktime += 1;
	
	if(sd->afk == 1 && sd->status.state == 0) {
		sd->totalafktime += 10;
		clif_sendaction(&sd->bl, 0x10, 0x4E, 0);
		return 0;
	}
	
	if(sd->afk == 1 && sd->status.state == 3) {
		sd->totalafktime += 10;
		map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,324,&sd->bl,0);
		return 0;
	}
	
	if(sd->afk == 1 && sd->status.state == PC_DIE && (sd->bl.m == 4 || sd->bl.m == 73)) {
		sd->totalafktime += 10;
		return 0;
	}
	
	if(sd->afktime >= 30) {
		if(sd->status.state == 0) {
			sd->totalafktime += 300;
			clif_sendaction(&sd->bl, 0x10, 0x4E, 0);
		} else if(sd->status.state == 3) {
			sd->totalafktime += 300;
			map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,324,&sd->bl,0);
		}
		
		sd->afk = 1;
		
		if (SQL_ERROR == Sql_Query(sql_handle, "INSERT INTO `AfkLogs` (`AfkChaId`, `AfkMapId`, `AfkX`, `AfkY`) VALUES ('%u', '%u', '%u', '%u')",
		sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y)) {
			Sql_ShowDebug(sql_handle);
			return 0;
		}
	}
	if(sd->afktime >= 180 && sd->status.state == PC_DIE && sd->bl.m != 4 && sd->bl.m != 73) {
		sd->totalafktime += 1800;
		
		if (map[sd->bl.m].region == 1) {
			pc_warp(sd, 4, 9, 7);
		} else if (map[sd->bl.m].region == 2) {
			pc_warp(sd, 73, 10, 6);
		}
	}
	
	return 0;
}

int pc_starttimer(USER *sd) {
	//sd->regenspeed = 45000;
	//sd->vregenspeed = 30000;
	//sd->mregenspeed = 30000;
	sd->timer=timer_insert(1000,1000,pc_timer,sd->bl.id,0);
	sd->regentimer=timer_insert(250,250,pc_regen,sd->bl.id,0);
	//sd->vregentimer=timer_insert(sd->vregenspeed,sd->vregenspeed,pc_vregen,sd->bl.id,0);
	//sd->mregentimer=timer_insert(sd->mregenspeed,sd->mregenspeed,pc_mregen,sd->bl.id,0);
	sd->pongtimer=timer_insert(30000,30000,pc_sendpong,sd->bl.id,0);
	//sd->savetimer=timer_insert(300000,300000,pc_savetimer,sd->bl.id,0);
	if(sd->status.gm_level < 50) {
		sd->afktimer=timer_insert(10000,10000,pc_afktimer,sd->bl.id,0);
	}
	sd->duratimer = timer_insert(1000, 1000, bl_duratimer, sd->bl.id, 0);
	sd->secondduratimer = timer_insert(250, 250, bl_secondduratimer, sd->bl.id, 0);
	sd->scripttimer = timer_insert(500, 500, pc_scripttimer, sd->bl.id, 0);
	
	return 0;
}
int pc_stoptimer(USER *sd) {

	if(sd->timer) timer_remove(sd->timer);
	if(sd->regentimer) timer_remove(sd->regentimer);
	//if(sd->vregentimer) timer_remove(sd->vregentimer);
	//if(sd->mregentimer) timer_remove(sd->mregentimer);
	if(sd->pongtimer)  timer_remove(sd->pongtimer);
	//if(sd->savetimer) timer_remove(sd->savetimer);
	if(sd->afktimer) timer_remove(sd->afktimer);
	if(sd->duratimer) timer_remove(sd->duratimer);
	if(sd->secondduratimer) timer_remove(sd->secondduratimer);
	if(sd->scripttimer) timer_remove(sd->scripttimer);

	return 0;
}
int bl_duratimer(int id, int none) {
	USER *sd = map_id2sd((unsigned int)id);
	struct block_list *tbl = NULL;
	MOB *tmob = NULL;
	int x, mid;
	long health;
	
	nullpo_ret(0, sd);

	for (x = 0; x < MAX_SPELLS; x++) {
		if (sd->status.skill[x] > 0) {
			sl_doscript_simple(magicdb_yname(sd->status.skill[x]), "while_passive", &sd->bl);
		}
	}
	
	for (x = 0; x < 14; x++) {
		if (sd->status.equip[x].id > 0) {
			sl_doscript_blargs(itemdb_yname(sd->status.equip[x].id), "while_equipped", 1, &sd->bl);
		}
	}
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
		mid = sd->status.dura_aether[x].id;
		if (sd->status.dura_aether[x].id > 0) {
			if (sd->status.dura_aether[x].caster_id > 0) {
				tbl = map_id2bl(sd->status.dura_aether[x].caster_id);
			}
			
			if (sd->status.dura_aether[x].duration > 0) {
				sd->status.dura_aether[x].duration -= 1000;
				
				//more options for while_cast
				if (tbl != NULL) {
					if (tbl->type == BL_MOB) {
						tmob = (MOB*)tbl;
						health = tmob->current_vita;
					}
			
					if (health > 0 || tbl->type == BL_PC) {
						sl_doscript_blargs(magicdb_yname(mid), "while_cast", 2, &sd->bl, tbl);
					}
				} else {
					sl_doscript_blargs(magicdb_yname(mid), "while_cast", 1, &sd->bl);
				}
			
				if(sd->status.dura_aether[x].duration <= 0) {
					sd->status.dura_aether[x].duration = 0;
					clif_send_duration(&sd->bl, sd->status.dura_aether[x].id, 0, map_id2sd(sd->status.dura_aether[x].caster_id));
					sd->status.dura_aether[x].caster_id = 0;
					map_foreachinarea(clif_sendanimation, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, sd->status.dura_aether[x].animation, &sd->bl, -1);
					sd->status.dura_aether[x].animation = 0;
					
					if(sd->status.dura_aether[x].aether == 0) {
						sd->status.dura_aether[x].id = 0;
					}
					
					if(tbl != NULL) {
						sl_doscript_blargs(magicdb_yname(mid), "uncast", 2, &sd->bl, tbl);
					} else {
						sl_doscript_blargs(magicdb_yname(mid), "uncast", 1, &sd->bl);
					}
				}
			}
			
			if (sd->status.dura_aether[x].aether > 0) {
				sd->status.dura_aether[x].aether -= 1000;
			
				if(sd->status.dura_aether[x].aether <= 0) {
					clif_send_aether(&sd->bl, sd->status.dura_aether[x].id, 0);
				
					if (sd->status.dura_aether[x].duration == 0) {
						sd->status.dura_aether[x].id = 0;
					}
				
					sd->status.dura_aether[x].aether = 0;
				}
			}
		}
	}

	return 0;
}

int bl_secondduratimer(int id, int none) {
	USER *sd = map_id2sd((unsigned int)id);
	struct block_list *tbl = NULL;
	MOB *tmob = NULL;
	int x, mid;
	long health;
	
	nullpo_ret(0, sd);

	for (x = 0; x < MAX_SPELLS; x++) {
		if (sd->status.skill[x] > 0) {
			sl_doscript_simple(magicdb_yname(sd->status.skill[x]), "while_passive_250", &sd->bl);
		}
	}
	
	for (x = 0; x < 14; x++) {
		if (sd->status.equip[x].id > 0) {
			sl_doscript_blargs(itemdb_yname(sd->status.equip[x].id), "while_equipped_250", 1, &sd->bl);
		}
	}
	
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
		mid = sd->status.dura_aether[x].id;
		if (sd->status.dura_aether[x].id > 0) {
			if (sd->status.dura_aether[x].caster_id > 0) {
				tbl = map_id2bl(sd->status.dura_aether[x].caster_id);
			}
			
			if (sd->status.dura_aether[x].duration > 0) {
				if (tbl != NULL) {
					if (tbl->type == BL_MOB) {
						tmob = (MOB*)tbl;
						health = tmob->current_vita;
					}
			
					if (health > 0 || tbl->type == BL_PC) {
						sl_doscript_blargs(magicdb_yname(sd->status.dura_aether[x].id), "while_cast_250", 2, &sd->bl, tbl);
					}
				} else {
					sl_doscript_blargs(magicdb_yname(sd->status.dura_aether[x].id), "while_cast_250", 1, &sd->bl);
				}
			}
		}
	}

	return 0;
}

int bl_aethertimer(int id, int none) {
	USER *sd = map_id2sd((unsigned int)id);
	int x;
	
	nullpo_ret(0, sd);
	for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
		if (sd->status.dura_aether[x].id > 0) {
			if(sd->status.dura_aether[x].aether > 0) {
				sd->status.dura_aether[x].aether -= 1000;
			}

			if(sd->status.dura_aether[x].aether <= 0) {
				clif_send_aether(&sd->bl, sd->status.dura_aether[x].id, 0);
			
				if (sd->status.dura_aether[x].duration == 0) {
					sd->status.dura_aether[x].id = 0;
				}
			
				sd->status.dura_aether[x].aether = 0;
				return 0;
			}
		}
	}

	return 0;
}

int pc_reload_aether(USER *sd) {
	int x;
	struct skill_info *p = NULL;
	for(x = 0; x < MAX_MAGIC_TIMERS; x++) {
		p = &sd->status.dura_aether[x];

		if(p->id > 0) {
			if(p->aether > 0) {
				clif_send_aether(sd, p->id, p->aether / 1000);
			}
		}
	}
	return 0;
}

int pc_magic_startup(USER *sd) {
	int x;
	struct skill_info *p = NULL;
	USER* tsd = NULL;
	
	nullpo_ret(0, sd);
	
	for(x = 0; x < MAX_MAGIC_TIMERS; x++) {
		p = &sd->status.dura_aether[x];

		if(p->id > 0) {
			if(p->duration > 0){
				tsd = map_id2sd(p->caster_id);
				clif_send_duration(sd, p->id, p->duration / 1000, map_id2sd(p->caster_id));
				
				if (tsd != NULL) {
					sd->target = p->caster_id;
					sd->attacker = p->caster_id;
					sl_doscript_blargs(magicdb_yname(p->id), "recast", 2, &sd->bl, &tsd->bl);
				} else {
					sd->target = sd->status.id;
					sd->attacker = sd->status.id;
					sl_doscript_blargs(magicdb_yname(p->id), "recast", 1, &sd->bl);
				}
			}
			
			if(p->aether > 0) {
				clif_send_aether(sd, p->id, p->aether / 1000);
			}
		}
	}
	
	return 0;
}


int pc_requestmp(USER* sd) {
	if(!sd) return 0;
	
	sd->flags = 0;

	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT `MalNew` FROM `Mail` WHERE `MalNew` = 1 AND `MalChaNameDestination` = '%s'", sd->status.name))
	{
		Sql_ShowDebug(sql_handle);
		return 0;
	}

	if(Sql_NumRows(sql_handle) > 0)
			sd->flags |= FLAG_MAIL;

	Sql_FreeResult(sql_handle);

	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT `ParItmId` FROM `Parcels` WHERE `ParChaIdDestination`='%u'",sd->status.id))
	{
		Sql_ShowDebug(sql_handle);
		return 0;
	}

	if(Sql_NumRows(sql_handle) > 0)
		sd->flags |= FLAG_PARCEL;

	Sql_FreeResult(sql_handle);
	return 0;
}

int pc_timer(int id,int none) {
	USER *sd = map_id2sd((unsigned int)id);
	int x;
	int reset = 0;
	
	nullpo_ret(1,sd);
	sd->time2 += 1000;
	sd->time = 0;
	sd->chat_timer = 0;
	
	if(sd->time2>=(60000)) {
		pc_requestmp(sd); //Request Mail/parcel
		sd->time2=0;
	}
	
	if(sd->coref) {
		sd->co_timer+=1000;
		if(sd->co_timer>=(300000)) {
			sl_async_freeco(sd);
			sd->co_timer=0;
		}
	}
	
	for (x = 0; x < 20; x++) {
		if (sd->pvp[x][1]) {
			if (gettick() - sd->pvp[x][1] >= 60000) {
				sd->pvp[x][0] = 0;
				sd->pvp[x][1] = 0;
				reset = 1;
			}
		}
	}
	
	if (sd->status.pk == 1 && sd->status.pkduration > 0) { //|| sd->bl.m == jailmap) {
		sd->status.pkduration -= 1000;
		
		if (sd->status.pkduration <= 0) {
			sd->status.pk = 0;
			clif_sendchararea(sd);
		}
	}

	if (reset) {
		clif_getchararea(sd);
	}
	
	return 0;
}

int pc_regen(int id,int none) {
	USER *sd;
	int x;
	float rate,rate2;
	int hp,mp;

	sd=map_id2sd(id);
	nullpo_ret(1,sd);

	if(sd->status.state==1 || sd->status.hp == 0) return 0;
	
	hp=sd->status.hp;
	mp=sd->status.mp;
	
	rate = ((((float)sd->max_hp * 0.01f) +
		((float)sd->max_hp * (((float)(sd->regen + sd->vregen) * 0.00025f) +
		((float)((sd->regen + sd->vregen) ^ 2) * 0.00000625f)) * 2.0f)) / 12.0f) + sd->vregenoverflow;
	//rate=(float)(((float)sd->max_hp * 0.015f) + ((float)sd->max_hp * (float)(sd->regen + sd->vregen) * 0.00075f));

	sd->vregenoverflow = (rate - (int)rate);
	
	/*if(rate > 0 && rate < 1) {
		rate = 1;
	}*/

	rate2 = ((((float)sd->max_mp * .0085f) +
		((float)sd->max_mp * ((float)((sd->regen + sd->mregen) + (sd->will / 5)) +
		((float)((sd->regen + sd->mregen) ^ 2) * 0.00000625f)) * 0.00042f)) / 12.0f) + sd->mregenoverflow;
	
	sd->mregenoverflow = (rate2 - (int)rate2);

	if ((long)sd->status.hp + (int)rate < 0) sd->status.hp = 0;
	else if (sd->status.hp + (int)rate > sd->max_hp) sd->status.hp = sd->max_hp;
	else sd->status.hp += (int)rate;
	
	if ((long)sd->status.mp + (int)rate2 < 0) sd->status.mp = 0;
	else if (sd->status.mp + (int)rate2 > sd->max_mp) sd->status.mp = sd->max_mp;
	else sd->status.mp += (int)rate2;

	if(sd->sendstatus_tick >= 20) clif_sendstatus(sd,SFLAG_HPMP);
	
	if (sd->sendstatus_tick >= 20) sd->sendstatus_tick = 1;
	else sd->sendstatus_tick++;
	
	return 0;
}
int pc_vregen(int id, int none) {
	USER *sd;
	int x;
	float rate;
	int hp;

	sd = map_id2sd(id);
	nullpo_ret(1, sd);
	
	if (sd->status.hp == sd->max_hp) return 0;

	if (sd->status.state == 1) return 0;
	
	hp = sd->status.hp;
	rate = (float)(((float)sd->max_hp * 0.015f) + ((float)sd->max_hp * (float)(sd->regen + sd->vregen) * 0.00075f));

	if (rate > 0 && rate < 1) {
		rate = 1;
	}

	sd->status.hp += (int)(rate + 0.5f);
	
	if (sd->status.hp > sd->max_hp) sd->status.hp = sd->max_hp;

	if (sd->status.hp != hp) clif_sendstatus(sd, SFLAG_HPMP);

	return 0;
}
int pc_mregen(int id, int none) {
	USER *sd;
	int x;
	float rate;
	int mp;

	sd = map_id2sd(id);
	nullpo_ret(1, sd);
	
	if (sd->status.mp == sd->max_mp) return 0;

	if (sd->status.state == 1) return 0;
	
	mp = sd->status.mp;
	
	if (sd->status.level < 10) {
		rate = (float)(((float)sd->max_mp * 0.015f) + ((float)sd->max_mp * ((float)(sd->regen + sd->mregen) + sd->will / 5) * 0.0005f));
	} else {
		rate = (float)(((float)sd->max_mp * 0.015f) + ((float)sd->max_mp * ((float)(sd->regen + sd->mregen) + sd->will / 10) * 0.0005f));
	}

	sd->status.mp += (int)(rate + 0.5f);

	if (sd->status.mp > sd->max_mp) sd->status.mp = sd->max_mp;

	if (sd->status.mp != mp) clif_sendstatus(sd, SFLAG_HPMP);

	return 0;
}
int pc_checklevel(USER *sd) {
	int x;
	int path;
	unsigned int lvlxp=0;
	int newlevel=0;
	path=sd->status.class;
	if(path>5) {
		path=classdb_path(path);
	}

	//newlevel=sd->status.level;
	for(x=sd->status.level;x<50;x++) {
		lvlxp=classdb_level(path,x);
		//printf("lvlxp: %u\n",lvlxp);
		/*if(newlevel>5 && path==0) {
			sd->status.exp=classdb_level(path,5)-1;
			sd->status.level=5;
			clif_sendstatus(sd,SFLAG_XPMONEY);
			return 0;
		}
		if(newlevel == 25 && sd->spec == 0) {
			sd->spec = 1;
			clif_popup(sd,"You have reached the 25th insight and can now choose a specialization. Hit the F1 key for more information.");
		}*/
		if(sd->status.exp>=lvlxp) {
			newlevel += 1;
			//break;
		}
	}
	for(x=0;x<newlevel;x++) {
		sl_doscript_blargs("onLevel", NULL, 1, &sd->bl);
	}
	//levelup info
	/*if(newlevel!=sd->status.level) {
		sd->status.level=newlevel;
		sd->status.basehp=58*newlevel;
		sd->status.basemp=58*newlevel;
		switch(path) {

		case 0:
			break;
		case 1: //Warrior
			sd->status.basemight=sd->status.level;
			sd->status.basegrace=sd->status.level/2;
			sd->status.basewill=sd->status.level/3;
			break;
		case 2: // Rogue
			sd->status.basegrace=sd->status.level;
			sd->status.basewill=sd->status.level/3;
			sd->status.basemight=sd->status.level/2;
			break;
		case 3: //Mage
			sd->status.basewill=sd->status.level;
			sd->status.basemight=sd->status.level/2;
			sd->status.basegrace=sd->status.level/3;
			break;
		case 4: //poet
			sd->status.basewill=sd->status.level;
			sd->status.basegrace=sd->status.level/2;
			sd->status.basemight=sd->status.level/3;
			break;
		default:
			break;
		}

		if(sd->status.basegrace<5) sd->status.basegrace=5;
		if(sd->status.basewill<5) sd->status.basewill=5;
		if(sd->status.basemight<5) sd->status.basemight=5;
		pc_calcstat(sd);
		pc_heal(sd,sd->max_hp,sd->max_mp,0);
		clif_sendstatus(sd,SFLAG_FULLSTATS|SFLAG_HPMP|SFLAG_XPMONEY);
		map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,253,&sd->bl,0);

	}*/
	return 0;
}
int pc_givexp(USER *sd, unsigned int exp, unsigned int xprate) {
	//int xp_rate=1;
	long long int totalxp;
	unsigned int defaultxp;
	unsigned int tempxp;
	unsigned int difxp;
	unsigned char xpstring[256];
	int len;
	int stack = 0;
	int bx,by;
	struct block_list *bl;
	USER *tsd = NULL;

	bx=sd->bl.x/BLOCK_SIZE;
	by=sd->bl.y/BLOCK_SIZE;

	//stack check
	for (bl = map[sd->bl.m].block[bx+by*map[sd->bl.m].bxs]; bl != NULL && stack < 32768; bl = bl->next) {
		tsd = map_id2sd(bl->id);
		if ((bl->type & BL_PC) && bl->x==sd->bl.x && bl->y==sd->bl.y && stack < 32768 && tsd->status.gm_level == 0) {
			stack += 1;
		}
	}
	if(stack > 1) {
		len=sprintf(xpstring,"You cannot gain experience while on top of other players.");
		clif_sendminitext(sd,xpstring,len);
		return 0;
	}

	//afk check
	if(sd->afk == 1) {
		len=sprintf(xpstring,"You cannot gain experience while AFK.");
		clif_sendminitext(sd,xpstring,len);
		return 0;
	}
	
	if(!exp) return 0;
	if(exp<0) {
		if(sd->status.exp<abs(exp)) {
			sd->status.exp=0;
		} else {
			sd->status.exp=sd->status.exp+exp;
		}
		//clif_sendstatus(sd);

		return 0;
	}

	totalxp=exp*xprate;
	difxp=4294967295-sd->status.exp;
	if(difxp>totalxp) {
		tempxp=sd->status.exp+totalxp;
		defaultxp=totalxp;
	} else {
		tempxp=sd->status.exp+difxp;
		defaultxp=difxp;
	}
	//tempxp=totalxp+sd->status.exp;

		sd->status.exp=tempxp;


	len=sprintf(xpstring,"%u experience!",defaultxp);

	pc_checklevel(sd);
	clif_sendminitext(sd,xpstring,len);
	clif_sendstatus(sd,SFLAG_XPMONEY);

	return 0;
}


int pc_calcstat(USER *sd) {
	USER* tsd = NULL;
	int x;
	unsigned int id;
	char buf[64];
	struct skill_info *p;
	
	nullpo_ret(0, sd);
	
	sd->dam=0;
	sd->hit=0;
	sd->miss=0;
	sd->crit = 0;
	sd->critmult = 0.0f;
	sd->deduction = 1.0f;
	sd->snare = 0;
	sd->sleep = 1.0f;
	sd->silence = 0;
	sd->paralyzed = 0;
	sd->blind = 0;
	sd->drunk = 0;
	sd->rage = 1.0f;
	sd->enchanted = 1.0f;
	sd->invis = 1.0f;
	sd->fury = 1.0f;
	sd->dodge = 0;
	sd->parry = 0;
	sd->block = 0;
	sd->pierce = 0;
	sd->armor = 0;
	sd->max_hp = sd->status.basehp;
	sd->max_mp = sd->status.basemp;
	sd->might = 0;
	sd->grace = 0;
	sd->will = 0;
	sd->max_dam=0;
	sd->min_dam=0;
	sd->speed = 80;
	sd->attack_speed = 20;
	sd->protection = 0;
	sd->ac = 0;
	sd->resist = 0;
	sd->physdeduct = 0;
	sd->regen = 0;
	sd->vregen = 0;
	sd->mregen = 0;
	sd->wisdom = 0;
	
	for (x = 0; x < 14; x++) {
		id=sd->status.equip[x].id;
		
		if(id>0) {
			sd->max_hp += itemdb_vita(id);
			sd->max_mp += itemdb_mana(id);
			sd->might += itemdb_might(id);
			sd->grace += itemdb_grace(id);
			sd->will += itemdb_will(id);
			sd->armor += itemdb_ac(id);
			sd->regen += itemdb_healing(id);
			sd->dam += itemdb_dam(id);
			sd->hit += itemdb_hit(id);
			sd->min_dam += itemdb_mindam(id);
			sd->max_dam += itemdb_maxdam(id);
			sd->dodge += itemdb_dodge(id);
			sd->parry += itemdb_parry(id);
			sd->block += itemdb_block(id);
			sd->protection += itemdb_protection(id);
			sd->resist += itemdb_resist(id);
			sd->vregen += itemdb_vregen(id);
			sd->mregen += itemdb_mregen(id);
			sd->attack_speed += itemdb_attackspeed(id);
			sd->wisdom += itemdb_wisdom(id);
			sd->physdeduct += itemdb_physdeduct(id);
		}
	}
	
	sl_doscript_blargs("remount", NULL, 1, &sd->bl);
		
	if (sd->status.state != PC_DIE) {
		for (x = 0; x < MAX_MAGIC_TIMERS; x++) {
			p = &sd->status.dura_aether[x];
			
			if (p->id > 0 && p->duration > 0) {
				tsd = map_id2sd(p->caster_id);
				
				if (tsd != NULL) {
					sl_doscript_blargs(magicdb_yname(p->id), "recast", 2, &sd->bl, &tsd->bl);
				} else {
					sl_doscript_simple(magicdb_yname(p->id), "recast", &sd->bl);
				}
			}
		}
		
		for (x = 0; x < MAX_SPELLS; x++) {
			if (sd->status.skill[x] > 0) {
				sl_doscript_simple(magicdb_yname(sd->status.skill[x]), "passive", &sd->bl);
			}
		}
		
		for (x = 0; x < 14; x++) {
			if (sd->status.equip[x].id > 0) {
				sl_doscript_blargs(itemdb_yname(sd->status.equip[x].id), "re_equip", 1, &sd->bl);
			}
		}
	}
	
	sd->ac = (int)(((float)sd->armor * (127.0f / 142462.0f)) + 0.5f);
	
	if (sd->ac > 85) sd->ac = 85;
	if (sd->might > 255) sd->might = 255;
	if (sd->grace > 255) sd->grace = 255;
	if (sd->will > 255) sd->will = 255;
	if (sd->dam > 255) sd->dam = 255;
	if (sd->ac < 0) sd->ac = 0;
	if (sd->might < 0) sd->might = 0;
	if (sd->grace < 0) sd->grace = 0;
	if (sd->will < 0) sd->will = 0;
	if (sd->dam < 0) sd->dam = 0;
	if (sd->attack_speed < 3) sd->attack_speed = 3;
	if (sd->speed < 0) sd->speed = 0;

	sd->max_hp += ((sd->might * 25) + (sd->grace * 15) + (sd->will * 5));
	sd->max_mp += ((sd->will * 25) + (sd->grace * 15) + (sd->might * 5));
				
	if (sd->status.hp>sd->max_hp) sd->status.hp = sd->max_hp;
	if (sd->status.mp>sd->max_mp) sd->status.mp = sd->max_mp;
	
	clif_sendstatus(sd, SFLAG_FULLSTATS | SFLAG_HPMP | SFLAG_XPMONEY);
	//map_foreachinarea(clif_updatestate, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, sd);
	return 0;

}
float pc_calcdamage(USER *sd) {
	float damage;
	int ran;

	damage = 6.0f + (float) sd->might/8.0f;
	//damage=damage+(pc_calcdam(sd)*5.0f);
	if(sd->min_dam>0 && sd->max_dam>0){
	ran=sd->max_dam-sd->min_dam;
	if(ran<=0) ran=1;
	ran=rnd(ran) + sd->min_dam;
	damage += (float) ran/2.0f;
	}
	return damage;
}
int pc_calcdam(USER *sd) {
	int dam=1;

	return dam;
}
int pc_runfloor_sub(USER *sd) {
	NPC *nd = NULL;
	nullpo_ret(0, sd);
	
	nd=(NPC *)map_firstincell(sd->bl.m, sd->bl.x, sd->bl.y, BL_NPC);
	nullpo_ret(0, nd);
	
	if (nd->bl.subtype!=FLOOR && nd->bl.subtype!=2)
		return 0;

	if(nd->bl.subtype == 2) {
		sl_async_freeco(sd);
		sl_doscript_blargs(nd->name, "click", 2, &sd->bl, &nd->bl);
	}
	
	return 0;
}

int pc_setpos(USER *sd,int m,int x,int y) {
	if (sd->bl.id >= MOB_START_NUM) return 0;
	sd->bl.m = m;
	sd->bl.x = x;
	sd->bl.y = y;
	sd->bl.type = BL_PC;

	return 0;
}

int pc_warp(USER *sd,int m,int x,int y) {
	nullpo_ret(0, sd);
	int i;
	int oldmap = sd->bl.m;
	int oldx = sd->bl.x;
	int oldy = sd->bl.y;
	
	if(m < 0)
		m = 0;

	if(m > MAX_MAP_PER_SERVER)
		m = MAX_MAP_PER_SERVER;

	if (!map_isloaded(m)) {
		//Nothing happen
		//We must discover change server packet first!!
		return 0;
	}

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (x >= map[m].xs) x = map[m].xs-1;
	if (y >= map[m].ys) y = map[m].ys-1;
	
	if(m != oldmap) {
		sl_doscript_blargs("mapLeave", NULL, 1, &sd->bl);
		
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `WarpLogs` (`WrpChaId`, `WrpMapId`, `WrpX`, `WrpY`, `WrpMapIdDestination`, `WrpXDestination`, `WrpYDestination`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u', '%u')",
				sd->status.id, oldmap, oldx, oldy, m, x, y)) {
			SqlStmt_ShowDebug(sql_handle);
		}
	}
	
	for (i = 0; i < MAX_SPELLS; i++) {
		sl_doscript_blargs(magicdb_yname(sd->status.skill[i]), "passive_before_warp", 1, &sd->bl);
	}
	
	for (i = 0; i < MAX_MAGIC_TIMERS; i++) {
		sl_doscript_blargs(magicdb_yname(sd->status.dura_aether[i].id), "before_warp_while_cast", 1, &sd->bl);
	}
	
	//sendaction
	//sendside
	//sendid
	//0x67 14LwE (11)L
	//mapinfo
	//updatestatus
	//sendxy
	//sendweather
	//music
	//destroy
	//get char info
	clif_quit(sd);
	pc_setpos(sd,m,x,y);
	clif_sendtime(sd);
	//clif_sendmapinfo(sd);
	clif_spawn(sd);
	clif_parserefresh(sd);
	//clif_sendxy(sd);
	//clif_mob_look_start(sd);
	//map_foreachinarea(clif_object_look_sub,sd->bl.m,sd->bl.x,sd->bl.y,SAMEAREA,BL_ALL,LOOK_GET,sd);
	//clif_mob_look_close(sd);
	//clif_getchararea(sd);
	if(m != oldmap) {
		sl_doscript_blargs("mapEnter", NULL, 1, &sd->bl);
	}
	
	for (i = 0; i < MAX_SPELLS; i++) {
		sl_doscript_blargs(magicdb_yname(sd->status.skill[i]), "passive_on_warp", 1, &sd->bl);
	}
	
	for (i = 0; i < MAX_MAGIC_TIMERS; i++) {
		sl_doscript_blargs(magicdb_yname(sd->status.dura_aether[i].id), "on_warp_while_cast", 1, &sd->bl);
	}
	
	return 0;
}

int pc_loadequiprealname(USER* sd) {

	int itemnum;
	int i;

	return 0;
}
int pc_loaditemrealname(USER* sd) {

	int itemnum;
	int i;

	return 0;
}
int pc_loaditem(USER *sd) {
	int i;
	for (i=0;i<sd->status.maxinv;i++) {
		if (sd->status.inventory[i].id) {
			clif_sendadditem(sd, i);
		}
	}
	return 0;
}

int pc_loadequip(USER *sd) {
	int i;

	for (i=0;i<14;i++) {


		if (sd->status.equip[i].id>0) {

			clif_sendequip(sd, i);
		}
	}
	//sql_free_row();
	return 0;
}

int pc_isinvenspace(USER *sd, int id, int owner, char *engrave) {
	int i;
	int uniqueamount = 0;
	
	if (itemdb_unique(id) > 0) {
		for (i = 0; i < sd->status.maxinv; i++) {
			if(sd->status.inventory[i].id == id && itemdb_unique(id) > 0 && sd->status.inventory[i].owner_id == owner && !(strcmpi(sd->status.inventory[i].real_name, engrave))) {
				uniqueamount += sd->status.inventory[i].amount;
			}
		}
	
		for (i = 0; i < 14; i++) {
			if (sd->status.equip[i].id == id && itemdb_unique(id) > 0 && sd->status.inventory[i].owner_id == owner && !(strcmpi(sd->status.inventory[i].real_name, engrave))) {
				uniqueamount += 1;
			}
		}
		
		for (i = 0; i < sd->status.maxinv; i++) {
			if (sd->status.inventory[i].id == id && sd->status.inventory[i].amount < itemdb_maxamount(id) && uniqueamount < itemdb_unique(id) && sd->status.inventory[i].owner_id == owner && !(strcmpi(sd->status.inventory[i].real_name, engrave)))
				return i;
		}
		
		for (i = 0; i < sd->status.maxinv; i++) {
			if (sd->status.inventory[i].id == 0 && uniqueamount < itemdb_unique(id) && sd->status.inventory[i].owner_id == owner && !(strcmpi(sd->status.inventory[i].real_name, engrave)))
				return i;
		}
		
		return sd->status.maxinv;
	} else {
		for (i = 0; i < sd->status.maxinv; i++) {
			if (sd->status.inventory[i].id == id && sd->status.inventory[i].amount < itemdb_maxamount(id) && sd->status.inventory[i].owner_id == owner && !(strcmpi(sd->status.inventory[i].real_name, engrave)))
				return i;
		}
	
		for (i = 0; i < sd->status.maxinv; i++) {
			if (sd->status.inventory[i].id == 0)
				return i;
		}
	
		return sd->status.maxinv;
	}
	
	return 0;
}

int pc_checkformail(USER *sd) {
	int num=0;
	//sql_request("SELECT * FROM nmail WHERE touser='%s' AND new=1",sd->status.name);

	//sd->hasmail=1;

	//if(sql_get_row()) {
		//sd->hasmail=0;
	//}





	//sql_free_row();

	return 0;
}

int pc_isinvenitemspace(USER *sd, int num, int id, int owner, char *engrave) {
	int i;
	int uniqueamount = 0;
	char msg[255];
	
	if (itemdb_unique(id) > 0) {
		for (i = 0; i < sd->status.maxinv; i++) {
			if(sd->status.inventory[i].id == id && itemdb_unique(id) > 0) {
				uniqueamount += sd->status.inventory[i].amount;
			}
		}
	
		for (i = 0; i < 14; i++) {
			if (sd->status.equip[i].id == id && itemdb_unique(id) > 0 && (sd->takeoffid == -1 || sd->status.equip[sd->takeoffid].id != id)) {
				uniqueamount += 1;
			}
		}
		
		if (!sd->status.inventory[num].id && itemdb_unique(id) - uniqueamount >= itemdb_maxamount(id)) {
			return itemdb_maxamount(id);
		} else if (sd->status.inventory[num].id != id || sd->status.inventory[num].owner_id != owner || (strcmpi(sd->status.inventory[num].real_name, engrave))) {
			return 0;
		} else {
			return itemdb_unique(id) - uniqueamount;
		}
	} else {
		if (!sd->status.inventory[num].id) {
			return itemdb_maxamount(id);
		} else if (sd->status.inventory[num].id != id || sd->status.inventory[num].owner_id != owner || (strcmpi(sd->status.inventory[num].real_name, engrave))) {
			return 0;
		} else {
			return itemdb_maxamount(id) - sd->status.inventory[num].amount;
		}
	}
	
	return 0;
}

int pc_additem(USER *sd, struct item *fl) {
	char RegStr[] = "goldbardupe";
	char msg[255], escape[255];
	int num;
	int i;
	
	if (fl->id == 0 && fl->amount)
    {
         //clif_Hacker(sd->status.name,"gold-bar-dupe");
         //int DupeTimes=pc_readglobalreg(sd, RegStr) + 1;
         //pc_setglobalreg(sd, RegStr, DupeTimes);
		return 0;
    }

	if ((num = pc_isinvenspace(sd, fl->id, fl->owner_id, fl->real_name)) >= sd->status.maxinv) {
		if (itemdb_unique(fl->id) > 0) {
			pc_dropitemfull(sd, fl);
			clif_sendminitext(sd, map_msg[MAP_ERRITMUNIQUE].message, map_msg[MAP_ERRITMUNIQUE].len);
		} else {
			clif_sendminitext(sd, map_msg[MAP_ERRITMFULL].message, map_msg[MAP_ERRITMFULL].len);
			pc_dropitemfull(sd, fl);
		}
		
		return 0; //Used to be 1
	}
	
	do {
		if (fl->amount > (i = pc_isinvenitemspace(sd, num, fl->id, fl->owner_id, fl->real_name))) {
			Sql_EscapeString(sql_handle,escape,fl->real_name);
			
			if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `PickupLogs` (`PicChaId`, `PicMapId`, `PicX`, `PicY`, `PicItmId`, `PicAmount`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u')",
					sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, fl->id, i)) {
				SqlStmt_ShowDebug(sql_handle);
			}
			
			sd->status.inventory[num].id = fl->id;
			sd->status.inventory[num].dura = fl->dura;
			sd->status.inventory[num].owner_id = fl->owner_id;
			strcpy(sd->status.inventory[num].real_name, fl->real_name);
			sd->status.inventory[num].custom = fl->custom;
			sd->status.inventory[num].amount += i;
			fl->amount -= i;
		} else {
			Sql_EscapeString(sql_handle,escape,fl->real_name);
		
			if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `PickupLogs` (`PicChaId`, `PicMapId`, `PicX`, `PicY`, `PicItmId`, `PicAmount`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u')",
					sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, fl->id, fl->amount)) {
				SqlStmt_ShowDebug(sql_handle);
			}
			
			sd->status.inventory[num].id = fl->id;
			sd->status.inventory[num].dura = fl->dura;
			sd->status.inventory[num].owner_id = fl->owner_id;
			strcpy(sd->status.inventory[num].real_name, fl->real_name);
			sd->status.inventory[num].custom = fl->custom;
			sd->status.inventory[num].amount += fl->amount;
			fl->amount = 0;
		}
		
		clif_sendadditem(sd, num);
		num = pc_isinvenspace(sd, fl->id, fl->owner_id, fl->real_name);
	} while (fl->amount && num < sd->status.maxinv);

	if (num >= sd->status.maxinv && fl->amount) {
		if (itemdb_unique(fl->id) > 0) {
			pc_dropitemfull(sd, fl);
			clif_sendminitext(sd, map_msg[MAP_ERRITMUNIQUE].message, map_msg[MAP_ERRITMUNIQUE].len);
		} else {
			pc_dropitemfull(sd, fl);
			clif_sendminitext(sd, map_msg[MAP_ERRITMFULL].message, map_msg[MAP_ERRITMFULL].len);
		}
	}
	
	return 0;
}

int pc_additemnolog(USER *sd, struct item *fl) {
	char RegStr[] = "goldbardupe";
	char msg[255];
	int num;
	int i;
	
	if (fl->id == 0 && fl->amount)
    {
         //clif_Hacker(sd->status.name,"gold-bar-dupe");
         //int DupeTimes=pc_readglobalreg(sd, RegStr) + 1;
         //pc_setglobalreg(sd, RegStr, DupeTimes);
		return 0;
    }

	if ((num = pc_isinvenspace(sd, fl->id, fl->owner_id, fl->real_name)) >= sd->status.maxinv) {
		if (itemdb_unique(fl->id) > 0) {
			pc_dropitemfull(sd, fl);
			clif_sendminitext(sd, map_msg[MAP_ERRITMUNIQUE].message, map_msg[MAP_ERRITMUNIQUE].len);
		} else {
			clif_sendminitext(sd, map_msg[MAP_ERRITMFULL].message, map_msg[MAP_ERRITMFULL].len);
			pc_dropitemfull(sd, fl);
		}
		
		return 0; //Used to be 1
	}
	
	do {
		if (fl->amount > (i = pc_isinvenitemspace(sd, num, fl->id, fl->owner_id, fl->real_name))) {
			sd->status.inventory[num].id = fl->id;
			sd->status.inventory[num].dura = fl->dura;
			sd->status.inventory[num].owner_id = fl->owner_id;
			strcpy(sd->status.inventory[num].real_name, fl->real_name);
			sd->status.inventory[num].custom = fl->custom;
			sd->status.inventory[num].amount += i;
			fl->amount -= i;
		} else {
			sd->status.inventory[num].id = fl->id;
			sd->status.inventory[num].dura = fl->dura;
			sd->status.inventory[num].owner_id = fl->owner_id;
			strcpy(sd->status.inventory[num].real_name, fl->real_name);
			sd->status.inventory[num].custom = fl->custom;
			sd->status.inventory[num].amount += fl->amount;
			fl->amount = 0;
		}
		
		clif_sendadditem(sd, num);
		num = pc_isinvenspace(sd, fl->id, fl->owner_id, fl->real_name);
	} while (fl->amount && num < sd->status.maxinv);

	if (num >= sd->status.maxinv && fl->amount) {
		if (itemdb_unique(fl->id) > 0) {
			pc_dropitemfull(sd, fl);
			clif_sendminitext(sd, map_msg[MAP_ERRITMUNIQUE].message, map_msg[MAP_ERRITMUNIQUE].len);
		} else {
			pc_dropitemfull(sd, fl);
			clif_sendminitext(sd, map_msg[MAP_ERRITMFULL].message, map_msg[MAP_ERRITMFULL].len);
		}
	}
	
	return 0;
}

int pc_delitem(USER *sd, int id, int amount, int type) {
	char minitext[255];
	
	if (!sd->status.inventory[id].id)
		return 0;

	sd->status.inventory[id].amount -= amount;
	
	if (sd->status.inventory[id].amount <= 0) {
		memset(&sd->status.inventory[id], 0, sizeof(struct item));
		clif_senddelitem(sd, id, type);
	} else {
		sprintf(minitext, "%s, (%d) removed.", itemdb_name(sd->status.inventory[id].id), amount);
		clif_sendminitext(sd, minitext, strlen(minitext));
		clif_sendadditem(sd, id);
	}
	
	return 0;
}

int pc_canequipitem(USER *sd, int id) {
	int itemid;
	//if(!sd->status.inventory[id].custom) {
		itemid = sd->status.inventory[id].id;
	//} else {
	//	itemid = sd->status.inventory[id].custom;
	//}

	//if(!sd->status.inventory[id].custom) {
		if (sd->status.level < itemdb_level(itemid))
			return MAP_ERRITMLEVEL;
		if (sd->might < itemdb_mightreq(itemid))
			return MAP_ERRITMMIGHT;
		if (sd->grace < itemdb_gracereq(itemid))
			return MAP_ERRITMGRACE;
		if (sd->will < itemdb_willreq(itemid))
			return MAP_ERRITMWILL;
		if (sd->status.sex != itemdb_sex(itemid) && itemdb_sex(itemid) != 2)
			return MAP_ERRITMSEX;
		if(itemdb_reqvita(itemid) > 0 && itemdb_reqmana(itemid) > 0) {
			if(sd->status.basehp < itemdb_reqvita(itemid) && sd->status.basemp < itemdb_reqmana(itemid)) {
				return MAP_ERRVITA;
			}
		} else {
			if( sd->status.basehp < itemdb_reqvita(itemid))
				return MAP_ERRVITA;
			if( sd->status.basemp < itemdb_reqmana(itemid))
				return MAP_ERRMANA;
		}
	//} else {
	//	if (sd->status.level < itemdb_levelc(itemid))
	//		return MAP_ERRITMLEVEL;
	//	if (sd->might < itemdb_mightreqc(itemid))
	//		return MAP_ERRITMMIGHT;
	//	if (sd->grace < itemdb_gracereqc(itemid))
	//		return MAP_ERRITMGRACE;
	//	if (sd->will < itemdb_willreqc(itemid))
	//		return MAP_ERRITMWILL;
	//	if (sd->status.sex != itemdb_sexc(itemid) && itemdb_sexc(itemid) != 2)
	//		return MAP_ERRITMSEX;
	//	if(itemdb_reqvitac(itemid) > 0 && itemdb_reqmanac(itemid) > 0) {
	//		if(sd->status.basehp < itemdb_reqvitac(itemid) && sd->status.basemp < itemdb_reqmanac(itemid)) {
	//			return MAP_ERRVITA;
	//		}
	//	} else {
	//		if( sd->status.basehp < itemdb_reqvitac(itemid))
	//			return MAP_ERRVITA;
	//		if( sd->status.basemp < itemdb_reqmanac(itemid))
	//			return MAP_ERRMANA;
	//	}
	//}


	return 0;
}
int pc_canequipstats(USER* sd, int id) {
	int i, itemid;

	for(i=0;i<sd->status.maxinv;i++) {
		if(sd->status.inventory[i].id == id) {
			itemid = i;
			break;
		}
	}
	
	//if(!sd->status.inventory[itemid].custom) {
		if(itemdb_vita(id) < 0 && abs(itemdb_vita(id)) > sd->max_hp) return 0;
		if(itemdb_mana(id) < 0 && abs(itemdb_mana(id)) > sd->max_mp) return 0;
	//} else {
	//	if(itemdb_vitac(id) < 0 && abs(itemdb_vitac(id)) > sd->max_hp) return 0;
	//	if(itemdb_manac(id) < 0 && abs(itemdb_manac(id)) > sd->max_mp) return 0;
	//}

	return 1;
}

	//if(itemdb_ac(id) < 0 && abs(itemdb_ac(id)) > sd->max_hp) return 0;
int pc_equipitem(USER *sd, int id) {
	int ret;
	
	if (!sd->status.inventory[id].id)
		return 0;
	
	if(sd->status.inventory[id].owner_id) {
		if(sd->status.inventory[id].owner_id!=sd->bl.id) {
			clif_sendminitext(sd,"This does not belong to you.",0);
			return 0;
		}

	}

	ret=pc_canequipitem(sd, id);

	if (ret) {
		clif_sendminitext(sd, map_msg[ret].message, map_msg[ret].len);
		return 0;
	}

	ret=itemdb_type(sd->status.inventory[id].id)-3;

	if (ret < 0 || ret > 14) //not equip item
		return 0;

	if(!pc_canequipstats(sd,sd->status.inventory[id].id)) {
		clif_sendminitext(sd, "Your stats are too low to equip that.",0);
		return 0;
	}
	
	/*if(ret==EQ_WEAP) {
		if(sd->status.equip[EQ_SHIELD].id!=0 && (itemdb_look(sd->status.inventory[id].id)>9999 && itemdb_look(sd->status.inventory[id].id)<30000)) {
			return 0;
		}
	}
	
	if(ret==EQ_SHIELD){
		if((itemdb_look(sd->status.equip[EQ_WEAP].id)>9999 && itemdb_look(sd->status.equip[EQ_WEAP].id)<30000)) {
			return 0;
		}
	}*/

	sd->equipid = sd->status.inventory[id].id;
	sd->invslot = id;
	sl_doscript_blargs("onEquip", NULL, 1, &sd->bl);
	return 0;
}

pc_equipscript(USER* sd) {
	char escape[255];
	int ret = itemdb_type(sd->equipid) - 3;
	struct item it;
	
	if (ret == EQ_LEFT) {
		if (sd->status.equip[EQ_LEFT].id && !sd->status.equip[EQ_RIGHT].id)
			ret=EQ_RIGHT;
		else
			ret=EQ_LEFT;
	}
	
	if (ret == EQ_RIGHT) {
		if (sd->status.equip[EQ_RIGHT].id && !sd->status.equip[EQ_LEFT].id)
			ret=EQ_LEFT;
		else
			ret=EQ_RIGHT;
	}
	
	if(ret == EQ_SUBLEFT) {
		if(sd->status.equip[EQ_SUBLEFT].id && !sd->status.equip[EQ_SUBRIGHT].id)
			ret=EQ_SUBLEFT;
		else
			ret=EQ_SUBRIGHT;
	}

	if(ret == EQ_SUBRIGHT) {
		if(sd->status.equip[EQ_SUBRIGHT].id && !sd->status.equip[EQ_SUBLEFT].id)
			ret=EQ_SUBLEFT;
		else
			ret=EQ_SUBRIGHT;
	}

	if (sd->status.equip[ret].id) {
		sd->target=sd->bl.id;
		sd->attacker = sd->bl.id;
		sd->takeoffid = ret;
		sl_doscript_blargs("onUnequip", NULL, 1, &sd->bl);
		sl_doscript_blargs(itemdb_yname(sd->equipid),"equip",1,&sd->bl);
		sd->equipid = 0;
		return 0;
	} else {
		Sql_EscapeString(sql_handle,escape,sd->status.inventory[sd->invslot].real_name);
			
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `EquipLogs` (`ElgChaId`, `ElgMapId`, `ElgX`, `ElgY`, `ElgItmId`) VALUES ('%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, sd->status.inventory[sd->invslot].id)) {
			SqlStmt_ShowDebug(sql_handle);
		}
		
		memcpy(&sd->status.equip[ret], &sd->status.inventory[sd->invslot], sizeof(struct item));
		pc_delitem(sd, sd->invslot, 1, 6);
		sl_doscript_blargs(itemdb_yname(sd->equipid),"equip",1,&sd->bl);
		sd->equipid = 0;
	}

/*		memcpy(&it, &sd->status.equip[ret], sizeof(struct item));
		memcpy(&sd->status.equip[ret], &sd->status.inventory[id], sizeof(struct item));

		pc_delitem(sd, id, 1, 6);
		pc_additem(sd, &it);
*/	

	if(ret==EQ_WEAP && sd->enchanted > 1.0f){
        sd->enchanted = 1.0f;
        sd->flank = 0;
        sd->backstab = 0;
        char text[] = "Your weapon loses its enchantment.";
        clif_sendminitext(sd, text, strlen(text));
    }

	clif_sendequip(sd, ret);
	sd->status.equip[ret].amount=1;
	pc_calcstat(sd);
	map_foreachinarea(clif_updatestate,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,sd);
	return 0;
}

int pc_unequip(USER *sd, int type) {
	if(sd->status.equip[type].id==0)
		return 1;
	
	sd->takeoffid = type;
	sl_doscript_blargs("onUnequip", NULL, 1, &sd->bl);
	return 0;
}

int pc_unequipscript(USER* sd) {
	char escape[255];
	struct item it;
	int type = sd->takeoffid;
	unsigned int takeoff = sd->status.equip[type].id;
	
	if (sd->equipid > 0) {
		Sql_EscapeString(sql_handle,escape,sd->status.inventory[sd->invslot].real_name);
			
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `EquipLogs` (`ElgChaId`, `ElgMapId`, `ElgX`, `ElgY`, `ElgItmId`) VALUES ('%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, sd->status.inventory[sd->invslot].id)) {
			SqlStmt_ShowDebug(sql_handle);
		}
		
		Sql_EscapeString(sql_handle,escape,it.real_name);
			
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `UnequipLogs` (`UlgChaId`, `UlgMapId`, `UlgX`, `UlgY`, `UlgItmId`) VALUES ('%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, it.id)) {
			SqlStmt_ShowDebug(sql_handle);
		}
	
		memcpy(&it, &sd->status.equip[type], sizeof(struct item));
		memcpy(&sd->status.equip[type], &sd->status.inventory[sd->invslot], sizeof(struct item));
		pc_delitem(sd, sd->invslot, 1, 6);
		pc_additem(sd, &it);
		clif_sendequip(sd, type);
		sd->status.equip[type].amount=1;
	} else {
		memcpy(&it, &sd->status.equip[type], sizeof(struct item));
	
		if(&it.id <= 0)
			return 1;

		if (pc_additem(sd, &it))
			return 1;

		Sql_EscapeString(sql_handle,escape,it.real_name);
			
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `UnequipLogs` (`UlgChaId`, `UlgMapId`, `UlgX`, `UlgY`, `UlgItmId`) VALUES ('%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, it.id)) {
			SqlStmt_ShowDebug(sql_handle);
		}
			
		memset(&sd->status.equip[type], 0, sizeof(struct item));
		sd->target=sd->bl.id;
		sd->attacker = sd->bl.id;
	}
	
	if(type==EQ_WEAP && sd->enchanted > 1.0f){
		sd->enchanted = 1.0f;
		sd->flank = 0;
		sd->backstab = 0;
        char text[] = "Your weapon loses its enchantment.";
        clif_sendminitext(sd, text, strlen(text));
	}
	
	sl_doscript_simple(itemdb_yname(takeoff),"unequip",&sd->bl);
	sd->takeoffid = -1;
	pc_calcstat(sd);
	map_foreachinarea(clif_updatestate,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,sd);
	//if (type > 0 && type < 15)
	//	map_foreachinarea(clif_updatestate,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,sd);
	return 0;
}
int pc_isyours(USER* sd, FLOORITEM* fl) {
	int x;
	if(sd->status.gm_level) return 1;
	if(fl->canloot[0]==0) return 1;

	for(x=0;x<MAX_GROUP_MEMBERS;x++) {
		if(fl->canloot[x]==sd->bl.id)
			return 1;
	}

return 0;
}

int pc_getitemmap(USER *sd, int type) {
	FLOORITEM *fl;
	
	fl=(FLOORITEM *)map_firstincell(sd->bl.m,sd->bl.x,sd->bl.y,BL_ITEM);
	nullpo_ret(0, sd);
	nullpo_ret(0,fl);

	if(!pc_isyours(sd,fl)) {
		clif_sendminitext(sd,"This item does not belong to you.",0);
		return 0;
	}
	
	sd->pickuptype = type;
	sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	return 0;
}

int pc_getitemsaround(USER *sd) {
	FLOORITEM *fl;
	USER *tsd;
	nullpo_ret(0, sd);
	
	sd->pickuptype = 3;
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x, sd->bl.y, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x, sd->bl.y, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x + 1, sd->bl.y, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x + 1, sd->bl.y, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x - 1, sd->bl.y, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x - 1, sd->bl.y, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x, sd->bl.y + 1, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x, sd->bl.y + 1, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x, sd->bl.y - 1, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x, sd->bl.y - 1, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x + 1, sd->bl.y + 1, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x + 1, sd->bl.y + 1, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x + 1, sd->bl.y - 1, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x + 1, sd->bl.y - 1, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x - 1, sd->bl.y + 1, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x - 1, sd->bl.y + 1, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	fl = (FLOORITEM *)map_firstincell(sd->bl.m, sd->bl.x - 1, sd->bl.y - 1, BL_ITEM);
	tsd = (USER *)map_firstincell(sd->bl.m, sd->bl.x - 1, sd->bl.y - 1, BL_PC);
	
	if (fl != NULL && pc_isyours(sd, fl) && (tsd == NULL || tsd != NULL && tsd->bl.id == sd->bl.id)) {
		sl_doscript_blargs("onPickUp", NULL, 2, &sd->bl, &fl->bl);
	}
	
	return 0;
}

int pc_getitemscript(USER *sd, int id) {
	FLOORITEM *fl = map_id2fl(id);
	struct item it;
	int add=0;
	char escape[255];

	if(fl->data.id<=10) {
		sd->status.money+=fl->data.amount;
		clif_sendstatus(sd,SFLAG_XPMONEY);
		clif_lookgone(&fl->bl);
		map_delitem(fl->bl.id);
		Sql_EscapeString(sql_handle,escape,fl->data.real_name);
	
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `PickupLogs` (`PicChaId`, `PicMapId`, `PicX`, `PicY`, `PicItmId`, `PicAmount`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, fl->data.id, fl->data.amount)) {
			SqlStmt_ShowDebug(sql_handle);
		}
		
		if(fl->timer) timer_remove(fl->timer);

		return 0;
	}
	
	if (sd->pickuptype > 0 && fl->data.amount > 1) {
		memcpy(&it, &fl->data, sizeof(struct item));
        add=1;
		fl->data.amount = 0;
	} else {
		memcpy(&it, &fl->data, sizeof(struct item));
		it.amount=1;
		fl->data.amount--;
		add=1;
	}
	
	if (fl->data.amount <= 0) {
		clif_lookgone(&fl->bl);
		map_delitem(fl->bl.id);

		if(fl->timer) timer_remove(fl->timer);
	}

	if(add)pc_additem(sd, &it);

	if(sd->pickuptype > 0)
		if(fl->data.amount > 0) return 0;


	return 0;
}

int pc_addtocurrent(struct block_list *bl, va_list ap) {

	int *def;
	int id=0;
	USER *sd;
	FLOORITEM *fl;
	int type=0;

	nullpo_ret(0,fl=(FLOORITEM*)bl);

	def=va_arg(ap,int*);
	id=va_arg(ap,int);
	type=va_arg(ap,int);
	nullpo_ret(0,sd=va_arg(ap,USER*));

	if(def[0]) return 0;
	//if(!fl->data.custom) {
		if(fl->data.dura<itemdb_dura(fl->data.id)) return 0;
	//} else {
	//	if(fl->data.dura<itemdb_durac(fl->data.custom)) return 0;
	//}

	if(fl->data.id==sd->status.inventory[id].id && fl->data.owner_id == sd->status.inventory[id].owner_id && !(strcmpi(fl->data.real_name, sd->status.inventory[id].real_name))) {
		fl->lastamount = fl->data.amount;
		
		if(type) {
			fl->data.amount+=sd->status.inventory[id].amount;
		} else {
			fl->data.amount+=1;
		}

		if(fl->timer) timer_remove(fl->timer);
		if(map[fl->bl.m].sweeptime) fl->timer=timer_insert(map[fl->bl.m].sweeptime,map[fl->bl.m].sweeptime,pc_item_timer,fl->bl.id,0);

		sl_doscript_blargs("onDrop", NULL, 2, &sd->bl, &fl->bl);
		def[0]=fl->bl.id;
	}
	
	return 0;
}
int pc_npc_drop(struct block_list* bl, va_list ap) {
	FLOORITEM *fl;
	struct npc_data *nd;
	USER *sd;
	nullpo_ret(0,nd=(struct npc_data*)bl);
	nullpo_ret(0,fl=va_arg(ap,FLOORITEM*));
	nullpo_ret(0,sd=va_arg(ap,USER*));

	if(nd->bl.subtype!=FLOOR) return 0;
	//if(def[1]) return 0;

	sl_doscript_blargs(nd->name,"on_drop",3,&sd->bl,&nd->bl,&fl->bl);

	return 0;
}

int pc_dropitemmap(USER *sd, int id, int type) {
	FLOORITEM *fl;
	char escape[255];
	
	if(id>sd->status.maxinv)
		return 0;

	if (!sd->status.inventory[id].id)
		return 0;

	if(sd->status.inventory[id].amount <= 0) { clif_senddelitem(sd,id,1); return 0; }
	
	int def[2];

	CALLOC(fl, FLOORITEM, 1);

	fl->bl.m = sd->bl.m;
	fl->bl.x = sd->bl.x;
	fl->bl.y = sd->bl.y;
	//printf("%d\n",type);
	memcpy(&fl->data, &sd->status.inventory[id], sizeof(struct item));

	def[0]=0;

	//item check goes here(to see if there are previous items added
	//fl->data.dura=sd->status.inventory[id].dura;
	//if(!fl->data.custom) {
		if(fl->data.dura==itemdb_dura(fl->data.id)) {
			map_foreachincell(pc_addtocurrent,sd->bl.m,sd->bl.x,sd->bl.y,BL_ITEM,def,id,type,sd);
		}
	//} else {
	//	if(fl->data.dura==itemdb_durac(fl->data.custom)) {
	//		map_foreachincell(pc_addtocurrent,sd->bl.m,sd->bl.x,sd->bl.y,BL_ITEM,def,id,type,sd);
	//	}
	//}

	sd->status.inventory[id].amount--;

	if(type || !sd->status.inventory[id].amount) {
		Sql_EscapeString(sql_handle,escape,fl->data.real_name);
	
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `DropLogs` (`DrpChaId`, `DrpMapId`, `DrpX`, `DrpY`, `DrpItmId`, `DrpAmount`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, fl->data.id, sd->status.inventory[id].amount + 1)) {
			SqlStmt_ShowDebug(sql_handle);
		}
	
		memset(&sd->status.inventory[id], 0, sizeof(struct item));
		clif_senddelitem(sd, id, 1);
	} else {
		Sql_EscapeString(sql_handle,escape,fl->data.real_name);
	
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `DropLogs` (`DrpChaId`, `DrpMapId`, `DrpX`, `DrpY`, `DrpItmId`, `DrpAmount`) VALUES ('%u', '%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, fl->data.id, 1)) {
			SqlStmt_ShowDebug(sql_handle);
		}
	
		fl->data.amount = 1;
		clif_sendadditem(sd, id);
	}

	map_foreachincell(pc_npc_drop,sd->bl.m,sd->bl.x,sd->bl.y,BL_NPC,fl,sd);
	
	if(!def[0]) {
		//map_sweepadd(fl);
		//fl->gone_tick=0;

		map_additem(&fl->bl);
		if(fl->timer) timer_remove(fl->timer);
		if(map[fl->bl.m].sweeptime) fl->timer=timer_insert(map[fl->bl.m].sweeptime,map[fl->bl.m].sweeptime,pc_item_timer,fl->bl.id,0);

		sl_doscript_blargs("onDrop", NULL, 2, &sd->bl, &fl->bl);
		map_foreachinarea(clif_object_look_sub2,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,LOOK_SEND,&fl->bl);
	} else {
		FREE(fl);
	}
	
	return 0;
}

int pc_changeitem(USER *sd, int id1, int id2) {
	struct item tmp;
	if(id1>=sd->status.maxinv) return 0;
	if(id2>=sd->status.maxinv) return 0;
	memcpy(&tmp, &sd->status.inventory[id2], sizeof(struct item));
	memcpy(&sd->status.inventory[id2], &sd->status.inventory[id1], sizeof(struct item));
	memcpy(&sd->status.inventory[id1], &tmp, sizeof(struct item));
	if (sd->status.inventory[id1].id) {
		if (!sd->status.inventory[id2].id)
			clif_senddelitem(sd, id2, 0);
		clif_sendadditem(sd, id1);
	}
	if (sd->status.inventory[id2].id) {
		if (!sd->status.inventory[id1].id)
			clif_senddelitem(sd, id1, 0);
		clif_sendadditem(sd, id2);
	}
	return 0;
}

int pc_useitem(USER *sd, int id) {
	char buf[255];
	char escape[255];
	
	if (!sd->status.inventory[id].id)
		return 0;

	if(sd->status.inventory[id].owner_id){
		if(sd->status.inventory[id].owner_id!=sd->status.id) {
		clif_sendminitext(sd,"You cannot use this, it does not belong to you!",0);
		return 0;

		}
	}
	if(itemdb_class(sd->status.inventory[id].id)!=0) {
		// If GM
		if (sd->status.class==50){
			// Give no errors.
		// If Item's Class requirement is less than 6
		} else if (itemdb_class(sd->status.inventory[id].id)<6) {
			// If subpath base class is same as not item's required base path
			if(classdb_path(sd->status.class)!=itemdb_class(sd->status.inventory[id].id)) {
				clif_sendminitext(sd,map_msg[MAP_ERRITMPATH].message,map_msg[MAP_ERRITMPATH].len);
				return 0;
			}
		// If Item's Class requirement is over 5 (subpath restricted)
		} else {
		// If player class is not class required by item
			if(sd->status.class!=itemdb_class(sd->status.inventory[id].id)) {
				clif_sendminitext(sd,map_msg[MAP_ERRITMPATH].message,map_msg[MAP_ERRITMPATH].len);
				return 0;
			}
		}
		if(sd->status.mark<itemdb_rank(sd->status.inventory[id].id)) {
			clif_sendminitext(sd,map_msg[MAP_ERRITMPATH].message,map_msg[MAP_ERRITMPATH].len);
			return 0;
		}
	}
	if (sd->status.state==PC_DIE && itemdb_type(sd->status.inventory[id].id)!=ITM_USESPC) {
		clif_sendminitext(sd, map_msg[MAP_ERRGHOST].message, map_msg[MAP_ERRGHOST].len);
		return 0;
	}
	
	if (itemdb_time(sd->status.inventory[id].id)) {
		if (!sd->status.inventory[id].timeleft) {
			sd->status.inventory[id].timeleft = time(NULL) + itemdb_time(sd->status.inventory[id].id);
		}
	}

	switch(itemdb_type(sd->status.inventory[id].id)) {
	case ITM_EAT:
		if (sd->status.state == 1 || sd->status.state == 3) {
			clif_sendminitext(sd, "You cannot do that now.", strlen("You cannot do that now."));
			return 0;
		}
		
		sd->invslot = id;
		Sql_EscapeString(sql_handle,escape,sd->status.inventory[id].real_name);
			
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `UseLogs` (`UseChaId`, `UseMapId`, `UseX`, `UseY`, `UseItmId`) VALUES ('%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, sd->status.inventory[id].id)) {
			SqlStmt_ShowDebug(sql_handle);
		}
	
		sl_doscript_simple(itemdb_yname(sd->status.inventory[id].id),"use",&sd->bl);
		pc_delitem(sd,id,1,2);
		break;
	case ITM_USE:
	case ITM_USESPC:
		if (sd->status.state == 1 || sd->status.state == 3) {
			clif_sendminitext(sd, "You cannot do that now.", strlen("You cannot do that now."));
			return 0;
		}
		
		sd->invslot = id;
		Sql_EscapeString(sql_handle,escape,sd->status.inventory[id].real_name);
			
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `UseLogs` (`UseChaId`, `UseMapId`, `UseX`, `UseY`, `UseItmId`) VALUES ('%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, sd->status.inventory[id].id)) {
			SqlStmt_ShowDebug(sql_handle);
		}
		
		sl_doscript_simple(itemdb_yname(sd->status.inventory[id].id),"use",&sd->bl);
		pc_delitem(sd,id,1,6);
		break;
	case ITM_SMOKE:
		if (sd->status.state == 1 || sd->status.state == 3) {
			clif_sendminitext(sd, "You cannot do that now.", strlen("You cannot do that now."));
			return 0;
		}
		
		sd->invslot = id;
		Sql_EscapeString(sql_handle,escape,sd->status.inventory[id].real_name);
			
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `UseLogs` (`UseChaId`, `UseMapId`, `UseX`, `UseY`, `UseItmId`) VALUES ('%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, sd->status.inventory[id].id)) {
			SqlStmt_ShowDebug(sql_handle);
		}
		
		sd->invslot = id;
		sl_doscript_simple(itemdb_yname(sd->status.inventory[id].id),"use",&sd->bl);
		
		if(sd->status.inventory[id].dura==0) {
			pc_delitem(sd,id,1,3);
		} else {
			clif_sendadditem(sd,id);
		}
		break;
	case ITM_HELM:
	case ITM_WEAP:
	case ITM_ARMOR:
	case ITM_SHIELD:
	case ITM_HAND:
	case ITM_COAT:
	case ITM_BOOTS:
	case ITM_LEFT:
	case ITM_RIGHT:
	case ITM_SUBLEFT:
	case ITM_SUBRIGHT:
	case ITM_FACEACC:
	case ITM_CROWN:
	case ITM_MANTLE:
	case ITM_NECKLACE:
			pc_equipitem(sd, id);
		break;
	case ITM_ETC:
		sd->invslot = id;
		Sql_EscapeString(sql_handle,escape,sd->status.inventory[id].real_name);
			
		if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `UseLogs` (`UseChaId`, `UseMapId`, `UseX`, `UseY`, `UseItmId`) VALUES ('%u', '%u', '%u', '%u', '%u')",
				sd->status.id, sd->bl.m, sd->bl.x, sd->bl.y, sd->status.inventory[id].id)) {
			SqlStmt_ShowDebug(sql_handle);
		}
		
		sl_doscript_simple(itemdb_yname(sd->status.inventory[id].id),"use",&sd->bl);
		break;
	default:
		break;
	}
	
	return 0;
}

/*==========================================
 * script Read&Set PC Reg
 *------------------------------------------
 */
int pc_readreg(USER *sd,int reg) {
	int i;

	nullpo_ret(0, sd);

	for(i=0;i<sd->reg_num;i++)
		if(sd->reg[i].index==reg)
			return sd->reg[i].data;

	return 0;
}

int pc_setreg(USER *sd,int reg,int val) {
	int i;

	nullpo_ret(0, sd);

	for (i = 0; i < sd->reg_num; i++) {
		if (sd->reg[i].index == reg){
			sd->reg[i].data = val;
			return 0;
		}
	}
	sd->reg_num++;
	REALLOC(sd->reg, struct script_reg, sd->reg_num);
	memset(sd->reg + (sd->reg_num - 1), 0, sizeof(struct script_reg));
	sd->reg[i].index = reg;
	sd->reg[i].data = val;

	return 0;
}

char *pc_readregstr(USER *sd,int reg) {
	int i;

	nullpo_ret(0, sd);

	for(i=0;i<sd->regstr_num;i++)
		if(sd->regstr[i].index==reg)
			return sd->regstr[i].data;

	return NULL;
}

int pc_setregstr(USER *sd,int reg,char *str) {
	int i;

	nullpo_ret(0, sd);

	if(strlen(str)+1 >= sizeof(sd->regstr[0].data)){
		printf("pc_setregstr: string too long !\n");
		return 0;
	}

	for(i=0;i<sd->regstr_num;i++)
		if(sd->regstr[i].index==reg){
			strcpy(sd->regstr[i].data,str);
			return 0;
		}

	sd->regstr_num++;
	REALLOC(sd->regstr, struct script_regstr, sd->regstr_num);
	memset(sd->regstr + (sd->regstr_num - 1), 0, sizeof(struct script_regstr));
	sd->regstr[i].index=reg;
	strcpy(sd->regstr[i].data,str);

	return 0;
}

int pc_readglobalreg(USER *sd,char *reg) {
	int i, exist;
	
	exist = -1;
	nullpo_ret(0, sd);

	for(i=0;i<MAX_GLOBALREG;i++) {
		if(strcmpi(sd->status.global_reg[i].str,reg) == 0) {
			exist = i;
			break;
		}
	}

	if(exist != -1){
		return sd->status.global_reg[exist].val;
	} else {
		return 0;
	}

	return 0;
}

/*int map_readglobalreg(int m,char* reg) {
	int i;

	if(!map_isloaded(m)) return 0;

	for(i=0;i<map[m].registry_num;i++) {
		if(strcmp(map[m].registry[i].str,reg)==0)
			return map[m].registry[i].val;
	}
	return 0;
}*/

int pc_setglobalreg(USER *sd,char *reg,unsigned long val) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, sd);
	nullpo_ret(0,reg);
	//if registry exists, get number
	for(i=0;i<MAX_GLOBALREG;i++) {
		if(strcmpi(sd->status.global_reg[i].str,reg) == 0) {
			exist = i;
			break;
		}
	}
	//if registry exists, set value
	if(exist != -1) {
		if(val == 0) {
			strcpy(sd->status.global_reg[exist].str,""); //empty registry
			sd->status.global_reg[exist].val = val;
			return 0;
		} else {
			sd->status.global_reg[exist].val = val;
			return 0;
		}
	} else {
		for(i=0;i<MAX_GLOBALREG;i++) {
			if(strcmpi(sd->status.global_reg[i].str,"") == 0) {
				strcpy(sd->status.global_reg[i].str,reg);
				sd->status.global_reg[i].val = val;
				return 0;
			}
		}
	}
	/*if(val==0){
		for(i=0;i<sd->status.global_reg_num;i++){
			if(strcmp(sd->status.global_reg[i].str,reg)==0){
				sd->status.global_reg[i]=sd->status.global_reg[sd->status.global_reg_num-1];
				memset(&sd->status.global_reg[sd->status.global_reg_num-1], 0, sizeof(struct global_reg));
				sd->status.global_reg_num--;
				break;
			}
		}
		return 0;
	}
	for(i=0;i<sd->status.global_reg_num;i++){
		if(strcmp(sd->status.global_reg[i].str,reg)==0){
			sd->status.global_reg[i].val=val;
			return 0;
		}
	}
	if(sd->status.global_reg_num<MAX_GLOBALREG-1){
		strcpy(sd->status.global_reg[i].str,reg);
		sd->status.global_reg[i].val=val;
		sd->status.global_reg_num++;
		return 0;
	}*/
	printf("pc_setglobalreg : couldn't set %s\n", reg);

	return 1;
}

int pc_readparam(USER *sd,int type) {
	int val=0;

	nullpo_ret(0, sd);

	switch (type) {
	case SP_HP:
		val=sd->status.hp;
		break;
	case SP_MP:
		val=sd->status.mp;
		break;
	case SP_MHP:
		val=sd->max_hp;
		break;
	case SP_MMP:
		val=sd->max_mp;
		break;
	}
	return val;
}

int pc_setparam(USER *sd,int type,int val) {
	nullpo_ret(0, sd);
	switch (type) {
	case SP_HP:
		sd->status.hp=val;
		break;
	case SP_MP:
		sd->status.mp=val;
		break;
	case SP_MHP:
		sd->max_hp=val;
		break;
	case SP_MMP:
		sd->max_mp=val;
		break;
	}
	clif_sendupdatestatus(sd);
	return 0;
}

int pc_die(USER *sd) {
	sl_doscript_blargs("onDeathPlayer", NULL, 1, &sd->bl);
	return 0;
}

int pc_diescript(USER *sd) {
	struct block_list *bl = map_id2sd(sd->attacker);
	USER* tsd = NULL;
	char slain[255];
	int i, id;
	int exist = -1;
	char msg[255];
	
	nullpo_ret(0, sd);
	
	sd->deathflag = 0;
	
	if (bl) {
		for (i = 0; i < 20; i++) {
			if (sd->pvp[i][0] == bl->id) {
				exist = i;
				break;
			}
		}
	
		if(bl->type == BL_PC) {
			tsd = (USER*)bl;
		}
	}
	
	if(tsd) {
		if (tsd->status.pk == 0 && sd->status.pk == 0  && exist != -1) {
			tsd->status.pk = 1;
			//tsd->status.killspk = 1;
			tsd->status.pkduration = 600000;
			clif_sendchararea(tsd);
			sprintf(msg, "You have killed %s unwarranted and now have PK status.", sd->status.name);
			clif_sendminitext(tsd, msg, strlen(msg));
		} else if (tsd->status.pk > 0 && sd->status.pk == 0 && exist != -1) {
			//tsd->status.killspk += 1;
			tsd->status.pkduration += 600000;
			sprintf(msg, "You have killed %s unwarranted and extended your PK status.", sd->status.name);
			clif_sendminitext(tsd, msg, strlen(msg));
		} else if (map[tsd->bl.m].pvp == 1) {
			//tsd->killspvp += 1;
		}
		
		if (sd->status.state!=1) {
			sprintf(slain,"%s %s has been vanquished by %s %s",classdb_name(sd->status.class,sd->status.mark),sd->status.name,classdb_name(tsd->status.class,tsd->status.mark),tsd->status.name);
			clif_broadcast(slain,sd->bl.m);
		}
		
		/*if (tsd->status.killspk >= 4) {
			pc_warp(tsd, map, x, y);
		}*/
	}
	
	if (bl) sd->status.killedby = bl->id;
	sd->status.state = 1;
	sd->status.hp = 0;
	
	for(i=0;i<MAX_MAGIC_TIMERS;i++) {
		id = sd->status.dura_aether[i].id;
		if(id > 0) {
			sd->status.dura_aether[i].duration = 0;
			clif_send_duration(sd, sd->status.dura_aether[i].id, 0, map_id2sd(sd->status.dura_aether[i].caster_id));
			sd->status.dura_aether[i].caster_id = 0;
			map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,sd->status.dura_aether[i].animation,&sd->bl,-1);
			sd->status.dura_aether[i].animation = 0;
			
			if(sd->status.dura_aether[i].aether == 0) {
				sd->status.dura_aether[i].id=0;
			}
			
			if(sd->status.dura_aether[i].caster_id != sd->bl.id) {
				bl = map_id2bl(sd->status.dura_aether[i].caster_id);
			}
			
			if(bl != NULL) {
				sl_doscript_blargs(magicdb_yname(id), "uncast", 2, &sd->bl, bl);
			} else {
				sl_doscript_blargs(magicdb_yname(id), "uncast", 1, &sd->bl);
			}
		}
	}
	
	for (i = 0; i < MAX_SPELLS; i++) {
		if (sd->status.skill[i] > 0) {
			if (bl != NULL) {
				sl_doscript_blargs(magicdb_yname(sd->status.skill[i]), "passive_after_death", 2, &sd->bl, bl);
			} else {
				sl_doscript_blargs(magicdb_yname(sd->status.skill[i]), "passive_after_death", 1, &sd->bl);
			}
		}
	}
	
	for (i = 0; i < MAX_MAGIC_TIMERS; i++) {
		if (sd->status.dura_aether[i].id > 0) {
			if (bl != NULL) {
				sl_doscript_blargs(magicdb_yname(sd->status.dura_aether[i].id), "after_death_while_cast", 2, &sd->bl, bl);
			} else {
				sl_doscript_blargs(magicdb_yname(sd->status.dura_aether[i].id), "after_death_while_cast", 1, &sd->bl);
			}
		}
	}
	
	//threat stuff
	unsigned int x;
	MOB* tmob = NULL;
	
	if (MOB_SPAWN_START != MOB_SPAWN_MAX) {
		for (x = MOB_SPAWN_START; x < MOB_SPAWN_MAX; x++) { //only process up to that.
			tmob = map_id2mob(x);
			if (!tmob) continue;
			
			for (i = 0; i < MAX_THREATCOUNT; i++) {
				if (tmob->threat[i].user == sd->bl.id) {
					tmob->threat[i].user = 0;
					tmob->threat[i].amount = 0;
				}
			}
		}
	}

	if (MOB_ONETIME_START != MOB_ONETIME_MAX) {
		for (x = MOB_ONETIME_START; x < MOB_ONETIME_MAX; x++) {
			tmob = map_id2mob(x);
			if (!tmob) continue;
			
			for (i = 0; i < MAX_THREATCOUNT; i++) {
				if (tmob->threat[i].user == sd->bl.id) {
					tmob->threat[i].user = 0;
					tmob->threat[i].amount = 0;
				}
			}
		}
	}

	sd->enchanted = 1.0f;
	sd->flank = 0;
	sd->backstab = 0;
	sd->dmgshield = 0;
	
	pc_calcstat(sd);
	map_foreachinarea(clif_updatestate, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, sd);
	//clif_sendchararea(sd);
	return 0;
}

int pc_res(USER *sd) {

	sd->status.state = PC_ALIVE;
	sd->status.hp = 100;
	clif_sendstatus(sd,SFLAG_HPMP);
	//clif_sendupdatestatus(sd);
	pc_warp(sd,sd->bl.m,sd->bl.x,sd->bl.y);
	//map_foreachinarea(clif_updatestate,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,sd);
	//clif_sendchararea(sd);
	return 0;
}



int pc_heal(USER *sd, int hp, int mp, int caster) {
	//if(sd->optFlags&optFlag_stealth)return 0;
	struct block_list *bl=NULL;
	USER *tsd= NULL;
	bl=map_id2bl(caster);
	tsd=map_id2sd(caster);

	if(sd->status.state==1) return 0;

	if (mp>=sd->max_mp-sd->status.mp && mp>0) {
		sd->status.mp = sd->max_mp;
	} else {
		sd->status.mp += mp;
	}

	if (hp>=sd->max_hp-sd->status.hp && hp>0) {
		sd->status.hp = sd->max_hp;
	} else {
		sd->status.hp += hp;
	}


	if(hp< 0) {
		tsd->damage = abs(hp);

		int x;

		for(x=0; x< MAX_MAGIC_TIMERS; x++) {
			if(sd->status.dura_aether[x].id > 0) {
				if(sd->status.dura_aether[x].duration > 0) {
            		sl_doscript_blargs(magicdb_yname(sd->status.dura_aether[x].id),"on_takedamage",2,&sd->bl,bl);
				}
			}
		}
		for(x=0;x<14;x++) {
			if(sd->status.equip[x].id > 0) {
				sl_doscript_blargs(itemdb_yname(sd->status.equip[x].id),"on_takedamage",2,&sd->bl,bl);
			}
		}
	}

	if (sd->status.hp <= 0 && sd->status.state!=PC_DIE) {
		pc_die(sd);
	} else {
		clif_sendupdatestatus(sd);
	}
	return 0;
}

int pc_saveacctregistry(USER *sd, int i) {
	struct global_reg* p = NULL;
	unsigned int save_id;
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	unsigned int acctid;
	unsigned int acctregid;
	
	nullpo_ret(0, sd);
	p = &sd->status.acctreg[i];
	
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `ChaActId` FROM `Character` WHERE `ChaId` = '%u'", sd->status.id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &acctid, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	if (SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ArgId` FROM `AccountRegistry` WHERE `ArgIdentifier` = '%s' AND `ArgActId` = '%u'", p->str, acctid)
		|| SQL_ERROR == SqlStmt_Execute(stmt)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &acctregid, 0, NULL, NULL)) {
			SqlStmt_ShowDebug(stmt);
			SqlStmt_Free(stmt);
			return 0;
		}
	}
	
	if (SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		save_id = acctregid;
	}
	
	SqlStmt_Free(stmt);

	if(save_id) {
		if(p->val == 0) {
			if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `AccountRegistry` WHERE `ArgIdentifier` = '%s' AND `ArgActId` = '%u'", p->str, acctid)) {
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		} else {
			if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `AccountRegistry` SET `ArgIdentifier` = '%s', `ArgValue` = '%d' WHERE `ArgId` = '%d' AND `ArgActId` = '%u'", p->str, p->val, save_id, acctid)) {
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		}
	} else {
		if(p->val > 0) {
			if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `AccountRegistry` (`ArgActId`, `ArgIdentifier`, `ArgValue`) VALUES ('%u', '%s', '%d')",
				acctid, p->str, p->val))
			{
				Sql_ShowDebug(sql_handle);
				return 0;
			}
		}
	}
	
	return 0;
}

int pc_setacctreg(USER* sd, char *reg, int val) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, sd);
	nullpo_ret(0, reg);

	//if registry exists, get number
	for(i = 0; i < MAX_GLOBALREG; i++) {
		if(strcmpi(sd->status.acctreg[i].str, reg) == 0) {
			exist = i;
			break;
		}
	}
	//if registry exists, set value
	if(exist != -1) {
		if(val == 0) {
			sd->status.acctreg[exist].val = val;
			pc_saveacctregistry(sd, exist);
			strcpy(sd->status.acctreg[exist].str, ""); //empty registry
			return 0;
		} else {
			sd->status.acctreg[exist].val = val;
			pc_saveacctregistry(sd, exist);
			return 0;
		}
	} else {
		for(i = 0; i < MAX_GLOBALREG; i++) {
			if(strcmpi(sd->status.acctreg[i].str, "") == 0) {
				strcpy(sd->status.acctreg[i].str, reg);
				sd->status.acctreg[i].val = val;
				pc_saveacctregistry(sd, i);
				return 0;
			}
		}
	}
	
	return 0;
}
//reads acct registry
int pc_readacctreg(USER* sd, char *reg) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, sd);
	nullpo_ret(0, reg);
	
	for(i = 0; i < MAX_GLOBALREG; i++) {
		if(strcmpi(sd->status.acctreg[i].str, reg) == 0) {
			exist = i;
			break;
		}
	}

	if(exist != -1) {
		return sd->status.acctreg[exist].val;
	} else {
		return 0;
	}
	
	return 0;
}

int pc_setnpcintreg(USER* sd, char *reg, int val) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, sd);
	nullpo_ret(0, reg);

	//if registry exists, get number
	for(i = 0; i < MAX_GLOBALREG; i++) {
		if(strcmpi(sd->status.npcintreg[i].str, reg) == 0) {
			exist = i;
			break;
		}
	}
	//if registry exists, set value
	if(exist != -1) {
		if(val == 0) {
			sd->status.npcintreg[exist].val = val;
			strcpy(sd->status.npcintreg[exist].str, ""); //empty registry
			return 0;
		} else {
			sd->status.npcintreg[exist].val = val;
			return 0;
		}
	} else {
		for(i = 0; i < MAX_GLOBALREG; i++) {
			if(strcmpi(sd->status.npcintreg[i].str, "") == 0) {
				strcpy(sd->status.npcintreg[i].str, reg);
				sd->status.npcintreg[i].val = val;
				return 0;
			}
		}
	}
	
	return 0;
}
//reads acct registry
int pc_readnpcintreg(USER* sd, char *reg) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, sd);
	nullpo_ret(0, reg);
	
	for(i = 0; i < MAX_GLOBALREG; i++) {
		if(strcmpi(sd->status.npcintreg[i].str, reg) == 0) {
			exist = i;
			break;
		}
	}

	if(exist != -1) {
		return sd->status.npcintreg[exist].val;
	} else {
		return 0;
	}
	
	return 0;
}

int pc_setquestreg(USER* sd, char *reg, int val) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, sd);
	nullpo_ret(0, reg);

	//if registry exists, get number
	for(i = 0; i < MAX_GLOBALREG; i++) {
		if(strcmpi(sd->status.questreg[i].str, reg) == 0) {
			exist = i;
			break;
		}
	}
	//if registry exists, set value
	if(exist != -1) {
		if(val == 0) {
			sd->status.questreg[exist].val = val;
			strcpy(sd->status.questreg[exist].str, ""); //empty registry
			return 0;
		} else {
			sd->status.questreg[exist].val = val;
			return 0;
		}
	} else {
		for(i = 0; i < MAX_GLOBALREG; i++) {
			if(strcmpi(sd->status.questreg[i].str, "") == 0) {
				strcpy(sd->status.questreg[i].str, reg);
				sd->status.questreg[i].val = val;
				return 0;
			}
		}
	}
	
	return 0;
}
//reads acct registry
int pc_readquestreg(USER* sd, char *reg) {
	int i, exist;

	exist = -1;
	nullpo_ret(0, sd);
	nullpo_ret(0, reg);
	
	for(i = 0; i < MAX_GLOBALREG; i++) {
		if(strcmpi(sd->status.questreg[i].str, reg) == 0) {
			exist = i;
			break;
		}
	}

	if(exist != -1) {
		return sd->status.questreg[exist].val;
	} else {
		return 0;
	}
	
	return 0;
}

int pc_scripttimer(int id, int none) {
	USER *sd = NULL;
	USER* tsd = NULL;
	int x = 0;
	
	nullpo_ret(1, sd = map_id2sd((unsigned int) id));
	//if (session[sd->fd]
	if (sd->selfbar) {
		clif_send_selfbar(sd);
	}
	
	if (sd->groupbars && sd->group_count > 1) {
		for (x = 0; x < sd->group_count; x++) {
			tsd = map_id2sd(groups[sd->groupid][x]);
			
			if (tsd->bl.m == sd->bl.m) {
				clif_send_groupbars(sd, tsd);
			}
		}
	}
	
	if (sd->mobbars) {
		map_foreachinarea(clif_send_mobbars, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_MOB, sd);
	}
	
	if (sd->status.hp <= 0 && sd->deathflag) {
		pc_diescript(sd);
		return 0;
	}
	
	if (sd->dmgshield > 0) {
		clif_send_duration(sd, 0, (unsigned int)sd->dmgshield + 1, NULL);
	}
	
	sd->deathflag = 0;
	
	sd->scripttick++;
	sl_doscript_blargs("pc_timer", "tick", 1, &sd->bl);
	
	if (sd->status.settingFlags&FLAG_ADVICE) {
		sl_doscript_blargs("pc_timer", "advice", 1, &sd->bl);
	}
	
	//if (sd->scriptinterval >= 1200) {
	//	sd->scriptinterval = 0;
	//}
	
	return 0;
}

int pc_atkspeed(int id, int none) {
	USER *sd = NULL;
	
	nullpo_ret(1, sd = map_id2sd((unsigned int)id));
	sd->attacked = 0;
	return 1;
}

int pc_disptimertick(int id, int none) {
	USER* sd = map_id2sd((unsigned int)id);
	
	nullpo_ret(1, sd);
	
	if ((long)(sd->disptimertick - 1) < 0) {
		sd->disptimertick = 0;
	} else {
		sd->disptimertick--;
	}
	
	if (sd->disptimertick <= 0) {
		sl_doscript_blargs("pc_timer", "display_timer", 1, &sd->bl);
		timer_remove(sd->disptimer);
		sd->disptimertype = 0;
		sd->disptimer = 0;
		return 1;
	}
	
	return 0;
}