
#ifndef _NPC_H_
#define _NPC_H_

int npc_src_clear();
int npc_src_add(const char*);
int npc_init();
//int npc_charinit();
unsigned int npc_id;
unsigned int npc_get_new_npctempid();
int npc_idlower(int);
int npc_runtimers(int,int);
int npc_action(NPC*);
int npc_duration(NPC*);
int npc_move_sub(struct block_list*,va_list);
int npc_move(NPC*);
int npc_warp(NPC*,int,int,int);
int npc_readglobalreg(NPC*,char*);
int npc_setglobalreg(NPC*,char*,int);

#endif
