rampage = {
cast = function(player)
	local duration = 600000
	local magicCost = 75
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (player:hasDuration("rampage")) then
		player:sendMinitext("That spell is already cast.")
		return
	end
	
	player:sendAction(6, 20)
	player:setDuration("rampage", duration)
	player:sendAnimation(71, 0)
	player:playSound(7)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:calcStat()
	player:sendMinitext("You cast Rampage.")
end,

recast = function(player)
	player.flank = true
	player.backstab = true
end,

uncast = function(player)
	player.flank = false
	player.backstab = false
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"A weak attack that targets three foes"}
	return level, items, itemAmounts, description
end
}