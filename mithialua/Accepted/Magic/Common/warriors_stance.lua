warriors_stance = {
cast = function(player)
	local aether = 30000
	local duration = 600000
	local magicCost = 50
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (player:hasDuration("warriors_stance")) then
		player:sendMinitext("That spell is already cast.")
		return
	end
	
	player:sendAction(6, 20)
	player:setDuration("warriors_stance", duration)
	player:setAether("warriors_stance", aether)
	player:playSound(4)
	player:sendAnimation(208, 0)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:calcStat()
	player:sendMinitext("You cast Warrior's Stance.")
end,

recast = function(player)
	player.enchant = player.enchant + 2
	player.dam = player.dam + 3
end,

uncast = function(player)
	player:calcStat()
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Increases your fighting ability by a small amount"}
	return level, items, itemAmounts, description
end
}