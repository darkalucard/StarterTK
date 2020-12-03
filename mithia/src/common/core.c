
//#include <stdio.h>
//#include <curses.h>
#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/time.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "socket.h"
#include "timer.h"
#include "version.h"
#define ISDIGIT(c) (isdigit((unsigned char)(c)))

int (*func_parse_it)(char*) = default_parse_input;
static void (*term_func)(void) = NULL;
char dmp_filename[128];
char log_filename[128];
char date_format[32] = "%Y-%m-%d %H:%M:%S";
static char h_svn_version[10] = "";
struct timeval start;
static long long check2 = 0;
// Main Routine
//----------------------------
int main(int argc,char **argv)
{



	Last_Eof = 0;

    gettimeofday(&start, NULL);

	int next;
	int run=1;
	//char str[65536];
	//memset(str,0,65536);
	server_shutdown=0;
	
	do_socket();

	signal(SIGPIPE,sig_proc);
	signal(SIGTERM,sig_proc);
	signal(SIGINT,sig_proc);
	db_init();
	display_title();
	timer_init();
	
	do_init(argc,argv);
	//initscr();
	//timeout(0);
	while(run){
		//#ifdef LOGGING_ENABLED
			//remove("function-log.txt"); //Remove file.
		//#endif
		//free_lock();
		next = timer_do(gettick_nocache());
		//free_unlock();
		do_sendrecv(next);
		do_parsepacket();

		//getstr(str);

	}
	/*pthread_mutex_init(&mutexl,NULL);

	if(pthread_create(&thread[0],NULL,timer_thread,NULL)!=0)
		perror("Pthread_create timer failed.");

	if(pthread_create(&thread[1],NULL,socket_thread,NULL)!=0)
		perror("Pthread_create socket failed.");

	//pthread_join(thread[0],NULL);
	//pthread_join(thread[1],NULL);
	while(run) {
	}*/
	return 0;
}

//#include <pthread.h>
const char* get_svn_revision(void)
{
	FILE *fp;

	if(*h_svn_version)
		return h_svn_version;

	if ((fp = fopen(".svn/entries", "r")) != NULL)
	{
		char line[1024];
		int rev;
		// Check the version
		if (fgets(line, sizeof(line), fp))
		{
			if(!ISDIGIT(line[0]))
			{
				// XML File format
				while (fgets(line,sizeof(line),fp))
					if (strstr(line,"revision=")) break;
				if (sscanf(line," %*[^\"]\"%d%*[^\n]", &rev) == 1) {
					snprintf(h_svn_version, sizeof(h_svn_version), "%d", rev);
				}
			}
			else
			{
				// Bin File format
				fgets(line, sizeof(line), fp); // Get the name
				fgets(line, sizeof(line), fp); // Get the entries kind
				if(fgets(line, sizeof(line), fp)) // Get the rev numver
				{
					snprintf(h_svn_version, sizeof(h_svn_version), "%d", atoi(line));
				}
			}
		}
		fclose(fp);
	}

	if(!(*h_svn_version))
		snprintf(h_svn_version, sizeof(h_svn_version), "Unknown");

	return h_svn_version;
}



unsigned int getTicks(void)
{

        struct timeval now;
        unsigned long ticks;
        long long ticks2;

        gettimeofday(&now, NULL);
        ticks2 = (((long long) now.tv_sec) * 1000000) + now.tv_usec;
        check2 = ticks2;
        ticks=(now.tv_sec-start.tv_sec)*1000+(now.tv_usec-start.tv_usec)/1000;
        return(ticks);
}


void crash_log(char *aids, ...)
{





}


// Set Dump Packet File
//----------------------------
void set_dmpfile(char *dmpfilename)
{
	memset(dmp_filename, 0, 128);
	strcpy(dmp_filename, dmpfilename);
}

// Add packet to dump
// Saved from RFIFO
//----------------------------
void add_dmp(int fd, int len)
{
	/*
	FILE *dmpfp;
	int i;
	struct timeval tv;
	char timetmp[128];

	unsigned char *p = (unsigned char *) &session[fd]->client_addr.sin_addr;
	char ip[16];
	sprintf(ip, "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);

	dmpfp = fopen("C:\\lastpacket.hex", "w");
	if (dmpfp) {
		gettimeofday(&tv, NULL);
		strftime(timetmp, 24, date_format, localtime(&(tv.tv_sec)));
		fprintf(dmpfp, "%s IP: %s len %d\n", timetmp, ip, len);
		fprintf(dmpfp, "[HEX]:");
		for(i=0;i < len;i++) {
			fprintf(dmpfp, "[%02X]", RFIFOB(fd, i));
		}
		fprintf(dmpfp, "\n[CHR]:");
		for(i=0;i < len;i++) {
			fprintf(dmpfp, "[ %c]", RFIFOB(fd, i));
		}
		fprintf(dmpfp, "\n");
	}
	fclose(dmpfp);
	*/
}

// Set Log File
//----------------------------
void set_logfile(char *logfilename)
{
	memset(log_filename, 0, 128);
	strcpy(log_filename, logfilename);
}




// Set terminate function
//----------------------------
void set_termfunc(void (*termfunc)(void))
{
	term_func = termfunc;
}

// Signal handling
//----------------------------
static void sig_proc(int sn)
{
	int i;
	switch(sn){
	case SIGINT:
	case SIGTERM:
		if(term_func)
			term_func();
		timer_clear();
		for(i=0;i<fd_max;i++){
			if(!session[i])
				continue;
			//close(i);
			session_eof(i);
		}
		//endwin();
		exit(0);
		break;
	case SIGPIPE:
		break;

	}
}

// Display Titlename
//----------------------------
static void display_title(void)
{
	printf("\033[2J"); // clear screen and go up/left (0, 0 position in text)

	printf("Mithia - Revision %s\n", get_svn_revision());
#if MITHIA_RELEASE_FLAG
	//printf("**Mithia Develop version!\n");
#endif
}
/*void *timer_thread(void *nothing) {
	int run=1;

	while(run) {
		pthread_mutex_lock(&mutexl);
		timer_do(gettick_nocache());
		pthread_mutex_unlock(&mutexl);
	}
}
void *socket_thread(void *nothing) {
	int run=1;
	while(run) {
		pthread_mutex_lock(&mutexl);
		do_sendrecv(1000);
		do_parsepacket();
		pthread_mutex_unlock(&mutexl);
	}
}*/

int set_default_input(int (*func)(char*)) {
	func_parse_it=func;
	return 0;
}
int default_parse_input(char* val) {
	return 0;
}

//
