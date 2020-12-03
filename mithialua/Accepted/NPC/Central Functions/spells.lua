function Player.learnSpell(player)
	local t = {graphic = 0, color = 0}
	local found
	local spellNames = player:getUnknownSpells()
	local spellName = {}
	local spellYName = {}
	local spellFuncs = {}
	local spellItemReq = {}
	local spellItemAmount={}
	local spellLevelReq = {}
	local spellDesc = {}
	
	player.npcGraphic = t.graphic
	player.npcColor = t.color
	
	for i = 1, #spellNames do
		if (i % 2 == 0 and player.registry["learned_"..spellNames[i]] == 0) then
			table.insert(spellYName, spellNames[i])
		elseif (i % 2 == 1 and player.registry["learned_"..spellNames[i + 1]] == 0) then
			table.insert(spellName, spellNames[i])
		end
	end
	
	for i = 1, #spellYName do
		local level = 1
		local items = {0}
		local amounts = {100}
		local desc = {"A spell"}
		local func = assert(loadstring("return "..spellYName[i]..".requirements"))(player)
		
		if (func ~= nil) then
			level, items, amounts, desc = func(player)
		end
		
		table.insert(spellLevelReq, level)
		table.insert(spellItemReq, items)
		table.insert(spellItemAmount, amounts)
		table.insert(spellDesc, desc)
	end
	
	if (#spellLevelReq > 0) then
		local i = 1
		local level
		local items
		local amounts
		local desc
		local name
		local yname
		
		repeat
			if (i <= #spellLevelReq and (spellLevelReq[i] > player.level or (spellLevelReq[i - 1] ~= nil and spellLevelReq[i] < spellLevelReq[i - 1]))) then
				level = table.remove(spellLevelReq, i)
				items = table.remove(spellItemReq, i)
				amounts = table.remove(spellItemAmount, i)
				desc = table.remove(spellDesc, i)
				name = table.remove(spellName, i)
				yname = table.remove(spellYName, i)
				
				if (level <= player.level) then
					table.insert(spellLevelReq, 1, level)
					table.insert(spellItemReq, 1, items)
					table.insert(spellItemAmount, 1, amounts)
					table.insert(spellDesc, 1, desc)
					table.insert(spellName, 1, name)
					table.insert(spellYName, 1, yname)
				end
				
				i = 1
			else
				i = i + 1
			end
		until i > #spellLevelReq
	end
	
	local c = player:menuString("What would you like to learn?", spellName)
	if(c ~= "" ) then
		for x = 1, #spellName do
			if (spellName[x] == c) then
				found = x
				break
			end
		end	
	end
		
	if(found > 0) then
		if(not player:dialogSeq(spellDesc[found], 1)) then
			return false
		end
		
		local c = player:menuString("Do you swear to use this for good?", {"Yes", "Yes *evil grin*", "No"})
		if (c == "Yes" or c == "Yes *evil grin*") then
			
			if (#spellItemReq[found] > 0) then
				if (player:checkItems(spellItemReq[found], spellItemAmount[found])) then
					player:removeItems(spellItemReq[found], spellItemAmount[found])
				else
					player:dialog("You do not have the required items.", {})
					return false
				end
			end
			
			if (player.level < spellLevelReq[found]) then
				player:dialog("You are not high enough level for that.", {})
				return false
			end
			
			player.registry["learned_"..spellYName[found]] = 1
			player:addSpell(spellYName[found])
			player:sendMinitext("Your mind expands as you learn "..spellName[found])
			
			if (c == "Yes *evil grin*") then
				player:dialog("Yeah, sure you are.  Wipe that smirk from your face, I've got my eye on you...",{})
			end
			
		elseif (c == "No") then
			return false
		end
	end
end

function Player.forgetSpell(player)
	local t = {graphic = 0, color = 0}
	player.npcGraphic = t.graphic
	player.npcColor = t.color
	
	local spellNames = player:getSpellName()
	local spellYNames = player:getSpellYName()
	local spellIDs = player:getSpells()
	local placeholder
	local found
	local selection

	selection = player:menuString("What would you like to forget?", spellNames)
	if(selection ~= "" ) then
		for x = 1, #spellNames do
			if(spellNames[x] == selection) then
				found = x
				break
			end
		end	
	end
	selection = player:menuString("Are you sure you wish to forget "..spellNames[found].."?", {"Yes", "No"})
	if(selection == "Yes") then
		player.registry["learned_"..spellYNames[found]] = 0
		player:removeSpell(spellIDs[found])
	end
end

function Player.checkItems(player, items, amounts)
	for x = 1, #items do
		if (items[x] == 0) then
			if (player.money < amounts[x]) then
			    return false
			end
		else
			if (player:hasItem(items[x], amounts[x]) == true) then
			else
				return false
			end
		end
	end
	
	return true
end

function Player.removeItems(player, items, amounts)
	for x = 1, #items do
		if (items[x] == 0) then
			player.money = player.money - amounts[x]
			player:sendStatus()
		else
			player:removeItem(items[x], amounts[x])
		end
	end
end


function Player.canLearnSpell(player, str)
	if (type(str) ~= "string") then
		return false
	end
	local spells = player:getSpells()
	if (#spells < 52) then
		if (player:hasSpell(""..str)) then
			return false
		end
		return true
	else
		return false
	end
end