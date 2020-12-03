increase_speed={
	cast=function(player,target)
		target.speed=40
                target:sendMinitext("Your speed increases!")
		target:setDuration("increase_speed",10000)
		target:updateState()
	end,
	uncast=function(player)
		player.speed=80
		player:sendMinitext("Your speed is back to normal.")
		player:updateState()
	end
}