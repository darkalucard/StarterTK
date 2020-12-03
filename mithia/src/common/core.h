
#ifndef	_CORE_H_
#define	_CORE_H_

int main(int,char**);
const char* get_svn_revision(void);
unsigned int getTicks(void);
void crash_log(char*,...);
void set_dmpfile(char*);
void add_dmp(int,int);
void set_logfile(char*);
void set_termfunc(void (*termfunc)(void));
static void sig_proc(int);
static void display_title(void);
int set_default_input(int(*func)(char*));
int default_parse_input(char*);
int do_init(int,char**);

unsigned long Last_Eof;

#endif	// _CORE_H_
