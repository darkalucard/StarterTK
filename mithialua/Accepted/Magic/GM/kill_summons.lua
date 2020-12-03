kill_summons = {

cast=function(player, target)
	local g = player:getObjectsInArea(BL_MOB)
	local x
	
	for x = 1, #g do
		target = Mob(g[x].ID)
		if(target.owner == player.ID) then
			if(target.summon == true) then
				target.target = 0
				target.attacker = player.ID
				target:removeHealth(target.health - 1)
				target:removeHealth(target.health)
				target.attacker = 0
			end
		end
	end
end,

}