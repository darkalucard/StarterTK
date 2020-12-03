
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "socket.h"
#include "char.h"
#include "saveif.h"
#include "mmo.h"
#include "crypt.h"
#include "char_db.h"

static const int packet_len_table[] = {3, 0};

int check_connect_save(int ip, int port) {
	if (save_fd <= 0 || session[save_fd] == NULL) {
		printf("Attempt to connect to save-server...\n");
		save_fd = make_connection(ip, port);
		session[save_fd]->func_parse = saveif_parse;
		realloc_rfifo(save_fd, FIFOSIZE_SERVER, FIFOSIZE_SERVER);
		WFIFOHEAD(save_fd,66);
		WFIFOW(save_fd,0) = 0x4000;
		memcpy(WFIFOP(save_fd,2), save_id, 32);
		memcpy(WFIFOP(save_fd,34), save_pw, 32);
		WFIFOSET(save_fd, 66);
	}
	return 0;
}
int saveif_parse_accept(int fd) {
		if (RFIFOB(fd,2)) {
			printf("CFG_ERR: Username or password to connect Save Server is invalid!\n");
			add_log("CFG_ERR: Username or password to connect Save Server is invalid!\n");
		} else {
			printf("Connected to Save Server.\n");
			add_log("Connected to Save Server.\n");
		}
		return 0;
}
int saveif_parse(int fd) {
	int cmd,packet_len;
	
	if (fd != save_fd) return 0;
	
	if(session[fd]->eof) {
		//mmo_setallonline(0);
		printf("Can't connect to Save Server.\n");
		add_log("Can't connect to Save Server.\n");
		save_fd = 0;
		
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
	case 0x4000: saveif_parse_accept(fd); break;
		
	default:
		
		break;
	}

	RFIFOSKIP(fd, packet_len);
	return 0;
}
