flashing_fox_ball = {
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
			fox_ball_flash.cast(mob, pcBlocks[i])
		end
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			fox_ball_flash.cast(mob, mobBlocks[i])
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
			
			fox_ball_flash.cast(mob, target)
			--target:sendAnimation(134, 0)
			target.attacker = mob.ID
			target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
		end
	end
end
}

fox_ball_flash = {
cast = function(mob, target)
	if (not target.blind and ((target.blType == BL_MOB and target.owner > 0 and target.owner < 1073741823) or target.blType == BL_PC)) then
		target:setDuration("fox_ball_flash", 2000)
	
		if (target.blType == BL_MOB) then
			target.blind = true
		elseif (target.blType == BL_PC) then
			target:calcStat()
		end
	end
end,

recast = function(block)
	block.blind = true
end,

uncast = function(block)
	if (block.blType == BL_MOB) then
		block.blind = false
	elseif (block.blType == BL_PC) then
		block:calcStat()
	end
end
}