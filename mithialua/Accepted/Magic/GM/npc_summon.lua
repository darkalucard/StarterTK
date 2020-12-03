npc_summon = {
cast = function (player)
	local nP = player.question
	local str, t
	if (nP~=nil) then
		if (string.byte(nP) <= 57) and (string.byte(nP) >= 48) then--num
			t = NPC(nP)
			t:warp(player.m, player.x, player.y)
			player:refresh()
		else
			player:sendMinitext("ID expected. Can not use ynames to summon.")
		end
	end
end
}