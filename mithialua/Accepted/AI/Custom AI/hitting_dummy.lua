hitting_dummy = {

move = function(mob,target)

	local moved = true
	
		--mob.side = 2
		--mob:sendSide()
		
		--objectCanMove (x, y, side) --returns true if can move
		--objectCanMoveFrom (x, y, side)
		
		if (mob.health < (mob.maxHealth*0.15)) then
			if (mob.mapRegistry['harakiri'] > 3) then
				mob.mapRegistry['harakiri'] = 0
			end
			
			if (mob.mapRegistry['harakiri'] ~= nil) then
				mob.mapRegistry['harakiri'] = (mob.mapRegistry['harakiri']+1)
			elseif (mob.mapRegistry['harakiri'] == nil) then
				mob.mapRegistry['harakiri'] = 1
			else
				mob:talk(0, "WTF I doing here!")
			end
			
			if (mob.mapRegistry['harakiri'] == 2) then
				mob:sendAnimation(124)
				mob:playSound(9)
			elseif (mob.mapRegistry['harakiri'] == 3) then
				local aichaichaich = mob.health
				mob:removeHealth(aichaichaich)
				mob.mapRegistry['harakiri'] = 0
			end
			
			mob:talk(2, "Harakiri!")
			
		elseif (mob.health < (mob.maxHealth*0.25)) then
			honey_rain_mob.cast(mob,mob)
		elseif (mob.health < (mob.maxHealth*0.35)) then
			honey_rain_mob.cast(mob,mob)
		elseif (mob.health < (mob.maxHealth*0.55)) then
			honey_rain_mob.cast(mob,mob)
		end
		
		mob.target = Player('Krisalia').ID
		
		local x = mob.x
		local y = mob.y
		local x2 = target.x
		local y2 = target.y
		
		if(x<(x2+1)) and (y ==(y2-1)) then --corner top left
			mob.side = 1
			mob.sendSide()
			
			if(check((mob.x), mob.y, mob.side, mob) == true) then
				moved = mob:move()
			end
		elseif (x==(x2+1)) and (y < (y2+1)) then --corner top right
			mob.side = 2
			mob.sendSide()
			
			if(check(mob.x, (mob.y+1), mob.side, mob) == true) then
				moved = mob:move()
			end
		elseif (x>(x2-1)) and (y == (y2+1)) then --corner bottom right
			mob.side = 3
			mob.sendSide()
			
			if(check((mob.x), mob.y, mob.side, mob) == true) then
				moved = mob:move()
			end
		elseif(x == (x2-1)) and (y > (y2-1)) then --corner bottom left
			mob.side = 0
			mob.sendSide()
			
			if(check(mob.x, (mob.y), mob.side, mob) == true) then
				moved=mob:move()
			end
		else
			if(mob.state ~= MOB_HIT) then
				mob.side=math.random(0,3)
			else
				mob_ai_basic.attack(mob,target)
			end
		end
end
}