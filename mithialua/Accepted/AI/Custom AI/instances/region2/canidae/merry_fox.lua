merry_fox = {
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
	
	if (math.random(100) <= 15) then
		merry_fox.unpara(mob)
	end
	if (mob.paralyzed or mob.sleep ~= 1) then
		if (math.random(100) <= 35) then
			merry_fox.unpara(mob)
		end
		return
	end
	
	if (chance <= 5000 and target == nil) then
		mob:sendAction(2, 80)
	else
		mob_ai_basic.move(mob, target)
	end
end,

attack = function(mob, target)
	--merry_fox.unpara(mob)
	mob_ai_basic.attack(mob, target)
end,

unpara = function(mob)
	targets = mob:getObjectsInArea(BL_MOB)
	if (#targets > 0) then
		for i = 1, #targets do
			if (targets[i].paralyzed == true and math.random(100000) <= 1500) then
				targets[i]:sendAnimation(10)
				targets[i].paralyzed = false
				mob:sendAction(2, 80)
				break
			end
		end
	end
end
}