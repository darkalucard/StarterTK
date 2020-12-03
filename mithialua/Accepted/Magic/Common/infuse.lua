infuse = {
cast = function(player, target)
	local aether = 4000
	local magicGiven = player.magic
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic == 0) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (target.state == 1 or target.blType == BL_MOB) then
		player:sendMinitext("That is no longer useful.")
		return
	end
	
	player:sendAction(6, 20)
	player:setAether("infuse", aether)
	player:playSound(84)
	target:sendAnimation(74, 0)
	
	if (target.magic + magicGiven > target.maxMagic) then
		magicGiven = (target.maxMagic - target.magic)
		target.magic = target.maxMagic
	else
		target.magic = target.magic + magicGiven
	end
	
	if (player.magic - magicGiven < 0) then
		player.magic = 0
	else
		player.magic = player.magic - magicGiven
	end
	
	player:sendStatus()
	player:sendMinitext("You cast Infuse.")
	target:sendStatus()
	target:sendMinitext(player.name.." infuses your mind.")
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Focuses your magic into another target"}
	return level, items, itemAmounts, description
end
}