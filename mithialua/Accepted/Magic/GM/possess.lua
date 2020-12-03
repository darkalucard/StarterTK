possess = {

cast=function(player, target)
	if (target.owner == 0) then
		target.owner = player.ID
		target.target = player.ID
	else
		if (target:hasDuration("possess") == true) then
			player:sendMinitext(""..target.name.." \a Last owner: "..Player(target.owner).name)
			target.owner = player.ID
			target.target = player.ID
			target:setDuration("possess",0)
		else
			target:setDuration("possess", 5000)
			
			if (target.summon == true) then
				player:sendMinitext(""..target.name.." \a Owner: "..Player(target.owner).name.." \a Summon: Yes.")
			else
				player:sendMinitext(""..target.name.." \a Owner: "..Player(target.owner).name.." \a Summon: No.")
			end
		end
	end
end,

}