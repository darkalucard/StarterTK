gm_aether = {
	cast=function(player,target)
	local x
		
		if(player.gmLevel<1) then
			player:sendMinitext("You do not understand how to use this spell.")
			return
		end
		--[[for x=1,1000 do
			if (player:hasAether(x)) then
				target:setAether (x, 0)
			end
		end]]--
		target:flushAether(5)
		--target:sendStatus()
		target:sendAnimation(106)--75 90 106
		player:playSound(13)--13
		if(player.ID~=target.ID) then
			player:sendMinitext("GM "..player.name.." dissipated your aethers.")
		end
		player:sendMinitext("You dissipated "..target.name.."'s aethers.")
		player:sendAction(6,35)		
	end
}