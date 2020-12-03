soothe = {
cast = function(player)
	local heal = 15
	local magicCost = 5
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	player:sendAnimation(5, 0)
	player.magic = player.magic - magicCost
	player.attacker = player.ID
	player:addHealthExtend(heal, 0, 0, 0, 0, 0)
	player:sendStatus()
	player:sendAction(6, 40)
end,

on_forget = function(player)
	player.registry["learned_soothe"] = 0
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Heals very small wounds on your self"}
	
	return level, items, itemAmounts, description
end
}


