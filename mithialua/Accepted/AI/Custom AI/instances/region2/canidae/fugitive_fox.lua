fugitive_fox = {
move = function(mob)
	local pcBlocks = mob:getObjectsInArea(BL_PC)
	local mobBlocks = getTargetsAround(mob, BL_MOB)
	local pcAround = getTargetsAround(mob, BL_PC)
	local holders = {}
	local distCheck = 100
	local direction = -1
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if (pcBlocks[i]:hasItem("fox_lure", 1, 0) == true) then
				table.insert(holders, pcBlocks[i].ID)
			end
		end
	end
	
	if (#holders > 0) then
		for i = 1, #holders do
			if (distance(Player(holders[i]), mob) < distCheck) then
				if (mob.target == 0) then
					distCheck = holders[i]
				else
					distCheck = mob.target
				end
			end
		end
	end
	
	if (distCheck ~= 100) then
		mob.target = distCheck
		
		if (#pcAround > 0) then
			for i = 1, #pcAround do
				if (pcAround[i].ID ~= mob.target) then
					if (math.random(100) < 35) then
						distCheck = pcAround[i].ID
						mob.target = distCheck
						break
					end
				end
			end
		end
		if (#mobBlocks > 0) then
			for i = 1, #mobBlocks do
				if (mobBlocks[i].mobID == 140) then
					direction = getSide(mob, mobBlocks[i])
					break
				end
			end
		end
		
		if (direction ~= -1) then
			if (direction == 0) then
				direction = 2
			elseif (direction == 1) then
				direction = 3
			elseif (direction == 2) then
				direction = 0
			elseif (direction == 3) then
				direction = 1
			end
			mob.side = direction
			mob:sendSide()
			mob:move()
			return
		end
		--mob:talk(2,""..mob.target)
		FindCoords(mob, Player(mob.target))
	end
end
}