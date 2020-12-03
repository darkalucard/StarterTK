--Mob damage formulas
function Mob.addHealthExtend(mob, amount, sleep, deduction, ac, ds, print)
	local healer
	local ded = 0
	
	if (mob.state == 1) then
		return
	end
	
	if (mob.attacker >= 1073741823) then
		healer = Mob(mob.attacker)
	elseif (mob.attacker > 0) then
		healer = Player(mob.attacker)
	end
	
	ded = mob.armor / (mob.armor + 400 + 95 * (healer.level + healer.tier^2 + healer.mark^3))
	
	if (healer:hasDuration("blossom")) then
		amount = amount * 2
	end
	
	if (sleep > 0 and print == 2) then
		amount = amount * mob.sleep
	elseif (sleep == 1) then
		amount = amount * mob.sleep
		mob.sleep = 1
	elseif (sleep == 2) then
		amount = amount * mob.sleep
	end
	
	if (deduction == 1) then
		if (mob.deduction < 0) then
			amount = 0
		elseif (mob.deduction > 0) then
			amount = amount * mob.deduction
		end
	end
	
	if (ac == 1) then
		if (ded < .85) then
			amount = amount * (1 - ded)
		else
			amount = amount * .15
		end
	end
	
	if (ds > 0 and print == 2) then
		amount = amount - mob.dmgShield
	elseif (ds == 1) then
		if (mob.dmgShield > 0) then
			if (mob.dmgShield > amount) then
				mob.dmgShield = mob.dmgShield - amount
				amount = 0
			else
				amount = amount - mob.dmgShield
				mob.dmgShield = 0
			end
		else
			amount = amount - mob.dmgShield
			mob.dmgShield = 0
		end
	elseif (ds == 2) then
		mob.dmgShield = mob.dmgShield - amount
	end
	
	amount = -amount
	
	if (healer ~= nil) then
		healer.damage = amount
		healer.critChance = 0
	else
		mob.damage = amount
		mob.critChance = 0
	end
	
	if (print == 1) then
		if (mob.health - amount > mob.maxHealth) then
			mob.health = mob.maxHealth
		else
			mob.health = mob.health - amount
		end
	elseif (print == 2) then
		return amount
	else
		if (mob.aiType == 0) then
			mob_ai_basic.on_healed(mob, healer)
		elseif (mob.aiType == 1) then
			mob_ai_normal.on_healed(mob, healer)
		elseif (mob.aiType == 2) then
			mob_ai_hard.on_healed(mob, healer)
		elseif (mob.aiType == 3) then
			mob_ai_boss.on_healed(mob, healer)
		elseif (mob.aiType == 4) then
			mob:callBase("on_healed")
		elseif (mob.aiType == 5) then
			mob_ai_ghost.on_healed(mob, healer)
		end
	end
end

function Mob.removeHealthWithoutDamageNumbers(mob, amount, type)
	local temp_health=0
	
	temp_health=mob.health-amount
	
	if temp_health<=0 then
		temp_health = 0
	end
	
	mob.health=temp_health
	
	if (mob.attacker >= 1073741823) then
		Mob(mob.attacker).damage = amount
		Mob(mob.attacker).critChance = type
	elseif (mob.attacker > 0) then
		Player(mob.attacker).damage = amount
		Player(mob.attacker).critChance = type
	else
		mob.damage = amount
		mob.critChance = type
	end
	
	if (mob.health == 0) then
		mob:removeHealth(0)
		--mob.state = 1
		--mob:sendAction(5, 0)
		--onDeathMob(mob)
	end
end

function Mob.removeHealthExtend(mob, amount, sleep, deduction, ac, ds, print)
	local attacker
	local ded = 0
	
	if (mob.attacker >= 1073741823) then
		attacker = Mob(mob.attacker)
	elseif (mob.attacker > 0) then
		attacker = Player(mob.attacker)
	end
	
	ded = mob.armor / (mob.armor + 400 + 95 * (attacker.level + attacker.tier^2 + attacker.mark^3))
	
	if (attacker:hasDuration("starburst")) then
		if (print ~= 2) then
			attacker:setDuration("starburst", 0)
		end
		
		amount = amount * 1.5
	end
	
	if (sleep > 0 and print == 2) then
		amount = amount * mob.sleep
	elseif (sleep == 1) then
		amount = amount * mob.sleep
		mob.sleep = 1
	elseif (sleep == 2) then
		amount = amount * mob.sleep
	end
	
	if (deduction == 1) then
		if (mob.deduction < 0) then
			amount = 0
		elseif (mob.deduction > 0) then
			amount = amount * mob.deduction
		end
	end
	
	if (ac == 1) then
		if (ded < .85) then
			amount = amount * (1 - ded)
		else
			amount = amount * .15
		end
	end
	
	if (ds > 0 and print == 2) then
		amount = amount - mob.dmgShield
	elseif (ds == 1) then
		if (mob.dmgShield > 0) then
			if (mob.dmgShield > amount) then
				mob.dmgShield = mob.dmgShield - amount
				amount = 0
			else
				amount = amount - mob.dmgShield
				mob.dmgShield = 0
			end
		else
			amount = amount - mob.dmgShield
			mob.dmgShield = 0
		end
	elseif (ds == 2) then
		mob.dmgShield = mob.dmgShield - amount
	end
	
	if (attacker ~= nil) then
		attacker.damage = amount
		attacker.critChance = 0
	else
		mob.damage = amount
		mob.critChance = 0
	end
	
	if (print == 1) then
		if (mob.health - amount <= 0) then
			mob.health = 0
			mob.state = MOB_DEAD
		else
			mob.health = mob.health - amount
		end
	elseif (print == 2) then
		return amount
	else
		if (mob.aiType == 0) then
			mob_ai_basic.on_attacked(mob, attacker)
		elseif (mob.aiType == 1) then
			mob_ai_normal.on_attacked(mob, attacker)
		elseif (mob.aiType == 2) then
			mob_ai_hard.on_attacked(mob, attacker)
		elseif (mob.aiType == 3) then
			mob_ai_boss.on_attacked(mob, attacker)
		elseif (mob.aiType == 4) then
			mob:callBase("on_attacked")
		elseif (mob.aiType == 5) then
			mob_ai_ghost.on_attacked(mob, attacker)
		end
	end
end




--Mob adjustments
function Mob.changeMove(mob, amount)
	if (mob.newMove + amount < 1) then
		return false
	else
		mob.newMove = mob.newMove + amount
		return true
	end
end

function Mob.changeAttack(mob, amount)
	if (mob.newAttack + amount < 1) then
		return false
	else
		mob.newAttack = mob.newAttack + amount
		return true
	end
end




--Mob actions
function Mob.attackIT(mob,block,max_dam,min_dam)
	local dam_calc=0
	local hit_miss=0	
	local hit=0
	local min_dam2=0
	local max_dam2=0
	min_dam2=min_dam2+min_dam
	max_dam2=max_dam2+max_dam
	mob:playSound(mob.sound)
	--[[if(player.blType==BL_MOB) then
			player:talk(0,"Omfg!")
			player.attacker=mob.ID
			player:removeHealth(10)
			return
	end]]--
	if(block~=nil) then
		mob.target = block.ID
		--[[local ac=block.ac
		dam_calc=math.random(min_dam2,max_dam2)
		hit_miss=(55+(mob.grace))-(block.grace*0.5)+(mob.hit)+(mob.level)-(block.level/2)
		if(hit_miss<5) then hit_miss=5 end
		if(hit_miss>95) then hit_miss=95 end
		
		local chance=math.random(0,100)
		if(chance<hit_miss) then
			local crit=(mob.hit*0.2)
			if(chance<crit) then
				hit=2
				block:playSound(349)
				block:playSound(351)
			else
				hit=1
			end
		end

		dam_calc=dam_calc*CalculateIncrease(mob,block,hit)
		--dam_calc=dam_calc+dam_calc*block.ac*0.01
		mob:sendAction(1,14)
		]]--
		mob.critChance = hitCritChance(mob, block)
		
		if(mob.critChance>0) then
			mob.damage = swingDamage(mob, block)
			block.attacker=mob.ID
			--dam_calc=math.floor(dam_calc+0.5)
			--if(dam_calc>0) then
			if(block.blType==BL_PC) then
				block:deductArmor()
				block:showHealth(mob.damage, mob.critChance)
			end
				--player:sendMinitext("They hit you for " .. dam_calc.. "")
				
			if(block.blType==BL_MOB) then
				block.attacker=mob.ID
				block:removeHealth(mob.damage, mob.critChance)
			end
				--mob:broadcast(block.m,""..dam_calc.."")
			--end
		end
	end	
end

function Mob.attackNoCrit(mob,block,max_dam,min_dam)
	local dam_calc=0
	local hit_miss=0	
	local hit=0
	local min_dam2=0
	local max_dam2=0
	min_dam2=min_dam2+min_dam
	max_dam2=max_dam2+max_dam
	mob:playSound(mob.sound)
	--[[if(block.blType==BL_MOB) then
			block:talk(0,"Omfg!")
			block.attacker=mob.ID
			block:removeHealth(10)
			return
	end]]--
	if(block~=nil) then
		mob.target = block.ID
		local ac=block.ac
		dam_calc=math.random(min_dam2,max_dam2)
		hit_miss=(55+(mob.grace))-(block.grace*0.5)+(mob.hit)+(mob.level)-(block.level/2)
		if(hit_miss<5) then hit_miss=5 end
		if(hit_miss>95) then hit_miss=95 end
		
		local chance=math.random(0,100)
		if(chance<hit_miss) then
			local crit=(mob.hit*0.2)
			if(chance<crit) then
				hit=1
				block:playSound(349)
				block:playSound(351)
			else
				hit=1
			end
		end
		
		dam_calc=dam_calc*CalculateIncrease(mob,block,0)
		--dam_calc=dam_calc+dam_calc*block.ac*0.01
		mob:sendAction(1,14)
		mob.critChance = hit
		mob.damage = dam_calc

		if(hit>0) then
			dam_calc=math.floor(dam_calc+0.5)
			if(dam_calc>0) then
				if(block.blType==BL_PC) then block:deductArmor() end
				--block:sendMinitext("They hit you for " .. dam_calc.. "")
				block.attacker=mob.ID
				block:removeHealthExtend(dam_calc, 1, 1, 1, 1, 0)
				--mob:broadcast(block.m,""..dam_calc.."")
			end
		end
	end	
end

function CalculateIncrease(mob,player,hit)
	local news=math.abs(mob.side-player.side)
	local total=1.5
	if(mob.side==player.side) then
		total=3;
	elseif(news==2) then
		total=1;
	end
	
	if(hit==2)  then
		total=total*3
	end
	return total
end

function Mob.adjustSide(mob,player)
	local dx=mob.x-player.x
	local dy=mob.y-player.y

	if(dx==-1) then
		if(mob.side~=1) then
			mob.side=1
			mob:sendSide()
		end
	elseif(dx==1) then
		if(mob.side~=3) then
			mob.side=3
			mob:sendSide()
		end
	elseif(dy==-1) then
		if(mob.side~=2) then

			mob.side=2
			mob:sendSide()
		end
	elseif(dy==1) then
		if(mob.side~=0) then
			mob.side=0
			mob:sendSide()
		end
	end
end

function FindCoords(mob,player)
	local i
	local found = false
	local oldside = mob.side
	local canmove = false
	local checkmove = math.random(0,2)
	
	if(mob:moveIntent(player.ID) == 0) then
		if(checkmove >= 1) then
			if(mob.y < player.y) then
				mob.side = 2
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.y > player.y and not canmove) then
				mob.side = 0
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.x < player.x and not canmove) then
				mob.side = 1
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.x > player.x and not canmove) then
				mob.side = 3
				mob:sendSide()
				canmove = mob:move()
			end
		else
			if(mob.x < player.x) then
				mob.side = 1
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.x > player.x and not canmove) then
				mob.side = 3
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.y < player.y and not canmove) then
				mob.side = 2
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.y > player.y and not canmove) then
				mob.side = 0
				mob:sendSide()
				canmove = mob:move()
			end
		end
		--Ok, so it's next to one of em, I THINK..let's check
		--if(dx==1 and dy==0) then return true end
		--if(dx==0 and dy==1) then return true end
	end
	
	if(mob:moveIntent(player.ID) == 1) then
		return true
	elseif(not canmove) then
		local tList = mob:getObjectsInArea(BL_PC)
		local new_T
		local loopControl = 0
		
		repeat
			new_T = math.random(#tList)
			loopControl = loopControl + 1
		until (tList[new_T].gmLevel == 0 or loopControl == 20)
		
		mob.target=tList[new_T].ID
		mob.side = oldside
		mob:sendSide()

		for i=0,10 do
			if(not found) then
				mob.side=math.random(0,3)
				mob:sendSide()
				found=mob:move()
			end
		end
	end
		--mob.target=tList[new_T].ID
		--mob:talk(0,"New Target is " .. tList[new_T].ID)
	--[[elseif(num==0) then
		local tList=mob:getObjectsInArea(BL_PC);
		
		local new_T=math.random(1,#tList);
		mob.target=tList[new_T].ID
		--mob:talk(0,"New Target is " .. tList[new_T].ID)
		for i=0,10 do
			if(found~=true) then
				mob.side=math.random(0,3)
				mob:sendSide()
				if(mob.side == oldside) then
					found=mob:move()
				end
			end
		end]]--
	return false
end

function RunAway(mob,player)
	local i
	local found = false
	local oldside = mob.side
	local canmove = false
	local checkmove = math.random(0,2)

	if (mob:moveIntent(player.ID) == 1) then
		if(mob.side <= 1) then
			mob.side = mob.side + 2
			mob:sendSide()
			canmove = mob:move()
		elseif(mob.side >= 2) then
			mob.side = mob.side - 2
			mob:sendSide()
			canmove = mob:move()
		end
	else
		if(checkmove >= 1) then
			if(mob.y < player.y) then
				mob.side = 0
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.y > player.y and not canmove) then
				mob.side = 2
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.x < player.x and not canmove) then
				mob.side = 3
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.x > player.x and not canmove) then
				mob.side = 1
				mob:sendSide()
				canmove = mob:move()
			end
		else
			if(mob.x < player.x) then
				mob.side = 3
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.x > player.x and not canmove) then
				mob.side = 1
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.y < player.y and not canmove) then
				mob.side = 0
				mob:sendSide()
				canmove = mob:move()
			end
			if(mob.y > player.y and not canmove) then
				mob.side = 2
				mob:sendSide()
				canmove = mob:move()
			end
		end
	end

	if (not canmove) then
		local tList = mob:getObjectsInArea(BL_PC)
		local new_T
		local loopControl = 0

		repeat
			new_T = math.random(#tList)
			loopControl = loopControl + 1
		until (tList[new_T].gmLevel == 0 or loopControl == 20)
		
		mob.target = tList[new_T].ID
		mob.side = oldside
		mob:sendSide()

		for i = 1, 10 do
			if (found == 0) then
				mob.side = math.random(0, 3)
				mob:sendSide()
				found = mob:move()
			end
		end
		
		return found
	else
		return canmove
	end
end

function FindCoordsGhost(mob,player)
	local i
	local found = false
	local oldside = mob.side
	local canmove = false
	local checkmove = math.random(0,2)
	
	if(mob:moveIntent(player.ID) == 0) then
		if(checkmove >= 1) then
			if(mob.y < player.y) then
				mob.side = 2
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.y > player.y and canmove == 0) then
				mob.side = 0
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.x < player.x and canmove == 0) then
				mob.side = 1
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.x > player.x and canmove == 0) then
				mob.side = 3
				mob:sendSide()
				canmove = mob:moveGhost()
			end
		else
			if(mob.x < player.x) then
				mob.side = 1
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.x > player.x and canmove == 0) then
				mob.side = 3
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.y < player.y and canmove == 0) then
				mob.side = 2
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.y > player.y and canmove == 0) then
				mob.side = 0
				mob:sendSide()
				canmove = mob:moveGhost()
			end
		end
	end
	
	if(mob:moveIntent(player.ID) == 1) then
		return true
	elseif(not canmove) then
		local tList=mob:getObjectsInArea(BL_PC)
		local new_T=math.random(1,#tList)

		if (mob.owner > 1073741823) then
			mob.target=tList[new_T].ID
		end
		
		mob.side = oldside
		mob:sendSide()

		for i=0,10 do
			if(not found) then
				mob.side=math.random(0,3)
				mob:sendSide()
				found=mob:moveGhost()
			end
		end
	end
	
	return false
end

function RunAwayGhost(mob,player)
	local i
	local found = false
	local oldside = mob.side
	local canmove = false
	local checkmove = math.random(0,2)

	if(mob:moveIntent(player.ID)==1) then
		if(mob.side <= 1) then
			mob.side = mob.side + 2
			mob:sendSide()
			mob:moveGhost()
		elseif(mob.side >= 2) then
			mob.side = mob.side - 2
			mob:sendSide()
			mob:moveGhost()
		end
	else
		if(checkmove >= 1) then
			if(mob.y < player.y) then
				mob.side = 0
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.y > player.y and canmove == 0) then
				mob.side = 2
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.x < player.x and canmove == 0) then
				mob.side = 3
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.x > player.x and canmove == 0) then
				mob.side = 1
				mob:sendSide()
				canmove = mob:moveGhost()
			end
		else
			if(mob.x < player.x) then
				mob.side = 3
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.x > player.x and canmove == 0) then
				mob.side = 1
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.y < player.y and canmove == 0) then
				mob.side = 0
				mob:sendSide()
				canmove = mob:moveGhost()
			end
			if(mob.y > player.y and canmove == 0) then
				mob.side = 2
				mob:sendSide()
				canmove = mob:moveGhost()
			end
		end
	end

	if(not canmove) then
		local tList=mob:getObjectsInArea(BL_PC)
		local new_T=math.random(1,#tList)

		mob.target=tList[new_T].ID
		mob.side = oldside
		mob:sendSide()

		for i=0,10 do
			if(not found) then
				mob.side=math.random(0,3)
				mob:sendSide()
				found=mob:moveGhost()
			end
		end
	end
end

function Mob.flank(mob)
	local pcBlocks = getTargetsAround(mob, BL_PC)
	local mobBlocks = getTargetsAround(mob, BL_MOB)
	local targets = {}
	local swing = true
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if ((mob.side == 0 or mob.side == 2) and pcBlocks[i].y == mob.y) then
				table.insert(targets, pcBlocks[i])
			elseif ((mob.side == 1 or mob.side == 3) and pcBlocks[i].x == mob.x) then
				table.insert(targets, pcBlocks[i])
			end
		end
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if ((mob.side == 0 or mob.side == 2) and mobBlocks[i].y == mob.y) then
				table.insert(targets, mobBlocks[i])
			elseif ((mob.side == 1 or mob.side == 3) and mobBlocks[i].x == mob.x) then
				table.insert(targets, mobBlocks[i])
			end
		end
	end
	
	if (#targets > 0) then
		for i = 1, #targets do
			swing = true
			
			if (targets[i].blType == BL_MOB and (targets[i].owner == 0 or targets[i].owner >= 1073741823)) then
				swing = false
			end
			
			if (swing) then
				hitCritChance(mob, targets[i])
					
				if (mob.critChance > 0) then
					swingDamage(mob, targets[i])
					mob.damage = mob.damage * .75
					
					if (targets[i].blType == BL_PC) then
						player_combat.on_attacked(targets[i], mob)
					elseif (targets[i].blType == BL_MOB) then
						if (targets[i].aiType == 0) then
							mob_ai_basic.on_attacked(targets[i], mob)
						elseif (targets[i].aiType == 1) then
							mob_ai_normal.on_attacked(targets[i], mob)
						elseif (targets[i].aiType == 2) then
							mob_ai_hard.on_attacked(targets[i], mob)
						elseif (targets[i].aiType == 3) then
							mob_ai_boss.on_attacked(targets[i], mob)
						elseif (targets[i].aiType == 4) then
							targets[i]:callBase("on_attacked")
						elseif (targets[i].aiType == 5) then
							mob_ai_ghost.on_attacked(targets[i], mob)
						end
					end
				end
			end
		end
	end
end

function Mob.flankWithAlly(mob)
	local pcBlocks = getTargetsAround(mob, BL_PC)
	local mobBlocks = getTargetsAround(mob, BL_MOB)
	local targets = {}
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if ((mob.side == 0 or mob.side == 2) and pcBlocks[i].y == mob.y) then
				table.insert(targets, pcBlocks[i])
			elseif ((mob.side == 1 or mob.side == 3) and pcBlocks[i].x == mob.x) then
				table.insert(targets, pcBlocks[i])
			end
		end
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if ((mob.side == 0 or mob.side == 2) and mobBlocks[i].y == mob.y) then
				table.insert(targets, mobBlocks[i])
			elseif ((mob.side == 1 or mob.side == 3) and mobBlocks[i].x == mob.x) then
				table.insert(targets, mobBlocks[i])
			end
		end
	end
	
	if (#targets > 0) then
		for i = 1, #targets do
			hitCritChance(mob, targets[i])
					
			if (mob.critChance > 0) then
				swingDamage(mob, targets[i])
				mob.damage = mob.damage * .75
				
				if (targets[i].blType == BL_PC) then
					player_combat.on_attacked(targets[i], mob)
				elseif (targets[i].blType == BL_MOB) then
					if (targets[i].aiType == 0) then
						mob_ai_basic.on_attacked(targets[i], mob)
					elseif (targets[i].aiType == 1) then
						mob_ai_normal.on_attacked(targets[i], mob)
					elseif (targets[i].aiType == 2) then
						mob_ai_hard.on_attacked(targets[i], mob)
					elseif (targets[i].aiType == 3) then
						mob_ai_boss.on_attacked(targets[i], mob)
					elseif (targets[i].aiType == 4) then
						targets[i]:callBase("on_attacked")
					elseif (targets[i].aiType == 5) then
						mob_ai_ghost.on_attacked(targets[i], mob)
					end
				end
			end
		end
	end
end

function Mob.backstab(mob)
	local pcBlocks = getTargetsAround(mob, BL_PC)
	local mobBlocks = getTargetsAround(mob, BL_MOB)
	local targets = {}
	local swing = true
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if (mob.side == 0 and pcBlocks[i].y > mob.y) then
				table.insert(targets, pcBlocks[i])
			elseif (mob.side == 1 and pcBlocks[i].x < mob.x) then
				table.insert(targets, pcBlocks[i])
			elseif (mob.side == 2 and pcBlocks[i].y < mob.y) then
				table.insert(targets, pcBlocks[i])
			elseif (mob.side == 3 and pcBlocks[i].x > mob.x) then
				table.insert(targets, pcBlocks[i])
			end
		end
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mob.side == 0 and mobBlocks[i].y > mob.y) then
				table.insert(targets, mobBlocks[i])
			elseif (mob.side == 1 and mobBlocks[i].x < mob.x) then
				table.insert(targets, mobBlocks[i])
			elseif (mob.side == 2 and mobBlocks[i].y < mob.y) then
				table.insert(targets, mobBlocks[i])
			elseif (mob.side == 3 and mobBlocks[i].x > mob.x) then
				table.insert(targets, mobBlocks[i])
			end
		end
	end
	
	if (#targets > 0) then
		for i = 1, #targets do
			swing = true
			
			if (targets[i].blType == BL_MOB and (targets[i].owner == 0 or targets[i].owner >= 1073741823)) then
				swing = false
			end
			
			if (swing) then
				hitCritChance(mob, targets[i])
					
				if (mob.crit > 0) then
					swingDamage(mob, targets[i])
					mob.damage = mob.damage * .5
					
					if (targets[i].blType == BL_PC) then
						player_combat.on_attacked(targets[i], mob)
					elseif (targets[i].blType == BL_MOB) then
						if (targets[i].aiType == 0) then
							mob_ai_basic.on_attacked(targets[i], mob)
						elseif (targets[i].aiType == 1) then
							mob_ai_normal.on_attacked(targets[i], mob)
						elseif (targets[i].aiType == 2) then
							mob_ai_hard.on_attacked(targets[i], mob)
						elseif (targets[i].aiType == 3) then
							mob_ai_boss.on_attacked(targets[i], mob)
						elseif (targets[i].aiType == 4) then
							targets[i]:callBase("on_attacked")
						elseif (targets[i].aiType == 5) then
							mob_ai_ghost.on_attacked(targets[i], mob)
						end
					end
				end
			end
		end
	end
end

function Mob.backstabWithAlly(mob)
	local pcBlocks = getTargetsAround(mob, BL_PC)
	local mobBlocks = getTargetsAround(mob, BL_MOB)
	local targets = {}
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if (mob.side == 0 and pcBlocks[i].y > mob.y) then
				table.insert(targets, pcBlocks[i])
			elseif (mob.side == 1 and pcBlocks[i].x < mob.x) then
				table.insert(targets, pcBlocks[i])
			elseif (mob.side == 2 and pcBlocks[i].y < mob.y) then
				table.insert(targets, pcBlocks[i])
			elseif (mob.side == 3 and pcBlocks[i].x > mob.x) then
				table.insert(targets, pcBlocks[i])
			end
		end
	end
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mob.side == 0 and mobBlocks[i].y > mob.y) then
				table.insert(targets, mobBlocks[i])
			elseif (mob.side == 1 and mobBlocks[i].x < mob.x) then
				table.insert(targets, mobBlocks[i])
			elseif (mob.side == 2 and mobBlocks[i].y < mob.y) then
				table.insert(targets, mobBlocks[i])
			elseif (mob.side == 3 and mobBlocks[i].x > mob.x) then
				table.insert(targets, mobBlocks[i])
			end
		end
	end
	
	if (#targets > 0) then
		for i = 1, #targets do
			hitCritChance(mob, targets[i])
				
			if (mob.crit > 0) then
				swingDamage(mob, targets[i])
				mob.damage = mob.damage * .5
				
				if (targets[i].blType == BL_PC) then
					player_combat.on_attacked(targets[i], mob)
				elseif (targets[i].blType == BL_MOB) then
					if (targets[i].aiType == 0) then
						mob_ai_basic.on_attacked(targets[i], mob)
					elseif (targets[i].aiType == 1) then
						mob_ai_normal.on_attacked(targets[i], mob)
					elseif (targets[i].aiType == 2) then
						mob_ai_hard.on_attacked(targets[i], mob)
					elseif (targets[i].aiType == 3) then
						mob_ai_boss.on_attacked(targets[i], mob)
					elseif (targets[i].aiType == 4) then
						targets[i]:callBase("on_attacked")
					elseif (targets[i].aiType == 5) then
						mob_ai_ghost.on_attacked(targets[i], mob)
					end
				end
			end
		end
	end
end

function Mob.checkToObject(mob, object)
	local canmove = false
	local found = false
	local checkmove = math.random(0,2)
	
	if (checkmove >= 1) then
		if (mob.y < object.y) then
			mob.side = 2
			mob:sendSide()
			canmove = mob:checkMove()
		end
		if (mob.y > object.y and canmove == false) then
			mob.side = 0
			mob:sendSide()
			canmove = mob:checkMove()
		end
		if (mob.x < object.x and canmove == false) then
			mob.side = 1
			mob:sendSide()
			canmove = mob:checkMove()
		end
		if (mob.x > object.x and canmove == false) then
			mob.side = 3
			mob:sendSide()
			canmove = mob:checkMove()
		end
	else
		if (mob.x < object.x) then
			mob.side = 1
			mob:sendSide()
			canmove = mob:checkMove()
		end
		if (mob.x > object.x and canmove == false) then
			mob.side = 3
			mob:sendSide()
			canmove = mob:checkMove()
		end
		if (mob.y < object.y and canmove == false) then
			mob.side = 2
			mob:sendSide()
			canmove = mob:checkMove()
		end
		if (mob.y > object.y and canmove == false) then
			mob.side = 0
			mob:sendSide()
			canmove = mob:checkMove()
		end
	end
	
	if (canmove == false) then
		for i = 0, 10 do
			if (found == false) then
				mob.side = math.random(0, 3)
				mob:sendSide()
				found = mob:checkMove()
			end
		end
		
		return found
	else
		return canmove
	end
end



roomExpTotalIx = function(player, interval)
	local mapBlocks = player:getObjectsInMap(player.m, BL_MOB)
	local maxPotential = 0
	local expectedPotential = 0
	local totalExp = 0;
	local averageSpawntime = 0
	local lastMobCount = 0
	local onceOffExp = player.exp
	
	player:talk(1, "<|>o========= Room #"..player.m.."====="..interval.." minutes =================o")
    --now recalculate
	if (#mapBlocks > 0) then
		for i = 1, #mapBlocks do
		    if (mapBlocks[i].experience > 0) then
			  maxPotential = maxPotential + mapBlocks[i].experience * (60/mapBlocks[i].spawnTime) * interval
			  totalExp = totalExp + mapBlocks[i].experience
			  lastMobCount = lastMobCount + 1
			  averageSpawntime = averageSpawntime + mapBlocks[i].spawnTime
			end
		end
	end
	
	--player:talk(1, "<|>      --- First wave ---")
	--player:talk(1, "<|> Max experience     : "..format_number(math.floor(maxPotential)))
	--player:talk(1, "<|> Expected experience: "..format_number(math.floor((maxPotential*0.65))))
	if (lastMobCount ~= 0) then
	  averageSpawntime = averageSpawntime/lastMobCount
	else
	  player:talk(1, "No mobs giving experience.")
	  return
	end
	
	killmobs.cast(player)
	--room_hellfire.cast(player,player)
		
	local mapBlocks = player:getObjectsInMap(player.m, BL_MOB)
	
	if (#mapBlocks > 0) then
		for i = 1, #mapBlocks do
		    if (mapBlocks[i].experience > 0) then
			  maxPotential = maxPotential + mapBlocks[i].experience * (60/averageSpawntime) * interval
			  totalExp = totalExp + mapBlocks[i].experience
			  lastMobCount = lastMobCount + 1
			end
		end
	end
	
	killmobs.cast(player)
	--room_hellfire.cast(player,player)
	
	onceOffExp = player.exp - onceOffExp
	
	expectedPotential = maxPotential * .65
	player:talk(1, "<|> # Mob count        : " .. (lastMobCount))
	player:talk(1, "<|> Average Exp per mob: " .. format_number(math.floor(totalExp/lastMobCount)))
	player:talk(1, "<|> Average Spawn times: " .. format_number(math.floor(10*averageSpawntime)/10).."seconds")
	player:talk(1, "<|> Room clear         : "..format_number(math.floor(onceOffExp)))
	player:talk(1, "<|> Max experience     : "..format_number(math.floor(maxPotential)))
	player:talk(1, "<|> Expected experience: "..format_number(math.floor((expectedPotential))))
	--player:talk(0, "Room: "..room.." Max: "..maxPotential.." Expected: "..expectedPotential)
end