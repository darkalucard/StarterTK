
#ifndef _LOGIN_H_
#define _LOGIN_H_

#define MSG_MAX 11
#define META_MAX 20

enum {
	LGN_ERRSERVER,
	LGN_WRONGPASS,
	LGN_WRONGUSER,
	LGN_ERRDB,
	LGN_USEREXIST,
	LGN_ERRPASS,
	LGN_ERRUSER,
	LGN_NEWCHAR,
	LGN_CHGPASS,
	LGN_DBLLOGIN,
	LGN_BANNED
};

extern int char_fd;
extern char login_id[];
extern char login_pw[];
extern char login_msg[MSG_MAX][256];
extern int require_reg;
extern char sql_id[];
extern char sql_pw[];
extern char sql_ip[];
extern char sql_db[];
extern int sql_port;
extern struct Sql* sql_handle;
char meta_file[META_MAX][256];
int metamax;
int nex_version;
int nex_deep;
extern int dump_save;
int getInvalidCount(unsigned int);
int setInvalidCount(unsigned int);

int lang_read(const char*);
int config_read(const char*);
int string_check(const char*,int);
int string_check_allchars(const char*, int);
void do_term(void);

#endif
