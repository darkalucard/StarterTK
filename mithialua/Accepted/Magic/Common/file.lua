file = {
cast = function(player)
	local aether = 30000
	local duration = 600000
	local magicCost = 100
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (player:hasDuration("sharpen") or player:hasDuration("hone")) then
		player:sendMinitext("A stronger version is already running.")
		return
	end
	
	if (player:hasDuration("file")) then
		player:sendMinitext("That spell is already cast.")
		return
	end
	
	player:sendAction(6, 20)
	player:setDuration("file", duration)
	player:setAether("file", aether)
	player:playSound(100)
	player:sendAnimation(107, 0)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:calcStat()
	player:sendMinitext("You cast File.")
end,

recast = function(player)
	player.enchant = player.enchant + 2
	player.hit = player.hit + 3
end,

uncast = function(player)
	player:calcStat()
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Prepares your weapon for battle"}
	return level, items, itemAmounts, description
end
}