
#ifndef _MAPIF_H_
#define _MAPIF_H_

enum {ALL, ALLWOS};

int mapif_parse_auth(int);
int mapif_parse(int);
int mapif_send(int,unsigned char*,int,int);

#endif
