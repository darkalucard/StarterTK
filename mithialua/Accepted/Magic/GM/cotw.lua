cotw = {

cast = function(player, target)
	local g
	
	player:spawn(6,player.x,player.y,1)
	g = player:getObjectsInCell(player.m, player.x, player.y, BL_MOB)
	Mob(g[1].ID).owner = player.ID
	Mob(g[1].ID).target = player.ID
	Mob(g[1].ID).summon = true
end,

}