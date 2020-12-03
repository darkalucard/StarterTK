arvagus_test = {
cast = function(player)
	--[[local mobBlocks
	
	for x = 0, player.xmax do
		for y = 0, player.ymax do
			mobBlocks = player:getObjectsInCell(player.m, x, y, BL_MOB)
			
			if ((x <= 2 and y <= 2) or (x >= player.xmax - 2 and y <= 2) or (x >= player.xmax - 2 and y >= player.ymax - 2) or (x <= 2 and y >= player.ymax - 2)) then
			else
				if (getPass(player.m, x, y) == 0 and math.random(100) < 60 and #mobBlocks < 1) then
					player:spawn(52, x, y, 1)
				end
			end
		end
	end]]--
end,

while_passive = function(player)
	--[[player:testPacket({170, 0, 19, 8, 3, 0, 0unknown, 5, 4, 0unknown, 40, 0, 0, 0, 50, 0, 0, 0, 50,
	1, 1, 3fishy, 3fishy, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0all unknown, 52, 0, 0, 0, 25, 0, 0, 0, 25,
	0, 0, 0, 5, 0, 0, 0, 5, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12605})]]
end
}