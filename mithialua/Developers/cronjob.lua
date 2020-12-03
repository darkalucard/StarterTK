cronJobSec = function()
	block = NPC(4294967295)
	
	if (#openingSceneQueue > 0) then
		if (#openingSceneUsedMaps < opening_scene.maxmaps) then
			for i = 0, opening_scene.maxmaps - #openingSceneUsedMaps do
				local map = opening_scene.getfreemap()
				
				opening_scene.startscene(Player(openingSceneQueue[1]), map)
			end
		end
	end
	--[[
	if (Player(4) ~= nil) then
		p = Player('Krisalia')
		local anim = {278, 337, 389}
		local times = math.random(8, 24)
		local r = math.random(1,3)
		
		--if (xtable == nil) then
			local xtable = {}
			local ytable = {}
		--end
		
		if (launch == nil) then
			local launch = 0
		end
		
		if (realMinute() % 10 == 0) then
			p.mapRegistry['fireworks'] = os.time()
		end
		
		if (p.mapRegistry['fireworks'] + 300 >= os.time()) then
			if (os.time() % 2 == 0) then
				if (launch == 0) then
					for i = 1, times do
						local x = math.random(p.x - 9, p.x + 9)
						local y = math.random(p.y - 8, p.y + 8)
					
						table.insert(xtable, i, x)
						table.insert(ytable, i, y)
					
						p:sendAnimationXY(131, xtable[i], ytable[i])
					end
					
					launch = 1
				else
					for i = 1, #xtable do
						p:sendAnimationXY(anim[r], xtable[i], ytable[i])
						table.remove(xtable, i)
						table.remove(ytable, i)
					end
					
					launch = 0
				end
			end
		end
	end]]--
	--[[if ((realMinute()%15==0) and (realSecond()%30 == 0)) then
		broadcast(-1, "Day: "..realDay().." hour: "..realHour().." minute: "..realMinute().." second: "..realSecond()..".")
	end]]--
end





cronJobMin = function()
	block = NPC(4294967295)
	
	if (realMinute()%15 == 0) then
		local timestamp = realHour()..":"..realMinute()
		broadcast(-1, "Day: "..realDay().." hour: "..realHour().." minute: "..realMinute().." second: "..realSecond()..".")
		--broadcast(-1, "["..timestamp.."] Ghari: I will be hosting 2 events at 20:00 Server time")
	end
	
	if (realMinute()%30 == 0) then
		clearInstanceMaps(block)
	end
	
	if (#openingSceneQueue > 0) then
		for i = 1, #openingSceneQueue do
			Player(openingSceneQueue[i]):msg(0, "You are position "..i.." in the queue for the opening scene.", openingSceneQueue[i])
		end
	end
end





cronJobHour = function()
	block = NPC(4294967295)
	--broadcast(-1, "[[ANNOUNCEMENT]]: Maintenance tonight, read GM Announcements for more details.")
	--broadcast(-1, "Day: "..realDay().." hour: "..realHour().." minute: "..realMinute().." second: "..realSecond()..".")
end





cronJobDay = function()
	block = NPC(4294967295)
	--broadcast(-1, "Day: "..realDay().." hour: "..realHour().." minute: "..realMinute().." second: "..realSecond()..".")
end





clearInstanceMapse = function(block)
	local i
	
	if (#openingSceneUsedMaps > 0) then
		i = 0
		clearOpeningScene(block, i)
	end
	
	if (#instances > 0) then
		i = 0
		clearInstances(block, i)
	end
	
	block:warp(0, 0, 0)
end

clearOpeningScene = function(block, i)
	repeat
		i = i + 1
		
		local openingPlayers = #block:getObjectsInMap(openingSceneUsedMaps[i], BL_PC)
		
		if (openingPlayers == 0) then
			table.remove(openingSceneUsedMaps, i)
			i = i - 1
		end
	until (i == #openingSceneUsedMaps)
end

clearInstances = function(block, i)
	repeat
		i = i + 1
		
		block:warp(instances[i], 0, 0)
		
		if (block.mapTitle == "Ruins") then
			i = clearRuins(block)
		elseif (block.mapTitle == "Merry Forest") then
			i = clearCanidae(block)
		end
	until (i == #instances)
end

clearRuins = function(block)
	local room1Players = #block:getObjectsInMap(instances[i], BL_PC)
	local room2Players = #block:getObjectsInMap(instances[i + 1], BL_PC)
	local room3Players = #block:getObjectsInMap(instances[i + 2], BL_PC)
	local room4Players = #block:getObjectsInMap(instances[i + 3], BL_PC)
	local room5Players = #block:getObjectsInMap(instances[i + 4], BL_PC)

	if (room1Players + room2Players + room3Players + room4Players + room5Players == 0) then
		unloadInstance(instances[i], 5)
		return 0
	end
end

clearCanidae = function(block)
	local room1Players = #block:getObjectsInMap(instances[i], BL_PC)
	local room2Players = #block:getObjectsInMap(instances[i + 1], BL_PC)
	local room3Players = #block:getObjectsInMap(instances[i + 2], BL_PC)
	local room4Players = #block:getObjectsInMap(instances[i + 3], BL_PC)
	local room5Players = #block:getObjectsInMap(instances[i + 4], BL_PC)
	local room6Players = #block:getObjectsInMap(instances[i + 5], BL_PC)

	if (room1Players + room2Players + room3Players + room4Players + room5Players + room6Players == 0) then
		unloadInstance(instances[i], 6)
		return 0
	end
end