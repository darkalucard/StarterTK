gm_objbarricade = {
cast = function(player)
	player:setDuration("gm_objbarricade", 60000)
end,

on_walk_while_cast = function (player)
	setBarricade(player)
end
}