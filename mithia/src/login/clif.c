#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "core.h"
#include "login.h"
#include "intif.h"
#include "socket.h"
#include "crypt.h"
#include "clif.h"
#include "malloc.h"
#include "zlib.h"
#include "db_mysql.h"
#include "timer.h"


extern int Check_Throttle(struct sockaddr_in S);
extern void Add_Throttle(struct sockaddr_in S);
const unsigned char svkey1packets[] = {2,10,68,94,96,98,102,111};
///int require_reg;
int isKey(int fd) {
	int x = 0;
	for (x = 0; x < (sizeof(svkey1packets) / sizeof(svkey1packets[0])); x++)
	{
		if (fd == svkey1packets[x])
			return 0;
	}
	return 1;
}
int encrypt(int fd, char* name, char* EncHash) {
	char key[16];
	set_packet_indexes(WFIFOP(fd, 0));
	crypt(WFIFOP(fd, 0));
	return (int) SWAP16(*(unsigned short*)WFIFOP(fd, 1)) + 3;
}

int clif_accept(int fd) {

//	session[fd]->client_addr.sin_addr

	int IPCount=0;

	int P;
	bool DDOS=false;

	for(P=0; P<= fd_max; P++)
	{
        if(P==fd)continue;
		if(session[P] == NULL)continue;
		if(session[fd]->client_addr.sin_addr.s_addr == session[P]->client_addr.sin_addr.s_addr)IPCount++;
        if(IPCount > 5){DDOS=true; break;}

	}



   	FILE *packetlog=  fopen("packetlog.txt", "a+");
	if(packetlog == NULL)
	{
		printf("Please run with sudo- cant open packet log\n");
	}
	else
	{
 
	  fprintf(packetlog, "Ip address connected: %u.%u.%u.%u\n",CONVIP(session[fd]->client_addr.sin_addr.s_addr));
	  fclose(packetlog);
	}




	if(DDOS)
	{

	  Add_Throttle( session[fd]->client_addr);
	  Log_Add("DDoS","<%02d:%02d> Login DDoS detected from %u.%u.%u.%u - ip throttled.\n",getHour(),getMinute(),CONVIP2(session[fd]->client_addr.sin_addr.s_addr));
	  printf("(DDOS)- Closing all connections from %u.%u.%u.%u\n",CONVIP(session[fd]->client_addr.sin_addr.s_addr));
	 
	  for(P=0;P <= fd_max; P++)
	  {
		  	  if(session[P] == NULL)continue;
	          if(P==fd)continue;
	          if(session[fd]->client_addr.sin_addr.s_addr == session[P]->client_addr.sin_addr.s_addr){session[P]->eof = 1; session_eof(P);}
	  }
	  session[fd]->eof = 1;
      session_eof(fd);
	 	  
	}
	else

	{
	  WFIFOHEAD(fd,22);
	  memcpy(WFIFOP(fd,0), "\xAA\x00\x13\x7E\x1B\x43\x4F\x4E\x4E\x45\x43\x54\x45\x44\x20\x53\x45\x52\x56\x45\x52\x0A", 22);
	  //set_packet_indexes(WFIFOP(fd, 0));
	  WFIFOSET(fd, 22);
	}
	
	return 0;
}

int clif_message(int fd, char code, char *buff) {
	int packet_len = strlen(buff)+6;
	WFIFOHEAD(fd,packet_len+3);
	WFIFOB(fd, 0) = 0xAA;
	WFIFOW(fd, 1) = SWAP16(packet_len);
	WFIFOB(fd, 3) = 0x02;
	WFIFOB(fd, 4) = 0x02;
	WFIFOB(fd, 5) = code;
	WFIFOB(fd, 6) = strlen(buff);
	strcpy(WFIFOP(fd, 7), buff);
	WFIFOW(fd, packet_len+8) = 0x00;
	set_packet_indexes(WFIFOP(fd, 0));
	crypt(WFIFOP(fd, 0));
	WFIFOSET(fd, packet_len+6);
	return 0;
	
}
int reg_check(const char* n, int len) {
	char buf[255];
	int flag=0;
	int nFlag=0;
	
	
	SqlStmt* stmt;
	
	memset(buf,0,255);
	memcpy(buf,n,len);
	
	
	stmt=SqlStmt_Malloc(sql_handle);
	if(stmt == NULL) 
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `ChaActId` FROM `Character` WHERE `ChaName` = '%s'",buf)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &nFlag, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	if(SQL_SUCCESS == SqlStmt_NextRow(stmt))
		flag=nFlag;
	
	SqlStmt_Free(stmt);
	return flag;
}

int clif_debug(unsigned char* stringthing,int len) {
	int i;
	
	for (i = 0; i < len; i++) {
		/*if (stringthing[i] < 16) {
			printf("%2X ", stringthing[i]);
		} else {*/
			printf("%02X ", stringthing[i]);
		//}
	}
	
	printf("\n");
	
	for (i = 0; i < len; i++) {
		if (stringthing[i] <= 32 || stringthing[i] > 126) {
			printf("   ");
		} else {
			printf("%02c ", stringthing[i]);
		}
	}
	
	printf("\n");
	return 0;
}

int clif_parse(int fd) {
	char name[31];
	char EncHash[0x401];
	unsigned short len;
	unsigned short ver=0;
	unsigned short deep=0;
	int lenn=10;
	if(session[fd]->eof) {
		session_eof(fd);
		return 0;
	}

	if(!session[fd]->session_data)
		CALLOC(session[fd]->session_data, struct login_session_data, 1);

	struct login_session_data *sd = session[fd]->session_data;

	if (RFIFOB(fd, 0) != 0xAA) {
		int head_err=0;
		while (RFIFOB(fd, 0) != 0xAA && RFIFOREST(fd)) {
			RFIFOSKIP(fd, 1);
			if (head_err++ > 30) {
				session[fd]->eof=1;
				return 0;
			}
		}
	}

	if (RFIFOREST(fd) < 3)
		return 0;

	len = SWAP16(RFIFOW(fd, 1)) + 3;
	int L;

	char OutStr[1024]="";
	sprintf(OutStr,"Packet (IP%u.%u.%u.%u L%i): ",CONVIP(session[fd]->client_addr.sin_addr.s_addr),len);

	for(L=0; L< len; L++)
	{
		  char HexStr[32];
		  sprintf(HexStr,"%X ", RFIFOB(fd, L));
	      strcat(OutStr,HexStr);
	}

	//ofstream LogFile("PacketLog.txt", ios::binary|ios::out|ios::app);
	//LogFile << OutStr << endl;
	//LogFile.close();
	
	//printf("%s\n", OutStr);

	FILE *packetlog=  fopen("packetlog.txt", "a+");
	if(packetlog == NULL)
	{
		printf("Please run with sudo- cant open packet log\n");
	}
	else
	{
      fprintf(packetlog, "%s\n", OutStr);
	  fclose(packetlog);
	}





	if (RFIFOREST(fd) < len)
		return 0;
	
	
	crypt(RFIFOP(fd, 0));
	
	switch (RFIFOB(fd, 3)) {
	case 0x00:
		
		crypt(RFIFOP(fd,0)); //reverse the encryption
		ver=SWAP16(RFIFOW(fd,4));
		deep=SWAP16(RFIFOW(fd,7));
		printf("got this far\n");
		if((ver>=nex_version)) {
			WFIFOB(fd, 0) = 0xAA;
			WFIFOB(fd, 1) = 0x00;
			WFIFOB(fd, 2) = 0x11;
			WFIFOB(fd, 3) = 0x00;
			WFIFOB(fd, 4) = 0x00;
			WFIFOB(fd, 5) = 0x27;
			WFIFOB(fd, 6) = 0x4F;
			WFIFOB(fd, 7) = 0x8A;
			WFIFOB(fd, 8) = 0x4A;
			WFIFOB(fd, 9) = 0x00;
			//added stuff for baram?
			WFIFOB(fd, 10) = 0x09;
			strcpy(WFIFOP(fd, 11), "KruIn7inc");
			//set_packet_indexes(WFIFOP(fd, 0));
			WFIFOSET(fd, 20);
		} else {
		printf("patching\n");
			WFIFOB(fd,0)=0xAA;
			WFIFOW(fd,1)=SWAP16(0x29);
			WFIFOB(fd,3)=0;
			WFIFOB(fd,4)=2;
			WFIFOW(fd,5)=SWAP16(nex_version);
			WFIFOB(fd,7)=1;
			WFIFOB(fd,8)=0x23;
			strcpy(WFIFOP(fd,9),"http://files.kru.com/NexusTK/patch/");
			set_packet_indexes(WFIFOP(fd, 0));
			WFIFOSET(fd,44 + 3);
			//session[fd]->eof=1;
		}
		printf("made it!\n");
		break;
	case 0x02:
		
		memset(sd->name, 0, 16);
		memset(sd->pass, 0, 16);
		//printf("Reading Login info.\n");
		
		if ((RFIFOB(fd, 5) > 15) || (RFIFOB(fd, 5) < 4) || string_check_allchars(RFIFOP(fd, 6),RFIFOB(fd, 5))) {
			clif_message(fd, 0x03, login_msg[LGN_ERRUSER]);
			break;
		}
		
		if ((RFIFOB(fd, 6+RFIFOB(fd, 5)) > 8) || (RFIFOB(fd, 6+RFIFOB(fd, 5)) < 3) || string_check(RFIFOP(fd, 7+RFIFOB(fd, 5)),RFIFOB(fd, 6+RFIFOB(fd, 5)))) {
			clif_message(fd, 0x05, login_msg[LGN_ERRPASS]);
			 
			break;
		}
		
		
		memcpy(sd->name, RFIFOP(fd, 6), RFIFOB(fd, 5));
		memcpy(sd->pass, RFIFOP(fd, 7+RFIFOB(fd, 5)), RFIFOB(fd, 6+RFIFOB(fd, 5)));
		
		if(strlen(sd->pass)>8 || strlen(sd->pass)<3)  {
			clif_message(fd,0x05, login_msg[LGN_ERRPASS]);
			break;
		}
		if (char_fd) {
			
			WFIFOHEAD(char_fd,20);
			WFIFOW(char_fd, 0) = 0x1001;
			WFIFOW(char_fd, 2) = fd;
			memcpy(WFIFOP(char_fd, 4), sd->name, 16);
			WFIFOSET(char_fd, 20);
		} else {
			clif_message(fd, 0x03, login_msg[LGN_ERRDB]);
		}
		break;
	case 0x03:
		
		
		
		
		if (string_check(RFIFOP(fd, 6),RFIFOB(fd, 5))) {
			clif_message(fd, 0x03, login_msg[LGN_ERRUSER]);
			break;
		}
		
		if (string_check(RFIFOP(fd, 7+RFIFOB(fd, 5)),RFIFOB(fd, 6+RFIFOB(fd, 5)))) {
			clif_message(fd, 0x05, login_msg[LGN_ERRPASS]);
			break;
		}
		if(require_reg) {
			if (!reg_check(RFIFOP(fd,6),RFIFOB(fd,5))) {
				clif_message(fd,0x03, "You must register your character first!\nPlease visit the Chronicles of Mithia Website to register.");
				break;
			}
		}
		memcpy(sd->name,RFIFOP(fd,6),RFIFOB(fd,5));
		char NameCheck[16];
		strcpy(NameCheck,sd->name);
		strlwr(NameCheck);
		
		memcpy(sd->pass,RFIFOP(fd,7+RFIFOB(fd,5)),RFIFOB(fd,6+RFIFOB(fd,5)));
		
		if (char_fd) {
			//printf("Sent information to thing.");
			WFIFOHEAD(char_fd,40);
			WFIFOW(char_fd, 0) = 0x1003;
			WFIFOW(char_fd, 2) = fd;
			memset(WFIFOP(char_fd, 4), 0, 16*2);
			memcpy(WFIFOP(char_fd, 4), RFIFOP(fd, 6), RFIFOB(fd, 5));
			memcpy(WFIFOP(char_fd, 20), RFIFOP(fd, 7+RFIFOB(fd, 5)), RFIFOB(fd, 6+RFIFOB(fd, 5)));
			WFIFOL(char_fd, 36) = session[fd]->client_addr.sin_addr.s_addr;
			WFIFOSET(char_fd, 40);
		} else {
			clif_message(fd, 0x03, login_msg[LGN_ERRDB]);
		}		
		break;
		
	case 0x04:
		
		if (!strlen(sd->name) || !strlen(sd->pass))
			session[fd]->eof = 1;

		sd->face = RFIFOB(fd, 6);
		sd->sex	= RFIFOB(fd, 10);
		sd->country = 1;
		sd->totem = RFIFOB(fd, 12);
		sd->hair = RFIFOB(fd,7);
		sd->face_color=RFIFOB(fd,8);
		sd->hair_color=RFIFOB(fd,9);

		if (char_fd) {
			WFIFOHEAD(char_fd,43);
			WFIFOW(char_fd, 0) = 0x1002;
			WFIFOW(char_fd, 2) = fd;
			memcpy(WFIFOP(char_fd, 4), sd->name, 16);
			memcpy(WFIFOP(char_fd, 20), sd->pass, 16);
			WFIFOB(char_fd, 36) = sd->face;
			WFIFOB(char_fd, 37) = sd->sex;
			WFIFOB(char_fd, 38) = sd->country;
			WFIFOB(char_fd, 39) = sd->totem;
			WFIFOB(char_fd, 40) = sd->hair;
			WFIFOB(char_fd, 41)= sd->hair_color;
			WFIFOB(char_fd, 42)= sd->face_color;
			WFIFOSET(char_fd, 43);
		} else {
			clif_message(fd, 0x03, login_msg[LGN_ERRDB]);
		}
		break;
	case 0x10:
		/*crypt(RFIFOP(fd,0)); //reverse the encryption
		printf("Testing Packet ID: %2X Packet content:\n", RFIFOB(fd, 3));
		clif_debug(RFIFOP(fd, 5), SWAP16(RFIFOW(fd, 1)) - 5);
		memset(name, 0, 31);
		memcpy(name, RFIFOP(fd, 16), RFIFOB(fd, 15));
		populate_table(&(name), &(EncHash), sizeof(EncHash));
		*/
		WFIFOHEAD(fd, 13);
		WFIFOB(fd, 0) = 0xAA;
		WFIFOW(fd, 1) = SWAP16(0x07);
		WFIFOB(fd, 3) = 0x60;
		WFIFOB(fd, 4) = 0x00;
		WFIFOB(fd, 5) = 0x55;
		WFIFOB(fd, 6) = 0xE0;
		WFIFOB(fd, 7) = 0xD8;
		WFIFOB(fd, 8) = 0xA2;
		WFIFOB(fd, 9) = 0xA0;
		set_packet_indexes(WFIFOP(fd, 0));
		WFIFOSET(fd, 13);
		//clif_accept(fd);
		break;
	case 0x26:
		
		if (string_check(RFIFOP(fd, 6),RFIFOB(fd, 5))) {
			clif_message(fd, 0x03, login_msg[LGN_ERRUSER]);
			break;
		}
		if(RFIFOB(fd,5)>16) return 0;
		if(RFIFOB(fd,6+RFIFOB(fd,5))>16) return 0;
		
		if (string_check(RFIFOP(fd, 7+RFIFOB(fd, 5)),RFIFOB(fd, 6+RFIFOB(fd, 5))) || string_check(RFIFOP(fd, 8+RFIFOB(fd, 5)+RFIFOB(fd, 6 + RFIFOB(fd, 5))), RFIFOB(fd, 7 + RFIFOB(fd, 5) + RFIFOB(fd, 6 + RFIFOB(fd, 5))))) {
			clif_message(fd, 0x05, login_msg[LGN_ERRPASS]);
			break;
		}
		if (char_fd) {
			WFIFOHEAD(char_fd,52);
			WFIFOW(char_fd, 0) = 0x1004;
			WFIFOW(char_fd, 2) = fd;
			memset(WFIFOP(char_fd, 4), 0, 16*3);
			memcpy(WFIFOP(char_fd, 4), RFIFOP(fd, 6), RFIFOB(fd, 5));
			memcpy(WFIFOP(char_fd, 20), RFIFOP(fd, 7+RFIFOB(fd, 5)), RFIFOB(fd, 6+RFIFOB(fd, 5)));
			memcpy(WFIFOP(char_fd, 36), RFIFOP(fd, 8+RFIFOB(fd, 5)+RFIFOB(fd, 6 + RFIFOB(fd, 5))), RFIFOB(fd, 7 + RFIFOB(fd, 5) + RFIFOB(fd, 6 + RFIFOB(fd, 5))));
			WFIFOSET(char_fd, 52);
		} else {
			clif_message(fd, 0x03, login_msg[LGN_ERRDB]);
		}
		break;
	case 0x57: //multi-server?
		//printf("Packet 0x57\n");
		/*WFIFOB(fd, 0) = 170;
		WFIFOB(fd, 1) = 0;
		WFIFOB(fd, 2) = 8;
		WFIFOB(fd, 3) = 96;
		WFIFOB(fd, 5) = 0;
		WFIFOB(fd, 6) = 43;//((rand()%255)+1);
		WFIFOB(fd, 7) = 90;//((rand()%255)+1);
		WFIFOB(fd, 8) = 147;//((rand()%255)+1);
		WFIFOB(fd, 9) = 230;//((rand()%255)+1);
		WFIFOB(fd, 10) = 0;
		set_packet_indexes(WFIFOP(fd, 0));
		crypt(WFIFOP(fd,0));
		WFIFOSET(fd,11 + 3);
		*/
		/*lenn=10;
		WFIFOB(fd,0)=0xAA;
		
		WFIFOB(fd,3)=0x03;
		WFIFOB(fd,4)=74;
		WFIFOB(fd,5)=130;
		WFIFOB(fd,6)=110;
		WFIFOB(fd,7)=236;
		WFIFOW(fd,8)=SWAP16(2010);
		WFIFOB(fd,10)=0x1B;
		WFIFOB(fd,11)=1;
		WFIFOB(fd,12)=9;
		strcpy(WFIFOP(fd,13),"NexonInc.");
		WFIFOB(fd,lenn+12)=0x0B;
		strcpy(WFIFOP(fd,lenn+13),"socket[361]");
		lenn+=12;
		WFIFOL(fd,lenn+12)=SWAP32(0x001AF7);
		lenn+=4;
		WFIFOW(fd,1)=SWAP16(lenn+9);
		WFIFOSET(fd,lenn+12);
*/
		break;
	case 0x71: //ping me!
		break;
	case 0x7B: //Request Item Information!
		
		switch(RFIFOB(fd,5)) {
		case 0: //Request the file asking for
			send_meta(fd);
			break;
		case 1: //Requqest the list to use
			send_metalist(fd);
			
			break;
		}
		break;
	case 0x62: //??
		break;
	
	case 0xFF:
		
		intif_auth(fd);
		break;
	default:
		printf("Unknown Packet ID: %02X Packet content:\n", RFIFOB(fd, 3));
		clif_debug(RFIFOP(fd, 5), SWAP16(RFIFOW(fd, 1)) - 5);
		break;
	}

	RFIFOSKIP(fd, len);
	return 0;
}
unsigned int metacrc(char* file) {
	FILE *fp=NULL;
	
	unsigned int checksum=0;
	unsigned int size;
	unsigned int size2;
	char fileinf[196608];
	fp=fopen(file,"rb");
	if(!fp) return 0;
	fseek(fp,0,SEEK_END);
	size=ftell(fp);
	fseek(fp,0,SEEK_SET);
	fread(fileinf,1,size,fp);
	fclose(fp);
	checksum=crc32(checksum,fileinf,size);
	
	return checksum;

}

int send_metafile(int fd, char* file) {
	int len=0;
	unsigned int checksum=0;
	unsigned int clen=0;
	Bytef* ubuf;
	Bytef* cbuf;
	unsigned int ulen=0;
	char filebuf[255];
	unsigned int retval;
	FILE *fp=NULL;

	sprintf(filebuf,"meta/%s",file);
	
	checksum=metacrc(filebuf);

	fp=fopen(filebuf,"rb");
	if(!fp) return 0;
	
	fseek(fp,0,SEEK_END);
	ulen=ftell(fp);
	fseek(fp,0,SEEK_SET);
	//CALLOC(ubuf,0,ulen);
	ubuf=(char*)calloc(ulen+1,sizeof(char));
	clen=compressBound(ulen);
	cbuf=(char*)calloc(clen+1,sizeof(char));
	fread(ubuf,1,ulen,fp);
	fclose(fp);
	
	retval=compress(cbuf,&clen,ubuf,ulen);
	
	if(retval!=0) printf("Fucked up %d\n",retval);
	WFIFOHEAD(fd,65535*2);
	WFIFOB(fd,0)=0xAA;
	WFIFOB(fd,3)=0x6F;
	//WFIFOB(fd,4)=0x08;
	WFIFOB(fd,5)=0; //this is sending file data
	WFIFOB(fd,6)=strlen(file);
	strcpy(WFIFOP(fd,7),file);
	len+=strlen(file)+1;
	WFIFOL(fd,len+6)=SWAP32(checksum);
	len+=4;
	WFIFOW(fd,len+6)=SWAP16(clen);
	len+=2;
	memcpy(WFIFOP(fd,len+6),cbuf,clen);
	len+=clen;
	WFIFOB(fd,len+6)=0;
	len+=1;
	//printf("%s\n",file);
	WFIFOW(fd,1)=SWAP16(len+3);
	set_packet_indexes(WFIFOP(fd, 0));
	crypt(WFIFOP(fd,0));
	WFIFOSET(fd,len+6 + 3);
	
	free(cbuf);
	free(ubuf);
	return 0;
}
int send_meta(int fd) {
	char temp[255];
	
	memset(temp,0,255);
	memcpy(temp,RFIFOP(fd,7),RFIFOB(fd,6));
	
	send_metafile(fd, temp);
	
	
	return 0;
}
int send_metalist(int fd) {
	int len=0;
	unsigned int checksum;
	char filebuf[255];
	int count=0;
	int x;

	WFIFOHEAD(fd,65535*2);
	WFIFOB(fd,0)=0xAA;
	WFIFOB(fd,3)=0x6F;
	//WFIFOB(fd,4)=0x00;
	WFIFOB(fd,5)=1;
	WFIFOW(fd,6)=SWAP16(metamax);
	len+=2;
	for(x=0;x<metamax;x++) {
		WFIFOB(fd,(len+6))=strlen(meta_file[x]);
		memcpy(WFIFOP(fd,len+7),meta_file[x],strlen(meta_file[x]));
		len+=strlen(meta_file[x])+1;
		sprintf(filebuf,"meta/%s",meta_file[x]);
		checksum=metacrc(filebuf);
		WFIFOL(fd,len+6)=SWAP32(checksum);
		len+=4;
	}
	
	WFIFOW(fd,1)=SWAP16(len+4);
	set_packet_indexes(WFIFOP(fd, 0));
	crypt(WFIFOP(fd,0));
	WFIFOSET(fd,len+7 + 3);

	return 0;
}

