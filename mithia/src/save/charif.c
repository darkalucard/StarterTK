
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "socket.h"
#include "save.h"
#include "charif.h"
#include "mmo.h"
#include "malloc.h"
#include "save_db.h"
#include "zlib.h"
#include "db_mysql.h"
#include "strlib.h"

static const int packet_len_table[] = {66, -1, -1, 0 }; //NULL terminator

int charif_parse_auth(int fd) {
	int cmd;
	
	if (fd == 0 || session[fd] == NULL || session[fd]->eof) {
		session_eof(fd);
		return 0;
	}
	cmd=RFIFOW(fd,0);
	
	if (cmd == 0x4000) {
		if ((strcmp(RFIFOP(fd,2),save_id)) && (strcmp(RFIFOP(fd,34),save_pw))) {
			WFIFOHEAD(fd,3);
			WFIFOW(fd,0) = 0x4000;
			WFIFOB(fd,2) = 0x01;
			WFIFOSET(fd, 3);
			session[fd]->eof=1;
			return 0;
		}

		char_fd = fd;
		
		session[char_fd]->func_parse = charif_parse;
		realloc_rfifo(char_fd, FIFOSIZE_SERVER, FIFOSIZE_SERVER);
		WFIFOHEAD(char_fd,3);
		WFIFOW(char_fd,0) = 0x4000;
		WFIFOB(char_fd,2) = 0x00;
		WFIFOSET(char_fd, 3);

		add_log("Connection from Char Server accepted.\n");
		printf("Connection from Char Server accepted.\n");
	}
	
	return 0;
}

int charif_parse_savechar(int fd) {
	char* cbuf;
	struct mmo_charstatus* c;
	unsigned int ulen, clen, retval;

	ulen = RFIFOL(fd, 2) - 6;
	clen = sizeof(struct mmo_charstatus);

	CALLOC(cbuf, char, clen);

	retval=uncompress(cbuf, &clen, RFIFOP(fd, 6), ulen);
	c = (struct mmo_charstatus*)cbuf;

	if(retval == Z_OK && mmo_char_todb(c))
		printf("Saved %s\n", c->name);
	else if (retval == Z_MEM_ERROR)
		printf("Not enough memory\n");
	else if (retval == Z_BUF_ERROR)
		printf("Buffer too small\n");
	else if (retval == Z_DATA_ERROR)
		printf("Corrupt or Incorrect Data\n");
	
	FREE(cbuf);
	c = NULL;

	return 0;
}
///Save and Logout character
int charif_parse_savecharlog(int fd) {
	char* cbuf;
	struct mmo_charstatus* c;
	unsigned int ulen, clen, retval;

	ulen = RFIFOL(fd, 2) - 6;
	clen = sizeof(struct mmo_charstatus);

	CALLOC(cbuf, char, clen);

	retval=uncompress(cbuf, &clen, RFIFOP(fd, 6), ulen);
	c = (struct mmo_charstatus*)cbuf;
	
	if(retval == Z_OK && mmo_char_todb(c))
		printf("Saved %s\n", c->name);
	else if (retval == Z_MEM_ERROR)
		printf("Not enough memory\n");
	else if (retval == Z_BUF_ERROR)
		printf("Buffer too small\n");
	else if (retval == Z_DATA_ERROR)
		printf("Corrupt or Incorrect Data\n");
	
	FREE(cbuf);
	c = NULL;

	return 0;
}

int charif_parse(int fd) {
	int id,i;
	unsigned int j;
	int cmd=0, packet_len;

	if (fd != char_fd) return 0;
	
	if(session[fd]->eof) {
		add_log("Char Server connection lost.\n");
		printf("Char Server connection lost.\n");
		char_fd = 0;
		session_eof(fd);
		return 0;
	}

	cmd = RFIFOW(fd,0);

	if(cmd<0x4000 || cmd>=0x4000+(sizeof(packet_len_table)/sizeof(packet_len_table[0])) ||
	   packet_len_table[cmd-0x4000]==0){
	   	return 0;
	}

	packet_len = packet_len_table[cmd-0x4000];

	if(packet_len==-1){
		if(RFIFOREST(fd)<6)
			return 2;
		packet_len = RFIFOL(fd,2);
	}
	if((int)RFIFOREST(fd)<packet_len){
		return 2;
	}

	switch (cmd) {
	case 0x4001: charif_parse_savecharlog(fd); break;
	case 0x4002: charif_parse_savechar(fd); break;
	default:	break;
	}

	RFIFOSKIP(fd, packet_len);
	return 0;
}
