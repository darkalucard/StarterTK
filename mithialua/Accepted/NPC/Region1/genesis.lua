genesis = {
--[[click = async(function(player,npc)
	local t={graphic=convertGraphic(1,"item"),color=1}

	player.npcGraphic = 1
	player.npcColor = 1
	player:dialogSeq({t,"Greetings young one.","Have you met my children?","Mulghan and Krisalia are my firstborns."},1)

end)]]--
click = async(function(player,npc)
	local t={graphic=0,color=0}

	player.npcGraphic = 0
	player.npcColor = 0

	local opts = {}
	
	dialog="Welcome "..player.name..", how may I be of service?"
	table.insert(opts,"Who are you?")
	--[[table.insert(opts,"Recognize bug tester")
	table.insert(opts,"Where am I?")
	table.insert(opts,"Alter Stats")
	table.insert(opts,"Make a person GM")]]--
	menuOption = player:menuString(dialog,opts)
	if (menuOption == "Who are you?") then
	player:talk(0,"menuOption is: "..menuOption)
	
	end

	
end),





action = function(npc)
	local moved = true
	local oldside = npc.side
	local checkmove = math.random(0, 10)
	local dist = 0
	dist = dist + math.abs(npc.x - npc.startX)
	dist = dist + math.abs(npc.y - npc.startY)

	if(checkmove >= 4) then
		npc.side = math.random(0, 3)
		npc:sendSide()
	
		if(npc.side == oldside) then
			if (dist > 8) then
				npc:warp(npc.startM,npc.startX,npc.startY)
			else
				moved = npc:move()
				--[[local hg = math.random(567)
				if (hg < 5) then
					if (npc:checkOnline("Mulghan") == 1) then
						npc:msg(0,"Genesis(Dreamweaver)\" Son, why can't I pop up properly?",2)
					end
				end]]--
			end
		end
	else
		if (dist > 8) then
		npc:warp(npc.startM,npc.startX,npc.startY)
		else
		moved = npc:move()
			--[[if (hg < 5) then
				npc:msg(0,"Genesis(Dreamweaver)\" Son, why can't I pop up properly?",2)
			end]]--
		end
	end
end
}