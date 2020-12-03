r1_rogue_trainer = {
click = async(function(player, npc)
	local t = {graphic = 0, color = 0}
	local menuOptions = {}
	
	player.npcGraphic = t.graphic
	player.npcColor = t.color
	player.dialogType = 1
	
	if (player.class == 0) then
		table.insert(menuOptions, "About Rogue")
	end
	
	if (player.class == 0 or player.baseClass == 2) then
		table.insert(menuOptions, "Learn Spell")
	end
	
	table.insert(menuOptions, "Forget Spell")
	
	local choice = player:menuString("How may I assist you, young fledgling?", menuOptions)
	
	if (choice == "About Rogue") then
		player:dialogSeq({t, "Rogues are a path of great speed, cunning, and deceit. Through their traps, tricks, and invisibility they are able to do high amounts of single target damage and get out of danger in a flash."}, 1)
		
		local spells = player:getSpells()
		local choice = player:menuString("Would you like to test Rogue spells?", {"Yes", "No"})
		
		if (choice == "Yes") then
			for x = 1, #spells do
				if (spells[x] ~= 1 and spells[x] ~= 2 and spells[x] ~= 4) then
					player:removeSpell(spells[x])
				end
			end
			
			if (player:canLearnSpell("cub_trap"))  then
				player:addSpell("cub_trap")
			end
			
			if (player:canLearnSpell("sand_trap"))  then
				player:addSpell("sand_trap")
			end
			
			if (player:canLearnSpell("leech_trap"))  then
				player:addSpell("leech_trap")
			end
			
			if (player:canLearnSpell("hilt_strike"))  then
				player:addSpell("hilt_strike")
			end
			
			if (player:canLearnSpell("file")) then
				player:addSpell("file")
			end
			
			player:flushDuration(1)
			player:sendMinitext("You learn beginner rogue spells.")
			
			if (player.quest["rogueTraining"] == 0) then
				player:giveXP(250)
				player.quest["rogueTraining"] = 250
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