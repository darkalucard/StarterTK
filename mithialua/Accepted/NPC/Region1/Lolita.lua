lolita = {
click = async(function(player,npc)
	local t={graphic=convertGraphic(607,"monster"),color=14}
	local opts = { }
	local optsbuy ={ }
	local optstalk = {}
	local menuOption
	local menuOptionBuy
	local lolita = player.npc["lolita"]
	local gender = player.sex
	local spouse = player.spouse --if spouse == 0 not married.
	local buyTable = {}
	
	player.npcGraphic=t.graphic
	player.npcColor=t.color
	player.dialogType = 0

	if (lolita==0) then
		player.npc["lolita"] = 1
	end
	
	table.insert(opts,"Buy")
	table.insert(opts,"Sell")
	table.insert(optsbuy,"Clothes")
	if(#opts~=0) then
		if (gender == 0) then
			buyTable = {200000000}
			if (lolita==1) then
				menuOption=player:menuString("Hey sexy lad, come closer I've got something for you.",opts)
				player.npc["lolita"] = 2
			elseif (lolita==3) then
				menuOption=player:menuString("Hey, "..player.name..", mind if I call you by name? What sort of services do you need from me?",opts)
				player.npc["lolita"] = 4
			elseif (lolita==4) then
				menuOption=player:menuString("Hey, "..player.name..", looking good today. How may I -serve- you?",opts)
				player:sendMinitext("Lolita winks at you.")
			else
				menuOption=player:menuString("Hey, looking good today.",opts)
			end
		elseif (gender == 1) then
			buyTable = {200000001}
			if (lolita == 1) then
				menuOption=player:menuString("Yo girl, who said age is a factor in sexiness?",opts)
				player.npc["lolita"] = 2
			elseif (lolita == 3) then
				menuOption=player:menuString("Welcome back, "..player.name..". I've got everything you'll need to stay in fashion until you hit my age! Er...",opts)
				player.npc["lolita"] = 4
			elseif (lolita ==4) then
				menuOption=player:menuString("Welcome back, "..player.name..", how may I help you?",opts)
			else
				menuOption=player:menuString("Hey, looking good today.",opts)
			end
		else
			menuOption=player:menuString("What kind of creature are you?",opts)
		end
		
		if(menuOption=="Buy") then
			menuOptionBuy=player:menuString("Don't be misguided by my dress, I was working with some dyes and accidentally spilled some on me. What would you like to buy?",optsbuy)
			if(menuOptionBuy=="Clothes") then
				player:buyExtend("What do you wish to buy?",buyTable)
			end
		elseif(menuOption=="Sell") then
			player:sellExtend("What do you wish to sell?",{200000000, 200000001})
		end
	end
end)
}