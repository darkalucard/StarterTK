doInstanceSpawns = function(player)
	doRuinsSpawns(player)
	doCanidaeSpawns(player)
end

doRuinsSpawns = function(player)
	if (player.mapTitle == "Ruins") then
		ruins_spawns.room1(player)
	elseif (player.mapTitle == "East Wing") then
		ruins_spawns.room2(player)
	elseif (player.mapTitle == "West Wing") then
		ruins_spawns.room3(player)
	elseif (player.mapTitle == "Hoary Hallway") then
		ruins_spawns.room4(player)
	elseif (player.mapTitle == "Precarious Passage") then
		ruins_spawns.room5(player)
	end
end

doCanidaeSpawns = function(player)
	if (player.mapTitle == "Merry Forest") then
		canidae_spawns.room1(player)
	elseif (player.mapTitle == "Cheerful Forest") then
		canidae_spawns.room2(player)
	elseif (player.mapTitle == "Joyous Forest") then
		canidae_spawns.room3(player)
	elseif (player.mapTitle == "Glade") then
		canidae_spawns.room4(player)
	elseif (player.mapTitle == "Twisting Pine") then
		canidae_spawns.room5(player)
	elseif (player.mapTitle == "Clearing") then
		canidae_spawns.room6(player)
	end
end

checkInstanceWarps = function(player)
	local mobs = player:getObjectsInSameMap(BL_MOB)
	
	checkRuins(player, mobs)
	checkCanidae(player, mobs)
end

checkRuins = function(player, mobs)
	if (ruinsPushBack(player, mobs) == true) then
		return
	end
	
	local players = player:getObjectsInSameMap(BL_PC)
	
	if (player.mapTitle == "Ruins") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["ruins1Spawned"] = 1
			end
		end
	elseif (player.mapTitle == "East Wing") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["ruins2Spawned"] = 1
			end
		end
		
		for x = 1, 1 do
			players = player:getObjectsInMap(player.m - x, BL_PC)
			
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["ruins2Spawned"] = 1
				end
			end
		end
	elseif (player.mapTitle == "West Wing") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["ruins3Spawned"] = 1
			end
		end
		
		for x = 1, 2 do
			players = player:getObjectsInMap(player.m - x, BL_PC)
			
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["ruins3Spawned"] = 1
				end
			end
		end
	elseif (player.mapTitle == "Hoary Hallway") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["ruins4Spawned"] = 1
			end
		end
		
		for x = 1, 3 do
			players = player:getObjectsInMap(player.m - x, BL_PC)
			
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["ruins4Spawned"] = 1
				end
			end
		end
	elseif (player.mapTitle == "Precarious Passage") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["ruins5Spawned"] = 1
			end
		end
		
		for x = 1, 4 do
			players = player:getObjectsInMap(player.m - x, BL_PC)
			
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["ruins5Spawned"] = 1
				end
			end
		end
	end
end

ruinsPushBack = function(player, mobs)
	local pushed = false
	local players
	
	if (player.mapTitle == "Ruins" and #mobs > 0 and ((player.x == 0 or player.x == 34) and (player.y == 5 or player.y == 6 or player.y == 7))) then
		pushBack(player)
		pushed = true
	elseif (player.mapTitle == "East Wing" and #mobs > 0 and ((player.x == 12 and (player.y == 32 or player.y == 33 or player.y == 34)) or ((player.x == 7 or player.x == 8) and player.y == 14))) then
		pushBack(player)
		pushed = true
	elseif (player.mapTitle == "West Wing" and #mobs > 0 and ((player.x == 17 and (player.y == 32 or player.y == 33 or player.y == 34)) or ((player.x == 21 or player.x == 22) and player.y == 14))) then
		pushBack(player)
		pushed = true
	elseif (player.mapTitle == "Hoary Hallway" and #mobs > 0 and (((player.x == 6 or player.x == 7) and player.y == 24) or ((player.x == 27 or player.x == 28) and player.y == 24) or ((player.x == 16 or player.x == 17 or player.x == 18) and player.y == 0))) then
		pushBack(player)
		pushed = true
	elseif (player.mapTitle == "Precarious Passage") then
		if (#mobs <= 2) then
			for x = 0, 4 do
				players = player:getObjectsInMap(player.m - x, BL_PC)
				
				if (#players > 0) then
					for i = 1, #players do
						players[i].registry["ruins5Spawned"] = 1
					end
				end
			end
		else
			pushed = true
		end
	end
	
	return pushed
end

checkCanidae = function(player, mobs)
	if (canidaePushBack(player, mobs) == true) then
		return
	end
	
	local players = player:getObjectsInSameMap(BL_PC)
	
	if (player.mapTitle == "Merry Forest") then
		if (player.y < 15) then
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["canidae1Spawned"] = 1
				end
			end
		elseif (player.y > 15) then
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["canidae1Spawned"] = 2
				end
			end
			
			for x = 1, 4 do
				players = player:getObjectsInMap(player.m + x, BL_PC)
			
				if (#players > 0) then
					for i = 1, #players do
						players[i].registry["canidae1Spawned"] = 2
					end
				end
			end
		end
	elseif (player.mapTitle == "Cheerful Forest") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["canidae2Spawned"] = 1
			end
		end
		
		for x = 1, 1 do
			players = player:getObjectsInMap(player.m - x, BL_PC)
		
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["canidae2Spawned"] = 1
				end
			end
		end
	elseif (player.mapTitle == "Joyous Forest") then
		if (player.x == 0 and (player.y == 25 or player.y == 26 or player.y == 27) and player.registry["canidae4Spawned"] == 0) then
			pushBack(player)
		else
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["canidae3Spawned"] = 1
				end
			end
			
			for x = 1, 2 do
				players = player:getObjectsInMap(player.m - x, BL_PC)
			
				if (#players > 0) then
					for i = 1, #players do
						players[i].registry["canidae3Spawned"] = 1
					end
				end
			end
		end
	elseif (player.mapTitle == "Glade") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["canidae4Spawned"] = 1
			end
		end
		
		for x = 1, 3 do
			players = player:getObjectsInMap(player.m - x, BL_PC)
				
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["canidae4Spawned"] = 1
				end
			end
		end
	elseif (player.mapTitle == "Twisting Pine") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["canidae5Spawned"] = 1
			end
		end
		
		for x = 1, 4 do
			players = player:getObjectsInMap(player.m - x, BL_PC)
				
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["canidae5Spawned"] = 1
				end
			end
		end
	elseif (player.mapTitle == "Clearing") then
		if (#players > 0) then
			for i = 1, #players do
				players[i].registry["canidae6Spawned"] = 1
			end
		end
		
		for x = 1, 5 do
			players = player:getObjectsInMap(player.m - x, BL_PC)
				
			if (#players > 0) then
				for i = 1, #players do
					players[i].registry["canidae6Spawned"] = 1
				end
			end
		end
	end
end

canidaePushBack = function(player, mobs)
	local pushed = false
	
	if (player.mapTitle == "Merry Forest" and #mobs > 0 and ((player.x == 29 and (player.y == 6 or player.y == 7 or player.y == 21 or player.y == 22 or player.y == 23)) or ((player.x == 6 or player.x == 7 or player.x == 8) and player.y == 29))) then
		pushBack(player)
		pushed = true
	elseif (player.mapTitle == "Cheerful Forest" and #mobs > 0 and ((player.x == 0 and (player.y == 25 or player.y == 26)) or (player.x == 29 and (player.y == 24 or player.y == 25 or player.y == 26)))) then
		pushBack(player)
		pushed = true
	elseif (player.mapTitle == "Joyous Forest") then
		if (#mobs > 0 and ((player.x == 0 and (player.y == 5 or player.y == 6 or player.y == 7)) or (player.x == 29 and (player.y == 11 or player.y == 12 or player.y == 13)))) then
			pushBack(player)
			pushed = true
		elseif (player.x == 0 and (player.y == 25 or player.y == 26 or player.y == 27) and player.registry["canidae4Spawned"] == 0) then
			pushBack(player)
			pushed = true
		end
	elseif (player.mapTitle == "Glade" and #mobs > 0 and (player.x == 0 and (player.y == 9 or player.y == 10 or player.y == 11))) then
		pushBack(player)
		pushed = true
	elseif (player.mapTitle == "Twisting Pine" and #mobs > 0 and ((player.x == 29 and (player.y == 4 or player.y == 5 or player.y == 6)) or (player.x == 0 and (player.y == 7 or player.y == 8 or player.y == 9)))) then
		pushBack(player)
		pushed = true
	elseif (player.mapTitle == "Clearing") then
		if (#mobs <= 1) then
			for x = 0, 5 do
				players = player:getObjectsInMap(player.m - x, BL_PC)
					
				if (#players > 0) then
					for i = 1, #players do
						players[i].registry["canidae6Spawned"] = 1
					end
				end
			end
		else
			pushed = true
		end
	end
	
	return pushed
end

instanceMapLeave = function(player)
	local pcBlocks = player:getObjectsInSameMap(BL_PC)
	local mT = player.mapTitle
	if (mT == "Clearing" or mt == "Glade") then--FOX
		if (#pcBlocks == 1) then
			local fox = {"bomb_fox_balls", "flashing_fox_balls", "fox_balls", "freezing_fox_balls",
			"interim_fox_balls", "lava_fox_balls", "poison_fox_balls", "superbomb_fox_balls",
			"fox_ball_refresh", "boss", "fox_alternate"}
			for i = 1, #fox do
				player.mapRegistry[fox[i]] = 0
			end
		end
	end
end