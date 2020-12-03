gm_sage = {
cast=function(player)
	local question = player.question
	if (player.gmLevel > 0) then
		if(string.len(question) > 0) then
			player:broadcast(-1, "[GM "..player.name.."]: "..question)
		else
			player:sendMinitext("Can't send an empty broadcast")
		end
		player:broadcast("Player "..player.name.."has a GM spell.", -1)
	end
end
}