fox_boss_summon = {
on_healed = function(mob, healer)
	fox_boss.on_healed(mob, healer)
end,

on_attacked = function(mob, attacker)
	fox_boss.on_attacked(mob, attacker)
end,

move = function(mob, target)
	if (mob.paralyzed or mob.sleep ~= 1) then
		mob.paralyzed = false
		mob.sleep = 1
	end
	
	mob_ai_basic.move(mob, target)
end,

attack = function(mob, target)
	local ballCheck = math.random(3)
	
	if (mob.paralyzed or mob.sleep ~= 1) then
		mob.paralyzed = false
		mob.sleep = 1
	end
	
	if (mob.registry["fox_ball_aether"] <= os.time()) then
		fox_boss_summon.summon_ball(mob)
		
		if (ballCheck > 1) then
			fox_boss_summon.summon_ball(mob)
		end
		
		if (ballCheck > 2) then
			fox_boss_summon.summon_ball(mob)
		end
		
		mob.registry["fox_ball_aether"] = os.time() + 4
	end

	mob_ai_basic.attack(mob, target)
end,

after_death = function(mob, attacker)
	local mobBlocks = mob:getObjectsInSameMap(BL_MOB)
	local pcBlocks = mob:getObjectsInSameMap(BL_PC)
	local bossCheck = false
	local boss = mob.mapRegistry["boss"]
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mobBlocks[i].mobID == 93) then
				bossCheck = true
				break
			end
		end
	end
	
	if (bossCheck) then
		for i = 1, #mobBlocks do
			if (mob.mobID == 94 and mobBlocks[i].mobID == 95) then
				mobBlocks[i].health = 0
				mobBlocks[i]:removeHealth(mobBlocks[i].health)
				if (Mob(boss).registry["fox_boss_phase"] == 2) then
					mob.mapRegistry["fox_alternate"] = 94
				elseif (Mob(boss).registry["fox_boss_phase"] % 2 == 0 and mob.mapRegistry["fox_alternate"] == 94) then
					Mob(boss):sendAnimation(5)
					Mob(boss).attacker = mob.ID
					Mob(boss):addHealthExtend((Mob(boss).maxHealth * 0.25), 0, 0, 0, 0, 0)
					if (Mob(boss).registry["fox_boss_phase"] >= 2) then
						Mob(boss).registry["fox_boss_phase"] = Mob(boss).registry["fox_boss_phase"] - 1
					end
				end
				break
			elseif (mob.mobID == 95 and mobBlocks[i].mobID == 94) then
				mobBlocks[i].health = 0
				mobBlocks[i]:removeHealth(mobBlocks[i].health)
				if (Mob(boss).registry["fox_boss_phase"] == 2) then
					mob.mapRegistry["fox_alternate"] = 95
				elseif (Mob(boss).registry["fox_boss_phase"] % 2 == 0 and mob.mapRegistry["fox_alternate"] == 95) then
					Mob(boss):sendAnimation(5)
					Mob(boss).attacker = mob.ID
					Mob(boss):addHealthExtend((Mob(boss).maxHealth * 0.15), 0, 0, 0, 0, 0)
					if (Mob(boss).registry["fox_boss_phase"] >= 2) then
						Mob(boss).registry["fox_boss_phase"] = Mob(boss).registry["fox_boss_phase"] - 1
					end
				end
				break
			end
		end
	else
		mob.registry["fox_ball_aether"] = 0
		
		for i = 1, #mobBlocks do
			if (mob.mobID == 94 and mobBlocks[i].mobID == 95) then
				bossCheck = true
				break
			elseif (mob.mobID == 95 and mobBlocks[i].mobID == 94) then
				bossCheck = true
				break
			end
		end
		
		if (not bossCheck) then
			mob.mapRegistry["flashing_fox_balls"] = 0
			mob.mapRegistry["lava_fox_balls"] = 0
			mob.mapRegistry["poison_fox_balls"] = 0
			mob.mapRegistry["freezing_fox_balls"] = 0
			mob.mapRegistry["bomb_fox_balls"] = 0
			mob.mapRegistry["fox_ball_refresh"] = 0
			mob.mapRegistry["fox_alternate"] = 0
			mob.mapRegistry["boss"] = 0
			if (#mobBlocks > 0) then
				for i = 1, #mobBlocks do
					if (mobBlocks[i].mobID >= 103 and mobBlocks[i].mobID <= 109) then
						mobBlocks[i]:delete()
					end
				end
			end
			--DROPS
			--[[Increase based on group
			if (#attacker.group > 0) then
				if (#pcBlocks > 0) then
					local delta = #attacker.group - #pcBlocks
					if (delta < 0) then
						delta = 0
					else
						delta = math.floor(((#attacker.group + #pcBlocks) / 2) * 0.75)
						if (delta > 1) then
							delta = 1
						end
					end
					
					if (#pcBlocks > 8) then
						local delta2 = #pcBlocks - 8
						delta2 = delta2 / 8
					end
					
					local chanceIncrease = 1 + delta + delta2
				end
			end
			]]--
			--GOLD DROPS
			mob:dropItem(0, 15000)
			if (math.random(100000) < 60000) then
				mob:dropItem(0, 12000)
			end
			if (math.random(100000) < 35000) then
				mob:dropItem(0, 3000)
			end
			if (math.random(100000) < 5000) then
				mob:dropItem(0, 5000)
			end
			--ITEM DROPS
			if (math.random(100000) < 5000) then--5% fox lure
				mob:dropItem(2500000000, 1)
			end
			if (math.random(100000) < 20000) then--20% fox fang (trade for boots)
				mob:dropItem(207, 1)
			end
			--END DROPS
		end
	end
end,

summon_ball = function(mob)
	local flashingFoxBalls = mob.mapRegistry["flashing_fox_balls"]
	local lavaFoxBalls = mob.mapRegistry["lava_fox_balls"]
	local poisonFoxBalls = mob.mapRegistry["poison_fox_balls"]
	local freezingFoxBalls = mob.mapRegistry["freezing_fox_balls"]
	local bombFoxBalls = mob.mapRegistry["bomb_fox_balls"]
	local totalFoxBalls = flashingFoxBalls + lavaFoxBalls + poisonFoxBalls + freezingFoxBalls + bombFoxBalls
	local spawnX = math.random(mob.x - 5, mob.x + 5)
	local spawnY = math.random(mob.y - 5, mob.y + 5)
	local spawnNum = math.random(3)
	
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
	
	if (totalFoxBalls >= 12) then
		return
	else
		if (mob.mobID == 94) then
			if (spawnNum == 1 and lavaFoxBalls < 6) then
				mob:spawn(104, spawnX, spawnY, 1)
			elseif (spawnNum == 2 and freezingFoxBalls < 3) then
				mob:spawn(105, spawnX, spawnY, 1)
			elseif (spawnNum == 3 and poisonFoxBalls < 6) then
				mob:spawn(108, spawnX, spawnY, 1)
			else
				return fox_boss.summon_ball(mob)
			end
		elseif (mob.mobID == 95) then
			if (spawnNum == 1 and bombFoxBalls < 6) then
				mob:spawn(103, spawnX, spawnY, 1)
			elseif (spawnNum == 2 and poisonFoxBalls < 6) then
				mob:spawn(108, spawnX, spawnY, 1)
			elseif (spawnNum == 3 and flashingFoxBalls < 2) then
				mob:spawn(109, spawnX, spawnY, 1)
			else
				return fox_boss.summon_ball(mob)
			end
		end
	end
end
}