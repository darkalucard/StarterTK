
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmo.h"
#include "rndm.h"
#include "version.h"
#include "malloc.h"
#include "save.h"
#include "save_db.h"
#include "zlib.h"
#include "md5calc.h"
#include "strlib.h"
#include "timer.h"

#ifdef USE_MYSQL
#include "db_mysql.h"

void save_db_init() {
	sql_handle=Sql_Malloc();
	if(sql_handle == NULL) {
		Sql_ShowDebug(sql_handle);
		exit(EXIT_FAILURE);
	}
	if( SQL_ERROR == Sql_Connect(sql_handle, sql_id, sql_pw, sql_ip, (uint16)sql_port, sql_db) )
	{
		Sql_ShowDebug(sql_handle);
		Sql_Free(sql_handle);
		exit(EXIT_FAILURE);
	}

}

void save_db_term() {
	Sql_Free(sql_handle);
	//sql_close();
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
	int i = 0;
	SqlStmt* stmt=SqlStmt_Malloc(sql_handle);

	if(!stmt)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	//char status save
	Sql_EscapeString(sql_handle,escape1,p->name);
	Sql_EscapeString(sql_handle,escape2,p->title);
	Sql_EscapeString(sql_handle,escape3,p->clan_title);
	Sql_EscapeString(sql_handle,escape4,p->f1name);
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt, "SELECT `ChaId` FROM `Character` WHERE `ChaName` = '%s' AND `ChaOnline` = '0'", escape1)
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0, SQLDT_UINT, &i, 0, NULL, NULL)) {
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
	
	if (SQL_SUCCESS == SqlStmt_NextRow(stmt)) {
		SqlStmt_Free(stmt);
		return 0;
	}
	
	SqlStmt_Free(stmt);
	
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
#endif



