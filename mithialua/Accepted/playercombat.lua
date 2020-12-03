player_combat = {
on_healed = function(player, healer)
	player.attacker = healer.ID
	player:sendHealth(healer.damage, healer.critChance)
end,

on_attacked = function(player, attacker)
	if (player:hasDuration("evade") and attacker.critChance == 0) then
		if (math.random(100000) <= 90000) then
			player:setDuration("evade", 0)
			return
		end
	end
	
	if (player:hasDuration("mark_of_death")) then
		attacker.damage = attacker.damage * 2
	end
	
	if (attacker.blType == BL_PC) then
		if (attacker.registry["dps_potion"] > 0) then
			attacker.dmgDealt = attacker.dmgDealt + attacker.damage
		end
	end
	
	if (player:hasDuration("step_in")) then
		local casterIDs = player:getCasterID("step_in")
		local interveneBlock = player:getBlock(casterIDs[1])
		
		if (interveneBlock ~= nil) then
			attacker.damage = attacker.damage / 2
			interveneBlock.attacker = attacker.ID
			interveneBlock:sendHealth(attacker.damage, attacker.critChance)
			interveneBlock:sendStatus()
		end
	end
	
	if (player:hasDuration("deflect")) then
		local damage = attacker.damage * .75
		
		attacker.damage = attacker.damage / 4
		attacker.attacker = player.ID
		attacker:sendHealth(damage, attacker.critChance)
		attacker:sendStatus()
	end
	
	player.attacker = attacker.ID
	player:sendHealth(attacker.damage, attacker.critChance)
end
}