
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "core.h"
#include "socket.h"
#include "timer.h"
#include "version.h"
#include "login.h"
#include "crypt.h"
#include "clif.h"
#include "zlib.h"
#include "db_mysql.h"
#include "db.h"

int login_port = 2010;

int login_fd;
int char_fd;
int require_reg=0;
char login_msg[MSG_MAX][256];
Sql* sql_handle=NULL;
DBMap* bf_lockout=NULL;
char login_id[32];
char login_pw[32];
int dump_save = 0;
//Sql ID/PW
char sql_id[32]="";
char sql_pw[32]="";
char sql_db[32]="mithia";
char sql_ip[32]="";
int sql_port;
//static char check_table_thing[]={'\\','/',';','=',':','(',')',NULL}
const char mask1[]="aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ";
const char mask2[]="aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ 1234567890";

extern void Remove_Throttle(int, int);



int Valid(const char* buf,const char* mask) {
	
	int x;
	int n;
	int found=0;
	
	for(x=0;x<strlen(buf);x++) {
		for(n=0;n<strlen(mask);n++) {
			if(buf[x]==mask[n]) {
				found=1;
			}
		}
		if(!found) { return 1; }
		else {found=0;}
	}
	
	return 0;
}

int string_check_allchars(const char *p, int len) {
	char buf[255];
	memset(buf,0,255);
	memcpy(buf,p,len);
	//printf("Allchars: %s\n",buf);
	return Valid(buf,mask1);
}
int string_check(const char *p,int len) {
	char buf[255];
	memset(buf,0,255);
	memcpy(buf,p,len);
	//printf("string_check: %s\n",buf);
	return Valid(buf,mask2);
}

int lang_read(const char *cfg_file) {
	char line[1024], r1[1024], r2[1024];
	int line_num = 0;
	FILE *fp;

	fp = fopen(cfg_file, "r");
	if (fp == NULL) {
		printf("CFG_ERR: Language file (%s) not found.\n", cfg_file);
		return 1;
	}

	while(fgets(line, sizeof(line), fp)) {
		line_num++;
		if (line[0] == '/' && line[1] == '/')
			continue;

		if (sscanf(line, "%[^:]: %[^\r\n]", r1, r2) == 2) {
			if(strcmpi(r1, "LGN_ERRSERVER") == 0) {
				strncpy(login_msg[LGN_ERRSERVER], r2, 256);
				login_msg[LGN_ERRSERVER][255] = '\0';
			} else if(strcmpi(r1, "LGN_WRONGPASS") == 0) {
				strncpy(login_msg[LGN_WRONGPASS], r2, 256);
				login_msg[LGN_WRONGPASS][255] = '\0';
			} else if(strcmpi(r1, "LGN_WRONGUSER") == 0) {
				strncpy(login_msg[LGN_WRONGUSER], r2, 256);
				login_msg[LGN_WRONGUSER][255] = '\0';
			} else if(strcmpi(r1, "LGN_ERRDB") == 0) {
				strncpy(login_msg[LGN_ERRDB], r2, 256);
				login_msg[LGN_ERRDB][255] = '\0';
			} else if(strcmpi(r1, "LGN_USEREXIST") == 0) {
				strncpy(login_msg[LGN_USEREXIST], r2, 256);
				login_msg[LGN_USEREXIST][255] = '\0';
			} else if(strcmpi(r1, "LGN_ERRPASS") == 0) {
				strncpy(login_msg[LGN_ERRPASS], r2, 256);
				login_msg[LGN_ERRPASS][255] = '\0';
			} else if(strcmpi(r1, "LGN_ERRUSER") == 0) {
				strncpy(login_msg[LGN_ERRUSER], r2, 256);
				login_msg[LGN_ERRUSER][255] = '\0';
			} else if(strcmpi(r1, "LGN_NEWCHAR") == 0) {
				strncpy(login_msg[LGN_NEWCHAR], r2, 256);
				login_msg[LGN_NEWCHAR][255] = '\0';
			} else if(strcmpi(r1, "LGN_CHGPASS") == 0) {
				strncpy(login_msg[LGN_CHGPASS], r2, 256);
				login_msg[LGN_CHGPASS][255] = '\0';
			} else if(strcmpi(r1, "LGN_DBLLOGIN") == 0) {
				strncpy(login_msg[LGN_DBLLOGIN], r2, 256);
				login_msg[LGN_DBLLOGIN][255] = '\0';
			} else if(strcmpi(r1, "LGN_BANNED") == 0) {
				strncpy(login_msg[LGN_BANNED], r2, 256);
				login_msg[LGN_BANNED][255] = '\0';
			} 
		}
	}
	fclose(fp);
	printf("Language File (%s) reading finished!\n", cfg_file);
	return 0;
}
	
int config_read(const char *cfg_file) {
	char line[1024], r1[1024], r2[1024];
	int line_num = 0;
	FILE *fp;

	fp = fopen(cfg_file, "r");
	if (fp == NULL) {
		printf("CFG_ERR: Configuration file (%s) not found.\n", cfg_file);
		return 1;
	}

	while(fgets(line, sizeof(line), fp)) {
		line_num++;
		if (line[0] == '/' && line[1] == '/')
			continue;

		if (sscanf(line, "%[^:]: %[^\r\n]", r1, r2) == 2) {
			if(strcmpi(r1, "login_port") == 0) {
				login_port = atoi(r2);
			} else if(strcmpi(r1, "login_id") == 0) {
				strncpy(login_id, r2, 32);
				login_id[31] = '\0';
			} else if(strcmpi(r1, "login_pw") == 0) {
				strncpy(login_pw, r2, 32);
				login_pw[31] = '\0';
			} else if(strcmpi(r1, "login_log") == 0) {
				set_logfile(r2);
			} else if(strcmpi(r1, "dump_log") == 0) {
				set_dmpfile(r2);
			} else if(strcmpi(r1, "dump_save") == 0) {
				dump_save = atoi(r2);
			} else if(strcmpi(r1, "meta") == 0) {
				add_meta(r2);
			} else if(strcmpi(r1,"version")==0) {
				nex_version=atoi(r2);
			} else if(strcmpi(r1,"deep")==0) {
				nex_deep=atoi(r2);
			} else if(strcmpi(r1,"sql_ip")==0) {
				strcpy(sql_ip,r2);
			} else if(strcmpi(r1, "sql_port") == 0) {
				sql_port=atoi(r2);
			} else if(strcmpi(r1, "sql_id") == 0) {
				strcpy(sql_id,r2);
			} else if(strcmpi(r1, "sql_pw") == 0) {
				strcpy(sql_pw,r2);
			} else if(strcmpi(r1, "sql_db") == 0) {
				strcpy(sql_db,r2);
			} else if(strcmpi(r1, "require_reg") == 0) {
			    require_reg=atoi(r2);
			}
		}
	}
	fclose(fp);
	printf("Configuration File (%s) reading finished!\n", cfg_file);
	return 0;
}

void do_term(void) {
	printf("Mithia Login Server Shutdown.\n");
	add_log("Shutdown.\n");
}

void help_screen() {
	printf("HELP LIST\n");
	printf("---------\n");
	printf(" --conf [FILENAME]  : set config file\n");
	printf(" --inter [FILENAME] : set inter file\n");
	printf(" --lang [FILENAME]  : set lang file\n");
	exit(0);
}

int do_init(int argc, char **argv) {
	int i;
	char *CONF_FILE = "conf/login.conf";
	char *LANG_FILE = "conf/lang.conf";
	char *INTER_FILE = "conf/inter.conf";

	srand(gettick());
	set_logfile("log/login.log");
	set_dmpfile("log/login_dump.log");

	for (i = 1; i < argc ; i++) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "--h") == 0 || strcmp(argv[i], "--?") == 0 || strcmp(argv[i], "/?") == 0)
			help_screen();
		else if (strcmp(argv[i], "--conf") == 0)
			CONF_FILE=argv[i+1];
		else if (strcmp(argv[i],"--inter") == 0)
			INTER_FILE = argv[i+1];
		else if (strcmp(argv[i],"--lang") == 0)
			LANG_FILE = argv[i+1];
	}

	config_read(CONF_FILE);
	config_read(INTER_FILE);
	config_read("conf/char.conf");
	sql_handle = Sql_Malloc();
	if(sql_handle==NULL)
	{
		Sql_ShowDebug(sql_handle);
		exit(EXIT_FAILURE);
	}
	if( SQL_ERROR == Sql_Connect(sql_handle, sql_id, sql_pw, sql_ip, (uint16)sql_port, sql_db) )
	{
		printf("id: %s pass: %s Port: %d\n",sql_id, sql_pw,sql_port);
		Sql_ShowDebug(sql_handle);
		Sql_Free(sql_handle);
		exit(EXIT_FAILURE);
	}
	
	//sql_init();
	lang_read(LANG_FILE);
	set_termfunc(do_term);
	//zlib_init();
	add_log("");
	add_log("Mithia Login Server Started.\n");

	set_defaultaccept(clif_accept);
	set_defaultparse(clif_parse);
	login_fd = make_listen_port(login_port);
	timer_insert(10 * 60 * 1000, 10 * 60 * 1000, Remove_Throttle, 0, 0);
	//Lockout DB
	bf_lockout=uidb_alloc(DB_OPT_BASE);
	
	printf("Mithia Login Server is \033[1;32mready\033[0m! Listening at %d.\n", login_port);
	add_log("Server Ready! Listening at %d.\n", login_port);
	return 0;
}

int getInvalidCount(unsigned int ip) {
	int c=uidb_get(bf_lockout,ip);
	return c;
}

int login_clear_lockout(int i, int d)
{
	uidb_remove(bf_lockout,(unsigned int)i);
	return 1;
}
int setInvalidCount(unsigned int ip) {
	int c=uidb_get(bf_lockout,ip);
	
	if(!c) {
		timer_insert(10*60*1000,10*60*1000,login_clear_lockout,ip,0);
	}

	uidb_put(bf_lockout,ip,c+1);
	
	return c+1;
}
int add_meta(char* file) {
	strcpy(meta_file[metamax],file);
	metamax++;
}

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
