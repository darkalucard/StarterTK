mathRandom = function(start, stop, precision)
	return math.random(start * precision, stop * precision) / precision
end

checkProtection = function(block, target, successRate)
	local blockWill = (block.will + 1)
	local blockLevel = block.level
	local targetWill = 1
	local targetLevel = 1
	local targetProtection = 1
	local minhit = .1
	local maxhit = .95
	
	if (target ~= nil) then
		targetWill = (target.will + 1)
		targetLevel = target.level
		targetProtection = (target.protection + 1)
	end
	
	if (block.blType == BL_PC or block.blType == 1) then
		--PLAYER hitchance
		local hitchance = .1 + (blockLevel / 1000) + (blockWill / 100)
		
		if (target ~= nil) then
			hitchance = hitchance - (targetWill / 100) + (targetProtection / 100) + (targetLevel / 1000)
		end
		
		if (hitchance < minhit) then
			hitchance = minhit
		elseif (hitchance > maxhit) then
			hitchance = maxhit
		end
		
		if (math.random(successRate) > math.random(hitchance * 10000)) then
			return true
		else
			return false
		end
	elseif (block.blType == BL_MOB) or (block.blType == 2) then
		--MOBs hitchance
		local hitchance = .1 + (blockLevel / 1000) + (blockWill / 100)
		
		if (target ~= nil) then
			hitchance = hitchance - (targetWill / 100) + (targetProtection / 100) + (targetLevel / 1000)
		end
		
		if (hitchance < minhit) then
			hitchance = minhit
		elseif (hitchance > maxhit) then
			hitchance = maxhit
		end
		
		if (math.random(successRate) > math.random(hitchance * 10000)) then
			return true
		else
			return false
		end
	end
end





hitCritChance = function(block, target)
	local bg = block.grace
	local bl = block.level
	local bm = block.might
	local bh = block.hit
	local bd = block.dam
	local bc = block.crit
	local bs = block.miss
	local tg = target.grace
	local tl = target.level
	
	local minhit = 5
	local maxhit = 95
	
	local critChanceIncrease = 0	
	
	if (math.random(10000) < bs) then
		block.critChance = 0
		block.damage = 0
		--if (block.blType == BL_PC) then
		--	block:sendMinitext("You have faltered.")
		--end
		return
	end
	
	if (block.blType == BL_PC) or (block.blType == 1) then
		
		--PLAYER hitchance
		local hitchance = (55 + ((bg + bl) * .75) + bh - ((tg + tl) * .5))
		
		if (hitchance < minhit) then
			hitchance = minhit
		elseif (hitchance > maxhit) then
			critChanceIncrease = (5 * ((hitchance - 95) / 193)) + 2.5
			hitchance = maxhit
		end
		
		--math.randomseed(math.random(os.clock()))
		local seed = math.random(100)
		
		if (seed < hitchance) then
			local mincrit = 1
			local maxcrit = 30
			--PLAYER critchance
			local critchance = 10 + critChanceIncrease
			--((bg*2) - (bl*1.5) - (tg/2) - (bm/3) + (bh*1.5) - bd + bc)
			--local rcrit = math.random(90,100)
			--block.critMult = math.abs(block.critMult + ((math.abs(critchance) - rcrit) / rcrit))
			if (critchance < mincrit) then
				critchance = mincrit
			elseif (critchance > maxcrit) then
				critchance = maxcrit
			end
			
			if (seed < critchance) then
				block.critChance = 2
			else
				block.critChance = 1
			end
			
			if (target.blType == BL_PC and not block:canPK(target)) then
				block.critChance = 0
			end
		else
			block.critChance = 0
		end
	elseif (block.blType == BL_MOB) or (block.blType == 2) then
		--MOBs hitchance
		local hitchance = (55 + (bg / 2)) - (tg / 3) + (bh * 3) + (bl - tl)
		
		block:sendAction(1,14)
		block:playSound(block.sound)
		
		if (hitchance < minhit) then
			hitchance = minhit
		elseif (hitchance > maxhit) then
			critChanceIncrease = (math.random(5)) + 2.5
			hitchance = maxhit
		end
		
		local seed = math.random(100)
		
		if (seed < hitchance) then
			--MOBs critchance
			local critchance = (block.hit / 5) + critChanceIncrease
			--local rcrit = math.random(90,100)
			--block.critMult = math.abs(block.critMult + ((math.abs(critchance) - rcrit) / rcrit))
			if (seed < critchance) then
				block.critChance = 2
				block:playSound(349)
				block:playSound(351)
			else
				block.critChance = 1
			end
		else
			block.critChance = 0
		end
	end
end




swing = function(player)
	local mobUp = player:getObjectsInCell(player.m, player.x, player.y - 1, BL_MOB)
	local mobLeft = player:getObjectsInCell(player.m, player.x - 1, player.y, BL_MOB)
	local mobRight = player:getObjectsInCell(player.m, player.x + 1, player.y, BL_MOB)
	local mobDown = player:getObjectsInCell(player.m, player.x, player.y + 1, BL_MOB)
	local mobUpExtend = player:getObjectsInCell(player.m, player.x, player.y - 2, BL_MOB)
	local mobLeftExtend = player:getObjectsInCell(player.m, player.x - 2, player.y, BL_MOB)
	local mobRightExtend = player:getObjectsInCell(player.m, player.x + 2, player.y, BL_MOB)
	local mobDownExtend = player:getObjectsInCell(player.m, player.x, player.y + 2, BL_MOB)
	local mobUpLeftExtend = player:getObjectsInCell(player.m, player.x - 1, player.y - 1, BL_MOB)
	local mobUpRightExtend = player:getObjectsInCell(player.m, player.x + 1, player.y - 1, BL_MOB)
	local mobDownLeftExtend = player:getObjectsInCell(player.m, player.x - 1, player.y + 1, BL_MOB)
	local mobDownRightExtend = player:getObjectsInCell(player.m, player.x + 1, player.y + 1, BL_MOB)
	local pcUp = player:getObjectsInCell(player.m, player.x, player.y - 1, BL_PC)
	local pcLeft = player:getObjectsInCell(player.m, player.x - 1, player.y, BL_PC)
	local pcRight = player:getObjectsInCell(player.m, player.x + 1, player.y, BL_PC)
	local pcDown = player:getObjectsInCell(player.m, player.x, player.y + 1, BL_PC)
	local pcUpExtend = player:getObjectsInCell(player.m, player.x, player.y - 2, BL_PC)
	local pcLeftExtend = player:getObjectsInCell(player.m, player.x - 2, player.y, BL_PC)
	local pcRightExtend = player:getObjectsInCell(player.m, player.x + 2, player.y, BL_PC)
	local pcDownExtend = player:getObjectsInCell(player.m, player.x, player.y + 2, BL_PC)
	local pcUpLeftExtend = player:getObjectsInCell(player.m, player.x - 1, player.y - 1, BL_PC)
	local pcUpRightExtend = player:getObjectsInCell(player.m, player.x + 1, player.y - 1, BL_PC)
	local pcDownLeftExtend = player:getObjectsInCell(player.m, player.x - 1, player.y + 1, BL_PC)
	local pcDownRightExtend = player:getObjectsInCell(player.m, player.x + 1, player.y + 1, BL_PC)
	local extendHit = player.extendHit
	
	if (player.side == 0) then		
		if (extendHit) then
			if (#mobUp > 0) then
				if (#mobUpExtend > 0) then
					for i = 1, #mobUpExtend do
						player:swingTarget(mobUpExtend[i])
					end
				end
				
				if (#mobUpLeftExtend > 0) then
					for i = 1, #mobUpLeftExtend do
						player:swingTarget(mobUpLeftExtend[i])
					end
				end
				
				if (#mobUpRightExtend > 0) then
					for i = 1, #mobUpRightExtend do
						player:swingTarget(mobUpRightExtend[i])
					end
				end
			end
			
			if (#mobLeft > 0 and player.flank) then
				if (#mobLeftExtend > 0) then
					for i = 1, #mobLeftExtend do
						player:swingTarget(mobLeftExtend[i])
					end
				end
				
				if (#mobUpLeftExtend > 0) then
					for i = 1, #mobUpLeftExtend do
						player:swingTarget(mobUpLeftExtend[i])
					end
				end
				
				if (#mobDownLeftExtend > 0) then
					for i = 1, #mobDownLeftExtend do
						player:swingTarget(mobDownLeftExtend[i])
					end
				end
			end
			
			if (#mobRight > 0 and player.flank) then
				if (#mobRightExtend > 0) then
					for i = 1, #mobRightExtend do
						player:swingTarget(mobRightExtend[i])
					end
				end
				
				if (#mobUpRightExtend > 0) then
					for i = 1, #mobUpRightExtend do
						player:swingTarget(mobUpRightExtend[i])
					end
				end
				
				if (#mobDownRightExtend > 0) then
					for i = 1, #mobDownRightExtend do
						player:swingTarget(mobDownRightExtend[i])
					end
				end
			end
			
			if (#mobDown > 0 and player.backstab) then
				if (#mobDownExtend > 0) then
					for i = 1, #mobDownExtend do
						player:swingTarget(mobDownExtend[i])
					end
				end
				
				if (#mobDownLeftExtend > 0) then
					for i = 1, #mobDownLeftExtend do
						player:swingTarget(mobDownLeftExtend[i])
					end
				end
				
				if (#mobDownRightExtend > 0) then
					for i = 1, #mobDownRightExtend do
						player:swingTarget(mobDownRightExtend[i])
					end
				end
			end
			
			if (#pcUp > 0) then
				if (#pcUpExtend > 0) then
					for i = 1, #pcUpExtend do
						if (player:canPK(pcUpExtend[i])) then
							player:swingTarget(pcUpExtend[i])
						end
					end
				end
				
				if (#pcUpLeftExtend > 0) then
					for i = 1, #pcUpLeftExtend do
						if (player:canPK(pcUpLeftExtend[i])) then
							player:swingTarget(pcUpLeftExtend[i])
						end
					end
				end
				
				if (#pcUpRightExtend > 0) then
					for i = 1, #pcUpRightExtend do
						if (player:canPK(pcUpRightExtend[i])) then
							player:swingTarget(pcUpRightExtend[i])
						end
					end
				end
			end
			
			if (#pcLeft > 0 and player.flank) then
				if (#pcLeftExtend > 0) then
					for i = 1, #pcLeftExtend do
						if (player:canPK(pcLeftExtend[i])) then
							player:swingTarget(pcLeftExtend[i])
						end
					end
				end
				
				if (#pcUpLeftExtend > 0) then
					for i = 1, #pcUpLeftExtend do
						if (player:canPK(pcUpLeftExtend[i])) then
							player:swingTarget(pcUpLeftExtend[i])
						end
					end
				end
				
				if (#pcDownLeftExtend > 0) then
					for i = 1, #pcDownLeftExtend do
						if (player:canPK(pcDownLeftExtend[i])) then
							player:swingTarget(pcDownLeftExtend[i])
						end
					end
				end
			end
			
			if (#pcRight > 0 and player.flank) then
				if (#pcRightExtend > 0) then
					for i = 1, #pcRightExtend do
						if (player:canPK(pcRightExtend[i])) then
							player:swingTarget(pcRightExtend[i])
						end
					end
				end
				
				if (#pcUpRightExtend > 0) then
					for i = 1, #pcUpRightExtend do
						if (player:canPK(pcUpRightExtend[i])) then
							player:swingTarget(pcUpRightExtend[i])
						end
					end
				end
				
				if (#pcDownRightExtend > 0) then
					for i = 1, #pcDownRightExtend do
						if (player:canPK(pcDownRightExtend[i])) then
							player:swingTarget(pcDownRightExtend[i])
						end
					end
				end
			end
			
			if (#pcDown > 0 and player.backstab) then
				if (#pcDownExtend > 0) then
					for i = 1, #pcDownExtend do
						if (player:canPK(pcDownExtend[i])) then
							player:swingTarget(pcDownExtend[i])
						end
					end
				end
				
				if (#pcDownLeftExtend > 0) then
					for i = 1, #pcDownLeftExtend do
						if (player:canPK(pcDownLeftExtend[i])) then
							player:swingTarget(pcDownLeftExtend[i])
						end
					end
				end
				
				if (#pcDownRightExtend > 0) then
					for i = 1, #pcDownRightExtend do
						if (player:canPK(pcDownRightExtend[i])) then
							player:swingTarget(pcDownRightExtend[i])
						end
					end
				end
			end
		end
		
		if (#mobLeft > 0 and player.flank) then
			for i = 1, #mobLeft do
				player:swingTarget(mobLeft[i])
			end
		end
		
		if (#mobRight > 0 and player.flank) then
			for i = 1, #mobRight do
				player:swingTarget(mobRight[i])
			end
		end
		
		if (#mobDown > 0 and player.backstab) then
			for i = 1, #mobDown do
				player:swingTarget(mobDown[i])
			end
		end
		
		if (#pcLeft > 0 and player.flank) then
			for i = 1, #pcLeft do
				if (player:canPK(pcLeft[i])) then
					player:swingTarget(pcLeft[i])
				end
			end
		end
		
		if (#pcRight > 0 and player.flank) then
			for i = 1, #pcRight do
				if (player:canPK(pcRight[i])) then
					player:swingTarget(pcRight[i])
				end
			end
		end
		
		if (#pcDown > 0 and player.backstab) then
			for i = 1, #pcDown do
				if (player:canPK(pcDown[i])) then
					player:swingTarget(pcDown[i])
				end
			end
		end
		
		if (#mobUp > 0) then
			for i = 1, #mobUp do
				player:swingTarget(mobUp[i])
			end
		end
		
		if (#pcUp > 0) then
			for i = 1, #pcUp do
				if (player:canPK(pcUp[i])) then
					player:swingTarget(pcUp[i])
				end
			end
		end
	elseif (player.side == 1) then
		if (extendHit) then
			if (#mobRight > 0) then
				if (#mobRightExtend > 0) then
					for i = 1, #mobRightExtend do
						player:swingTarget(mobRightExtend[i])
					end
				end
				
				if (#mobUpRightExtend > 0) then
					for i = 1, #mobUpRightExtend do
						player:swingTarget(mobUpRightExtend[i])
					end
				end
				
				if (#mobDownRightExtend > 0) then
					for i = 1, #mobDownRightExtend do
						player:swingTarget(mobDownRightExtend[i])
					end
				end
			end
			
			if (#mobUp > 0 and player.flank) then
				if (#mobUpExtend > 0) then
					for i = 1, #mobUpExtend do
						player:swingTarget(mobUpExtend[i])
					end
				end
				
				if (#mobUpLeftExtend > 0) then
					for i = 1, #mobUpLeftExtend do
						player:swingTarget(mobUpLeftExtend[i])
					end
				end
				
				if (#mobUpRightExtend > 0) then
					for i = 1, #mobUpRightExtend do
						player:swingTarget(mobUpRightExtend[i])
					end
				end
			end
			
			if (#mobDown > 0 and player.flank) then
				if (#mobDownExtend > 0) then
					for i = 1, #mobDownExtend do
						player:swingTarget(mobDownExtend[i])
					end
				end
				
				if (#mobDownLeftExtend > 0) then
					for i = 1, #mobDownLeftExtend do
						player:swingTarget(mobDownLeftExtend[i])
					end
				end
				
				if (#mobDownRightExtend > 0) then
					for i = 1, #mobDownRightExtend do
						player:swingTarget(mobDownRightExtend[i])
					end
				end
			end
			
			if (#mobLeft > 0 and player.backstab) then
				if (#mobLeftExtend > 0) then
					for i = 1, #mobLeftExtend do
						player:swingTarget(mobLeftExtend[i])
					end
				end
				
				if (#mobUpLeftExtend > 0) then
					for i = 1, #mobUpLeftExtend do
						player:swingTarget(mobUpLeftExtend[i])
					end
				end
				
				if (#mobDownLeftExtend > 0) then
					for i = 1, #mobDownLeftExtend do
						player:swingTarget(mobDownLeftExtend[i])
					end
				end
			end
			
			if (#pcRight > 0) then
				if (#pcRightExtend > 0) then
					for i = 1, #pcRightExtend do
						if (player:canPK(pcRightExtend[i])) then
							player:swingTarget(pcRightExtend[i])
						end
					end
				end
				
				if (#pcUpRightExtend > 0) then
					for i = 1, #pcUpRightExtend do
						if (player:canPK(pcUpRightExtend[i])) then
							player:swingTarget(pcUpRightExtend[i])
						end
					end
				end
				
				if (#pcDownRightExtend > 0) then
					for i = 1, #pcDownRightExtend do
						if (player:canPK(pcDownRightExtend[i])) then
							player:swingTarget(pcDownRightExtend[i])
						end
					end
				end
			end
			
			if (#pcUp > 0 and player.flank) then
				if (#pcUpExtend > 0) then
					for i = 1, #pcUpExtend do
						if (player:canPK(pcUpExtend[i])) then
							player:swingTarget(pcUpExtend[i])
						end
					end
				end
				
				if (#pcUpLeftExtend > 0) then
					for i = 1, #pcUpLeftExtend do
						if (player:canPK(pcUpLeftExtend[i])) then
							player:swingTarget(pcUpLeftExtend[i])
						end
					end
				end
				
				if (#pcUpRightExtend > 0) then
					for i = 1, #pcUpRightExtend do
						if (player:canPK(pcUpRightExtend[i])) then
							player:swingTarget(pcUpRightExtend[i])
						end
					end
				end
			end
			
			if (#pcDown > 0 and player.flank) then
				if (#pcDownExtend > 0) then
					for i = 1, #pcDownExtend do
						if (player:canPK(pcDownExtend[i])) then
							player:swingTarget(pcDownExtend[i])
						end
					end
				end
				
				if (#pcDownLeftExtend > 0) then
					for i = 1, #pcDownLeftExtend do
						if (player:canPK(pcDownLeftExtend[i])) then
							player:swingTarget(pcDownLeftExtend[i])
						end
					end
				end
				
				if (#pcDownRightExtend > 0) then
					for i = 1, #pcDownRightExtend do
						if (player:canPK(pcDownRightExtend[i])) then
							player:swingTarget(pcDownRightExtend[i])
						end
					end
				end
			end
			
			if (#pcLeft > 0 and player.backstab) then
				if (#pcLeftExtend > 0) then
					for i = 1, #pcLeftExtend do
						if (player:canPK(pcLeftExtend[i])) then
							player:swingTarget(pcLeftExtend[i])
						end
					end
				end
				
				if (#pcUpLeftExtend > 0) then
					for i = 1, #pcUpLeftExtend do
						if (player:canPK(pcUpLeftExtend[i])) then
							player:swingTarget(pcUpLeftExtend[i])
						end
					end
				end
				
				if (#pcDownLeftExtend > 0) then
					for i = 1, #pcDownLeftExtend do
						if (player:canPK(pcDownLeftExtend[i])) then
							player:swingTarget(pcDownLeftExtend[i])
						end
					end
				end
			end
		end
		
		if (#mobUp > 0 and player.flank) then
			for i = 1, #mobUp do
				player:swingTarget(mobUp[i])
			end
		end
		
		if (#mobDown > 0 and player.flank) then
			for i = 1, #mobDown do
				player:swingTarget(mobDown[i])
			end
		end
		
		if (#mobLeft > 0 and player.backstab) then
			for i = 1, #mobLeft do
				player:swingTarget(mobLeft[i])
			end
		end
		
		if (#pcUp > 0 and player.flank) then
			for i = 1, #pcUp do
				if (player:canPK(pcUp[i])) then
					player:swingTarget(pcUp[i])
				end
			end
		end
		
		if (#pcDown > 0 and player.flank) then
			for i = 1, #pcDown do
				if (player:canPK(pcDown[i])) then
					player:swingTarget(pcDown[i])
				end
			end
		end
		
		if (#pcLeft > 0 and player.backstab) then
			for i = 1, #pcLeft do
				if (player:canPK(pcLeft[i])) then
					player:swingTarget(pcLeft[i])
				end
			end
		end
		
		if (#mobRight > 0) then
			for i = 1, #mobRight do
				player:swingTarget(mobRight[i])
			end
		end
		
		if (#pcRight > 0) then
			for i = 1, #pcRight do
				if (player:canPK(pcRight[i])) then
					player:swingTarget(pcRight[i])
				end
			end
		end
	elseif (player.side == 2) then
		if (extendHit) then
			if (#mobDown > 0) then
				if (#mobDownExtend > 0) then
					for i = 1, #mobDownExtend do
						player:swingTarget(mobDownExtend[i])
					end
				end
				
				if (#mobDownLeftExtend > 0) then
					for i = 1, #mobDownLeftExtend do
						player:swingTarget(mobDownLeftExtend[i])
					end
				end
				
				if (#mobDownRightExtend > 0) then
					for i = 1, #mobDownRightExtend do
						player:swingTarget(mobDownRightExtend[i])
					end
				end
			end
			
			if (#mobLeft > 0 and player.flank) then
				if (#mobLeftExtend > 0) then
					for i = 1, #mobLeftExtend do
						player:swingTarget(mobLeftExtend[i])
					end
				end
				
				if (#mobUpLeftExtend > 0) then
					for i = 1, #mobUpLeftExtend do
						player:swingTarget(mobUpLeftExtend[i])
					end
				end
				
				if (#mobDownLeftExtend > 0) then
					for i = 1, #mobDownLeftExtend do
						player:swingTarget(mobDownLeftExtend[i])
					end
				end
			end
			
			if (#mobRight > 0 and player.flank) then
				if (#mobRightExtend > 0) then
					for i = 1, #mobRightExtend do
						player:swingTarget(mobRightExtend[i])
					end
				end
				
				if (#mobUpRightExtend > 0) then
					for i = 1, #mobUpRightExtend do
						player:swingTarget(mobUpRightExtend[i])
					end
				end
				
				if (#mobDownRightExtend > 0) then
					for i = 1, #mobDownRightExtend do
						player:swingTarget(mobDownRightExtend[i])
					end
				end
			end
			
			if (#mobUp > 0 and player.backstab) then
				if (#mobUpExtend > 0) then
					for i = 1, #mobUpExtend do
						player:swingTarget(mobUpExtend[i])
					end
				end
				
				if (#mobUpLeftExtend > 0) then
					for i = 1, #mobUpLeftExtend do
						player:swingTarget(mobUpLeftExtend[i])
					end
				end
				
				if (#mobUpRightExtend > 0) then
					for i = 1, #mobUpRightExtend do
						player:swingTarget(mobUpRightExtend[i])
					end
				end
			end
			
			if (#pcDown > 0) then
				if (#pcDownExtend > 0) then
					for i = 1, #pcDownExtend do
						if (player:canPK(pcDownExtend[i])) then
							player:swingTarget(pcDownExtend[i])
						end
					end
				end
				
				if (#pcDownLeftExtend > 0) then
					for i = 1, #pcDownLeftExtend do
						if (player:canPK(pcDownLeftExtend[i])) then
							player:swingTarget(pcDownLeftExtend[i])
						end
					end
				end
				
				if (#pcDownRightExtend > 0) then
					for i = 1, #pcDownRightExtend do
						if (player:canPK(pcDownRightExtend[i])) then
							player:swingTarget(pcDownRightExtend[i])
						end
					end
				end
			end
			
			if (#pcLeft > 0 and player.flank) then
				if (#pcLeftExtend > 0) then
					for i = 1, #pcLeftExtend do
						if (player:canPK(pcLeftExtend[i])) then
							player:swingTarget(pcLeftExtend[i])
						end
					end
				end
				
				if (#pcUpLeftExtend > 0) then
					for i = 1, #pcUpLeftExtend do
						if (player:canPK(pcUpLeftExtend[i])) then
							player:swingTarget(pcUpLeftExtend[i])
						end
					end
				end
				
				if (#pcDownLeftExtend > 0) then
					for i = 1, #pcDownLeftExtend do
						if (player:canPK(pcDownLeftExtend[i])) then
							player:swingTarget(pcDownLeftExtend[i])
						end
					end
				end
			end
			
			if (#pcRight > 0 and player.flank) then
				if (#pcRightExtend > 0) then
					for i = 1, #pcRightExtend do
						if (player:canPK(pcRightExtend[i])) then
							player:swingTarget(pcRightExtend[i])
						end
					end
				end
				
				if (#pcUpRightExtend > 0) then
					for i = 1, #pcUpRightExtend do
						if (player:canPK(pcUpRightExtend[i])) then
							player:swingTarget(pcUpRightExtend[i])
						end
					end
				end
				
				if (#pcDownRightExtend > 0) then
					for i = 1, #pcDownRightExtend do
						if (player:canPK(pcDownRightExtend[i])) then
							player:swingTarget(pcDownRightExtend[i])
						end
					end
				end
			end
			
			if (#pcUp > 0 and player.backstab) then
				if (#pcUpExtend > 0) then
					for i = 1, #pcUpExtend do
						if (player:canPK(pcUpExtend[i])) then
							player:swingTarget(pcUpExtend[i])
						end
					end
				end
				
				if (#pcUpLeftExtend > 0) then
					for i = 1, #pcUpLeftExtend do
						if (player:canPK(pcUpLeftExtend[i])) then
							player:swingTarget(pcUpLeftExtend[i])
						end
					end
				end
				
				if (#pcUpRightExtend > 0) then
					for i = 1, #pcUpRightExtend do
						if (player:canPK(pcUpRightExtend[i])) then
							player:swingTarget(pcUpRightExtend[i])
						end
					end
				end
			end
		end
		
		if (#mobLeft > 0 and player.flank) then
			for i = 1, #mobLeft do
				player:swingTarget(mobLeft[i])
			end
		end
		
		if (#mobRight > 0 and player.flank) then
			for i = 1, #mobRight do
				player:swingTarget(mobRight[i])
			end
		end
		
		if (#mobUp > 0 and player.backstab) then
			for i = 1, #mobUp do
				player:swingTarget(mobUp[i])
			end
		end
		
		if (#pcLeft > 0 and player.flank) then
			for i = 1, #pcLeft do
				if (player:canPK(pcLeft[i])) then
					player:swingTarget(pcLeft[i])
				end
			end
		end
		
		if (#pcRight > 0 and player.flank) then
			for i = 1, #pcRight do
				if (player:canPK(pcRight[i])) then
					player:swingTarget(pcRight[i])
				end
			end
		end
		
		if (#pcUp > 0 and player.backstab) then
			for i = 1, #pcUp do
				if (player:canPK(pcUp[i])) then
					player:swingTarget(pcUp[i])
				end
			end
		end
		
		if (#mobDown > 0) then
			for i = 1, #mobDown do
				player:swingTarget(mobDown[i])
			end
		end
		
		if (#pcDown > 0) then
			for i = 1, #pcDown do
				if (player:canPK(pcDown[i])) then
					player:swingTarget(pcDown[i])
				end
			end
		end
	elseif (player.side == 3) then
		if (extendHit) then
			if (#mobLeft > 0) then
				if (#mobLeftExtend > 0) then
					for i = 1, #mobLeftExtend do
						player:swingTarget(mobLeftExtend[i])
					end
				end
				
				if (#mobUpLeftExtend > 0) then
					for i = 1, #mobUpLeftExtend do
						player:swingTarget(mobUpLeftExtend[i])
					end
				end
				
				if (#mobDownLeftExtend > 0) then
					for i = 1, #mobDownLeftExtend do
						player:swingTarget(mobDownLeftExtend[i])
					end
				end
			end
			
			if (#mobUp > 0 and player.flank) then
				if (#mobUpExtend > 0) then
					for i = 1, #mobUpExtend do
						player:swingTarget(mobUpExtend[i])
					end
				end
				
				if (#mobUpLeftExtend > 0) then
					for i = 1, #mobUpLeftExtend do
						player:swingTarget(mobUpLeftExtend[i])
					end
				end
				
				if (#mobUpRightExtend > 0) then
					for i = 1, #mobUpRightExtend do
						player:swingTarget(mobUpRightExtend[i])
					end
				end
			end
			
			if (#mobDown > 0 and player.flank) then
				if (#mobDownExtend > 0) then
					for i = 1, #mobDownExtend do
						player:swingTarget(mobDownExtend[i])
					end
				end
				
				if (#mobDownLeftExtend > 0) then
					for i = 1, #mobDownLeftExtend do
						player:swingTarget(mobDownLeftExtend[i])
					end
				end
				
				if (#mobDownRightExtend > 0) then
					for i = 1, #mobDownRightExtend do
						player:swingTarget(mobDownRightExtend[i])
					end
				end
			end
			
			if (#mobRight > 0 and player.backstab) then
				if (#mobRightExtend > 0) then
					for i = 1, #mobRightExtend do
						player:swingTarget(mobRightExtend[i])
					end
				end
				
				if (#mobUpRightExtend > 0) then
					for i = 1, #mobUpRightExtend do
						player:swingTarget(mobUpRightExtend[i])
					end
				end
				
				if (#mobDownRightExtend > 0) then
					for i = 1, #mobDownRightExtend do
						player:swingTarget(mobDownRightExtend[i])
					end
				end
			end
			
			if (#pcLeft > 0) then
				if (#pcLeftExtend > 0) then
					for i = 1, #pcLeftExtend do
						if (player:canPK(pcLeftExtend[i])) then
							player:swingTarget(pcLeftExtend[i])
						end
					end
				end
				
				if (#pcUpLeftExtend > 0) then
					for i = 1, #pcUpLeftExtend do
						if (player:canPK(pcUpLeftExtend[i])) then
							player:swingTarget(pcUpLeftExtend[i])
						end
					end
				end
				
				if (#pcDownLeftExtend > 0) then
					for i = 1, #pcDownLeftExtend do
						if (player:canPK(pcDownLeftExtend[i])) then
							player:swingTarget(pcDownLeftExtend[i])
						end
					end
				end
			end
			
			if (#pcUp > 0 and player.flank) then
				if (#pcUpExtend > 0) then
					for i = 1, #pcUpExtend do
						if (player:canPK(pcUpExtend[i])) then
							player:swingTarget(pcUpExtend[i])
						end
					end
				end
				
				if (#pcUpLeftExtend > 0) then
					for i = 1, #pcUpLeftExtend do
						if (player:canPK(pcUpLeftExtend[i])) then
							player:swingTarget(pcUpLeftExtend[i])
						end
					end
				end
				
				if (#pcUpRightExtend > 0) then
					for i = 1, #pcUpRightExtend do
						if (player:canPK(pcUpRightExtend[i])) then
							player:swingTarget(pcUpRightExtend[i])
						end
					end
				end
			end
			
			if (#pcDown > 0 and player.flank) then
				if (#pcDownExtend > 0) then
					for i = 1, #pcDownExtend do
						if (player:canPK(pcDownExtend[i])) then
							player:swingTarget(pcDownExtend[i])
						end
					end
				end
				
				if (#pcDownLeftExtend > 0) then
					for i = 1, #pcDownLeftExtend do
						if (player:canPK(pcDownLeftExtend[i])) then
							player:swingTarget(pcDownLeftExtend[i])
						end
					end
				end
				
				if (#pcDownRightExtend > 0) then
					for i = 1, #pcDownRightExtend do
						if (player:canPK(pcDownRightExtend[i])) then
							player:swingTarget(pcDownRightExtend[i])
						end
					end
				end
			end
			
			if (#pcRight > 0 and player.backstab) then
				if (#pcRightExtend > 0) then
					for i = 1, #pcRightExtend do
						if (player:canPK(pcRightExtend[i])) then
							player:swingTarget(pcRightExtend[i])
						end
					end
				end
				
				if (#pcUpRightExtend > 0) then
					for i = 1, #pcUpRightExtend do
						if (player:canPK(pcUpRightExtend[i])) then
							player:swingTarget(pcUpRightExtend[i])
						end
					end
				end
				
				if (#pcDownRightExtend > 0) then
					for i = 1, #pcDownRightExtend do
						if (player:canPK(pcDownRightExtend[i])) then
							player:swingTarget(pcDownRightExtend[i])
						end
					end
				end
			end
		end
		
		if (#mobUp > 0 and player.flank) then
			for i = 1, #mobUp do
				player:swingTarget(mobUp[i])
			end
		end
		
		if (#mobDown > 0 and player.flank) then
			for i = 1, #mobDown do
				player:swingTarget(mobDown[i])
			end
		end
		
		if (#mobRight > 0 and player.backstab) then
			for i = 1, #mobRight do
				player:swingTarget(mobRight[i])
			end
		end
		
		if (#pcUp > 0 and player.flank) then
			for i = 1, #pcUp do
				if (player:canPK(pcUp[i])) then
					player:swingTarget(pcUp[i])
				end
			end
		end
		
		if (#pcDown > 0 and player.flank) then
			for i = 1, #pcDown do
				if (player:canPK(pcDown[i])) then
					player:swingTarget(pcDown[i])
				end
			end
		end
		
		if (#pcRight > 0 and player.backstab) then
			for i = 1, #pcRight do
				if (player:canPK(pcRight[i])) then
					player:swingTarget(pcRight[i])
				end
			end
		end
		
		if (#mobLeft > 0) then
			for i = 1, #mobLeft do
				player:swingTarget(mobLeft[i])
			end
		end
		
		if (#pcLeft > 0) then
			for i = 1, #pcLeft do
				if (player:canPK(pcLeft[i])) then
					player:swingTarget(pcLeft[i])
				end
			end
		end
	end
end





swingDamage = function(block, target, printf)
	local baseDamage = 1 + (block.might * 4) + (block.grace * 3)
	local finalDamage
	
	if (printf ~= 1) then
		printf = 0
	end
	
	if (block.blType == BL_PC) then
		local weaponDamage = math.floor(mathRandom(block.minDam / 2, block.maxDam / 2, 1000) + mathRandom(block.minDam / 2, block.maxDam / 2, 1000))
		
		baseDamage = baseDamage * block.fury
		weaponDamage = weaponDamage * block.enchant
		finalDamage = (baseDamage + weaponDamage) * block.rage * block.invis
	elseif (block.blType == BL_MOB) then
		local weaponDamage = math.floor(mathRandom(block.minDam / 3, block.maxDam / 3, 1000) + mathRandom(block.minDam / 3, block.maxDam / 3, 1000) + mathRandom(block.minDam / 3, block.maxDam / 3, 1000))
		
		finalDamage = (baseDamage + weaponDamage) * block.invis
	end
		
	if (finalDamage < 1) then
		finalDamage = 1
	end
		
	if (target ~= nil and block.critChance > 0) then
		local deduction = 1 - ((target.armor * acPerArmor) / 100)
		local targetsAround = getTargetsAround(block, BL_MOB)
		
		block.target = target.ID
		
		if (block.blType == BL_MOB) then
			targetsAround = getTargetsAround(block, BL_PC)
		end
			
		if (target.blType == BL_MOB) then
			target:sendAction(25, 20)
		elseif (target.blType == BL_PC) then
			target:deductArmor(block.critChance)
		end
		
		if (deduction > .15) then
			finalDamage = finalDamage * deduction
		else
			finalDamage = finalDamage * .15
		end
		
		if (finalDamage < 1) then
			finalDamage = 1
		end
		
		if (target.target == 0 and target.blType == BL_MOB) then
			target.target = block.ID
			target.state = MOB_HIT
		end
		
		if (block.critChance == 2) then
			finalDamage = finalDamage * 1.75
		end
		
		finalDamage = finalDamage * target.sleep
		target.sleep = 1
		
		if (target.confused) then
			target.confused = false
			target.confuseTarget = 0
		end
		
		if (target.deduction > 0) then
			finalDamage = finalDamage * target.deduction
		else
			finalDamage = 0
		end
		
		if (target.dmgShield > 0) then
			if (target.dmgShield > finalDamage) then
				target.dmgShield = target.dmgShield - finalDamage
				finalDamage = 0
			else
				finalDamage = finalDamage - target.dmgShield
				target.dmgShield = 0
			end
		else
			finalDamage = finalDamage - target.dmgShield
			target.dmgShield = 0
		end
		
		if (finalDamage < 1) then
			if (target.dmgShield > 0) then
				finalDamage = 0
			else
				finalDamage = 1
			end
		end
		
		if ((block.blType ~= BL_MOB or (block.owner > 0 and block.owner < 1073741823)) and target.blType == BL_MOB) then
			if (block.blType == BL_PC and #block.group > 1) then
				target:setGrpDmg(block.ID, finalDamage)
			elseif (block.blType == BL_PC) then
				target:setIndDmg(block.ID, finalDamage)
			elseif (block.blType == BL_MOB and #Player(block.owner).group > 1) then
				target:setGrpDmg(block.owner, finalDamage)
			elseif (block.blType == BL_MOB) then
				target:setIndDmg(block.owner, finalDamage)
			end
		end
		
		if ((block.flank == true or block.backstab == true) and #targetsAround > 1) then
			local frontBlock = false
			local position = ""
			
			for i = 1, #targetsAround do
				if (targetsAround[i].ID == target.ID) then
					if (block.y - targetsAround[i].y > 0) then
						if (block.side == 0) then
							frontBlock = true
						end
						
						position = 0
					elseif (block.x - targetsAround[i].x < 0) then
						if (block.side == 1) then
							frontBlock = true
						end
						
						position = 1
					elseif (block.y - targetsAround[i].y < 0) then
						if (block.side == 2) then
							frontBlock = true
						end
						
						position = 2
					elseif (block.x - targetsAround[i].x > 0) then
						if (block.side == 3) then
							frontBlock = true
						end
						
						position = 3
					else
					end
				end
			end
			
			if (#targetsAround == 2) then
				if (block.side == 0) then
					if (block.side == position) then
						finalDamage = finalDamage * .6
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .4
					else
						finalDamage = finalDamage * .5
					end
				elseif (block.side == 1) then
					if (block.side == position) then
						finalDamage = finalDamage * .6
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .4
					else
						finalDamage = finalDamage * .5
					end
				elseif (block.side == 2) then
					if (block.side == position) then
						finalDamage = finalDamage * .6
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .4
					else
						finalDamage = finalDamage * .5
					end
				elseif (block.side == 3) then
					if (block.side == position) then
						finalDamage = finalDamage * .6
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .4
					else
						finalDamage = finalDamage * .5
					end
				end
			elseif (#targetsAround == 3) then
				if (block.side == 0) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .25
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .4
						else
							finalDamage = finalDamage * .3
						end
					end
				elseif (block.side == 1) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .25
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .4
						else
							finalDamage = finalDamage * .3
						end
					end
				elseif (block.side == 2) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .25
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .4
						else
							finalDamage = finalDamage * .3
						end
					end
				elseif (block.side == 3) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .25
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .4
						else
							finalDamage = finalDamage * .3
						end
					end
				end
			elseif (#targetsAround == 4) then
				if (block.side == 0) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .2
						else
							finalDamage = finalDamage * .15
						end
					end
				elseif (block.side == 1) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .2
						else
							finalDamage = finalDamage * .15
						end
					end
				elseif (block.side == 2) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .2
						else
							finalDamage = finalDamage * .15
						end
					end
				elseif (block.side == 3) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .2
						else
							finalDamage = finalDamage * .15
						end
					end
				end
			end
		end
		
		if (printf == 1) then
			return finalDamage
		end
		
		if (target.blType == BL_MOB
		or (block.blType == BL_MOB and (block.owner == 0 or block.owner >= 1073741823))
		or (block.blType == BL_PC and block:canPK(target) == true)
		or (block.blType == BL_MOB and block.owner > 0 and block.owner < 1073741823 and Player(block.owner):canPK(target) == true)) then
			target.attacker = block.ID
			block.damage = finalDamage
		end
	elseif (printf == 1) then
		return finalDamage
	end
end

magicDamage = function(block, target, func)
	local baseDamage = 1 + (block.will * 4) + (block.grace * 3)
	local finalDamage
	
	if (printf ~= 1) then
		printf = 0
	end
	
	if (block.blType == BL_PC) then
		local weaponDamage = math.floor(mathRandom(block.minDam / 3, block.maxDam / 3, 1000) + mathRandom(block.minDam / 3, block.maxDam / 3, 1000) + mathRandom(block.minDam / 3, block.maxDam / 3, 1000))
		
		baseDamage = baseDamage * block.fury
		weaponDamage = weaponDamage * block.enchant
		finalDamage = (baseDamage + weaponDamage) * block.rage * block.invis
	elseif (block.blType == BL_MOB) then
		local weaponDamage = math.floor(mathRandom(block.minDam / 3, block.maxDam / 3, 1000) + mathRandom(block.minDam / 3, block.maxDam / 3, 1000) + mathRandom(block.minDam / 3, block.maxDam / 3, 1000))
		
		finalDamage = (baseDamage + weaponDamage) * block.invis
	end

	--ADD FOR WISDOM?	
	
	if (finalDamage < 1) then
		finalDamage = 1
	end
		
	if (target ~= nil and block.critChance > 0) then
		local deduction = 1 - ((target.armor * acPerArmor) / 100)
		local targetsAround = getTargetsAround(block, BL_MOB)
		
		block.target = target.ID
		
		if (block.blType == BL_MOB) then
			targetsAround = getTargetsAround(block, BL_PC)
		end
			
		if (target.blType == BL_MOB) then
			target:sendAction(25, 20)
		elseif (target.blType == BL_PC) then
			target:deductArmor(block.critChance)
		end
		
		if (deduction > .15) then
			finalDamage = finalDamage * deduction
		else
			finalDamage = finalDamage * .15
		end
		
		if (finalDamage < 1) then
			finalDamage = 1
		end
		
		if (target.target == 0) then
			target.target = block.ID
			target.state = MOB_HIT
		end
		
		if (block.critChance == 2) then
			finalDamage = finalDamage * 1.75
		end
		
		finalDamage = finalDamage * target.sleep
		target.sleep = 1
		
		if (target.confused) then
			target.confused = false
			target.confuseTarget = 0
		end
		
		if (target.deduction > 0) then
			finalDamage = finalDamage * target.deduction
		else
			finalDamage = 0
		end
		
		if (target.dmgShield > 0) then
			if (target.dmgShield > finalDamage) then
				target.dmgShield = target.dmgShield - finalDamage
				finalDamage = 0
			else
				finalDamage = finalDamage - target.dmgShield
				target.dmgShield = 0
			end
		else
			finalDamage = finalDamage - target.dmgShield
			target.dmgShield = 0
		end
		
		if (finalDamage < 1) then
			if (target.dmgShield > 0) then
				finalDamage = 0
			else
				finalDamage = 1
			end
		end
		
		if ((block.blType ~= BL_MOB or (block.owner > 0 and block.owner < 1073741823)) and target.blType == BL_MOB) then
			if (block.blType == BL_PC and #block.group > 1) then
				target:setGrpDmg(block.ID, finalDamage)
			elseif (block.blType == BL_PC) then
				target:setIndDmg(block.ID, finalDamage)
			elseif (block.blType == BL_MOB and #Player(block.owner).group > 1) then
				target:setGrpDmg(block.owner, finalDamage)
			elseif (block.blType == BL_MOB) then
				target:setIndDmg(block.owner, finalDamage)
			end
		end
		
		if ((block.flank == true or block.backstab == true) and #targetsAround > 1) then
			local frontBlock = false
			local position = ""
			
			for i = 1, #targetsAround do
				if (targetsAround[i].ID == target.ID) then
					if (block.y - targetsAround[i].y > 0) then
						if (block.side == 0) then
							frontBlock = true
						end
						
						position = 0
					elseif (block.x - targetsAround[i].x < 0) then
						if (block.side == 1) then
							frontBlock = true
						end
						
						position = 1
					elseif (block.y - targetsAround[i].y < 0) then
						if (block.side == 2) then
							frontBlock = true
						end
						
						position = 2
					elseif (block.x - targetsAround[i].x > 0) then
						if (block.side == 3) then
							frontBlock = true
						end
						
						position = 3
					else
					end
				end
			end
			
			if (#targetsAround == 2) then
				if (block.side == 0) then
					if (block.side == position) then
						finalDamage = finalDamage * .6
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .4
					else
						finalDamage = finalDamage * .5
					end
				elseif (block.side == 1) then
					if (block.side == position) then
						finalDamage = finalDamage * .6
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .4
					else
						finalDamage = finalDamage * .5
					end
				elseif (block.side == 2) then
					if (block.side == position) then
						finalDamage = finalDamage * .6
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .4
					else
						finalDamage = finalDamage * .5
					end
				elseif (block.side == 3) then
					if (block.side == position) then
						finalDamage = finalDamage * .6
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .4
					else
						finalDamage = finalDamage * .5
					end
				end
			elseif (#targetsAround == 3) then
				if (block.side == 0) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .25
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .4
						else
							finalDamage = finalDamage * .3
						end
					end
				elseif (block.side == 1) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .25
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .4
						else
							finalDamage = finalDamage * .3
						end
					end
				elseif (block.side == 2) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .25
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .4
						else
							finalDamage = finalDamage * .3
						end
					end
				elseif (block.side == 3) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					elseif (frontBlock == true) then
						finalDamage = finalDamage * .25
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .4
						else
							finalDamage = finalDamage * .3
						end
					end
				end
			elseif (#targetsAround == 4) then
				if (block.side == 0) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .2
						else
							finalDamage = finalDamage * .15
						end
					end
				elseif (block.side == 1) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .2
						else
							finalDamage = finalDamage * .15
						end
					end
				elseif (block.side == 2) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .2
						else
							finalDamage = finalDamage * .15
						end
					end
				elseif (block.side == 3) then
					if (block.side == position) then
						finalDamage = finalDamage * .5
					else
						if (math.abs(block.side - position) == 2) then
							finalDamage = finalDamage * .2
						else
							finalDamage = finalDamage * .15
						end
					end
				end
			end
		end
		
		if (target.blType == BL_MOB
		or (block.blType == BL_MOB and (block.owner == 0 or block.owner >= 1073741823))
		or (block.blType == BL_PC and block:canPK(target) == true)
		or (block.blType == BL_MOB and block.owner > 0 and block.owner < 1073741823 and Player(block.owner):canPK(target) == true)) then
			target.attacker = block.ID
			
			if (func ~= nil) then
				return func(block, target, finalDamage)
			else
				return finalDamage
			end
		end
	else
		if (func ~= nil) then
			return func(block, target, finalDamage)
		else
			return finalDamage
		end
	end
end