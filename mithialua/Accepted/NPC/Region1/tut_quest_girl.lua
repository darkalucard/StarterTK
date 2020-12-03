tut_quest_girl = {
click = async(function(player,npc)
	local t={graphic=convertGraphic(596,"monster"),color=27}
	local s={graphic=convertGraphic(596,"monster"),color=4}
	local m = {graphic = convertGraphic(25, "monster"), color = 9}
	local mr = {graphic = convertGraphic(25, "monster"), color = 5}
	local c={graphic=convertGraphic(2267,"item"),color=8}
	local f1 = {graphic = convertGraphic(838, "monster"), color = 0}
	--local i={graphic=convertGraphic(3696,"item"),color=0}
	local coco = Item(121)
	local qgirl = player.quest["tut_quest_girl"]
	
	player.npcGraphic=t.graphic
	player.npcColor=t.color
	player.dialogType = 0
	
	if (player.quest["tut_quest_girl"] == 0) then
		player:dialogSeq({t,"My sister got lost again and my mom always scolds me for it.",
		t,"Would you help me find her please?",
		s,"This is a recent picture of her.",
		t,"Please tell her to come back."},1)
		player.quest["tut_quest_girl"] = 1
		player.quest["tut_quest_noling"] = 1
	elseif (player.quest["tut_quest_girl"] == 1) then
		player:dialogSeq({t,"Still can't find her?",s,"This is what she looks like.",
		t,"She might be picking up random things from the sand again."},1)
	elseif (player.quest["tut_quest_girl"] == 2) then
		player:dialogSeq({t,"Thank you for finding her!",
		t,"She better get back soon or my mom will be mad."},1)
		player:addGold(50)
		player:giveXP(150)
		player.quest["tut_quest_girl"] = 3
	elseif (player.quest["tut_quest_girl"] == 3) then
		player:dialogSeq({t, "Argh, these pesky squirrels keep stealing my coconuts!",
		m, "Would you take care of them for me? 10 should do it. Oh, and bring me a Hard coconut?"}, 1)
		player.quest["tut_quest_girl"] = 4
		player:flushKills(2)
	elseif (player.quest["tut_quest_girl"] == 4) then
		if (player:killCount(2) >= 10 and player:hasItem(120, 1) == true) then
			player:dialogSeq({t, "Oh thank you so much! Momma will be proud of me.",
			t, "Wait... I think this might be more useful.\n-Ailana runs away briefly before returning-",
			c, "Momma says if you take it down to Noling he can make a hat out of it. You might look silly with a coconut on your head though.",
			"Don't lose it, Noling will need it to make you a coconut helmet!"}, 1)
			player:addGold(100)
			player:giveXP(350)
			player.quest["tut_quest_girl"] = 5
			player.quest["tut_quest_noling"] = 2
		elseif (player:killCount(2) < 10) then
			player:dialogSeq({m, "Have you slain all the squirrels?"}, 0)
		elseif (player:hasItem(120, 1) ~= true) then
			player:dialogSeq({c, "Have you found me a Hard coconut yet?"}, 0)
		else
			player:dialogSeq({t, "I think you're hacking, I'm telling momma!"}, 0)
		end
	elseif (player.quest["tut_quest_girl"] == 6) then
		player:dialogSeq({t, "If you have a moment to spare, I could use your help again..",
		mr, "I've seen an odd squirrel running around; though it seems quite rare. I wonder how his blood differs...",
		t, "I know! If you could bring me your weapon with his blood freshly on it, maybe I could do something with it.",
		f1, "Fresh blood? Could she perhaps mean that it is the last monster you've killed?"}, 1)
		player.quest["tut_quest_girl"] = 7
	elseif (player.quest["tut_quest_girl"] == 7) then
		if (player:killRank(11) == 1) then
			player:addLegend("Took time out for the children. "..curT(), "tut_squirrel", 74, 80)
			player:addGold(500)
			player:giveXP(1000)
			player.quest["tut_quest_girl"] = 8
		elseif (player:killCount(11) > 0 and player:killRank(11) > 1) then
			player:dialogSeq({t, "I'll need a bit more blood than that if I'm to do anything."}, 0)
		elseif (player:killCount(11) == 0) then
			player:dialogSeq({mr, "Still haven't found him? No worries.. Keep looking if you have the time."}, 0)
		else
			player:dialogSeq({t, "What are you doing here? Contact a GM."}, 0)
		end
	else
	--[[	if (not player:hasLegend("tut_squirrel")) then
			player:addLegend("Took time out for the children. "..curT(), "tut_squirrel", 74, 80)
		end
	]]--	
		player:dialogSeq({t, "Thank you for all the help!"}, 1)
	end
	
end),

action = function(npc)
	local people = {}
	local i, rand
	local t={graphic=convertGraphic(596,"monster"),color=27}
	local s={graphic=convertGraphic(596,"monster"),color=4}

	people = npc:getObjectsInArea(BL_PC)
	for i = 1, #people do
		local person = Player(people[i].ID)
		if (person.coRef == 0 and person.npc["tut_quest_girl_pop-up_timer"] + 60 <= os.time()) then
			person.npc["tut_quest_girl_pop-up_timer"] = os.time()
			tut_quest_girl.questpopup(person, npc, t, s)
		end
	end
	rand = math.random(240)
	if (rand == 38) then
		local str = {"Ailana: My sister is always getting lost, and when we find her she has a bunch of shells.",
		"Ailana: Where has my sister gone now?",
		"Ailana: Why does she always come back with sand and silly trinkets?",
		"Ailana: I bet she's scavenging in the sand again, if I get scolded again...",
		"Ailana: Where are my coconuts? Those pesky squirrels..."}
		local rand2 = math.random(#str)
		npc:talk(0,""..str[rand2])
	end
end,

questpopup = async(function(player, npc, t, s)
	player.lastClick = npc.ID
	if (distance(npc,player) <= 10) then
		if (player.quest["tut_quest_girl"] == 0) then
			player:dialogSeq({t,"Oh, hello there!",t,"I'm over here, could you do me a favor?"},1)
		elseif (player.quest["tut_quest_girl"] == 2) then
			player:dialogSeq({t,"Have you found my sister yet?",s,"This is what she looks like."},1)
		elseif (player.quest["tut_quest_girl"] == 4) then
			player:dialogSeq({t,"Argh, these pesky rodents!",
			t,"Would you do me another favor?"},1)
		elseif (player.quest["tut_quest_girl"] == 6) then
			player:dialogSeq({t,"If you have a moment...",
			t,"I have another task, if you're interested."}, 1)
		end
	end
end)
}