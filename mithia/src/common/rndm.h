#ifndef _RNDM_H
#define _RNDM_H

#define rnd(x) ((int)(randomMT()&0xFFFFFF)%(x))
#define rndscale(x) ((float) ((randomMT()&0xFFFFFF)*(double) (x)/(double) (0xFFFFFF)))

extern void seedMT(unsigned int seed);
extern unsigned int randomMT(void);

#endif

