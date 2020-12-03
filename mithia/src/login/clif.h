
#ifndef _CLIF_H_
#define _CLIF_H_

struct login_session_data {
	unsigned int id;
	char name[16];
	char pass[16];
	char face, country, totem, sex, hair,face_color,hair_color;
};

int clif_message(int,char,char*);

int clif_accept(int);
int clif_parse(int);

#endif
