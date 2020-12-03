#ifndef _SL_H_
#define _SL_H_
#include <lua.h>
#include "map.h"

lua_State *sl_gstate;
void sl_init();
void sl_runfunc(char *, struct block_list *);
int sl_doscript_blargs(char *, char *, int, ...);
int sl_doscript_stackargs(char *, char *, int);
int sl_updatepeople(struct block_list*, va_list);
#define sl_doscript_simple(root, method, bl) \
	sl_doscript_blargs(root, method, 1, bl)
void sl_resumemenu(unsigned int, USER *);
void sl_resumedialog(unsigned int, USER *);
void sl_resumebuy(unsigned int,USER *);
void sl_resumeinput(char *,char *,USER *);
void sl_resumesell(unsigned int,USER *);
void sl_exec(USER *, char *);
void sl_async_freeco(USER*);
int sl_reload();
int sl_luasize(USER*);
void sl_fixmem();
#endif
