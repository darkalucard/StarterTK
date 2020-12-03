--Damage formulas
function Player.addHealthExtend(player, amount, sleep, deduction, ac, ds, print)
	local healer
	local ded = 0
	
	if (player.state == 1) then
		return
	end
	
	if (player.attacker >= 1073741823) then
		healer = Mob(player.attacker)
	elseif (player.attacker > 0) then
		healer = Player(player.attacker)
	end
	
	ded = 1 * string.format("%.2f", player.armor / (player.armor + 400 + 95 * (healer.level + healer.tier^2 + healer.mark^3)))
	
	if (healer:hasDuration("blossom")) then
		amount = amount * 2
	end
	
	if (sleep > 0 and print == 2) then
		amount = amount * player.sleep
	elseif (sleep == 1) then
		amount = amount * player.sleep
		player.sleep = 1
		player:updateState()
	elseif (sleep == 2) then
		amount = amount * player.sleep
	end
	
	if (deduction == 1) then
		if (player.deduction < 0) then
			amount = 0
		elseif (player.deduction > 0) then
			amount = amount * player.deduction
		end
	end
	
	if (ac == 1) then
		if (ded < .85) then
			amount = amount * (1 - ded)
		else
			amount = amount * .15
		end
	end
	
	if (ds > 0 and print == 2) then
		amount = amount - player.dmgShield
	elseif (ds == 1) then
		if (player.dmgShield > 0) then
			if (player.dmgShield > amount) then
				player.dmgShield = player.dmgShield - amount
				amount = 0
			else
				amount = amount - player.dmgShield
				player.dmgShield = 0
			end
		else
			amount = amount - player.dmgShield
			player.dmgShield = 0
		end
	elseif (ds == 2) then
		player.dmgShield = player.dmgShield - amount
	end
	
	amount = -amount
	
	if (healer ~= nil) then
		healer.damage = amount
		healer.critChance = 0
	else
		player.damage = amount
		player.critChance = 0
	end
	
	if (print == 1) then
		if (player.health - amount > player.maxHealth) then
			player.health = player.maxHealth
			player:updateState()
		else
			player.health = player.health - amount
			player:sendStatus()
		end
	elseif (print == 2) then
		return amount
	else
		player_combat.on_healed(player, healer)
		player:sendStatus()
	end
end

function Player.removeHealthWithoutDamageNumbers(player, amount, type)
	local temp_health=0
	
	temp_health=player.health-amount
	
	if temp_health<=0 then
		temp_health=0
	end
	
	player.health=temp_health
	
	if (player.attacker >= 1073741823) then
		Mob(player.attacker).damage = amount
		Mob(player.attacker).critChance = type
	elseif (player.attacker > 0) then
		Player(player.attacker).damage = amount
		Player(player.attacker).critChance = type
	else
		player.damage = amount
		player.critChance = type
	end
	
	if (player.health == 0) then
		onDeathPlayer(player)
	else
		player:sendStatus()
	end
end

function Player.addHealth2(player, amount,type)
	local temp_health
	local temp_amount
	local change_health

	change_health=player.health
	temp_health=player.health+amount

	if temp_health>player.maxHealth then
		player.health=player.maxHealth
	else
		player.health=temp_health
	end
	if temp_health>(math.pow(256,4)-1) then
	   player.health=player.maxHealth
	end

	if player.health==change_health then return false end

	player:sendStatus()
end

function Player.removeHealthExtend(player, amount, sleep, deduction, ac, ds, print)
	local attacker
	local ded = 0
	
	if (player.attacker >= 1073741823) then
		attacker = Mob(player.attacker)
	elseif (player.attacker > 0) then
		attacker = Player(player.attacker)
	end
	
	ded = 1 * string.format("%.2f", player.armor / (player.armor + 400 + 95 * (attacker.level + attacker.tier^2 + attacker.mark^3)))
	
	if (attacker:hasDuration("starburst")) then
		if (print ~= 2) then
			attacker:setDuration("starburst", 0)
		end
		
		amount = amount * 1.5
	end
	
	if (sleep > 0 and print == 2) then
		amount = amount * player.sleep
	elseif (sleep == 1) then
		amount = amount * player.sleep
		player.sleep = 1
		player.updateState()
	elseif (sleep == 2) then
		amount = amount * player.sleep
	end
	
	if (deduction == 1) then
		if (player.deduction < 0) then
			amount = 0
		elseif (player.deduction > 0) then
			amount = amount * player.deduction
		end
	end
	
	if (ac == 1) then
		if (ded < .85) then
			amount = amount * (1 - ded)
		else
			amount = amount * .15
		end
	end
	
	if (ds > 0 and print == 2) then
		amount = amount - player.dmgShield
	elseif (ds == 1) then
		if (player.dmgShield > 0) then
			if (player.dmgShield > amount) then
				player.dmgShield = player.dmgShield - amount
				amount = 0
			else
				amount = amount - player.dmgShield
				player.dmgShield = 0
			end
		else
			amount = amount - player.dmgShield
			player.dmgShield = 0
		end
	elseif (ds == 2) then
		player.dmgShield = player.dmgShield - amount
	end
	
	if (attacker ~= nil) then
		attacker.damage = amount
		attacker.critChance = 0
	else
		player.damage = amount
		player.critChance = 0
	end
	
	if (print == 1) then
		if (player.health - amount <= 0) then
			player.health = 0
			player.state = 1
			player:updateState()
		else
			player.health = player.health - amount
			player:sendStatus()
		end
	elseif (print == 2) then
		return amount
	else
		player_combat.on_attacked(player, attacker)
		player:sendStatus()
	end
end

function Player.addMagic(player, amount)
	local magic = player.magic + amount
	
	if (amount < 0) then
		player:sendMinitext("This is using the wrong function, please contact a GM.")
		return
	end
	
	if (magic > player.maxMagic) then
		player.magic = player.maxMagic
	else
		player.magic = magic
	end
	
	player:sendStatus()
end

function Player.removeMagic(player, amount)
	local magic = player.magic - amount
	
	if (amount < 0) then
		player:sendMinitext("This is using the wrong function, please contact a GM.")
		return
	end
	
	if (magic < 0) then
		player.magic = 0
	else
		player.magic = magic
	end
	
	player:sendStatus()
end

function Player.addMagicExtend(player, amount)
	local temp_magic
	local ded = 0
	
	ded = 1 * string.format("%.2f", player.armor / (player.armor + 400 + 95 * (attacker.level + attacker.tier^2 + attacker.mark^3)))

	if(player.sleep~=nil) then
		amount=amount*player.sleep
	end

	if(player.deduction>0) then
		amount=amount*player.deduction
	end
	
	if (ded < .85) then
		amount = amount * (1 - ded)
	else
		amount = amount * .15
	end

	temp_magic = player.magic + amount
	if(player.magic<amount or temp_magic<=0) then
		player.magic=0
	end
	
	if temp_magic>player.maxMagic then
		player.magic=player.maxMagic
	else
		player.magic=temp_magic
	end
	
	if temp_magic>(math.pow(256,4)-1) then
	   player.magic=player.maxMagic
	end
	
	player:sendStatus()
end

function Player.addShield(player, shielding, maxShield)
	local shield = player.dmgShield
	
	if (maxShield == nil or maxShield > player.maxHealth * .5) then
		maxShield = player.maxHealth * .5
	end
	
	if (shield + shielding > maxShield) then
		player.dmgShield = maxShield
	else
		player.dmgShield = shield + shielding
	end
end

function Player.removeShield(player, unshielding, negative)
	local shield = player.dmgShield
	
	if (negative == nil and shield - unshielding < 0) then
		player.dmgShield = 0
	else
		player.dmgShield = shield - unshielding
	end
end




--Dialogs and menus
function Player.dialogSeq(player, commands, continue)
	local messages, state = { }, { graphic = player.npcGraphic, color = player.npcColor }
	for _, command in pairs(commands) do
		if type(command) == "table" then
			state.graphic = command.graphic
			state.color = command.color
		elseif type(command) == "string" then
			table.insert(messages, {
				graphic = state.graphic,
				color = state.color,
				text = command
			} )
		end
	end
	local pos = 1
	while pos <= #messages do
		local options = { }
		if pos ~= 1 then table.insert(options, "previous") end
		if pos ~= #messages then table.insert(options, "next") end
		if pos == #messages and continue==1 then table.insert(options,"next") end
		
		player.npcGraphic = messages[pos].graphic
		player.npcColor = messages[pos].color
		
		if (messages[pos].graphic == 0 and messages[pos].color == 0) then
			player.dialogType = 1
		else
			player.dialogType = 0
		end
		
		local choice = player:dialog(messages[pos].text, options)
		if(choice == "next") then pos = pos + 1 end
		if(choice == "previous") then pos = pos - 1 end
		if(choice == "quit") then return false end
	end
	return true
end
function Player.menuString(player, message, options)
	if (options == nil) then
		options = {}
	end
	selection = player:menu(message, options)
	return options[selection]
end
function Player.menuString2(player, message, options)
	if (options == nil) then
		options = {}
	end
	selection = player:menu(message, options)
	return options[selection]
end
function Player.menuHairFace(player, message, options)
	if (options == nil) then
		options = {}
	end
	
	local selection = player:hairFaceMenu(message, options)
	return options[selection]
end

function Player.buyDialog(player, dialog, items)
	local amount={}
	for x=1,#items do
		table.insert(amount,Item(items[x]).price)
	end

	local temp=player:buy(dialog,items,amount,{},{})



	return temp, Item(temp).price
end

function Player.showBank(player,dialog)
	local bankItemTable = player:checkBankItems()
	local bankCountTable = player:checkBankAmounts()
	local bankOwnerTable = player:checkBankOwners()
	local bankEngraveTable = player:checkBankEngraves()
	local found = 0
	local amount = 0
	local counter = 0
	local next = next
	
	for i = 1, #bankItemTable do
		if (bankItemTable[i] == 0) then
			counter = #bankItemTable
			
			for j = i, counter do
				table.remove(bankItemTable, i)
				table.remove(bankCountTable, i)
				table.remove(bankOwnerTable, i)
				table.remove(bankEngraveTable, i)
			end
			
			break
		end
	end
	
	if (next(bankItemTable) == 0) then
		player:dialogSeq({"Your bank is currently empty."})
		return false
	end
	
	local temp = player:buy(dialog, bankItemTable, bankCountTable, {}, {})

	for i = 1, 255 do
		if (bankItemTable[i] == temp) then
			found = i
			break
		end
	end

	if (found == 0) then
		return nil
	end

	if (Item(bankItemTable[found]).maxAmount > 1) then
		amount = tonumber(player:input("How many shall you withdraw?"))
		amount = math.ceil(math.abs(amount))
		
		if (amount > bankCountTable[found]) then
			amount = bankCountTable[found]
		end
	else
		amount = 1
	end

	if (amount <= 0) then
		return false
	end

	if (player:hasSpace(bankItemTable[found], amount, bankOwnerTable[found], bankEngraveTable[found]) ~= true) then
		player:sendMinitext("You do not have enough space in your inventory to withdraw that")
		return false
	end

	local worked = player:addItem(bankItemTable[found], amount, 0, bankOwnerTable[found], bankEngraveTable[found])
	
	if (worked == true) then
		player:bankWithdraw(bankItemTable[found], amount, bankOwnerTable[found], bankEngraveTable[found])
	else
		player:sendMinitext("Cannot withdraw "..amount.." "..Item(bankItemTable[found]).name.."(s).")
		return false
	end

	return true
end

function Player.showBankAdd(player)
	local amountCheck = 0
	local itemTable = {}
	local amount = 0
	local found = 0
	
	for i = 0, player.maxInv do
		local nItem = player:getInventoryItem(i)
		
		if (nItem ~= nil and nItem.id > 0) then
			if (#itemTable > 0) then
				found = 0
				
				for j = 1, #itemTable do
					if (itemTable[j] == nItem.id) then
						found = 1
						break
					end
				end
				
				if (found == 0) then
					table.insert(itemTable, nItem.id)
				end
			else
				table.insert(itemTable, nItem.id)
			end
		end
	end
	
	local choice = player:sell("What would you like to deposit?", itemTable)
	local dItem = player:getInventoryItem(choice - 1)
	
	if (dItem == nil) then
		return
	end
	
	if (dItem.depositable) then
		player:sendMinitext("You cannot deposit that item.")
		return false
	end
	
	if (dItem.maxAmount > 1 and dItem.amount > 1) then
		amount = math.abs(tonumber(math.floor(player:input("How many would you like to deposit?"))))

		if (player:hasItem(dItem.id, amount) ~= true) then
			amountCheck = player:hasItem(dItem.id, amount)
			amount = amountCheck
		end
	else
		amount = 1
	end

	if (amount == 0) then
		player:dialogSeq({"You try to hand nothing, but fail."})
		return false
	end

	if (dItem.dura == dItem.maxDura) then
		local moneyAmount = 0
		local moneyChoice = ""
		
		if (dItem.price ~= 0) then
			moneyAmount = math.ceil(dItem.price * 0.10 * amount)
		else
			moneyAmount = math.ceil(dItem.sell * 0.05 * amount)
		end
		
		moneyChoice = player:menuString("The fee is "..moneyAmount.." do you accept?", {"Yes", "No"})
		
		if (moneyChoice == "Yes") then
			if (player:hasItem(dItem.id, amount) == true) then
				if (player.money >= moneyAmount) then
					player.money = player.money - moneyAmount
					player:bankDeposit(dItem.id, amount, dItem.owner, dItem.realName)
					
					if (amount == 1) then
						player:removeItemSlot((choice - 1), amount, 2)
					else
						player:removeItem(dItem.id, amount, 2)
					end
					
					player:sendStatus()
				else
					player:dialogSeq({"You do not have enough money to cover my safe keeping fees."})
					return false
				end
			else
				player:dialogSeq({"You do not have what it is you want me to hold for you."})
				return false
			end
		else
			return false
		end
	else
		player:sendMinitext("I don't want your junk, ask a smith to fix it.")
		return false
	end
	
	return true
end

function Player.bankAddMoney(player)


	--player:dialogSeq({"Disabled until further notice."})

	local maxamount=(2^32) - 1
	local amount=player:input("How much would you like to deposit? (Currently deposited: "..player.bankMoney..", Your Maximum: "..maxamount..")")
	amount=tonumber(math.ceil(math.abs(amount)))
	if(amount==0) then return false end
	if(amount>player.money) then
		amount=player.money
	end
	if(amount>maxamount) then
		amount=maxamount
	end
	if(player.bankMoney + amount > maxamount) then
		amount=maxamount-player.bankMoney
	end
	if(player.bankMoney == maxamount) then
		player:dialogSeq({"Your bank is already full of money. (Current maximum: "..maxamount..")"})
	end

	player.money=player.money-amount
	player.bankMoney=player.bankMoney+amount
	player:sendStatus()
	return true
end

function Player.bankWithdrawMoney(player)


	--player:dialogSeq({"Disabled until further notice."})

	local inBank=player.bankMoney
	if(inBank<=0) then
		player:dialogSeq({"Sorry, You currently do not have anything in your bank."})
		return
	end
	local amount=player:input("You have " .. inBank .. " coins in your account.  How much would you like to withdraw?")
	amount=tonumber(math.ceil(math.abs(amount)))
	if (amount<=0) then return end

	if(amount>inBank) then
		amount=inBank
	end

		player.money=player.money+amount
		player.bankMoney=player.bankMoney-amount
		player:sendStatus()
end

function Player.sellExtend(player,dialog,items)
	local amount=1
	local choice=player:sell(dialog,items)
	player:talk(0, ""..choice)
	local sell = player:getInventoryItem(choice-1)
	local sell_item = {id = sell.id, dura = sell.dura, maxDura = sell.maxDura, sell = sell.sell, name = sell.name, amount = sell.amount}
	if(sell_item.amount>1) then
		amount=math.abs(tonumber(math.ceil(player:input("How many do you wish to sell?"))))

		if(amount>4000000000 or player:hasItem(sell_item.id,amount) ~= true) then
			player:sendMinitext("You cannot do that.")
			return
		end


	else
		amount=1
	end

	if (sell.owner > 0 or sell.realName ~= "") then
		player:dialogSeq({"I am sorry, but I am not interested in personal items."})
		return
	end
	
	local duracheck=1
	if(sell_item.maxDura~=0) then
		if(sell_item.dura~=sell_item.maxDura) then
			player:dialogSeq({"I am sorry, but I am not interested by items that are not in perfect condition."})
			return
		end
	else
		duracheck=1
	end
	
	local choice=player:menuString("I will buy your " .. amount .. " " .. sell_item.name .. " for " .. math.floor(sell_item.sell*amount*duracheck) .. " gold.  Deal?", {"Yes","No"})
	if(choice=="Yes") then
		local selltable = {sell_item.id, amount}
		player.money=player.money+math.floor(sell_item.sell*amount*duracheck)
		player:removeItem(sell_item.id,amount,1)
  		player:sendStatus()
		return selltable
	end
end

function Player.buyExtend(player,dialog,items,prices,maxamounts)
	if(items==nil) then items={ } end
	if(prices==nil) then prices={ } end
	local x=0
	local amount=1
	local t = {graphics = player.npcGraphic, color = player.npcColor}
	if #prices==0 then
		for i=1,#items do
			table.insert(prices,Item(items[i]).price)
		end
	end

	local choice=player:buy(dialog,items,prices,{},{})
	
	for i=1,#items do
		if(Item(items[i]).id==choice) then
			x=i
			break
		end
	end
	if(x==0) then return nil end

	if(Item(choice).maxAmount>1) then
		amount=player:input("How many would you like?")
		amount = amount * 1
	end
	
	if (maxamounts ~= nil and maxamounts[x] ~= nil and maxamounts[x] < amount) then
		player:dialog("I can only sell you "..maxamounts[x].." more "..Item(choice).name..".", t)
	end

	if(player.money<(prices[x]*amount)) then
		player:dialog("You do not have enough money.",{})
		return nil
	end
	amount=math.abs(amount)
	local newChoice=player:menuString("I will sell you " .. amount .. " " .. Item(choice).name .. " for " .. prices[x]*amount .. " gold. Deal?",{"Yes","No"})
	if(newChoice=="Yes") then
		if(player:hasSpace(Item(choice).name,amount) and (player.money>=(prices[x]*amount))) then
			local buytable = {Item(choice).id, amount}
			player:addItem(Item(choice).name,amount)
			player.money=player.money-(prices[x]*amount)
			player:sendStatus()
			return buytable
		else
			player:sendMinitext("You do not have enough space in your inventory.")
		end
	end
end

function Player.repairExtend(player)
	local repairList= { }

	for x=0,(player.maxInv - 1) do
		local item=player:getInventoryItem(x)
		if(item ~= nil) then
			if(item.price>0) then
				if(item.type>2 and item.type<17) then
					if(item.dura<item.maxDura) then
						table.insert(repairList,x)
					end
				end
			end
		end
	end

	if(#repairList==0) then
		player:dialogSeq({"You have nothing to repair"})
	return end

	local choice=player:sell2("What shall I repair?",repairList)
	local item=player:getInventoryItem(choice-1)
	local cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.30)
	local nchoice=player:menuString("This item will require " .. cost .. " gold to be repaired. Shall I fix?",{"Yes","No"})
	if(nchoice=="Yes") then
		if(player.money>cost) then
			player.money=player.money-cost
			item.dura=item.maxDura
			player:updateInv()
			player:sendStatus()
			item.repairCheck = 0
		end
	else
	end

end

function Player.repairAll(player,npc)
	local cost=0
	local total=0
	local item=player:getEquippedItem(EQ_FACEACC)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.30)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_HELM)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_CROWN)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_WEAP)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_ARMOR)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_SHIELD)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_LEFT)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_MANTLE)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_RIGHT)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_SUBLEFT)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_COAT)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_SUBRIGHT)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_NECKLACE)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	item=player:getEquippedItem(EQ_BOOTS)
	if(item~=nil) then
		if(item.price~=0) then
			if(item.dura<item.maxDura) then
				if(item.dura<item.maxDura) then
					cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
					if(player.money>=cost) then
						player.money=player.money-cost
						total=total+cost
						item.dura=item.maxDura
						player:sendStatus()
						player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
						item.repairCheck = 0
					else
						player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
					end
				end
			end
		else
			player:sendMinitext(""..item.name.." is not a repairable item.")
		end
	end
	for x=0,25 do
		item=player:getInventoryItem(x)
			if(item~=nil and item.type>2 and item.type<17) then
				if(item.price~=0) then
					if(item.dura<item.maxDura) then
						if(item.dura<item.maxDura) then
							cost=math.ceil((item.price/item.maxDura)*(item.maxDura-item.dura)*0.90)
							if(player.money>=cost) then
								player.money=player.money-cost
								total=total+cost
								item.dura=item.maxDura
								player:sendStatus()
								player:sendMinitext(""..item.name.." repaired for "..cost.." coins.")
								item.repairCheck = 0
							else
								player:sendMinitext(""..item.name.." was not repaired: cost is "..cost.." coins.")
							end
						end
					end
				else
					player:sendMinitext(""..item.name.." is not a repairable item.")
				end
			end
	end
	--player:updateInv()
	player:sendStatus()
	player:updateState()
	player:sendMinitext("Total spent repairing: "..total.." coins.")
end


function Player.clanShowBank(player,dialog)


	player:dialogSeq({"Disabled until further notice."})

	local maxslots=player.mapRegistry["clanbankmaxslots"]
	local max=player.mapRegistry["clanbankcount"]
	local bank_item_table={}
	local bank_count_table={}
	local regname
	local found
	local amount=0
	if max==0 then
		player:dialogSeq({"Your clan bank is currently empty."})
		return false
	end

	for x=1,max do
		regname="clanbank" .. x
		table.insert(bank_item_table,player.mapRegistry[regname])
		regname="clanbank" .. x .. "count"
		table.insert(bank_count_table,player.mapRegistry[regname])
	end
	local temp=player:buy("You are currently using "..max.." bank slots over a maximum of "..maxslots..".",bank_item_table,bank_count_table)

	for x=1,max do
		if bank_item_table[x]==temp then
			found=x
			break
		end
	end

	if(found==0) then return nil end

	if(Item(bank_item_table[found]).maxAmount>1) then
		amount=player:input("How many shall you withdraw?")
		amount=math.ceil(math.abs(tonumber(amount)))
		if(amount>bank_count_table[found]) then amount=bank_count_table[found] end
	else
		amount=1
	end

        player.registry["isBanking"] = 1
	local worked = player:addItem(bank_item_table[found],amount)
        player.registry["isBanking"] = 0
	if(worked==true) then
		player.mapRegistry["clanbank" .. found .. "count"]=bank_count_table[found]-amount
		player:clanSaveBank()
	else
		player:sendMinitext("Cannot withdraw " .. amount)
		return false
	end

	return true
end

function Player.clanSaveBank(player)


	player:dialogSeq({"Disabled until further notice."})


	local maxslots=player.mapRegistry["clanbankmaxslots"]
	local max=player.mapRegistry["clanbankcount"]
	local y=0
	local x
	for x=1,max do
		if(player.mapRegistry["clanbank" .. x .. "count"]==0) then
			y=1
		end
		if(y>0) then
			player.mapRegistry["clanbank" .. x .. "count"]=player.mapRegistry["clanbank" .. x+1 .. "count"]
			player.mapRegistry["clanbank" .. x]=player.mapRegistry["clanbank" .. x+1]
		end
	end
	if(y>0) then
		player.mapRegistry["clanbankcount"]=max-1
	end
end

function Player.clanAddToBank(player,item,amount)


	player:dialogSeq({"Disabled until further notice."})


	local maxslots=player.mapRegistry["clanbankmaxslots"]
	local max=player.mapRegistry["clanbankcount"]

	if(max==0) then
		player.mapRegistry["clanbankcount"]=1
		player.mapRegistry["clanbank1"]=item
		player.mapRegistry["clanbank1count"]=amount
		return true
	end

	for i=1,max do
		if(player.mapRegistry["clanbank" .. i]==item) then
			player.mapRegistry["clanbank" .. i .. "count"]=player.mapRegistry["clanbank" .. i .. "count"]+amount
			return true
		end
	end

	player.mapRegistry["clanbankcount"]=max+1
	player.mapRegistry["clanbank" .. max+1]=item
	player.mapRegistry["clanbank" .. max+1 .. "count"]=amount

	return true
end

function Player.clanShowBankAdd(player)


	player:dialogSeq({"Disabled until further notice."})

	local maxslots=player.mapRegistry["clanbankmaxslots"]
	local max=player.mapRegistry["clanbankcount"]
	local itemTable = {}
	local amount=1
	local found=0
        player.npcGraphic=0
	player.npcColor=0
	for i=0,player.maxInv - 1 do
		local nItem=player:getInventoryItem(i)
		if(nItem~=nil) then
			if(nItem.id>0) then
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
	local choice=player:sell("What would you like to deposit?",itemTable)
	local dItem=player:getInventoryItem(choice-1)
	if(not dItem.depositable) then
		player:sendMinitext("You cannot deposit that!")
		return
	end
	if(not dItem.droppable) then
		player:sendMinitext("You cannot deposit that!")
		return
	end
	if(not dItem.exchangeable) then
		player:sendMinitext("You cannot deposit that!")
		return
	end
	if(dItem.realName~="") then
		player:sendMinitext("You cannot deposit an engraved item.")
		return
	end
	if(dItem.owner~=0) then
		player:sendMinitext("You cannot deposit a bonded item.")
		return
	end
	if(max>=maxslots) then
		player:dialogSeq({"Sorry, but your clan bank is filled to capacity."})
	end
	if(dItem.amount>1) then
		amount=math.abs(tonumber(math.ceil(player:input("How many would you like to deposit?"))))

		if(amount>=2147483648) then amount=1 end
		if(player:hasItem(dItem.id,amount) ~= true) then
			amount=dItem.amount
		end
	end
	if(dItem.dura==dItem.maxDura) then
				player:clanAddToBank(dItem.id,amount)
				player:removeItem(dItem.id,amount,2)
				player:sendStatus()
	else
		player:sendMinitext("That item must be in perfect condition to be deposited.")
	end
	return true

end

function Player.clanBankAddMoney(player)


	player:dialogSeq({"Disabled until further notice."})


	local amount=player:input("How much would you like to deposit to your clan Bank ?")
	amount=tonumber(math.ceil(math.abs(amount)))
	if(amount==0) then return false end
	if(amount>player.money) then
		amount=player.money
	end

	player.money=player.money-amount
	player.mapRegistry["clanbankmoney"]=player.mapRegistry["clanbankmoney"]+amount
	player:sendStatus()
	return true
end

function Player.clanBankWithdrawMoney(player)


	player:dialogSeq({"Disabled until further notice."})


	local inBank=player.mapRegistry["clanbankmoney"]
	local amount=player:input("Your clan bank currently holds " .. inBank .. " coins.  How much would you like to withdraw?")
	amount=tonumber(math.ceil(math.abs(amount)))
	if(amount>inBank) then
		amount=inBank
	end

		player.money=player.money+amount
		player.mapRegistry["clanbankmoney"]=player.mapRegistry["clanbankmoney"]-amount
		player:sendStatus()
end

function Player.clanViewBank(player,dialog)


	player:dialogSeq({"Disabled until further notice."})


	local maxslots=player.mapRegistry["clanbankmaxslots"]
	local max=player.mapRegistry["clanbankcount"]
	local bank_item_table={}
	local bank_count_table={}
	local regname
	local found
	local amount=0
	if max==0 then
		player:dialogSeq({"Your clan bank is currently empty."})
		return false
	end

	for x=1,max do
		regname="clanbank" .. x
		table.insert(bank_item_table,player.mapRegistry[regname])
		regname="clanbank" .. x .. "count"
		table.insert(bank_count_table,player.mapRegistry[regname])
	end
	local temp=player:buy("This tool allows you to see the contents of your Clan Bank. You may not withdraw anything from this window. You are currently using "..max.." bank slots over a maximum of "..maxslots..".",bank_item_table,bank_count_table)
end




--Player actions and spells
function Player.addItemExtend(player,id,amount,engrave)

	local worked=player:addItem(id,amount,0,engrave)

	if(not worked) then
		player:drop(id,amount)
	end
end

function Player.calcThrow(player)
	local side=player.side
	local temp = {}
	local tempfinal = {}
	tempfinal = nil
	local bowrange=player.mapRegistry["bowrange"]
	if(bowrange==0) then
		bowrange=1
	end

	if(side==0) then
		for y=(player.y-bowrange),(player.y-1) do
			temp=player:getObjectsInCell(player.m,player.x,y,BL_NPC)
			if(#temp>0) then
				tempfinal=temp
			end
			temp=player:getObjectsInCell(player.m,player.x,y,BL_MOB)
			if(#temp>0) then
				tempfinal=temp
			end
			temp=player:getObjectsInCell(player.m,player.x,y,BL_PC)
			if(#temp>0 and temp[1].armorColor~=15) then
				tempfinal=temp
			end
			if(getPass(player.m,player.x,y)~=0) then
				tempfinal=nil
			end
			if(player:objectCanMove(player.x,y,0)==false) then
				tempfinal=nil
			end
		end
	elseif(side==1) then
		for x=(player.x+bowrange),(player.x+1),-1 do
			temp=player:getObjectsInCell(player.m,x,player.y,BL_NPC)
			if(#temp>0) then
				tempfinal=temp
			end
			temp=player:getObjectsInCell(player.m,x,player.y,BL_MOB)
			if(#temp>0) then
				tempfinal=temp
			end
			temp=player:getObjectsInCell(player.m,x,player.y,BL_PC)
			if(#temp>0 and temp[1].armorColor~=15) then
				tempfinal=temp
			end
			if(getPass(player.m,x,player.y)~=0) then
				tempfinal=nil
			end
			if(player:objectCanMove(x,player.y,1)==false) then
				tempfinal=nil
			end
		end
	elseif(side==2) then
		for y=(player.y+bowrange),(player.y+1),-1 do
			temp=player:getObjectsInCell(player.m,player.x,y,BL_NPC)
			if(#temp>0) then
				tempfinal=temp
			end
			temp=player:getObjectsInCell(player.m,player.x,y,BL_MOB)
			if(#temp>0) then
				tempfinal=temp
			end
			temp=player:getObjectsInCell(player.m,player.x,y,BL_PC)
			if(#temp>0 and temp[1].armorColor~=15) then
				tempfinal=temp
			end
			if(getPass(player.m,player.x,y)~=0) then
				tempfinal=nil
			end
			if(player:objectCanMove(player.x,y,2)==false) then
				tempfinal=nil
			end
		end
	elseif(side==3) then
		for x=(player.x-bowrange),(player.x-1) do
			temp=player:getObjectsInCell(player.m,x,player.y,BL_NPC)
			if(#temp>0) then
				tempfinal=temp
			end
			temp=player:getObjectsInCell(player.m,x,player.y,BL_MOB)
			if(#temp>0) then
				tempfinal=temp
			end
			temp=player:getObjectsInCell(player.m,x,player.y,BL_PC)
			if(#temp>0 and temp[1].armorColor~=15) then
				tempfinal=temp
			end
			if(getPass(player.m,x,player.y)~=0) then
				tempfinal=nil
			end
			if(player:objectCanMove(x,player.y,3)==false) then
				tempfinal=nil
			end
		end
	end
	return tempfinal
end

function Player.calcRangedDamage(player,target)
	local mindamage=0
	local maxdamage=0
	local finaldamage=0
	local enchant=0

	

	local item=player:getEquippedItem(EQ_FACEACC)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_HELM)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_CROWN)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_WEAP)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_ARMOR)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_SHIELD)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_LEFT)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_MANTLE)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_RIGHT)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_SUBLEFT)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_SUBRIGHT)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_COAT)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_NECKLACE)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end
	item=player:getEquippedItem(EQ_BOOTS)
	if(item~=nil) then
		mindamage=mindamage+item.minDmg
		maxdamage=maxdamage+item.maxDmg
	end

	
	local calcrange

	
	if(player.level < 99) then
		calcrange=10+(player.level/6)/2
	elseif(player.level >= 99) then
		calcrange=(40+(player.maxMagic/2500)+(player.maxHealth/2500))/2
		
	end

	enchant = calcrange * 1.2

	finaldamage=math.random(mindamage,maxdamage)
	finaldamage=finaldamage*enchant


	--[[ commenting out as target doesnt import properly

	finaldamage=finaldamage+(finaldamage*player.dam*0.01)


	finaldamage=finaldamage+(finaldamage*target.ac*0.01)

	if(target.sleep>0) then
		finaldamage=finaldamage*target.sleep
	end
	if(target.deduction>0) then
		finaldamage=finaldamage*target.deduction
	end		

	]]--

	return finaldamage

end

function Player.calcRangedHit(player,bullshit,target)
	
	local hitcalc=0
	local critcalc=0
	local hitchance=0
	local result=0

	hitcalc=55+(player.grace/2)-(target.grace/2)+(player.hit*1.5)+(player.level)-(target.level)

	if hitcalc<=5 then hitcalc=5 end
	if hitcalc>=95 then hitcalc=95 end

	hitchance=math.random(1,100)
	critcalc=player.hit/3

	if critcalc<=1 then critcalc=1 end
	if critcalc>=99 then critcalc=99 end

	if hitchance>hitcalc then result=0 end
	if hitchance<hitcalc then result=1 end
	
	if(result==1 and hitchance<critcalc) then result=3 end

	return result
end

function Player.activeSpells(player,spells)
	local isActive=false
	for _,spell in pairs(spells) do
		if(isActive~=true) then
			isActive=player:hasDuration(spell)
		end
	end
	return isActive
end

function Player.addGold(player, amount)
	if (amount > 0) then
		player.money = player.money + amount
		player:sendStatus()
		return true
	elseif (amount < 0) then
		player:sendMinitext("This is using the wrong function, please inform a GM.")
		return false
	end
end

function Player.removeGold(player, amount)
	if (amount > 0 and player.money < amount) then
		player:sendMinitext("You do not have enough money.")
		player:sendStatus()
		return false
	elseif (amount > 0 and player.money >= amount) then
		player.money = player.money - amount
		player:sendStatus()
		return true
	elseif (amount < 0) then
		player:sendMinitext("This is using the wrong function, please inform a GM.")
		return false
	end
end

function Player.getEquippedDura(player, id, slot)
	local item
	
	if (slot ~= nil) then
		item = player:getEquippedItem(slot)
		
		if (item ~= nil and item.id == id) then
			return item.dura
		end
	else
		for i = 0, 13 do
			item = player:getEquippedItem(i)
			
			if (item ~= nil and item.id == id) then
				return item.dura
			end
		end
	end
	
	return nil
end

function Player.calcDPS(player, times)
	local iterations = ((times * 1000) / ((player.attackSpeed * 1000) / 50))
	local hits = 0
	local damage = 0
	local target = getTargetFacing(player, BL_MOB)
	
	if (target == nil) then
		target = getTargetFacing(player, BL_PC)
	end
	
	if (target == nil) then
		return
	end
	
	for i = 1, iterations do
		hitCritChance(player, target)
		
		if (player.critChance > 0) then
			hits = hits + 1
			swingDamage(player)
			swingDamage(player, target)
			damage = damage + player.damage
		end
	end
	
	player:msg(0, "You dealt "..damage.." damage in "..(times / 60).." minute(s).", player.ID)
	player:msg(0, "Damage Per Second: "..(damage / times).." Hits: "..hits, player.ID)
end




function Player.canAction(player, dead, mount, disguise)
	if (player.state == -1) then
		player:sendMinitext("You cannot do that right now.")
		return false
	end
	
	if (dead == 1 and player.state == 1) then
		player:sendMinitext("You try, but fail because you are dead!")
		return false
	end
	
	if (mount == 1 and player.state == 3) then
		player:sendMinitext("You cannot do that right now.")
		return false
	end
	
	if (disguise == 1 and player.state == 4) then
		player:sendMinitext("You cannot do that right now.")
		return false
	elseif (disguise == 2 and (player.state == 4 or player.gfxClone == 1)) then
		player:sendMinitext("You cannot do that right now.")
		return false
	end
	
	return true
end



function Player.canCast(player, dead, mount, disguise)
	local minitext = "You cannot cast that now."
	
	if (player.state == -1) then
		player:sendMinitext(minitext)
		return false
	end
	
	if (dead == 1 and player.state == 1) then
		player:sendMinitext("You are too busy being dead to cast that spell.")
		return false
	end
	
	if (mount == 1 and player.state == 3) then
		player:sendMinitext(minitext)
		return false
	end
	
	if (disguise == 1 and player.state == 4) then
		player:sendMinitext(minitext)
		return false
	elseif (disguise == 2 and (player.state == 4 or player.gfxClone == 1)) then
		player:sendMinitext(minitext)
		return false
	end
	
	return true
end

function Player.canPK(player, target)
	if (target.pvp == 0 and target.PK > 0 and not player:getPK(target.ID)) then
		target:setPK(player.ID)
		player:sendMinitext("You have attacked "..target.name.." on a non-pvp map and are now free to be killed by them.")
	elseif (target.pvp == 2 and target.PK == 0 and not player:getPK(target.ID)) then
		target:setPK(player.ID)
		player:sendMinitext("You have attacked "..target.name.." unwarranted and are now free to be killed by them.")
	end
	
	if (target.pvp > 0 or target.PK > 0 or player:getPK(target.ID)) then
		if (#player.group > 1) then
			for i = 1, #player.group do
				if (target.id == player.group[i] and target.sleep == 1) then
					return false
				end
			end
		elseif (player.m >= 7010 and player.m <= 7011) then
			if (player.armorColor == target.armorColor and target.sleep == 1) then
				return false
			end
		end
		
		return true
	end
	
	return false
end

function Player.sellExperience(player, npc)
	local npcGraphic = {graphic = convertGraphic(npc.look, "monster"), color = npc.lookColor}
	local menuOptions = {}
	local menuChoice = ""
	
	if (npc.npcType == 1) then
		player.dialogType = 1
	else
		player.npcGraphic = npcGraphic.graphic
		player.npcColor = npcGraphic.color
		player.dialogType = 0
	end

	
	if (player:calculateIncrease(false).vitaMana > 0) then
		table.insert(menuOptions, "Yes, I'd like to train my body.")
		table.insert(menuOptions, "Yes, I'd like to train my mind.")
	end
	
	if (player.level >= 251) then
		table.insert(menuOptions, "Yes, I'd like to rebirth.")
	end
	
	table.insert(menuOptions, "No, I just liked touching your beard.")
	player.lastClick = npc.ID
	menuChoice = player:menuString("Hello there. Looking to train your body or mind?", menuOptions)
	
	if (menuChoice == "Yes, I'd like to train my body." or menuChoice == "Yes, I'd like to train my mind.") then
		local subMenuOptions = {"I'd like to train a small amount.", "I'd like to train to my fullest."}
		local subMenuChoice = player:menu("How much would you like to train today?", subMenuOptions)
		local level = 0
		
		if (subMenuChoice == 1) then
			local results = player:calculateIncrease(false)
			
			if (menuChoice == "Yes, I'd like to train my body.") then
				player.baseHealth = player.baseHealth + results.vitaMana
				player.exp = player.exp - results.experience
				player.expSold = player.expSold + results.experience
				player.level = player.level + results.level
			elseif (menuChoice == "Yes, I'd like to train my mind.") then
				player.baseMagic = player.baseMagic + results.vitaMana
				player.exp = player.exp - results.experience
				player.expSold = player.expSold + results.experience
				player.level = player.level + results.level
			end
			
			player:calcStat()
			player:sendStatus()
			
			if (results.vitaMana >= 0) then
				--player:trainingHelperOne(npc)
				player:sellExperience(npc)
			end
		elseif (subMenuChoice == 2) then
			local confirmOptions = {"Yes", "No"}
			local confirm = ""
			local results = player:calculateIncrease(true)
			local maxVitaMana = results.vitaMana / 100
			local input = 0
			
			if (menuChoice == "Yes, I'd like to train my body.") then
				input = tonumber(player:input("How much would you like to train? You can train "..maxVitaMana.." times."))
				
				if (input > maxVitaMana) then
					input = maxVitaMana
				end
				
				confirm = player:menuString("Really train "..(input * 100).." health?", confirmOptions)
				
				if (confirm == "Yes") then
					for i = 1, input do
						local subResults = player:calculateIncrease(false)
						
						player.baseHealth = player.baseHealth + subResults.vitaMana
						player.exp = player.exp - subResults.experience
						player.expSold = player.expSold + subResults.experience
						player.level = player.level + subResults.level
					end
				end
			elseif (menuChoice == "Yes, I'd like to train my mind.") then
				input = tonumber(player:input("How much would you like to train? You can train "..maxVitaMana.." times."))
				
				if (input > maxVitaMana) then
					input = maxVitaMana
				end
				
				confirm = player:menuString("Really train "..(input * 100).." magic?", confirmOptions)
				
				if (confirm == "Yes") then
					for i = 1, input do
						local subResults = player:calculateIncrease(false)
						
						player.baseMagic = player.baseMagic + subResults.vitaMana
						player.exp = player.exp - subResults.experience
						player.expSold = player.expSold + subResults.experience
						player.level = player.level + subResults.level
					end
				end
			end
			
		    -- calculate level
		    player:calcStat()
			player:sendStatus()
			
			if (results.vitaMana >= 0) then
			--	player:trainingHelperOne(npc)
				player:sellExperience(npc)
			end
		end
	elseif (menuChoice == "Yes, I'd like to rebirth.") then
		local menuOptions = {"Yes", "No"}
		local subMenuChoice = ""
		
		subMenuChoice = player:menuString("Return all your training and start fresh? Cost is 40% of your training and 25,000 gold. ((This makes you level 1 with 100 vita and 100 mana and 60% sold experience returned.))", menuOptions)
		
		if (subMenuChoice == "Yes" and player:removeGold(1) == true) then
			player:giveXP(100000)
			player.expSold = 2000
			player.baseHealth = 100
			player.baseMagic = 100
			player.level = 1
			player:calcStat()
			player:sendStatus()
		end
	elseif (menuChoice == "No, I just liked touching your beard.") then
		player:dialogSeq({npcGraphic, "I see then, goodbye."}, 1)
	else
		player:talk(1,menuChoice)
	end 
		
end




function Player.calculateIncrease(player, full)
	local currentExperience = player.exp
	local level = player.level + (player.mark*250)
	local results = {vitaMana = 0, experience = 0, level = 0}
	local increase = 100
	local totalSells = math.floor((player.baseHealth + player.baseMagic)/100)
	local costAtLevel = expCostAtLevel(level)
	
	--if the stats you have exceed the amount of sales that are supposed to be at your san*250 + level, then there
	-- is probably a san glitch do not sell
	if (totalSells > totalSellsAtLevel(level+1)) then
		return results
	end
	
	while ((currentExperience - (results.experience + costAtLevel)) >= 0 and (player.level + results.level) <= 250) do
		
		costAtLevel = expCostAtLevel(level)
		results.vitaMana = results.vitaMana + increase
		results.experience = results.experience + costAtLevel
		
		totalSells = totalSells + 1
		
		if (totalSells >= totalSellsAtLevel(level)) then
			results.level = results.level + 1
			level = level + 1
		end
				
		if (full == false) then
			break
		end			
	end

	return results
end




--GM functions
function Player.gmMsg(player, msg, level)
	local online = {}
	online = player:getUsers()

	if (level == nil) then
		level = 0
	end
	
	if (msg == nil) then
		return
	end
	
	for i = 1, #online do
		if (online[i].gmLevel > level) then
			player:msg(12, msg, online[i].ID)
		end
	end
end