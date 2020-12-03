
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "core.h"
#include "socket.h"
#include "timer.h"
#include "version.h"
#include "save.h"
#include "charif.h"
#include "mmo.h"
#include "malloc.h"
#include "db.h"
#include "save_db.h"
#include "zlib.h"

#ifdef USE_MYSQL
#include "db_mysql.h"
#endif

int save_port = 2006;

struct Sql* sql_handle;
int save_fd;
int char_fd;

char save_id[32];
char save_pw[32];
//Sql ID/PW
char sql_id[32]="";
char sql_pw[32]="";
char sql_db[32]="mithia";
char sql_ip[32]="";
int sql_port;

int dump_save=0;

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
			//SAVE
			if(strcmpi(r1, "save_port") == 0) {
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
			}
		}
	}
	fclose(fp);
	printf("Configuration File (%s) reading finished!\n", cfg_file);
	return 0;
}

void do_term(void) {
	save_db_term();
	printf("Mithia Save Server Shutdown.\n");
	add_log("Shutdown.\n");
}

int do_init(int argc, char **argv) {
	int i;
	char *CONF_FILE = "conf/save.conf";
	char *INTER_FILE = "conf/inter.conf";

	srand(gettick());
	set_logfile("log/save.log");
	set_dmpfile("log/save_dump.log");

	for (i = 1; i < argc ; i++) {
		if (strcmp(argv[i], "--conf") == 0)
			CONF_FILE=argv[i+1];
		else if (strcmp(argv[i],"--inter") == 0)
			INTER_FILE = argv[i+1];
	}

	config_read(CONF_FILE);
	config_read(INTER_FILE);
	set_termfunc(do_term);

	add_log("");
	add_log("Mitha Save Server Started.\n");
	
	save_db_init();

	set_defaultparse(charif_parse_auth);
	save_fd = make_listen_port(save_port);

	printf("Mithia Save Server is \033[1;32mready\033[0m! Listening at %d.\n", save_port);
	add_log("Server Ready! Listening at %d.\n", save_port);
	
	zlib_init();
	return 0;
}
