
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common/mmo.h"
#include "../common/db.h"
#include "core.h"
#include "socket.h"
#include "timer.h"
#include "map.h"
#include "intif.h"
#include "mmo.h"
#include "malloc.h"
#include "pc.h"
#include "clif.h"
#include "zlib.h"
#include "strlib.h"
#include "db_mysql.h"
#include "crypt.h"

struct DBMap* auth_db;
extern unsigned int getTicks(void);



static const int packet_len_table[] = {
		3, -1, 38, -1, 6, 0, 0, 0,5,-1,-1,6,6,8,6,sizeof(struct boards_read_post_1)+2,  //0x3800-0x380F
		0 };

int intif_shutdown(int fd) {
	exit(0);
}
int check_connect_char(int ip, int port) {
	//printf("Called Connect\n");
	if (char_fd <= 0 || session[char_fd] == NULL) {
		printf("Attempt to connect to char-server...\n");
		char_fd = make_connection(ip, port);
		session[char_fd]->func_parse = intif_parse;
		session[char_fd]->func_shutdown=intif_shutdown;
		realloc_rfifo(char_fd, FIFOSIZE_SERVER, FIFOSIZE_SERVER);
		WFIFOHEAD(char_fd,72);
		WFIFOW(char_fd,0) = 0x3000;
		memcpy(WFIFOP(char_fd,2), char_id, 32);
		memcpy(WFIFOP(char_fd,34), char_pw, 32);
		WFIFOL(char_fd, 66) = map_ip;
		WFIFOW(char_fd, 70) = map_port;
		WFIFOSET(char_fd, 72);
	}
	return 0;
}

int intif_quit(USER *sd) {
	if (char_fd <= 0)
		return -1;

	WFIFOHEAD(char_fd,6);
	WFIFOW(char_fd, 0) = 0x3005;
	WFIFOL(char_fd, 2) = sd->status.id;
	WFIFOSET(char_fd, 6);
	return 0;
}

int intif_load(int fd, int id, char *name) {
	if (char_fd <= 0)
		return -1;

	//printf("	Sent FD: %d\n",fd);
	WFIFOHEAD(char_fd,24);
	WFIFOW(char_fd,0)=0x3003;
	WFIFOW(char_fd,2)=fd;
	WFIFOL(char_fd,4)=id;
	memcpy(WFIFOP(char_fd,8),name,16);
	WFIFOSET(char_fd,24);
	//WFIFOB(char_fd, 0) = 0x03;
	//WFIFOW(char_fd, 1) = fd;
	//WFIFOL(char_fd, 3) = id;
	//memcpy(WFIFOP(char_fd, 7), name, 16);
	//WFIFOSET(char_fd, 23);
	return 0;
}

//Save bank data to DB
int membankdata_todb(struct bank_data bank[], int max, int id) {
	SqlStmt* stmt;
	int save_id[max];
	int item_id = -1;
	int i;
	char escape[64];

	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `BnkPosition` FROM `Banks` WHERE `BnkChaId` = '%u' ORDER BY `BnkPosition` ASC LIMIT 255", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &item_id, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[item_id] = item_id;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		Sql_EscapeString(sql_handle, escape, bank[i].real_name);

		if (save_id[i] == i) {
			if(bank[i].item_id == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `Banks` WHERE `BnkChaId` = '%u' AND `BnkPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Banks` SET `BnkItmId` = '%u', `BnkAmount` = '%u', `BnkChaIdOwner` = '%u', `BnkEngrave` = '%s' WHERE `BnkChaId` = '%u' AND `BnkPosition` = '%d'",
					bank[i].item_id, bank[i].amount, bank[i].owner_id, escape, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		} else {
			if(bank[i].item_id > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Banks` (`BnkChaId`, `BnkItmId`, `BnkAmount`, `BnkChaIdOwner`, `BnkEngrave`, `BnkPosition`) VALUES ('%u', '%u', '%u', '%u', '%s', '%d')",
					id, bank[i].item_id, bank[i].amount, bank[i].owner_id, escape, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}
	
	return 1;
}

//Save item data to DB
int meminvdata_todb(struct item items[], int max, int id) {
	SqlStmt* stmt;
	struct item item;
	StringBuf buf;
	int save_id[max];
	int item_id = -1;
	int i;
	char escape[255];

	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `InvPosition` FROM `Inventory` WHERE `InvChaId` = '%u' ORDER BY `InvPosition` ASC LIMIT 52", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &item_id, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[item_id] = item_id;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		Sql_EscapeString(sql_handle,escape,items[i].real_name);
		
		if (save_id[i] == i) {
			if(items[i].id == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `Inventory` WHERE `InvChaId` = '%u' AND `InvPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Inventory` SET `InvItmId` = '%u', `InvAmount` = '%u', `InvDurability` = '%u', `InvChaIdOwner` = '%u', `InvEngrave` = '%s' WHERE `InvChaId` = '%u' AND `InvPosition` = '%d'",
					items[i].id, items[i].amount, items[i].dura, items[i].owner_id,  escape, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
		else
		{
			if(items[i].id > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Inventory` (`InvChaId`, `InvItmId`, `InvAmount`, `InvDurability`, `InvChaIdOwner`, `InvEngrave`, `InvPosition`) VALUES ('%u', '%u', '%u', '%u', '%u', '%s', '%u')",
					id, items[i].id, items[i].amount, items[i].dura, items[i].owner_id, escape, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}
	
	return 1;
}

//Save item data to DB
int memeqpdata_todb(struct item items[], int max, int id) {
	SqlStmt* stmt;
	struct item item;
	StringBuf buf;
	int save_id[max];
	int item_id = -1;
	int i;
	char escape[255];

	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `EqpSlot` FROM `Equipment` WHERE `EqpChaId` = '%u' ORDER BY `EqpSlot` ASC LIMIT 15", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &item_id, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[item_id] = item_id;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		Sql_EscapeString(sql_handle,escape,items[i].real_name);
		
		if (save_id[i] == i) {
			if(items[i].id == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `Equipment` WHERE `EqpChaId` = '%u' AND `EqpSlot` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Equipment` SET `EqpItmId` = '%u', `EqpDurability` = '%u', `EqpChaIdOwner` = '%u', `EqpEngrave` = '%s' WHERE `EqpChaId` = '%u' AND `EqpSlot` = '%d'",
					items[i].id, items[i].dura, items[i].owner_id,  escape, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
		else
		{
			if(items[i].id > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Equipment` (`EqpChaId`, `EqpItmId`, `EqpDurability`, `EqpChaIdOwner`, `EqpEngrave`, `EqpSlot`) VALUES ('%u', '%u', '%u', '%u', '%s', '%u')",
					id, items[i].id, items[i].dura, items[i].owner_id, escape, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}
	
	return 1;
}
//Registry to Database
int memreg_todb(struct global_reg regs[], int max, int id) {
	SqlStmt* stmt;
	int save_id[max];
	int regid = -1;
	int i;

	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `RegPosition` FROM `Registry` WHERE `RegChaId` = '%u' ORDER BY `RegPosition` ASC LIMIT 500", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &regid, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[regid] = regid;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		if (save_id[i] == i) {
			if(regs[i].val == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `Registry` WHERE `RegChaId` = '%u' AND `RegPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Registry` SET `RegIdentifier` = '%s', `RegValue` = '%u' WHERE `RegChaId` = '%u' AND `RegPosition` = '%d'",
					regs[i].str, regs[i].val, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
		else
		{
			if(regs[i].val > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Registry` (`RegChaId`, `RegIdentifier`, `RegValue`, `RegPosition`) VALUES ('%u', '%s', '%u', '%d')",
					id, regs[i].str, regs[i].val, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}

	return 1;
}
//Registry to Database
int memnpcreg_todb(struct global_reg regs[], int max, int id) {
	SqlStmt* stmt;
	int save_id[max];
	int regid = -1;
	int i;

	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `NrgPosition` FROM `NPCRegistry` WHERE `NrgChaId` = '%u' ORDER BY `NrgPosition` LIMIT 500", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &regid, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[regid] = regid;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		if (save_id[i] == i) {
			if(regs[i].val == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `NPCRegistry` WHERE `NrgChaId` = '%u' AND `NrgPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `NPCRegistry` SET `NrgIdentifier` = '%s', `NrgValue` = '%u' WHERE `NrgChaId` = '%u' AND `NrgPosition` = '%d'",
					regs[i].str, regs[i].val, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
		else
		{
			if(regs[i].val > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `NPCRegistry` (`NrgChaId`, `NrgIdentifier`, `NrgValue`, `NrgPosition`) VALUES ('%u', '%s', '%u', '%d')",
					id, regs[i].str, regs[i].val, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}

	return 1;
}
//Registry to Database
int memquestreg_todb(struct global_reg regs[], int max, int id) {
	SqlStmt* stmt;
	int save_id[max];
	int regid = -1;
	int i;

	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL) {
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `QrgPosition` FROM `QuestRegistry` WHERE `QrgChaId` = '%u' ORDER BY `QrgPosition` ASC LIMIT 500", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &regid, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[regid] = regid;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		if (save_id[i] == i) {
			if(regs[i].val == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `QuestRegistry` WHERE `QrgChaId` = '%u' AND `QrgPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `QuestRegistry` SET `QrgIdentifier` = '%s', `QrgValue` = '%u' WHERE `QrgChaId` = '%u' AND `QrgPosition` = '%d'",
					regs[i].str, regs[i].val, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
		else
		{
			if(regs[i].val > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `QuestRegistry` (`QrgChaId`, `QrgIdentifier`, `QrgValue`, `QrgPosition`) VALUES ('%u', '%s', '%u', '%d')",
					id, regs[i].str, regs[i].val, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}

	return 1;
}
//save skills to db
int memspells_todb(unsigned short skill[], int max, int id) {
	SqlStmt* stmt;
	int save_id[max];
	int magic_id = -1;
	int i;

	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `SbkPosition` FROM `SpellBook` WHERE `SbkChaId` = '%u' ORDER BY `SbkPosition` ASC LIMIT 52", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &magic_id, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[magic_id] = magic_id;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		if (save_id[i] == i) {
			if(skill[i] == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `SpellBook` WHERE `SbkChaId` = '%u' AND `SbkPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `SpellBook` SET `SbkSplId` = '%u' WHERE `SbkChaId` = '%u' AND `SbkPosition` = '%d'",
					skill[i], id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		} else {
			if(skill[i] > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `SpellBook` (`SbkChaId`, `SbkSplId`, `SbkPosition`) VALUES ('%u', '%u', '%d')",
					id, skill[i], i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}
	
	return 1;
}
//save aethers to db
int memaethers_todb(struct skill_info dura_aether[], int max, int id) {
	SqlStmt* stmt;
	int save_id[max];
	int magic_id = -1;
	int i;
	
	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `AthPosition` FROM `Aethers` WHERE `AthChaId` = '%u' ORDER BY `AthPosition` ASC LIMIT 200", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &magic_id, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[magic_id] = magic_id;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		if (save_id[i] == i) {
			if(dura_aether[i].id == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `Aethers` WHERE `AthChaId` = '%u' AND `AthPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Aethers` SET `AthSplId` = '%u', `AthDuration` = '%d', `AthAether` = '%d' WHERE `AthChaId` = '%u' AND `AthPosition` = '%d'",
					dura_aether[i].id, dura_aether[i].duration, dura_aether[i].aether, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		} else {
			if(dura_aether[i].id > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Aethers` (`AthChaId`, `AthSplId`, `AthDuration`, `AthAether`, `AthPosition`) VALUES ('%u', '%u', '%d', '%d', '%d')",
					id, dura_aether[i].id, dura_aether[i].duration, dura_aether[i].aether, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}
	
	return 1;
}
//save killcount to db
int memkillcount_todb(struct mq quest[], int max, int id) {
	SqlStmt* stmt;
	int save_id[max];
	int kill_id = -1;
	int i;
	
	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `KilPosition` FROM `Kills` WHERE `KilChaId` = '%u' ORDER BY `KilPosition` ASC LIMIT 10", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &kill_id, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[kill_id] = kill_id;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		if (save_id[i] == i) {
			if(quest[i].mob_id == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `Kills` WHERE `KilChaId` = '%u' AND `KilPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Kills` SET `KilAmount` = '%u', `KilMobId` = '%u' WHERE `KilChaId` = '%u' AND `KilPosition` = '%u'",
					quest[i].amount, quest[i].mob_id, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		} else {
			if(quest[i].mob_id > 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Kills` (`KilChaId`, `KilMobId`, `KilAmount`, `KilPosition`) VALUES ('%u', '%u', '%d', '%d')",
					id, quest[i].mob_id, quest[i].amount, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}
	
	return 1;
}
//save legend to db
int memlegend_todb(struct legend legends[], int max, int id) {
	SqlStmt* stmt;
	int save_id[max];
	int legendid = -1;
	int i;
	char escape[255];
	
	memset(save_id, 0, max * sizeof(int));
	stmt=SqlStmt_Malloc(sql_handle);
	
	if(stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}

	if(SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `LegPosition` FROM `Legends` WHERE `LegChaId` = '%u' ORDER BY `LegPosition` ASC LIMIT 255", id)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_INT, &legendid, 0, NULL, NULL)
	)
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}

	for (i = 0; i < max; i++)
		save_id[i] = -1;
	
	for (i = 0; i < SqlStmt_NumRows(stmt) && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
		save_id[legendid] = legendid;
	
	SqlStmt_Free(stmt);
	
	for (i = 0; i < max; i++) {
		Sql_EscapeString(sql_handle,escape,legends[i].text);
		
		if (save_id[i] == i) {
			if(strcmpi(legends[i].name,"") == 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"DELETE FROM `Legends` WHERE `LegChaId` = '%u' AND `LegPosition` = '%d'", id, i)) {
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			} else {
				if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Legends` SET `LegIcon` = '%u', `LegColor` = '%u', `LegDescription` = '%s', `LegIdentifier` = '%s' WHERE `LegChaId` = '%u' AND `LegPosition` = '%d'",
					legends[i].icon, legends[i].color, escape, legends[i].name, id, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		} else {
			if(strcmpi(legends[i].name,"") != 0) {
				if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Legends` (`LegChaId`, `LegIcon`, `LegColor`, `LegDescription`, `LegIdentifier`, `LegPosition`) VALUES ('%u', '%u', '%u', '%s', '%s', '%d')",
					id, legends[i].icon, legends[i].color, escape, legends[i].name, i))
				{
					Sql_ShowDebug(sql_handle);
					return 0;
				}
			}
		}
	}
	
	return 1;
}
///Save Character TO database
int mmo_char_todb(struct mmo_charstatus *p) {
	char escape1[16];
	char escape2[255];
	char escape3[255];
	char escape4[16];
	int i;
	//char status save
	Sql_EscapeString(sql_handle,escape1,p->name);
	Sql_EscapeString(sql_handle,escape2,p->title);
	Sql_EscapeString(sql_handle,escape3,p->clan_title);
	Sql_EscapeString(sql_handle,escape4,p->f1name);
	
	if(SQL_ERROR == Sql_Query(sql_handle,"UPDATE `Character` SET `ChaName` = '%s', `ChaClnId` = '%u', `ChaClanTitle` = '%s', `ChaTitle` = '%s', `ChaLevel` = '%u', `ChaPthId` = '%u', `ChaMark` = '%u', "
		"`ChaTotem` = '%u', `ChaKarma` = '%u', `ChaCurrentVita` = '%u', `ChaMaximumVita` = '%u', `ChaCurrentMana` = '%u', `ChaMaximumMana` = '%u', `ChaExperience` = '%u', `ChaGold` = '%u', "
		"`ChaSex` = '%d', `ChaNation` = '%d', `ChaFace` = '%u', `ChaHairColor` = '%u', `ChaArmorColor` = '%u', `ChaMapId` = '%u', `ChaX` = '%u', `ChaY` = '%u', `ChaSide` = '%d', `ChaState` = '%d', "
		"`ChaHair` = '%u', `ChaFaceColor` = '%u', `ChaSkinColor` = '%u', `ChaChaIdSpouse` = '%u', `ChaClanChat` = '%d', `ChaPathChat` = '%d', `ChaSettings` = '%u', `ChaGMLevel` = '%d', "
		"`ChaDisguise` = '%u', `ChaDisguiseColor` = '%u', `ChaMaximumBankSlots` = '%u', `ChaBankGold` = '%u', `ChaF1Name` = '%s', `ChaMaximumInventory` = '%u', `ChaPK` = '%u', `ChaKilledBy` = '%u', "
		"`ChaKillsPK` = '%u', `ChaPKDuration` = '%u', `ChaMuted` = '%u', `ChaHeroes` = '%u', `ChaTier` = '%u', `ChaExperienceSold` = '%llu' WHERE `ChaId` = '%u'",
			escape1, p->clan_id, escape3, escape2, p->level, p->class, p->mark,
			p->totem, p->karma, p->hp, p->basehp, p->mp, p->basemp, p->exp, p->money,
			p->sex, p->country, p->face, p->hair_color, p->armor_color, p->last_pos.m, p->last_pos.x, p->last_pos.y, p->side, p->state,
			p->hair, p->face_color, p->skin_color, p->spouse, p->clan_chat, p->subpath_chat, p->settingFlags, p->gm_level,
			p->disguise, p->disguisecolor, p->maxslots, p->bankmoney, escape4, p->maxinv, p->pk, p->killedby,
			p->killspk, p->pkduration, p->mute, p->heroes, p->tier, p->expsold, p->id))
	{
		Sql_ShowDebug(sql_handle);
		return 0;
	}
	
	//inventory save
	meminvdata_todb(p->inventory,52,p->id);

	//equip save
	memeqpdata_todb(p->equip,15,p->id);

	//skill save
	memspells_todb(p->skill,52,p->id);
	
	//aether save
	memaethers_todb(p->dura_aether,200,p->id);

	//Reg Save
	memreg_todb(p->global_reg,500,p->id);

	//NPC Interaction Save
	memnpcreg_todb(p->npcintreg, 500, p->id);
	
	//Quest Save
	memquestreg_todb(p->questreg, 500, p->id);
	
	//Kill count save
	memkillcount_todb(p->quest,10,p->id);
	
	//legend save
	memlegend_todb(p->legends,255,p->id);
	
	//bank save
	membankdata_todb(p->banks,255,p->id);

	return 1;
}

int intif_savequit(USER *sd) {
	unsigned int clen = 0, ulen = 0;
	int retval = 0;
	nullpo_ret(0,sd);

	sd->status.last_pos.m = sd->bl.m;
	sd->status.last_pos.x = sd->bl.x;
	sd->status.last_pos.y = sd->bl.y;
	sd->status.disguise = sd->disguise;
	sd->status.disguisecolor = sd->disguise_color;

	if (mmo_char_todb(&sd->status)) {
		printf("Saved %s\n", sd->status.name);
	}
	
	//ulen = sizeof(struct mmo_charstatus);
	//clen = compressBound(ulen);

	//WFIFOHEAD(char_fd, clen + 6);
	//retval = compress2(WFIFOP(char_fd, 6), &clen, &sd->status, ulen, 1); //fastest compression.

	//if (retval) {
	//	printf("Error Compressing: %s\n", sd->status.name);
	//	return 0;
	//}

	//WFIFOW(char_fd, 0) = 0x3007;
	//WFIFOL(char_fd, 2) = clen + 6;
	//WFIFOSET(char_fd, clen + 6);

	return 0;
}
int intif_save(USER *sd) {
	unsigned int clen = 0, ulen = 0;
	int retval = 0;
	nullpo_ret(0,sd);
	
	sd->status.last_pos.m = sd->bl.m;
	sd->status.last_pos.x = sd->bl.x;
	sd->status.last_pos.y = sd->bl.y;
	sd->status.disguise = sd->disguise;
	sd->status.disguisecolor = sd->disguise_color;

	//if (mmo_char_todb(&sd->status)) {
	//	printf("Saved %s\n", sd->status.name);
	//}
	
	ulen = sizeof(struct mmo_charstatus);
	clen = compressBound(ulen);

	WFIFOHEAD(char_fd, clen + 6);
	retval = compress2(WFIFOP(char_fd, 6), &clen, &sd->status, ulen, 1); //fastest compression.

	if (retval) {
		printf("Error Compressing: %s\n", sd->status.name);
		return 0;
	}

	WFIFOW(char_fd, 0) = 0x3004;
	WFIFOL(char_fd, 2) = clen + 6;
	WFIFOSET(char_fd, clen + 6);

	return 0;
}

int intif_mmo_tosd(int fd, struct mmo_charstatus* p) {
	USER *sd;
	int n;
	if(fd==map_fd) {
		return 0;
	}
	if(!p) {
		session[fd]->eof=7;
		return 0;
	}
	if(session[fd]->session_data) { //data already exists
		//session[fd]->eof=8;
		return 0;
	}


	CALLOC(sd, USER, 1);
	memcpy(&sd->status, p, sizeof(struct mmo_charstatus));

	sd->fd = fd;

	session[fd]->session_data = sd;

	populate_table(&(sd->status.name), &(sd->EncHash), sizeof(sd->EncHash));
	sd->bl.id = sd->status.id;
	sd->bl.prev = sd->bl.next = NULL;
	
	sd->disguise = sd->status.disguise;
    sd->disguise_color = sd->status.disguisecolor;
	sd->viewx = 8;
	sd->viewy = 7;

	if(sd->status.gm_level) sd->optFlags = optFlag_walkthrough; //.
	if(!map_isloaded(sd->status.last_pos.m)) {
		sd->status.last_pos.m=0; sd->status.last_pos.x=8; sd->status.last_pos.y=7;
	}
	
	pc_setpos(sd,sd->status.last_pos.m,sd->status.last_pos.x,sd->status.last_pos.y);
	pc_loadmagic(sd);
	pc_starttimer(sd);
	pc_requestmp(sd);

	clif_sendack(sd);
	clif_sendtime(sd);
	clif_sendid(sd);
	clif_sendmapinfo(sd);
	clif_sendstatus(sd,SFLAG_FULLSTATS|SFLAG_HPMP|SFLAG_XPMONEY);
	clif_mystaytus(sd);
	clif_spawn(sd);
	clif_parserefresh(sd);
	clif_sendxy(sd);
	clif_getchararea(sd);


	clif_mob_look_start(sd);
	map_foreachinarea(clif_object_look_sub,sd->bl.m,sd->bl.x,sd->bl.y,SAMEAREA,BL_ALL,LOOK_GET,sd);
	clif_mob_look_close(sd);

	pc_loaditem(sd);
	pc_loadequip(sd);

	pc_magic_startup(sd);
	map_addiddb(&sd->bl);

	///test stuff
	/*
	WFIFOB(sd->fd,0)=0xAA;
	WFIFOB(sd->fd,1)=0x00;
	WFIFOB(sd->fd,2)=0x03;
	WFIFOB(sd->fd,3)=0x49;
	WFIFOB(sd->fd,4)=0x23;
	WFIFOB(sd->fd,5)=0x6D;
	WFIFOSET(sd->fd,6);
	*/
	mmo_setonline(sd->status.id,1);

	if(sd->status.gm_level) {
		//sd->optFlags|=optFlag_stealth;
		printf("GM(%s) set to stealth.\n",sd->status.name);
	}

	pc_calcstat(sd);
	map_foreachinarea(clif_updatestate, sd->bl.m, sd->bl.x, sd->bl.y, AREA, BL_PC, sd);
	clif_retrieveprofile(sd);
	return 0;
}
int authdb_init() {
	//auth_db=strdb_alloc(DB_OPT_BASE,32);
	return 0;
}
int auth_timer(int id, int none) {
	//struct auth_node* a=(struct auth_node*)strdb_get(auth_db,(char*)id);
	//sql_request("DELETE FROM auth WHERE char_id='%u'",id);
	//sql_get_row();
	//sql_free_row();
	Sql_Query(sql_handle,"DELETE FROM `Authorize` WHERE `AutChaId` = '%d'",id);
	//if(!a) return 1;
	//strdb_remove(auth_db,a->name);
	//FREE(a);

	//auth_fifo[id].ip=0;
	//auth_fifo[id].id=0;
	//memset(auth_fifo[id].name,0,16);
	return 1;
}
int auth_check(char* name,unsigned int ip) {
	unsigned int i;
	unsigned int id;
	char* data;

	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT `AutIP`, `AutChaId` FROM `Authorize` WHERE `AutChaName` = '%s'",name))
		Sql_ShowDebug(sql_handle);

	if(SQL_SUCCESS != Sql_NextRow(sql_handle))
		return 0; //Not available

	Sql_GetData(sql_handle, 0, &data, 0); i=strtoul(data,NULL,10);
	Sql_GetData(sql_handle, 1, &data, 0); id=(unsigned int)strtoul(data, NULL, 10);
	Sql_FreeResult(sql_handle);
	if(i==ip)
		return id;

	return 0;
	//struct auth_node* t=(struct auth_node*)strdb_get(auth_db,name);

}
int auth_delete(char* name) {
	char* data;
	Sql_Query(sql_handle,"SELECT `AutTimer` FROM `Authorize` WHERE `AutChaName` = '%s'",name);

	if(SQL_SUCCESS != Sql_NextRow(sql_handle))
		return 0;

	Sql_GetData(sql_handle,0,&data,0); timer_remove((unsigned int)strtoul(data, NULL, 10));
	Sql_FreeResult(sql_handle);

	Sql_Query(sql_handle,"DELETE FROM `Authorize` WHERE `AutChaName` = '%s'",name);
	//sql_request("SELECT timer FROM auth WHERE name='%s'",name);
	//if(sql_get_row())
	//	return 0;

	//timer_remove(sql_get_int(0));

	//sql_request("DELETE FROM auth WHERE name='%s'",name);
	//sql_get_row();

	//sql_free_row();
	return 0;
}
int auth_add(char* name,unsigned int id, unsigned int ip) {
	int timer;
	//sql_request("SELECT * FROM auth WHERE name='%s'",name);
	if(SQL_ERROR == Sql_Query(sql_handle,"SELECT * FROM `Authorize` WHERE `AutChaName` = '%s'",name))
		Sql_ShowDebug(sql_handle);

	if(SQL_SUCCESS == Sql_NextRow(sql_handle))
		return 0;


	timer=timer_insert(120000,120000,auth_timer,id,0);

	if(SQL_ERROR == Sql_Query(sql_handle,"INSERT INTO `Authorize` (`AutChaName`, `AutChaId`, `AutIP`, `AutTimer`) VALUES('%s', '%u', '%u', '%u')", name, id, ip, timer))
		Sql_ShowDebug(sql_handle);


	//sql_request("INSERT INTO auth (name,char_id,ip,timer) VALUES('%s','%u','%u','%u')",name,id,ip,timer);
	//sql_get_row();
	//sql_free_row();
	//strdb_put(auth_db,t->name,t);
	return 0;
}
int intif_parse_accept(int fd) {
		int i,j=0;
		if (RFIFOB(fd,2)) {
			printf("CFG_ERR: Username or password to connect Login Server is Invalid!\n");
			add_log("CFG_ERR: Username or password to connect Login Server is Invalid!\n");
			return 0;
		}
		add_log("Connected to Char Server.\n");
		printf("Connected to Char Server.\n");
		WFIFOHEAD(fd,	map_n*2+8);
		WFIFOW(fd, 0) = 0x3001;
		WFIFOL(fd, 2) = map_n*2+8;
		WFIFOW(fd, 6) = map_n;
		for(i=0;i<MAX_MAP_PER_SERVER;i++) {
			if(map[i].tile != NULL) {
				WFIFOW(fd, j*2+8) = i;
				j++;
			}
			if(j>=map_n)
				break;
		}
		WFIFOSET(fd, map_n*2+8);
		return 0;
}
int intif_parse_mapset(int fd) {
	//if(RFIFOREST(fd) < RFIFOW(fd, 7)*2+9)
			//return 0;
		return 0;
		int i;
		for(i=0;i<RFIFOW(fd, 7);i++)
			map_setmapip(RFIFOW(fd, i*2+9), RFIFOL(fd, 1), RFIFOW(fd, 5));
		{
			unsigned char *p;
			p=RFIFOP(fd, 1);
			printf("Received %d maps from map server %u.%u.%u.%u:%d\n", RFIFOW(fd, 7), p[0], p[1], p[2], p[3], RFIFOW(fd, 5));
		}
		//RFIFOSKIP(fd, RFIFOW(fd, 7)*2+9);
		return 0;
}
int intif_parse_authadd(int fd) {

		auth_add(RFIFOP(fd,8),RFIFOL(fd,4),RFIFOL(fd,34));

		//printf("Auth add: %s\n",RFIFOP(fd,8));
		//printf("Accepting IP: %u - %s\n",RFIFOL(fd,33), auth_fifo
		WFIFOHEAD(fd,20);
		WFIFOW(fd, 0) = 0x3002;
		WFIFOW(fd, 2) = RFIFOW(fd, 2);
		memset(WFIFOP(fd, 4), 0, 16);
		memcpy(WFIFOP(fd, 4), RFIFOP(fd, 8), 16);
		WFIFOSET(fd, 20);

	return 0;
}
int intif_parse_charload(int fd) {
		unsigned int ulen,clen,retval,x;
		char* cbuf = NULL;

		struct mmo_charstatus* a=NULL;
		if(!session[RFIFOW(fd,6)])
			return 0;

		ulen=sizeof(struct mmo_charstatus);
		clen=ulen;
		CALLOC(cbuf,char,ulen);
		retval=uncompress(cbuf,&clen,RFIFOP(fd,8),RFIFOL(fd,2)-8);
			/*if(!retval) {
				a=(struct mmo_charstatus*)cbuf;

				for(x=0;x<fd_max;x++) {
					if(session[x] && !strcmpi(session[x]->name,a->name)) {
						intif_mmo_tosd(x,a);
						break;
					}
				}
			}
		*/
		intif_mmo_tosd(RFIFOW(fd,6),(struct mmo_charstatus*)cbuf);
		FREE(cbuf);
		return 0;
}
int intif_parse_checkonline(int fd) {
	//Come back here
	//return 0;
	USER* sd=NULL;

	sd=map_id2sd(RFIFOL(fd,2));

	if(sd) { //User is not online, FORCE login to delete
		session[sd->fd]->eof=20;
		//WFIFOW(fd,0)=0x3005;
		//WFIFOL(fd,2)=RFIFOL(fd,2);
	}
	return 0;
}
int intif_parse_unknown(int fd) {
	return 0;
}

int intif_parse_whisp(int fd) {
	return 0;
	if (RFIFOREST(fd) < RFIFOW(fd, 33)+36)
			return 0;

		{
			USER *sd;
			if (RFIFOB(fd, 1) == 0x00) {
				sd = map_name2sd(RFIFOP(fd, 18));
				if (sd)
					clif_sendwisp(sd, RFIFOP(fd, 2), RFIFOP(fd, 36), RFIFOW(fd, 36));
			} else if (RFIFOB(fd, 1) == 0x01) {
				sd = map_name2sd(RFIFOP(fd, 2));
				if (sd)
					clif_retrwisp(sd, RFIFOP(fd, 18), RFIFOP(fd, 36), RFIFOW(fd, 36));
			} else {
				sd = map_name2sd(RFIFOP(fd, 2));
				if (sd)
					clif_failwisp(sd);
			}
		}
		RFIFOSKIP(fd, RFIFOW(fd, 33)+36);
}
int intif_parse_deletepostresponse(int fd) {
	int sfd=RFIFOW(fd,2);
	int response=RFIFOB(fd,4);
	USER* sd=NULL;

	if(sfd>FD_SETSIZE || sfd >= fd_max || sfd <= 0)
		return 0;

	if(session[sfd]) sd=(USER*)session[sfd]->session_data;
	if(!sd) return 0;

	switch(response) {
	case 0x00: //Got the OK!
		nmail_sendmessage(sd,"The message has been deleted.",7,1);
		break;
	case 0x01: //Not Your Post, Cannot delete
		nmail_sendmessage(sd,"You can only delete your messages.",7,0);
		break;
	case 0x02: //SQL Error
		nmail_sendmessage(sd,"DB Error. Please ticket Chronicles of Mithia staff.",7,0);
		break;
	default:
		break;
	}
	return 0;
}
int intif_parse_showpostresponse(int fd) {
	USER* sd=NULL;
	struct boards_show_header_1 header;
	struct boards_show_array_1 post;
	int len=0,pos,flen;


	//Zero out Variables
	memset(&header,0,sizeof(header));
	memset(&post,0,sizeof(post));

	memcpy(&header, RFIFOP(fd,6),sizeof(header));

	int x;
	StringBuf buf;
	//return 0;
	if(header.fd>FD_SETSIZE || header.fd >= fd_max || header.fd <= 0)
		return 0;

	if(session[header.fd]) sd=(USER*)session[header.fd]->session_data;

	if(!sd) return 0;

	if(strcmpi(sd->status.name,header.name)) return 0;

	StringBuf_Init(&buf);
	len=0;
	WFIFOHEAD(header.fd,65535);
	WFIFOHEADER(header.fd,0x31,0);
	WFIFOB(header.fd,5)=header.flags2;
	WFIFOB(header.fd,6)=header.flags1;
	WFIFOW(header.fd,7)=SWAP16(header.board);
	WFIFOB(header.fd,9)=strlen(boarddb_name(header.board));
	strcpy(WFIFOP(header.fd,10),boarddb_name(header.board));
	len=strlen(boarddb_name(header.board))+1;
	pos=len+9;
	flen=sizeof(header)+6; //Skip past header

	if(header.array<=0)
		WFIFOB(header.fd,pos)=0;
	else
	{
		WFIFOB(header.fd,pos)=header.array;
		len++;

		for(x=0;x<header.array;x++) {
			memset(&post,0,sizeof(post));
			memcpy(&post,RFIFOP(fd,flen),sizeof(post));

			WFIFOB(header.fd,len+9)=post.color; //Color
			WFIFOW(header.fd,len+10)=SWAP16(post.post_id); //POST ID

			StringBuf_Clear(&buf);
			if(header.board && post.board_name)
				StringBuf_Printf(&buf,"%s %s",bn_name(post.board_name),post.user);
			else
				StringBuf_Printf(&buf,"%s",post.user);

			WFIFOB(header.fd,len+12)=StringBuf_Length(&buf);

			memcpy(WFIFOP(header.fd,len+13),StringBuf_Value(&buf),StringBuf_Length(&buf)); //Name

			len+=StringBuf_Length(&buf)+4;

			WFIFOB(header.fd,len+9)=post.month;
			WFIFOB(header.fd,len+10)=post.day;
			len+=2;
			WFIFOB(header.fd,len+9)=strlen(post.topic);
			memcpy(WFIFOP(header.fd,len+10),post.topic,strlen(post.topic));
			flen+=sizeof(post);
			len+=strlen(post.topic)+1;

			}
	}

		WFIFOW(header.fd,1)=SWAP16(len+7);
		WFIFOSET(header.fd,encrypt(header.fd));
		sd->bcount++;
		StringBuf_Destroy(&buf);
		return 0;

}

int intif_parse_userlist(int fd) {
	int sfd = RFIFOW(fd, 6);
	int count = RFIFOW(fd, 8);
	int i, len, class, mark, clan, path, hunter;
	unsigned int nation = 0;
	char name[16];

	USER* sd = NULL;
	if (sfd > FD_SETSIZE || sfd >= fd_max || sfd <= 0)
		return 0;
	if (session[sfd])
		sd = session[sfd]->session_data;

	if (!sd)
		return 0;

	if (!session[sd->fd])
	{
		session[sd->fd]->eof = 8;
		return 0;
	}

	int usercount = 0;

	USER* tsd = NULL;
	struct socket_data* p;
	for (int i = 0; i < fd_max; i++) {
		p = session[i];
		if (p && (tsd = p->session_data)) {
			usercount += 1; // This gets true number of users on the specific server. most cases will match count.
		}
	}

	// USER LIST PACKET //

	WFIFOHEAD(sd->fd, 0);
	WFIFOB(sd->fd, 0) = 0xAA;
	WFIFOB(sd->fd, 3) = 0x36;
	//WFIFOB(sd->fd,4)=0x01;
	WFIFOW(sd->fd, 5) = SWAP16(count); //total users
	WFIFOW(sd->fd, 7) = SWAP16(count); //users on server

	//WFIFOB(sd->fd,9)=0x01;
	WFIFOB(sd->fd, 9) = 1;
	len = 10;

	//testing//

	for (i = 0; i < count; i++) {
		hunter = RFIFOW(fd, i * 22 + 10);
		class = RFIFOW(fd, i * 22 + 12);
		mark = RFIFOW(fd, i * 22 + 14);
		clan = RFIFOW(fd, i * 22 + 16);
		nation = RFIFOW(fd, i * 22 + 18);
		memcpy(name, RFIFOP(fd, i * 22 + 20), 16);

		if (class > 4) {
			path = classdb_path(class);
		}
		else {
			path = class;
		}

		// region 0 = neutral, 1 = kugnae, 2 = buya, 3 = nagnang, 4 = gogoon

		// 1 = neutral warrior, 2 = rogue, 3 = mage, 4 = poet, 5 = peasant
		// 9 = neutral warrior, 10 = rogue, 11 = mage, 12 = poet, 13 = peasant
		// 17 = koguryo warrior, 18= rogue, 19 = mage, 20 = poet, 21 = peasant,
		// 25 = koguryo warrior, 26 = rogue, 27 = mage, 28 = poet, 29 = peasant
		// 33 = buya warrior, 34 = rogue, 35 = mage, 36 = poet, 37 = peasant
		// 41 = buya warrior, 42 = rogue, 43 = mage, 44 = poet, 45 = peasant
		// 49 = nagnang warrior, 50 = rogue, 51 = mage, 52 = poet, 53 = peasant
		// 57 = nagnang warrior, 58 = rogue, 59 = mage, 60 = poet, 61 = peasant
		// 65 = gogoon warrior, 66 = rogue, 67 = mage, 68 = poet, 69 = peasant
		// 73 = gogoon warrior, 74 = rogue, 75 = mage, 76 = poet, 77 = peasant

		WFIFOB(sd->fd, len) = path + (16 * nation);
		WFIFOB(sd->fd, len + 1) = (16 * mark) + classdb_icon(class);
		//printf("16*mark= %u, classdb_icon = %i\n",16*mark,classdb_icon(class));

		WFIFOB(sd->fd, len + 2) = hunter; // hunter flag

		// Color Flag
		WFIFOB(sd->fd, len + 3) = 143; // white
		if (classdb_path(class) == 5) { WFIFOB(sd->fd, len + 3) = 47; } //GM   39 = yellow, 47 = red

		WFIFOB(sd->fd, len + 4) = strlen(name);

		strcpy(WFIFOP(sd->fd, len + 5), name);
		len = len + strlen(name) + 5;
	}

	WFIFOW(sd->fd, 1) = SWAP16(len + 3);

	WFIFOSET(sd->fd, encrypt(sd->fd));

	return 0;
}

int intif_parse_boardpostresponse(int fd) {
	int sfd=RFIFOW(fd,2);
	USER* sd=NULL;
	if(sfd>FD_SETSIZE || sfd >= fd_max || sfd <= 0)
		return 0;
	if(session[sfd])
		sd=session[sfd]->session_data;

	if(!sd) return 0;

	switch(RFIFOW(fd,4))
	{
		case 0: //Worked!
			nmail_sendmessage(sd,"Your message has been posted.",6,1);
			break;
		case 1: //Sql Error
			nmail_sendmessage(sd,"(Sql Error) Please ticket Chronicles of Mithia staff.",6,0);
			break;
	}

	return 0;
}
int intif_parse_nmailwriteresponse(int fd) {
	int sfd=RFIFOW(fd,2);
	USER* sd=NULL;
	if(sfd>FD_SETSIZE || sfd >= fd_max || sfd <= 0)
		return 0;
	if(session[sfd])
		sd=session[sfd]->session_data;

	if(!sd) return 0;

	switch(RFIFOW(fd,4))
	{
		case 0: //Worked
			nmail_sendmessage(sd,"Your message has been sent.",6,1);
			break;
		case 1: //Sql Error
			nmail_sendmessage(sd,"(Sql Error) Please ticket Chronicles of Mithia staff.",6,0);
			break;
		case 2: //Invalid user
			nmail_sendmessage(sd,"User does not exist.",6,0);
			break;
	}
	return 0;
}
int intif_parse_findmp(int fd) {
	unsigned int id = RFIFOL(fd,2);
	int mailFlags=RFIFOW(fd,6);
	USER* sd=NULL;

	sd=map_id2sd(id);
	if(sd)
	{
		if(mailFlags&FLAG_MAIL)
			sd->flags |= FLAG_MAIL;

		if(mailFlags&FLAG_PARCEL)
			sd->flags |= FLAG_PARCEL;

		clif_sendstatus(sd,SFLAG_XPMONEY);
	}
	return 0;
}
int intif_parse_setmp(int fd) {
	int sfd=RFIFOW(fd,2);
	int flags=RFIFOW(fd,4);

	USER* sd=NULL;
	if(sfd>FD_SETSIZE || sfd >= fd_max || sfd <= 0)
		return 0;
	if(session[sfd])
		sd=session[sfd]->session_data;

	if(!sd) return 0;

	sd->flags=flags;


	clif_sendstatus(sd,SFLAG_XPMONEY);
	return 0;
}
int intif_parse_readpost(int fd) {
	struct boards_read_post_1 h;

	memset(&h, 0, sizeof(h));

	memcpy(&h, RFIFOP(fd,2),sizeof(h));


	int len;
	StringBuf buf;

	USER* sd=NULL;
	if(h.fd>FD_SETSIZE || h.fd >= fd_max ||h.fd <= 0)
		return 0;

	if(session[h.fd])
		sd=session[h.fd]->session_data;

	if(!sd) return 0;

	if(strcmpi(sd->status.name, h.name)) return 0; //Name does not match User, Return
	
	if (!session[sd->fd])
	{
		session[sd->fd]->eof = 8;
		return 0;
	}

	WFIFOHEAD(sd->fd,4500);
	WFIFOB(sd->fd,0)=0xAA;
	WFIFOB(sd->fd,3)=0x31;
	WFIFOB(sd->fd,4)=0x03;
	WFIFOB(sd->fd,5)=h.type;
	WFIFOB(sd->fd,6)=h.buttons;
	WFIFOB(sd->fd,7)=0;
	if(!h.board) WFIFOB(sd->fd,7)=0x01;
	WFIFOW(sd->fd,8)=SWAP16(h.post);

		len=strlen(h.user);
		WFIFOB(sd->fd,10)=len;
		len++;
		strcpy(WFIFOP(sd->fd,11),h.user);
		WFIFOB(sd->fd,len+10)=h.month;
		WFIFOB(sd->fd,len+11)=h.day;
		len+=2;
		WFIFOB(sd->fd,len+10)=strlen(h.topic);
		strcpy(WFIFOP(sd->fd,len+11),h.topic);
		len+=strlen(h.topic)+1;
		WFIFOW(sd->fd,len+10)=SWAP16(strlen(h.msg));
		strcpy(WFIFOP(sd->fd,len+12),h.msg);
		len+=strlen(h.msg)+2;
		WFIFOW(sd->fd,1)=SWAP16(len+7);
		WFIFOSET(sd->fd,encrypt(sd->fd));
	return 0;
}

int intif_parse(int fd) {
	int packet_len, cmd;

	if(session[fd]->eof) {
		add_log("Can't connect to Char Server.\n");
		printf("Can't connect to Char Server.\n");
		char_fd = 0;
		session_eof(fd);
		return 0;
	}





	cmd = RFIFOW(fd,0);

	if(cmd<0x3800 || cmd>=0x3800+(sizeof(packet_len_table)/sizeof(packet_len_table[0])) ||
	   packet_len_table[cmd-0x3800]==0){
	   	return 0;
	}

	packet_len = packet_len_table[cmd-0x3800];
	if(packet_len==-1){
		if(RFIFOREST(fd)<6)
			return 2;
		packet_len = RFIFOL(fd,2);
	}
	if((int)RFIFOREST(fd)<packet_len){
		return 2;
	}


	//printf("MAP: %d\n",cmd);
	switch (cmd) {
	case 0x3800: intif_parse_accept(fd); break;
	case 0x3801: intif_parse_mapset(fd); break;
	case 0x3802: intif_parse_authadd(fd); break;
	case 0x3803: intif_parse_charload(fd); break;
	case 0x3804: intif_parse_checkonline(fd); break;
	case 0x3805: intif_parse_unknown(fd); break;
	case 0x3806: intif_parse_whisp(fd); break;
	case 0x3808: intif_parse_deletepostresponse(fd); break;
	case 0x3809: intif_parse_showpostresponse(fd); break;
	case 0x380A: intif_parse_userlist(fd); break;
	case 0x380B: intif_parse_boardpostresponse(fd); break;
	case 0x380C: intif_parse_nmailwriteresponse(fd); break;
	case 0x380D: intif_parse_findmp(fd);
	case 0x380E: intif_parse_setmp(fd);
	case 0x380F: intif_parse_readpost(fd); break;

	default: break;
	}

	RFIFOSKIP(fd, packet_len);
	//}
	return 0;
}

int intif_wisp(char *srcname, char *dstname, unsigned char *msg, int len) {
	if (char_fd <= 0)
		return -1;

	return 0; //Disable for now.
	WFIFOB(char_fd, 0) = 0x06;
	memcpy(WFIFOP(char_fd, 1), srcname, 16);
	memcpy(WFIFOP(char_fd, 17), dstname, 16);
	WFIFOW(char_fd, 33) = len;
	memcpy(WFIFOP(char_fd, 35), msg, len);
	WFIFOSET(char_fd, len+35);
	return 0;
}

int intif_timer(int data1, int data2) {
	return 0;
	/*int i;
	USER *sd;
	for(i=0;i<fd_max;i++){
		if(session[i] && (sd=(USER*)session[i]->session_data)) {
			intif_save(sd);
		}
	}*/
	//return 0;
}

int intif_init() {
	//timer_insert(300000,300000,intif_timer,save_time,0);
	//intif_table_recv[3] = sizeof(struct mmo_charstatus)+3;
	return 0;
}
