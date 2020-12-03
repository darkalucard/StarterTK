freezing_fox_ball = {
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
			fox_ball_freezing.cast(mob, pcBlocks[i])
		end
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			fox_ball_freezing.cast(mob, mobBlocks[i])
		end
	end
	
	mob:sendAnimationXY(22, mob.x, mob.y, 0)
	mob_ai_ghost.move(mob, target)
end,

attack = function(mob, target)
	fox_ball.attack(mob, target)
end,

after_death = function(mob, target)
	if (target.blType == BL_PC) then
		if (target.state ~= 1) then
			local damage = mob.maxDam
			if (target:hasDuration("fox_ball_freezing") == false) then
				target:setDuration("fox_ball_freezing", 3000)
			end
			target:sendAnimation(22, 0)
			target:sendAnimation(24, 0)
			target.attacker = mob.ID
			target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
			target:calcStat()
		end
	end
end
}

fox_ball_freezing = {
cast = function(mob, target)

	if (target:hasDuration("fox_ball_freezing") == true) then
		return
	end
	
	if (not target.paralyzed and ((target.blType == BL_MOB and target.owner > 0 and target.owner < 1073741823) or target.blType == BL_PC)) then
		if (target.state == 1 or target:hasDuration("fox_ball_freezing") == true) then
			return
		end
		target:setDuration("fox_ball_freezing", 3000)
		target:sendAnimation(23, 0)
	
		if (target.blType == BL_MOB) then
			target.paralyzed = true
		elseif (target.blType == BL_PC) then
			target:calcStat()
		end
	end
end,

recast = function(block)
	block.paralyzed = true
end,

uncast = function(block)
	local damage = 1475

	block.attacker = block.ID
	block:removeHealthExtend(damage, 0, 1, 0, 1, 0)
	
	if (block.blType == BL_MOB) then
		block.paralyzed = false
	elseif (block.blType == BL_PC) then
		block.paralyzed = false
	end
end
}