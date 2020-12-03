poison_fox_ball = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

on_attacked = function(mob, attacker)
	mob_ai_basic.on_attacked(mob, attacker)
end,

on_spawn = function(mob)
	fox_ball.on_spawn(mob)
end,

move = function(mob, target)
	local pcBlocks = getTargetsAround(mob, BL_PC)
	local mobBlocks = getTargetsAround(mob, BL_MOB)
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			fox_ball_poison.cast(mob, pcBlocks[i])
		end
		if (mob.target == 0 or mob.target >= 1073741823) then
			mob.target = pcBlocks[math.random(#pcBlocks)].ID
			threat.calcHighestThreat(mob)
		end			
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			fox_ball_poison.cast(mob, mobBlocks[i])
		end
	end
	
	mob_ai_ghost.move(mob, target)
end,

attack = function(mob, target)
	fox_ball.attack(mob, target)
end,

after_death = function(mob, target)
	if (target.blType == BL_PC) then
		if (target.state ~= 1) then
			local damage = mob.maxDam

			fox_ball_poison.cast(mob, target)
			target:sendAnimation(415, 0)
			target.attacker = mob.ID
			target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
		end
	end
end
}

fox_ball_poison = {
cast = function(mob, target)
	if ((target.blType == BL_MOB and mob.owner > 0 and mob.owner < 1073741823) or target.blType == BL_PC) then
		if (target.state ~= 1 and target:hasDuration("fox_ball_poison") == false) then
			target:setDuration("fox_ball_poison", 10000)
		end
	end
end,

while_cast = function(block)
	local pcBlocks = getTargetsAround(block, BL_PC)
	local damage = 975
	
	if (#pcBlocks > 0 and block.blType == BL_PC) then
		for i = 1, #pcBlocks do
			if (not pcBlocks[i]:hasDuration("fox_ball_poison") and pcBlocks[i].state ~= 1) then
				pcBlocks[i]:setDuration("fox_ball_poison", 12000)
			end
		end
	end
	
	block:sendAnimation(139, 0)
	block.attacker = 1073743399
	if (block:hasDuration("turtle_shell") == true) then
		damage = block:removeHealthExtend(damage, 0, 0, 1, 0, 2) / 5
		damage = damage + (block.maxHealth * 0.04) + (block.health * 0.09)
	else
		damage = damage + (block.maxHealth * 0.04) + (block.health * 0.09)
	end
	block:removeHealthExtend(damage, 1, 0, 1, 1, 0)
end
}