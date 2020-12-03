r2_r_juh = {
click = async(function(player, npc)
	local options = {"Yes", "No"}
	local choice = ""
	
	if (npc.registry["isInDialog"] > 0) then
		return
	end
	
	npc:talk(2, "**Talks to "..player.name.."**")	
	
	player.npcGraphic = 0
	player.npcColor = 0
	player.dialogType = 0
	npc.registry["isInDialog"] = os.time() + 60
	choice = player:menuString("Would you like to leave this foul place?", options)
	
	if (choice == "Yes") then
		local mobs = player:getObjectsInSameMap(BL_MOB)
		local players = player:getObjectsInSameMap(BL_PC)
		
		if (mobs == nil or #mobs <= 2) then
			if (#players > 0) then
				for i = 1, #players do
					if (players[i].m == npc.m) then
						players[i]:giveXP(500000)
						players[i]:warp(30, math.random(58, 61), math.random(10, 12))
					end
				end
				
				unloadInstance(npc.m - 4, 5)
				npc:delete()
			else
				player:giveXP(500000)
				player:warp(30, math.random(58, 61), math.random(10, 12))
				npc.registry["isInDialog"] = 0
			end
		else
			player:dialogSeq({"I'll never be saved. -sobs-"}, 1)
			
			if (#players > 0) then
				for i = 1, #players do
					if (players[i].m == npc.m) then
						players[i]:warp(30, math.random(58, 61), math.random(10, 12))
					end
				end
				
				unloadInstance(npc.m - 4, 5)
				npc:delete()
			else
				player:warp(30, math.random(58, 61), math.random(10, 12))
				npc.registry["isInDialog"] = 0
			end
		end
	else
		npc.registry["isInDialog"] = 0
	end
end),

move = function(npc)
	if (npc.registry["isInDialog"] < os.time()) then
		npc.registry["isInDialog"] = 0
	end
end
}