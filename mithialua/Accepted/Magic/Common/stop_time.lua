stop_time = {
cast = function(player, target)
	local aether = 1000
	local duration = 20000
	local magicCost = 20
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (target.state == 1 or target.blType ~= BL_MOB) then
		player:sendMinitext("That is no longer useful.")
		return
	end
	
	player:sendAction(6, 20)
	player:setAether("stop_time", aether)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:sendMinitext("You cast Stop Time.")
	player:playSound(20)
	
	if (target:hasDuration("break_time")) then
		target:setDuration("break_time", 0)
	end
	
	target:sendAnimation(258, 0)
	target:setDuration("stop_time", duration, 0, 1)
	target.paralyzed = true
end,

recast = function(mob)
	mob.paralyzed = true
end,

uncast = function(mob)
	mob.paralyzed = false
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Freezes a foe in place for a short time"}
	return level, items, itemAmounts, description
end
}