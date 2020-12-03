killmobs = {
cast=function(player)
	local mobBlocks = player:getObjectsInSameMap(BL_MOB)
	local counter = 0
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			target = mobBlocks[i]
			if(target.blType==BL_MOB) then
				target.attacker=player.ID
				if (target.subType==4) then
					target:removeHealth(target.maxHealth*0.25)
					target:removeHealth(1)
					target:sendAnimation(14)
				else
					target:removeHealth(target.health-1)
					target:removeHealth(target.health)
					target:sendAnimation(14)
					counter = counter + 1
				end
			else
				target.attacker=player.ID
				target:addHealth(-target.health+1)
				target:addHealth(-target.health)
				target:sendAnimation(14)
			end
			player:sendMinitext(counter.."/"..#mobBlocks.." were killed.")
		end
	end
end       
}