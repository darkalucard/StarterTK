r1_mage_trainer = {
click = async(function(player, npc)
	local t = {graphic = 0, color = 0}
	local menuOptions = {}
	
	player.npcGraphic = t.graphic
	player.npcColor = t.color
	player.dialogType = 1
	
	if (player.class == 0) then
		table.insert(menuOptions, "About Mage")
	end
	
	if (player.class == 0 or player.baseClass == 3) then
		table.insert(menuOptions, "Learn Spell")
	end
	
	table.insert(menuOptions, "Forget Spell")
	
	local choice = player:menuString("How may I assist you, young fledgling?", menuOptions)
	
	if (choice == "About Mage") then
		player:dialogSeq({t, "Mages are a path of extremely high single target damage and crowd control. Their ability to manipulate the enemies around them is unrivaled and their burst unmatched."}, 1)
		
		local spells = player:getSpells()
		local choice = player:menuString("Would you like to test Mage spells?", {"Yes", "No"})
		
		if (choice == "Yes") then
			for x = 1, #spells do
				if (spells[x] ~= 1 and spells[x] ~= 2 and spells[x] ~= 4) then
					player:removeSpell(spells[x])
				end
			end
			
			if (player:canLearnSpell("inhibit"))  then
				player:addSpell("inhibit")
			end
			
			if (player:canLearnSpell("singe"))  then
				player:addSpell("singe")
			end
			
			if (player:canLearnSpell("stop_time"))  then
				player:addSpell("stop_time")
			end
			
			player:flushDuration(1)
			player:sendMinitext("You learn beginner mage spells.")
			
			if (player.quest["mageTraining"] == 0) then
				player:giveXP(250)
				player.quest["mageTraining"] = 250
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