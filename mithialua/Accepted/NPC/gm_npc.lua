gm_npc = {
f1click = function(player,npc)
	local optsgm = {}
	local gfx = {graphic = convertGraphic(654,"monster"),color=15}
	local menuOption
	local opts = {}
	
	player.npcGraphic = gfx.graphic
	player.npcColor = gfx.color
	player.dialogType = 0

	if(player.gmLevel == 99) then
		table.insert(optsgm, "Alter Stats")
		table.insert(optsgm, "Recognize bug tester")
		table.insert(optsgm, "Make a person GM")
	end
	
	if (player.gmLevel >= 1) then
		dialog="Welcome "..player.name..", how may I be of service?"
		
		table.insert(optsgm, "Obtain Char ID")
		table.insert(optsgm, "Where am I?")
		table.insert(optsgm, "Clear gfxToggle")
	else
		dialog="Hacker."
		if (player:checkOnline('mulghan') == 1) then
			player:msg(8, player.name.." attempting to hack GM functions.", 2)
		elseif (player:checkOnline('dirk') == 1) then
			player:msg(8, player.name.." attempting to hack GM functions.", 46)
		elseif (player:checkOnline('ghari') == 1) then
			player:msg(8, player.name.." attempting to hack GM functions.", 6)
		elseif (player:checkOnline('erebus') == 1) then
			player:msg(8, player.name.." attempting to hack GM functions.", 160)
		elseif (player:checkOnline('namash') == 1) then
			player:msg(8, player.name.." attempting to hack GM functions.", 208)
		end
	end

	menuOption = player:menuString(dialog,optsgm)

	if(menuOption=="Obtain Char ID" and player.gmLevel >= 50) then
		local charname
		charname=player:input("Whose do you need?")
		player:sendMinitext(""..getOfflineID(charname).."")
	elseif(menuOption=="Recognize bug tester" and player.gmLevel==99) then
		local bugtester = {}
		table.insert(bugtester,"Add Legend")
		table.insert(bugtester,"Remove Legend")
		local testerchoice=player:input("Hello, Who shall be affected? (Enter player name)")
		if(Player(testerchoice)==nil) then
			player:dialogSeq({t,"He is not online."})
			return
		end
		local testermenu=player:menuString2("Which applies?",bugtester)
		if(testermenu=="Add Legend" and Player(testerchoice)~=nil) then
			Player(testerchoice):removeLegendbyName("bugtester")
			Player(testerchoice):addLegend("Bug Hunter, blessed by the Chronicles of Mithia Staff","bugtester",10,9)
			Player(testerchoice):sendMinitext("You have been recognized by the Gods.")
			return
		elseif(testermenu=="Remove Legend" and Player(testerchoice)~=nil) then
			Player(testerchoice):removeLegendbyName("bugtester")
			Player(testerchoice):sendMinitext("The Gods have forsaken you.")
			return
		end
	elseif menuOption=="Where am I?" then
		player:sendMinitext("You are located at:")
		player:sendMinitext("Map: "..player.m.." \a X: "..player.x.." \a Y: "..player.y)
	elseif(menuOption=="Make a person GM" and player.gmLevel==99) then
		targetof=player:input("Who shall I make a GM?")
		levelof=player:input("What level of GM should "..targetof.." be?")
		Player(targetof).gmLevel = levelof * 1
		Player(targetof):sendMinitext("You are now a GM!")
		player:sendMinitext(Player(targetof).name .. " has been made a GM!")
	elseif(menuOption=="Alter Stats" and player.gmLevel==99) then
		targetof=player:input("Who shall have their stats altered?")
		stat=player:input("Which stat should "..targetof..
		" have changed?\n(Level, Vita, Mana, Might, Will, Grace, Exp, Regen, vRegen, mRegen, Skin, Gender, Title, armorDuraMod, weapDuraMod, reset)")
		lstat = string.lower(stat)
		if (lstat=="level")then
			quantity=player:input("Set "..stat.." to: ")
			Player(targetof).level = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="vita")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).baseHealth = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="mana")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).baseMagic = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="might")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).baseMight = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="will")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).baseWill = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="grace")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).baseGrace = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="exp")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).exp = quantity * 1
			Player(targetof):giveXP(1)
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="regen")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).regen = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="vregen")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).vRegen = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="mregen")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).mRegen = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="regentime")or(lstat=="regent")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).regenTime = quantity * 1
			Player(targetof):calcStat()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif(lstat=="skin")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).skinColor = quantity * 1
			Player(targetof):updateState()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif(lstat=="gender")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).sex = quantity * 1
			Player(targetof):updateState()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif(lstat=="title")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).title = quantity
			Player(targetof):updateState()
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="weapDuraMod")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).weapDuraMod = quantity * 1
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="armorDuraMod")then
			quantity=player:input("Set "..lstat.." to: ")
			Player(targetof).armorDuraMod = quantity * 1
			player:sendMinitext(Player(targetof).name .. "'s "..lstat.." has been set to "..quantity)
		elseif (lstat=="reset")then
			Player(targetof).exp = 0
			Player(targetof):giveXP(1)
			Player(targetof).baseMight = 1
			Player(targetof).baseGrace = 1
			Player(targetof).baseWill = 1
			Player(targetof).baseAC = 75
			Player(targetof).baseResist = 75
			Player(targetof).baseHealth = 100
			Player(targetof).baseMagic = 100
			Player(targetof).baseRegen = 0
			Player(targetof).baseVRegen = 0
			Player(targetof).baseMRegen = 0
			Player(targetof).baseMinDam = 0
			Player(targetof).baseMaxDam = 0
			Player(targetof).basePhysDeduct = 0
			Player(targetof).baseProtection = 0
			Player(targetof).baseSpeed = 80
			Player(targetof).weapDuraMod = 1
			Player(targetof).armorDuraMod = 1
			Player(targetof).registry["base_might"] = 0
			Player(targetof).registry["base_grace"] = 0
			Player(targetof).registry["base_will"] = 0
			Player(targetof).registry["base_ac"] = 0
			Player(targetof).registry["base_mr"] = 0
			Player(targetof).registry["base_vita"] = 0
			Player(targetof).registry["base_mana"] = 0
			Player(targetof).registry["base_regen"] = 0
			Player(targetof).registry["base_vregen"] = 0
			Player(targetof).registry["base_mregen"] = 0
			Player(targetof).registry["base_mindam"] = 0
			Player(targetof).registry["base_maxdam"] = 0
			Player(targetof).registry["base_pd"] = 0
			Player(targetof).registry["base_prot"] = 0
			Player(targetof).registry["base_speed"] = 0
			Player(targetof).registry["base_wdm"] = 0
			Player(targetof).registry["base_adm"] = 0
			Player(targetof).ap = 0
			Player(targetof).sp = 0
			Player(targetof).level = 1
			Player(targetof).class = 0
			Player(targetof):calcStat()
			Player(targetof).health = Player(targetof).maxHealth
			Player(targetof).magic = Player(targetof).maxMagic
			player:sendMinitext(Player(targetof).name .. " has been reset to level 1.")
		else
			player:sendMinitext("Error.")
		end
	elseif (menuOption == "Clear gfxToggle" and player.gmLevel == 99) then
		player:speak("/gfxtoggle", 0)
		player.gfxName = player.name
		player.gfxFace = player.face
		player.gfxFaceC = player.faceColor
		player.gfxHair = player.hair
		player.gfxHairC = player.hairColor
		player.gfxSkinC = player.skinColor
		player.gfxDye = player.armorColor
		player.gfxWeap = -1
		player.gfxWeapC = 0
		player.gfxArmor = player.sex
		player.gfxArmorC = 0
		player.gfxShield = -1
		player.gfxShieldC = 0
		player.gfxHelm = -1
		player.gfxHelmC = 0
		player.gfxCape = -1
		player.gfxCapeC = 0
		player.gfxCrown = -1
		player.gfxCrownC = 0
		player.gfxFaceA = -1
		player.gfxFaceAC = 0
		player.gfxFaceAT = -1
		player.gfxFaceATC = 0
		player.gfxNeck = -1
		player.gfxNeckC = 0
		player.gfxBoots = player.sex
		player.gfxBootsC = 0
		player:refresh()
	end
	return
end
}