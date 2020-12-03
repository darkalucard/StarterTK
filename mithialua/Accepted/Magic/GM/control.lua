control = {

cast=function(player, target)
	local g = {}
	local x
	local mob
	
	g = player:getObjectsInArea(BL_MOB)
	
	for x = 1, #g do
		mob = Mob(g[x].ID)
		
		if (mob.owner > 0) then
			if (player:hasDuration("control") == true) then
				if (mob.owner == player.ID) then
					if (target.blType == BL_PC) and (target.pvp == false) then
						mob.target = player.ID
					else
						mob.target = target.ID
					end
				end
			else
				if (mob.owner == player.ID) and (mob.state == 0) then
					if (target.blType == BL_PC) and (target.pvp == false) then
						mob.target = player.ID
					else
						mob.target = target.ID
					end
				end
			end
		end
	end
	
	if (player:hasDuration("control") == false) then
		player:setDuration("control", 2000)
	end
end,

}