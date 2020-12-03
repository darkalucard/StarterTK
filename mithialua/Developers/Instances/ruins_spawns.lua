ruins_spawns = {
room1 = function(player)
	--Room 1
	if (player.registry["ruins1Spawned"] ~= 0 or #player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	
	player:spawn(123, 14, 32, 1, player.m)
	player:spawn(123, 18, 36, 1, player.m)
	player:spawn(123, 31, 37, 1, player.m)
	player:spawn(123, 31, 27, 1, player.m)
	player:spawn(123, 21, 18, 1, player.m)
	player:spawn(123, 28, 4, 1, player.m)
	player:spawn(123, 4, 6, 1, player.m)
	player:spawn(123, 7, 22, 1, player.m)
	player:spawn(123, 3, 36, 1, player.m)
	player:spawn(123, 3, 30, 1, player.m)
	player:spawn(123, 15, 35, 1, player.m)
	player:spawn(124, 10, 6, 1, player.m)
	player:spawn(124, 20, 11, 1, player.m)
	player:spawn(124, 29, 4, 1, player.m)
	player:spawn(124, 22, 20, 1, player.m)
	player:spawn(124, 30, 30, 1, player.m)
	player:spawn(124, 23, 37, 1, player.m)
	player:spawn(124, 11, 33, 1, player.m)
	player:spawn(124, 4, 25, 1, player.m)
	player:spawn(124, 13, 17, 1, player.m)
	player:spawn(124, 5, 39, 1, player.m)
end,

room2 = function(player)
	--Room 2
	if (player.registry["ruins2Spawned"] ~= 0 or #player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	
	player:spawn(123, 19, 33, 1, player.m)
	player:spawn(123, 27, 29, 1, player.m)
	player:spawn(123, 17, 21, 1, player.m)
	player:spawn(123, 25, 12, 1, player.m)
	player:spawn(123, 9, 7, 1, player.m)
	player:spawn(124, 7, 8, 1, player.m)
	player:spawn(124, 23, 8, 1, player.m)
	player:spawn(124, 17, 17, 1, player.m)
	player:spawn(124, 28, 27, 1, player.m)
	player:spawn(124, 19, 36, 1, player.m)
	player:spawn(125, 23, 28, 1, player.m)
	player:spawn(125, 23, 7, 1, player.m)
	player:spawn(126, 9, 8, 1, player.m)
	player:spawn(126, 25, 33, 1, player.m)
end,

room3 = function(player)
	--Room 3
	if (player.registry["ruins3Spawned"] ~= 0 or #player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	
	player:spawn(123, 18, 6, 1, player.m)
	player:spawn(123, 5, 12, 1, player.m)
	player:spawn(123, 10, 18, 1, player.m)
	player:spawn(123, 2, 26, 1, player.m)
	player:spawn(123, 10, 33, 1, player.m)
	player:spawn(124, 12, 35, 1, player.m)
	player:spawn(124, 1, 27, 1, player.m)
	player:spawn(124, 14, 25, 1, player.m)
	player:spawn(124, 9, 16, 1, player.m)
	player:spawn(124, 8, 7, 1, player.m)
	player:spawn(125, 15, 7, 1, player.m)
	player:spawn(125, 14, 25, 1, player.m)
	player:spawn(126, 4, 30, 1, player.m)
	player:spawn(126, 14, 7, 1, player.m)
end,

room4 = function(player)
	--Room 4
	if (player.registry["ruins4Spawned"] ~= 0 or #player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	
	player:spawn(123, 9, 29, 1, player.m)
	player:spawn(123, 16, 23, 1, player.m)
	player:spawn(123, 9, 16, 1, player.m)
	player:spawn(123, 9, 7, 1, player.m)
	player:spawn(123, 16, 4, 1, player.m)
	player:spawn(123, 24, 9, 1, player.m)
	player:spawn(123, 29, 16, 1, player.m)
	player:spawn(123, 21, 26, 1, player.m)
	player:spawn(123, 29, 29, 1, player.m)
	player:spawn(123, 15, 32, 1, player.m)
	player:spawn(123, 20, 25, 1, player.m)
	player:spawn(123, 12, 19, 1, player.m)
	player:spawn(123, 22, 19, 1, player.m)
	player:spawn(124, 26, 29, 1, player.m)
	player:spawn(124, 16, 27, 1, player.m)
	player:spawn(124, 7, 31, 1, player.m)
	player:spawn(124, 13, 18, 1, player.m)
	player:spawn(124, 5, 12, 1, player.m)
	player:spawn(124, 16, 10, 1, player.m)
	player:spawn(124, 16, 3, 1, player.m)
	player:spawn(124, 21, 9, 1, player.m)
	player:spawn(124, 25, 17, 1, player.m)
	player:spawn(124, 31, 13, 1, player.m)
	player:spawn(124, 21, 28, 1, player.m)
	player:spawn(124, 18, 32, 1, player.m)
	player:spawn(124, 12, 25, 1, player.m)
	player:spawn(124, 22, 16, 1, player.m)
	player:spawn(125, 21, 7, 1, player.m)
	player:spawn(125, 13, 18, 1, player.m)
	player:spawn(125, 25, 30, 1, player.m)
	player:spawn(125, 14, 29, 1, player.m)
	player:spawn(125, 19, 24, 1, player.m)
	player:spawn(126, 9, 27, 1, player.m)
	player:spawn(126, 15, 17, 1, player.m)
	player:spawn(126, 25, 5, 1, player.m)
	player:spawn(126, 4, 29, 1, player.m)
	player:spawn(126, 30, 29, 1, player.m)
end,

room5 = function(player)
	--Room 5
	local mobs
	
	if (#player:getObjectsInSameMap(BL_MOB) > 0) then
		return
	end
	
	player:spawn(136, 3, 3, 1, player.m)
	mobs = player:getObjectsInSameMap(BL_MOB)
	mobs[1].side = 2
	mobs[1]:sendSide()
	
	if (player.registry["ruins5Spawned"] == 0) then
		player:spawn(135, 8, 11, 1, player.m)
	end
	
	player:spawn(134, 9, 6, 1, player.m)
end
}