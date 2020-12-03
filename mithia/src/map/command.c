
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "mmo.h"
#include "map.h"
#include "pc.h"
#include "clif.h"
#include "command.h"
#include "itemdb.h"
#include "board_db.h"
#include "npc.h"
#include "magic.h"
#include "sl.h"
#include <lauxlib.h>
#include "mob.h";
//for debug command
#include "socket.h"
#include "crypt.h"
#include "db_mysql.h"
#include "malloc.h"
#include "rndm.h"
//#include "../metan/metan.h"

int spellgfx;
int musicfx;
int soundfx;
int downtimer;

char command_code='/';
int command_url(USER*, char*,lua_State*);

int command_reloadmob(USER*,char*,lua_State*);
int command_reloadspawn(USER*,char*,lua_State*);
int command_debug(USER*,char*,lua_State*);
int command_item(USER*,char*,lua_State*);
int command_res(USER*,char*,lua_State*);
int command_hair(USER*,char*,lua_State*);
int command_checkdupes(USER*,char*,lua_State*);
int command_checkwpe(USER*,char*,lua_State*);
int command_kill(USER*,char*,lua_State*);
int command_killall(USER*,char*,lua_State*);
int command_deletespell(USER*,char*,lua_State*);
int command_xprate(USER*,char*,lua_State*);
int command_heal(USER*,char*,lua_State*);
int command_level(USER*,char*,lua_State*);
int command_randomspawn(USER*,char*,lua_State*);
int command_drate(USER*,char*,lua_State*);
int command_spell(USER*,char*,lua_State*);
int command_val(USER*,char*,lua_State*);
int command_mon(USER*,char*,lua_State*);
int command_disguise(USER*,char*,lua_State*);
int command_warp(USER*,char*,lua_State*);
int command_makegm(USER*,char*,lua_State*);
int command_givespell(USER*,char*,lua_State*);
int command_side(USER*,char*,lua_State*);
int command_state(USER*,char*,lua_State*);
int command_armorcolor(USER*,char*,lua_State*);
int command_makegm(USER*,char*,lua_State*);
int command_who(USER*,char*,lua_State*);
int command_legend(USER*,char*,lua_State*);
int command_luareload(USER *, char *, lua_State *);
int command_magicreload(USER *, char *,lua_State*);
int command_lua(USER *, char *,lua_State*);
int command_speed(USER*,char*,lua_State*);
int command_reloaditem(USER*,char*,lua_State*);
int command_pvp(USER*,char*,lua_State*);
int command_spellwork(USER*,char*,lua_State*);
int command_broadcast(USER*,char*,lua_State*);
int command_luasize(USER*,char*,lua_State*);
int command_luafix(USER*,char*,lua_State*);
int command_respawn(USER*,char*,lua_State*);
int command_kc(USER*,char*,lua_State*);
int command_ban(USER*,char*,lua_State*);
int command_unban(USER*,char*,lua_State*);
int command_blockcount(USER*,char*,lua_State*);
int command_stealth(USER*,char*,lua_State*);
int command_ghosts(USER*,char*,lua_State*);
int command_unphysical(USER*,char*,lua_State*);
int command_immortality(USER*,char*,lua_State*);
int command_silence(USER*,char*,lua_State*);
int command_shutdowncancel(USER*,char*,lua_State*);
int command_shutdown(USER*,char*,lua_State*);
int command_roll(USER*,char*,lua_State*);
int command_weap(USER*,char*,lua_State*);
int command_armor(USER*,char*,lua_State*);
int command_shield(USER*,char*,lua_State*);
int command_crown(USER*,char*,lua_State*);
int command_mantle(USER*,char*,lua_State*);
int command_necklace(USER*,char*,lua_State*);
int command_boots(USER*,char*,lua_State*);
int command_faceacc(USER*,char*,lua_State*);
int command_helm(USER*,char*,lua_State*);
int command_gfxtoggle(USER*,char*,lua_State*);
int command_weather(USER*,char*,lua_State*);
int command_light(USER*,char*,lua_State*);
int command_gm(USER*, char*,lua_State*);
int command_guide(USER*, char*, lua_State*);
int command_report(USER*,char*,lua_State*);
int command_cinv(USER*,char*,lua_State*);
int command_cfloor(USER*,char*,lua_State*);
int command_cspells(USER*,char*,lua_State*);
int command_job(USER*,char*,lua_State*);
int command_music(USER*,char*,lua_State*);
int command_musicn(USER*,char*,lua_State*);
int command_musicp(USER*,char*,lua_State*);
int command_musicq(USER*,char*,lua_State*);
int command_sound(USER*,char*,lua_State*);
int command_nsound(USER*,char*,lua_State*);
int command_psound(USER*,char*,lua_State*);
int command_soundq(USER*,char*,lua_State*);
int command_nspell(USER*,char*,lua_State*);
int command_pspell(USER*,char*,lua_State*);
int command_spellq(USER*,char*,lua_State*);
int command_reloadboard(USER*,char*,lua_State*);
int command_online(USER*,char*,lua_State*);
//int command_metan(USER*,char*);
int command_reload(USER*,char*,lua_State*);
int command_reloadnpc(USER*,char*,lua_State*);
//int command_reloadrecipe(USER*,char*);
int command_reloadmaps(USER*,char*,lua_State*);
int command_reloadclass(USER*,char*,lua_State*);
int command_reloadwarps(USER*,char*,lua_State*);
extern unsigned long Last_Eof;
struct {
	int (*func)(USER*,char*,lua_State*);
	char *name;
	int level;
} command[]= {
	{command_debug,"debug",99},
	{command_item,"item",50},
	{command_res,"res",99},
	{command_hair,"hair",99},
	{command_checkdupes,"checkdupes",99},
	{command_checkwpe,"checkwpe",99},
	{command_kill,"kill",99},
	{command_killall,"killall",99},
	{command_deletespell,"deletespell",99},
	{command_xprate,"xprate",99},
	{command_heal,"heal",99},
	{command_level,"level",99},
	{command_randomspawn,"randomspawn17",99},
	{command_drate,"droprate",99},
	{command_spell,"spell",99},
	{command_val,"val",99},
	{command_mon,"mon",99},
	{command_disguise,"disguise",99},
	{command_warp,"warp",10},
	{command_givespell,"givespell",50},
	{command_side,"side",99},
	{command_state,"state",20},
	{command_armorcolor,"armorc",99},
	{command_makegm,"makegm",99},
	{command_who,"who",99},
	{command_legend,"legend",99},
	{command_luareload,"reloadlua",99},
	{command_magicreload,"reloadmagic",99},
	{command_lua,"lua",0},
	{command_speed,"speed",10},
	{command_reloaditem,"reloaditem",99},
	{command_reloadmob,"reloadmob",99},
	{command_reloadspawn,"reloadspawn",99},
	{command_pvp,"pvp",20},
	{command_spellwork,"spellwork",99},
	{command_broadcast,"bc",50},
	{command_luasize,"luasize",99},
	{command_luafix,"luafix",99},
	{command_respawn,"respawn",99},
	{command_kc,"kc",99},
	{command_ban,"ban",99},
	{command_unban,"unban",99},
	{command_blockcount,"blockc",99},
	{command_stealth,"stealth",1},
	{command_ghosts,"ghosts",1},
	{command_unphysical,"unphysical",99},
	{command_immortality,"immortality",99},
	{command_silence,"silence",99},
	{command_shutdowncancel,"a",99},
	{command_shutdown,"shutdown",99},
	{command_roll,"roll",0},
	{command_weap,"weap",99},
	{command_shield,"shield",99},
	{command_armor,"armor",99},
	{command_boots,"boots",99},
	{command_mantle,"mantle",99},
	{command_necklace,"necklace",99},
	{command_faceacc,"faceacc",99},
	{command_crown,"crown",99},
	{command_helm,"helm",99},
	{command_gfxtoggle,"gfxtoggle",99},
	{command_weather,"weather",50},
	{command_light,"light",50},
	{command_gm,"gm",20},
	{command_guide, "guide", 0},
	{command_report,"report",0},
	{command_url,"url",99},
	{command_cinv,"cinv",50},
	{command_cfloor,"cfloor",50},
	{command_cspells,"cspells",50},
	{command_job,"job",20},
	{command_music,"music",50},
	{command_musicn,"musicn",99},
	{command_musicp,"musicp",99},
	{command_musicq,"musicq",99},
	{command_sound,"sound",50},
	{command_nsound,"nsound",99},
	{command_psound,"psound",99},
	{command_soundq,"soundq",99},
	{command_nspell,"nspell",99},
	{command_pspell,"pspell",99},
	{command_spellq,"spellq",99},
	{command_reloadboard,"reloadboard",99},
	{command_item,"i",50},
	{command_online,"online",1},
//	{command_metan,"metan",99},
	{command_reload,"reload",50},
	{command_reloadnpc,"reloadnpc",99},
//	{command_reloadrecipe,"reloadrecipe",99),
	{command_reloadmaps, "reloadmaps", 99},
	{command_reloadclass, "reloadclass", 99},
	{command_reloadwarps, "reloadwarps", 99},
	{NULL,NULL,NULL}
};

struct userlist_data userlist;
int command_report(USER* sd, char* line,lua_State* state) {
	USER* tsd=NULL;
	int x;
	char buf[65535];
	//if(sscanf(line,"%254s",message)<1)
		//return -1;

		sprintf(buf,"<REPORT>%s: %s",sd->status.name,line);
		for(x=1;x<fd_max;x++) {
		tsd=NULL;
		if(session[x] && (tsd=(USER*)session[x]->session_data) && !session[x]->eof && tsd->status.gm_level) {
			clif_sendmsg(tsd,12,buf,strlen(buf));
			}
		}

		return 0;
	}

int command_gm(USER* sd, char* line,lua_State* state) {
	USER* tsd=NULL;
	char buf[65535];
	int x;
	char escape[255];

	sprintf(buf,"<GM>%s: %s",sd->status.name,line);
	Sql_EscapeString(sql_handle,escape,line);
	
	if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `SayLogs` (`SayChaId`, `SayMessage`, `SayType`) VALUES ('%u', '%s', '%s')",
		sd->status.id, escape, "GM")) {
		SqlStmt_ShowDebug(sql_handle);
	}
	
	for(x=1;x<fd_max;x++) {
		tsd=NULL;
		if(session[x] && (tsd=(USER*)session[x]->session_data) && !session[x]->eof && tsd->status.gm_level) {
			clif_sendmsg(tsd,11,buf,strlen(buf));
		}
	}

	return 0;
}

int command_guide(USER* sd, char* line,lua_State* state) {
	USER* tsd=NULL;
	char buf[65535];
	int x;
	char escape[255];

	sprintf(buf,"<Guide>%s: %s",sd->status.name,line);
	Sql_EscapeString(sql_handle,escape,line);
	
	if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `SayLogs` (`SayChaId`, `SayMessage`, `SayType`) VALUES ('%u', '%s', '%s')",
		sd->status.id, escape, "Guide")) {
		SqlStmt_ShowDebug(sql_handle);
	}
	
	if ((sd->status.class > 5 || sd->status.level > 25) && sd->status.gm_level == 0
		&& pc_readglobalreg(sd, "guide") == 0) {
		return 0;
	}
	
	for(x=1;x<fd_max;x++) {
		tsd=NULL;
		if(session[x] && (tsd=(USER*)session[x]->session_data) && !session[x]->eof 
			&& ((tsd->status.class <= 5 && tsd->status.level <= 25) || tsd->status.gm_level > 0
			|| pc_readglobalreg(tsd, "guide") > 0)) {
			clif_sendmsg(tsd, 12, buf, strlen(buf));
		}
	}

	return 0;
}
	

int command_weather(USER *sd, char* line,lua_State* state) {
	int weather=5,x;
	USER* tmpsd;
	sscanf(line,"%u",&weather);
	
	map[sd->bl.m].weather=weather;
	for(x=1;x<fd_max;x++) {
		if(session[x] && (tmpsd=(USER*)session[x]->session_data) && !session[x]->eof) {
			if(tmpsd->bl.m==sd->bl.m) {
				clif_sendweather(tmpsd);
				//pc_warp(tmpsd,tmpsd->bl.m,tmpsd->bl.x,tmpsd->bl.y);
			}
		}
	}

	return 0;
}
int command_light(USER *sd, char* line,lua_State* state) {
int weather=232,x;
	USER* tmpsd;
	sscanf(line,"%u",&weather);

	map[sd->bl.m].light=weather;
	for(x=1;x<fd_max;x++) {
		if(session[x] && (tmpsd=(USER*)session[x]->session_data) && !session[x]->eof) {
			if(tmpsd->bl.m==sd->bl.m) {
				pc_warp(tmpsd,tmpsd->bl.m,tmpsd->bl.x,tmpsd->bl.y);
				//clif_sendmapinfo(sd);
			}
		}
	}

	return 0;
}
int is_command(USER *sd,const char *p, int len) {
	int i;
	char cmd_line[257];
	char *cmdp;
	if (*p!=command_code)
		return 0;
	p++;
	//copy command to buffer
	memcpy(cmd_line,p,len);
	cmd_line[len]=0x00;
	cmdp = cmd_line;
	//Search for end of command string
	while(*cmdp && !isspace(*cmdp))
		cmdp++;
	//set null for comparement
	*cmdp='\0';
	for(i=0;command[i].func;i++) {
		if (!strcmpi(cmd_line,command[i].name))
			break;
	}
	//wrong command, exit!
	if (command[i].func==NULL)
		return 0;

	if (sd->status.gm_level<command[i].level) return 0;

	//found command
	//skip our null terminate
	cmdp++;
	//let's go
	printf("%d\n", i);
	command[i].func(sd, cmdp, NULL);
	return 1;
}
int command_shutdowncancel(USER *sd, char* line,lua_State* state) {
	if(downtimer) {
		clif_broadcast("---------------------------------------------------",-1);
		clif_broadcast("Server shutdown cancelled.",-1);
		clif_broadcast("---------------------------------------------------",-1);
		timer_remove(downtimer);
		downtimer = 0;
	} else {
		clif_sendminitext(sd,"Server is not shutting down.",strlen("Server is not shutting down."));
	}

	return 0;
}

int command_shutdown(USER *sd, char* line,lua_State* state) {
	int t_time=0;
	char timelen[5];
	int d;
	char msg[255];
	if(!downtimer) {
		if (sscanf(line,"%d %s",&t_time, timelen) < 1) {
			return -1;
		} else if(sscanf(line,"%d %s",&t_time,timelen) == 2) {
			if(strcmpi(timelen,"s") == 0 || strcmpi(timelen,"sec") == 0) {
				t_time *= 1000;
			} else if(strcmpi(timelen,"m") == 0 || strcmpi(timelen,"min") == 0) {
				t_time *= 60000;
			} else if(strcmpi(timelen,"h") == 0 || strcmpi(timelen,"hr") == 0) {
				t_time *= 3600000;
			}
		}

		if(t_time>=60000) {
			sprintf(msg,"Server resetting in %d minutes.",t_time/60000);
			clif_broadcast("---------------------------------------------------",-1);
			clif_broadcast(msg,-1);
			clif_broadcast("---------------------------------------------------",-1);
			d=t_time/60000;
			t_time=d*60000;

		} else {
			sprintf(msg, "Server resetting in %d seconds.",t_time/1000);
			clif_broadcast("---------------------------------------------------",-1);
			clif_broadcast(msg,-1);
			clif_broadcast("---------------------------------------------------",-1);
			d=t_time/1000;
			t_time=d*1000;

		}
	//server_shutdown=1;
		downtimer = timer_insert(250,250,map_reset_timer,t_time,250);
	} else {
		clif_sendminitext(sd,"Server is already shutting down.",strlen("Server is already shutting down."));
	}

	return 0;
}
int command_luareload(USER *sd, char *line, lua_State *state) {
	int errors = sl_reload(state);
	
	nullpo_ret(errors, sd);
	clif_sendminitext(sd,"LUA Scripts reloaded!",strlen("LUA Scripts reloaded!"));
	return errors;
}
int command_magicreload(USER *sd, char *line,lua_State* state) {
	magicdb_read();
	nullpo_ret(0, sd);
	clif_sendminitext(sd,"Magic DB reloaded!",strlen("Magic DB reloaded!"));
	return 0;
}
int command_lua(USER *sd, char *line,lua_State* state) {
	sd->luaexec = 0;
	sl_doscript_blargs("canRunLuaTalk", NULL, 1, &sd->bl);
	
	if (sd->luaexec) {
		sl_exec(sd, line);
	}
	
	return 0;
}

int command_luasize(USER *sd, char *line,lua_State* state) {
	sl_luasize(sd);
	return 0;
}
int command_luafix(USER* sd, char* line,lua_State* state) {
	sl_fixmem();
	return 0;
}
int command_blockcount(USER* sd,char* line,lua_State* state) {

	return 0;
}

int command_stealth(USER* sd, char* line,lua_State* state) {
	if(sd->optFlags & optFlag_stealth) {
		sd->optFlags ^= optFlag_stealth;
		clif_parserefresh(sd);
    	clif_sendminitext(sd, "Stealth :OFF", 0);
	} else {
		clif_lookgone(&sd->bl);
		sd->optFlags ^= optFlag_stealth;
		clif_parserefresh(sd);
    	clif_sendminitext(sd, "Stealth :ON", 0);
	}
}

int command_ghosts(USER* sd, char* line,lua_State* state) {
	sd->optFlags ^= optFlag_ghosts;
	clif_parserefresh(sd);
    
	if(sd->optFlags & optFlag_ghosts)
		clif_sendminitext(sd, "Ghosts :ON", 0);
	else
    	clif_sendminitext(sd, "Ghosts :OFF", 0);
	
	return 0;
}

int command_unphysical(USER* sd, char* line,lua_State* state) {

	 sd->uFlags ^= uFlag_unphysical;

	if(sd->uFlags & uFlag_unphysical) {
		clif_sendminitext(sd,"Unphysical :ON",0);
	} else {
		clif_sendminitext(sd,"Unphysical :OFF",0);
	}
}

int command_immortality(USER* sd, char* line,lua_State* state) {

	 sd->uFlags ^= uFlag_immortal;

	if(sd->uFlags & uFlag_immortal) {
		clif_sendminitext(sd,"Immortality :ON",0);
	} else {
		clif_sendminitext(sd,"Immortality :OFF",0);
	}

}
int command_silence(USER* sd, char* line,lua_State* state) {


	char name[32];
	if (sscanf(line,"%s",&name)<1)
		return -1;

	USER* tsd=map_name2sd(name);

	if(tsd)
	{
	  tsd->uFlags ^= uFlag_silenced;

	  if(tsd->uFlags & uFlag_silenced) {
		clif_sendminitext(sd,"Silenced.",0);
		clif_sendminitext(tsd,"You have been silenced.",0);
	  } else {
		clif_sendminitext(sd,"Unsilenced.",0);
		clif_sendminitext(tsd,"Silence lifted.",0);
	  }
	}
	else
		clif_sendminitext(sd,"User not on.",0);

}

int command_ban(USER* sd, char* line,lua_State* state) {
	unsigned int ipaddr=0;
	char name[32];
	if (sscanf(line,"%s",&name)<1)
		return -1;

	USER* tsd=map_name2sd(name);

	if(tsd) {
		printf("Banning %s\n",name);
		ipaddr=session[tsd->fd]->client_addr.sin_addr.s_addr;

		if (SQL_ERROR == Sql_Query(sql_handle, "UPDATE `Character` SET ChaBanned = '1' WHERE `ChaName` = '%s'", name)) {
			SqlStmt_ShowDebug(sql_handle);
		}
		//sql_request("INSERT INTO `banlist` (`ipaddy`) VALUES('%u')",ipaddr);
		//sql_free_row();
		session[tsd->fd]->eof=1;
	}
	return 0;
}

int command_unban(USER* sd, char* line,lua_State* state) {
	char name[32];
	
	if (sscanf(line, "%s", &name) < 1)
		return -1;

	printf("Unbanning %s\n", name);

	if (SQL_ERROR == Sql_Query(sql_handle, "UPDATE `Character` SET ChaBanned = '0' WHERE `ChaName` = '%s'", name)) {
		SqlStmt_ShowDebug(sql_handle);
	}
	
	return 0;
}

int command_gfxtoggle(USER *sd, char *line,lua_State* state) {

	sd->gfx.toggle^=1;
	return 0;
}
int command_disguise(USER *sd,char *line,lua_State* state) {
	int d;
	int e=0;
	int os;
	if (sscanf(line,"%d %d",&d,&e)<1)
		return -1;
	os=sd->status.state;
	sd->status.state=0;
	map_foreachinarea(clif_updatestate,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,sd);
	sd->status.state=os;
	sd->disguise=d;
	sd->disguise_color=e;
	map_foreachinarea(clif_updatestate,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,sd);


	return 0;
}
int command_broadcast(USER *sd, char* line,lua_State* state) {
	char buf[65535];
	char escape[255];
	
	sprintf(buf,"%s",line);
	Sql_EscapeString(sql_handle,escape,line);
		
	if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `SayLogs` (`SayChaId`, `SayMessage`, `SayType`) VALUES ('%u', '%s', '%s')",
		sd->status.id, escape, "GM Broadcast")) {
		SqlStmt_ShowDebug(sql_handle);
	}
	
	clif_broadcast(buf,-1);
	return 0;
}
int command_speed(USER *sd, char *line,lua_State* state) {
	int d;
	if(sscanf(line,"%d",&d)<1)
		return -1;

	sd->speed=d;
	clif_sendchararea(sd);
	clif_getchararea(sd);
}
int command_warp(USER *sd,char *line,lua_State* state) {
	int m,x,y;
	if (sscanf(line,"%d %d %d",&m,&x,&y)<1)
		return -1;

	pc_warp(sd,m,x,y);
	return 0;
}
int command_pvp(USER *sd,char* line,lua_State* state) {
	int pvp;
	char msg[64];
	
	if (sscanf(line, "%d", &pvp) < 1) {
		return -1;
	}
	
	sprintf(msg,"PvP set to: %d", pvp);
	clif_sendminitext(sd, msg, strlen(msg));
	map[sd->bl.m].pvp = pvp;
	return 0;
}
int command_handle_mob(struct block_list *bl, va_list ap) {
	MOB* mob=(MOB*)bl;
	//nullpo_ret(0,mob);
	if(!mob) return 0;

	//mob->spawncheck=mobdb_spawntime(mob->id);
	if(mob->state==MOB_DEAD && (!mob->onetime)) {
		mob_respawn(mob);
	}
	return 0;
}
int command_respawn(USER *sd, char* line,lua_State* state) {
	int m = sd->bl.m;
	
	map_respawn(command_handle_mob, m, BL_MOB);
	return 0;
}
int command_kc(USER *sd, char* line,lua_State* state) {
	int x;
	char buf[255];

	for(x=0;x<10;x++) {
		sprintf(buf,"%d (%d)", sd->status.quest[x].mob_id,sd->status.quest[x].amount);
		clif_sendminitext(sd,buf,0);
	}
	return 0;
}
int command_spellwork(USER *sd,char* line,lua_State* state) {
	map[sd->bl.m].spell^=1;
	return 0;

}
int command_debug(USER *sd, char *line,lua_State* state) {
	char *str[64],*p,*np;
	int strnum,i,skip,packnum;
	if (sscanf(line, "%d%n", &packnum, &skip) < 1)
		return -1;
	p=line+skip;
	while(*p && isspace(*p))
		p++;
	strnum=0;
	for(i=0,np=p;i<64 && p;i++) {
		str[i]=p;
		p=strchr(p,',');
		strnum++;
		if(p) {*p++=0; np=p;}
		else {break;}
	}
	WFIFOB(sd->fd, 0)=0xAA;
	WFIFOW(sd->fd, 1)=SWAP16(strnum+2);
	WFIFOB(sd->fd, 3)=packnum;
	WFIFOB(sd->fd, 4)=0x03;
	for(i=0;i<strnum;i++)
		WFIFOB(sd->fd, 5+i)=atoi(str[i]);
	WFIFOSET(sd->fd, encrypt(sd->fd));
	return 0;
}

int command_item(USER *sd, char *line,lua_State* state) {
	struct item it;
	char itemname[32];
	unsigned int itemnum=0;
	unsigned int itemid=0;
	int x;
	int count = 0;
	
	if (!isdigit(line[0])) {
		if (sscanf(line, "%31s %d", itemname, &itemnum)<1)
			return -1;
	} else {
		if (sscanf(line, "%u %d", &itemid, &itemnum) < 1)
			return -1;
	}

	if (!itemid)
		itemid = itemdb_id(itemname);
	
	if (!itemid)
		return -1;

	if (itemnum<=0)
		itemnum=1;

	memset(&it,0,sizeof(struct item));
	it.id = itemid;
	it.dura = itemdb_dura(itemid);
	it.amount = itemnum;
	it.owner_id=0;
	pc_additem(sd, &it);
	return 0;
}

int command_legend(USER *sd, char *line,lua_State* state) {
	sd->status.legends[0].icon=12;
	sd->status.legends[0].color=128;
	strcpy(sd->status.legends[0].text,"What the fuck?");

	return 0;
}

int command_res(USER *sd, char *line,lua_State* state) {
	if (sd->status.state==PC_DIE)
		pc_res(sd);
	return 0;
}
int command_spell(USER *sd, char *line,lua_State* state) {
	int spell;
	int sound;
	
	if(sscanf(line,"%d %d",&spell,&sound)>-1) {
		spellgfx = spell;
		soundfx = sound;
		map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,spellgfx,&sd->bl,soundfx);
	} else {
		map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,spellgfx,&sd->bl,soundfx);
	}

	//map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,spellgfx,&sd->bl,soundfx);
	//clif_playsound(&sd->bl,sound);
	//run_script(magicdb_script(spell),0,sd->bl.id,0);
	//clif_sendanimation(sd,spell);
	return 0;
}

int command_nspell(USER *sd, char *line,lua_State* state) {
	spellgfx += 1;
	if (spellgfx > 427) spellgfx = 427;
	map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,spellgfx,&sd->bl,soundfx);
}

int command_pspell(USER *sd, char *line,lua_State* state) {
	spellgfx -= 1;
	if (spellgfx < 0) spellgfx = 0;
	map_foreachinarea(clif_sendanimation,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,spellgfx,&sd->bl,soundfx);
}

int command_spellq(USER *sd, char *line,lua_State* state) {
	char mini[25];
	
	sprintf(mini,"Current Spell is: %d\0",spellgfx);
	clif_sendminitext(sd,mini,strlen(mini));
}

int command_hair(USER *sd, char *line,lua_State* state) {
	int hair;
	int hair_color=0;
	if (sscanf(line, "%d %d",&hair,&hair_color)<1)
		return -1;

	sd->status.hair=hair;
	sd->status.hair_color=hair_color;
	clif_sendchararea(sd);
	clif_getchararea(sd);
	return 0;
}
int command_checkdupes(USER *sd, char *line,lua_State* state) {

      USER *tmpsd;
	char BufStr[64];
	int blen;
      int x;
      blen=sprintf(BufStr,"longest eof = %ims", Last_Eof);
	clif_sendminitext(sd,BufStr,blen);




	for(x=1;x<fd_max;x++) {
		if(session[x] && (tmpsd=session[x]->session_data) && !session[x]->eof) {


                 int numDupes = pc_readglobalreg(tmpsd, "goldbardupe");
                 if(numDupes)
                 {
                      blen=sprintf(BufStr,"%s gold bar %i times", tmpsd->status.name, numDupes);
                      clif_sendminitext(sd,BufStr,blen);

                 }


		}
	}







    return 0;

}




int command_checkwpe(USER *sd, char *line,lua_State* state) {

      USER *tmpsd;
	char BufStr[64];
	int blen;
      int x;


	for(x=1;x<fd_max;x++) {
		if(session[x] && (tmpsd=session[x]->session_data) && !session[x]->eof) {


                 int numDupes = pc_readglobalreg(tmpsd, "WPEtimes");
                 if(numDupes)
                 {
                      blen=sprintf(BufStr,"%s WPE attempt %i times", tmpsd->status.name, numDupes);
                      clif_sendminitext(sd,BufStr,blen);

                 }


		}
	}

    return 0;

}








int command_kill(USER *sd, char *line,lua_State* state) {

	char buf[64];
    int  len;

	USER* tsd=map_name2sd(line);
	if(tsd)
	{
	    len = sprintf(buf,"Done.");

		if(session[tsd->fd])session[tsd->fd]->eof = 1;
		clif_sendminitext(sd,buf,len);
	}
	else
	{
        len = sprintf(buf,"User not found.");
		clif_sendminitext(sd,buf,len);

	}

	return 0;
}


int command_killall(USER *sd, char *line,lua_State* state) {

    USER *tmpsd= NULL;
	char buf[16];
    int x;
	for(x=1;x<fd_max;x++) {
		tmpsd=NULL;

		if(session[x] && (tmpsd=session[x]->session_data) && !session[x]->eof) {


                    if(!tmpsd->status.gm_level)
					{session[x]->eof=1;}

		}
	}

      //int len = sprintf(buf,"All but GMs have been mass booted.");
		if(!session[sd->fd]->eof)
			clif_sendminitext(sd,"All but GMs have been mass booted.",0);

	return 0;
}

int command_side(USER *sd, char *line,lua_State* state) {
	int side;
	if (sscanf(line, "%d",&side)<1)
		return -1;

	sd->status.side=side;
	clif_sendchararea(sd);
	clif_getchararea(sd);
	return 0;
}
int command_state(USER *sd, char *line,lua_State* lstate) {
	int state;
	if (sscanf(line, "%d",&state)<1)
		return -1;
	if(sd->status.state==1 && state!=1) {
		pc_res(sd);
	} else {
	sd->status.state = state % 5;
	map_foreachinarea(clif_updatestate,sd->bl.m,sd->bl.x,sd->bl.y,AREA,BL_PC,sd);
	}
	//clif_sendchararea(sd);
	//clif_getchararea(sd);
	return 0;
}
int command_armorcolor(USER *sd, char *line,lua_State* state) {
	int armorcolor;
	if (sscanf(line, "%d",&armorcolor)<1)
		return -1;

	sd->status.armor_color=armorcolor;
	clif_sendchararea(sd);
	clif_getchararea(sd);
	return 0;
}

int command_givespell(USER *sd, char *line,lua_State* state) {
	int spell=0;
	char name[32];
	int x;
	if(sscanf(line,"%s",&name)<1)
		return -1;

	spell=magicdb_id(name);
	for(x=0;x<52;x++) {
		if(sd->status.skill[x]<=0) { //vacant spot
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

int command_deletespell(USER *sd, char *line,lua_State* state) {
	int spell=0;
	char name[32];
	if(sscanf(line,"%s",&name)<1)
		return -1;


	if(spell>=0&&spell<52)
	{
	  sd->status.skill[spell]=0;
	  pc_loadmagic(sd);
	}
	return 0;
}

int command_val(USER *sd, char *line,lua_State* state) {
	//int val1=0;
	//int valval=0;
	//if(sscanf(line,"%d %d",&val1,&valval)<1)
	//	return -1;
	//val[val1]=valval;
	char buf[255];
	memset(buf,0,255);
	sprintf(buf,"Mob spawn count: %d",MOB_SPAWN_MAX-MOB_SPAWN_START+MOB_ONETIME_MAX-MOB_ONETIME_START);
	clif_sendminitext(sd,buf,strlen(buf));

	//clif_sendminitext(Mob Spawn count: %d\n",MOB_SPAWN_MAX-MOB_SPAWN_START);
	//sd->status.guide[val1]=valval;
	/*WFIFOB(sd->fd,0)=0xAA;
	WFIFOW(sd->fd,1)=SWAP16(0x07);
	WFIFOB(sd->fd,3)=0x12;
	WFIFOB(sd->fd,4)=0x03;
	WFIFOB(sd->fd,5)=0x00;
	WFIFOB(sd->fd,6)=0x02;
	WFIFOW(sd->fd,7)=0;
	WFIFOB(sd->fd,8)=0;
	WFIFOB(sd->fd,9)=1;
	encrypt(WFIFOP(sd->fd,0));
	WFIFOSET(sd->fd,10);*/

	//clif_sendstatus3(sd);
	//sd->val[val]=valval;
	//pc_warp(sd,sd->bl.m,sd->bl.x,sd->bl.y);
	//clif_sendmapinfo(sd);
	//clif_sendstatus2(sd);
	//clif_sendchararea(sd);
	//clif_getchararea(sd);
	return 0;
}

int command_makegm(USER *sd, char *line,lua_State* state) {
	int name[32];
	USER *tsd;
	if(sscanf(line,"%31s",&name)<1)
		return -1;

	tsd=map_name2sd(name);
	if(tsd) {
	tsd->status.gm_level=99;
	}
	return 0;
}



int command_xprate(USER *sd, char*line,lua_State* state) {
	int rate;
	char buf[256];
	int len;

	if (sscanf(line,"%d",&rate)<1)
		return -1;

	len=sprintf(buf,"Experience rate: %u x",rate);
	clif_sendminitext(sd,buf,len);
	xp_rate=rate;

	return 0;
}
int command_drate(USER *sd, char *line,lua_State* state) {
	int rate;
	char buf[256];
	int len;

	if (sscanf(line,"%d",&rate)<1)
		return -1;

	len=sprintf(buf,"Drop rate: %u x",rate);
	clif_sendminitext(sd,buf,len);
	d_rate=rate;
	return 0;
}
int command_who(USER *sd, char *line,lua_State* state) {
	int len;
	char buf[256];

	len=sprintf(buf,"There are %d users online.",userlist.user_count);
	clif_sendminitext(sd,buf,len);
	return 0;
}

int command_heal(USER *sd, char *line,lua_State* state) {
	sd->status.hp=sd->max_hp;
	sd->status.mp=sd->max_mp;
	clif_sendstatus(sd,SFLAG_HPMP);
	return 0;
}
int command_level(USER *sd, char *line,lua_State* state) {
	int level;
	if (sscanf(line,"%d",&level)<1)
		return -1;

	sd->status.level=level;
	clif_sendstatus(sd,SFLAG_FULLSTATS);
	return 0;
}
int command_mon(USER *sd, char *line,lua_State* state) {
	unsigned int id = 0;
	int start = 25;
	int end = 25;
	unsigned int replace = 0;
	
	if (sscanf(line, "%d %d %d %d", &id, &start, &end, &replace) < 1)
		return -1;

	map_addmob(sd, id, start, end, replace);
	mobspawn_onetime(id, sd->bl.m, sd->bl.x, sd->bl.y, 0, start, end, replace);
	clif_sendminitext(sd,"Monster added!",0);
	return 0;
}
int command_roll(USER *sd, char* line,lua_State* state) {
	int rand = 0;
	int num = 100;
	char msg[255];
	
	sscanf(line, "%d", &num);
	
	if (num <= 0)
		rand = rnd(100) + 1;
	else
		rand = rnd(num) + 1;
	
	sprintf(msg, "%s rolled a %d out of %d", sd->status.name, rand, num);
	map_foreachinarea(clif_broadcast_sub, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, msg);
	return 0;
}
int command_reloadmob(USER* sd, char* line,lua_State* state) {
	mobdb_read("nothing");
	nullpo_ret(0, sd);
	clif_sendminitext(sd,"Mob DB Reloaded",strlen("Mob DB Reloaded"));
	return 0;
}
int command_reloadspawn(USER* sd, char* line,lua_State* state) {
	mobspawn_read("nothing");
	nullpo_ret(0, sd);
	clif_sendminitext(sd,"Spawn DB Reloaded",strlen("Spawn DB Reloaded"));
	return 0;
}
int command_reloaditem(USER *sd, char* line,lua_State* state) {
	itemdb_read("nothing");
	nullpo_ret(0, sd);
	clif_sendminitext(sd,"Item DB Reloaded!",strlen("Item db reloaded!"));
	return 0;
}
int command_randomspawn(USER *sd, char *line,lua_State* state) {
	int x;
	int y;
	int m;
	int id;
	int times;
	int z;
	int a;
	int max;
	struct block_list *bl;
	MOB *mob;
	/*
	char buf[255];
	if (sscanf(line,"%d %d",&id,&times)<1)
		return -1;

	m=sd->bl.m;
	max=map[m].block_mob_count+1;
	for(z=0;z<times;z++) {
	reloopit:
		x=rand()%map[m].xs;
		y=rand()%map[m].ys;

		for(a=1;a<max;a++){
			if(map[m].block_mob[a]->startx==x && map[m].block_mob[a]->starty==y)
				goto reloopit;
		}

		mobspawn_onetime(id,m,x,y);

	}

	//z=sprintf(buf,"Added %d spawns.",times);
	//clif_sendbluemessage(sd,buf,z);
	*/
	return 0;
}
int command_weap(USER *sd, char *line,lua_State* state) {
	int id;
	int color=0;
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;

	sd->gfx.weapon=id;
	sd->gfx.cweapon=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_url(USER *sd, char* line,lua_State* state) {
	USER* tsd=NULL;
	char name[32];
	char url[128];
	if(sscanf(line,"%s %s",name,url)<1)
		return -1;

	tsd=map_name2sd(name);

	if(!tsd)
		return -1;

	clif_sendurl(tsd,url);
	return 0;
}
int command_armor(USER *sd, char *line,lua_State* state) {
int id;
	int color=0;
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;

	sd->gfx.armor=id;
	sd->gfx.carmor=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_shield(USER *sd, char *line,lua_State* state) {
int id;
	int color=0;
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;

	sd->gfx.shield=id;
	sd->gfx.cshield=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_faceacc(USER *sd, char *line,lua_State* state) {
int id;
	int color=0;
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;

	sd->gfx.faceAcc=id;
	sd->gfx.cfaceAcc=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_crown(USER *sd, char *line,lua_State* state) {
int id;
	int color=0;
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;

	sd->gfx.crown=id;
	sd->gfx.ccrown=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_necklace(USER *sd, char *line,lua_State* state) {
int id;
	int color=0;
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;
	sd->gfx.necklace=id;
	sd->gfx.cnecklace=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_mantle(USER *sd, char *line,lua_State* state) {
int id;
	int color=0;
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;
	sd->gfx.mantle=id;
	sd->gfx.cmantle=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_boots(USER *sd, char *line,lua_State* state) {
int id;
	int color=0;
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;

	sd->gfx.boots=id;
	sd->gfx.cboots=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_helm(USER *sd, char *line,lua_State* state) {
	int id;
	int color = 0;
	
	if(sscanf(line,"%d %d",&id,&color)<1)
		return -1;

	sd->gfx.helm=id;
	sd->gfx.chelm=color;
	clif_getchararea(sd);
	clif_sendchararea(sd);
	return 0;
}
int command_cinv(USER *sd, char *line,lua_State* state) {
	int x;
	int slotone = -1;
	int	slottwo = -1;

	if(sscanf(line,"%d %d",&slotone,&slottwo) < 0) {
		for(x=0;x<MAX_INVENTORY;x++) {
			if(sd->status.inventory[x].id > 0) {
				if(sd->status.inventory[x].amount > 0) {
					pc_delitem(sd,x,sd->status.inventory[x].amount,itemdb_type(sd->status.inventory[x].id));
				}
			}
		}
	} else {
		for(x=slotone;x<=slottwo;x++) {
			if(sd->status.inventory[x].id > 0) {
				if(sd->status.inventory[x].amount > 0) {
					pc_delitem(sd,x,sd->status.inventory[x].amount,itemdb_type(sd->status.inventory[x].id));
				}
			}
		}
	}

	return 0;
}

int command_cfloor(USER *sd, char *line,lua_State* state) {
	/*FLOORITEM* fl=NULL;
	int bx,by,nx,ny,x0,x1,y0,y1,m,x,y,owner,radius;
	int nAreaSizeX, nAreaSizeY;
	int blocks = 0;
	struct block_list *bl;
	
	if(sscanf(line,"%d",&radius) == 1) {
		nAreaSizeX = radius;
		nAreaSizeY = radius;
		m=sd->bl.m;
		x=sd->bl.x;
		y=sd->bl.y;
		nx=map[m].xs-x;
		ny=map[m].ys-y;

		if(nx<18) nAreaSizeX=nAreaSizeX*2;//-nx;
		if(ny<16) nAreaSizeY=nAreaSizeY*2;//-ny;
		if(x<18) nAreaSizeX=nAreaSizeX*2;//-x;
		if(y<16) nAreaSizeY=nAreaSizeY*2;//-y;

		x0 = x - nAreaSizeX;
		x1 = x + nAreaSizeX;
		y0 = y - nAreaSizeY;
		y1 = y + nAreaSizeY;

		if (x0 < 0) x0 = 0;
		if (y0 < 0) y0 = 0;
		if (x1 >= map[m].xs) x1 = map[m].xs-1;
		if (y1 >= map[m].ys) y1 = map[m].ys-1;

		for(by = y0 / BLOCK_SIZE; by <= y1 / BLOCK_SIZE; by++) {
			for(bx = x0 / BLOCK_SIZE; bx <= x1 / BLOCK_SIZE; bx++) {
				for( bl = map[m].block[bx+by*map[m].bxs] ; bl != NULL && blocks<32768; bl = bl->next ) {
					blocks++;
					if(bl->type == BL_ITEM && bl->x>=x0 && bl->x<=x1 && bl->y>=y0 && bl->y<=y1 && blocks<32768) {
						fl=(FLOORITEM*)map_id2bl((unsigned int)bl->id);
						clif_lookgone(&fl->bl);
						map_delitem(fl->bl.id);
						if(fl->timer) timer_remove(fl->timer);
					}
				}
			}
		}
	} else {
		bx=sd->bl.x/BLOCK_SIZE;
		by=sd->bl.y/BLOCK_SIZE;

		for( bl = map[sd->bl.m].block[bx+by*map[sd->bl.m].bxs] ; bl != NULL && blocks<32768; bl = bl->next ) {
			blocks++;
			if(bl->type==BL_ITEM && bl->x==sd->bl.x && bl->y==sd->bl.y && blocks<32768) {
				fl=(FLOORITEM*)map_id2bl((unsigned int)bl->id);
				clif_lookgone(&fl->bl);
				map_delitem(fl->bl.id);
				if(fl->timer) timer_remove(fl->timer);
			}
		}
	}
*/
	return 0;
}
int command_cspells(USER *sd, char *line,lua_State* state) {
	int x;
	int spellone = -1;
	int spelltwo = -1;

	if(sscanf(line,"%d %d",&spellone,&spelltwo) < 1) {
		for(x=0;x<MAX_SPELLS;x++) {
			if(sd->status.skill[x] > 0) {
				sd->status.skill[x] = 0;
				pc_loadmagic(sd);
			}
		}
	} else {
		for(x=spellone;x<=spelltwo;x++) {
			if(sd->status.skill[x] > 0) {
				sd->status.skill[x] = 0;
				pc_loadmagic(sd);
			}
		}
	}

	return 0;
}

int command_job(USER *sd, char *line,lua_State* state) {
	int job;
	int subjob;

	if(sscanf(line,"%d %d",&job,&subjob) < 1) {
		return 0;
	}

	sd->status.class = job;
	sd->status.mark = subjob;
	clif_mystaytus(sd);
	return 0;
}

int command_music(USER *sd, char *line,lua_State* state) {
	int oldm, oldx, oldy;
	int music;
	
	if(sscanf(line,"%d",&music) > -1) {
		musicfx = music;
		oldm = sd->bl.m;
		oldx = sd->bl.x;
		oldy = sd->bl.y;
		map[sd->bl.m].bgm = musicfx;
		pc_warp(sd, 10002, 0, 0);
		pc_warp(sd, oldm, oldx, oldy);
	}

	return 0;
}

int command_musicn(USER *sd, char *line,lua_State* state) {
	int oldm, oldx, oldy;
	
	musicfx += 1;
	oldm = sd->bl.m;
	oldx = sd->bl.x;
	oldy = sd->bl.y;
	map[sd->bl.m].bgm = musicfx;
	pc_warp(sd, 10002, 0, 0);
	pc_warp(sd, oldm, oldx, oldy);
	return 0;
}

int command_musicp(USER *sd, char *line,lua_State* state) {
	int oldm, oldx, oldy;
	
	musicfx -= 1;
	oldm = sd->bl.m;
	oldx = sd->bl.x;
	oldy = sd->bl.y;
	map[sd->bl.m].bgm = musicfx;
	pc_warp(sd, 10002, 0, 0);
	pc_warp(sd, oldm, oldx, oldy);
	return 0;
}

int command_musicq(USER *sd, char *line,lua_State* state) {
	char mini[25];
	
	sprintf(mini,"Current music is: %d\0",musicfx);
	clif_sendminitext(sd,mini,strlen(mini));
	return 0;
}

int command_sound(USER *sd, char *line,lua_State* state) {
	int sound;
	
	if(sscanf(line,"%d",&sound) > -1) {
		soundfx = sound;
		clif_playsound(sd,soundfx);
	} else {
		clif_playsound(sd,soundfx);
	}

	return 0;
}

int command_nsound(USER *sd, char *line,lua_State* state) {
	soundfx += 1;
	if (soundfx > 125) soundfx = 125;
	clif_playsound(sd,soundfx);
	return 0;
}

int command_psound(USER *sd, char *line,lua_State* state) {
	soundfx -= 1;
	if (soundfx < 0) soundfx = 0;
	clif_playsound(sd,soundfx);
	return 0;
}

int command_soundq(USER *sd, char *line,lua_State* state) {
	char mini[25];
	
	sprintf(mini,"Current sound is: %d\0",soundfx);
	clif_sendminitext(sd,mini,strlen(mini));
	return 0;
}

int command_reloadboard(USER *sd, char *line,lua_State* state) {
	boarddb_read();
	bn_read();
	nullpo_ret(0, sd);
	clif_sendminitext(sd,"Board DB reloaded!",strlen("Board DB reloaded!"));
	return 0;
}

int command_online(USER *sd, char *line,lua_State* state) {
	int x,numid;
	unsigned int idlist;
	char name[100];
	SqlStmt* stmt;

	stmt=SqlStmt_Malloc(sql_handle);
	
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `ChaId` FROM `Character` WHERE `ChaOnline` = '1'")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &idlist, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	numid = SqlStmt_NumRows(stmt);
	clif_sendminitext(sd, "----------------", strlen("----------------"));
	
	for (x = 0; x < numid && SQL_SUCCESS == SqlStmt_NextRow(stmt); x++) {
		USER* tsd = map_id2sd(idlist);
		sprintf(name,"%s\a\0", tsd->status.name);
		clif_sendminitext(sd, name, strlen(name));
	}

	SqlStmt_Free(stmt);
	return 0;
}

int command_reloadnpc(USER *sd, char *line,lua_State* state) {
	npc_init();
	nullpo_ret(0, sd);
	clif_sendminitext(sd,"NPC DB reloaded!",strlen("NPC DB reloaded!"));
	return 0;
}

/*int command_metan(USER *sd, char *line) {
	metan_main();
	clif_sendminitext(sd,"Metan Complete!",strlen("Metan Complete!"));
	return 0;
}*/

/*int command_reloadrecipe(USER *sd, char *line) {
	recipedb_term();
	recipedb_init();
	clif_sendminitext(sd, "Recipe DB reloaded!", strlen("Recipe DB reloaded!"));
	return 0;
}*/

int command_reloadmaps(USER *sd, char *line,lua_State* state) {
	map_reload();
	
	if (char_fd > 0 && session[char_fd] != NULL) {
		int i = 0;
		int j = 0;
		
		WFIFOHEAD(char_fd, map_n * 2 + 8);
		WFIFOW(char_fd, 0) = 0x3001;
		WFIFOL(char_fd, 2) = map_n * 2 + 8;
		WFIFOW(char_fd, 6) = map_n;
		
		for(i = 0; i < MAX_MAP_PER_SERVER; i++) {
			if (map[i].tile != NULL) {
				WFIFOW(char_fd, j * 2 + 8) = i;
				j++;
			}
			
			if (j >= map_n)
				break;
		}
		
		WFIFOSET(char_fd, map_n * 2 + 8);
	}
	
	nullpo_ret(0, sd);
	clif_sendminitext(sd, "Maps reloaded!", strlen("Maps reloaded!"));
	return 0;
}

int command_reloadclass(USER* sd, char* line,lua_State* state) {
	classdb_read();
	nullpo_ret(0, sd);
	clif_sendminitext(sd, "Classes and Levels reloaded!", strlen("Classes and Levels reloaded!"));
	return 0;
}

int command_reloadwarps(USER* sd, char* line,lua_State* state) {
	warp_init();
	nullpo_ret(0, sd);
	clif_sendminitext(sd, "Warps reloaded!", strlen("Warps reloaded!"));
	return 0;
}

int command_reload(USER *sd, char *line, lua_State* state) {
	//command_metan(sd,line);
	int errors = command_luareload(sd,line,state);
	
	command_magicreload(sd,line,state);
	command_reloadmob(sd,line,state);
	command_reloadspawn(sd,line,state);
	command_reloaditem(sd,line,state);
	command_reloadnpc(sd,line,state);
	command_reloadboard(sd,line,state);
	//command_reloadrecipe(sd,line);
	command_reloadmaps(sd, line,state);
	command_reloadclass(sd, line,state);
	command_reloadwarps(sd, line,state);
	
	nullpo_ret(errors, sd);
	clif_sendminitext(sd,"Mini reset complete!",strlen("Mini reset complete!"));
	return errors;
}
int at_command(USER *sd,const char *p, int len) {
	int i;
	char cmd_line[257];
	char *cmdp;
	if (*p!=command_code)
		return 0;
	p++;
	//copy command to buffer
	memcpy(cmd_line,p,len);
	cmd_line[len]=0x00;
	cmdp = cmd_line;
	//Search for end of command string
	while(*cmdp && !isspace(*cmdp))
		cmdp++;
	//set null for comparement
	*cmdp='\0';
	for(i=0;command[i].func;i++) {
		if (!strcmpi(cmd_line,command[i].name))
			break;
	}
	//wrong command, exit!
	if (command[i].func==NULL)
		return 0;
	//found command
	//skip our null terminate
	cmdp++;
	//let's go
	command[i].func(sd, cmdp, NULL);
	return 1;
}
