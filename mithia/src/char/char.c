
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "core.h"
#include "socket.h"
#include "timer.h"
#include "version.h"
#include "char.h"
#include "logif.h"
#include "saveif.h"
#include "mapif.h"
#include "mmo.h"
#include "malloc.h"
#include "db.h"
#include "char_db.h"
#include "zlib.h"

#ifdef USE_MYSQL
#include "db_mysql.h"
#endif

int char_port = 2005;

struct DBMap *login_data;
struct mmo_charstatus *char_dat;
struct map_fifo_data map_fifo[MAX_MAP_SERVER];
int map_fifo_n = 0;
int map_fifo_max = 0;
struct Sql* sql_handle;
int char_fd;
int login_fd;
int save_fd;

int start_money = 100;
struct point start_pos;

char char_id[32];
char char_pw[32];
//Sql ID/PW
char sql_id[32]="";
char sql_pw[32]="";
char sql_db[32]="mithia";
char sql_ip[32]="";
int sql_port;

int dump_save=0;

char login_id[32];
char login_pw[32];
char login_ip_s[16];
int login_ip;
int login_port;
char save_id[32];
char save_pw[32];
char save_ip_s[16];
int save_ip;
int save_port;

int zlib_init(void) {
	z_stream strm;
	int ret;
	strm.zalloc=Z_NULL;
	strm.zfree=Z_NULL;
	strm.opaque=Z_NULL;
	ret=deflateInit(&strm,Z_DEFAULT_COMPRESSION);
	//printf("ret: %d\n",ret);
	return 0;
}
int mapfifo_from_mapid(int map) {
	int i,j;
	for(i=0;i<map_fifo_max;i++) {
		if(map_fifo[i].fd > 0) {
			for(j=0;j<map_fifo[i].map_n;j++) {
				if(map_fifo[i].map[j] == map) {
					return i;
				}
			}
		}
	}
	return -1;
}

/*==========================================
 * Login Data
 *  Register online character via memory
 *  and sql. But we prefer to use memory
 *  for fast and accurate result.
 *------------------------------------------
 */
int logindata_init() {
	login_data = uidb_alloc(DB_OPT_BASE);
	//mmo_setallonline(0);
	return 0;
}

int logindata_final(void *key,void *data,va_list ap) {
	struct item_data *dat;
	nullpo_ret(0,dat=data);
	FREE(dat);
	return 0;
}

int logindata_term() {
	if(login_data){
		//numdb_final(login_data, logindata_final);
		login_data=NULL;
	}
	//mmo_setallonline(0);
	return 0;
}

struct char_login_data* logindata_search(unsigned int id) {
	struct char_login_data *dat;
	dat = (struct char_login_data*)uidb_get(login_data, id);
	return dat;
}



/*int logindata_del(unsigned int id) {
	struct char_login_data *dat;
	dat = numdb_search(login_data, id);
	//mmo_setonline(id,0);
	
	if (!dat)
		return 0;

	FREE(dat);
	numdb_erase(login_data, id);
	//mmo_setonline(id, 0);
	return 0;
}*/

int logindata_change(unsigned int id, int map_server) {
	struct char_login_data *dat;
	dat = (struct char_login_data*)uidb_get(login_data, id);
	if (!dat)
		return -1;

	dat->map_server = map_server;
	return 0;	
}

/*==========================================
 * Configuration File Read
 *------------------------------------------
 */
int config_read(const char *cfg_file) {
	char line[1024], r1[1024], r2[1024];
	int line_num = 0;
	FILE *fp;

	fp = fopen(cfg_file, "r");
	if (fp == NULL) {
		printf("CFG_ERR: Configuration file (%s) not found.\n", cfg_file);
		add_log("CFG_ERR: Configuration file (%s) not found.\n", cfg_file);
		exit(1);
	}

	while(fgets(line, sizeof(line), fp)) {
		line_num++;
		if (line[0] == '/' && line[1] == '/')
			continue;

		if (sscanf(line, "%[^:]: %[^\r\n]", r1, r2) == 2) {
			//CHAR
			if(strcmpi(r1, "char_port") == 0) {
				char_port = atoi(r2);
			} else if(strcmpi(r1, "char_id") == 0) {
				strncpy(char_id, r2, 32);
				char_id[31] = '\0';
			} else if(strcmpi(r1, "char_pw") == 0) {
				strncpy(char_pw, r2, 32);
				char_pw[31] = '\0';
			//LOGIN
			} else if(strcmpi(r1, "login_ip") == 0) {
				strncpy(login_ip_s, r2, 16);
				login_ip_s[15] = '\0';
				login_ip = inet_addr(login_ip_s);
			} else if(strcmpi(r1, "login_port") == 0) {
				login_port = atoi(r2);
			} else if(strcmpi(r1, "login_id") == 0) {
				strncpy(login_id, r2, 32);
				login_id[31] = '\0';
			} else if(strcmpi(r1, "login_pw") == 0) {
				strncpy(login_pw, r2, 32);
				login_pw[31] = '\0';
			//Save
			} else if(strcmpi(r1, "save_ip") == 0) {
				strncpy(save_ip_s, r2, 16);
				save_ip_s[15] = '\0';
				save_ip = inet_addr(save_ip_s);
			} else if(strcmpi(r1, "save_port") == 0) {
				save_port = atoi(r2);
			} else if(strcmpi(r1, "save_id") == 0) {
				strncpy(save_id, r2, 32);
				save_id[31] = '\0';
			} else if(strcmpi(r1, "save_pw") == 0) {
				strncpy(save_pw, r2, 32);
				save_pw[31] = '\0';
#ifdef USE_MYSQL
			//SQL
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
#endif
			//DUMP & LOG
			} else if(strcmpi(r1, "char_log") == 0) {
				set_logfile(r2);
			} else if(strcmpi(r1, "dump_log") == 0) {
				set_dmpfile(r2);
			} else if(strcmpi(r1, "dump_save") == 0) {
				dump_save = atoi(r2);
			//NEW CHARS
			} else if(strcmpi(r1, "start_money") == 0) {
				start_money = atoi(r2);
			} else if(strcmpi(r1, "start_point") == 0) {
				int m, x, y;
				sscanf(r2, "%d,%d,%d", &m, &x, &y);
				start_pos.m = m;
				start_pos.x = x;
				start_pos.y = y;
			}
		}
	}
	fclose(fp);
	printf("Configuration File (%s) reading finished!\n", cfg_file);
	return 0;
}

void do_term(void) {
	logindata_term();
	char_db_term();
	session_eof(login_fd);
	printf("Mithia Char Server Shutdown.\n");
	add_log("Shutdown.\n");
}


void help_screen() {
	printf("HELP LIST\n");
	printf("---------\n");
	printf(" --conf [FILENAME]  : set config file\n");
	printf(" --inter [FILENAME] : set inter file\n");
	exit(0);
}
int keep_login_alive(int n, int a) {
	if(login_fd) {
		WFIFOB(login_fd,0)=0xFF;
		WFIFOB(login_fd,1)=1;
		WFIFOSET(login_fd,2);
	}
	return 0;
}
int do_init(int argc, char **argv) {
	int i;
	char *CONF_FILE = "conf/char.conf";
	char *INTER_FILE = "conf/inter.conf";

	srand(gettick());
	set_logfile("log/char.log");
	set_dmpfile("log/char_dump.log");

	for (i = 1; i < argc ; i++) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "--h") == 0 || strcmp(argv[i], "--?") == 0 || strcmp(argv[i], "/?") == 0)
			help_screen();
		else if (strcmp(argv[i], "--conf") == 0)
			CONF_FILE=argv[i+1];
		else if (strcmp(argv[i],"--inter") == 0)
			INTER_FILE = argv[i+1];
	}

	config_read(CONF_FILE);
	config_read(INTER_FILE);
	set_termfunc(do_term);

	add_log("");
	add_log("Mithia Char Server Started.\n");

	char_db_init();
	logindata_init();

	set_defaultparse(mapif_parse_auth);
	char_fd = make_listen_port(char_port);

	timer_insert(1000,1000*10,check_connect_login, login_ip, login_port);
	timer_insert(1000,1000,check_connect_save, save_ip, save_port);
	//timer_insert(5000,5000,keep_login_alive,0,0);
	CALLOC(char_dat, struct mmo_charstatus, 1);

	printf("Mithia Char Server is \033[1;32mready\033[0m! Listening at %d.\n", char_port);
	add_log("Server Ready! Listening at %d.\n", char_port);
	
	zlib_init();
	return 0;
}
