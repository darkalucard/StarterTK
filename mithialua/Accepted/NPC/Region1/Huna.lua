huna = {
click = async(function(player,npc)
	local t={graphic=convertGraphic(610,"monster"),color=31}
	local opts = {"Yes", "No"}
	
	player.npcGraphic=t.graphic
	player.npcColor=t.color
	player.dialogType = 0
	
	if(player.state~=1) then
		player:dialogSeq({t,"Nice skull you have, have you thought about donating it?"})
	else
		local menuOption=player:menuString("Even the best sometimes fall. Do you wish to be resurrected?",opts)
		
		if(menuOption=="Yes") then
			player.state = 0
			player.attacker = player.ID
			player:addHealthExtend(player.maxHealth, 0, 0, 0, 0, 0)
			player.magic = player.maxMagic
			player:updateState()
			player:addHealthExtend(1, 0, 0, 0, 0, 0)
			player:sendAnimation(96)
			player:playSound(112)
			player.registry["lastrez"] = os.time()
			npc:talk(0,"Huna: We shall meet again, " .. player.name.. ".")
		else
			player:dialogSeq({t,"Suit yourself, spirit. Come visit me anytime.",t,"Don't forget to submit your skull release form!"})
		end
	end
end),

say = function(player, npc)
	local speech = string.lower(player.speech)
	
	if ((string.find(speech, "(.*)res(.*)") or string.find(speech, "(.*)rez(.*)")) and player.state == 1) then
		player.state = 0
		player.attacker = player.ID
		player:addHealthExtend(player.maxHealth, 0, 0, 0, 0, 0)
		player.magic = player.maxMagic
		player:updateState()
		player:addHealthExtend(1, 0, 0, 0, 0, 0)
		player:sendAnimation(96)
		player:playSound(112)
		player.registry["lastrez"] = os.time()
		npc:talk(0, "Shaman: We shall meet again, ".. player.name..".")
	end
end
}