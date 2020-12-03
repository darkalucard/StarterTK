r1_warrior_trainer = {
click = async(function(player, npc)
	local t = {graphic = 0, color = 0}
	local menuOptions = {}
	
	player.npcGraphic = t.graphic
	player.npcColor = t.color
	player.dialogType = 1
	
	if (player.class == 0) then
		table.insert(menuOptions, "About Warrior")
	end
	
	if (player.class == 0 or player.baseClass == 1) then
		table.insert(menuOptions, "Learn Spell")
	end
	
	table.insert(menuOptions, "Forget Spell")
	
	local choice = player:menuString("How may I assist you, young fledgling?", menuOptions)
	
	if (choice == "About Warrior") then
		player:dialogSeq({t, "Warriors are a path of great AoE damage and superior tankiness. They can charge into battle hitting every monster around them and when the going gets tough they can hunker down for defense and sustain great amounts of damage."}, 1)
		
		local spells = player:getSpells()
		local choice = player:menuString("Would you like to test Warrior spells?", {"Yes", "No"})
		
		if (choice == "Yes") then
			for x = 1, #spells do
				if (spells[x] ~= 1 and spells[x] ~= 2 and spells[x] ~= 4) then
					player:removeSpell(spells[x])
				end
			end
			
			if (player:canLearnSpell("defenders_stance"))  then
				player:addSpell("defenders_stance")
			end
			
			if (player:canLearnSpell("warriors_stance"))  then
				player:addSpell("warriors_stance")
			end
			
			if (player:canLearnSpell("rampage"))  then
				player:addSpell("rampage")
			end
			
			player:flushDuration(1)
			player:sendMinitext("You learn beginner warrior spells.")
			
			if (player.quest["warriorTraining"] == 0) then
				player:giveXP(250)
				player.quest["warriorTraining"] = 250
			end
		end
	elseif (choice == "Learn Spell") then
		player:learnSpell()
	elseif (choice == "Forget Spell") then
		player:forgetSpell()
	end
	
	player:calcStat()
end),

action = function(npc, player)

end
}