singe = {
cast = function(player, target)
	local aether = 1000
	local damage = (player.level * 5 + (((.01 * player.magic) + (.01 * player.health)) / 2.25))
	local threat
	local healthCost = (.01 * player.maxHealth)
	local magicCost = (.01 * player.maxMagic)
	local minHealth = 20
	
	if (not player:canCast(1, 1, 0)) then
		return
	end
	
	if (player.health < minHealth or player.health - healthCost <= 0) then
		player:sendMinitext("Not enough vita.")
		return
	end
	
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end
	
	if (target.state == 1) then
		player:sendMinitext("That is no longer useful.")
		return
	end
	
	if (target.blType == BL_MOB) then
		player:sendAction(6, 20)
		--player:setAether("singe", aether)
		player.health = player.health - healthCost
		player.magic = player.magic - magicCost
		player:sendStatus()
		player:playSound(58)
	
		--if (checkProtection(player, target, 1)) then
		--	player:sendMinitext("Singe has been deflected.")
		--else
			target:sendAnimation(28, 1)
			player:sendMinitext("You cast Singe.")
			target.attacker = player.ID
			threat = target:removeHealthExtend(damage, 1, 1, 1, 1, 2)
			player:addThreat(target.ID, threat)
			
			if (#player.group > 1) then
				target:setGrpDmg(player.ID, threat)
			else
				target:setIndDmg(player.ID, threat)
			end
			target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
		--end
		
	elseif (target.blType == BL_PC and player:canPK(target)) then
		player:sendAction(6, 20)
		player:setAether("singe", aether)
		player.health = player.health - healthCost
		player.magic = player.magic - magicCost
		player:sendStatus()
		player:playSound(58)
	
		--if (checkProtection(player, target, 1)) then
		--	player:sendMinitext("Singe has been deflected.")
		--else
			target:sendAnimation(28, 1)
			player:sendMinitext("You cast Singe.")
			target.attacker = player.ID
			target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
			target:sendMinitext(player.name.." singes you.")
		--end
	end
end,

requirements = function(player)
	local level = 1
	local items = {}
	local itemAmounts = {}
	local description = {"Weak elemental attack on a target"}
	return level, items, itemAmounts, description
end
}