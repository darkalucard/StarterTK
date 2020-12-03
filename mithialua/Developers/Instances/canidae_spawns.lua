canidae_spawns = {
room1 = function(player)
	--Room 1
	local merry = 87
	local cheerful = 88
	local joyous = 89
	local swift = 90
	local striking = 91
	local sly = 92
	
	local group = #player.group
	if (group > 8) then
		group = ((group - 8) / 10) + 1
	else
		group = 1
	end
	if (player.registry["canidae1Spawned"] == 0 and #player:getObjectsInSameMap(BL_MOB) == 0
	and player.y < 15) then
		player:spawn(merry, 7, 6, math.floor(7 * group), player.m)
		player:spawn(cheerful, 7, 6, math.floor(5 * group), player.m)
		player:spawn(joyous, 7, 6, math.floor(4 * group), player.m)
		player:spawn(swift, 7, 6, math.floor(3 * group), player.m)
		player:spawn(striking, 7, 6, math.floor(3 * group), player.m)
		player:spawn(sly, 7, 6, math.floor(2 * group), player.m)	
	elseif (player.registry["canidae1Spawned"] == 1 and #player:getObjectsInSameMap(BL_MOB) == 0
	and player.y > 15) then
		player:spawn(merry, 8, 22, math.floor(5 * group), player.m)
		player:spawn(cheerful, 8, 22, math.floor(5 * group), player.m)
		player:spawn(joyous, 8, 22, math.floor(2 * group), player.m)
		player:spawn(swift, 8, 22, math.floor(5 * group), player.m)
		player:spawn(striking, 8, 22, math.floor(5 * group), player.m)
		player:spawn(sly, 8, 22, math.floor(3 * group), player.m)	
	end
end,

room2 = function(player)
	--Room 2
	if (player.registry["canidae2Spawned"] ~= 0 or #player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	local merry = 87
	local cheerful = 88
	local joyous = 89
	local swift = 90
	local striking = 91
	local sly = 92
	
	local group = #player.group
	if (group > 8) then
		group = ((group - 8) / 10) + 1
	else
		group = 1
	end
	player:spawn(merry, 8, 22, math.floor(7 * group), player.m)
	player:spawn(cheerful, 8, 22, math.floor(10 * group), player.m)
	player:spawn(joyous, 8, 22, math.floor(8 * group), player.m)
	player:spawn(swift, 8, 22, math.floor(8 * group), player.m)
	player:spawn(striking, 8, 22, math.floor(8 * group), player.m)
	player:spawn(sly, 8, 22, math.floor(4 * group), player.m)	
end,

room3 = function(player)
	--Room 3
	if (player.registry["canidae3Spawned"] ~= 0 or #player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	local merry = 87
	local cheerful = 88
	local joyous = 89
	local swift = 90
	local striking = 91
	local sly = 92
	
	local group = #player.group
	if (group > 8) then
		group = ((group - 8) / 10) + 1
	else
		group = 1
	end
	player:spawn(merry, 8, 22, math.floor(5 * group), player.m)
	player:spawn(cheerful, 8, 22, math.floor(6 * group), player.m)
	player:spawn(joyous, 8, 22, math.floor(8 * group), player.m)
	player:spawn(swift, 8, 22, math.floor(7 * group), player.m)
	player:spawn(striking, 8, 22, math.floor(7 * group), player.m)
	player:spawn(sly, 8, 22, math.floor(4 * group), player.m)
end,

room4 = function(player)
	--Room 4
	if (player.registry["canidae4Spawned"] ~= 0 or #player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	local fake = 137
	player:spawn(fake, 8, 6, 1, player.m)	
end,

room5 = function(player)
	--Room 5
	if (player.registry["canidae5Spawned"] ~= 0 or #player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	
	local merry = 87
	local cheerful = 88
	local joyous = 89
	local swift = 90
	local striking = 91
	local sly = 92
	
	local group = #player.group
	if (group > 8) then
		group = ((group - 8) / 10) + 1
	else
		group = 1
	end	
	player:spawn(merry, 8, 22, math.floor(8 * group), player.m)
	player:spawn(cheerful, 8, 22, math.floor(11 * group), player.m)
	player:spawn(joyous, 8, 22, math.floor(8 * group), player.m)
	player:spawn(swift, 8, 22, math.floor(9 * group), player.m)
	player:spawn(striking, 8, 22, math.floor(9 * group), player.m)
	player:spawn(sly, 8, 22, math.floor(4 * group), player.m)
end,

room6 = function(player)
	--Room 6
	if (#player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	
	if (#player:getObjectsInSameMap(BL_MOB) == 0) then
		player:spawn(138, 0, 7, 1, player.m)
	end

	if (player.registry["canidae6Spawned"] == 0) then
		local ninetails = 93
		player:spawn(ninetails, 9, 9, 1, player.m)
	end
end
}