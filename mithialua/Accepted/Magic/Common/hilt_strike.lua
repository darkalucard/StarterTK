hilt_strike = {
cast = function(player)
	local weapon = player:getEquippedItem(EQ_WEAP)
	local mobTarget = getTargetFacing(player, BL_MOB)
	local pcTarget = getTargetFacing(player, BL_PC)
	local damage = 0
	local sound = 9
	local aether = 5000
	local magicCost = (.01 * player.maxMagic)
	local threat
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
		
	if (mobTarget == nil and pcTarget == nil) then
		player:sendMinitext("That is no longer useful.")
		return
	end
	
	if (weapon ~= nil) then
		damage = (3 * math.random(weapon.minDmg, weapon.maxDmg))
		sound = weapon.soundHit
	end
	
	player:setAether("hilt_strike", aether)
	player:playSound(sound)
	player:sendAction(1, 20)
	player:sendMinitext("You cast Hilt Strike.")
	player:playSound(107)
	
	if (mobTarget ~= nil) then
		player.magic = player.magic - magicCost
		player:sendStatus()
		mobTarget:sendAnimation(31, 0)
		mobTarget.attacker = player.ID
		mobTarget:removeHealthExtend(damage, 1, 1, 1, 1, 0)
	elseif (pcTarget ~= nil) then
		pcTarget:sendAnimation(31, 0)
		
		if (player:canPK(pcTarget)) then
			player.magic = player.magic - magicCost
			player:sendStatus()
			pcTarget.attacker = player.ID
			pcTarget:removeHealthExtend(damage, 1, 1, 1, 1, 0)
			pcTarget:sendMinitext(player.name.." strikes you with their hilt.")
		end
	end
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"A quick weapon strike dealing weak damage"}
	return level, items, itemAmounts, description
end
}