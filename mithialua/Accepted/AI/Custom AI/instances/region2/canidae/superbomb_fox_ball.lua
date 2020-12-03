superbomb_fox_ball = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

on_attacked = function(mob, attacker)
	mob_ai_basic.on_attacked(mob, attacker)
end,

on_spawn = function(mob)
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mobBlocks[i].mobID == 93) then
				mob.target = mobBlocks[i].target
				break
			end
		end
	end
end,

move = function(mob, target)
	local targetsAround = getTargetsAround(mob, BL_PC)
	local pcOnMob = mob:getObjectsInCell(mob.m, mob.x, mob.y, BL_PC)
	local pcBlocks = mob:getObjectsInArea(BL_PC)

	if (#targetsAround > 0 or #pcOnMob > 0) then
		if (#targetsAround > 0) then
			superbomb_fox_ball.suicide(mob, targetsAround[math.random(#targetsAround)])
		else
			superbomb_fox_ball.suicide(mob, pcOnMob[math.random(#pcOnMob)])
		end
	end
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if (distance(mob, pcBlocks[i]) <= 4) then
				local checkNPCNorth = pcBlocks[i]:getObjectsInCell(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y - 1, BL_NPC)
				local checkNPCEast = pcBlocks[i]:getObjectsInCell(pcBlocks[i].m, pcBlocks[i].x + 1, pcBlocks[i].y, BL_NPC)
				local checkNPCWest = pcBlocks[i]:getObjectsInCell(pcBlocks[i].m, pcBlocks[i].x - 1, pcBlocks[i].y, BL_NPC)
				local checkNPCSouth = pcBlocks[i]:getObjectsInCell(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y + 1, BL_NPC)
				local side = getSide(mob, pcBlocks[i])
				
				if (#checkNPCNorth > 0) then
					for i = 1, #checkNPCNorth do
						if (checkNPCNorth[i] ~= nil and (checkNPCNorth[i].subType == 2 or checkNPCNorth[i].subType == 3)) then
							table.remove(checkNPCNorth, i)
							i = i - 1
						end
					end
				end
				
				if (#checkNPCEast > 0) then
					for i = 1, #checkNPCEast do
						if (checkNPCEast[i] ~= nil and (checkNPCEast[i].subType == 2 or checkNPCEast[i].subType == 3)) then
							table.remove(checkNPCEast, i)
							i = i - 1
						end
					end
				end
				
				if (#checkNPCWest > 0) then
					for i = 1, #checkNPCWest do
						if (checkNPCWest[i] ~= nil and (checkNPCWest[i].subType == 2 or checkNPCWest[i].subType == 3)) then
							table.remove(checkNPCWest, i)
							i = i - 1
						end
					end
				end
				
				if (#checkNPCSouth > 0) then
					for i = 1, #checkNPCSouth do
						if (checkNPCSouth[i] ~= nil and (checkNPCSouth[i].subType == 2 or checkNPCSouth[i].subType == 3)) then
							table.remove(checkNPCSouth, i)
							i = i - 1
						end
					end
				end
				
				if (side == 0) then
					if (#checkNPCSouth == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x, pcBlocks[i].y + 1, 2) == true and getPass(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y + 1) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y + 1)
					elseif (#checkNPCWest == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x - 1, pcBlocks[i].y, 3) == true and getPass(pcBlocks[i].m, pcBlocks[i].x - 1, pcBlocks[i].y) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x - 1, pcBlocks[i].y)
					elseif (#checkNPCEast == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x + 1, pcBlocks[i].y, 1) == true and getPass(pcBlocks[i].m, pcBlocks[i].x + 1, pcBlocks[i].y) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x + 1, pcBlocks[i].y)
					end
				elseif (side == 1) then
					if (#checkNPCWest == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x - 1, pcBlocks[i].y, 3) == true and getPass(pcBlocks[i].m, pcBlocks[i].x - 1, pcBlocks[i].y) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x - 1, pcBlocks[i].y)
					elseif (#checkNPCSouth == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x, pcBlocks[i].y + 1, 2) == true and getPass(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y + 1) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y + 1)
					elseif (#checkNPCNorth == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x, pcBlocks[i].y - 1, 0) == true and getPass(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y - 1) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y - 1)
					end
				elseif (side == 2) then
					if (#checkNPCNorth == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x, pcBlocks[i].y - 1, 0) == true and getPass(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y - 1) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y - 1)
					elseif (#checkNPCEast == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x + 1, pcBlocks[i].y, 1) == true and getPass(pcBlocks[i].m, pcBlocks[i].x + 1, pcBlocks[i].y) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x + 1, pcBlocks[i].y)
					elseif (#checkNPCWest == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x - 1, pcBlocks[i].y, 3) == true and getPass(pcBlocks[i].m, pcBlocks[i].x - 1, pcBlocks[i].y) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x - 1, pcBlocks[i].y)
					end
				elseif (side == 3) then
					if (#checkNPCEast == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x + 1, pcBlocks[i].y, 1) == true and getPass(pcBlocks[i].m, pcBlocks[i].x + 1, pcBlocks[i].y) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x + 1, pcBlocks[i].y)
					elseif (#checkNPCNorth == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x, pcBlocks[i].y - 1, 0) == true and getPass(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y - 1) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y - 1)
					elseif (#checkNPCSouth == 0 and pcBlocks[i]:objectCanMove(pcBlocks[i].x, pcBlocks[i].y + 1, 2) == true and getPass(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y + 1) == 0) then
						pcBlocks[i]:warp(pcBlocks[i].m, pcBlocks[i].x, pcBlocks[i].y + 1)
					end
				end
			end
		end
	end
	
	mob_ai_ghost.move(mob, target)
end,

attack = function(mob, target)
	superbomb_fox_ball.move(mob, target)
end,

suicide = function(mob, target)
	local pcBlocks = mob:getObjectsInArea(BL_PC)
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	local damage = mob.maxDam
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if (distance(mob, pcBlocks[i]) <= 4) then
				pcBlocks[i].attacker = mob.ID
				pcBlocks[i]:removeHealthExtend(damage, 1, 1, 1, 1, 0)
			end
		end
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mobBlocks[i].owner > 0 and mobBlocks[i].owner < 1073741823 and distance(mob, mobBlocks[i]) <= 4) then
				mobBlocks[i].attacker = mob.ID
				mobBlocks[i]:removeHealthExtend(damage, 1, 1, 1, 1, 0)
			end
		end
	end
	
	mob.attacker = target.ID
	mob.health = 0
	mob:removeHealth(mob.health)
end,

after_death = function(mob, target)
	mob:sendAnimationXY(393, mob.x, mob.y, 0)
	mob:playSound(4)
	mob:playSound(59)
end
}