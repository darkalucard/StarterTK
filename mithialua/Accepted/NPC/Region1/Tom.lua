tom = {
click = async(function(player, npc)
	local t={graphic=0,color=0}

	player.npcGraphic = 0
	player.npcColor = 0
	player.dialogType = 1
	player:dialogSeq({t,"..Whaadddyaa wannt..?","I ..isssh... ..druunnk..\nGoooo awwaayaa","zzZ... zzzZ...",},1)
	npc.lastAction = os.time()
end),





action = function(npc, player)
	local rand = math.random(2079)
	
	if (npc.lastAction + 12 <= os.time()) then
		npc:sendAction(16,80)
		npc.lastAction = os.time()
	end

	if (rand <= 14) then
		npc:talk(0,"-hic- ..aaannotttherer..  ..rouund pleeeaasee.. -hic-")
		npc.mapRegistry["drunk_tavern"] = 1
		npc.lastAction = os.time()
	end
end,




say = async(function(player, npc)

	local speech = player.speech
	local lspeech = string.lower(speech)
	local t={graphic=convertGraphic(1,"item"),color=1}

	player.npcGraphic = 1
	player.npcColor = 1	

	if (lspeech == "hello tom") then
		--npc:talk(0,"Tom: herro")
	end
	
end)
}