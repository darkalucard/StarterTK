
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "malloc.h"
#include "map.h"
#include "npc.h"
#include "script.h"
#include "db.h"
#include "db_mysql.h"
#include "strlib.h"
#include "sl.h"
#include "timer.h"
#include "clif.h"
#include "mob.h"

struct npc_src_list {
	struct npc_src_list *next;
	char *file;
};
struct npc_src_list *npc_src_first = NULL;
struct npc_src_list *npc_src_last = NULL;
struct npc_src_list *warp_src_first =NULL;
struct npc_src_list *warp_src_last=NULL;

//unsigned int npcchar_id = CNPC_START_NUM;
unsigned int npctemp_id = NPCT_START_NUM;

// Function Prototype
//----------------------------
unsigned int npc_get_new_npcid();
//unsigned int npc_get_new_npccharid();
unsigned int npc_get_new_npctempid();
unsigned int npc_get_new_npcid() {
	unsigned int x;
	struct block_list* bl = NULL;

	for (x = NPC_START_NUM; x <= npc_id; x++) {
		if(x == npc_id) npc_id++;
		bl = map_id2bl(x);
		if (!bl) {
			return x;
		}
	}
	
	npc_id++;
	return npc_id;
}

unsigned int npc_get_new_npctempid() {
	unsigned int x;
	struct block_list* bl = NULL;

	for (x = NPCT_START_NUM; x <= npctemp_id; x++) {
		if(x == npctemp_id) npctemp_id++;
		bl = map_id2bl(x);
		if (!bl) {
			return x;
		}
	}
	
	npctemp_id++;
	return npctemp_id;
}
int npc_idlower(int id) {
	if (id >= NPCT_START_NUM && id != F1_NPC) {
		npctemp_id--;
	}
	return 0;
}
int npc_src_clear() {
	struct npc_src_list *p = npc_src_first;

	while(p) {
		struct npc_src_list *p2 = p;
		p = p->next;
		FREE(p2->file);
		FREE(p2);
	}
	npc_src_first = NULL;
	npc_src_last = NULL;
	return 0;
}

int npc_src_add(const char *file) {
	struct npc_src_list *new;

	CALLOC(new, struct npc_src_list, 1);
	CALLOC(new->file, char, strlen(file) + 1);
	new->next = NULL;
	strncpy(new->file, file, strlen(file));
	if (npc_src_first == NULL)
		npc_src_first = new;
	if (npc_src_last)
		npc_src_last->next = new;

	npc_src_last = new;
	return 0;
}
int npc_warp_add(const char *file) {
	struct npc_src_list *new;

	CALLOC(new, struct npc_src_list,1);
	CALLOC(new->file,char,strlen(file)+1);
	new->next=NULL;
	strncpy(new->file, file, strlen(file));
	if (warp_src_first == NULL)
		warp_src_first = new;
	if (warp_src_last)
		warp_src_last->next = new;

	warp_src_last = new;
	return 0;
}

int warp_init() {
	FILE *fp;
	char line[1024];
	char *str[10];
	char *p,*np;
	int lines = 0;
	int x;
	int mx,my,mm,tm,tx,ty;
	struct npc_src_list *i;
	struct warp_list* war = NULL;
	
	for(i=warp_src_first;i;i=i->next) {
		if ((fp=fopen(i->file,"r"))==NULL) {
			printf("WARP_ERR: WARP file not found (%s).\n",i->file);
			continue;
		}
		
		while (fgets(line,sizeof(line),fp)) {
			lines++;
			if ((line[0] == '/' && line[1] == '/') || line[0] == '\n')
				continue;
			memset(str,0,sizeof(str));
			if (sscanf(line, "%u,%u,%u,%u,%u,%u", &mm, &mx, &my, &tm, &tx, &ty) != 6) continue;
			/*(for(x=0,np=p=line;x<6 && p;x++) {
				str[x]=p;
				p=strchr(p,',');
				if(p){ *p++=0; np=p; }
			}*/
			
			CALLOC(war,struct warp_list,1);
			/*mm=atoi(str[0]);
			mx=atoi(str[1]);
			my=atoi(str[2]);
			tm=atoi(str[3]);
			tx=atoi(str[4]);
			ty=atoi(str[5]);*/
			if (!map_isloaded(mm) || !map_isloaded(tm)) continue;
			war->x=mx;
			war->y=my;
			war->tm=tm;
			war->tx=tx;
			war->ty=ty;
			war->next=map[mm].warp[(mx/BLOCK_SIZE)+(my/BLOCK_SIZE)*map[mm].bxs];
			if(war->next) war->next->prev=war;
			map[mm].warp[(mx/BLOCK_SIZE)+(my/BLOCK_SIZE)*map[mm].bxs]=war;
			/*map[mm].warp[(mx/BLOCK_SIZE)+(my/BLOCK_SIZE)*map[mm].bxs]
			map[mm].warp[(mx/BLOCK_SIZE)+(my/BLOCK_SIZE)*map[mm].bxs].y=my;
			map[mm].warp[(mx/BLOCK_SIZE)+(my/BLOCK_SIZE)*map[mm].bxs].tm=tm;
			map[mm].warp[(mx/BLOCK_SIZE)+(my/BLOCK_SIZE)*map[mm].bxs].tx=tx;
			map[mm].warp[(mx/BLOCK_SIZE)+(my/BLOCK_SIZE)*map[mm].bxs].ty=ty;
			map[mm].warp[*/
		}
			
			fclose(fp);
	}

}

int npc_init() {
	NPC *nd = NULL;
	struct item item;
	unsigned int x, i, count, id, gid, gc, subtype, time, ids;
	unsigned short sex, m, xc, yc, face, facecolor, hair, haircolor, armorcolor, skincolor;
	char name[45];
	char c_name[45];
	char side, state, npctype, pos;
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	memset(&item, 0, sizeof(item));
	
	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `NpcId`, `NpcIdentifier`, `NpcDescription`, `NpcType`, `NpcMapId`, `NpcX`, `NpcY`, `NpcLook`, `NpcLookColor`, `NpcTimer`, `NpcSex`, `NpcSide`, `NpcState`,"
		"`NpcFace`, `NpcFaceColor`, `NpcHair`, `NpcHairColor`, `NpcSkinColor`, `NpcIsChar` FROM `NPCs` ORDER BY `NpcId`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_STRING, &name, sizeof(name), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_STRING, &c_name,sizeof(c_name), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_UCHAR, &subtype,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_USHORT, &m,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_USHORT, &xc,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_USHORT, &yc,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 7, SQLDT_UINT, &gid,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 8, SQLDT_UINT, &gc,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 9, SQLDT_UINT, &time,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 10, SQLDT_USHORT, &sex,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 11, SQLDT_CHAR, &side,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 12, SQLDT_CHAR, &state,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 13, SQLDT_USHORT, &face,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 14, SQLDT_USHORT, &facecolor,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 15, SQLDT_USHORT, &hair,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 16, SQLDT_USHORT, &haircolor,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 17, SQLDT_USHORT, &skincolor,0,NULL,NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 18, SQLDT_CHAR, &npctype,0,NULL,NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	count = SqlStmt_NumRows(stmt);

	for (x = 1; x <= count && SQL_SUCCESS == SqlStmt_NextRow(stmt); x++) {
		nd = map_id2npc(x);
		if (id == 1) {
			nd = map_id2npc(F1_NPC);
			if (nd == NULL) {
				CALLOC(nd, NPC, 1);
			} else {
				map_deliddb(&nd->bl);
			}
		} else if (nd == NULL) {
			CALLOC(nd, NPC, 1);
		} else {
			map_delblock(&nd->bl);
			map_deliddb(&nd->bl);
		}
		
		memcpy(nd->name, name, sizeof(name));
		memcpy(nd->npc_name, c_name, sizeof(c_name));
		nd->bl.type = BL_NPC;
		nd->bl.subtype = subtype;
		nd->bl.graphic_id = gid;
		nd->bl.graphic_color = gc;
		nd->startm = m;
		nd->startx = xc;
		nd->starty = yc;
		nd->id = id;
		nd->actiontime = time;
		nd->sex = sex;
		nd->side = side;
		nd->state = state;
		nd->face = face;
		nd->face_color = facecolor;
		nd->hair = hair;
		nd->hair_color = haircolor;
		nd->armor_color = 0;
		nd->skin_color = skincolor;
		nd->npctype = npctype;
		
		if (nd->bl.id < NPC_START_NUM) {
			nd->bl.m = m;
			nd->bl.x = xc;
			nd->bl.y = yc;
			if(id == 1) {
				nd->bl.id = F1_NPC;
			} else {
				nd->bl.id = NPC_START_NUM + id - 2;
				npc_id = NPC_START_NUM + id - 2;
			}
		}
		
		if(nd->bl.subtype < 3) {
			if(map_addblock(&nd->bl)) printf("Error NPC\n");
		}
		
		map_addiddb(&nd->bl);
	}
	
	for (x = NPC_START_NUM; x <= npc_id; x++) {
		nd = map_id2npc((unsigned int)x);
		
		if (nd) {
			if (nd->npctype == 1) {
				if (SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT '', `NeqLook`, '1', '0', '0', `NeqColor`, `NeqSlot` FROM `NPCEquipment` WHERE `NeqNpcId` = '%u' LIMIT 14", nd->id)
				|| SQL_ERROR == SqlStmt_Execute(stmt)
				|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_STRING, &item.real_name, sizeof(item.real_name), NULL, NULL)
				|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_UINT, &item.id, 0, NULL, NULL)
				|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_UINT, &item.amount, 0, NULL, NULL)
				|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_UINT, &item.dura, 0, NULL, NULL)
				|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_UINT, &item.owner_id, 0, NULL, NULL)
				|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_UINT, &item.custom, 0, NULL, NULL)
				|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_UCHAR, &pos, 0, NULL, NULL)) {
					SqlStmt_ShowDebug(stmt);
					SqlStmt_Free(stmt);
					return 0;
				}

				//Equip Read
				for (i = 0; i < 14 && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++) {
					memcpy(&nd->equip[pos], &item, sizeof(item));
				}
			}
		}
	}
	
	SqlStmt_Free(stmt);
	printf("NPC script read done. Total %u NPC read!\n", count);
	return 0;
}

int npc_runtimers(int none, int nonetwo) {
	NPC *nd = NULL;
	unsigned int x;
	
	//regular npc
	for (x = NPC_START_NUM; x <= npc_id; x++) {
		nd = map_id2npc(x);
		
		if (nd) {
			if (nd->actiontime > 0) {
				npc_action(nd);
			}

			if (nd->duration > 0) {
				npc_duration(nd);
			}
		}
	}
	
	//char npc
	/*for (x = CNPC_START_NUM; x < npcchar_id; x++) {
		bl = map_id2bl(x);
		
		if (bl) {
			if (bl->type == BL_NPC && bl->npctype == 1) {
				cnd = (CNPC*)bl;
		
				if (cnd) {
					if (cnd->actiontime > 0) {
						npc_action(&cnd->bl);
					
						if (cnd->duration > 0) {
							npc_duration(&cnd->bl);
						}
					}
				}
			}
		}
	}*/
	
	//temp npc
	for (x = NPCT_START_NUM; x <= npctemp_id; x++) {
		nd = map_id2npc(x);
		
		if (nd) {
			if (nd->actiontime > 0) {
				npc_action(nd);
			}
			
			if (nd->duration > 0) {
				npc_duration(nd);
			}
		}
	}
	return 0;
}

int npc_action(NPC *nd) {
	USER *tsd = NULL;
	
	nullpo_ret(0, nd);
	
	nd->time += 100;
		
	if (nd->owner) {
		tsd = map_id2sd(nd->owner);
	}
		
	if (tsd) {
		if (nd) {
			if (nd->time >= nd->actiontime) {
				nd->time = 0;
				sl_doscript_blargs(nd->name, "action", 2, &nd->bl, &tsd->bl);
			}
		}
	} else {
		if (nd) {
			if (nd->time >= nd->actiontime) {
				nd->time = 0;
				sl_doscript_blargs(nd->name, "action", 1, &nd->bl);
			}
		}
	}
	
	return 0;
}

int npc_duration(NPC *nd) {
	USER *tsd = NULL;
	
	nullpo_ret(0, nd);
	
	nd->duratime += 100;
		
	if (nd->owner) {
		tsd = map_id2sd(nd->owner);
	}
		
	if (tsd) {
		if (nd) {
			if (nd->duratime >= nd->duration) {
				nd->duratime = 0;
				sl_doscript_blargs(nd->name, "endAction", 2, &nd->bl, &tsd->bl);
			}
		}
	} else {
		if (nd) {
			if (nd->duratime >= nd->duration) {
				nd->duratime = 0;
				sl_doscript_blargs(nd->name, "endAction", 1, &nd->bl);
			}
		}
	}
	
	return 0;
}

int npc_move_sub(struct block_list *bl, va_list ap) {
	NPC *nd = NULL;
	MOB *mob = NULL;
	USER *sd = NULL;

	nullpo_ret(0, nd = va_arg(ap, NPC*));
	
	if(nd->canmove == 1) return 0;
	
	if(bl->type==BL_NPC) {
		if(bl->subtype) {
			return 0;
		}
	} else if(bl->type==BL_MOB) {
		mob=(MOB*)bl;
		
		if(mob) {
			if(mob->state==MOB_DEAD) {
				return 0;
			}
		}
	} else if(bl->type==BL_PC) {
		sd = (USER*)bl;
		
		if ((map[nd->bl.m].show_ghosts && sd->status.state == PC_DIE) || sd->status.state == -1 || sd->status.gm_level >= 50) {
			return 0;
		}
	}

	nd->canmove=1;
	return 0;
}

int npc_move(NPC* nd){
	int direction, backx, backy, m, dx, dy, cm, x0, y0, x1, y1;
	int c = 0;
	struct warp_list* i;
	int nothingnew = 0;
	int subt[1];
	int def[2];
	
	subt[0] = 0;
	m = nd->bl.m;
	backx = nd->bl.x;
	backy = nd->bl.y;
	dx = backx;
	dy = backy;
	direction = nd->side;
	x0 = nd->bl.x;
	y0 = nd->bl.y;

	switch(direction){
	case 0: //UP
		if (backy>0) {
			dy = backy-1;
			x0 -= 9;
			
			if (x0 < 0) x0 = 0;
			
			y0 -= 9;
			y1 = 1;
			x1 = 19;
			
			if (y0 < 7) nothingnew = 1;
			
			if (y0 == 7) {
			    y1 += 7;
			    y0 = 0;
			}
			
			if (x0 + 19 + 9 >= map[m].xs)
                x1 += 9 - ((x0 + 19 + 9) - map[m].xs);
            
			if (x0 <= 8) {
                x1 += x0;
                x0 = 0;
            }
		}
		break;
	case 1: //Right
		if (backx<map[m].xs) {
			x0 += 10;
			y0 -= 8;
			
			if(y0 < 0) y0 = 0;
			
			dx = backx+1;
			y1 = 17;
			x1 = 1;
			
			if (x0 > map[m].xs - 9) nothingnew = 1;
			
			if (x0 == map[m].xs - 9) x1 += 9;
			
			if (y0 + 17 + 8 >= map[m].ys)
                y1 += 8 - ((y0 + 17 + 8) - map[m].ys);
            
			if (y0 <= 7) {
                y1 += y0;
                y0 = 0;
            }

		}
		break;
	case 2: //down
		if (backy<map[m].ys) {
			x0 -= 9;
			
			if(x0 < 0) x0 = 0;
			
			y0 += 9;
			dy = backy + 1;
			y1 = 1;
			x1 = 19;
            
			if (y0 + 8 > map[m].ys) nothingnew = 1;
            
			if (y0 + 8 == map[m].ys) y1 += 8;
            
			if (x0 + 19 + 9 >= map[m].xs)
				x1 += 9 - ((x0 + 19 + 9) - map[m].xs);
            
			if (x0 <= 8) {
                x1 += x0;
                x0 = 0;
            }
		}
		break;
	case 3: //left
		if (backx > 0) {
			x0 -= 10;
			y0 -= 8;
			
			if(y0 < 0) y0 = 0;
			
			y1 = 17;
			x1 = 1;
			dx = backx-1;
            
			if(x0 < 8) nothingnew = 1;
            
			if(x0 == 8){
                x0 = 0;
                x1 += 8;
            }
            
			if (y0 + 17 + 8 >= map[m].ys)
                y1 += 8 - ((y0 + 17 + 8) - map[m].ys);
            
			if (y0 <= 7) {
                y1 += y0;
                y0 = 0;
            }
		}
		break;
	default:
		break;
	}

	if (dx >= map[m].xs) dx = map[m].xs - 1;
	
	if (dy >= map[m].ys) dy = map[m].ys - 1;
	
	for (i = map[m].warp[dx / BLOCK_SIZE + (dy / BLOCK_SIZE) * map[m].bxs]; i && !c; i = i->next) {
		if (i->x == dx && i->y == dy) {
			c = 1;
			return 0;
		}
	}

	map_foreachincell(npc_move_sub, m, dx, dy, BL_MOB, nd);
	map_foreachincell(npc_move_sub, m, dx, dy, BL_PC, nd);
	map_foreachincell(npc_move_sub, m, dx, dy, BL_NPC, nd);
	
	if (clif_object_canmove(m, dx, dy, direction)) {
		nd->canmove = 0;
		return 0;
	}
	
	if (clif_object_canmove_from(m, backx, backy, direction)) {
		nd->canmove = 0;
		return 0;
	}
	
	cm = nd->canmove;
	
	if (map_canmove(m, dx, dy) == 1 || nd->canmove == 1) {
		nd->canmove = 0;
		return 0;
	}

	if (x0 > map[m].xs) x0 = map[m].xs - 1;
	
	if (y0 > map[m].ys) y0 = map[m].ys - 1;
	
	if (x0 < 0) x0 = 0;
	
	if (y0 < 0) y0 = 0;

	if (dx >= map[m].xs) dx = backx;
	
	if (dy >= map[m].ys) dy = backy;
	
	if (dx < 0) dx = backx;
	
	if (dy < 0) dy = backy;

	if (dx != backx || dy != backy) {
		nd->bl.bx = backx;
		nd->bl.by = backy;
		map_moveblock(&nd->bl, dx, dy);
	
		if(!nothingnew) {
			if (nd->npctype == 1) {
				map_foreachinblock(clif_cnpclook_sub, nd->bl.m, x0, y0, x0+(x1-1), y0+(y1-1), BL_PC, LOOK_SEND, nd);
			} else {
				map_foreachinblock(clif_mob_look_start_func,nd->bl.m,x0,y0,x0+(x1-1),y0+(y1-1),BL_PC,nd);
				map_foreachinblock(clif_object_look_sub,nd->bl.m,x0,y0,x0+(x1-1),y0+(y1-1),BL_PC,LOOK_SEND,&nd->bl);
				map_foreachinblock(clif_mob_look_close_func,nd->bl.m,x0,y0,x0+(x1-1),y0+(y1-1),BL_PC,nd);
			}
		}

		map_foreachinarea(clif_npc_move, m, nd->bl.x, nd->bl.y, AREA, BL_PC, LOOK_SEND, nd);
		return 1;
	} else {
		return 0;
	}
}

int npc_warp(NPC *nd, int m, int x, int y) {
	nullpo_ret(0, nd);
	if (nd->bl.id < NPC_START_NUM) return 0;
	map_delblock(&nd->bl);
	clif_lookgone(&nd->bl);
	nd->bl.m = m;
	nd->bl.x = x;
	nd->bl.y = y;
	nd->bl.type = BL_NPC;
	
	if(map_addblock(&nd->bl)) {
		printf("Error warping npcchar.\n");
	}
	
	if (nd->npctype == 1) {
		map_foreachinarea(clif_cnpclook_sub, nd->bl.m, nd->bl.x, nd->bl.y, AREA, BL_PC, LOOK_SEND, nd);
	} else {
		map_foreachinarea(clif_object_look_sub2, nd->bl.m, nd->bl.x, nd->bl.y, AREA, BL_PC, LOOK_SEND, nd);
	}
	return 0;
}

int npc_readglobalreg(NPC *nd, char *reg) {
	int i, exist;
	
	exist = -1;
	nullpo_ret(0, nd);

	for (i = 0; i < MAX_GLOBALREG; i++) {
		if (strcmpi(nd->registry[i].str, reg) == 0) {
			exist = i;
			break;
		}
	}

	if (exist != -1){
		return nd->registry[exist].val;
	} else {
		return 0;
	}

	return 0;
}

int npc_setglobalreg(NPC *nd, char *reg, int val) {
	int i, exist;
	
	exist = -1;
	nullpo_ret(0, nd);
	nullpo_ret(0, reg);
	
	//if registry exists, get number
	for (i = 0; i < MAX_GLOBALREG; i++) {
		if (strcmpi(nd->registry[i].str, reg) == 0) {
			exist = i;
			break;
		}
	}
	
	//if registry exists, set value
	if (exist != -1) {
		if(val == 0) {
			strcpy(nd->registry[exist].str, ""); //empty registry
			nd->registry[exist].val = val;
			return 0;
		} else {
			nd->registry[exist].val = val;
			return 0;
		}
	} else {
		for (i = 0; i < MAX_GLOBALREG; i++) {
			if(strcmpi(nd->registry[i].str, "") == 0) {
				strcpy(nd->registry[i].str, reg);
				nd->registry[i].val = val;
				return 0;
			}
		}
	}
	
	printf("npc_setglobalreg : couldn't set %s\n", reg);
	return 1;
}