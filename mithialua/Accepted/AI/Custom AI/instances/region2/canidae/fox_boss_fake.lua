fox_boss_fake = {
on_healed = function(mob, healer)
	local mobHealth = (mob.health / mob.maxHealth)
	if (mobHealth <= .25) then
		return
	else
		mob_ai_basic.on_healed(mob, healer)
	end
end,

on_attacked = function(mob, attacker)
	local maxDamage = (mob.maxHealth * 0.005)
	local mobHealth = (mob.health / mob.maxHealth)
	
	if (attacker.damage > maxDamage) then
		attacker.damage = maxDamage
	end
	
	if (mobHealth <= .25) then
		if (mob.registry["panda_god"] == 0) then
			mob.registry["panda_god"] = 1
			fox_boss_fake.summon_panda(mob)
		end
	else
		mob_ai_basic.on_attacked(mob, attacker)
	end
end,

summon_panda = function(mob)
	if (mob.registry["panda_god"] == 1) then
		local x = 2
		local y = 2
		mob:spawn(220000000, x, y, 1)
		local mobs = mob:getObjectsInCell(mob.m, x, y, BL_MOB)
		if (#mobs > 0) then
			for i = 1, #mobs do
				if (mobs[i].yname == "generic_mob") then
					mobs[i].side = 2
					mobs[i]:sendSide()
					mob.registry["panda_id"] = mobs[i].ID
					break
				end
			end
		end
		mob.registry["panda_god"] = 2
	else
		mob.registry["panda_god"] = mob.registry["panda_god"] + 1
	end
	
	local panda_god = mob.registry["panda_god"]
	local panda_id = mob.registry["panda_id"]
	local panda = Mob(panda_id)
	if (panda ~= nil) then
		if (panda_god == 5) then
			panda:sendAction(21, 40)
		elseif (panda_god == 7) then
			panda:sendAction(18, 40)
		elseif (panda_god == 8) then
			panda:sendAction(18, 40)
		elseif (panda_god == 13) then
			panda:sendAction(6, 180)
		elseif (panda_god == 15) then
			panda:sendAction(20, 150)
		elseif (panda_god == 17) then
			panda:sendAction(6, 180)
		elseif (panda_god == 18) then
			mob:sendAnimation(393)
		elseif (panda_god == 19) then
			mob:warp(mob.m, 19, 0)
			local pcBlocks = mob:getObjectsInSameMap(BL_PC)
			if (#pcBlocks > 0) then
				for i = 1, #pcBlocks do
					pcBlocks[i]:giveXP(550000)
				end
			end
			panda:delete()
			mob:delete()
		elseif (panda_god > 19) then
		end
	end
end,

on_spawn = function(mob)
	mob:warp(mob.m, 9, 9)
end,

move = function(mob, target)
	local flashingFoxBalls = mob.mapRegistry["flashing_fox_balls"]
	local lavaFoxBalls = mob.mapRegistry["lava_fox_balls"]
	local poisonFoxBalls = mob.mapRegistry["poison_fox_balls"]
	local freezingFoxBalls = mob.mapRegistry["freezing_fox_balls"]
	local bombFoxBalls = mob.mapRegistry["bomb_fox_balls"]
	local totalFoxBalls = flashingFoxBalls + lavaFoxBalls + poisonFoxBalls + freezingFoxBalls + bombFoxBalls
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	local bossCheck = false
	local moved = false
	
	if (mob.registry["panda_god"] >= 2) then
		fox_boss_fake.summon_panda(mob)
	end
	
	if (mob.registry["superbomb_fox_ball_exhaust"] > os.time()) then
		mob.registry["superbomb_fox_ball_swing"] = 1
		return
	end
	
	if (mob.registry["superbomb_fox_ball_swing"] == 1) then
		mob:sendAction(1, 80)
		mob.registry["superbomb_fox_ball_swing"] = 0
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mobBlocks[i].mobID == 94 or mobBlocks[i].mobID == 95) then
				bossCheck = true
				break
			end
		end
	end
	
	if (bossCheck) then
		mob.paralyzed = true
		mob.deduction = -1
	else
		mob.paralyzed = false
		mob.deduction = 1
	end
	
	if (mob.paralyzed or mob.sleep ~= 1 or mob.blind or mob.snare) then
		return
	end
	
	if (target ~= nil and distance(mob, target) <= 4 and target.m == mob.m) then
		canAmbush(mob, target)
		
		if (bombFoxBalls > 3 and bombFoxBalls < 6) then
			if (totalFoxBalls <= 10) then
				mob:spawn(103, mob.x, mob.y, (6 - bombFoxBalls))
			else
				mob:spawn(103, mob.x, mob.y, 1)
			end
		elseif (bombFoxBalls <= 3) then
			if (totalFoxBalls <= 9) then
				mob:spawn(103, mob.x, mob.y, 3)
			else
				mob:spawn(103, mob.x, mob.y, (12 - totalFoxBalls))
			end
		end
	end
	
	mob_ai_basic.move(mob, target)
end,

attack = function(mob, target)
	local flashingFoxBalls = mob.mapRegistry["flashing_fox_balls"]
	local lavaFoxBalls = mob.mapRegistry["lava_fox_balls"]
	local poisonFoxBalls = mob.mapRegistry["poison_fox_balls"]
	local freezingFoxBalls = mob.mapRegistry["freezing_fox_balls"]
	local bombFoxBalls = mob.mapRegistry["bomb_fox_balls"]
	local totalFoxBalls = flashingFoxBalls + lavaFoxBalls + poisonFoxBalls + freezingFoxBalls + bombFoxBalls
	local targetsAround = getTargetsAround(mob, BL_PC)
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	local bossCheck = false
	local ambushCheck = false
	local ballCheck = math.random(3)
	local superbombChance = math.random(100000)
	local bombX = math.random(mob.x - 5, mob.x + 5)
	local bombY = math.random(mob.y - 5, mob.y + 5)

	if (mob.registry["panda_god"] >= 2) then
		fox_boss_fake.summon_panda(mob)
	end
	
	if (bombX < 0) then
		bombX = 0
	elseif (bombX > mob.xmax) then
		bombX = mob.xmax
	end
	
	if (bombY < 0) then
		bombY = 0
	elseif (bombY > mob.ymax) then
		bombY = mob.ymax
	end
	
	if (mob.registry["superbomb_fox_ball_exhaust"] > os.time()) then
		mob.registry["superbomb_fox_ball_swing"] = 1
		return
	end
	
	if (mob.registry["superbomb_fox_ball_swing"] == 1) then
		mob:sendAction(1, 80)
		mob.registry["superbomb_fox_ball_swing"] = 0
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mobBlocks[i].mobID == 94 or mobBlocks[i].mobID == 95) then
				bossCheck = true
				break
			end
		end
	end
	
	if (bossCheck) then
		mob.paralyzed = true
		mob.deduction = -1
	else
		mob.paralyzed = false
		mob.deduction = 1
	end
	
	if (mob.paralyzed or mob.sleep ~= 1 or mob.blind or mob.snare) then
		return
	end
	
	if (not canMove(mob) and #targetsAround > 0) then
		for i = 1, #targetsAround do
			if (canAmbush(mob, targetsAround[i], 1)) then
				ambushCheck = true
				break
			end
		end
		
		if (ambushCheck) then
			canAmbush(mob, targetsAround[math.random(#targetsAround)])
		elseif (bombFoxBalls < 6 and totalFoxBalls < 12) then
			mob:spawn(103, bombX, bombY, 1)
		end
	end
	
	if (mob.registry["fox_ball_aether"] <= os.time()) then
		fox_boss_fake.summon_ball(mob)
		
		if (ballCheck > 1) then
			fox_boss_fake.summon_ball(mob)
		end
		
		if (ballCheck > 2) then
			fox_boss_fake.summon_ball(mob)
		end
		
		mob.registry["fox_ball_aether"] = os.time() + 5
	end
	
	if (mob.registry["superbomb_fox_ball_aether"] <= os.time() and superbombChance <= 50000 and not bossCheck) then
		if (mob.side == 0) then
			mob:spawn(106, mob.x, mob.y - 1, 1)
		elseif (mob.side == 1) then
			mob:spawn(106, mob.x + 1, mob.y, 1)
		elseif (mob.side == 2) then
			mob:spawn(106, mob.x, mob.y + 1, 1)
		elseif (mob.side == 3) then
			mob:spawn(106, mob.x - 1, mob.y, 1)
		else
			mob:talk("I'm stuck in a bad way, please call the GM.")
		end
		
		mob.registry["superbomb_fox_ball_aether"] = os.time() + 24
		mob.registry["superbomb_fox_ball_exhaust"] = os.time() + 2
		return
	end
	
	mob_ai_basic.attack(mob, target)
end,

after_death = function(mob, target)
	mob.registry["fox_boss_fake_phase"] = 0
	mob.registry["fox_ball_aether"] = 0
	mob.registry["superbomb_fox_ball_aether"] = 0
	mob.registry["superbomb_fox_ball_exhaust"] = 0
	mob.registry["superbomb_fox_ball_swing"] = 0
end,

summon_ball = function(mob)

	if (mob.registry["summon_ball_aether"] > os.time()) then
		return
	end

	local flashingFoxBalls = mob.mapRegistry["flashing_fox_balls"]
	local lavaFoxBalls = mob.mapRegistry["lava_fox_balls"]
	local poisonFoxBalls = mob.mapRegistry["poison_fox_balls"]
	local freezingFoxBalls = mob.mapRegistry["freezing_fox_balls"]
	local bombFoxBalls = mob.mapRegistry["bomb_fox_balls"]
	local totalFoxBalls = flashingFoxBalls + lavaFoxBalls + poisonFoxBalls + freezingFoxBalls + bombFoxBalls
	local spawnX = math.random(mob.x - 5, mob.x + 5)
	local spawnY = math.random(mob.y - 5, mob.y + 5)
	local spawnNum = math.random(5)
	
	if (spawnX < 0) then
		spawnX = 0
	elseif (spawnX > mob.xmax) then
		spawnX = mob.xmax
	end
	
	if (spawnY < 0) then
		spawnY = 0
	elseif (spawnY > mob.ymax) then
		spawnY = mob.ymax
	end
	
	if (totalFoxBalls >= 8) then
		return
	else
		if (spawnNum == 1 and bombFoxBalls < 6) then
			mob:spawn(103, spawnX, spawnY, 1)
		elseif (spawnNum == 2 and lavaFoxBalls < 6) then
			mob:spawn(104, spawnX, spawnY, 1)
		elseif (spawnNum == 3 and freezingFoxBalls < 3) then
			mob:spawn(105, spawnX, spawnY, 1)
		elseif (spawnNum == 4 and poisonFoxBalls < 6) then
			mob:spawn(108, spawnX, spawnY, 1)
		elseif (spawnNum == 5 and flashingFoxBalls < 2) then
			mob:spawn(109, spawnX, spawnY, 1)
		else
			return fox_boss_fake.summon_ball(mob)
		end
	end
	mob.registry["summon_ball_aether"] = os.time() + 3
end
}