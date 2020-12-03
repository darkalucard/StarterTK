--??
function Player.staff(player, str, int)
	local loreTable = {charIDs = {1, 1}, charLevels = {5, 1}}
	local mapTable = {charIDs = {1, 1, 1, 1, 1}, charLevels = {3, 3, 3, 3, 3}}
	local scriptTable = {charIDs = {1, 1, 1, 1}, charLevels = {5, 3, 3, 5}}
	local projectManagerTable = {charIDs = {1, 1}, charLevels = {1, 1}}
	local accessLevel = -1
	
	if (str ~= nil) then
		if (int ~= nil) then
			if (str == "lore") then
				for i = 1, #loreTable.charIDs do
					if (player.ID == loreTable.charIDs[i] and loreTable.charLevels[i] >= int) then
						accessLevel = true
					end
				end
			elseif (str == "map") then
				for i = 1, #mapTable.charIDs do
					if (player.ID == mapTable.charIDs[i] and mapTable.charLevels[i] >= int) then
						accessLevel = true
					end
				end
			elseif (str == "script") then
				for i = 1, #scriptTable.charIDs do
					if (player.ID == scriptTable.charIDs[i] and scriptTable.charLevels[i] >= int) then
						accessLevel = true
					end
				end
			end
			
			for i = 1, #projectManagerTable.charIDs do
				if (str == "pm") then
					if (player.ID == projectManagerTable.charIDs[i] and projectManagerTable.charLevels[i] >= int) then
						accessLevel = true
					end
				else
					if (player.ID == projectManagerTable.charIDs[i]) then
						accessLevel = true
					end
				end
			end
		else
			if (str == "lore") then
				for i = 1, #loreTable.charIDs do
					if (player.ID == loreTable.charIDs[i]) then
						accessLevel = loreTable.charLevels[i]
					end
				end
			elseif (str == "map") then
				for i = 1, #mapTable.charIDs do
					if (player.ID == mapTable.charIDs[i]) then
						accessLevel = mapTable.charLevels[i]
					end
				end
			elseif (str == "script") then
				for i = 1, #scriptTable.charIDs do
					if (player.ID == scriptTable.charIDs[i]) then
						accessLevel = scriptTable.charLevels[i]
					end
				end
			end
			
			for i = 1, #projectManagerTable.charIDs do
				if (str == "pm") then
					if (player.ID == projectManagerTable.charIDs[i]) then
						accessLevel = projectManagerTable.charLevels[i]
					end
				else
					if (player.ID == projectManagerTable.charIDs[i]) then
						accessLevel = 5
					end
				end
			end
		end
	else
		for i = 1, #loreTable.charIDs do
			if (player.ID == loreTable.charIDs[i]) then
				accessLevel = true
			end
		end
		
		for i = 1, #mapTable.charIDs do
			if (player.ID == mapTable.charIDs[i]) then
				accessLevel = true
			end
		end
		
		for i = 1, #scriptTable.charIDs do
			if (player.ID == scriptTable.charIDs[i]) then
				accessLevel = true
			end
		end
		
		for i = 1, #projectManagerTable.charIDs do
			if (player.ID == projectManagerTable.charIDs[i]) then
				accessLevel = true
			end
		end
	end
		
	return accessLevel
end





--[[
unsafeLua = {"addGold", "sleep", "msleep", "getObjects", "getBlock", "delFromIDDB", "addPermanentSpawn", "dropItem", "sendParcel",
			"addItem", "addClan", "removeLegend", "addSpell", "removeSpell", "mapSelection", "addGuide", "delGuide", "lock",
			"unlock", "powerBoard", "swing", "bankDeposit", "clanBankDeposit", "getParcelList", "removeParcel", "setTimeValues",
			"expireItem", "logBuySell", "gameRegistry", "accountRegistry", "clan", "gmLevel", "money", "state", "boardDel", "boardWrite",
			"uflags", "optflags", "attackSpeed", "baseAttackSpeed", "speech", "settings", "target", "deduction", "baseSpeed", "spouse",
			"attacker", "coContainer", "maxSlots", "maxPerSlot", "maxBankMoney", "bankMoney", "afk", "stealth", "talkType", "maxInv",
			"killedBy", "steps", "dmgDealt", "dmgTaken", "onlineTime", "lastHealth", "mute", "PK", "pvp", "Mob", "NPC", "Parcel", "Item",
			"buyExtend", "sellExtend", "speak", "cast", "setDuration", "setAether", "passive", "flushDuration", "flushAether", "os.execute",
			"luaReload"}
			
unsafeLuaPlus = {"setTile", "setObject"}
]]--

canRunLuaTalk = function(player)
	checkLuaSafety(player, player.speech)
end

canRunLuaMail = function(player)
	checkLuaSafety(player, player.mail)
end

checkLuaSafety = function(player, check)
	local strCheck = 0
	local checkCount = 0
	
	repeat
		strCheck = string.find(check, "Player", strCheck + 1)
		
		if (strCheck ~= nil) then
			checkCount = checkCount + 1
		end
	until (strCheck == nil)
	
	if (player:staff("script") == 5) then
		player.luaExec = 1
		return
	end
	
	--[[
	if (player:staff("script") >= 3) then
		for i = 1, #unsafeLua do
			if (string.find(check, unsafeLua[i])) then
				return
			end
		end
	end
	
	if (player:staff("script") >= 1) then
		for i = 1, #unsafeLua do
			if (string.find(check, unsafeLua[i])) then
				return
			end
		end
		
		for i = 1, #unsafeLuaPlus do
			if (string.find(check, unsafeLuaPlus[i])) then
				return
			end
		end
	end
	]]--
	
	if (player:staff("script") >= 1) then
		if (checkCount == 0) then
			player.luaExec = 1
		else
			strCheck = 0
			
			repeat
				strCheck = string.find(check, "Player", strCheck + 1)
				
				if (string.byte(check, strCheck + 7) == 34) then
					strCheck = string.match(check, '("'..player.name..'")', strCheck + 6)
					
					if (strCheck == nil) then
						player.luaExec = 0
					else
						player.luaExec = 1
					end
				elseif (string.byte(check, strCheck + 7) == 39) then
					strCheck = string.match(check, "('"..player.name.."')", strCheck + 6)
					
					if (strCheck == nil) then
						player.luaExec = 0
					else
						player.luaExec = 1
					end
				else
					strCheck = string.match(check, "("..player.ID..")", strCheck + 6)
					
					if (strCheck == nil) then
						player.luaExec = 0
					else
						player.luaExec = 1
					end
				end
				
				checkCount = checkCount - 1
			until (checkCount == 0)
		end
	end
end

Player.upgradeGear = function(player, start, multType)
	local multiplier = .06
	local upgrade = start
	
	if (multType == 1) then
		multiplier = .1
	elseif (multType == 2) then
		multiplier = .15
	end
	
	player:talk(0, "Plain: "..math.floor(start + .5))
	
	for i = 0, 8 do
		if (i < 3) then
			upgrade = upgrade + (upgrade * multiplier)
		elseif (i >= 3 and i < 5) then
			upgrade = upgrade + (upgrade * (multiplier + .02))
		elseif (i >= 5 and i < 7) then
			upgrade = upgrade + (upgrade * (multiplier + .04))
		elseif (i == 7) then
			upgrade = upgrade + (upgrade * (multiplier + .06))
		elseif (i == 8) then
			upgrade = upgrade + (upgrade * (multiplier + .08))
		end
		
		player:talk(0, "+"..(i + 1)..": "..math.floor(upgrade + .5))
	end
end