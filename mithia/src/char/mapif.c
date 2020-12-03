
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "socket.h"
#include "char.h"
#include "mapif.h"
#include "mmo.h"
#include "malloc.h"
#include "char_db.h"
#include "zlib.h"
#include "db_mysql.h"
#include "strlib.h"

static const int packet_len_table[] = {
	72, -1, 20, 24, -1, 6, 0, -1, 28 , sizeof(struct board_show_0)+2, sizeof(struct boards_read_post_0)+2 , 4 , sizeof(struct boards_post_0)+2 , 4124 , 20 , 0,  //0x3000 - 0x300F
	0 }; //NULL terminator



int mapif_send(int fd, unsigned char* buf, int len, int type) {
	int i;

	switch(type) {
	case ALL:
		for(i = 0; i < map_fifo_max; i++) {
			if (map_fifo[i].fd > 0) {
				WFIFOHEAD(map_fifo[i].fd,len);
				memcpy(WFIFOP(map_fifo[i].fd,0), buf, len);
				WFIFOSET(map_fifo[i].fd,len);
			}
		}
		break;
	case ALLWOS:
		for(i = 0; i < map_fifo_max; i++) {
			if ((map_fifo[i].fd > 0) && (map_fifo[i].fd != fd)) {
				WFIFOHEAD(map_fifo[i].fd,len);
				memcpy(WFIFOP(map_fifo[i].fd,0), buf, len);
				WFIFOSET(map_fifo[i].fd,len);
			}
		}
		break;
	}

	return 0;
}

int mapif_send_newmp(int fd, unsigned int id, unsigned int flags) {
	char data[8];

	WBUFW(data,0)=0x380D;
	WBUFL(data,2)=id;
	WBUFW(data,6)=flags;

	mapif_send(fd,data,8,ALL); //Send to all servers.
	return 0;
}
int mapif_parse_auth(int fd) {
	int cmd, packet_len;

	//if(!packet_len_table[9]) packet_len_table[9]=; //0x3009

	if(session[fd]->eof) {

		session_eof(fd);
		return 0;
	}

	cmd = RFIFOW(fd,0);

	if(cmd<0x3000 || cmd>=0x3000+(sizeof(packet_len_table)/sizeof(packet_len_table[0])) ||
	   packet_len_table[cmd-0x3000]==0){
	   	return 0;
	}

	packet_len = packet_len_table[cmd-0x3000];

	if(packet_len==-1){
		if(RFIFOREST(fd)<6)
			return 2;
		packet_len = RFIFOL(fd,2);
	}
	if((int)RFIFOREST(fd)<packet_len){
		return 2;
	}

	if(cmd==0x3000) {
		if ((strcmp(RFIFOP(fd,2),char_id)) && (strcmp(RFIFOP(fd,34),char_pw))) {
			WFIFOHEAD(fd,3);
			WFIFOW(fd,0) = 0x3800;
			WFIFOB(fd,2) = 0x01;
			WFIFOSET(fd, 3);
			session[fd]->eof = 1;
			RFIFOSKIP(fd, packet_len);
			return 0;
		}

		int i;

		for(i=0;i < map_fifo_max;i++) {
			if(!map_fifo[i].fd)
				break;
		}

		if(i>=map_fifo_max)
			map_fifo_max++;

		map_fifo_n++;
		map_fifo[i].fd = fd;
		map_fifo[i].ip = RFIFOL(fd, 66);
		map_fifo[i].port = RFIFOW(fd, 70);
		session[fd]->func_parse = mapif_parse;
		realloc_rfifo(fd, FIFOSIZE_SERVER, FIFOSIZE_SERVER);

		WFIFOHEAD(fd,3);
		WFIFOW(fd,0) = 0x3800;
		WFIFOB(fd,2) = 0x00;
		WFIFOSET(fd, 3);

		unsigned char *p;
		p=(unsigned char*)&map_fifo[i].ip;
		printf("Map Server #%d connected.(%u.%u.%u.%u:%d).\n", i, p[0], p[1], p[2], p[3], map_fifo[i].port);
		add_log("Map Server #%d connected(%u.%u.%u.%u:%d).\n", i, p[0], p[1], p[2], p[3], map_fifo[i].port);



	} else {
		session[fd]->eof = 1;
	}

	RFIFOSKIP(fd, packet_len);
	return 0;
}

///Parse Map Info data from Map server
int mapif_parse_mapset(int fd,int id) {
		//int id;
		int i,j;
		map_fifo[id].map_n = RFIFOW(fd, 6);
		for(i=0;i<map_fifo[id].map_n;i++)
			map_fifo[id].map[i] = RFIFOW(fd, i*2+8);

		{

			int j,k;

			//get other map server presence
			for(i=0;i<map_fifo_max;i++) {
				if ((i != id) && (map_fifo[i].fd > 0) && (map_fifo[i].map_n > 0)) {
					for(j=0;j<map_fifo[i].map_n;j++) {
						//WFIFOW(fd, j*2+9) = map_fifo[i].map[j];
						for(k=0;(k<map_fifo[id].map_n) && (k<map_fifo[i].map_n);k++) {
							if (map_fifo[i].map[j] == map_fifo[id].map[k]) {
								//found same map, just report and let admin fix it...
								printf("MAPERR: Same map number(#%d) at server #%d and server #%d\n", map_fifo[id].map[k], id, i);
								add_log("MAPERR: Same map number(#%d) at server #%d and server #%d\n", map_fifo[id].map[k], id, i);
							}
						}
					}

				}
			}
			//done!
		}

		printf("Map Server #%d(%d map) is \033[1;32mready\033[0m!\n", id, map_fifo[id].map_n);
		add_log("Map Server #%d send %d map.\n", id, map_fifo[id].map_n, id);

		return 0;
}

///Parse Login Request
int mapif_parse_login(int fd, int id) {

		WFIFOHEAD(login_fd,27);
		WFIFOW(login_fd, 0) = 0x2003;
		WFIFOW(login_fd, 2) = RFIFOW(fd, 2);
		WFIFOB(login_fd, 4) = 0x00;
		memset(WFIFOP(login_fd, 5), 0, 16);
		memcpy(WFIFOP(login_fd, 5), RFIFOP(fd, 4), 16);
		WFIFOL(login_fd, 21) = map_fifo[id].ip;
		WFIFOW(login_fd, 25) = map_fifo[id].port;
		WFIFOSET(login_fd, 27);
		return 0;
}

///Parse Request of Character to Map Server
int mapif_parse_requestchar(int fd) {
		unsigned int ulen,clen,retval;
		char* cbuf;
		ulen=sizeof(struct mmo_charstatus);
		clen=compressBound(ulen);
		memset(char_dat, 0, sizeof(struct mmo_charstatus));
		mmo_char_fromdb(RFIFOL(fd, 4), char_dat, RFIFOP(fd, 8));


		CALLOC(cbuf,char,clen);
		retval=compress(cbuf,&clen,(char*)char_dat,ulen);

		if(retval) {
			FREE(cbuf);
			return 0;
		}

		WFIFOHEAD(fd,clen+8);
		WFIFOW(fd, 0) = 0x3803;
		WFIFOL(fd, 2)= clen+8;
		WFIFOW(fd, 6) = RFIFOW(fd, 2);

		memcpy(WFIFOP(fd,8),cbuf,clen);
		WFIFOSET(fd, clen+8);
		FREE(cbuf);

		return 0;
}
int mapif_parse_savechar(int fd) {
	//char* cbuf;
	//struct mmo_charstatus* c;
	unsigned int ulen, clen, retval;

	ulen = RFIFOL(fd, 2) - 6;
	WFIFOHEAD(char_fd, ulen + 6);
	WFIFOW(save_fd, 0) = 0x4002;
	WFIFOL(save_fd, 2) = ulen + 6;
	memcpy(WFIFOP(save_fd, 6), RFIFOP(fd, 6), ulen);
	WFIFOSET(save_fd, ulen + 6);
	//clen = sizeof(struct mmo_charstatus);

	//CALLOC(cbuf, char, clen);
	//CALLOC(c, struct mmo_charstatus, 1);

	//retval=uncompress(cbuf, &clen, RFIFOP(fd, 6), ulen);
	//c = (struct mmo_charstatus*)cbuf;

	//if(retval == Z_OK && mmo_char_todb(c))
	//	printf("Saved %s\n", c->name);
	//else if (retval == Z_MEM_ERROR)
	//	printf("Not enough memory for %s\n", c->name);
	//else if (retval == Z_BUF_ERROR)
	//	printf("Buffer too small for %s\n", c->name);
	//else if (retval == Z_DATA_ERROR)
	//	printf("Corrupt or Incorrect Data for %s\n", c->name);
	
	//FREE(cbuf);

	return 0;
}
///Parse Map Logout(non-existant atm)
int mapif_parse_logout(int fd) {
	logindata_del(RFIFOL(fd, 2));
	return 0;
}
///parse Inter-server whispers(disabled atm)
int mapif_parse_whisp(int fd) {
	return 0;
	int j;
	if (RFIFOREST(fd) < RFIFOW(fd, 33)+35)
			return 0;

		j=char_db_idfromname(RFIFOP(fd, 17));
		if (!j) {
			WFIFOHEAD(fd,36);
			WFIFOB(fd, 0) = 0x06;
			WFIFOB(fd, 1) = 0x02;
			memcpy(WFIFOP(fd, 2), RFIFOP(fd, 1), 32);
			WFIFOW(fd, 34) = 0x00;
			WFIFOSET(fd, 36);
		} else {
			struct char_login_data *dat;
			dat = logindata_search(j);
			if (!dat) {
				WFIFOHEAD(fd,36);
				WFIFOB(fd, 0) = 0x06;
				WFIFOB(fd, 1) = 0x02;
				memcpy(WFIFOP(fd, 2), RFIFOP(fd, 1), 32);
				WFIFOW(fd, 34) = 0x00;
				WFIFOSET(fd, 36);
			} else {
				//Fix nickname, an easy way is change from RFIFO
				WFIFOHEAD(fd,RFIFOW(fd,33)+36);
				WFIFOHEAD(map_fifo[dat->map_server].fd,RFIFOW(fd,33)+36);
				memcpy(RFIFOP(fd, 17), dat->name, 16);
				WFIFOB(map_fifo[dat->map_server].fd, 0) = 0x06;
				WFIFOB(map_fifo[dat->map_server].fd, 1) = 0x00;
				memcpy(WFIFOP(map_fifo[dat->map_server].fd, 2), RFIFOP(fd, 1), RFIFOW(fd, 33)+34);
				WFIFOSET(map_fifo[dat->map_server].fd, RFIFOW(fd, 33)+36);
				WFIFOB(fd, 0) = 0x06;
				WFIFOB(fd, 1) = 0x01;
				memcpy(WFIFOP(fd, 2), RFIFOP(fd, 1), RFIFOW(fd, 33)+34);
				WFIFOSET(fd, RFIFOW(fd, 33)+36);
			}
		}
		RFIFOSKIP(fd, RFIFOW(fd, 33)+36);
		return 0;
}

///Save and Logout character
int mapif_parse_savecharlog(int fd) {
	//char* cbuf;
	//struct mmo_charstatus* c;
	unsigned int ulen, clen, retval;

	ulen = RFIFOL(fd, 2) - 6;
	WFIFOHEAD(char_fd, ulen + 6);
	WFIFOW(save_fd, 0) = 0x4002;
	WFIFOL(save_fd, 2) = ulen + 6;
	memcpy(WFIFOP(save_fd, 6), RFIFOP(fd, 6), ulen);
	WFIFOSET(save_fd, ulen + 6);
	//clen = sizeof(struct mmo_charstatus);

	//CALLOC(cbuf, char, clen);
	//CALLOC(c, struct mmo_charstatus, 1);

	//retval=uncompress(cbuf, &clen, RFIFOP(fd, 6), ulen);
	//c = (struct mmo_charstatus*)cbuf;
	
	//if(retval == Z_OK && mmo_char_todb(c))
	//	printf("Saved %s\n", c->name);
	//else if (retval == Z_MEM_ERROR)
	//	printf("Not enough memory for %s\n", c->name);
	//else if (retval == Z_BUF_ERROR)
	//	printf("Buffer too small for %s\n", c->name);
	//else if (retval == Z_DATA_ERROR)
	//	printf("Corrupt or Incorrect Data for %s\n", c->name);
	
	//FREE(cbuf);

	return 0;
}
///Delete From Nmail
int mapif_nmail_delete(int fd, char* name, int post) {

	if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Mail` SET `MalDeleted` = '1', `MalNew` = '0' WHERE `MalChaNameDestination` = '%s' AND `MalPosition` = '%d'", name, post))
		{
			WFIFOB(fd,4)=0x02;
			WFIFOSET(fd,5);
			Sql_ShowDebug(sql_handle);
			return 0;
		}

		WFIFOB(fd,4)=0x00;
		WFIFOSET(fd,5);
		return 0;

}
int mapif_parse_deletepost(int fd) {
	int sfd=RFIFOW(fd,2);
	int gmLevel=RFIFOW(fd,4);
	int canDelete=RFIFOW(fd,6);
	int board=RFIFOW(fd,8);
	int post=RFIFOW(fd,10);
	char name[16];


	memcpy(name,RFIFOP(fd,12),16);

	WFIFOHEAD(fd,5);
	WFIFOW(fd,0)=0x3808;
	WFIFOW(fd,2)=sfd;

	if(!board) { mapif_nmail_delete(fd,name,post); return 0; } //Handling NMAIL




	if(gmLevel>=50 || canDelete) {
		if(SQL_ERROR==Sql_Query(sql_handle,"DELETE FROM `Boards` WHERE `BrdBnmId` = %d AND `BrdPosition` = %d",board,post))
		{	Sql_ShowDebug(sql_handle); WFIFOB(fd,4)=0x02; }
		else
			WFIFOB(fd,4)=0x00;



	} else {
		if(SQL_ERROR == Sql_Query(sql_handle,"SELECT * FROM `Boards` WHERE `BrdBnmId` = %d AND `BrdPosition` = %d AND `BrdChaName` = '%s'",board,post,name)
		)
		{
			Sql_ShowDebug(sql_handle);
			WFIFOB(fd,4)=0x02;
			WFIFOSET(fd,5);
			return 0;
		}

		if(SQL_SUCCESS != Sql_NextRow(sql_handle)) {
			WFIFOB(fd,4)=0x01;

		} else {

			if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `Boards` WHERE `BrdBnmId` = %d AND `BrdPosition` = %d",board,post))
			{
				Sql_ShowDebug(sql_handle);
				WFIFOB(fd,4)=0x02;
			} else
				WFIFOB(fd,4)=0x00;


		}
	}

	Sql_FreeResult(sql_handle);
	WFIFOSET(fd,5);
	return 0;
}

int mapif_parse_showposts(int fd) {
	struct board_show_0 a;
	struct boards_show_header_1 board_header;
	struct boards_show_array_1 post;
	int x;
	int numRows=0;
	int len=0;
	SqlStmt* stmt;
	StringBuf buf;


	//Set all Variables to 0
	memset(&a,0,sizeof(struct board_show_0));
	memset(&board_header,0,sizeof(struct boards_show_header_1));
	memset(&post, 0, sizeof(struct boards_show_array_1));

	//Copy Header
	memcpy(&a, RFIFOP(fd,2),sizeof(struct board_show_0));

	//Copy Name and Variables to Header
	memcpy(board_header.name, a.name, 16);
	board_header.fd = a.fd;
	board_header.array = 0;
	board_header.board=a.board;
	//printf("BCount %s (%d)\n",a.name,a.bcount);
	//Make sure Enough Space for Header
	WFIFOHEAD(fd,sizeof(struct boards_show_header_1)+6);
	WFIFOW(fd,0)=0x3809;
	//Set Header Flags
	if(a.popup && a.board) {
	if(!(a.flags&BOARD_CAN_WRITE)) {
			//WFIFOB(fd,10)=0; //Can't write
			board_header.flags1 = 0;
		} else {
			board_header.flags1 = 2;
		}
	} else {
		if(!(a.flags&BOARD_CAN_WRITE)) {
			board_header.flags1 = 1;
			//WFIFOB(fd,10)=1; //Can't write
		} else {
			board_header.flags1 = 3;
			//WFIFOB(fd,10)=3; //Can write
		}
	}

	//Set Header Flags (Alternate)

	if(a.board == 0)
		board_header.flags2 = 4;

	else
		board_header.flags2 = 2;


	//Allocate SQL Handle
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) {

		//SQL Handle Not Allocated, Send Header Only
		memcpy(WFIFOP(fd,6),&board_header,sizeof(struct boards_show_header_1));

		WFIFOL(fd,2)=sizeof(struct boards_show_header_1)+6;
		WFIFOSET(fd,sizeof(struct boards_show_header_1)+6);
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	//Initialize SQL Statement String
	StringBuf_Init(&buf);

	//Choose Correctly Board or NMAIL
	if(a.board == 0)  //NMail, Use Appropriate Statement
		StringBuf_Printf(&buf,"SELECT `MalNew`, `MalChaName`, `MalSubject`, `MalPosition`, `MalMonth`, `MalDay`, `MalId` FROM `Mail` WHERE `MalChaNameDestination` = '%s' AND `MalDeleted` = 0 ORDER BY `MalPosition` DESC LIMIT %d,20",a.name,a.bcount*20);
	else //Board, Use Appropriate Statement
		StringBuf_Printf(&buf,"SELECT `BrdHighlighted`, `BrdChaName`, `BrdTopic`, `BrdPosition`, `BrdMonth`, `BrdDay`, `BrdBtlId` FROM `Boards` WHERE `BrdBnmId` = %d ORDER BY `BrdPosition` DESC LIMIT %d,20",a.board,a.bcount*20);

	//Make SQL Statement
	if(SQL_ERROR == SqlStmt_Prepare(stmt,StringBuf_Value(&buf))
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &post.color, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_STRING, &post.user, sizeof(post.user), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_STRING, &post.topic, sizeof(post.topic), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_INT, &post.post_id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4, SQLDT_INT, &post.month, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_INT, &post.day, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6, SQLDT_INT, &post.board_name, 0, NULL, NULL)
	)
	{
		//There was an Error, Destroy and send Header ONLY
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		StringBuf_Destroy(&buf);

		memcpy(WFIFOP(fd,6),&board_header,sizeof(struct boards_show_header_1));
		WFIFOL(fd,2)=sizeof(struct boards_show_header_1)+6;
		WFIFOSET(fd,sizeof(struct boards_show_header_1)+6);
		return 0;
	}

	numRows=SqlStmt_NumRows(stmt);
	//CALLOC(posts,struct board_show_array_1,numRows);

	if(numRows<=0) { //NO results, do not show
		memcpy(WFIFOP(fd,6),&board_header,sizeof(struct boards_show_header_1));
		WFIFOL(fd,2)=sizeof(struct boards_show_header_1)+6;
		WFIFOSET(fd,sizeof(struct boards_show_header_1)+6);

		SqlStmt_Free(stmt);
		StringBuf_Destroy(&buf);
		return 0;
	}

	numRows=SqlStmt_NumRows(stmt);

	WFIFOHEAD(fd,sizeof(board_header)+sizeof(post)*numRows+6); //Make sure enough space for header + Posts
	board_header.array=numRows;

	for(x=0;x<numRows && SQL_SUCCESS==SqlStmt_NextRow(stmt);x++) {
		memcpy(WFIFOP(fd,len+sizeof(struct boards_show_header_1)+6),&post,sizeof(post));
		len+=sizeof(post);
	}

	memcpy(WFIFOP(fd,6),&board_header,sizeof(board_header));

	WFIFOL(fd,2)=(sizeof(board_header)+sizeof(post)*numRows+6);

	WFIFOSET(fd,(sizeof(board_header)+sizeof(post)*numRows+6));
	SqlStmt_Free(stmt);
	StringBuf_Destroy(&buf);
	return 0;

}
int mapif_parse_userlist(int fd) {
	int sfd = RFIFOW(fd, 2);
	int class, mark, rank, clan, x;
	unsigned int nation = 0;
	char name[32];

	memset(name, 0, 32);

	SqlStmt* stmt;

	stmt = SqlStmt_Malloc(sql_handle);
	if (stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	WFIFOHEAD(fd, 65535);
	WFIFOW(fd, 0) = 0x380A; ///Change this Number
	WFIFOL(fd, 2) = 10;
	WFIFOW(fd, 6) = sfd;
	WFIFOW(fd, 8) = 0; //This is where the User count goes.

	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ChaPthId`, `ChaMark`, `ChaClnId`, `ChaName`, `ChaNation` FROM `Character` WHERE `ChaOnline` = 1 AND `ChaHeroes` = 1  GROUP BY `ChaName`, `ChaId` ORDER BY `ChaMark` DESC, `ChaLevel` DESC, SUM((`ChaMaximumMana`*2) + `ChaMaximumVita`) DESC, `ChaId` ASC")
		|| SQL_ERROR == SqlStmt_Execute(stmt)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &class, 0, NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1, SQLDT_INT, &mark, 0, NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2, SQLDT_INT, &clan, 0, NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3, SQLDT_STRING, &name, sizeof(name), NULL, NULL)
		|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5, SQLDT_UINT, &nation, 0, NULL, NULL))
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		WFIFOSET(fd, 10);
		return 0;
	}

	WFIFOW(fd, 8) = SqlStmt_NumRows(stmt);

	for (x = 0; x < WFIFOW(fd, 8) && SQL_SUCCESS == SqlStmt_NextRow(stmt); x++)
	{
		WFIFOW(fd, x * 22 + 10) = 0;
		WFIFOW(fd, x * 22 + 12) = class;
		WFIFOW(fd, x * 22 + 14) = mark;
		WFIFOW(fd, x * 22 + 16) = clan;
		WFIFOW(fd, x * 22 + 18) = nation;
		memcpy(WFIFOP(fd, x * 22 + 20), name, 16);
	}

	SqlStmt_Free(stmt);

	WFIFOL(fd, 2) = WFIFOW(fd, 8) * 22 + 36;
	WFIFOSET(fd, WFIFOW(fd, 8) * 22 + 36);

	return 0;
}


int mapif_parse_readpost(int fd) {
	char* data;
	struct boards_read_post_0 header;
	struct boards_read_post_1 post;
	StringBuf buf;

	int max=0;

	memset(&header, 0, sizeof(header));
	memset(&post, 0, sizeof(post));

	memcpy(&header, RFIFOP(fd,2),sizeof(header));





	post.fd=header.fd;
	post.board=header.board;

	size_t len;

	post.type=3;
	if(header.flags&BOARD_CAN_WRITE)
		post.buttons = 3;
	else
		post.buttons = 1;

	if(!header.board) {
		post.buttons = 3;
		post.type = 5;
	}

	memcpy(post.name,header.name,16);
	//memcpy(name,RFIFOP(fd,12),16); //name used for NMAIL
	//memset(msg,0,4000);
	//memset(writer,0,52);
	//memset(topic,0,52);

	StringBuf_Init(&buf);
	//StringBuf_Init(&buf2);
	//printf("Post: %d\n",post);
	if(header.board == 0)  //This is nmail
		StringBuf_Printf(&buf,"SELECT MAX(`MalPosition`) FROM `Mail` WHERE `MalChaNameDestination` = '%s' AND `MalDeleted` = '0'", header.name);

	 else
		StringBuf_Printf(&buf,"SELECT MAX(`BrdPosition`) FROM `Boards` WHERE `BrdBnmId` = '%d'", header.board);


	if(SQL_ERROR == Sql_Query(sql_handle,StringBuf_Value(&buf)))
	{
		Sql_ShowDebug(sql_handle);
		StringBuf_Destroy(&buf);
		return 0;
	}

	if(SQL_ERROR == Sql_NextRow(sql_handle))
	{
		Sql_ShowDebug(sql_handle);
		StringBuf_Destroy(&buf);
	return 0;
	}

	Sql_GetData(sql_handle,0,&data,0);

	if(!data)
		return 0;

	max=atoi(data);

	Sql_FreeResult(sql_handle);

	if(header.post>max)
		header.post = 1;
		
	StringBuf_Clear(&buf);
	if(header.board == 0)
		StringBuf_Printf(&buf,"SELECT `MalChaName`, `MalSubject`, `MalBody`, `MalPosition`, `MalMonth`, `MalDay`, `MalId` FROM `Mail` WHERE `MalChaNameDestination` = '%s' AND `MalPosition` >= %d AND `MalDeleted` = '0' ORDER BY `MalPosition` LIMIT 1",header.name,header.post);
	else
		StringBuf_Printf(&buf,"SELECT `BrdChaName`, `BrdTopic`, `BrdPost`, `BrdPosition`, `BrdMonth`, `BrdDay`, `BrdBtlId` FROM `Boards` WHERE `BrdBnmId` = %d AND `BrdPosition` >= %d ORDER BY `BrdPosition` LIMIT 1",header.board,header.post);

	if(SQL_ERROR == Sql_Query(sql_handle,StringBuf_Value(&buf)))
	{
		Sql_ShowDebug(sql_handle);
		StringBuf_Destroy(&buf);
		return 0;
	}

	if(SQL_SUCCESS != Sql_NextRow(sql_handle)) {
		Sql_ShowDebug(sql_handle);
		StringBuf_Destroy(&buf);
		return 0;
	}

	Sql_GetData(sql_handle,0,&data,&len); memcpy(post.user,data,len);
	Sql_GetData(sql_handle,1,&data,&len); memcpy(post.topic,data,len);
	Sql_GetData(sql_handle,2,&data,&len); memcpy(post.msg,data,len);
	Sql_GetData(sql_handle,3,&data,0); post.post=atoi(data);
	Sql_GetData(sql_handle,4,&data,0); post.month=atoi(data);
	Sql_GetData(sql_handle,5,&data,0); post.day=atoi(data);
	Sql_GetData(sql_handle,6,&data,0); post.board_name=atoi(data);
	
	Sql_FreeResult(sql_handle);

	WFIFOHEAD(fd,sizeof(post)+2);
	WFIFOW(fd,0)=0x380F;
	memcpy(WFIFOP(fd,2),&post,sizeof(post));
	WFIFOSET(fd,sizeof(post)+2);

	StringBuf_Destroy(&buf);

	if(header.board == 0) {
		Sql_Query(sql_handle,"UPDATE `Mail` SET `MalNew` = 0 WHERE `MalPosition` = %d AND `MalChaNameDestination` = '%s'",header.post,header.name);
		Sql_Query(sql_handle,"SELECT * FROM `Mail` WHERE `MalChaNameDestination` = '%s' AND `MalNew`=1",header.name);
		if(Sql_NumRows(sql_handle)<=0)
		{
			WFIFOHEAD(fd,6);
			WFIFOW(fd,0)=0x380E;
			WFIFOW(fd,2)=header.fd;
			WFIFOW(fd,4)=0;
			WFIFOSET(fd,6);
		}
	}
	return 0;
	//printf("Sent board info\n");
}
int mapif_parse_boardpost(int fd) {
	struct boards_post_0 header;
	int result=-1;
	int newPostID=1;
	char* data;
	char topic[104];
	char post[8000];

	memset(&header,0,sizeof(header));
	memcpy(&header,RFIFOP(fd,2),sizeof(header));
	//memcpy(name,RFIFOP(fd,8),16);
	Sql_EscapeString(sql_handle,topic,header.topic);
	Sql_EscapeString(sql_handle,post,header.post);

	WFIFOHEAD(fd,6);
	WFIFOW(fd,0)=0x380B; //Change
	WFIFOW(fd,2)=header.fd;

	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT MAX(`BrdPosition`) FROM `Boards` WHERE `BrdBnmId` = '%d'", header.board))
	{
		Sql_ShowDebug(sql_handle);
		WFIFOW(fd,4)=1; //1 == SQL_ERROR
		WFIFOSET(fd,6);
		return 0;
	}
	result=Sql_NextRow(sql_handle);
	if(SQL_ERROR == result)
	{
		Sql_ShowDebug(sql_handle);
		WFIFOW(fd,4)=1;
		WFIFOSET(fd,6);
		return 0;
	} else if(SQL_SUCCESS == result) {
		Sql_GetData(sql_handle,0,&data,0);
		if(data) newPostID=atoi(data)+1;
		Sql_FreeResult(sql_handle);
	}

	if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Boards`"
		"(`BrdBnmId`, `BrdHighlighted`, `BrdChaName`, `BrdTopic`, `BrdPost`, `BrdPosition`, `BrdMonth`, `BrdDay`, `BrdBtlId`)"
		"VALUES('%d','%d','%s','%s','%s','%d',DATE_FORMAT(CURDATE(),'%%m'),DATE_FORMAT(CURDATE(),'%%d'),'%d')",header.board,0,header.name,topic,post,newPostID,header.nval))
		{
			Sql_ShowDebug(sql_handle);
			WFIFOW(fd,4)=1;
			WFIFOSET(fd,6);
			return 0;
		}

	WFIFOW(fd,4)=0x00;
	WFIFOSET(fd,6);

	return 0;
}
int mapif_parse_nmailwrite(int fd) {
	int sfd=RFIFOW(fd,2);
	char from[32];
	char to[104];
	char topic[104];
	char msg[8000];
	int newMailID=1;
	unsigned int toID=0;
	char* data;
	int result;

	Sql_EscapeString(sql_handle,from,RFIFOP(fd,4));
	Sql_EscapeString(sql_handle,to,RFIFOP(fd,20));
	Sql_EscapeString(sql_handle,topic,RFIFOP(fd,72));
	Sql_EscapeString(sql_handle,msg,RFIFOP(fd,124));

	WFIFOHEAD(fd,6);
	WFIFOW(fd,0)=0x380C;
	WFIFOW(fd,2)=sfd;
	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT `ChaId` FROM `Character` WHERE `ChaName` = '%s'",to))
	{
		Sql_ShowDebug(sql_handle);
		WFIFOW(fd,6)=1;
		WFIFOSET(fd,6);
		return 0;
	}

	if(SQL_SUCCESS != Sql_NextRow(sql_handle))
	{
		WFIFOW(fd,6)=2;
		WFIFOSET(fd,6);
		Sql_FreeResult(sql_handle);
		return 0;
	}

	Sql_GetData(sql_handle,0,&data,0); toID=(unsigned int)strtoul(data,NULL,10);

	Sql_FreeResult(sql_handle);

	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT MAX(`MalPosition`) FROM `Mail` WHERE `MalChaNameDestination` = '%s'",to))
	{
		Sql_ShowDebug(sql_handle);
		WFIFOW(fd,6)=1; //Sql_Error
		WFIFOSET(fd,6);
		return 0;
	}

	result=Sql_NextRow(sql_handle);
	if(result == SQL_ERROR) {
		Sql_ShowDebug(sql_handle);
		WFIFOW(fd,6)=1;
		WFIFOSET(fd,6);
		return 0;
	}

	if(SQL_SUCCESS==result) Sql_GetData(sql_handle,0,&data,0);

		if(data) {
printf("max: %d, new: %d\n", atoi(data), newMailID);
			newMailID=atoi(data)+1;
		}

	Sql_FreeResult(sql_handle);
	if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Mail` (`MalChaName`, `MalChaNameDestination`, `MalPosition`, `MalSubject`, `MalBody`, `MalMonth`, `MalDay`, `MalNew`) VALUES('%s', '%s', %d, '%s', '%s', DATE_FORMAT(CURDATE(),'%%m'), DATE_FORMAT(CURDATE(), '%%d'), 1)", from, to, newMailID, topic, msg))
	{
		Sql_ShowDebug(sql_handle);
		WFIFOW(fd,6)=1;
		WFIFOSET(fd,6);
		return 0;
	}

	WFIFOW(fd,6)=0;
	WFIFOSET(fd,6);
	mapif_send_newmp(fd,toID,FLAG_MAIL);
	return 0;
}
int mapif_parse_findnewmp(int fd) {
	/*int sfd=RFIFOW(fd,2);
	char name[32];
	int flags=0;
	memset(name,0,32);

	Sql_EscapeString(sql_handle,name,RFIFOP(fd,4));

	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT `new` FROM `nmail` WHERE `new`=1 AND `touser`='%s'",name))
	{
		Sql_ShowDebug(sql_handle);
		return 0;
	}

	if(Sql_NumRows(sql_handle)>0)
			flags |= FLAG_MAIL;

	Sql_FreeResult(sql_handle);

	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT `item` FROM `parcel` WHERE `user`='%s'",name))
	{
		Sql_ShowDebug(sql_handle);
		return 0;
	}

	if(Sql_NumRows(sql_handle)>0)
		flags |= FLAG_PARCEL;

	Sql_FreeResult(sql_handle);

	WFIFOHEAD(fd,6);
	WFIFOW(fd,0)=0x380E;
	WFIFOW(fd,2)=sfd;
	WFIFOW(fd,4)=flags;
	WFIFOSET(fd,6);
	return 0;*/
}
int mapif_parse(int fd) {
	int id,i;
	unsigned int j;
	int cmd=0, packet_len;
	for(id=0;id<map_fifo_max;id++) {
		if(map_fifo[id].fd == fd)
			break;
	}

	if(session[fd]->eof) {
		add_log("Map Server #%d connection lost.\n",id);
		printf("Map Server #%d connection lost.\n",id);
		map_fifo[id].fd = 0;
		map_fifo_n--;
		if(id==(map_fifo_max-1)) {
			for(i=map_fifo_max-1;i>=0;i--) {
				if(!map_fifo[id].fd)
					map_fifo_max--;
				else
					break;
			}
		}
		//printf("Map FIFO Server Connected: %d, FIFO Cache: %d\n", map_fifo_n, map_fifo_max);
		mmo_setallonline(0);
		session_eof(fd);
		return 0;
	}


	cmd = RFIFOW(fd,0);

	if(cmd<0x3000 || cmd>=0x3000+(sizeof(packet_len_table)/sizeof(packet_len_table[0])) ||
	   packet_len_table[cmd-0x3000]==0){
	   	return 0;
	}

	packet_len = packet_len_table[cmd-0x3000];

	if(packet_len==-1){
		if(RFIFOREST(fd)<6)
			return 2;
		packet_len = RFIFOL(fd,2);
	}
	if((int)RFIFOREST(fd)<packet_len){
		return 2;
	}

	//printf("MAPIF: %d\n",cmd);
	switch (cmd) {
	case 0x3001: mapif_parse_mapset(fd,id); break;
	case 0x3002: mapif_parse_login(fd,id); break;
	case 0x3003: mapif_parse_requestchar(fd); break;
	case 0x3004: mapif_parse_savechar(fd); break;
	case 0x3005: mapif_parse_logout(fd); break;
	case 0x3006: mapif_parse_whisp(fd); break;
	case 0x3007: mapif_parse_savecharlog(fd); break;
	case 0x3008: mapif_parse_deletepost(fd); break;
	case 0x3009: mapif_parse_showposts(fd); break;
	case 0x300A: mapif_parse_readpost(fd); break;
	case 0x300B: mapif_parse_userlist(fd); break;
	case 0x300C: mapif_parse_boardpost(fd); break;
	case 0x300D: mapif_parse_nmailwrite(fd); break;
	case 0x300E: mapif_parse_findnewmp(fd); break;
	//case 0x300E: mapif_parse_checkformp(fd); break; //Check for Mail and Parcel
	default:	break;
	}

	RFIFOSKIP(fd, packet_len);
	return 0;
}
