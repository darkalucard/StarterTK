bandit_mage_elite = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

after_death = function(mob, block)
	mob:sendAnimationXY(103,mob.x,mob.y,1)
	mob:playSound(7)
end,

on_attacked = function(mob, attacker)
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	local rangeCheck = false
	
	if (not mob.paralyzed and mob.sleep == 1) then
		if (#mobBlocks > 0) then
			for i = 1, #mobBlocks do
				if (mobBlocks[i].ID ~= mob.ID and distance(mob, mobBlocks[i]) <= 10 and (mobBlocks[i].owner == 0 or mobBlocks[i].owner >= 1073741823)) then
					rangeCheck = true
					break
				end
			end
		end
		
		if (attacker.critChance ~= 0 and ((attacker.x == mob.x and (attacker.y == mob.y + 1 or attacker.y == mob.y - 1))
		or (attacker.y == mob.y and (attacker.x == mob.x + 1 or attacker.x == mob.x - 1))) and not rangeCheck) then
			RunAway(mob, attacker)
		end
	end
	
	mob_ai_basic.on_attacked(mob, attacker)
end,

move = function(mob, target)
	local found
	local moved=true
	local oldside = mob.side
	local checkmove = math.random(0,10)

	if (mob.paralyzed or mob.sleep ~= 1) then
		return
	end
	
	if (target and target.m == mob.m) then
		bandit_mage.zap(mob, target)
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
	
		if (mob.state ~= MOB_HIT and distance(mob, target) <= 12 and mob.owner == 0) then
			if(checkmove >= 4) then
				mob.side=math.random(0,3)
				mob:sendSide()
				if(mob.side == oldside) then
					moved=mob:move()
				end
			else
				moved=mob:move()
			end
		elseif (target ~= nil) then
			moved=FindCoords(mob,target)
		end
	end
	
	if (found == true) then
		mob.newMove = 0
		mob.deduction = mob.deduction + 1
		mob.returning = false
	end
end,

attack = function(mob, target)
	mob.state = MOB_ALIVE
	bandit_mage.move(mob, target)
end,

zap = function(mob, target)
	local damage = 350
	
	if (distance(mob, target) <= 12) then
		mob:sendAction(6, 40)
		mob:playSound(55)
		target:sendAnimation(4, 0)
		target.attacker = mob.ID
		target:removeHealthExtend(damage, 1, 1, 1, 1, 0)
	end
end
}