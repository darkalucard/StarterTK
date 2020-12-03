afflict = {
cast = function(player, target)
	local duration = 60000
	local magicCost = 30
	
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
	
	if (target:hasDuration("curse") or target:hasDuration("plague") or target:hasDuration("destructive_tune")) then
		player:sendMinitext("A stronger version is already running.")
		return
	end
	
	if (target:hasDuration("afflict")) then
		player:sendMinitext("That spell is already cast.")
		return
	end
	
	if (target.blType == BL_MOB) then
		player:sendAction(6, 20)
		player.magic = player.magic - magicCost
		player:sendStatus()
		player:playSound(24)
		player:sendMinitext("You cast Afflict.")
		target:setDuration("afflict", duration)
		target:sendAnimation(144, 0)
	elseif (target.blType == BL_PC and player:canPK(target)) then
		player:sendAction(6, 20)
		player.magic = player.magic - magicCost
		player:sendStatus()
		player:playSound(24)
		
		if (checkProtection(player, target, 1)) then
			player:sendMinitext("Afflict has been deflected.")
		else
			player:sendMinitext("You cast Afflict.")
			target:setDuration("afflict", duration)
			target:sendAnimation(144, 0)
			target:sendMinitext(player.name.." afflicts you.")
			target:calcStat()
		end
	end
end,

while_cast = function(block)
	if (block.blType == BL_MOB and block.charState ~= 2) then
		block:sendAnimation(34, 0)
	elseif (block.blType == BL_PC and block.state ~= 2) then
		block:sendAnimation(34, 0)
	end
end,

recast = function(block)
	block.armor = block.armor * .8
end,

uncast = function(block)
	block:calcStat()
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Reduces armor by a small amount on a target"}
	return level, items, itemAmounts, description
end
}