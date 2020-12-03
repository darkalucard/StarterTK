
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "login.h"
#include "socket.h"
#include "mmo.h"
#include "crypt.h"
#include "intif.h"
#include "clif.h"
#include "timer.h"

static int packet_len_table[] = {69, 5, 5, 27, 5,0};

int intif_debug(int fd, int len) {
	int x;
	for(x=0;x<len;x++) {
		printf("%u ",RFIFOB(fd,x));
	}
	printf("\n");
	return 0;
}
int intif_auth(int fd) {
	int cmd,packet_len;

	if(char_fd > 0) {
		session_eof(fd);
		return 0;
	}
	cmd=RFIFOB(fd,3);
	packet_len=69;
	if(RFIFOREST(fd)<packet_len)
		return 0;

	if ((strcmp(RFIFOP(fd,5),login_id)) && (strcmp(RFIFOP(fd,37),login_pw))) {
		WFIFOHEAD(fd,3);
		WFIFOW(fd,0) = 0x1000;
		WFIFOB(fd,2) = 0x01;
		WFIFOSET(fd, 3);
		session[fd]->eof=1;
		return 0;
	}

	char_fd = fd;

	session[char_fd]->func_parse = intif_parse;
	realloc_rfifo(char_fd, FIFOSIZE_SERVER, FIFOSIZE_SERVER);
	WFIFOHEAD(char_fd,3);
	WFIFOW(char_fd,0) = 0x1000;
	WFIFOB(char_fd,2) = 0x00;
	WFIFOSET(char_fd, 3);

	add_log("Connection from Char Server accepted.\n");
	printf("Connection from Char Server accepted.\n");
	return 0;
}
int intif_parse_2001(int fd) {
	if (!session[RFIFOW(fd, 2)])
			return 0;

		if (RFIFOB(fd, 4)==0x01)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_USEREXIST]);
		else if (RFIFOB(fd, 4))
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_ERRDB]);
		else
			clif_message(RFIFOW(fd, 2), 0x00, "\x00");

		return 0;
}
int intif_parse_2002(int fd) {
	if (!session[RFIFOW(fd, 2)])
			return 0;

		if (RFIFOB(fd, 4) == 0x01)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_USEREXIST]);
		else if (RFIFOB(fd, 4))
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_ERRDB]);
		else
			clif_message(RFIFOW(fd, 2), 0x00, login_msg[LGN_NEWCHAR]);

		return 0;
}
int intif_parse_connectconfirm(int fd) {

	if (!session[RFIFOW(fd, 2)])
			return 0;

	struct login_session_data* sd=session[RFIFOW(fd,2)]->session_data;

		if (RFIFOB(fd, 4)==0) {
			Log_Add("validlogin","<%02d:%02d> User: %s IP: %u.%u.%u.%u\n",getHour(),getMinute(),sd->name,CONVIP(session[RFIFOW(fd,2)]->client_addr.sin_addr.s_addr));
			//printf("	Connection confirmed. Sending LOGIN OK.\n");
			WFIFOHEAD(RFIFOW(fd,2),8);
			WFIFOB(RFIFOW(fd, 2), 0) = '\xAA';
			WFIFOB(RFIFOW(fd, 2), 1) = '\x00';
			WFIFOB(RFIFOW(fd, 2), 2) = '\x05';
			WFIFOB(RFIFOW(fd, 2), 3) = '\x02';
			WFIFOB(RFIFOW(fd, 2), 4) = '\x17';
			WFIFOB(RFIFOW(fd, 2), 5) = '\x00';
			WFIFOB(RFIFOW(fd, 2), 6) = '\x00';
			WFIFOB(RFIFOW(fd, 2), 7) = '\x00';
			set_packet_indexes(WFIFOP(RFIFOW(fd, 2), 0));
			crypt(WFIFOP(RFIFOW(fd, 2), 0));
			WFIFOSET(RFIFOW(fd, 2), 8 + 3);
			int len;
			int newlen;
			char* thing;
			//intif_debug(fd,26);
			WFIFOHEAD(RFIFOW(fd,2),23+255);
			WFIFOB(RFIFOW(fd, 2), 0) = '\xAA';
			WFIFOB(RFIFOW(fd, 2), 3) = '\x03';
			WFIFOL(RFIFOW(fd, 2), 4) = SWAP32(RFIFOL(fd, 21));
			WFIFOW(RFIFOW(fd, 2), 8) = SWAP16(RFIFOW(fd, 25));
			//printf("Connecting to Number %u\n",RFIFOL(fd,20));
			len = strlen(RFIFOP(fd, 5))+16;
			thing=RFIFOP(fd,5);
			WFIFOB(RFIFOW(fd, 2), 10) = len;

			WFIFOW(RFIFOW(fd, 2), 11) = SWAP16(9);



			strcpy(WFIFOP(RFIFOW(fd,2),13),"KruIn7inc");
			WFIFOB(RFIFOW(fd, 2), 22)=strlen(thing);
			strcpy(WFIFOP(RFIFOW(fd,2),23),thing);
			WFIFOL(RFIFOW(fd, 2), 23+strlen(thing))=SWAP32(RFIFOW(fd,2));

			//memcpy(WFIFOP(fd, 11), RFIFOP(fd, 4), len+1);
			WFIFOW(RFIFOW(fd, 2), 1) = SWAP16(len+8);
			set_packet_indexes(WFIFOP(RFIFOW(fd, 2), 0));
			WFIFOSET(RFIFOW(fd, 2), 11+len + 3);
		} else if (RFIFOB(fd, 4) == 0x01)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_ERRDB]);
		else if (RFIFOB(fd, 4) == 0x02)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_WRONGUSER]);
		else if (RFIFOB(fd, 4) == 0x03) {
			Log_Add("invalidlogin","<%02d:%02d> Login:%s IP:%u.%u.%u.%u Password used:%s\n",getHour(),getMinute(),sd->name,CONVIP(session[RFIFOW(fd,2)]->client_addr.sin_addr.s_addr),sd->pass);
			if(setInvalidCount(session[RFIFOW(fd,2)]->client_addr.sin_addr.s_addr)>=10) {
				add_ip_lockout(session[RFIFOW(fd,2)]->client_addr.sin_addr.s_addr);
				session[RFIFOW(fd,2)]->eof=1;
			}
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_WRONGPASS]);
		}
		else if (RFIFOB(fd, 4) == 0x04)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_BANNED]);
		else if (RFIFOB(fd, 4) == 0x05)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_ERRSERVER]);
		else if (RFIFOB(fd, 4) == 0x06)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_DBLLOGIN]);
		else
			printf("Serious error!\n");



	return 0;
}
int intif_parse_changepass(int fd) {
	if (!session[RFIFOW(fd, 2)])
			return 0;

		if (!RFIFOB(fd, 4)) {
			Log_Add("validchange","<%d:%d> IP: %u.%u.%u.%u\n",getHour(),getMinute(),CONVIP(session[RFIFOW(fd,2)]->client_addr.sin_addr.s_addr));
			clif_message(RFIFOW(fd, 2), 0x00, login_msg[LGN_CHGPASS]);
		} else if (RFIFOB(fd, 4) == 0x01)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_ERRDB]);
		else if (RFIFOB(fd, 4) == 0x02)
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_WRONGUSER]);
		else if (RFIFOB(fd, 4) == 0x03) {
			Log_Add("invalidchange","<%02d:%02d> IP:%u.%u.%u.%u\n",getHour(),getMinute(),CONVIP(session[RFIFOW(fd,2)]->client_addr.sin_addr.s_addr));
			if(setInvalidCount(session[RFIFOW(fd,2)]->client_addr.sin_addr.s_addr)>=10) {
				add_ip_lockout(session[RFIFOW(fd,2)]->client_addr.sin_addr.s_addr);
				session[RFIFOW(fd,2)]->eof=1;
			}
			clif_message(RFIFOW(fd, 2), 0x03, login_msg[LGN_WRONGPASS]);
		}
		return 0;

	}
int intif_parse(int fd) {
	int cmd,packet_len;
	if(session[fd]->eof) {
		add_log("Char Server connection lost.\n");
		printf("Char Server connection lost.\n");
		char_fd = 0;
		session_eof(fd);
		return 0;
	}

	if(RFIFOB(fd,0) == 0xAA) {
		int len = SWAP16(RFIFOW(fd, 1)) + 3;
		if(len <= RFIFOREST(fd))
			RFIFOSKIP(fd, len);
		return 0;
	}

	cmd = RFIFOW(fd,0);

	if(cmd<0x2000 || cmd>=0x2000+(sizeof(packet_len_table)/sizeof(packet_len_table[0])) ||
	   packet_len_table[cmd-0x2000]==0){
	   	return 0;
	}

	packet_len = packet_len_table[cmd-0x2000];

	if(packet_len==-1){
		if(RFIFOREST(fd)<6)
			return 2;
		packet_len = RFIFOL(fd,2);
	}
	if((int)RFIFOREST(fd)<packet_len){
		return 2;
	}
	//printf("LOGIN: %d\n",cmd);
	switch (cmd) {
	case 0x2001: intif_parse_2001(fd); break;
	case 0x2002: intif_parse_2002(fd); break;
	case 0x2003: intif_parse_connectconfirm(fd); break;
	case 0x2004: intif_parse_changepass(fd); break;

	default: break;
	}

	RFIFOSKIP(fd, packet_len);
	return 0;
}

