criminal_judgement = {
cast = function(player)
	if (player.blType ~= BL_PC) then
		return
	end
	player:sendMinitext("You must be quite dumb to cast this spell.")
	if (player:hasDuration("criminal_judgement") == false) then
		if (player.registry["warden"] > 0) then
			player:warp(134, 8, 8)
		else
			player:setDuration("criminal_judgement", 5000)
		end
	end
end,
while_cast = function(player)
	if (player.m ~= 134) then
		player:calcStat()
	end
end,
recast = function(player)
	if (player.m ~= 134) then
		justice.jail(player)
	end
end,
uncast = function(player)
	player:warp(134, 8, 8)
	player:gmMsg(player.name.." has been freed from jail.")
	addChatLog(player, player.name.." has been freed from jail.")
end
}