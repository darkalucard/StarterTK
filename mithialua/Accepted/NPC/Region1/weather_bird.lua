weather_bird = {

click = async(function(player,npc)
	local t={graphic=convertGraphic(1010,"monster"),color=0}
	local bw={graphic=convertGraphic(604,"monster"),color=4}
	local item = {}
	local menuOption
	
	player.npcGraphic = t.graphic
	player.npcColor = t.color
	player.dialogType = 0

	if (player.quest["tut_quest_bird_worshipper"] == 0)then
		player:dialogSeq({bw, "Please, seek out my worshippers. They'll know what to do.",
		t, "I must rescue my baby..."}, 1)
		return
	end
	
	if (player.quest["tut_quest_bird"] == 0) then
		if (player:killCount(9) >= 1 and player:killCount(10) >= 1) then
			if (player:hasSpace(171, 5)) then
				player:dialogSeq({t, "Thank you so much. Now I will be able to save my baby.",
				bw, "Go talk to my followers for a reward. I've got no pockets; as you can see I'm just a bird."}, 1)
				player.quest["tut_quest_bird"] = 1
				player:addLegend("Aided the weather bird. "..curT(), "tut_quest_bird", 22, 16)
				player:addItem(171, 5)
				player.quest["tut_quest_bird_worshipper"] = 2
			else
				player:dialog("Make some room in your inventory first.", t)
			end
		else
			player:dialogSeq({t,"I must rescue my baby.",
			t,"Please, kill the worms that block my path."},1)
		end
	else
		player:dialog("I must rescue my baby...",t)
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
			end
		end
	else
		if (dist > 8) then
			npc:warp(npc.startM,npc.startX,npc.startY)
		else
			moved = npc:move()
		end
	end
end
}