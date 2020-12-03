spot_traps = {

cast = function(player, target)
	player:setDuration("spot_traps",180000)
	player:calcStat()
end,

uncast = function(player, target)
	player.spotTraps = false
	player:refresh()
end,

recast = function(player)
	player.spotTraps = true
	player:refresh()
end,

}