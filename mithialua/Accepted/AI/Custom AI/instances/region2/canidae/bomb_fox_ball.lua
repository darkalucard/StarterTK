bomb_fox_ball = {
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
--[[	local pcs = mob:getObjectsInSameArea(BL_PC)

	if (#pcs > 0) then
		if (mob.target == 0 or mob.target >= 1073741823) then
			mob.target = pcBlocks[math.random(#pcBlocks)].ID
			threat.calcHighestThreat(mob)
		end			
	end]]--
	fourPush(mob, BL_PC)
	mob:sendAnimationXY(10, mob.x, mob.y, 0)
	mob_ai_ghost.move(mob, target)
end,

attack = function(mob, target)
	fox_ball.attack(mob, target)
end,

after_death = function(mob, target)
	if (target.blType == BL_PC) then
		if (target.state ~= 1) then
			local damage = math.random(mob.maxDam, mob.maxDam * 2.5)
			
			fourPush(target, BL_PC)
			mob:sendAnimationXY(13, mob.x, mob.y, 0)
			target:sendAnimation(3, 0)
			target.attacker = mob.ID
			target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
			target:calcStat()
			target:playSound(29)
		end
	end
end
}