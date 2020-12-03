controlthings={
	cast=function(player,target)
		if target.blType==BL_MOB then
			target:sendAnimation(341)
			target.controlled=true
			target.controller=player.ID
			target.target=player.target
		end
	end
}