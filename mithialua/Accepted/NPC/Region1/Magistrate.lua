magistrate = {
click = async(function(player,npc)
	local t={graphic=0,color=0}
	local opts = {"Who are you?", "Become an official candidate."}
	
	player.npcGraphic = 0
	player.npcColor = 0
	player.dialogType = 1

	dialog="Welcome "..player.name..", how may I be of service?"
	--[[menuOption = player:menuString(dialog,opts)

	if (menuOption == "Who are you?") then]]--
		if ((npc.lastAction + 30) <= os.time()) then
		player:dialogSeq({t,"zzZ... zzzZ...","(Better wake him up if you want him to answer...)\n\rTry asking him to: 'wake up!'"},0)
		end
		player:dialogSeq({t,"My name is Roll, I am a Magistrate of -insert Kingdom name-. I have been a Magistrate since the previous ruler.",
		"Magistrates are civil officers in charge of the administration of the laws.",
		"We are somewhat similar to Constables, but at the same time different. We serve different functions than Constables.",
		"In my case I serve as the Electoral Magistrate, also known as 'EM'.",
		"My main role as an EM is to overview mayor elections in these regions.",
		"I will assist future candidates into enrolling and applying to be an official candidate for several positions.",
		"First I have to make sure those that apply comply with the electability required for the position.",
		"Then I must register the electioneers for said person and the electioneers must come and register within a period of time.",
		"The timeframe given for electioneers to register is based upon the position applied.",
		"There might also be a minimum requirement of electioneers based on the position.",
		"Once all electioneers have been properly registered then the person that applied becomes a candidate.",
		"Then the candidate will be available in the voting polls starting on the next election period.",
		"(Not the next elections, but the one next to it.)",
		"Once the week of the election the candidate will partake in begins both the candidate and the electioneers will be given assigned powers.",
		"These temporary grants will be given in order to aid them with their campaign.",
		"Any abuse of these temporary powers will be deemed as a very serious offense and will result in a criminal offense, followed by a prosecution.",
		"On the election day access to these powers will be revoked as campaigning during the election day is\n\r\n\r<b>       **FORBIDDEN**.",
		"Candidates may not be the current holder of a position nor can they be the previous to which they apply.",
		"Electioneers may not be the current holder of the position seeked during the campaign period.",
		"If the candidate is dissatisfied with one of their electioneers they may remove the electioneer from their list so long as they still comply with the minimum requirements.",
		"Once a campaign period has started a candidate may not include any additional electioneers.",
		"On the election day all electants that wish to partake in the election process must do so during the given timeframe or their vote will be counted as null.",
		"Electants may not change their voting choice and all votes will remain anonymous.",
		"At the end of the election day the electee candidate will be chosen based on the number of votes.",
		"The electee candidate will become an elected official once the Magistrates are done verifying there was no election fraud.",
		"If any sort of election fraud is found, Constables will be dispatched to imprison the criminals upon future trial.",
		"If the fraud is severe to the point that the election can not be concluded, then the Magistrates will decide what will happen next.",
		"Election fraud is considered as a very serious criminal offense which will grant the heavy arm of the law vested upon the criminals.",
		"If the election continues normally or is granted by an EM that it proceeds, the elected official will then be appointed by an EM.",
		"Failure to fulfill duties they were elected for would result in a trial with an EM.",
		"That pretty much sums up who I am and what I do.",
		"Would you be interested in knowing some more?",
		"You could ask more about \"becoming an official candidate\",\n\"Available positions\",\n\"Etc...\".",
		"LAST!"},1)
		npc.lastAction = os.time()
	--[[	
	elseif (menuOption == "Become an offical candidate.") then
	
	end]]--

	
end),





say = async(function(player, npc)

	local speech = player.speech
	local lspeech = string.lower(speech)
	local t={graphic=convertGraphic(1,"item"),color=1}

	player.npcGraphic = 1
	player.npcColor = 1

	if (string.lower(lspeech) == "becoming an official candidate") then
		player:dialog("<b>You do not meet the\n<b>minimum requirements yet.",t)
	elseif (string.lower(lspeech) == "available positions") then
		player:dialog("Open positions are mayor, etc...",t)
	elseif (string.lower(lspeech) == "etc") then
		player:dialog("Etc... to you too...",t)
	elseif (string.lower(lspeech) == "repair all") then
		npc:talk(0,"Magistrate: Do I look like a SMITH to you?")
	elseif (string.lower(lspeech) == "wake up!") then
		if ((npc.lastAction + 30) <= os.time()) then
			npc.registry["tempo"] = player.ID
			npc.registry["awoken"] = 1
			npc.registry["wake1"] = 1
			npc.registry["tempt"] = os.time()
			npc.lastAction = os.time()
		end
	--elseif (lspeech == "input") then
		--local strput
		--strput=player:input("Input works?")
		--player:talk(0,""..type(strput))
		--player:talk(0,""..strput)
	end
end),




action = function(npc)
	local n --/lua Player(3).registry["test"] = 1 Player(2).registry["test"] = 1
	n = {2,3,23,32,36,37}
	for x = 1, #n do
		if (checkOnline(n[x]) == 1) then
			local qnx = Player(n[x])
			if (qnx.registry["test"] == 1) then
				qnx:sendAction(27,150)
				qnx.registry["test"] = os.time()
			end
			if (qnx.registry["test"] > 1) then
				if (((qnx.registry["test"] + 4) < os.time()) and (qnx.registry["timecheck"] == 1)) then
					qnx:warp(qnx.m,qnx.x+1,qnx.y)
					qnx:warp(qnx.m,qnx.x-1,qnx.y)
					--qnx:refresh()
					qnx:sendAction(6,60)
					qnx.registry["test"] = 0
					qnx.registry["timecheck"] = 0
				else
					qnx.registry["timecheck"] = 1
				end
			end
		end
	end
	--vvvvv OTHER STUFF NOT FROM THIS NPC vvvvv
	for x = 1, #n do--/lua Player(2).registry["test_move"] = 1
		if (checkOnline(n[x]) == 1) then
			local qnx = Player(n[x])
			if ((qnx.registry["test_move"] < 7) and (qnx.registry["test_move"] > 0)) then
				qnx:warp(qnx.m, qnx.x +1, qnx.y)
				qnx.registry["test_move"] = qnx.registry["test_move"] + 1
			else
				if (qnx.registry["test_move"] == 8) then
					qnx:warp(qnx.m, qnx.x - 6, qnx.y)
					qnx.registry["test_move"] = 0
				else
					if (qnx.registry["test_move"] > 0) then
						qnx.registry["test_move"] = qnx.registry["test_move"] + 1
						qnx:sendAction(1,14)
					end
				end
			end
		end
	end
	--^^^^^ OTHER STUFF NOT FROM THIS NPC ^^^^^
	if (npc.registry["timer"] == 12) then
		if ((npc.lastAction + 30) <= os.time()) then
			npc:sendAction(16,80)
		end
		npc.registry["timer"] = 0
	end
	npc.registry["timer"] = npc.registry["timer"] + 1
	if (npc.registry["awoken"] == 1) then
		tempo = npc.registry["tempo"]
		tempt = npc.registry["tempt"]
		--local x = Player(tempo)
		local h = {}
		h = npc:getObjectsInArea(BL_PC)
		if (npc.registry["wake1"] == 1) then
			if ((tempt + 2) <= os.time()) then
				npc:talk(0,"Roll: Uh oh...")
				npc.registry["wake1"] = 2
			end
		elseif (npc.registry["wake1"] == 2) then
			if ((tempt + 3) <= os.time()) then
				--for i = 1,#h do
					npc:talk(0,"~Roll clears drool off his face.~")
				--end
				npc.registry["wake1"] = 3
			end
		elseif (npc.registry["wake1"] == 3) then
			if ((tempt + 6) <= os.time()) then
				for i = 1,#h do
					if (h[i].ID == Player(tempo).ID) then
						npc:msg(0,"Roll(Magistrate)\" Did you need something?",h[i].ID)
					end
				end
				npc.registry["awoken"] = 0
				npc.registry["wake1"] = 0
				npc.registry["tempo"] = 0
				npc.lastAction = os.time()
			end
		end
	end
end
}