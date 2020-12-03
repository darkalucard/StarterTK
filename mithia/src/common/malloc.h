
#ifndef _MALLOC_H_
#define _MALLOC_H_
#define __func__ __FUNCTION__
#include <stddef.h>

//#define ALC_MARK __FILE__, __LINE__, __func__
//#include "memwatch.h"
//memory allocation macro
//----------------------------
#define ALC_MARK __FILE__, __LINE__, __func__

//#define malloc(n) GC_malloc(n)
//#define calloc(n,t) GC_malloc(n*t)
//#define realloc(p,x) GC_realloc((p),(x))
//#define free(x) GC_free(x)
#ifdef NO_MEMMGR
	#define aMalloc(n) malloc(n)
	#define aCalloc(m,n) calloc(m,n)
	#define aRealloc(p,n) _realloc(p,n,ALC_MARK)
	#define aFree(p) free(p)
	#define STRDUP(p,file,line,func)	strdup(p)
	
	
#endif

#if !defined(NO_MEMMGR) && !defined(USE_MEMMGR)
#define USE_MEMMGR
#endif

#define aStrdup(a) aStrdup_(a,ALC_MARK)
char* aStrdup_(const char*, const char*, int, const char*);
//////////////////////////////////////////////////////////////////////
// Athena's built-in Memory Manager
#ifdef USE_MEMMGR

// Enable memory manager logging by default
#define LOG_MEMMGR

#	define amalloc(n)		_mmalloc(n,ALC_MARK)
#	define acalloc(m,n)		_mcalloc(m,n,ALC_MARK)
#	define arealloc(p,n)	_mrealloc(p,n,ALC_MARK)
//#	define strdup(p)		_mstrdup(p,ALC_MARK)
#	define afree(p)			_mfree(p,ALC_MARK)

	void* _mmalloc	(size_t size, const char *file, int line, const char *func);
	void* _mcalloc	(size_t num, size_t size, const char *file, int line, const char *func);
	void* _mrealloc	(void *p, size_t size, const char *file, int line, const char *func);
	//char* _mstrdup	(const char *p, const char *file, int line, const char *func);
	void  _mfree	(void *p, const char *file, int line, const char *func);
#endif

char* strlwr(char*);
void add_log(char*, ...);
#ifndef strcmpi
#define strcmpi(x,y) strcasecmp(x,y)
#endif

#define CALLOC(result, type, number) if (!((result) = (type *)aCalloc((number),sizeof(type)))) \
	printf("SYS_ERR: calloc failure at %s:%d(%s).\n", __FILE__, __LINE__, __func__), exit(1)

#define REALLOC(result, type, number)  (result) = (type *)aRealloc((result), sizeof(type) * (number))

#define FREE(result) do { aFree(result); result = NULL; } while(0)
//void* _mmalloc	(size_t size, const char *file, int line, const char *func);
//void* _mcalloc	(size_t num, size_t size, const char *file, int line, const char *func);
//void* _mrealloc	(void *p, size_t size, const char *file, int line, const char *func);
//char* _mstrdup	(const char *p, const char *file, int line, const char *func);
//void  _mfree	(void *p, const char *file, int line, const char *func);

//#define CALLOC(result,type,number) result=(type*)_mcalloc((number),sizeof(type),ALC_MARK)
//#define REALLOC(result,type,number) result=(type*)_mrealloc((result),sizeof(type)*(number),ALC_MARK)
//#define FREE(result) if((result)) _mfree((result),ALC_MARK), (result) = NULL
int free_lock(void);
int free_unlock(void);
char* s_inject(char*);
//void FREE(void*);
//null pointer macro check [Aier]
//----------------------------
//if null pointer found, then return
#define nullpo_ret(result, target) if (!(target)) return (result)

//critical null pointer failure, terminate program!
#define nullpo_chk(target) if (!(target)) \
	printf("SYS_ERR: nullpo failure at %s:%d(%s).\n", __FILE__, __LINE__, __func__), exit(1)

#endif
