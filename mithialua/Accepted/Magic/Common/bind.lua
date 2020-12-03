bind = {
cast = function(player)
	local magicCost = 50
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (player.bind ~= 1) then
		player:sendMinitext("You cannot bind here.")
		return
	end
	
	player:sendAction(6, 20)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player.bindMap = player.m
	player.bindX = player.x
	player.bindY = player.y
	player:sendMinitext("You cast Bind.")
end,

on_forget = function(player)
	player.registry["learned_bind"] = 0
end,

requirements = function(player)
	local level = 10
	local items = {}
	local itemAmounts = {}
	local description = {"Creates a point of return from the shaman"}
	return level, items, itemAmounts, description
end
}