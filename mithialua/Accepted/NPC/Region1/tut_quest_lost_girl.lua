tut_quest_lost_girl = {
click = async(function(player,npc)
	local s={graphic=convertGraphic(596,"monster"),color=27}
	local t={graphic=convertGraphic(596,"monster"),color=4}
	local i={graphic=convertGraphic(428,"item"),color=31}
	--[[
	local c={graphic=convertGraphic(2267,"item"),color=28}
	local w={graphic=convertGraphic(2,"item"),color=4}
	local i={graphic=convertGraphic(3696,"item"),color=0}]]--
	local coco = Item(121)
	
	player.npcGraphic=t.graphic
	player.npcColor=t.color
	player.dialogType = 0
	
	if (player.quest["tut_quest_girl"] == 1) then
		player:dialogSeq({t,"Oh, so she says I need to go back?",
		t,"I will grab a few more seashells first and then head back."},1)
		player.quest["tut_quest_girl"] = 2
		player.quest["tut_quest_lost_girl"] = 1
	else--Not in quest or quest completed.
		if (player.quest["tut_quest_lost_timer"] <= os.time()) then
			if (math.random(1123123) == 1) then
				player:addItem(171,1)
				player.quest["tut_quest_lost_timer"] = os.time() + 10800
				player:dialog("I found this shiny thing, would you like to have it?",i)
			else
				player:dialog("These seashells are so pretty!",t)
			end
		else
			player:dialog("These seashells are so pretty!",t)
		end
	end
	
end)
}