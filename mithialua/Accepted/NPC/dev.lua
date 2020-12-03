dev = {
click = async(function(player,npc)

end),

f1click = function(player, npc)
	local menuOption
	local g = {graphic = convertGraphic(39, "monster"), color = 16}
	player.npcGraphic = g.graphic
	player.npcColor = g.color

	local opts = {}
	
	if ((player:staff("pm") > 0) or (player:staff("map") > 0) or (player:staff("lore") > 0) or (player:staff("script") > 0)) then
		table.insert(opts,"Meeting room")
		table.insert(opts,"Show Meetings board")
	end
	
	if (player:staff("pm") > 0) then
		table.insert(opts,"Show Dev board")
		if (player.gmLevel > 50) then
			table.insert(opts,"+ Dev spells")
		end
	end
	
	if (player:staff("lore") > 0) then
		table.insert(opts,"Show Lore board")
	end
	if (player:staff("map") > 0) then
		table.insert(opts,"Show Map board")
		table.insert(opts,"+ Mapper Spells")
	end
	
	if (player:staff()) then
		table.insert(opts,"Clear gfxToggle")
	end

	if (player:staff("script") > 0) then
		table.insert(opts,"Show Script board")
		if (player.gmLevel >= 50) or (player:staff("script") >= 3) then
			table.insert(opts,"Reload")
		end
		table.insert(opts,"Debugger")
	end
	
	if (player.gmLevel >= 50) then
	--[[
		table.insert(opts, "Warrior 50 items")
		table.insert(opts, "Rogue 50 items")
		table.insert(opts, "Mage 50 items")
		table.insert(opts, "Poet 50 items")
	]]--
	end
	
	
	if(#opts~=0) then
		menuOption=player:menuString("Greetings "..player.name..", how may I assist you?",opts)
		if (menuOption == "Meeting room") then
			if (player.m ~= 10010) and (player.m ~= 10002) then
				player.registry["temp_map"] = player.m
				player.registry["temp_x"] = player.x
				player.registry["temp_y"] = player.y
				player:warp(10010,8,7)
			elseif (player.m == 10010) then
				player:warp(player.registry["temp_map"],player.registry["temp_x"],player.registry["temp_y"])
			end
		elseif (menuOption == "Debugger") then
			if (player.m == 10002) then
				player:warp(10010,8,7)
			elseif (player.m == 10010) then
				player:warp(10002,math.random(191,198),math.random(191,198))
			else
				player:sendMinitext("Must be in the meeting room to use this option")
			end
			
		elseif (menuOption == "Show Meetings board") then
			player:showBoard(268)
		elseif (menuOption == "Show Dev board") then
			player:showBoard(269)
		elseif (menuOption == "+ Dev spells") then
			player:addSpell("kill")
			player:addSpell("kill_summons")
			player:addSpell("possess")
			player:addSpell("control")
			player:addSpell("ping_user")
			player:addSpell("info")
			player:addSpell("summon")
			player:addSpell("approach")
			player:addSpell("gm_do")
			player:addSpell("mass_res")
			player:addSpell("mass_summon")
			player:addSpell("where")
			player:addSpell("gm_dispell")
			player:addSpell("gm_aether")
			player:addSpell("room_hellfire")
			player:addSpell("decoy")	
			player:addSpell("sneak")
			player:addSpell("spot_traps")
			player:addSpell("npc_approach")
			player:addSpell("npc_summon")
		elseif (menuOption == "Show Lore board") then
			player:showBoard(270)
		elseif (menuOption == "Show Map board") then
			player:showBoard(271)
		elseif (menuOption == "+ Mapper Spells") then
			player:addSpell("direction_warp")
		elseif (menuOption == "Show Script board") then
			player:showBoard(272)
		elseif (menuOption == "Reload") then
			os.execute("cd ../mithiamaps;svn up;cd ../mithialua; svn up; cd ../mithia; svn up")
			local errors = luaReload()
			local errorString = ""
			
			if (#errors > 0) then
				for i = 1, #errors do
					errorString = errors[i].."\n"..errorString
				end
				
				player:sendMail(player.name, "Lua Error", errorString)
			end
			
			player:gmMsg("<Console>: "..player.name.." has svn up and reloaded the server.", 50)
			os.execute("echo "..player.name.." has SVN UP and RELOADED server.")
			addChatLog(player.ID, "I, "..player.name..", have reloaded server.")
			player:refresh()
		elseif (menuOption == "Clear gfxToggle") then
			--player:speak("/gfxtoggle", 0)
			if (player.gfxClone == 0) then
				player.gfxClone = 1
			else
				player.gfxClone = 0
			end
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
		elseif (menuOption == "Warrior 50 items") then
			player:addItem("celler_glove", 2)
			player:addItem("subterranian_helm", 1)
			player:addItem("superior_solar_platemail", 1)
			player:addItem("balanced_darksteel_sword", 1)
			player:addItem("studded_boots", 1)
		elseif (menuOption == "Rogue 50 items") then
			player:addItem("enraged_dwellers_bone", 2)
			player:addItem("forgotten_helm", 1)
			player:addItem("superior_solar_armor", 1)
			player:addItem("sharp_gnarly_branch", 1)
			player:addItem("studded_boots", 1)
		elseif (menuOption == "Mage 50 items") then
			player:addItem("enchanted_dweller_skull", 2)
			player:addItem("braided_wrappings", 1)
			player:addItem("superior_solar_clothes", 1)
			player:addItem("enchanted_meditation_staff", 1)
			player:addItem("studded_boots", 1)
		elseif (menuOption == "Poet 50 items") then
			player:addItem("mysterious_text", 2)
			player:addItem("ancient_cap", 1)
			player:addItem("superior_solar_mantle", 1)
			player:addItem("fine_deadly_blade_fan", 1)
			player:addItem("studded_boots", 1)
		end
	else
	--alert hacker
	end
end
}