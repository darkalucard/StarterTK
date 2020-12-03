summon = {
cast=function(player,target)
	local question = Player(player.question)

	if(question == nil) then
		player:sendMinitext("They are not online.")
	else
		if (player.region == question.region or player.gmLevel > 50) then
			question:warp(player.m, player.x, player.y)
		else
			player:sendMinitext("That player is in a different region from yours.")
			player:setAether("summon", 5000)
		end
	end
end,
on_aethers = function(player)
	local question=Player(player.question)

	if(question == nil) then
		player:sendMinitext("They are not online.")
	else
		if (player.region == question.region) then
			question:warp(player.m, player.x, player.y)
		else
			question:warp(player.m, player.x, player.y)
			player:sendMinitext("You have summoned "..player.question.." from region "..question.region".")
		end
	end
end
}