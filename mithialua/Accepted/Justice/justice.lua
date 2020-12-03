justice = {

jail = function(player, target)
	local cells = {
	{math.random(1, 5), math.random(3, 5)},
	{math.random(1, 5), math.random(10, 12)},
	{math.random(11, 15), math.random(4, 5)},
	{math.random(11, 15), math.random(10, 12)}
	}
	if (target == nil) then
		target = player
	end
	local randCell = math.random(#cells)
	
	if (player.ID == target.ID) then
		addChatLog(player, "SYSTEM: "..target.name.." has been jailed.")
		player:gmMsg("SYSTEM: "..target.name.." has been jailed.")
	else
		addChatLog(player, ""..player.name..": "..target.name.." has been jailed.")
		player:gmMsg(""..player.name..": "..target.name.." has been jailed.")
	end
	target:warp(134, cells[randCell][1], cells[randCell][2])
end
}