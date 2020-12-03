#ifndef _MD5CALC_H_
#define _MD5CALC_H_

#ifndef PROTO
#if defined (USE_PROTOTYPES) ? USE_PROTOTYPES : defined (__STDC__)
#define PROTO(ARGS) ARGS
#else
#define PROTO(ARGS) ()
#endif
#endif

typedef unsigned long cvs_uint32;

struct cvs_MD5Context {
	cvs_uint32 buf[4];
	cvs_uint32 bits[2];
	unsigned char in[64];
};

void cvs_MD5Init PROTO ((struct cvs_MD5Context *context));
void cvs_MD5Update PROTO ((struct cvs_MD5Context *context,
			   unsigned char const *buf, unsigned len));
void cvs_MD5Final PROTO ((unsigned char digest[16],
			  struct cvs_MD5Context *context));
void cvs_MD5Transform PROTO ((cvs_uint32 buf[4], const unsigned char in[64]));
void MD5_String(const char * string, char * output);
void MD5_Binary(const char * string, unsigned char * output);
#endif /* _MD5CALC_H_ */
