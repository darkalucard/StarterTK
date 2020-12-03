#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "recipedb.h"
#include "map.h"
#include "db.h"
#include "malloc.h"
#include "core.h"
#include "script.h"
#include "db_mysql.h"
#include "strlib.h"

DBMap *recipedb;

int recipedb_searchname_sub(DBKey *key, void *data, va_list ap)
{
	struct recipe_data *recipe = (struct recipe_data *)data, **dst;
	char *str;
	
	str = va_arg(ap, char *);
	dst = va_arg(ap, struct recipe_data**);
	nullpo_ret(0, str);
	
	if (strcmpi(recipe->identifier, str) == 0)
		*dst = recipe;
	else if(strcmpi(recipe->description, str) == 0)
		*dst = recipe;
	else if(strcmpi(recipe->critIdentifier, str) == 0)
		*dst = recipe;
	else if(strcmpi(recipe->critDescription, str) == 0)
		*dst = recipe;
	
	return 0;
}

struct recipe_data* recipedb_searchname(const char *str)
{
	struct recipe_data *recipe = NULL;
	
	recipedb->foreach(recipedb, recipedb_searchname_sub, str, &recipe);
	return recipe;
}

struct recipe_data* recipedb_search(unsigned int id)
{
	static struct recipe_data *db = NULL;
	
	if (db && db->id == id)
		return db;
	
	db = uidb_get(recipedb, id);
	
	if (db)
		return db;
	
	CALLOC(db, struct recipe_data, 1);
	uidb_put(recipedb, id, db);
	db->id = id;
	strcpy(db->identifier, "??");
	strcpy(db->description, "??");
	strcpy(db->critIdentifier, "??");
	strcpy(db->critDescription, "??");
	return db;
}

struct recipe_data* recipedb_searchexist(unsigned int id)
{
	struct recipe_data *db = NULL;
	
	db = uidb_get(recipedb, id);
	return db;
}

void recipedb_read()
{
	struct recipe_data *db;
	unsigned int i, rec = 0;
	struct recipe_data recipe;
	SqlStmt* stmt = SqlStmt_Malloc(sql_handle);
	
	if (stmt == NULL)
	{
		SqlStmt_ShowDebug(stmt);
		return 0;
	}
	
	memset(&recipe, 0, sizeof(recipe));
	
	if (SQL_ERROR == SqlStmt_Prepare(stmt,"SELECT `RecId`, `RecIdentifier`, `RecDescription`, `RecSuccessRate`, `RecCraftTime`,"
		"`RecSkillAdvance`, `RecCritIdentifier`, `RecCritDescription`, `RecTokensRequired`, `RecMaterial1`, `RecAmount1`,"
		"`RecMaterial2`, `RecAmount2`, `RecMaterial3`, `RecAmount3`, `RecMaterial4`, `RecAmount4`, `RecMaterial5`, `RecAmount5`,"
		"`RecCritRate`, `RecBonus`, `RecSkillRequired`, `RecSuperiorMaterial1`, `RecSuperiorAmount1` FROM `Recipes`")
	|| SQL_ERROR == SqlStmt_Execute(stmt)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 0 ,SQLDT_UINT, &recipe.id, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 1 ,SQLDT_STRING, &recipe.identifier, sizeof(recipe.identifier), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 2 ,SQLDT_STRING, &recipe.description, sizeof(recipe.description), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 3 ,SQLDT_UINT, &recipe.successRate, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 4 ,SQLDT_UINT, &recipe.craftTime, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 5 ,SQLDT_UINT, &recipe.skillAdvance, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 6 ,SQLDT_STRING, &recipe.critIdentifier, sizeof(recipe.critIdentifier), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 7 ,SQLDT_STRING, &recipe.critDescription, sizeof(recipe.critDescription), NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 8 ,SQLDT_UINT, &recipe.tokensRequired, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 9 ,SQLDT_UINT, &recipe.materials[0], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 10 ,SQLDT_UINT, &recipe.materials[1], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 11 ,SQLDT_UINT, &recipe.materials[2], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 12 ,SQLDT_UINT, &recipe.materials[3], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 13 ,SQLDT_UINT, &recipe.materials[4], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 14 ,SQLDT_UINT, &recipe.materials[5], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 15 ,SQLDT_UINT, &recipe.materials[6], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 16 ,SQLDT_UINT, &recipe.materials[7], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 17 ,SQLDT_UINT, &recipe.materials[8], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 18 ,SQLDT_UINT, &recipe.materials[9], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 19 ,SQLDT_UINT, &recipe.critRate, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 20 ,SQLDT_UINT, &recipe.bonus, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 21 ,SQLDT_UINT, &recipe.skillRequired, 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 22 ,SQLDT_UINT, &recipe.superiorMaterials[0], 0, NULL, NULL)
	|| SQL_ERROR == SqlStmt_BindColumn(stmt, 23 ,SQLDT_UINT, &recipe.superiorMaterials[1], 0, NULL, NULL))
	{
		SqlStmt_ShowDebug(stmt);
		SqlStmt_Free(stmt);
		return 0;
	}
		
	rec = SqlStmt_NumRows(stmt);
	
	for (i = 0; i < rec && SQL_SUCCESS == SqlStmt_NextRow(stmt); i++)
	{
		db = recipedb_search(recipe.id);
		memcpy(db, &recipe, sizeof(recipe));
	}
	
	SqlStmt_Free(stmt);
	printf("Recipe DB read done. %d recipes loaded!\n", rec);
}

void recipedb_term()
{
	if (recipedb)
		db_destroy(recipedb);
}

void recipedb_init()
{
	recipedb = uidb_alloc(DB_OPT_BASE);
	recipedb_read();
}