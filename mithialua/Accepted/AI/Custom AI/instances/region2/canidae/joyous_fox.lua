joyous_fox = {
on_spawn = function(mob)
	fox_instance.on_spawn(mob)
end,

after_death = function(mob, block)
	fox_instance.after_death(mob, block)
	mob.registry["fox_whistle_aether"] = 0	
end,

on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

on_attacked = function(mob, attacker)
	mob_ai_basic.on_attacked(mob, attacker)
end,

move = function(mob, target)
	local chance = math.random(100000)
	
	if (mob.paralyzed or mob.sleep ~= 1) then
		if (math.random(100) <= 25) then
			merry_fox.unpara(mob)
		end
		return
	end
	
	if (target == nil and chance <= 30000 and mob.registry["fox_whistle_aether"] < os.time()) then
		mob:sendAction(2, 80)
		mob.registry["fox_whistle_aether"] = os.time() + 8
	end
	
	mob_ai_basic.move(mob, target)
end,

attack = function(mob, target)
	mob_ai_basic.attack(mob, target)
end
}