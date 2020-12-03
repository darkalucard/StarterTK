summon_pig = {
click = async(function(player, mob)
	player:msg(0, 'Bacon(Pet)" Oink!', player.id)
	if (player.id == mob.owner) then
		local mobG = {graphic = convertGraphic(mob.look, "monster"), color = mob.lookColor}
		player.npcGraphic = mobG.graphic
		player.npcColor = mobG.color	
		local options = {"attack", "retreat"}
		local blockPC = mob:getObjectsInArea(BL_PC)
		
		local menuOpt = player:menu("Awaiting orders!", options)
		if (menuOpt == 1) then
			mob.target = player.attacker
			if (mob.look ~= 19) then
				mob.look = 19
				mob.lookColor = 4
				if (#blockPC > 0) then
					for i = 1, #blockPC do
						blockPC[i]:refresh()
					end
				end
			end
		elseif (menuOpt == 2) then
			mob.target = mob.owner
			if (mob.look ~= 20) then
				mob.look = 20
				mob.lookColor = 4
				if (#blockPC > 0) then
					for i = 1, #blockPC do
						blockPC[i]:refresh()
					end
				end
			end
		end
	end
end),

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
	local magic = mob.magic
	local mMagic = mob.maxMagic
	
	if ((magic + 1) < mMagic) then--regen
		mob.magic = magic + 1
	end	

	if(mob.retDist <= distance(mob, owner) and mob.retDist > 1 and mob.returning == false) then
		mob.state = MOB_ALIVE
	elseif (target ~= nil) then
		--if (target.ID == mob.owner and distance(mob, target) > 2)) then
			--moved = FindCoords(mob, target)
		if (target.ID ~= mob. ID and distance(mob, target) > 1) then
			moved = FindCoords(mob, target)
		end
		if(moved and mob.target ~= mob.owner) then
			--We are right next to them, so attack!
			if (mob.look ~= 153) then
				mob.look = 153
				mob.lookColor = 4
				target:refresh()
			end
			mob:attack(target.ID)
		end
		summon_pig.heal(mob)
		--summon_heal.cast(mob, target)
	end
end,

heal = function(mob)
	local health = mob.health
	local mHealth = mob.maxHealth
	local magic = mob.magic

	if (magic < 5) then
		return
	end
	
	if ((health / mHealth * 100) <= 90) then
		mob.magic = mob.magic - 5
		mob:addHealth(7500)
		mob:sendAnimation(5)
	end
end
}