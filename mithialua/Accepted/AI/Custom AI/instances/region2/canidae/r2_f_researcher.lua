r2_f_researcher = {
on_spawn = function(mob)
	mob.side = 1
	mob:sendSide()
end,

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
	choice = player:menuString("Would you like to take my research notes back?", options)
	
	if (choice == "Yes") then
		local mobs = player:getObjectsInSameMap(BL_MOB)
		local players = player:getObjectsInSameMap(BL_PC)
		
		if (mobs == nil or #mobs <= 1) then
			if (#players > 0) then
				for i = 1, #players do
					if (players[i].m == npc.m) then
						players[i]:giveXP(80123456)
						players[i]:warp(27, math.random(66, 68), math.random(70, 72))
					end
				end
				
				unloadInstance(npc.m - 5, 6)
				npc:delete()
			else--How would it even go in here?
				player:giveXP(80123456)
				player:warp(27, math.random(66, 68), math.random(70, 72))
				npc.registry["isInDialog"] = 0
			end
		else
			player:dialogSeq({"I don't see how you will even get it back with these monsters still here.",
			"We will never complete the research...",
			"I'll never be saved. -sobs-",
			"((You will only get a portion of the experience of completing the instance, because you only experienced part of it.))",
			"Good bye...\n\n\n<b>((PRESS NEXT ONLY IF YOU WANT TO LEAVE\n<b>WITHOUT FINISHING THE INSTANCE))"}, 1)
			
			if (#players > 0) then
				for i = 1, #players do
					if (players[i].m == npc.m) then
						players[i]:giveXP(16024691)
						players[i]:warp(27, math.random(66, 68), math.random(70, 72))
					end
				end
				
				unloadInstance(npc.m - 5, 6)
				npc:delete()
			else
				player:giveXP(16024691)
				player:warp(27, math.random(66, 68), math.random(70, 72))
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