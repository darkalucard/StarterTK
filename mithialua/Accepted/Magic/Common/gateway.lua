gateway = {
cast = function(player)
	local magicCost = 50
	local gate = string.lower(player.question)
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	player.magic = player.magic - magicCost
	player:sendStatus()
	
	if (player.region == 2 and player.warpOut == 1) then
		if (gate == "n" and string.sub(gate, 1, 1) == "n") then
			local x = math.random(77, 80)
			local y = math.random(9, 11)
			
			player:warp(40, x, y)
			player:sendMinitext("You have arrived at North gate.")
		elseif (gate == "e" or string.sub(gate, 1, 1) == "e") then
			local x = math.random(152, 154)
			local y = math.random(52, 54)
			
			player:warp(40, x, y)
			player:sendMinitext("You have arrived at East gate.")
		elseif (gate == "w" or string.sub(gate, 1, 1) == "w") then
			local x = math.random(3, 5)
			local y = math.random(52, 54)
			
			player:warp(40, x, y)
			player:sendMinitext("You have arrived at West gate.")
		elseif (gate == "s" or string.sub(gate, 1, 1) == "s") then
			local x = math.random(76, 81)
			local y = math.random(70, 71)
			
			player:warp(40, x, y)
			player:sendMinitext("You have arrived at South gate.")
		elseif (string.match(gate, "n(%w+)") ~= nil) then
			local x = math.random(77, 80)
			local y = math.random(9, 11)
			
			player:warp(40, x, y)
			player:sendMinitext("You have arrived at North gate.")
		elseif (string.match(gate, "e(%w+)") ~= nil) then
			local x = math.random(152, 154)
			local y = math.random(52, 54)
			
			player:warp(40, x, y)
			player:sendMinitext("You have arrived at East gate.")
		elseif (string.match(gate, "w(%w+)") ~= nil) then
			local x = math.random(3, 5)
			local y = math.random(52, 54)
			
			player:warp(40, x, y)
			player:sendMinitext("You have arrived at West gate.")
		elseif (string.match(gate, "s(%w+)") ~= nil) then
			local x = math.random(76, 81)
			local y = math.random(70, 71)
			
			player:warp(40, x, y)
			player:sendMinitext("You have arrived at South gate.")
		end
	end
end,

on_forget = function(player)
	player.registry["learned_gateway"] = 0
end,

requirements = function(player)
	local level = 10
	local items = {}
	local itemAmounts = {}
	local description = {"Teleports you to a gate in town"}
	return level, items, itemAmounts, description
end
}