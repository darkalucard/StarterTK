
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../common/malloc.h"
#include "../common/showmsg.h"

char* aStrdup_(const char *p, const char *file, int line, const char *func)
{
	char *ret = STRDUP(p, file, line, func);
	// ShowMessage("%s:%d: in func %s: aStrdup %p\n",file,line,func,p);
	if (ret == NULL){
		ShowFatalError("%s:%d: in func %s: aStrdup error out of memory!\n", file, line, func);
		exit(EXIT_FAILURE);
	}
	return ret;
}
char *strlwr(char *string)
{
      char *s;

      if (string)
      {
            for (s = string; *s; ++s)
                  *s = tolower(*s);
      }
      return string;
}
// Add log line
//----------------------------
void add_log(char *fmt, ...)
{
	/*FILE *logfp;
	va_list ap;
	struct timeval tv;
	char tmpstr[128];

	va_start(ap, fmt);

	logfp = fopen(log_filename, "a");
	if (logfp) {
		if (fmt[0] == '\0') // jump a line if no message
			fprintf(logfp, "\n");
		else {
			gettimeofday(&tv, NULL);
			strftime(tmpstr, 24, date_format, localtime(&(tv.tv_sec)));
			sprintf(tmpstr + strlen(tmpstr), ".%03d: %s", (int)tv.tv_usec / 1000, fmt);
			vfprintf(logfp, tmpstr, ap);
		}
		fclose(logfp);
	}

	va_end(ap);*/
}
void * _realloc(void* ptr, size_t size, char* file, int line, char* func) {

	void* newp;
	newp=realloc(ptr,size);
	if(!newp) {
		free(ptr);
		printf("Error Reallocing at %s in %s at line %d\n",func,file,line);
		return 0;
	}
	
	return newp;
}
//Injection Fix
char* s_inject(char* s) {
	char *p;
	int i;
	int newpos=0;
	int n_size=0;
	
	if(!s) {
		CALLOC(p,char,16);
		return p;
	}
	
	if(!strlen(s)) {
		CALLOC(p,char,16);
		return p;
	}
	//First, find how many things we need to increase
	for(i=0;i<strlen(s);i++) {
		if(s[i]=='\\') {
			n_size+=2;
		} else if(s[i]=='\'') {
			n_size+=2;
		} else {
			n_size+=1;
		}
		
	}
	
	CALLOC(p,char,n_size+1);
	memset(p,0,n_size);
	if(n_size==strlen(s)) {
		for(i=0;i<strlen(s)+1;i++) {
			p[i]=s[i];
		}
		//s[n_size]=0;
		return p;
	}

	for(i=0;i<strlen(s)+1;i++) {
		if(s[i]=='\\') {
			p[newpos]='\\';
			p[newpos+1]='\\';
			//p[newpos+2]='\\';
			//p[newpos+3]='\\';
			newpos+=2;
		} else if(s[i]=='\'') {
			p[newpos]='\\';
			//p[newpos+1]='\\';
			p[newpos+1]='\'';
			//p[newpos+2]='\'';
			newpos+=2;
		} else {
			p[newpos]=s[i];
			newpos+=1;
		}
		
	}
	//s[newpos]=0;
	
	//printf("In: %s\ Out: n%s\n",s,p);
	//printf("Length: %d\nLength 2: %d\nNewPos: %d\n",strlen(s),strlen(p),newpos);

	return p;

}
	