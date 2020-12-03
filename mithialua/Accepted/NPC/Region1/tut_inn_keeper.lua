tut_inn_keeper = {
click = async(function(player,npc)
	local t={graphic=convertGraphic(605,"monster"),color=0}
	local opts = { }
	local optsPO = {}
	local welcomemsg = {"Welcome to my inn. How can I help you?","Welcome!","Welcome, how may I help you?","Welcome, we have the finest island wine in the island!","Welcome "..player.name..". Would you fancy some wine?"}
		if (player.npc["tut_inn_keeper"] > 0) then
			table.insert(welcomemsg,"Welcome back "..player.name..", what would you like today?")
		end
	local rwelcome = math.random(#welcomemsg)
	local buymsg = {"What do you wish to buy?","What do you wish to buy today?","What would you like to buy?","What can I offer you?"}
	local rbuy = math.random(#buymsg)
	local sellmsg = {"What do you wish to sell?","What would you like to sell?","What would you like to sell today?"}
	local rsell = math.random(#sellmsg)
	local offer_engrave = "Special Offer T-Shirt"
	local offer_items = {}
	
	if (player.sex == 0) then
		offer_items = {190000001,190000007,190000013,190000019}
	else
		offer_items = {190000002,190000008,190000014,190000020}
	end
	
	local offer_item = offer_items[math.random(#offer_items)]
	
	player.npcGraphic=t.graphic
	player.npcColor=t.color
	player.dialogType = 0
	table.insert(opts,"Buy")
	table.insert(opts,"Sell")
	table.insert(opts,"Power Nap")
	table.insert(opts,"Deposit Item")
	table.insert(opts,"Withdraw Item")
	table.insert(opts,"Deposit Money")
	table.insert(opts,"Withdraw Money")
	table.insert(opts,"Post Office Services")
		if (player.npc["tut_inn_keeper"] == 15) then
			player.npc["tut_inn_keeper_special_offer_timer"] = os.time()
		elseif ((player.npc["tut_inn_keeper"] >= 20) and 
		(player.npc["tut_inn_keeper"] <= 25) and 
		((player.npc["tut_inn_keeper_special_offer_timer"] + 300) <= os.time()) and 
		(player:hasSpace(offer_item, 1, player.ID, ""..offer_engrave) == true) and 
		(player.money >= 200)) then
			table.insert(opts,"Special Offer")
			player.npc["tut_inn_keeper"] = 1
		end
		
	if(#opts~=0) then
		local menuOption=player:menuString(""..welcomemsg[rwelcome],opts)
			if (player.npc["tut_inn_keeper"] < 30) then
				player.npc["tut_inn_keeper"] = player.npc["tut_inn_keeper"] + 1
			elseif (player.npc["tut_inn_keeper"] >= 30) then
				player.npc["tut_inn_keeper"] = 1
			end
		if(menuOption=="Buy") then
			player:buyExtend(buymsg[rbuy],{170,169})
		elseif(menuOption=="Sell") then
			player:sellExtend(sellmsg[rsell],{0})
		elseif(menuOption=="Power Nap") then
			player:dialogSeq({t, "A power nap allows you to borrow one of our beds for a nap. Power naps are used by the strongest of adventurers to regain their health and mana."}, 1)
			local nap = player:menuString("Would you like to spend "..(player.level * 50).." gold to power nap?", {"Yes", "No"})
			if (nap == "Yes") then
				if (player:removeGold(player.level * 50)) then
					power_nap.cast(npc, player)
				end
			end
		elseif(menuOption=="Deposit Item") then
			player:showBankAdd("What do you wish to deposit?")
		elseif(menuOption=="Withdraw Item") then
			--player.registry["isBanking"] = 1
			player:showBank("What do you wish to withdraw?")
			--player.registry["isBanking"] = 0
		elseif(menuOption=="Deposit Money") then
			player:bankAddMoney()
		elseif(menuOption=="Withdraw Money") then
			player:bankWithdrawMoney()
		elseif(menuOption=="Post Office Services") then
			local item = player:getParcel()
			table.insert(optsPO,"Send")
			if (item ~= false) then
				table.insert(optsPO,"Receive")
				--table.insert(optsPO,"Receive List")
			end
			local menuOptionPO=player:menuString("Post Office Services",optsPO)
			if (menuOptionPO=="Send") then
				player:sendParcelTo(npc)
			elseif (menuOptionPO=="Receive") then
				player:receiveParcelFrom(npc)
			elseif (menuOptionPO=="Receive List") then
				player:receiveParcelFromList(npc)
			end
		elseif(menuOption=="Special Offer") then
			player:dialogSeq({t,"Right now we have a very unique offer for you.",
			t,"For a very small price you will have the chance to purchase the special offer.",
			t,"If you click next you will complete the purchase.",
			t,"You can pick it up at your closest post office."},1)
			npc:sendParcel(player.ID, npc.id, offer_item, 1, player.ID, ""..offer_engrave, 1)
			player:removeGold(200)
			player:sendStatus()
		end
	end
end),





action = function(npc, player)
	if (npc.mapRegistry["drunk_tavern"] > 0) and (npc.mapRegistry["drunk_tavern"] < 3) then
		npc.mapRegistry["drunk_tavern"] = npc.mapRegistry["drunk_tavern"] + 1
	elseif (npc.mapRegistry["drunk_tavern"] == 3) then
		npc:talk(0,"I will be with you in a minute.")
		npc.mapRegistry["drunk_tavern"] = 4
	elseif (npc.mapRegistry["drunk_tavern"] >= 4) and (npc.mapRegistry["drunk_tavern"] < 7) then
		npc.mapRegistry["drunk_tavern"] = npc.mapRegistry["drunk_tavern"] + 1
	elseif (npc.mapRegistry["drunk_tavern"] == 7) then
		npc:talk(0,"What can I say? He keeps my business running!")
		npc.mapRegistry["drunk_tavern"] = 0
	end
end
}