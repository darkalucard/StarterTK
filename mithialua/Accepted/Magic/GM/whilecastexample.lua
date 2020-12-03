whilecastexample = {
cast=function(player,target)
		target:setDuration("whilecastexample",5000,player.ID)
end,
	
while_cast=function(player,target)
		if(target.ID == player.ID) then
			player:talk(0,"Cast this on the one you like.")
		else
			player:talk(0,"I love you.")
			target:talk(0,"I love you too.")
		end	

--[[	if (player.name == "Mulghan" or player.name == "Krisalia") then
		if (player.name == "Mulghan") then
			local pp = Player("Krisalia")
			if (player.side == 0) then
				g = player:getObjectsInCell(player.m, player.x, player.y - 1, BL_PC)
				if (g[1].name == "Krisalia") then
					pp.side = 2
					pp:warp(pp.m,pp.x,pp.y)
					player:sendAction(24, 255)
					pp:sendAction(24, 255)
					pp:talk(0,"Krisalia: I love you too")
				end
			elseif (player.side == 1) then
				g = player:getObjectsInCell(player.m, player.x + 1, player.y, BL_PC)
				if (g[1].name == "Krisalia") then
					pp.side = 3
					pp:warp(pp.m,pp.x,pp.y)
					player:sendAction(24, 255)
					pp:sendAction(24, 255)
					pp:talk(0,"Krisalia: I love you too")
				end
			elseif (player.side == 2) then
				g = player:getObjectsInCell(player.m, player.x, player.y + 1, BL_PC)
				if (g[1].name == "Krisalia") then
					pp.side = 0
					pp:warp(pp.m,pp.x,pp.y)
					player:sendAction(24, 255)
					pp:sendAction(24, 255)
					pp:talk(0,"Krisalia: I love you too")
				end
			elseif (player.side == 3) then
				g = player:getObjectsInCell(player.m, player.x - 1, player.y, BL_PC)
				if (g[1].name == "Krisalia") then
					pp.side = 1
					pp:warp(pp.m,pp.x,pp.y)
					player:sendAction(24, 255)
					pp:sendAction(24, 255)
					pp:talk(0,"Krisalia: I love you too")
				end
			end
		elseif (player.name == "Krisalia") then
			local pp = Player("Mulghan")
			if (player.side == 0) then
				g = player:getObjectsInCell(player.m, player.x, player.y - 1, BL_PC)
				if (g[1].name == "Mulghan") then
					pp.side = 2
					pp:warp(pp.m,pp.x,pp.y)
					player:sendAction(24, 255)
					pp:sendAction(24, 255)
					pp:talk(0,"Mulghan: I love you too")
				end
			elseif (player.side == 1) then
				g = player:getObjectsInCell(player.m, player.x + 1, player.y, BL_PC)
				if (g[1].name == "Mulghan") then
					pp.side = 3
					pp:warp(pp.m,pp.x,pp.y)
					player:sendAction(24, 255)
					pp:sendAction(24, 255)
					pp:talk(0,"Mulghan: I love you too")
				end
			elseif (player.side == 2) then
				g = player:getObjectsInCell(player.m, player.x, player.y + 1, BL_PC)
				if (g[1].name == "Mulghan") then
					pp.side = 0
					pp:warp(pp.m,pp.x,pp.y)
					player:sendAction(24, 255)
					pp:sendAction(24, 255)
					pp:talk(0,"Mulghan: I love you too")
				end
			elseif (player.side == 3) then
				g = player:getObjectsInCell(player.m, player.x - 1, player.y, BL_PC)
				if (g[1].name == "Mulghan") then
					pp.side = 1
					pp:warp(pp.m,pp.x,pp.y)
					player:sendAction(24, 255)
					pp:sendAction(24, 255)
					pp:talk(0,"Mulghan: I love you too")
				end
			end
		end
	else
	end]]--
end
}