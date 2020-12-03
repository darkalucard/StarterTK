god_of_time = {
click = async(function(player,npc)
	local npcG = {graphic = 0, color = 0}
	local menuOptions = {}
	
	player.npcGraphic = npcG.graphic
	player.npcColor = npcG.color
	player.dialogType = 1--what's this?
	
	if (player.state == 1) then
		player.state = 0
		player.attacker = player.ID
		player:addHealthExtend(player.maxHealth, 0, 0, 0, 0, 0)
		player.magic = player.maxMagic
		player:updateState()
		player:addHealthExtend(1, 0, 0, 0, 0, 0)
		player:sendAnimation(96)
		player:playSound(112)
		player:flushAether(5)
	end
	--[[
	local spells = {"learned_slaughter", "learned_throw_hatchet", "learned_knockout_blow", "learned_insanity",
	"learned_ancient_protection", "learned_thick_skin", "learned_guardians_stance", "learned_wardens_scream",
	"learned_overpower", "learned_regenerate", "learned_reflective_shield", "learned_guillotine",
	"learned_concussive_blow", "learned_throw_axe", "learned_champions_stance", "learned_bellow", "learned_intervene",
	"learned_muscle", "learned_warriors_rage", "learned_shout", "learned_leap", "learned_pound", "learned_renew",
	"learned_taunt", "learned_throw_stone", "learned_bind", "learned_gateway", "learned_light", "learned_fighters_rage",
	"learned_fighters_stance", "learned_champions_rage", "learned_blood_boil", "learned_knights_horn", "learned_helping_hand",
	"learned_relinquish", "learned_decimate", "learned_slam", "learned_warriors_stance", "learned_defenders_stance", "learned_protectors_stance"}
	]]--

--[[This didn't fix SirWozzle's spells can't be learned...
	local spells = player:getSpells()
	for i = 1, #spells do
		player:removeSpell(spells[i])
	end
	player.quest["innerStrength"] = 0
	repeat
		player:removeLegendbyName("subpath")
	until (player:hasLegend("subpath") == false)
	player:addLegend("Great protector. "..curT(), "subpath", 16, 80)
	player.class = 7
	player:status()
	npc:talk(0, ""..npc.name..": "..player.name.." has been set back in time.")

	--player:talk(0, ""..player.name.." stop clicking me.")
	]]--
end),


say = async(function(player, npc)
	local speech = player.speech
	local lspeech = string.lower(speech)
	local npcTable = {}
	local mobTable = {}
	
	--[[
	if (player.id == 25 and player.name == "Krisalia") then
		--npc:talk(0,""..lspeech)
	
		if (lspeech == "npcid") then
			npcTable = npc:getObjectsInArea(BL_NPC)
			if (#npcTable > 0) then
				for i = 1, #npcTable do
					npcTable[i]:talk(0,"Name: "..npcTable[i].name.." yname: "..npcTable[i].yname.." id: "..npcTable[i].id)
				end
			else
				npc:talk(0,"No NPCs in range")			
			end
		elseif (lspeech == "mobid") then
			mobTable = npc:getObjectsInArea(BL_MOB)
			if (#mobTable > 0) then
				for i = 1, #mobTable do
					mobTable[i]:talk(0,"mobID: "..mobTable[i].mobID.." id: "..mobTable[i].id.." ID: "..mobTable[i].ID)
				end
			else
				npc:talk(0,"No mobs in range")
			end
		end
		if (lspeech == "mulghan") then
			npc:talk(0,"Mulghan PM: "..Player(2):staff("pm").." Mulghan Script: "..Player(2):staff("script"))
		elseif (lspeech == "ghari") then
			Player(6).disguise = 1
			Player(6).state = 0
			Player(6):updateState()
		end
		if (lspeech == "npc6") then
			player.state = 4
			player:updateState()
			player.disguise = 6
			player.disguiseColor = 12
			player:updateState()
		elseif (lspeech == "npc77") then
			player.state = 4
			player:updateState()
			player.disguise = 77
			player.disguiseColor = 0
			player:updateState()
		elseif (lspeech == "npc226") then
			player.state = 4
			player:updateState()
			player.disguise = 226
			player.disguiseColor = 0
			player:updateState()
		elseif (lspeech == "npcplayer") then
			player.disguise = 1
			player.disguiseColor = 1
			player.state = 0
			player:updateState()
		elseif (lspeech == "state-") then
			player.state = -1
			player:updateState()
		elseif (lspeech == "state0") then
			player.state = 0
			player:updateState()
		elseif (lspeech == "state1") then
			player.state = 1
			player:updateState()
		elseif (lspeech == "state2") then
			player.state = 2
			player:updateState()
		end
	else
	end]]--
end)
}