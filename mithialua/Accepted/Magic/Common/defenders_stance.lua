defenders_stance = {
cast = function(player)
	local aether = 40000
	local duration = 20000
	local magicCost = 50
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (player:hasDuration("defenders_stance")) then
		player:sendMinitext("That spell is already cast.")
		return
	end
	
	player:sendAction(6, 20)
	player:setDuration("defenders_stance", duration)
	player:setAether("defenders_stance", aether)
	player:playSound(27)
	player:sendAnimation(220, 0)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:calcStat()
	player:sendMinitext("You cast Defender's Stance.")
end,

recast = function(player)
	player.armor = player.armor * 1.2
end,

uncast = function(player)
	player:calcStat()
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Brace your resolve strengthening your armor by a small amount"}
	return level, items, itemAmounts, description
end
}