tutorial_spirit = {
check = async(function(player)
	local npcG = {graphic = convertGraphic(1035, "monster"), color = 0}
	local options = {"Wake up (Tutorial)", "Continue dreaming (Skip Tutorial)"}
	
	player.npcGraphic = npcG.graphic
	player.npcColor = npcG.color
	player.lastClick = F1_NPC
	player.dialogType = 0
	
	local menu = player:menu("You are still sleepy, what would you like to do?", options)
	
	if (menu == 1) then --Tutorial
		player:dialogSeq({npcG, "Wake up! You have been asleep for quite some time my little friend. I have big plans for you, your journey will be full of adventure and excitment. First off we have to get you used to your surroundings in the world of Mithia.",
		npcG, "((You can move around with the 'Up', 'Down', 'Left', and 'Right' keys, also, if you use the combination 'Shift'+'F12' you can turn on click to move so you can move around with your mouse.))",
		npcG, "Now go to the hut and talk to the lady who will lead you further on your most grand and epic journey.",
		npcG, "((If you look in the bottom right of the interface you will notice your coordinates, marked with X and Y. The hut can be found around X5, Y27.))", 1})
	elseif (menu == 2) then --Skip
		if (player.sex == 0) then
			player:addItem("novice_shirt", 1)
		else
			player:addItem("novice_blouse", 1)
		end
		
		player:addSpell("soothe")
		player:addItem("wooden_sword", 1)
		player.blind = true
		player.registry["opening_warp"] = os.time() + 2
	end
end)
}

tutorial_hut = {
click = async(function(player, npc)
	local npcG = {graphic = convertGraphic(3, "monster"), color = 3}
	local armor = {graphic = convertGraphic(119, "item"), color = 0}
	local sheep = {graphic = convertGraphic(173, "monster"), color = 8}
	local rabbit = {graphic = convertGraphic(21, "monster"), color = 21}
	local woolAmount = player:hasItem("wool", 999999)
	local shears = player:getEquippedItem(EQ_WEAP)
	
	player.npcGraphic = npcG.graphic
	player.npcColor = npcG.color
	player.dialogType = 0
	
	if (player.sex == 1) then
		armor.graphic = 179
	end
	
	if (player.quest["tutorial"] == 0) then
		player:dialogSeq({npcG, "Huh? Oh it's been quite a while since I've seen a new face around here.",
		armor, "What's that you say? You need some clothes? You do look a bit chilly, my darling!",
		sheep, "Very well I will make you some clothes, but I need some work done around here first. If you could trim up my sheep for me I would be very grateful. Take these shears and just clip off 5 wool for me. My sheep are very tame, so don't worry!",
		npcG, "((Go to your inventory with the 'i' button and then press 'u' followed by whatever letter the shears are in or double click on them with the mouse. Then, walk up to the sheep near 15, 39 and press 'space' to attack them.))"}, 1)
		player:addItem("shears", 1)
		player.quest["tutorial"] = 1
	elseif (player.quest["tutorial"] == 1) then
		if (player:hasItem("wool", 5) == true) then
			player:dialogSeq({npcG, "Great my chores for the day are done! I can now make you some clothes.",
			rabbit, "First you will need to go and hunt down some rabbits and get 3 rabbit pelts.",
			npcG, "((Once you kill a rabbit they may drop a rabbit pelt, pick it up with the comma key (','). If there is more than one item in the same space use the less-than key ('Shift'+',') and you will pick up everything in that space, unless your inventory is full.))"}, 1)
			
			if (shears ~= nil and shears.id == 1000000000) then
				player:deductDura(EQ_WEAP, shears.dura)
			else
				player:removeItem("shears", 1)
			end
			
			player:removeItem("wool", woolAmount)
			player.quest["tutorial"] = 2
		else
			player:dialogSeq({sheep, "I'm gonna need those 5 wools before I can make you any clothes. You can find the sheep in their pen. ((15, 39))"}, 1)
		end
	elseif (player.quest["tutorial"] == 2) then
		if (player:hasItem("rabbit_pelt", 3) == true) then
			player:dialogSeq({npcG, "Great, you have the pelts! Please give me just a moment to fashion you some clothes.",
			npcG, "-she stitches with precision and speed and before you know it she is done-",
			armor, "Here you go I do hope you like them m'dear!",
			npcG, "Now follow the road, there is a man that sits under the great tree that may help you find a suitable weapon."}, 1)
			
			if (player.sex == 0) then
				player:addItem("novice_shirt", 1)
			else
				player:addItem("novice_blouse", 1)
			end
			
			player:removeItem("rabbit_pelt", 3)
			player:giveXP(100)
			player.quest["tutorial"] = 3
		else
			player:dialogSeq({rabbit, "I'm gonna need those 3 rabbit pelts before I can make you any clothes."}, 1)
		end
	elseif (player.quest["tutorial"] == 3) then
		player:dialogSeq({npcG, "Now follow the road, there is a man that sits under the great tree that may help you find a suitable weapon."}, 1)
	end
end)
}

tutorial_wood = {
click = async(function(player, npc)
	local npcG = {graphic = convertGraphic(2, "monster"), color = 0}
	local sword = {graphic = convertGraphic(2, "item"), color = 4}
	local tree = {graphic = convertGraphic(761, "monster"), color = 0}
	local vein = {graphic = convertGraphic(853, "monster"), color = 4}
	local troll = {graphic = convertGraphic(525, "monster"), color = 14}
	local woodAmount = player:hasItem("ash_log", 999999)
	local oreAmount = player:hasItem("iron_ore", 999999)
	local axePick = player:getEquippedItem(EQ_WEAP)
	
	player.npcGraphic = npcG.graphic
	player.npcColor = npcG.color
	player.dialogType = 0

	if (player.state == 1) then
		player.state = 0
		player.health = player.maxHealth
		player.magic = player.maxMagic
		player:updateState()
		player:sendStatus()
		npc:talk(0, "Wood: Good as new.")
		return
	end
	
	if (player.quest["tutorial"] == 3) then
		player:dialogSeq({npcG, "Huh? What? Time to cut more wood!",
		sword, "Oh what's this? You need some work? Here take my axe and go cut 10 wood for me. I will make you a wooden sword for your troubles.",
		tree, "The trees are just to the northwest from here. ((34, 7))"}, 1)
		player:addItem("logging_axe", 1)
		player.quest["tutorial"] = 4
	elseif (player.quest["tutorial"] == 4) then
		if (player:hasItem("ash_log", 10) == true) then
			player:dialogSeq({npcG, "Great job! Err... I've seemingly misplaced the ore I use to whittle and sharpen the wood.",
			vein, "There is a cave to the northeast of here ((59, 17)) that holds the ore I need. You will need this mining pick to dig the ore out of the stones. Bring me 5 of them to choose from, this is precision work afterall.",
			troll, "Take this minor healing spell to protect yourself. I hear some strange noises coming from the cave lately and I believe some monsters dwell there now.",
			npcG, "Be careful. Get the ore and come back to me. We don't need anything to happen to you!"}, 1)
			
			if (axePick ~= nil and axePick.id == 1000000100) then
				player:deductDura(EQ_WEAP, axePick.dura)
			else
				player:removeItem("logging_axe", 1)
			end
			
			player:removeItem("ash_log", woodAmount)
			player:addItem("mining_pick", 1)
			player:sendMinitext("You learned soothe.")
			player:addSpell("soothe")
			player.quest["tutorial"] = 5
		else
			player:dialogSeq({tree, "I'm gonna need those 10 logs before I can make you a weapon. Find the grove northwest from here. ((34, 7))"}, 1)
		end
	elseif (player.quest["tutorial"] == 5) then
		if (player:hasItem("iron_ore", 5) == true) then
			player:dialogSeq({npcG, "You made it out alive, and with the ore I see! Give me just a little bit to whittle this wood.",
			npcG, "-he carefully looks at all the ore before finally choosing one and throwing the rest behind him- Ah, this one will do nicely.",
			sword, "-he whittles away at the wood until a crudely sharp sword is presented to you-",
			npcG, "Thank you for all your help! Now, I believe there is a man on a raft in the water to the south east that can help you continue your journey."}, 1)
			
			if (axePick ~= nil and axePick.id == 1000000400) then
				player:deductDura(EQ_WEAP, axePick.dura)
			else
				player:removeItem("mining_pick", 1)
			end
			
			player:removeItem("iron_ore", oreAmount)
			player:addItem("wooden_sword", 1)
			player:giveXP(100)
			player.quest["tutorial"] = 6
		else
			player:dialogSeq({vein, "I'm gonna need those 5 ores before I can make you a weapon. Find the cave northeast of here. ((59, 17))"}, 1)
		end
	elseif (player.quest["tutorial"] == 6) then
		player:dialogSeq({npcG, "Thank you for all your help! Now, I believe there is a man on a raft in the water to the south east that can help you continue your journey."}, 1)
	end
end),

action = function(npc)
	local playersInMap = npc:getObjectsInSameMap(BL_PC)
	
	if (#playersInMap > 0) then
		for i = 1, #playersInMap do
			if (playersInMap[i].state == 1) then
				playersInMap[i]:msg(4, "Wood! Dead? Come visit me under the great tree.", playersInMap[i].ID)
			end
		end
	end
	
	npc:talk(2, "Wood snores loudly under the tree.")
end
}

tutorial_raft = {
click = async(function(player,npc)
	local npcG = {graphic = convertGraphic(4, "monster"), color = 21}

	player.npcGraphic = npcG.graphic
	player.npcColor = npcG.color
	player.dialogType = 0
	
	if (player.quest["tutorial"] == 6) then-- >= 6) then
		player:dialogSeq({npcG, "Oh, ye finally found me eh?",
		npcG, "So ye wish to get across? The world beyond be very dangerous but rewarding. I see yer ready, ye have a weapon and some clothes. Hop on, I will take ye across.",
		npcG, "((Click Next to leave the tutorial and proceed.))"}, 1)
		
		local item = player:getEquippedItem(EQ_WEAP)
		
		if (item ~= nil and item.id ~= 100000000) then
			player:deductDura(EQ_WEAP, item.dura)
		end
		
		for i = 1, player.maxInv do
			item = player:getInventoryItem(i - 1)
			
			if (item ~= nil and item.id ~= 100000000) then
				player:removeItemSlot(i - 1, item.maxAmount)
			end
		end
		--TEMP
		--[[if (player.quest["tutorial"] >= 6) then
			player.clan = 1
			player.registry["login_count"] = 1
			player:warp(1, 9, 17)
			player.quest["tutorial"] = 7
		end]]--
		--END TEMP
		player:giveXP(100)

		player.blind = true
		player.registry["opening_warp"] = os.time() + 2
		player.quest["tutorial"] = 7
	elseif (player.quest["tutorial"] == 7) then
		player.blind = true
		player.registry["opening_warp"] = os.time() + 2
	else
		player:dialogSeq({npcG, "Make sure ye help everyone out around here, then I'll help ye out."}, 1)
	end
end),

action = function(npc, player)
	local playersInMap = npc:getObjectsInSameMap(BL_PC)
	
	if (#playersInMap > 0) then
		for i = 1, #playersInMap do
			if (playersInMap[i].quest["tutorial"] == 6) then
				playersInMap[i]:msg(4, "Raft! I'm over here, on the raft. ((Use 'Ctrl'+'Right' and 'Ctrl'+'Down' to find me.))", playersInMap[i].ID)
			end
		end
	end
end
}