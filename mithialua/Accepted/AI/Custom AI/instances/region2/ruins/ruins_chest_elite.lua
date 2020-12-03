ruins_chest_elite = {
click = async(function(player, mob)
	local lootTableCommon = {"pattern_cowhide_elite", 1, "pattern_bulwark_elite", 1, "pattern_studded_shield_elite", 1, "pattern_sleighted_shield_elite", 1, "pattern_starshine_elite", 1, "pattern_fragment_of_time_elite", 1, "pattern_preservative_elite", 1, "pattern_balance_elite", 1}
	local lootTableUncommon = {"pattern_cowhide_elite", 3, "pattern_bulwark_elite", 3, "pattern_studded_shield_elite", 3, "pattern_sleighted_shield_elite", 3, "pattern_starshine_elite", 3, "pattern_fragment_of_time_elite", 3, "pattern_preservative_elite", 3, "pattern_balance_elite", 3}
	local lootTableRare = {"pattern_cowhide_elite", 5, "pattern_bulwark_elite", 5, "pattern_studded_shield_elite", 5, "pattern_sleighted_shield_elite", 5, "pattern_starshine_elite", 5, "pattern_fragment_of_time_elite", 5, "pattern_preservative_elite", 5, "pattern_balance_elite", 5}
	
	local oddsCommon = 100
	local oddsUncommon = 23
	local oddsRare = 8

	local chestOpts = {}
	
	local npcGraphics = {graphic = convertGraphic(mob.look, "monster"), color = mob.lookColor}
	player.npcGraphic = npcGraphics.graphic
	player.npcColor = npcGraphics.color
	player.dialogType = 0
	
	if (player.state == 1) then
		player:sendMinitext("You cannot do that while dead.")
		return
	end
	
	if(mob.registry["beingOpened"] == 1) then
		player:dialog("Someone is already opening this.", npcGraphics)
		return
	else
		mob.registry["beingOpened"] = 1
	end
	
	if(player:hasItem("bandit_key_elite", 1)== true) then
		table.insert(chestOpts, "Open with the elite bandit's key")
	end
	
	table.insert(chestOpts, "Nevermind..")
	
	local menu1 = player:menuString("Before you sits a chest adorned with jewels and an intricate looking lock mechanism.", chestOpts)
	
	if(menu1 == "Open with the elite bandit's key" and player:hasItem("bandit_key_elite", 1) == true) then
		player:removeItem("bandit_key_elite", 1)
		
		for i = 1, 4 do
			local r = math.random(100)
			
			if(r <= oddsRare) then
				local picker = (math.random(#lootTableRare / 2) - 1) * 2 + 1
				local amount = lootTableRare[picker+1]
				local chosenItem = Item(lootTableRare[picker])
				
				player:addItem(chosenItem.yname, amount)
				
				if (#player.group > 1) then
					for i = 1, #player.group do
						if (player.group[i] ~= player.ID) then
							player:msg(4, player.name.." has received "..amount.." "..chosenItem.name.."(s).", player.group[i])
						end
					end
				end
				
				mob:removeHealth(mob.health)
				player:dialogSeq({{graphic = chosenItem.icon, color = chosenItem.iconC}, "In the chest you found "..amount.." "..chosenItem.name.."."}, 1)
			elseif(r <= oddsUncommon) then
				local picker = (math.random(#lootTableUncommon / 2) - 1) * 2 + 1
				local amount = lootTableUncommon[picker+1]
				local chosenItem = Item(lootTableUncommon[picker])
				
				player:addItem(chosenItem.yname, amount)
				
				if (#player.group > 1) then
					for i = 1, #player.group do
						if (player.group[i] ~= player.ID) then
							player:msg(4, player.name.." has received "..amount.." "..chosenItem.name.."(s).", player.group[i])
						end
					end
				end
				
				mob:removeHealth(mob.health)
				player:dialogSeq({{graphic = chosenItem.icon, color = chosenItem.iconC}, "In the chest you found "..amount.." "..chosenItem.name.."."}, 1)
			elseif(r <= oddsCommon) then
				local picker = (math.random(#lootTableRare / 2) - 1) * 2 + 1
				local amount = lootTableCommon[picker+1]
				local chosenItem = Item(lootTableCommon[picker])
				
				player:addItem(chosenItem.yname, amount)
				
				if (#player.group > 1) then
					for i = 1, #player.group do
						if (player.group[i] ~= player.ID) then
							player:msg(4, player.name.." has received "..amount.." "..chosenItem.name.."(s).", player.group[i])
						end
					end
				end
				
				mob:removeHealth(mob.health)
				player:dialogSeq({{graphic = chosenItem.icon, color = chosenItem.iconC}, "In the chest you found "..amount.." "..chosenItem.name.."."}, 1)
			end
		end
	else
		mob.registry["beingOpened"] = 0
		return
	end
end),

move = function(mob, target)
	mob.registry["beingOpened"] = 0
end	
}