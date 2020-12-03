bandit_boss_elite = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

after_death = function(mob, block)
	mob:sendAnimationXY(103, mob.x, mob.y, 1)
	mob:playSound(7)
end,

on_spawn = function(mob)
	mob.side = 2
	mob:sendSide()
end,

on_attacked = function(mob, attacker)
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	local targets = mob:getObjectsInArea(BL_MOB)
	local rangeCheck = false
	if (mob.paralyzed == true) then
		if (math.random(10) == 3) then
			mob.paralyzed = false
			mob:sendAnimation(3)
		end
	end
	if (distance(mob, attacker) == 1 and math.random(100) <= 20) then
		entangle.cast(attacker)
	end
	if (mob.m == 93 and mob.x == 14 and mob.y == 13) then
		if (#targets > 0) then
			for i = 1, #targets do
				if (math.random(100) <= 35) then
					bandit_boss.swap(mob, targets[i])
					break
				end
			end
		end
	end
	if (attacker:hasDuration("volley") == false) then
		if (math.random(100) <= 40) then
			volley.cast(mob, attacker)
		end
	elseif (attacker:hasDuration("snipe") == false) then
		if (math.random(100) <= 30) then
			snipe.cast(mob, attacker)
		end
	end
	
	if (distance(mob, attacker) >= 2) then
		if (math.random(100) <= 10) then
			bandit_boss.swap(mob, attacker)
		elseif (math.random(100) <= 20) then
			volley.cast(mob, attacker)
		end
	end
	if (math.random(100) <= 70) then
		bandit_boss.backstep(mob, attacker)
	end
	
	if (math.random(100) <= 8) then
		if (#targets > 0) then
			for i = 1, #targets do
				if (distance(mob, targets[i]) <= 11) then
					if (math.random(100) <= 40) then
						bandit_boss.swap(mob, targets[i])
						break
					end
				end
			end
		end
	end

	mob_ai_basic.on_attacked(mob, attacker)
end,

move = function(mob, target)
	local found
	local moved = false
	local oldside = mob.side
	local checkmove = math.random(0,10)
	local targets = mob:getObjectsInSameMap(BL_PC)
	local allies = mob:getObjectsInSameMap(BL_MOB)
	local stopStrings = {"You can't stop me.", "The power of my necklace protects me from such weak magic.", "That's going to get you killed."}

	if (math.random(100) <= 45) then
		bandit_boss.unpara(mob)
	end
	local healCheck = 0
	if(math.random(100) <= 30) then
		if (#targets > 0) then
			for i = 1, #targets do
				if (targets[i].state ~= 1 and targets[i].gmLevel == 0) then
					healCheck = 1
					break
				end
			end
		end
		if (healCheck == 0) then
			bandit_boss.heal(mob)
		end
	end
	
	if (math.random(100) <= 15) then
		eye_focus.cast(mob)
	end
	
	if (mob.paralyzed == true or mob.sleep ~= 1) then
		if (#targets > 0 and mob:hasDuration("eye_focus") == true) then
			for i = 1, #targets do
				if (targets[i].baseClass == 3) then
					if (math.random(100) <= 40) then
						volley.cast(mob, targets[i])
						local randString = math.random(#stopStrings)
						local randString = math.random(#stopStrings)
						if (math.random(100) <= 10 and targets[i].state ~= 1) then
							targets[i]:msg(0, mob.name..'" '..stopStrings[randString], targets[i].ID)
						end
						if (math.random(100) <= 30 and targets[i].state ~= 1 and targets[i]:hasDuration("snipe") == false) then
							targets[i]:sendAnimation(361)
							targets[i]:setDuration("snipe", 3000, mob.ID)
						end
					end
				end
			end
		end
		return
	end
	if (#targets > 0 and mob:hasDuration("eye_focus") == false) then
		for i = 1, #targets do
			if (targets[i].baseClass == 4) then
				if (math.random(100) <= 80) then
					volley.cast(mob, targets[i])
					local randString = math.random(#stopStrings)
					local randString = math.random(#stopStrings)
					if (math.random(100) <= 10 and targets[i].state ~= 1) then
						targets[i]:msg(0, mob.name..'" '..stopStrings[randString], targets[i].ID)
					end
					if (math.random(100) <= 35 and targets[i].state ~= 1 and targets[i]:hasDuration("snipe") == false) then
						targets[i]:sendAnimation(361)
						targets[i]:setDuration("snipe", 3000, mob.ID)
					end
				end
			end
		end
	end


	
	if (mob:hasDuration("eye_focus") == true) then
		if (math.random(100) <= 10) then
			if (#allies > 0) then
				for i = 1, #allies do
					if (distance(mob, allies[i]) <= 18) then
						if (allies[i].mobID == 111 or allies[i].mobID == 112 or
						allies[i].mobID == 113 or allies[i].mobID == 114) then
							if (math.random(100) <= 15) then
								bandit_boss.swap(mob, allies[i])
								break
							end
						end
					end
				end
			end
		end
	end

	if (mob.retDist <= distanceXY(mob, mob.startX, mob.startY) and mob.retDist > 1 and mob.returning == false) then
		mob.newMove = 250
		mob.deduction = mob.deduction - 1
		mob.returning = true
	elseif (mob.returning == true and mob.retDist > distanceXY(mob, mob.startX, mob.startY) and mob.retDist > 1) then
		mob.newMove = mob.baseMove
		mob.deduction = mob.deduction + 1
		mob.returning = false
	end

	if (mob.returning == true) then
		found = toStart(mob, mob.startX, mob.startY)
	else
		if (mob.owner == 0 or mob.owner > 1073741823) then
			threat.calcHighestThreat(mob)
			local block = mob:getBlock(mob.target)
			if (block ~= nil) then
				target = block
			end
		end
	
		if (mob.state ~= MOB_HIT and target == nil and mob.owner == 0) then
			if(checkmove >= 4) then
				mob.side=math.random(0,3)
				mob:sendSide()
				if(mob.side == oldside and not mob.snare and not mob.blind) then
					moved=mob:move()
				end
			elseif (not mob.snare and not mob.blind) then
				moved=mob:move()
			end
		else
			local owner = mob:getBlock(mob.owner)
			if (target == nil and owner ~= nil) then
				target = mob:getBlock(owner.target)
				mob.target = owner.target
			end
			
			if (target ~= nil and distance(mob, target) >= 6) then
				if (not mob.snare and not mob.blind) then
					moved=FindCoords(mob,target)
				end
				
				if((moved or mob:moveIntent(target.ID) == 1) and mob.target ~= mob.owner) then
					mob.state = MOB_HIT
				end
				bandit_boss.attack(mob, target)
			else
				bandit_boss.attack(mob, target)
			end
		end
	end
	
	if (found == true) then
		mob.newMove = 0
		mob.deduction = mob.deduction + 1
		mob.returning = false
	end
end,

attack = function(mob, target)
	--mob.state = MOB_ALIVE
	--bandit_boss.move(mob, target)
	local moved
	
	if (mob.target == 0) then
		mob.state = MOB_ALIVE
		mob_ai_basic.move(mob, target)
		return
	end
	
	if (mob.paralyzed or mob.sleep ~= 1) then
		return
	end
	
	if (target) then
		threat.calcHighestThreat(mob)
		local block = mob:getBlock(mob.target)
		if (block ~= nil) then
			target = block
		end
		--moved=distance(mob, target)
		if (distance(mob, target) <= 22 and mob.target ~= mob.owner) then
			--We are right next to them, so attack!
			if (distance(mob, target) <= 12) then
				bandit_boss.zap(mob, target)
			end
			if (math.random(100) <= 25) then
				local targets = mob:getObjectsInArea(BL_PC)
				if (#targets > 0) then
					for i = 1, #targets do
						if (distance(mob, targets[i]) < 10) then
							if (math.random(100) <= 80) then
								if (targets[i].state ~= 1) then
									volley.cast(mob, targets[i])
								end
							end
						end
					end
					local ratio = 1
					local saveID = 0
					for i = 1, #targets do
						if (distance(mob, targets[i]) < 18) then
							if (math.random(100) <= 20) then
								if (targets[i].blType == BL_PC and targets[i].state ~= 1) then
									if((targets[i].health / targets[i].maxHealth) < ratio) then
										ratio = (targets[i].health / targets[i].maxHealth)
										saveID = targets[i].ID
									end
								end
							end
						end
					end
					if (saveID ~= 0) then
						snipe.cast(mob, Player(saveID))
					end
					--mob.registry["volley_aether"] = os.time() + 8
				end
			end
		else
			mob.state = MOB_ALIVE
		end
	else
		mob.state = MOB_ALIVE
	end	
	---
	--[[
	213 major grace buff
	230 pre volley
	303 volley
	398 can follow 361
	399 entangle for melee only
	339 charm
	
	]]--
	---
	--203 falling arrow
	--209 eye mark
	--213 darker eye mark
	--217 blue eye mark
	--226 arrow buff
	--227 poison tip arrow
	--230 stronger arrow  buff
	--233 corrosive tip arrow
	--234 maximum arrow buff
	--236 ice arrow tip
	--238 electric arrow
	--239 supreme arrow buff
	--263 great ice arrow
	--264 flame arrow
	--265 arrow of roses
	--299 single target volley
	--303 3 arrow volley
	--339 love arrow
	--361-363 target
	--367-369 target
	--398 big arrow drop
	--399 entangle
	mob.state = MOB_ALIVE
end,

damage = function(mob, target)
	cLevel = 28
	tLevel = target.level
	multiplier = tLevel/cLevel * 1.1
	return multiplier
end,

zap = function(mob, target)
	local mobBlocks = mob:getObjectsInArea(BL_PC)
	local targets = mob:getObjectsInSameMap(BL_PC)
	--local targetPlayers = mob:getObjectsInSameMap(BL_PC)
	local rangeCheck = false
	local damage = target.maxHealth * 0.02
	local aethers = math.random(7,9)
	local range = 12
	local arrow = 6
	local tempDamage = 0
	local critChance = 8
	local minDam = bandit_boss.damage(mob, target) * mob.minDam
	local maxDam = bandit_boss.damage(mob, target) * mob.maxDam
	damage = damage + math.random(minDam, maxDam)

	if (target.m ~= mob.m) then
		target = targets[math.random(#targets)]
	end
	
	if (distance(mob, target) == 0) then
		mob:move()
		if (distance(mob, target) == 0) then
			bandit_boss.swap(mob, mobBlocks[math.random(1, #mobBlocks)])
		end
	end
	local targetPlayers = table.getn(targets)
		
	if (#targets > 0) then
		for i = 1, #targets do
			if (targets[i].gmLevel == 0 and targets[i].state ~= 1 and targets[i]:hasDuration("snipe") == false) then
				--targetPlayers = targetPlayers + 1
			else
				targetPlayers = targetPlayers - 1
			end
		end
	end
	if (targetPlayers <= 2) then
		damage = damage * math.random(1, 3)
	elseif (targetPlayers <= 3) then
		damage = damage * math.random(1, 2)
	elseif (targetPlayers <= 4) then
		damage = damage * 1.4
	elseif (targetPlayers <= 5) then
		damage = damage * 1.3
	elseif (targetPlayers <= 6) then
		damage = damage * 1.2
	elseif (targetPlayers <= 7) then
		damage = damage * 1.1
	end

	
	if (distance(mob, target) <= range and distance(mob, target) > 0) then
		if (target.x == mob.x and target.y < mob.y) then--0
			mob.side = 0
			mob:sendSide()
		elseif (target.x == mob.x and target.y > mob.y) then--2
			mob.side = 2
			mob:sendSide()
		elseif (target.y == mob.y and target.x > mob.x) then--1
			mob.side = 1
			mob:sendSide()
		elseif (target.y == mob.y and target.x < mob.x) then--3
			mob.side = 3
			mob:sendSide()
		elseif (target.x > mob.x) then--1
			mob.side = 1
			mob:sendSide()
		elseif (target.x < mob.x) then--3
			mob.side = 3
			mob:sendSide()
		end
		if (mob:hasDuration("eye_focus") == true) then
			critChance = critChance * math.random(2, 3)
		else
			damage = damage * 2
		end

		
		if (math.random(100) <= critChance) then--crit
			damage = damage * math.random(1.5, 5.5)
			target:sendAnimation(345)
		end
		
		mob:sendAction(1, 40)
		mob:throw(target.x, target.y, arrow + mob.side, 0, 1)
		target.attacker = mob.ID
		if (target:hasDuration("deflect") == true and math.random(100) <= 75  and target:hasDuration("snipe") == false) then
			target:sendAnimation(361)
			target:setDuration("snipe", 3000, mob.ID)
			return
		end
		if (mob:hasDuration("eye_focus") == true and target:hasDuration("turtle_shell") == true) then
			tempDamage = target:removeHealthExtend((target.maxHealth * 0.05), 0, 0, 0, 0, 2)
			damage = tempDamage + target:removeHealthExtend(damage, 0, 1, 0, 0, 2)
			target:removeHealthExtend(damage, 1, 0, 1, 1, 0)
		else
			target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
		end
	end
end,

heal = function(mob)
	local heal = math.random(mob.minDam, mob.maxDam) * math.random(10, 30)
	
	if (mob:hasDuration("eye_focus") == true) then
		heal = heal * math.random(2, 3)
	end
	if ((mob.health + heal) < mob.maxHealth) then
		mob.attacker = mob.ID
		mob:sendAction(6, 40)
		mob:sendAnimation(66)
		mob:addHealthExtend(heal, 0, 0, 0, 0, 0)
	end
end,

backstep = function(mob, target)
	local dist = distance(mob, target)
	if (dist == 1) then
		if (canPush(target, mob, 1) == true) then
			if (target.y < mob.y) then--0
				mob.side = 0
			elseif (target.x > mob.x) then--1
				mob.side = 1
			elseif (target.y > mob.y) then--2
				mob.side = 2
			elseif (target.x < mob.x) then--3
				mob.side = 3
			end
			mob:sendSide()
			canPush(target, mob)
			mob:sendAnimation(16)
			--return
		else
			bandit_boss.swap(mob, target)--swap
			return
		end
		if (math.random(100) <= 10) then
			bandit_boss.swap(mob, target)
		end
	end
end,

swap = function(mob, target)
	if (target.blType == BL_PC) then
		local rand = math.random(10)
		if (rand > 5) then
			target = Player(mob.target)
		end
	end
	local mobX = mob.x
	local mobY = mob.y
	mob:warp(target.m, target.x, target.y)
	target:warp(mob.m, mobX, mobY)
	mob:playSound(82)
	mob:sendAnimation(177, 0)
	target:sendAnimation(177, 0)
	if (target.blType == BL_PC) then
		target:sendMinitext(mob.name.." makes you swap.")
	end
end,

unpara = function(mob)
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	local target = mob
	local rand = math.random(100)
	
	if (#mobBlocks > 0 and rand <= 35) then
		for i = 1, #mobBlocks do
			if (distance(mob, mobBlocks[i]) <= 10 and mobBlocks[i].paralyzed == true) then
				target = mobBlocks[i]
				mob:sendAnimation(3)
				target:sendAnimation(16)
				target.paralyzed = false
				return
			end
		end
	end
end
}
eye_focus = {
cast = function(block)
	if (block:hasDuration("eye_focus") == true) then
		return
	end
	block:sendAnimation(213)
	block:setDuration("eye_focus", 20000)
	block:calcStat()
	block.newMove = 250
end,
recast = function(block)
	block.grace = block.grace + 50
	block.newMove = 250
end,
uncast = function(block)
	block:calcStat()
	--block:talk(0,""..block.grace)
end
}
volley = {
cast = function(block, target)
	--if (block.registry["volley_aether"] <= os.time()) then
	if (target.blType == BL_PC) then
		if (target:hasDuration("volley") == false and target.state ~= 1) then
			target:setDuration("volley", 2000, block.ID)
			block:sendAnimation(230)
		end
	end
	--end
end,

uncast = function(block, caster)
	local minDam = bandit_boss.damage(caster, block) * caster.minDam
	local maxDam = bandit_boss.damage(caster, block) * caster.maxDam
	local multiplier = 3
	local targets = caster:getObjectsInSameMap(BL_PC)
	local damage = (math.random(minDam, maxDam))
	if (caster:hasDuration("eye_focus") == true) then
		multiplier = math.random(multiplier, multiplier + 2)
	end
	if (multiplier < 1) then
		multiplier = 1
	end
	local targetPlayers = #targets
	if (#targets > 0) then
		for i = 1, #targets do
			if (targets[i].gmLevel == 0 and targets[i].state ~= 1 and targets[i]:hasDuration("volley") == false) then
				--player available for increased damage
			else
				targetPlayers = targetPlayers - 1
			end
		end
	end
	if (targetPlayers <= 2) then
		damage = damage * 1.5
	elseif (targetPlayers <= 3) then
		damage = damage * 1.4
	elseif (targetPlayers <= 4) then
		damage = damage * 1.3
	elseif (targetPlayers <= 5) then
		damage = damage * 1.2
	elseif (targetPlayers <= 6) then
		damage = damage * 1.1
	elseif (targetPlayers <= 7) then
		damage = damage * 1
	end
	if (math.random(100) <= 8) then--crit
		multiplier = multiplier * 1.6
	end
	damage = damage * multiplier
	damage = damage + (block.maxHealth * 0.08)
	block:sendAnimation(303)
	block.attacker = caster.ID
	block:removeHealthExtend(damage, 1, 1, 1, 1, 0)
end
}
snipe = {
cast = function(block, target)
	if (target:hasDuration("snipe") == false and mob.registry["snipe_aether"] <= os.time()
	and target.blType == BL_PC and target.state ~= 1) then
		target:sendAnimation(361)
		target:setDuration("snipe", 3000, block.ID)
		mob.registry["snipe_aether"] = os.time() + 6
	end
end,
uncast = function(block, caster)
	local minDam = bandit_boss.damage(caster, block) * caster.minDam
	local maxDam = bandit_boss.damage(caster, block) * caster.maxDam
	local damage = (math.random(minDam, maxDam))
	damage = damage + (block.baseHealth * 0.35) + (block.health * 0.1)
	block:sendAnimation(398)
	block.attacker = caster.ID
	block:removeHealthExtend(damage, 1, 0, 0, 1, 0)
end
}
entangle = {
cast = function(block)
	if (block:hasDuration("entangle") == false) then
		block:sendAnimation(399, 0)
		block:setDuration("entangle", 3000, block.ID)
		block:calcStat()
	end
end,
while_cast = function(block)
	block:sendAnimation(399, 0)	
end,
recast = function(block)
	block.snare = true
end,
uncast = function(block, caster)
	block:calcStat()
end
}