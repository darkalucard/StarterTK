nogo = {
click = async(function(player, npc)
	local npcGraphics = {graphic = convertGraphic(635, "monster"), color = 0}
	
	player.npcGraphic = npcGraphics.graphic
	player.npcColor = npcGraphics.color
	player.dialogType = 1
	player.lastClick = npc.ID
	
	if (player.quest["nogoTraining"] == 0) then
		player:dialogSeq({npcGraphic, "Hey! Come over here! I bet you're wondering how you get stronger in this world. Let me explain to you how it works.",
			"Each time you gain experience it gets put into that bar in the bottom right, think of it like a bank.",
			"You can spend that experience to buy more vita (hp) or mana (mp), the more of those you get, the stronger you become.",
			"As you sell more of your experience into stats you'll be able to enter new areas, obtain new spells, and wear new items.",
			"Let's try it! Here, take some experience from me and then talk to me when you're ready."}, 1)
		player:giveXP(100)
		player.quest["nogoTraining"] = 1
	elseif (player.quest["nogoTraining"] == 1) then
		local menuChoice = player:menu("Alright! Select \"I'd like to train a small amount.\" and see what happens.", {"I'd like to train a small amount."})
		
		if (menuChoice == 1) then
			player.baseHealth = player.baseHealth + 50
			player.exp = player.exp - 100
			player:calcStat()
			player:sendStatus()
			player.quest["nogoTraining"] = 2
		end
	elseif (player.quest["nogoTraining"] == 2) then
		player:dialogSeq({npcGraphic, "Great! You got a little bit stronger. Now let's try it with mana."}, 1)
		player:giveXP(100)
		player.quest["nogoTraining"] = 3
	elseif (player.quest["nogoTraining"] == 3) then
		local menuChoice = player:menu("Now do the same thing you did before but this time let's get mana.", {"I'd like to train a small amount."})
		
		if (menuChoice == 1) then
			player.baseMagic = player.baseMagic + 50
			player.exp = player.exp - 100
			player:calcStat()
			player:sendStatus()
			player.quest["nogoTraining"] = 4
		end
	elseif (player.quest["nogoTraining"] == 4) then
		player:dialogSeq({npcGraphic, "Now you've got it! You can do that anytime you get experience all over the world from now on.",
			"As you get stronger it takes more experience to get vita and mana, so keep that in mind. Good luck champ!"}, 1)
		player:giveXP(1000)
		player.quest["nogoTraining"] = 5
	elseif (player.quest["nogoTraining"] == 5) then
		player:sellExperience(npc)
	end
end),

action = function(npc)
	local peopleinArea = {}
	local i
	local npcGraphic = {graphic = convertGraphic(635, "monster"), color = 0}

	peopleinArea = npc:getObjectsInArea(BL_PC)
	
	for i = 1, #peopleinArea do
		local person = Player(peopleinArea[i].ID)
		
		if (person.coRef == 0 and person.npc["nogoTrainingPopUpTimer"] + 60 <= os.time()) then
			person.npc["nogoTrainingPopUpTimer"] = os.time()
			nogo.questPopUp(person, npc, npcGraphic)
		end
	end
end,

questPopUp = async(function(player, npc, npcGraphic)
	player.lastClick = npc.ID
	
	if (distance(npc,player) <= 10) then
		if (player.quest["nogoTraining"] == 0) then
			player:dialogSeq({npcGraphic, "Come over here!"}, 1)
		end
	end
end)
}