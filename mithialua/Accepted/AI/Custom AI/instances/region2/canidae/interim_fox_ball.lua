interim_fox_ball = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

on_attacked = function(mob, attacker)
	mob_ai_basic.on_attacked(mob, attacker)
end,

on_spawn = function(mob)
	mob.registry["interim_fox_ball_exhaust"] = os.time() + 2
end,

move = function(mob, target)
	if (mob.registry["interim_fox_ball_exhaust"] > os.time()) then
		return
	end
	
	mob.registry["interim_fox_ball_exhaust"] = 0
	mob.attacker = mob.ID
	mob.health = 0
	mob:removeHealth(mob.health)
	mob:spawn(107, mob.x, mob.y, 1)
end
}