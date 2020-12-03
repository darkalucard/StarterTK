teamB = {
on_healed = function(mob, healer)
	mob_ai_basic.on_healed(mob, healer)
end,

on_attacked = function(mob, attacker)
	mob_ai_basic.on_attacked(mob, attacker)
end,

move = function(mob,target)
	mob.side = 3
	local facing = getTargetFacing(mob, BL_MOB, 0)
	if (facing == nil or facing.mobID == mob.mobID) then
		facing = getTargetFacing(mob, BL_PC, 0)
	end
	
	mob.side = 0
	local up = getTargetFacing(mob, BL_MOB, 0)
	if (up == nil or up.mobID == mob.mobID) then
		up = getTargetFacing(mob, BL_PC, 0)
	end
	
	mob.side = 2
	local down = getTargetFacing(mob, BL_MOB, 0)
	if (down == nil or down.mobID == mob.mobID) then
		down = getTargetFacing(mob, BL_PC, 0)
	end
	
	if (facing == nil and up == nil and down == nil) then
		mob.side = 3
		mob:sendSide()
		mob:move()
	elseif (up ~= nil or down ~= nil) then
		local rand = 0
		
		if (up ~= nil) then
			rand = rand + 1
		end
		
		if (down ~= nil) then
			rand = rand + 2
		end
		
		if (rand == 3) then
			rand = math.random(2)
		end
		
		if (rand == 1) then
			mob.side = 0
			mob:sendSide()
			mob.target = up.ID
			up.attacker = mob.ID
			mob:attack(up.ID)
		else
			mob.side = 2
			mob:sendSide()
			mob.target = down.ID
			down.attacker = mob.ID
			mob:attack(down.ID)
		end
	else
		mob.side = 3
		mob:sendSide()
		mob.target = facing.ID
		facing.attacker = mob.ID
		mob:attack(facing.ID)
	end
end,

attack = function(mob, target)
	mob.state = MOB_ALIVE
	--[[if (target) then
		local moved = FindCoords(mob, target)
		if (moved) then
			mob:attack(target.ID)
		else
			mob.state = MOB_ALIVE
		end
	else
		mob.state = MOB_ALIVE
	end]]--
end
}