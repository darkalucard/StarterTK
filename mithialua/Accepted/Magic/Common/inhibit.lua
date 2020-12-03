inhibit = {
cast = function(player, target)
	local aether = 30000
	local duration = 3000
	local magicCost = 100
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (target.state == 1) then
		player:sendMinitext("That is no longer useful.")
		return
	end
	
	if (target:hasDuration("restrain") or target:hasDuration("suppress")) then
		player:sendMinitext("A stronger version is already running.")
		return
	end
	
	if (target:hasDuration("inhibit")) then
		player:sendMinitext("That spell is already cast.")
		return
	end
	
	if (target.blType == BL_MOB) then
		player:sendAction(6, 20)
		player:setAether("inhibit", aether)
		player.magic = player.magic - magicCost
		player:sendStatus()
		player:sendMinitext("You cast Inhibit.")
		target:setDuration("inhibit", duration)
		player:playSound(41)
		target:sendAnimation(3, 0)
		target.deduction = target.deduction + .1
		target.snare = true
	elseif (target.blType == BL_PC and player:canPK(target)) then
		player:sendAction(6, 20)
		player:setAether("inhibit", aether)
		player.magic = player.magic - magicCost
		player:sendStatus()
		player:sendMinitext("You cast Inhibit.")
		target:setDuration("inhibit", duration)
		player:playSound(41)
		target:sendAnimation(3, 0)
		target:calcStat()
		target:sendMinitext(player.name.." inhibits you.")
	end
end,

while_cast = function(block)
	block:sendAnimation(34, 0)
end,

recast = function(player)
	player.deduction = player.deduction + .1
	player.snare = true
end,

uncast = function(block)
	if (block.blType == BL_MOB) then
		block.deduction = block.deduction - .1
		block.snare = false
	elseif (block.blType == BL_PC) then
		block:calcStat()
	end
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Weakens a target causing them to take more damage"}
	return level, items, itemAmounts, description
end
}