brace = {
cast = function(player, target)
	local duration = 240000
	local magicCost = 40
	
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
	
	if (target:hasDuration("bolster") or target:hasDuration("reinforce") or target:hasDuration("divine_ballad")) then
		player:sendMinitext("A stronger version is already running.")
		return
	end
	
	player:sendAction(6, 20)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:sendMinitext("You cast Brace.")
	target:setDuration("brace", duration, 0, 1)
	player:playSound(92)
	target:sendAnimation(110, 0)
	
	if (target.blType == BL_PC) then
		target:sendMinitext(player.name.." braces your armor.")
	end
	
	target:calcStat()
end,

recast = function(block)
	block.armor = block.armor * 1.2
end,

uncast = function(block)
	block:calcStat()
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Increases armor by a small amount on a target"}
	return level, items, itemAmounts, description
end
}