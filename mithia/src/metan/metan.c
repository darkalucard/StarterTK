#include <dirent.h>
#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include "db_mysql.h"
#include "metan.h"
#include "itemdb.h"
#include "class_db.h"
#include "guide.h"
#include "malloc.h"
char scheme[32];
int filecount;
int guide_count;
char sql_id[32]="";
char sql_pw[32]="";
char sql_ip[32]="";
char sql_db[32]="";
int sql_port;
Sql* sql_handle=NULL;

unsigned short swap16(unsigned short val) {
	
	int hi,lo;
	unsigned short y;

	hi=val/256;
	lo=val%256;

	y=(lo*256)+hi;
	
	//printf("hi: %d\nlo:%d\n",hi,lo);

	
	return y;

}
int config_read(const char *cfg_file) {
	char line[1024], r1[1024], r2[1024];
	int line_num = 0;
	FILE *fp;

	fp = fopen(cfg_file, "r");
	if (fp == NULL) {
		printf("CFG_ERR: Configuration file (%s) not found.\n", cfg_file);
		//add_log("CFG_ERR: Configuration file (%s) not found.\n", cfg_file);
		exit(1);
	}

	while(fgets(line, sizeof(line), fp)) {
		line_num++;
		if (line[0] == '/' && line[1] == '/')
			continue;

		if (sscanf(line, "%[^:]: %[^\r\n]", r1, r2) == 2) {
			//CHAR
			
			if(strcmpi(r1, "sql_ip") == 0) {
				strcpy(sql_ip,r2);
			} else if(strcmpi(r1, "sql_port") == 0) {
				sql_port=atoi(r2);
			} else if(strcmpi(r1, "sql_id") == 0) {
				strcpy(sql_id,r2);
			} else if(strcmpi(r1, "sql_pw") == 0) {
				strcpy(sql_pw,r2);
			} else if(strcmpi(r1, "sql_db") == 0) {
				strcpy(sql_db,r2);
			}
		}
	}
	fclose(fp);
	printf("Configuration File (%s) reading finished!\n", cfg_file, sql_db);
	return 0;
}

int main(int argc,char **argv) {
	char filename[128];
	int x=0;
	unsigned int id;
	unsigned int charicinfo[100000];
	int customequip[100000];
	unsigned int iteminfo[100000];
    int ci_max=0;
	int ce_max=0;
	int ii_max=0;
    int ci_count=0;
	int ce_count=0;
	int ii_count=0;
	int a,b,c,d;
	int filecount2;
	
	config_read("conf/char.conf");
	//sql_init();
	sql_handle=Sql_Malloc();
	if(sql_handle == NULL)
	{
		Sql_ShowDebug(sql_handle);
		exit(1);
	}
	if( SQL_ERROR == Sql_Connect(sql_handle, sql_id, sql_pw, sql_ip, (uint16)sql_port, sql_db) )
	{
		Sql_ShowDebug(sql_handle);
		Sql_Free(sql_handle);
		exit(1);
	}
	itemdb_init(); //Load itemdb
	classdb_init(); //Load classdb
	//guide_count=guidedb_init();
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if (stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ItmId` FROM `Items` WHERE `ItmType` > 2 AND `ItmType` < 17 ORDER BY `ItmId`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	//Sql_Query(sql_handle,"SELECT id FROM itemdb WHERE type>2 AND type<17");
	
	x = SqlStmt_NumRows(stmt);
	
	ci_max=x;
	for (a = 0; a < ci_max && SQL_SUCCESS == SqlStmt_NextRow(stmt); a++) {
		//char* data;
		//sql_get_row();
		//Sql_GetData(sql_handle, 0, &data, NULL);
		charicinfo[ci_count] = id;
		ci_count++;
	}
	//Sql_FreeResult(sql_handle);

	/*Sql_Query(sql_handle,"SELECT id FROM customitemdb WHERE type>2 AND type<17");

	x=Sql_NumRows(sql_handle);

	ce_max=x;
	for(b=0;b<ce_max && SQL_SUCCESS==Sql_NextRow(sql_handle);b++) {
		char* data;
		Sql_GetData(sql_handle,0,&data,NULL);
		customequip[ce_count]=atoi(data);
		ce_count++;
	}
	Sql_FreeResult(sql_handle);*/

	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ItmId` FROM `Items` WHERE `ItmType` < 3 OR `ItmType` > 16 ORDER BY `ItmId`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &id, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	//Sql_Query(sql_handle,"SELECT id FROM itemdb WHERE type<3 OR type>16 OR type=0");
	
	//sql_request("SELECT id FROM itemdb WHERE type<3 OR type>16 OR type=0");
	x = SqlStmt_NumRows(stmt);
	ii_max = x;
	for (c = 0; c < ii_max && SQL_SUCCESS == SqlStmt_NextRow(stmt); c++) {
		//char* data;
		//Sql_GetData(sql_handle,0,&data,NULL);
		iteminfo[ii_count] = id;
		ii_count++;
	}
	//Sql_FreeResult(sql_handle);
	SqlStmt_Free(stmt);
	//printf("Naming Scheme: ");
	//scanf("%s",scheme);
	//printf("%d\n",ci_max);
	filecount=ci_max/1000+1;
	for(x=0;x<filecount;x++) {
		sprintf(filename,"meta/CharicInfo%d",x);
		printf("	%s\n",filename);

		output_meta(filename,x,charicinfo,ci_max);

	}

	/*if(ce_max>0) {
		for(x=0;x<filecount;x++) {
			sprintf(filename,"meta/CustomEquip%d",x);
			printf("	%s\n",filename);

			output_metac(filename,x,customequip,ce_max);
		}
	}*/
	
	//printf("%d\n",ii_max);
	filecount=ii_max/1000+1;
	if(ii_max>0) {
		for(x=0;x<filecount;x++) {
			sprintf(filename,"meta/ItemInfo%d",x);
			printf("	%s\n",filename);
			output_meta(filename,x,iteminfo,ii_max);
		}
	}
		
	//output_groupnames();
	//output_collections();
	return 0;
}

int output_meta(char* filename,int num,unsigned int list[],int max) {
	FILE *fp;
	int offset;
	int offset2;
	int x;
	char buf[64];
	char buf1;
	unsigned short buf2;
	unsigned int buf3;
	
	if (max < ((num + 1) * 1000)) {
		offset = max - (num * 1000);
	} else {
		offset = (num + 1) * 1000;
	}
	
	fp=fopen(filename,"wb"); //open up the damn file!
	
	buf2=swap16(offset); //set the size
	fwrite(&buf2,2,1,fp);
	
	for (x = (num * 1000); x < offset + (num * 1000); x++) {
		buf1=strlen(itemdb_name(list[x]));
		fwrite(&buf1,1,1,fp); //write namelen
		fwrite(itemdb_name(list[x]),1,buf1,fp); //write name
		
		buf2=swap16(20);
		fwrite(&buf2,2,1,fp); //write 20
		
		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_mightreq(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); //MightReq len
		fwrite(buf,1,strlen(buf),fp); //MightReq
		
		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_price(list[x]));
		buf2=swap16(strlen(buf));
		//printf("%d\n",buf2);
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);
		
		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_dura(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);
		
		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_ac(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_hit(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_dam(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_vita(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_mana(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);
		
		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_might(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_grace(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);
		
		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_will(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_wisdom(list[x])); //Wisdom increase
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);
		
		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_con(list[x])); //etc2 //Con
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",classdb_path(itemdb_class(list[x])));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_rank(list[x])); //rank
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_level(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_healing(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);
		
		memset(buf,0,64);
		sprintf(buf,"%d",itemdb_protection(list[x]));
		buf2=swap16(strlen(buf));
		fwrite(&buf2,2,1,fp); 
		fwrite(buf,1,strlen(buf),fp);

		if(itemdb_mindam(list[x])==0) {
			sprintf(buf,"%d",0);
			buf2=swap16(strlen(buf));
		} else {
			sprintf(buf,"%dm%d",itemdb_mindam(list[x]),itemdb_maxdam(list[x]));
			buf2=swap16(strlen(buf));
		}
		fwrite(&buf2,2,1,fp);
		fwrite(buf,1,strlen(buf),fp);
		fwrite(&buf2,2,1,fp);
		fwrite(buf,1,strlen(buf),fp);
	}
	fclose(fp);


	return 0;
}