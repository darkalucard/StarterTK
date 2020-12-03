lava_fox_ball = {
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
			lava_fox_ball.lava(mob, pcBlocks[i])
		end
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mobBlocks[i].owner > 0 and mobBlocks[i].owner < 1073741823) then
				lava_fox_ball.lava(mob, mobBlocks[i])
			end
		end
	end
	
	mob:sendAnimationXY(45, mob.x, mob.y, 0)
	mob_ai_ghost.move(mob, target)
end,

attack = function(mob, target)
	fox_ball.attack(mob, target)
end,

after_death = function(mob, target)
	if (target.blType == BL_PC) then
		if (target.state ~= 1) then
			local damage = mob.maxDam

			target:sendAnimation(47, 0)
			target.attacker = mob.ID
			target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
		end
	end
end,

lava = function(mob, target)
	if (target.blType == BL_PC and target.state == 1) then
		return
	end
	local damage = math.random(mob.minDam, mob.maxDam * 1.4)

	target:sendAnimation(46, 0)
	target.attacker = mob.ID
	if (target:hasDuration("turtle_shell") == true) then
		damage = target:removeHealthExtend(damage, 0, 0, 1, 0, 2) / 5
		damage = damage + (target.maxHealth * 0.023) + (target.health * 0.11)
	else
		damage = damage + (target.maxHealth * 0.023) + (target.health * 0.11)
	end
	target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
end
}