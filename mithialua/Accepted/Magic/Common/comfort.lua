comfort = {
cast = function(player, target)
	local heal = 150
	local magicCost = 50
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (target.state == 1) then
		player:sendMinitext("That cannot save them now.")
		return
	end
	
	player:sendAction(6, 20)
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:sendMinitext("You cast Comfort.")
	player:playSound(98)
	target:sendAnimation(66, 0)
	target.attacker = player.ID
	target:addHealthExtend(heal, 0, 0, 0, 0, 0)
	target:sendMinitext(player.name.." comforts you.")
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Weak healing spell that allows you to heal others"}
	return level, items, itemAmounts, description
end
}