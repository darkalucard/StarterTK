r1_poet_trainer = {
click = async(function(player, npc)
	local t = {graphic = 0, color = 0}
	local menuOptions = {}
	
	player.npcGraphic = t.graphic
	player.npcColor = t.color
	player.dialogType = 1
	
	if (player.class == 0) then
		table.insert(menuOptions, "About Poet")
	end
	
	if (player.class == 0 or player.baseClass == 4) then
		table.insert(menuOptions, "Learn Spell")
	end
	
	table.insert(menuOptions, "Forget Spell")
	
	local choice = player:menuString("How may I assist you, young fledgling?", menuOptions)
	
	if (choice == "About Poet") then
		player:dialogSeq({t, "Poets are a path of supporting healers, through buffs, heals, and smaller amounts of damage they are a necessity in all groups and can often function as impressive tanks as well."}, 1)
		
		local spells = player:getSpells()
		local choice = player:menuString("Would you like to test Poet spells?", {"Yes", "No"})
		
		if (choice == "Yes") then
			for x = 1, #spells do
				if (spells[x] ~= 1 and spells[x] ~= 2 and spells[x] ~= 4) then
					player:removeSpell(spells[x])
				end
			end
			
			if (player:canLearnSpell("brace"))  then
				player:addSpell("brace")
			end
			
			if (player:canLearnSpell("guard"))  then
				player:addSpell("guard")
			end
			
			if (player:canLearnSpell("comfort"))  then
				player:addSpell("comfort")
			end
			
			if (player:canLearnSpell("afflict"))  then
				player:addSpell("afflict")
			end
			
			if (player:canLearnSpell("invoke"))  then
				player:addSpell("invoke")
			end
			
			if (player:canLearnSpell("infuse")) then
				player:addSpell("infuse")
			end
			
			player:flushDuration(1)
			player:sendMinitext("You learn beginner poet spells.")
			
			if (player.quest["poetTraining"] == 0) then
				player:giveXP(250)
				player.quest["poetTraining"] = 250
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