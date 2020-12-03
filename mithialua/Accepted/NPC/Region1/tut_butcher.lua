tut_butcher = {
click = async(function(player,npc)
	local i = 0
	local t={graphic=convertGraphic(609,"monster"),color=0}
	local opts = { }
	local optsbuy ={ }
	local menuOption
	local menuOptionBuy
	local sander = player.npc["tut_butcher"]
	local welcomemsg = {}
	local rwm
	local talkmsg = {}
	local rtm
	local buyitems = {121,122,176}
	local buyamts = {1,1}
	local sellitems = {121,122,120,123,124}
	
	player.npcGraphic=t.graphic
	player.npcColor=t.color
	player.dialogType = 0
	
	if ((sander <= 3) or (sander >= 5))then
		player.npc["tut_butcher"] = sander + 1
	end
	
	if (sander>=2) then
		table.insert(opts,"Talk")
	end
	
	table.insert(opts,"Buy")
	table.insert(opts,"Sell")
	table.insert(welcomemsg,"Welcome to my shop. We're low on stock.")
	table.insert(welcomemsg,"Welcome to my shop.")
	table.insert(welcomemsg,"Welcome to my shop. I'm spanking it to make it tender!")
	table.insert(talkmsg,"")
	
	if(#opts~=0) then
		if (sander >= 5) then
			table.insert(welcomemsg,"Would you like to see me spank the cock?")
			
		end
		
		rwm = math.random(#welcomemsg)
		
		menuOption=player:menuString(welcomemsg[rwm],opts)
		
		if(menuOption=="Talk") then
			--rtm = math.random(#talkmsg)
			--menuOption=player:menuString(talkmsg[rtm],opts)
			player:dialogSeq({t,"Ever since Todd's father left I have had to take care of Todd.",
			t,"It's not easy, having Lolita around trying to corrupt Todd's innocence."},1)
			if (sander<=5) then
				player.npc["tut_butcher"] = 5
			end
		elseif(menuOption=="Buy") then
			local concat1 = player.mapRegistry["tut_butcher_item_121"]
			local concat2 = player.mapRegistry["tut_butcher_item_122"]
			if (concat1 <= 0) then
				player.mapRegistry["tut_butcher_item_121"] = 1
			end
			if (concat2 <= 0) then
				player.mapRegistry["tut_butcher_item_122"] = 1
			end
			if (concat1 < 2) then
				table.remove(buyitems, 1)
				table.remove(buyamts, 1)
			end
			if (#buyitems > 1) then
				if (concat2 < 2) then
					table.remove(buyitems, 2)
					table.remove(buyamts, 2)
				end
			else
				if (concat2 < 2) then
					table.remove(buyitems, 1)
					table.remove(buyamts, 1)
				end
			end
			
			if ((player.mapRegistry["tut_butcher_item_121"] == 1) and (player.mapRegistry["tut_butcher_item_122"] == 1)) then
				player:dialog("Nothing left for sale at the moment. Come back later",t)
			end
			
			local buytable = player:buyExtend("We are very low on stock.",buyitems,{},buyamts)
			if (buytable ~= nil) then
				concat1 = player.mapRegistry["tut_butcher_item_"..buytable[1]]
				player.mapRegistry["tut_butcher_item_"..buytable[1]] =  concat1 - buytable[2]
			end
		elseif(menuOption=="Sell") then
			local sellReg
			local selltable = player:sellExtend("What do you wish to sell?",sellitems)
			if (selltable ~= nil) then
				local concat = "tut_butcher_item_"..selltable[1]
				sellReg = player.mapRegistry[concat]
				if (math.random(10) < 8) then
					player.mapRegistry[concat] =  sellReg + selltable[2]
				--else
				end
			end
		end
	end
end),


action = function(npc)
	local buyitems = {121,122}
	local itemmax = {20,5}
	local item1 = npc.mapRegistry["tut_butcher_item_"..buyitems[1]]
	local item2 = npc.mapRegistry["tut_butcher_item_"..buyitems[2]]
	if (item1 <= itemmax[1]) then
		if (math.random(100) > 25) then
			npc.mapRegistry["tut_butcher_item_"..buyitems[1]] = item1 + 1
		end
	end
	if (item1 <= itemmax[2]) then
		if (math.random(100) > 75) then
			npc.mapRegistry["tut_butcher_item_"..buyitems[2]] = item2 + 1
		end
	end
end
}