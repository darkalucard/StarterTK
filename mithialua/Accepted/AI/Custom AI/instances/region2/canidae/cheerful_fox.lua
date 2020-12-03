cheerful_fox = {
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
	local chance = math.random(100000)
	local castChance = math.random(100000)
	
	if (mob.paralyzed or mob.sleep ~= 1) then
		if (math.random(100) <= 75) then
			cheerful_fox.heal(mob)
		end
		return
	end
	
	if (chance <= 50000 and target == nil) then
		mob:sendAction(2, 80)
		cheerful_fox.heal(mob)
	elseif (castChance <= 17000 and target ~= nil) then
		mob:sendAction(2, 80)
		cheerful_fox_sleep.cast(mob)
	else
		mob_ai_basic.move(mob, target)
	end
end,

attack = function(mob, target)
	local castChance = math.random(100000)
	
	if (mob.paralyzed or mob.sleep ~= 1) then
		return
	end
	
	if (castChance <= 6000) then
		mob:sendAction(2, 80)
		cheerful_fox_sleep.cast(mob)
	else
		mob_ai_basic.attack(mob, target)
	end
end,

heal = function(mob)
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	local injuredFoxes = {}
	local heal = math.random(mob.minDam, mob.maxDam)
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if ((mobBlocks[i].mobID == 87 or mobBlocks[i].mobID == 88 or mobBlocks[i].mobID == 89 or mobBlocks[i].mobID == 90
			or mobBlocks[i].mobID == 91 or mobBlocks[i].mobID == 92 or mobBlocks[i].mobID == 93) and mobBlocks[i].health ~= mobBlocks[i].maxHealth
			and distance(mob, mobBlocks[i]) <= 6) then
				table.insert(injuredFoxes, mobBlocks[i])
			end
		end
	end
	
	if (#injuredFoxes > 0) then
		local fox = injuredFoxes[math.random(#injuredFoxes)]
		fox:sendAnimation(3)
		fox.attacker = mob.ID
		fox:addHealthExtend(heal, 0, 0, 0, 0, 0)
	end
end
}

cheerful_fox_sleep = {
cast = function(mob)
	local pcBlocks = mob:getObjectsInArea(BL_PC)
	local sleepChance
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if (not pcBlocks[i]:hasDuration("cheerful_fox_sleep") and distance(mob, pcBlocks[i]) <= 4) then
				if (pcBlocks[i].protection >= 5) then
					sleepChance = 1
				else
					sleepChance = (((5 - pcBlocks[i].protection) / 20) * 100)
				end
				
				if (sleepChance > math.random(100) and pcBlocks[i].gmLevel == 0) then
					pcBlocks[i]:setDuration("cheerful_fox_sleep", 7000)
					pcBlocks[i]:sendAnimation(324, 1)
					pcBlocks[i]:calcStat()
				end
			end
		end
	end
end,

while_cast = function(block)
	block:sendAnimation(324, 1)
	if (block.blType == BL_PC) then
		if (math.random(100) <= (block.will / 10)) then
			block:sendMinitext("Your strong will wakes you out of the lullaby.")
			block:flushDuration(0, 7015)
		end
	end
end,

recast = function(block)
	block.sleep = 1.1
end,

uncast = function(block)
	block.sleep = 1
	block:sendAnimation(324, -1)
end,

on_takedamage_while_cast = function(block, attacker)
	--block:flushDuration(7015)
end
}