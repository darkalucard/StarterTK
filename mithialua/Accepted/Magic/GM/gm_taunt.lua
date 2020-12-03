gm_taunt = {
cast = function(player)
	local targets = player:getObjectsInSameMap(BL_MOB)
	
	for i = 1, #targets do
		targets[i].attacker = player.ID
		targets[i].target = player.ID
	end
	
	player:talk(1, player.name.."! Get over here!")
end
}