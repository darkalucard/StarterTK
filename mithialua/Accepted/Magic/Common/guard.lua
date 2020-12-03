guard = {
cast = function(player, target)
	local duration = 240000
	local magicCost = 60
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (target.state == 1) then
		player:sendMinitext("That cannot save them now.")
		return
	end
	
	if (target:hasDuration("protect") or target:hasDuration("barrier") or target:hasDuration("divine_ballad")) then
		player:sendMinitext("A stronger version is already running.")
		return
	end
	
	player:sendAction(6, 20)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:sendMinitext("You cast Guard.")
	target:setDuration("guard", duration, 0, 1)
	player:playSound(92)
	target:sendAnimation(295, 0)
	
	if (target.blType == BL_MOB) then
		target.deduction = target.deduction - .15
	elseif (target.blType == BL_PC) then
		target:calcStat()
		target:sendMinitext(player.name.." guards you.")
	end
end,

recast = function(player)
	player.deduction = player.deduction - .15
end,

uncast = function(block)
	if (block.blType == BL_MOB) then
		block.deduction = block.deduction + .15
	elseif (block.blType == BL_PC) then
		block:calcStat()
	end
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Reduces damage by a small amount on a target"}
	return level, items, itemAmounts, description
end
}