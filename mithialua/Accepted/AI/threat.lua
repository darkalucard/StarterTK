threat = {
	calcHighestThreat=function(mob)
		local threat_id = {}
		local threat_amount = {}
		local randtarget = math.random(4)
		local highest_threat = 0
		local current_target = 0
		local targetFacing = getTargetFacing(mob, BL_PC)
		
		local e=mob:getObjectsInCell(mob.m,mob.x+1,mob.y,BL_PC)
		--i=mob:getObjectsInCell(mob.m,mob.x+1,mob.y,BL_MOB)
		local f=mob:getObjectsInCell(mob.m,mob.x,mob.y-1,BL_PC)
		--j=mob:getObjectsInCell(mob.m,mob.x,mob.y-1,BL_MOB)
		local g=mob:getObjectsInCell(mob.m,mob.x-1,mob.y,BL_PC)
		--k=mob:getObjectsInCell(mob.m,mob.x-1,mob.y,BL_MOB)
		local h=mob:getObjectsInCell(mob.m,mob.x,mob.y+1,BL_PC)
		--l=mob:getObjectsInCell(mob.m,mob.x,mob.y+1,BL_MOB)
		
		-- if surrounded by 4 players
		if (mob.target > 0 and targetFacing ~= nil and targetFacing.ID ~= mob.target and not mob:checkToObject(mob:getBlock(mob.target)) and (#e > 0 or #f > 0 or #g > 0 or #h > 0)) then -- or #i > 0 or #j > 0 or #k > 0 or #l > 0)) then
			if (#e > 0) then
				for a = 1, #e do
					if (e[a].m == mob.m and mob:checkThreat(e[a].ID) > highest_threat) then
						highest_threat = mob:checkThreat(e[a].ID)
						current_target = e[a].ID
					end
				end
			end
			if (#f > 0) then
				for a = 1, #f do
					if (f[a].m == mob.m and mob:checkThreat(f[a].ID) > highest_threat) then
						highest_threat = mob:checkThreat(f[a].ID)
						current_target = f[a].ID
					end
				end
			end
			if (#g > 0) then
				for a = 1, #g do
					if (g[a].m == mob.m and mob:checkThreat(g[a].ID) > highest_threat) then
						highest_threat = mob:checkThreat(g[a].ID)
						current_target = g[a].ID
					end
				end
			end
			if (#h > 0) then
				for a = 1, #h do
					if (h[a].m == mob.m and mob:checkThreat(h[a].ID) > highest_threat) then
						highest_threat = mob:checkThreat(h[a].ID)
						current_target = h[a].ID
					end
				end
			end
			--[[if (#i > 0) then
				for a = 1, #i do
					if(mob:checkThreat(i[a].ID) > highest_threat) then
						highest_threat = mob:checkThreat(i[a].ID)
						current_target = i[a].ID
					end
				end
			end
			if (#j > 0) then
				for a = 1, #j do
					if(mob:checkThreat(j[a].ID) > highest_threat) then
						highest_threat = mob:checkThreat(j[a].ID)
						current_target = j[a].ID
					end
				end
			end
			if (#k > 0) then
				for a = 1, #k do
					if(mob:checkThreat(k[a].ID) > highest_threat) then
						highest_threat = mob:checkThreat(k[a].ID)
						current_target = k[a].ID
					end
				end
			end
			if (#l > 0) then
				for a = 1, #l do
					if(mob:checkThreat(l[a].ID) > highest_threat) then
						highest_threat = mob:checkThreat(l[a].ID)
						current_target = l[a].ID
					end
				end
			end]]--
			--mob.state = MOB_HIT
			--[[repeat
				if(current_target == 0) then
					if(randtarget == 1 and #e > 0) then
						if ((e[1]:hasDuration("sneak") and (mob.seeInvis == 1 or mob.seeInvis == 3 or mob.seeInvis == 5))
						or (e[1]:hasDuration("cloak") and (mob.seeInvis == 2 or mob.seeInvis == 3 or mob.seeInvis == 5))
						or (e[1]:hasDuration("hide") and (mob.seeInvis == 4 or mob.seeInvis == 5))
						or not(e[1]:hasDuration("sneak") and e[1]:hasDuration("cloak") and e[1]:hasDuration("hide"))) then
							current_target = e[1].ID
						end
					elseif (randtarget == 2 and #f > 0) then
						if ((f[1]:hasDuration("sneak") and (mob.seeInvis == 1 or mob.seeInvis == 3 or mob.seeInvis == 5))
						or (f[1]:hasDuration("cloak") and (mob.seeInvis == 2 or mob.seeInvis == 3 or mob.seeInvis == 5))
						or (f[1]:hasDuration("hide") and (mob.seeInvis == 4 or mob.seeInvis == 5))
						or not(f[1]:hasDuration("sneak") and f[1]:hasDuration("cloak") and f[1]:hasDuration("hide"))) then
							current_target = f[1].ID
						end
					elseif (randtarget == 3 and #g > 0) then
						if ((g[1]:hasDuration("sneak") and (mob.seeInvis == 1 or mob.seeInvis == 3 or mob.seeInvis == 5))
						or (g[1]:hasDuration("cloak") and (mob.seeInvis == 2 or mob.seeInvis == 3 or mob.seeInvis == 5))
						or (g[1]:hasDuration("hide") and (mob.seeInvis == 4 or mob.seeInvis == 5))
						or not(g[1]:hasDuration("sneak") and g[1]:hasDuration("cloak") and g[1]:hasDuration("hide"))) then
							current_target = g[1].ID
						end
					elseif (randtarget == 4 and #h > 0) then
						if ((h[1]:hasDuration("sneak") and (mob.seeInvis == 1 or mob.seeInvis == 3 or mob.seeInvis == 5))
						or (h[1]:hasDuration("cloak") and (mob.seeInvis == 2 or mob.seeInvis == 3 or mob.seeInvis == 5))
						or (h[1]:hasDuration("hide") and (mob.seeInvis == 4 or mob.seeInvis == 5))
						or not(h[1]:hasDuration("sneak") and h[1]:hasDuration("cloak") and h[1]:hasDuration("hide"))) then
							current_target = h[1].ID
						end
					elseif (#e == 0 and #f == 0 and #g == 0 and #h == 0) then
						break
					elseif(randtarget == 5 and #i > 0) then
						current_target = i[1].ID
					elseif(randtarget == 6 and #j > 0) then
						current_target = j[1].ID
					elseif(randtarget == 7 and #k > 0) then
						current_target = k[1].ID
					elseif(randtarget == 8 and #l > 0) then
						current_target = l[1].ID
					end
				end
				randtarget = math.random(4)
			until (current_target > 0)]]--
		-- otherwise it will check for everything in room
		else
			threat_id = mob:getObjectsInArea(BL_PC)
			
			if (#threat_id>0) then
				for i = 1, #threat_id do
					if (threat_id[i].m == mob.m and mob:checkThreat(threat_id[i].ID) ~= false) then
						threat_amount[i] = mob:checkThreat(threat_id[i].ID)
					end
				end
			end
			
			if (#threat_amount>0) then
				for i = 1, #threat_amount do
					if (threat_amount[i] > highest_threat) then
						highest_threat = threat_amount[i]
						current_target = threat_id[i].ID
						--mob.state = MOB_HIT
					end
				end
			end
		end
		
		if (current_target > 0) then
			mob.target = current_target
		end
	end,
	
	getHighestThreat=function(mob)
		local threat_id = {}
		local threat_amount = {}
		local highest_threat = 0

		threat_id = mob:getObjectsInArea(BL_PC)
		if(#threat_id>0) then
			for i=1 , #threat_id do
				if(mob:checkThreat(threat_id[i].ID) ~= false) then
					threat_amount[i] = mob:checkThreat(threat_id[i].ID)
				end
			end
		end
		if(#threat_amount>0) then
			for i=1 , #threat_amount do
				if(threat_amount[i] > highest_threat) then
					highest_threat = threat_amount[i]
				end
			end
		end
		return highest_threat
	end,

	getThreatTable=function(mob)
		local threat_id = {}
		local threat_amount = {}

		threat_id = mob:getObjectsInArea(BL_PC)
		if(#threat_id>0) then
			for i=1 , #threat_id do
				if(mob:checkThreat(threat_id[i].ID) ~= false) then
					threat_amount[i] = mob:checkThreat(threat_id[i].ID)
				end
			end
		end

		table.sort(threat_amount, function(a,b) return a>b end)

		return threat_amount
	end,

	getThreatID=function(mob,amount)
		local threat_id = {}
		
		threat_id = mob:getObjectsInArea(BL_PC)
		if(#threat_id>0) then
			for i=1 , #threat_id do
				if(mob:checkThreat(threat_id[i].ID) ~= false) then
					if(mob:checkThreat(threat_id[i].ID) == amount) then
						return threat_id[i]
					end
				end
			end
		end
	end
}