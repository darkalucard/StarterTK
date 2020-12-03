bandit_rogue_elite = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

after_death = function(mob, block)
	mob:sendAnimationXY(103,mob.x,mob.y,1)
	mob:playSound(7)
end,

on_attacked = function(mob, attacker)
	local pcTargetsAround = getTargetsAround(mob, BL_PC)
	local mobTargetsAround = getTargetsAround(mob, BL_MOB)
	local target = mob:getBlock(mob.target)
	local chance = math.random(100000)
	local targetFound = false
	
	if (not mob.paralyzed and mob.sleep == 1) then
		if (target ~= nil and #pcTargetsAround > 0) then
			for i = 1, #pcTargetsAround do
				if (pcTargetsAround[i].ID == target.ID and chance <= 30000) then
					targetFound = true
					canAmbush(mob, target)
					break
				end
			end
		end
		
		if (target ~= nil and not targetFound and #mobTargetsAround > 0) then
			for i = 1, #mobTargetsAround do
				if (mobTargetsAround[i].ID == target.ID and chance <= 30000) then
					targetFound = true
					canAmbush(mob, target)
					break
				end
			end
		end
		
		if (not targetFound and #pcTargetsAround > 0) then
			for i = 1, #pcTargetsAround do
				if (pcTargetsAround[i].ID == attacker.ID and chance <= 30000) then
					targetFound = true
					canAmbush(mob, attacker)
					break
				end
			end
		end
		
		if (not targetFound and #mobTargetsAround > 0) then
			for i = 1, #mobTargetsAround do
				if (mobTargetsAround[i].ID == attacker.ID and chance <= 30000) then
					canAmbush(mob, attacker)
					break
				end
			end
		end
	end
	
	mob_ai_basic.on_attacked(mob, attacker)
end,

move = function(mob, target)
	local chance = math.random(100000)
	
	if (mob.paralyzed or mob.sleep ~= 1) then
		return
	end
	
	if (chance <= 25000 and target ~= nil) then
		bandit_rogue_invis.cast(mob)
	end
	
	mob_ai_basic.move(mob, target)
end,

attack = function(mob, target)
	local pcTargetsAround = getTargetsAround(mob, BL_PC)
	local mobTargetsAround = getTargetsAround(mob, BL_MOB)
	local invisChance = math.random(100000)
	local ambushChance = math.random(100000)
	local targetFound = false
	
	if (mob.paralyzed or mob.sleep ~= 1) then
		return
	end
	
	if (invisChance <= 15000 and target ~= nil) then
		bandit_rogue_invis.cast(mob)
	end
	
	if (#pcTargetsAround > 0) then
		for i = 1, #pcTargetsAround do
			if (pcTargetsAround[i].ID == target.ID and ambushChance <= 15000) then
				targetFound = true
				canAmbush(mob, target)
				break
			end
		end
	end
	
	if (not targetFound and #mobTargetsAround > 0) then
		for i = 1, #mobTargetsAround do
			if (mobTargetsAround[i].ID == target.ID and ambushChance <= 15000) then
				canAmbush(mob, target)
				break
			end
		end
	end
	
	mob_ai_basic.attack(mob, target)
end
}

bandit_rogue_invis = {
cast = function(mob)
	if (not mob:hasDuration("bandit_rogue_invis")) then
		mob:setDuration("bandit_rogue_invis", 18000)
		mob.charState = 2
		mob:updateState()
		mob.invis = mob.invis + 4
	end
end,

on_hit_while_cast = function(mob)
	mob:flushDuration(7026)
end,

uncast = function(mob)
	mob.charState = 0
	mob:updateState()
	mob.invis = mob.invis - 4
end
}