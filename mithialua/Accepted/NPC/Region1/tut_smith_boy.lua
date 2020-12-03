tut_smith_boy = {
click = async(function(player,npc)
	local t={graphic=convertGraphic(597,"monster"),color=4}
	local s={graphic=convertGraphic(25,"monster"),color=9}
	local c={graphic=convertGraphic(2267,"item"),color=28}
	local w={graphic=convertGraphic(2,"item"),color=4}
	local i={graphic=convertGraphic(3696,"item"),color=0}
	local f1={graphic=convertGraphic(838,"monster"),color=0}
	local opts = {"Yes", "No"}
	local coco = Item(121)
	local qbrat = player.quest["quest_brat"]
	
	player.npcGraphic=t.graphic
	player.npcColor=t.color
	player.dialogType = 0
	
	if (player.level == 1 or qbrat == 0) then
		player.quest["quest_brat"] = 1
		player:dialog("I am sorry, but my dad is out. Please come back later.",t)
	elseif (qbrat == 1) then
		player:dialogSeq({t, "I am sorry, but my dad is out. Would you get me something?",
			"Well, since I'm stuck here I can't go out and get anything myself.",
			"How about you get me something to snack on?",
			s, "I'm sure you've met the squirrels of the island.",
			c, "They tend to carry delicious snacks, bring me some of the green ones.",
			w, "I'll give you one of the sample weapons in exchange.",
			t, "Bring me 2 and I should be satisfied..."}, 1)
		
		local menuOption = player:menuString("Will you bring me 2 "..coco.name.."?",opts)
		
		if (menuOption == "Yes") then
			player.quest["quest_brat"] = 2
			player:dialog("Please hurry, I'm starving.", t)
		elseif (menuOption == "No") then
			player.quest["quest_brat"] = 0
			player:dialog("Why not? If it was you I would do it for you.", t)
		end
	elseif (qbrat == 2) then --Check for items PART1
		if (player:hasItem(121, 2) == true) then
			player.quest["quest_brat"] = 3
			player:removeItem(121, 2)
			player:giveXP(100)
			NPC(4):talk(0, "Sander: I hope you're not giving away your father's merchandise!")
			player:dialog("Argh, talk to you in a bit...", t)
		else
			player:dialog("Please hurry and bring me 2 "..coco.name.."s, I'm starving!",t)
		end
	elseif (qbrat == 3) then
		player:dialogSeq({t, "Argh, Sander caught us... I won't be able to give you a sample...",
			i, "((My ideas are shaped by stars!))\n\nMaybe if you get me one more "..coco.name.."?",
			t, "I promise I will get you a sample weapon."}, 1)
		
		local menuOption = player:menuString("What do you say? One more "..coco.name.." and I promise to convince Sander.",opts)
		
		if (menuOption == "Yes") then
			player.quest["quest_brat"] = 4
			player:dialog("Please hurry or Sander will tell my dad!.",t)
		end
	elseif (qbrat == 4) then
		if (player:hasItem(121, 1) == true) then
			player:dialogSeq({t, "It's too late... My dad already knows, so now I can't give it out."}, 1)
			player:removeItem(121, 1)
			player:giveXP(200)
			player.quest["quest_brat"] = 5
			player:addLegend("Tricked by Todd "..curT(), "quest_brat", 74, 16)
			npc:sendParcel(player.ID, npc.id, 100000000, 1, player.ID, player.name.."'s first sword", 1)
		else
			player:dialog("Please hurry or Sander will tell my dad!.", t)
		end
	else
		player:dialog("My dad hasn't come back, try later.", t)
	end
end)
}