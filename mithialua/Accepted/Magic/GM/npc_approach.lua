npc_approach = {
cast = function (player)
	local nP = player.question
	local str, t
	if (nP ~= nil) then
		if (NPC(nP) ~= nil) then
			t = NPC(nP)
			player:warp(t.m, t.x, t.y)
		else
			player:sendMinitext("Does not exist")
		end
		--[[if (string.byte(nP) <= 57) and (string.byte(nP) >= 48) then--numerical
			t = NPC(nP)
			player:warp(t.m, t.x, t.y)
		else
			player:sendMinitext("ID expected.")
		end]]--
	end
end
}