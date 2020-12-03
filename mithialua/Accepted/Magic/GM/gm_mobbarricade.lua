gm_mobbarricade = {
cast = function(player)
	player:setDuration("gm_mobbarricade", 60000)
end,

on_walk_while_cast = function(player)
	player:spawn(110, player.x, player.y, 1)
end
}