summon_healer = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

on_attacked = function(mob, attacker)
	mob_ai_basic.on_attacked(mob, attacker)
end,
	
move = function(mob, target)
	local found
	local moved=true
	local oldside = mob.side
	local checkmove = math.random(0, 10)
	local owner = mob:getBlock(mob.owner)

	if (mob.paralyzed) then
		return
	end
	
	if (mob.sleep ~= 1) then
		return
	end
	
	if (mob.retDist <= distance(mob, owner) and mob.retDist > 1 and mob.returning == false) then
		mob.newMove = 250
		mob.deduction = mob.deduction - 1
		mob.returning = true
	elseif (mob.returning == true and mob.retDist > distance(mob, owner) and mob.retDist > 1) then
		mob.newMove = 0
		mob.deduction = mob.deduction + 1
		mob.returning = false
	end

	if (mob.returning == true) then
		local randDist = math.random(mob.retDist)
		local randPlusMinus = math.random(4)
		
		if (randPlusMinus == 1) then
			found = toXY(mob, owner.x + randDist, owner.y + randDist)
		elseif (randPlusMinus == 2) then
			found = toXY(mob, owner.x - randDist, owner.y - randDist)
		elseif (randPlusMinus == 3) then
			found = toXY(mob, owner.x + randDist, owner.y - randDist)
		else
			found = toXY(mob, owner.x - randDist, owner.y + randDist)
		end
	else
		if (mob.owner == 0 or mob.owner > 1073741823) then
			threat.calcHighestThreat(mob)
			local block = mob:getBlock(mob.target)
			if (block ~= nil) then
				target = block
			end
		end
	
		if (mob.state ~= MOB_HIT and target == nil and mob.owner == 0) then
			if (checkmove >= 4) then
				mob.side = math.random(0, 3)
				mob:sendSide()
				if (mob.side == oldside) then
					moved = mob:move()
				end
			else
				moved = mob:move()
			end
		else
			if (target ~= nil and target.ID == mob. ID) then
				moved = FindCoords(mob, owner)
			elseif (target ~= nil) then
				moved = FindCoords(mob, target)
			end
			
			if(mob.retDist > distance(mob, owner) and mob.retDist > 1 and mob.returning == false and target.ID ~= mob.ID) then
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
	if(mob.retDist <= distance(mob, owner) and mob.retDist > 1 and mob.returning == false) then
		mob.state = MOB_ALIVE
	elseif (target ~= nil) then
		if (target.ID ~= mob. ID and distance(mob, target) > 6) then
			moved = FindCoords(mob, target)
		end
		summon_heal.cast(mob, target)
	end
end
}