cub_trap = {
cast = function(player)
	local aether = 25000
	local magicCost = 20
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	player:sendAction(6, 20)
	player:setAether("cub_trap", aether)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:addNPC("cub_trap", player.m, player.x, player.y, 0, 3600000, player.ID)
	player:sendMinitext("You cast Cub Trap.")
	addSpotTrap(player, player.m, player.x, player.y)
end,

click = function(block, npc)
	local damage = 1
	local duration = 2000
	local owner = npc:getBlock(npc.owner)
	
	if (owner == nil or block:hasDuration("cub_trap")) then
		return
	end
	
	damage = damage + owner.level
	
	if (block.blType == BL_MOB) then
		block:playSound(33)
		block:sendAnimation(32, 0)
		block.attacker = owner.ID
		block:removeHealthExtend(damage, 1, 1, 1, 1, 0)
		block:setDuration("cub_trap", duration)
		block.snare = true
		removeTrap(npc)
	elseif (block.blType == BL_PC and owner:canPK(block)) then
		block:playSound(33)
		block:sendAnimation(32, 0)
		block.attacker = owner.ID
		block:removeHealthExtend(damage, 1, 1, 1, 1, 0)
		block:setDuration("cub_trap", duration)
		block:calcStat()
		block:sendMinitext("Your legs have been mauled by a cub.")
		removeTrap(npc)
	end
end,

endAction = function(npc, owner)
	removeTrap(npc)
end,

recast = function(block)
	block.snare = true
end,

uncast = function(block)
	if (block.blType == BL_MOB) then
		block.snare = false
	elseif (block.blType == BL_PC) then
		block:calcStat()
	end
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Sets a trap that will weaken your foes"}
	return level, items, itemAmounts, description
end
}