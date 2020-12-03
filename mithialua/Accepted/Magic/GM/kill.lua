kill = {
	cast=function(player,target)
		if(target.blType==BL_MOB) then
			target.attacker=player.ID
			if (target.subType==4) then
				target:removeHealth(target.maxHealth*0.25)
				target:removeHealth(1)
			else
				target:removeHealth(target.health-1)
				target:removeHealth(target.health)
			end
        else 
			target.attacker=player.ID
			target:addHealth(-target.health+1)
			target:addHealth(-target.health)
        end               
    end       
}