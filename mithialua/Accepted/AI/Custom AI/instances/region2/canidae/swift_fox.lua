swift_fox = {
on_spawn = function(mob)
	fox_instance.on_spawn(mob)
end,

after_death = function(mob, block)
	fox_instance.after_death(mob, block)
end,

on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

on_attacked = function(mob, attacker)
	mob_ai_basic.on_attacked(mob, attacker)
end,

move = function(mob, target)
	mob_ai_basic.move(mob, target)
end,

attack = function(mob, target)
	local chance = math.random(100000)
	
	if (mob.paralyzed or mob.sleep ~= 1) then
		return
	end
	
	if (chance <= 35000) then
		swift_fox.strike(mob)
	end
	
	mob_ai_basic.attack(mob, target)
end,

strike = function(mob)
	local pcBlocks = mob:getObjectsInArea(BL_PC)
	local damage = 500
	damage = damage + math.random(mob.minDam, mob.maxDam)	
	local player
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if (player == nil and distance(mob, pcBlocks[i]) <= 8) then
				if (math.random(100) <= 25) then
					player = pcBlocks[i]
					break
				end
			elseif (player ~= nil and pcBlocks[i].health < player.health and distance(mob, pcBlocks[i]) <= 8) then
				if (math.random(100) <= 25) then
					player = pcBlocks[i]
					break
				end
			end
		end
	end
	
	if (player ~= nil and player.state ~= 1) then
		player:sendAnimation(33, 0)
		player.attacker = mob.ID
		if (player:hasDuration("turtle_shell") == true) then
			local tempDamage = player:removeHealthExtend((player.maxHealth * 0.05), 0, 0, 0, 0, 2)
			damage = tempDamage + player:removeHealthExtend(damage, 0, 1, 0, 0, 2)
			player:removeHealthExtend(damage, 1, 0, 1, 1, 0)
		else
			damage = damage + (player.maxHealth * 0.08)
			player:removeHealthExtend(damage, 1, 1, 1, 1, 0)
		end
	end
end
}