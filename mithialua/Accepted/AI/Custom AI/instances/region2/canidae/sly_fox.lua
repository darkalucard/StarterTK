sly_fox = {
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
	if (math.random(100) <= 21) then
		sly_fox.warp(mob, attacker)
	end
	mob_ai_basic.on_attacked(mob, attacker)
end,

move = function(mob, target)
	local pcBlocks = mob:getObjectsInArea(BL_PC)
	local chance = math.random(100000)
	local found
	local moved=true
	local oldside = mob.side
	local checkmove = math.random(0,10)

	if (mob.paralyzed or mob.sleep ~= 1) then
		if (math.random(100) <= 35) then
			sly_fox.warp(mob, target)
		end	
		return
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
		if (mob.target > 0) and (mob.owner == 0 or mob.owner > 1073741823) then
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
				if(mob.side == oldside) then
					if (chance <= 20000 and #pcBlocks > 0) then
						for i = 1, #pcBlocks do
							if (distance(mob, pcBlocks[i]) <= 12) then
								mob.registry["layTrap_aether"] = os.time() + 3
								sly_fox.layTrap(mob)
								break
							end
						end
					end
					
					moved=mob:move()
				end
			else
				if (chance <= 20000 and #pcBlocks > 0) then
					for i = 1, #pcBlocks do
						if (distance(mob, pcBlocks[i]) <= 12) then
							mob.registry["layTrap_aether"] = os.time() + 3
							sly_fox.layTrap(mob)
							break
						end
					end
				end
				
				moved=mob:move()
			end
		elseif (target ~= nil) then
			moved=FindCoords(mob,target)
			if(moved and mob.target ~= mob.owner) then
				mob.state = MOB_HIT
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
	local npcBlocks
	local traps = {}
	local randomTrap
	
	if (math.random(100000) <= 5000) then
		sly_fox.layTrap(mob)
	end
	if (mob.paralyzed or mob.sleep ~= 1) then
		if (math.random(100) <= 35) then
			sly_fox.warp(mob, target)
		end
		return
	end
	
	if (target ~= nil and math.random(100) <= 8) then
		sly_fox.warp(mob, target)
	end
	
	if (not canMove(mob)) then
		npcBlocks = mob:getObjectsInArea(BL_NPC)
		
		if (#npcBlocks > 0) then
			for i = 1, #npcBlocks do
				if (npcBlocks[i].subType == 2 and distance(mob, npcBlocks[i]) <= 12 and distance(mob, npcBlocks[i]) > 0) then
					table.insert(traps, npcBlocks[i])
				end
			end
		end
		
		if (#traps > 0) then
			randomTrap = traps[math.random(#traps)]
			mob:warp(randomTrap.m, randomTrap.x, randomTrap.y)
		else
			sly_fox.layTrap(mob, target.m, target.x, target.y)
		end
	else
		mob_ai_basic.attack(mob, target)
	end
end,

warp = function(mob, target)
	local npcBlocks = target:getObjectsInArea(BL_NPC)
	local pcBlocks = target:getObjectsInArea(BL_PC)
	local randomTrap
	local traps = {}
	local night
	
	if (curTime() > 18 or curTime() < 6) then
		night = true
	else
		night = false
	end
	
	if (#npcBlocks > 0) then
		for i = 1, #npcBlocks do
			if (npcBlocks[i].subType == 2 and distance(mob, npcBlocks[i]) <= 3 and distance(mob, npcBlocks[i]) > 0) then
				table.insert(traps, npcBlocks[i])
			end
		end
	end
	
	if (mob.registry["warp_aether"] > os.time()) then
		--aethers!
		return
	end
	
	if (night == true) then
		if (#traps > 0) then
			randTrap = math.random(#traps)
			randomTrap = traps[randTrap]
			if (target == nil) then
				for i = 1, #pcBlocks do
					if ((randomTrap.x ~= pcBlocks[i].x and randomTrap.y ~= pcBlocks[i].y) and distanceXY(pcBlocks[i], randomTrap.x, randomTrap.y) <= 5) then
						target = pcBlocks[i]
					end
				end
			end
			if (target == nil or target == 0) then
				sly_fox.layTrap(mob)
				return
			end
			target:warp(randomTrap.m, randomTrap.x, randomTrap.y)
			target:sendAnimation(190)
			mob.registry["warp_aether"] = os.time() + 2
		else
			sly_fox.layTrap(mob)
		end	
	else--DAY TIME
		if (#traps > 0) then
			randTrap = math.random(#traps)
			randomTrap = traps[randTrap]
			if (target == nil) then
				for i = 1, #pcBlocks do
					if ((randomTrap.x ~= pcBlocks[i].x and randomTrap.y ~= pcBlocks[i].y) and distanceXY(pcBlocks[i], randomTrap.x, randomTrap.y) <= 5) then
						target = pcBlocks[i]
						
					end
				end
			end
			if (target == nil or target == 0) then
				sly_fox.layTrap(mob)
				return
			end
			
			if (target.blType == BL_PC) then
				if (target:hasSpell("swap") == true) then
					if (target:hasAether("swap") == false) then
						target:setAether("swap", 30000)
						target:sendMinitext("The daylight allows you to spot the trap and you swap away from it.")
						mob.registry["warp_aether"] = os.time() + 2
						target:sendAnimation(177)
						target:sendAnimationXY(177, randomTrap.x, randomTrap.y)
						return
					end
				elseif (target:hasSpell("decoy") == true) then
					if (target:hasAether("decoy") == false) then
						target:setAether("decoy", 30000)
						target:sendMinitext("A gleam allows you to spot the trap and you decoy away from it.")
						mob.registry["warp_aether"] = os.time() + 2
						target:spawn(101, randomTrap.x, randomTrap.y, 1)
						return
					end
				elseif (target:hasSpell("counter_attack") == true) then
					if (target:hasAether("counter_attack") == false) then
						target:setAether("counter_attack", 9000)
						target:sendMinitext("The daylight allows you to spot the trap and you counter attack it.")
						mob.registry["warp_aether"] = os.time() + 2
						target:sendAnimation(89)
						target:sendAnimationXY(89, randomTrap.x, randomTrap.y)
						return
					end
				elseif (target:hasSpell("evade") == true) then
					if (target:hasAether("evade") == false) then
						target:setAether("evade", 10000)
						target:sendMinitext("A gleam allows you to spot the trap and you quickly use rabbit's luck.")
						mob.registry["warp_aether"] = os.time() + 2
						target:sendAnimation(72)
						return
					end
				elseif (target:hasSpell("shove") == true) then
					if (target:hasAether("shove") == false and math.random(100) < 30) then
						target:setAether("shove", 3000)
						target:sendMinitext("As you see the fox through a gleam trying to push you, you shove against it.")
						mob.registry["warp_aether"] = os.time() + 2
						target:sendAnimation(190)
						return
					end
				elseif (target:hasSpell("displace") == true) then
					if (target:hasAether("displace") == false and math.random(100) < 15) then
						target:setAether("displace", 10000)
						target:sendMinitext("You feel a gleam through time that displaces you away from a trap.")
						mob.registry["warp_aether"] = os.time() + 2
						target:sendAnimation(13)
						return
					end
				--Spell durations
				elseif (target:hasDuration("ancient_legacy") == true) then
					if (target:getDuration("ancient_legacy") > 30000 and math.random(100) < 30) then
						target:sendMinitext("A gleam reflects on your ancient protection.")
						target:setDuration("ancient_legacy", (target:getDuration("ancient_legacy") - 30000))
						mob.registry["warp_aether"] = os.time() + 2
						return
					end
				end
			end
			target:warp(randomTrap.m, randomTrap.x, randomTrap.y)
			target:sendAnimation(190)
			mob.registry["warp_aether"] = os.time() + 2
		else
			sly_fox.layTrap(mob)
		end	
	end
end,

layTrap = function(mob, map, x, y)
	local chance = math.random(100000)
	if (mob.registry["layTrap_aether"] > os.time()) then
		return
	end
		
	if (map ~= nil and x ~= nil and y ~= nil) then
		if (chance <= 15000) then
			mob:addNPC("sly_fox_dart", map, x, y, 0, 180000, mob.ID)
		elseif (chance > 15000 and chance <= 50000) then
			mob:addNPC("sly_fox_spear", map, x, y, 0, 180000, mob.ID)
		elseif (chance > 50000 and chance <= 70000) then
			mob:addNPC("sly_fox_venom", map, x, y, 0, 180000, mob.ID)
		else
			mob:addNPC("sly_fox_debuff", map, x, y, 0, 180000, mob.ID)
		end
	
		addSpotTrap(mob, map, x, y)
	else
		if (chance <= 15000) then
			mob:addNPC("sly_fox_dart", mob.m, mob.x, mob.y, 0, 180000, mob.ID)
		elseif (chance > 15000 and chance <= 50000) then
			mob:addNPC("sly_fox_spear", mob.m, mob.x, mob.y, 0, 180000, mob.ID)
		elseif (chance > 50000 and chance <= 70000) then
			mob:addNPC("sly_fox_venom", mob.m, mob.x, mob.y, 0, 180000, mob.ID)
		else
			mob:addNPC("sly_fox_debuff", mob.m, mob.x, mob.y, 0, 180000, mob.ID)
		end
	
		addSpotTrap(mob)
	end
	mob.registry["layTrap_aether"] = os.time() + 3
end
}

sly_fox_dart = {
click = function(player, npc)
	local damage = 1750
	
	if (player.blType ~= BL_PC or (player.blType == BL_PC and player.state == 1)) then
		return
	else
		if (player:hasDuration("turtle_shell") == true) then
			damage = damage / 1.5
		end
		player:sendAnimation(33, 0)
		player.attacker = 1073743399
		player:removeHealthExtend(damage, 0, 0, 0, 0, 0)
		npc.registry["triggers"] = npc.registry["triggers"] + 1
	end
	
	if (npc.registry["triggers"] == 3) then
		removeTrap(npc)
	end
end,

endAction = function(npc, block)
	removeTrap(npc)
end
}

sly_fox_spear = {
click = function(player, npc)
	local damage = 4250
	if (player.blType ~= BL_PC or (player.blType == BL_PC and player.state == 1)) then
		return
	else
		if (player:hasDuration("turtle_shell") == true) then
			damage = damage / 1.5
		end	
		player:sendAnimation(6, 0)
		player.attacker = 1073743399
		player:removeHealthExtend(damage, 0, 0, 0, 0, 0)
		removeTrap(npc)
	end
end,

endAction = function(npc, block)
	removeTrap(npc)
end
}

sly_fox_venom = {
click = function(player, npc)
	if (player.blType ~= BL_PC or (player.blType == BL_PC and player.state == 1) or player:hasDuration("sly_fox_venom") == true) then
		return
	else
		player:setDuration("sly_fox_venom", 12000, 1073743399)
		removeTrap(npc)
	end
end,

endAction = function(npc, block)
	removeTrap(npc)
end,

while_cast = function(player, caster)
	local damage = 2250
	local rand = math.random(1000, 2000)
	rand = rand / 1000
	damage = damage * rand
	if (player:hasDuration("turtle_shell") == true) then
		damage = damage / 1.5
	end
	player:sendAnimation(1, 0)
	player.attacker = 1073743399
	player:removeHealthExtend(damage, 0, 0, 0, 0, 0)
end
}

sly_fox_debuff = {
click = function(player, npc)
	if (player.blType ~= BL_PC or (player.blType == BL_PC and player.state == 1) or player:hasDuration("sly_fox_debuff") == true) then
		return
	else
		player:setDuration("sly_fox_debuff", 30000)
		player:sendAnimation(10, 0)
		player:calcStat()
		removeTrap(npc)
	end
end,

endAction = function(npc, block)
	removeTrap(npc)
end,

recast = function(player)
	player.armor = player.armor * 0.8
end,

uncast = function(player)
	player:calcStat()
end
}