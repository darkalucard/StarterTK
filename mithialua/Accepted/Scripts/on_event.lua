onKill = function(block, player)

	if (block.blType == BL_MOB) then
		local killTotal
		local total
		if (block.mobID == 111 or block.mobID == 112 or block.mobID == 113 or block.mobID == 114) then
		--r2_s_shepherd3 50 bandits repeatable
			total = 50
			for i = 1, #player.group do
				local killCount111 = Player(player.group[i]):killCount(111)
				local killCount112 = Player(player.group[i]):killCount(112)
				local killCount113 = Player(player.group[i]):killCount(113)
				local killCount114 = Player(player.group[i]):killCount(114)
				killTotal = killCount111 + killCount112 + killCount113 + killCount114
				if (Player(player.group[i]).state ~= 1 and Player(player.group[i]).m == player.m
				and Player(player.group[i]).afk == false
				and Player(player.group[i]).quest["banditQuest"] > 0 and Player(player.group[i]).quest["banditQuest"] < 100
				and (killTotal % 5) == 0) then
					onKillMsg(player, killTotal, total)
				end
			end
		elseif (block.mobID == 117 or block.mobID == 118 or block.mobID == 119 or block.mobID == 120 or block.mobID == 121) then
		--r2_s_chief zombie kills
			total = 1000
			for i = 1, #player.group do
				local killCount117 =  Player(player.group[i]):killCount(117)
				local killCount118 =  Player(player.group[i]):killCount(118)
				local killCount119 =  Player(player.group[i]):killCount(119)
				local killCount120 =  Player(player.group[i]):killCount(120)
				local killCount121 =  Player(player.group[i]):killCount(121)
				killTotal = killCount117 + killCount118 + killCount119 + killCount120 + killCount121
				if (Player(player.group[i]).state ~= 1 and Player(player.group[i]).m == player.m
				and Player(player.group[i]).afk == false
				and Player(player.group[i]).level >= 43
				and (killTotal % 5) == 0) then
					onKillMsg(Player(player.group[i]), killTotal, total)
				end
			end
		elseif (block.mobID == 66 or block.mobID == 67) then
		--r2_lj_hunter bear Quest
			for i = 1, #player.group do
				total = Player(player.group[i]).registry["bearQuestKillAmount"]
				local killCount66 = Player(player.group[i]):killCount(66)
				local killCount67 = Player(player.group[i]):killCount(67)
				killTotal = killCount66 + killCount67
				if (Player(player.group[i]).state ~= 1 and Player(player.group[i]).m == player.m
				and Player(player.group[i]).afk == false
				and Player(player.group[i]).quest["bearQuest"] > 0
				and (killTotal % 5) == 0) then
					onKillMsg(Player(player.group[i]), killTotal, total)
				end
			end
		elseif (block.mobID == 81 or block.mobID == 82 or block.mobID == 83 or block.mobID == 84) then
		--r2_lj_mercenary wolf Quest
			for i = 1, #player.group do
				total = Player(player.group[i]).registry["wolfQuestKillAmount"]
				local killCount81 = Player(player.group[i]):killCount(81)
				local killCount82 = Player(player.group[i]):killCount(82)
				local killCount83 = Player(player.group[i]):killCount(83)
				local killCount84 = Player(player.group[i]):killCount(84)
				killTotal = killCount81 + killCount82 + killCount83 + killCount84
				if (Player(player.group[i]).state ~= 1 and Player(player.group[i]).m == player.m
				and Player(player.group[i]).afk == false
				and Player(player.group[i]).quest["wolfQuest"] > 0
				and (killTotal % 5) == 0) then
					onKillMsg(Player(player.group[i]), killTotal, total)
				end
			end
		elseif (block.mobID == 2) then
		--tut_quest_girl squirrel quest
			for i = 1, #player.group do
				total = 10
				local killCount2 = Player(player.group[i]):killCount(2)
				killTotal = killCount2
				if (Player(player.group[i]).state ~= 1 and Player(player.group[i]).m == player.m
				and Player(player.group[i]).afk == false
				and Player(player.group[i]).quest["tut_quest_girl"] == 4) then
					onKillMsg(Player(player.group[i]), killTotal, total)
				end
			end
		elseif (block.mobID == 7) then
			for i = 1, #player.group do
				total = 6
				local killCount7 = Player(player.group[i]):killCount(7)
				killTotal = killCount7
				if (Player(player.group[i]).state ~= 1 and Player(player.group[i]).m == player.m
				and Player(player.group[i]).afk == false
				and Player(player.group[i]).quest["tut_quest_bird_worshipper"] == 3) then
					onKillMsg(Player(player.group[i]), killTotal, total)
				end
			end
		end
	elseif (block.blType == BL_PC) then
	end
end

onKillMsg = function(player, killTotal, total)
	if (player.registry["questCount"] == 0) then
		player:msg(4, player.f1Name..'(Spirit of Genesis)" '.."You have killed "..killTotal.."/"..total..". ((Toggle on F1))", player.ID)
	end
end

onLevel = function(player)
	local primary = math.ceil(((player.level + 1) * 1.325))
	local secondary = math.floor((primary * .665))
	local tertiary = math.ceil((secondary * .48))

	--[[if (player.level >= 50) then
		return
	end
	
	if (player.class == 0) then
		if (player.level == 4) then
			player:msg(8, "You have reached level 5, you must now choose a path to continue.", player.ID)
		elseif (player.level >= 5) then
			player:sendMinitext("You cannot increase your level without choosing a path first.")
			return
		end
	elseif (player.class < 5) then
		if (player.level == 24) then
			player:msg(8, "You have reached level 25, you must now choose a subpath to continue.", player.ID)
		elseif (player.level >= 25) then
			player:sendMinitext("You cannot increase your level without choosing a subpath first.")
			return
		end
	end

	player.baseMagic = player.baseMagic + (60 + math.random(15))
	player.baseHealth = player.baseHealth + (60 + math.random(15))
	
	if (player.registry["base_might"] > 0) then
		player.baseHealth = player.baseHealth - (player.registry["base_might"] * 20)
		player.registry["base_might"] = 0
	end
	
	if (player.registry["base_grace"] > 0) then
		player.baseHealth = player.baseHealth - (player.registry["base_grace"] * 10)
		player.baseMagic = player.baseMagic - (player.registry["base_grace"] * 10)
		player.registry["base_grace"] = 0
	end
	
	if (player.registry["base_will"] > 0) then
		player.baseMagic = player.baseMagic - (player.registry["base_will"] * 20)
		player.registry["base_will"] = 0
	end
	
	if (player.baseClass == 1) then
		player.baseMight = primary
		player.baseGrace = secondary
		player.baseWill = tertiary
	elseif (player.baseClass == 2) then
		player.baseMight = tertiary
		player.baseGrace = primary
		player.baseWill = secondary
	elseif (player.baseClass == 3) then
		player.baseMight = tertiary
		player.baseGrace = secondary
		player.baseWill = primary
	elseif (player.baseClass == 4) then
		player.baseMight = secondary
		player.baseGrace = tertiary
		player.baseWill = primary
	elseif (player.baseClass == 0) then
		player.baseMight = math.ceil(player.level / 2)
		player.baseGrace = math.ceil(player.level / 2)
		player.baseWill = math.ceil(player.level / 2)
	end
	
	player.level = player.level + 1
	player.baseAC = 0
	player:calcStat()
	player.health = player.maxHealth
	player.magic = player.maxMagic
	player:sendStatus()
	player:sendAnimation(253, 0)]]--
end

onPickUp = function(player,item)
	if (player.pickUpType == 1) then--< Big pick up all.
		--player:talk(0,"Picked up "..item.amount.." "..Item(item.id).name.."("..item.id..")")
	else
		--player:talk(0,"Picked up 1 "..Item(item.id).name.."("..item.id..")")
	end

	if (Item(item.id).type ~= 20) then
		player:pickUp(item.ID)--Back to Core
	end
end

onDrop = function(player,item)
	--player:talk(0,"Dropped: "..Item(item.id).name.." ID: "..item.id.." Item amount: "..item.amount - item.lastAmount)
end

onEquip = function(player,item)
	--player:talk(0,"Equipped "..player.equipID)
	
	player:equip()
end

onUnequip = function(player,item)
	--player:talk(0,"Unequipped "..player:getEquippedItem(takeOffID).id)
	
	player:takeOff()
end

onDeathPlayer = function(player)
	--player:talk(0,"wew died!")
	if (player.registry["immortal"] == 1) then
		return
	elseif (player.registry["immortal"] == 2) then--from decoy
		player.registry["immortal"]	= 0
		return
	end
	
	player.deathFlag = 1
end

onCast = function(player)
	if (player.confused) then
		local randtargetpc = player:getObjectsInArea(player.m, player.x, player.y, BL_PC)
		local randtargetmob = player:getObjectsInArea(player.m, player.x, player.y, BL_MOB)
		local rand = math.random(100)
		
		if (rand <= 50) then
			player.target = randtargetpc[math.random(#randtargetpc)].ID
		elseif (rand > 50) then
			player.target = randtargetmob[math.random(#randtargetmob)].ID
		end
		
		player:sendMinitext("Your target has changed due to confusion!")
	end
	--player:talk(0,"Casted")
end

onBreak = function(player)
	--player:talk(0,"Broken "..player.breakID)
end

onThrow = function(player,item)
	--local y = player:getInventoryItem(player.invSlot) --returns inventory slot
	--player:talk(0,"Slot: "..player.invSlot.." Item returned "..y.name)
	player:throwItem()
end

onAction = function(player)
	
end

onSign = function(player, signType)
	local objFacing = getObjFacing(player)
	local m = player.m
	local x = player.x
	local y = player.y
	local side = player.side
	local str = ""

	if (signType == 1) then--onLook
		if (m == 40) then
			if (objFacing == 15514 or objFacing == 15515) then
				if (x >= 100 and x <= 103 and (y == 14 or y == 15)) then--Rat Cave sign
					str = "<b>DANGER!\n\n"
					str = str.."Rat infestation"
					player:popUp(str)
				end
			end
		end
	elseif (signType == 2) then--onScriptedTile
		if (m == 40) then
			if ((x == 100 and y == 14 and side == 1) or
			((x == 101 or x == 102) and y == 15 and side == 0) or
			(x == 103 and y == 14 and side == 3)) then--Rat Cave sign
				str = "<b>DANGER!\n\n"
				str = str.."Rat infestation"
				player:popUp(str)
			elseif ((x == 114 or x == 115) and y == 44 and side == 0) then--The Mithian Edict
				player:showBoard(256)
			end
		end
	elseif (signType == 3) then--onTurn
		if (m == 40) then
			if ((x == 114 or x == 115) and y == 44 and side == 0) then--The Mithian Edict
				player:showBoard(256)
			end
		end
	end
end

onLook = function(player, block)
	onSign(player, 1)
	player:lookAt(block.ID)
	
	--Check to avoid double printing for GMs
	if (player:staff("script") >= 3 and player:staff("pm") == -1 and player.gmLevel < 50) then
		--BL_PC, BL_MOB, BL_NPC, BL_ITEM
		local str = ""
		if (block.blType == BL_PC) then
			str = ""..block.classNameMark.." | "..block.name.." ("..string.format("%.2f", (block.health / block.maxHealth * 100)).."%) ID: "..block.id
			player:msg(0, ""..str, player.id)
		elseif (block.blType == BL_MOB) then
			str = ""..block.name.." | "..block.yname.." ("..string.format("%.2f", (block.health / block.maxHealth * 100)).."%) id: "..block.id.." ID: "..block.ID.." mobID: "..block.mobID
			player:msg(0, ""..str, player.id)
		elseif (block.blType == BL_NPC) then
			str = ""..block.name.." | "..block.yname.." "..block.id
			player:msg(0, ""..str, player.id)
		elseif (block.blType == BL_ITEM) then
			str = ""..block.name.." | "..block.yname.." "..block.id
			player:msg(0, ""..str, player.id)
		end
	end
end

onDismount = function(player)
	player.registry["mount_type"] = 0
	player.disguise = 0
	player.state = 0
	player:calcStat()
	player:updateState()
end

remount = function(player)
	local mount = player.registry["mount_type"]
	
	if (mount > 0 and player.state == 3) then
		if (mount == 1) then
			player.speed = player.speed - 20
		elseif (mount == 2) then
			player.speed = player.speed - 25
		end
		
		player:updateState()
	else
		player.registry["mount_type"] = 0
	end
end

onDeathMob = function(mob)
	local mobBlocks = mob:getObjectsInArea(BL_MOB)
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mobBlocks[i].target == mob.ID and mobBlocks[i].owner > 0) then
				mobBlocks[i].target = mobBlocks[i].owner
			end
		end
	end
end
