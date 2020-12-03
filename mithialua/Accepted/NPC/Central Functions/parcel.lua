function Player.sendParcelTo(player, npc)
	local itemlist = {}
	for i=0,player.maxInv - 1 do
		local nItem=player:getInventoryItem(i)
		if(nItem~=nil) then
			if(nItem.id>0) then
				if(#itemlist>0) then
					found=0
					for j=1,#itemlist do
						if(itemlist[j]==nItem.id) then
							found=1
						end
					end
					if(found==0) then
						table.insert(itemlist,nItem.id)
					end
				else
					table.insert(itemlist,nItem.id)
				end
			end
		end
	end
	local choice = player:sell("What would you like to send?", itemlist)
	local item = player:getInventoryItem(choice - 1)
	local amount = 1
	if (item.maxAmount > 1) then
		amount = tonumber(player:input("How many "..item.name.." would you like to send?"))
		if (type(amount) == "string") then
			player:sendMinitext("Please input a number, with no letters.")
		else
			amount = math.abs(amount)
		end
	end
	local receiver = player:input("Who will you send your "..amount.." "..item.name.."(s) to?")
	receiver = getOfflineID(receiver)
	if (receiver ~= false) then
		if (player:hasItem(item.id, amount) ~= true) then
			player:dialog("You only have "..player:hasItem(item.id, amount).." "..item.name"(s).", {})
		elseif (player:hasItemDura(item.id, amount) ~= true) then
			player:dialog("All items must be in perfect condition, please repair and try again.", {})
		else
			if (player:sendParcel(receiver, player.ID, item.id, amount, item.owner, item.realName, 0) == true) then
				if (amount > item.maxAmount) then
					player:removeItem(item.id, amount, 2)
				else
					player:removeItemSlot(choice - 1, amount, 2)
				end
			end
		end
	else
		player:sendMinitext("Receiver does not exist!")
	end
end

function Player.receiveParcelFrom(player, npc)
	local item = player:getParcel()
	if (player:hasSpace(item.id, item.amount, item.owner, item.realName)) then
		player:addItem(item.id, item.amount, 0, item.owner, item.realName)
		player:removeParcel(item.sender, item.id, item.amount, item.pos, item.owner, item.realName, item.npcflag)
		if (item.npcFlag > 0) then
			local sender = NPC(item.sender)
			player:sendMinitext(sender.name.." sent you an item!")
		else
			local sender = getOfflineID(item.sender)
			if (sender ~= false) then
				player:sendMinitext(sender.." sent you an item!")
			else
				player:sendMinitext("Sender does not exist!")
			end
		end
	else
		player:dialog("You do not have enough space for "..item.amount.." "..item.name.."(s), please clear some room and try again.", t)
	end
	player:sendStatus()
end

function Player.receiveParcelFromList(player, npc)
	--[[local parcellist = player:getParcelList()
	local idlist = {}
	local amountlist = {}
	local namelist = {}
	for x = 1, #parcellist do
		table.insert(idlist, parcellist[x].id)
		table.insert(amountlist, parcellist[x].amount)
		if (string.len(parcellist[x].realName) > 0) then
			table.insert(namelist, parcellist[x].realName)
		else
			table.insert(namelist, parcellist[x].name)
		end
	end
	local choice = player:sell("Testing Parcel List.", idlist, amountlist, namelist)
	if (player:hasSpace(parcellist[choice].id, parcellist[choice].amount, parcellist[choice].owner, parcellist[choice].realName) == true) then
		player:addItem(parcellist[choice].id, parcellist[choice].amount, parcellist[choice].owner, parcellist[choice].realName)
		player:removeParcel(parcellist[choice].sender, parcellist[choice].id, parcellist[choice].amount, parcellist[choice].pos, parcellist[choice].owner, parcellist[choice].realName, parcellist[choice].npcflag)
	end]]--
				
end