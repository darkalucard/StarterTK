increase_attack_speed={
	cast=function(player,target)
		target.attackSpeed=100
		target:sendMinitext("Your attack speed increases!")
		target:setDuration("increase_attack_speed",200000)
	end,
	uncast=function(player)
		player.attackSpeed=0
		player:sendMinitext("Your attack speed is back to normal.")
	end
}