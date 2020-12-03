bandit_warrior_elite = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

after_death = function(mob, block)
	mob:sendAnimationXY(103,mob.x,mob.y,1)
	mob:playSound(7)
end,

on_attacked = function(mob, attacker)
	mob_ai_basic.on_attacked(mob, attacker)
end,

move = function(mob, target)
	mob_ai_basic.move(mob, target)
end,

attack = function(mob, target)
	if (mob.paralyzed or mob.sleep ~= 1) then
		return
	end
	
	mob_ai_basic.attack(mob, target)
	mob:flank()
end
}