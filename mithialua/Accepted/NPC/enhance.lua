enhance = {
click = async(function(player, npc)
	local found = 0
	local result = 0
	local item
	local nItem
	local itemTable = {}
	local g = {graphic = convertGraphic(39, "monster"), color = 16}
	player.npcGraphic = g.graphic
	player.npcColor = g.color
	
	
	for i=0,player.maxInv - 1 do
		nItem=player:getInventoryItem(i)
		if(nItem~=nil) then
			if(nItem.id>0 and nItem.type >= 3 and nItem.type <= 16) then
				if(#itemTable>0) then
					found=0
					for j=1,#itemTable do
						if(itemTable[j]==nItem.id) then
							found=1
						end
					end
					if(found==0) then
						table.insert(itemTable,nItem.id)
					end
				else
					table.insert(itemTable,nItem.id)
				end
			end
		end
	end
	
	item = player:sell("What would you like to upgrade?", itemTable) - 1
	nItem = player:getInventoryItem(item)
	if (nItem.type == 3) then
		result = weapon.enhance(player, item)
	elseif (nItem.type == 4) then
		result = armor.enhance(player, item)
	elseif (nItem.type >= 5 and nItem.type <= 16) then
		result = equipment.enhance(player, item)
	else
		player:dialog("That item is not upgradable.", g)
	end
	
	if (result == 1) then
		player:dialog("Upgrade successful.", g)
	elseif (result == 2) then
		player:dialog("Upgrade unsuccessful, item broken.", g)
	elseif (result == 3) then
		player:dialog("Upgrade unsuccessful, item downgraded.", g)
	elseif (result == 4) then
		player:dialog("Upgrade unsuccessful, durability reduced.", g)
	elseif (result == 5) then
		player:dialog("Upgrade unsuccessful.", g)
	else
		player:dialog("Insufficient items.", {0, 0})
	end
end)
}