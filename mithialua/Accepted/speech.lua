onSay = function(player)
	local printf = 1
	local caps = 0
	local speech = player.speech
	local p = player
	
	if (speech == "") then
		return
	end
	
	local lspeech = string.lower(player.speech)
	--local light = "utevo lux"
	local length = string.len(lspeech)
	local online = {}
	local talkType = player.talkType
	online = player:getUsers()
	--GM Stuff, Commands
	if (player.gmLevel >= 20) then
		if (lspeech=="gmt") then
			if (player.registry["gm_talk"]== 0) then
				player.registry["gm_talk"] = 1
				player:sendMinitext("GM Chat on.")
			else
				player.registry["gm_talk"] = 0
				player:sendMinitext("GM Chat off.")
			end
			printf = 0
		elseif (player.registry["gm_talk"]== 1) then
			if (string.byte(lspeech,1) == 47) then
				--String starting with "/"
			else
				player.speech = "/gm "..speech..""
			end
		end
	end
	if (player.gmLevel >= 50) then
		--GM commands
		if (lspeech == "reload") or (lspeech == "/reload") then
			os.execute("cd ../mithiamaps;svn up;cd ../mithialua; svn up; cd ../mithia; svn up")
			player.speech = "/reload"
			player:gmMsg("<Console>: "..player.name.." has svn up and reloaded the server.", 50)
			os.execute("echo "..player.name.." has SVN UP and RELOADED server.")
		elseif (lspeech == "svn up") then
			os.execute("cd ../mithiamaps;svn up;cd ../mithialua; svn up; cd ../mithia; svn up")
			player:gmMsg("<Console>: "..player.name.." has svn up the server.", 50)
			os.execute("echo "..player.name.." has SVN UP the server.")
			printf = 0
		elseif (lspeech == "metan") or (lspeech == "/metan") then
			os.execute("./metan")
			player:gmMsg("<Console>: "..player.name.." has used ./metan", 50)
			os.execute("echo "..player.name.." has used ./metan")
			printf = 0
		elseif (lspeech == "/make map") then
			os.execute("svn up")
			player:gmMsg("<Console>: "..player.name.." has begun SVN UP + MAP compile.", 50)
			player.gameRegistry["make"] = os.time()
			os.execute("make map")
			local make = os.difftime(os.time(),player.gameRegistry["make"])
			if (make < 4) then
				player:gmMsg("<Console>: Compilation ERROR on MAP by "..player.name.." ("..make..")", 50)
			else
				player:gmMsg("<Console>: "..player.name.." has SVN UP & compiled map in "..make.." seconds.", 50)
				os.execute("echo "..player.name.." has svn up and compiled map.")
			end
			printf = 0
		elseif (lspeech == "/make char") then
			os.execute("svn up")
			player:gmMsg("<Console>: "..player.name.." has begun SVN UP + CHAR compile.", 50)
			player.gameRegistry["make"] = os.time()
			os.execute("make char")
			local make = os.difftime(os.time(),player.gameRegistry["make"])
			player:gmMsg("<Console>: "..player.name.." has SVN UP & compiled char in "..make.." seconds.", 50)
			os.execute("echo "..player.name.." has svn up and compiled char.")
			printf = 0
		elseif (lspeech == "/make login") then
			os.execute("svn up")
			player:gmMsg("<Console>: "..player.name.." has begun SVN UP + LOGIN compile.", 50)
			player.gameRegistry["make"] = os.time()
			os.execute("make login")
			local make = os.difftime(os.time(),player.gameRegistry["make"])
			player:gmMsg("<Console>: "..player.name.." has SVN UP & compiled LOGIN in "..make.." seconds.", 50)
			os.execute("echo "..player.name.." has svn up and compiled login.")
			printf = 0
		elseif (lspeech == "/test") then
			player:warp(10002,10,10)
		elseif (lspeech=="mob:walk") then
			player:sendAction(0,100)
			printf = 0
		elseif (lspeech=="mob:attack") then
			player:sendAction(1,100)
			printf = 0
		elseif (lspeech=="mob:cast") then
			player:sendAction(2,100)
			printf = 0
		elseif (lspeech=="mob:cast2") then
			player:sendAction(3,100)
			printf = 0
		elseif (lspeech=="mob:die") then
			player:sendAction(4,100)
			printf = 0
		elseif (lspeech=="mob:turn") then
			player:sendAction(7,100)
			printf = 0
		elseif (speech=="nside")then
			if (player.side == -2) then
				player:sendMinitext("Skipping -1.")
				player.side = 0
				return
			end
			player.side = player.side + 1
			player:sendSide()
			printf = 0
		elseif (speech=="pside")then
			if (player.side == 0) then
				player:sendMinitext("Can't have negative side.")
				return
			end
			player.side = player.side - 1
			player:sendSide()
			printf = 0			
		elseif (speech=="side")then
			player:sendMinitext("Side: "..player.side)
			printf = 0			
		elseif(lspeech=="what is your id?")then
			local npc = player:getObjectsInCell (player.m,player.x,player.y,BL_NPC)
			for x = 1, #npc do
				--player:talk(0,""..npc[x].ID)
				local realid = npc[x].ID - 3221225470
				player:sendMinitext("Real NPC ID: "..realid)
			end
		elseif (speech == "nobj") then
			if (getObject(p.m, p.x, p.y) == 18527) then
				player:sendMinitext("You are at the last object: 18527")
			else
				setObject(p.m, p.x, p.y, getObject(p.m, p.x, p.y) + 1)
			end
			printf = 0
		elseif (speech == "pobj") then
			if (getObject(p.m, p.x, p.y) == 0) then
				player:sendMinitext("You are at the first object: 0")
			else
				setObject(p.m, p.x, p.y, getObject(p.m, p.x, p.y) - 1)
			end
			printf = 0
		elseif (speech == "ntile") then
			if (getTile(p.m, p.x, p.y) == 38108) then
				player:sendMinitext("You are at the last tile: 38108")
			else
				setTile(p.m, p.x, p.y, getTile(p.m, p.x, p.y) + 1)
			end
			printf = 0
		elseif (speech == "ptile") then
			if (getTile(p.m, p.x, p.y) == 0) then
				player:sendMinitext("You are at the first tile: 0")
			else
				setTile(p.m, p.x, p.y, getTile(p.m, p.x, p.y) - 1)
			end
			printf = 0
		end
	end
	--Generic Staff
	if (player:staff() ~= -1) then
		local say = {"nweap", "pweap", "weap", "narmor", "parmor", "armor", "nshield", "pshield", "shield", "nhelm", "phelm", "helm", "ncape", "pcape", "cape", "ncrown", "pcrown", "crown", "nface", "pface", "face", "nboots", "pboots", "boots", "nneck", "pneck", "neck", "nfacea", "pfacea", "facea", "nfaceat", "pfaceat", "faceat", "nhair", "phair", "hair","nsdye","psdye"}
		local name = {"Weapon", "Weapon", "Weapon", "Armor", "Armor", "Armor", "Shield", "Shield", "Shield", "Helmet", "Helmet", "Helmet", "Cape", "Cape", "Cape", "Crown", "Crown", "Crown", "Face", "Face", "Face", "Boots", "Boots", "Boots", "Necklace", "Necklace", "Necklace", "Face Accessory", "Face Accessory", "Face Accessory", "Face AccessoryT", "Face AccessoryT", "Face AccessoryT", "Hair", "Hair", "Hair"}
		local var = {p.gfxWeap, p.gfxWeap, p.gfxWeap, p.gfxArmor, p.gfxArmor, p.gfxArmor, p.gfxShield, p.gfxShield, p.gfxShield, p.gfxHelm, p.gfxHelm, p.gfxHelm, p.gfxCape, p.gfxCape, p.gfxCape, p.gfxCrown, p.gfxCrown, p.gfxCrown, p.gfxFace, p.gfxFace, p.gfxFace, p.gfxBoots, p.gfxBoots, p.gfxBoots, p.gfxNeck, p.gfxNeck, p.gfxNeck, p.gfxFaceA, p.gfxFaceA, p.gfxFaceA, p.gfxFaceAT, p.gfxFaceAT, p.gfxFaceAT, p.gfxHair, p.gfxHair, p.gfxHair}
		local x
		
		--GFX toggles
		for x = 1, #say do
			if ((lspeech==""..say[x] or string.match(lspeech, ""..say[x].." (%d+)") ~= nil or string.match(lspeech, ""..say[x].." %d+ (%d+)") ~= nil) and string.sub(lspeech, 0, string.len(say[x])) == say[x]) then

				local nn = 0
				local x2 = x - 2

				if (x%3==0) then
					if (string.match(lspeech, ""..say[x].." (%d+)") ~= nil) then
						if (x == 3) then
							p.gfxWeap = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 6) then
							p.gfxArmor = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 9) then
							p.gfxShield = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 12) then
							p.gfxHelm = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 15) then
							p.gfxCape = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 18) then
							p.gfxCrown = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 21) then
							p.gfxFace = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 24) then
							p.gfxBoots = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 27) then
							p.gfxNeck = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 30) then
							p.gfxFaceA = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 33) then
							p.gfxFaceAT = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						elseif (x == 36) then
							p.gfxHair = tonumber(string.match(lspeech, ""..say[x].." (%d+)"))
						end
						
						if (string.match(lspeech, ""..say[x].." %d+ (%d+)") ~= nil) then
							if (x == 3) then
								p.gfxWeapC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 6) then
								p.gfxArmorC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 9) then
								p.gfxShieldC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 12) then
								p.gfxHelmC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 15) then
								p.gfxCapeC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 18) then
								p.gfxCrownC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 21) then
								p.gfxFaceC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 24) then
								p.gfxBootsC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 27) then
								p.gfxNeckC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 30) then
								p.gfxFaceAC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 33) then
								p.gfxFaceATC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							elseif (x == 36) then
								p.gfxHairC = tonumber(string.match(lspeech, ""..say[x].." %d+ (%d+)"))
							end
						end
						
						player:refresh()
					elseif (lspeech == say[x]) then
						player:sendMinitext(""..name[x].." GFX: "..var[x])
					end
					nn = 1
				end
				if (nn == 0) then
					if (x2%3 == 0) then
						if (var[x] == 65535) then
							player:sendMinitext("You have reached the minimum of  "..name[x].." GFX; -1.")
						else
							if (x <= 3) then
								if (var[x] == 10000) then
									p.gfxWeap = 385
								elseif (var[x] == 20000) then
									p.gfxWeap = 10122
								elseif (var[x] == 30000) then
									p.gfxWeap = 20129
								else
									p.gfxWeap = var[x] - 1
								end
							elseif (x <= 6) then
								if (var[x] == 10000) then
									p.gfxArmor = 344
								else
									p.gfxArmor = var[x] - 1
								end
							elseif (x <= 9) then
								if (var[x] == 10000) then
									p.gfxShield = 44
								else
									p.gfxShield = var[x] - 1
								end
							elseif (x <= 12) then
								p.gfxHelm = var[x] - 1
							elseif (x <= 15) then
								p.gfxCape = var[x] - 1
							elseif (x <= 18) then
								p.gfxCrown = var[x] - 1
							elseif (x <= 21) then
								p.gfxFace = var[x] - 1
							elseif (x <= 24) then
								p.gfxBoots = var[x] - 1
							elseif (x <= 27) then
								p.gfxNeck = var[x] - 1
							elseif (x <= 30) then
								p.gfxFaceA = var[x] - 1
							elseif (x <= 33) then
								p.gfxFaceAT = var[x] - 1
							elseif (x <= 36) then
								p.gfxHair = var[x] - 1
							end
							player:refresh()
						end					
					else
						if (x <= 3) then
							if (var[x] == 385) then
								p.gfxWeap = 10000
							elseif (var[x] == 10122) then
								p.gfxWeap = 20000
							elseif (var[x] == 20129) then
								p.gfxWeap = 30000
							else
								p.gfxWeap = var[x] + 1
							end
						elseif (x <= 6) then
							if (var[x] == 344) then
								p.gfxArmor = 10000
							else
								p.gfxArmor = var[x] + 1
							end
						elseif (x <= 9) then
							if (var[x] == 44) then
								p.gfxShield = 10000
							else
								p.gfxShield = var[x] + 1
							end
						elseif (x <= 12) then
							p.gfxHelm = var[x] + 1
						elseif (x <= 15) then
							p.gfxCape = var[x] + 1
						elseif (x <= 18) then
							p.gfxCrown = var[x] + 1
						elseif (x <= 21) then
							p.gfxFace = var[x] + 1
						elseif (x <= 24) then
							p.gfxBoots = var[x] + 1
						elseif (x <= 27) then
							p.gfxNeck = var[x] + 1
						elseif (x <= 30) then
							p.gfxFaceA = var[x] + 1
						elseif (x <= 33) then
							p.gfxFaceAT = var[x] + 1
						elseif (x <= 36) then
							p.gfxHair = var[x] + 1
						end
						player:refresh()
					end
				end
				printf = 0
				break
			end
		end
		
		if (string.match(lspeech, "icon (%d+)") ~= nil and string.match(lspeech, "icon %d+ (%d+)") == nil) then
			local x = tonumber(string.match(lspeech, "icon (%d+)"))
			player.registry["gfx_icons"] = x
			player.registry["gfx_icons_color"] = 0
			async(iconViewer(player, "n"))
			printf = 0
		elseif (string.match(lspeech, "icon %d+ (%d+)") ~= nil) then
			local x = tonumber(string.match(lspeech, "icon (%d+)"))
			local y = tonumber(string.match(lspeech, "icon %d+ (%d+)"))
			player.registry["gfx_icons"] = x
			player.registry["gfx_icons_color"] = y
			async(iconViewer(player, "nc"))
			printf = 0
		elseif (lspeech == "nicon") then
			async(iconViewer(player, "n"))
			printf = 0
		elseif (lspeech == "picon") then
			async(iconViewer(player, "p"))
			printf = 0
		elseif (lspeech == "niconc") then
			async(iconViewer(player, "nc"))
			printf = 0
		elseif (lspeech == "piconc") then
			async(iconViewer(player, "pc"))
			printf = 0
		elseif (speech=="ndis") then
			if (player.disguise == 1508) then
				player:sendMinitext("You may not go over 1508 on disguise.")
				return
			end
			local oldstate = player.state
			
			player.state = 0
			player:updateState()
			if (player.disguise == 1341) or (player.disguise == 1393) then
				player.disguise = player.disguise + 2
			else
				player.disguise = player.disguise + 1
			end
			player.state = oldstate
			player:updateState()
			--player:refresh()
			printf = 0
		elseif (speech=="pdis") then
			if (player.disguise == 0) then
				player:sendMinitext("You may not have negative disguise.")
				return
			end
			local oldstate = player.state
			
			player.state = 0			
			player:updateState()
			if (player.disguise == 1343) or (player.disguise == 1395) then
				player.disguise = player.disguise - 2
			else
				player.disguise = player.disguise - 1
			end
			player.state = oldstate
			player:updateState()
			--player:refresh()
			printf = 0
		elseif (speech=="dis")then
			player:sendMinitext("Disguise #: "..player.disguise)
			printf = 0
			--Disguise Color
		elseif (string.match(speech, "dis (%d+)") ~= nil and string.sub(speech, 0, 3) == "dis") then
			local oldstate = player.state
				
			player.state = 0
			player:updateState()
			
			if (tonumber(string.match(speech, "dis (%d+)")) < 0 or tonumber(string.match(speech, "dis (%d+)")) == 1342 or tonumber(string.match(speech, "dis (%d+)")) == 1394) then
				player:sendMinitext("Disguise not allowed.")
			else
				player.disguise = tonumber(string.match(speech, "dis (%d+)"))
			end
			
			player.state = oldstate
			player:updateState()
			printf = 0
		elseif (speech=="ndisc") then
			if (player.disguiseColor == 255) then
				player:sendMinitext("You may not go over 255 on disguiseColor.")
				return
			end
			local oldstate = player.state
			
			player.state = 0
			player:updateState()
			player.disguiseColor = player.disguiseColor + 1
			player.state = oldstate
			player:updateState()
			printf = 0
		elseif (speech=="pdisc") then
			if (player.disguiseColor == 0) then
				player:sendMinitext("You may not have negative disguise color.")
				return
			end
			local oldstate = player.state
			
			player.state = 0			
			player:updateState()
			player.disguiseColor = player.disguiseColor - 1
			player.state = oldstate
			player:updateState()
			printf = 0
		elseif (speech=="disc")then
			player:sendMinitext("Disguise Color #: "..player.disguiseColor)
			printf = 0			
			--End Disguise Color
		elseif (string.match(lspeech,"mob:(%d+)") ~= nil) then
			if (string.len(lspeech) < 7) then
				local j = string.match(lspeech,"mob:(%d+)")
				j = j * 1
				player:sendAction(j,50)
			end
			printf = 0
		elseif (speech=="ndye")then
			if (player.gfxClone == 1) then
				player.gfxDye = player.gfxDye + 1
			else
				player.armorColor = player.armorColor + 1
			end
			
			player:refresh()
			printf = 0
		elseif (speech=="pdye")then
			if (player.gfxClone == 1) then
				player.gfxDye = player.gfxDye - 1
			else
				player.armorColor = player.armorColor - 1
			end
			
			player:refresh()
			printf = 0
			
		elseif (speech=="nsdye")then 
			if (player.gfxClone == 1) then
				player.gfxShieldC = player.gfxShieldC + 1
			else
				player.armorColor = player.armorColor + 1
			end
			
			player:refresh()
			printf = 0
		elseif (speech=="psdye")then
			--player:talk(0,"what the "..player.gfxClone)
			if (player.gfxClone == 1) then
				
				player.gfxShieldC = player.gfxShieldC - 1
			else
				player.armorColor = player.armorColor - 1
			end
			
			player:refresh()
			printf = 0
		elseif (speech=="sdye")then
			--player:talk(0,"what the "..player.gfxClone)
			if (player.gfxClone == 1) then
				player:sendMinitext("gfxShieldC: "..player.gfxShieldC)
			else
				player.armorColor = player.armorColor - 1
			end
			
			player:refresh()
			printf = 0
		elseif (speech=="dye")then
			if (player.gfxClone == 1) then
				player:sendMinitext("gfxDye: "..player.gfxDye)
			else
				player:sendMinitext("Dye: "..player.armorColor)
			end
			
			printf = 0
		elseif (string.match(speech, "dye (%d+)") ~= nil and string.sub(speech, 0, 3) == "dye") then
			if (player.gfxClone == 1) then
				player.gfxDye = tonumber(string.match(speech, "dye (%d+)"))
			else
				player.armorColor = tonumber(string.match(speech, "dye (%d+)"))
			end
			
			player:refresh()
			printf = 0
		elseif (speech=="nskin")then
			if (player.gfxClone == 1) then
				player.gfxSkinC = player.gfxSkinC + 1
			else
				player.skinColor = player.skinColor + 1
			end
			
			player:updateState()
			printf = 0
		elseif (speech=="pskin")then
			if (player.gfxClone == 1) then
				player.gfxSkinC = player.gfxSkinC - 1
			else
				player.skinColor = player.skinColor - 1
			end
			
			player:updateState()
			printf = 0			
		elseif (speech=="skin")then
			if (player.gfxClone == 1) then
				player:talk(0, "gfxSkinC: "..player.gfxSkinC)
			else
				player:talk(0,"Skin color: "..player.skinColor)
			end
			
			printf = 0
			--[[elseif (string.byte(lspeech,1)==112 and string.byte(lspeech,2)==58) then
			p=Player(""..player.name)
			eq = string.gsub(lspeech,"p:","")]]--
		elseif (string.match(speech, "skin (%d+)") ~= nil and string.sub(speech, 0, 4) == "skin") then
			if (player.gfxClone == 1) then
				player.gfxSkinC = tonumber(string.match(speech, "skin (%d+)"))
			else
				player.skinColor = tonumber(string.match(speech, "skin (%d+)"))
			end
			
			player:updateState()
			printf = 0
		elseif (speech=="nspell") then
			if (player.registry["gfx_spell"] == 427) then
				player:sendMinitext("You may not go over 427 on spell graphics.")
			else
				player.registry["gfx_spell"] = player.registry["gfx_spell"] + 1
				player:selfAnimation(player.ID, player.registry["gfx_spell"], 0)
			end
			
			printf = 0
		elseif (speech=="pspell") then
			if (player.registry["gfx_spell"] == 0) then
				player:sendMinitext("You may not have negative spell graphics.")
			else
				player.registry["gfx_spell"] = player.registry["gfx_spell"] - 1
				player:selfAnimation(player.ID, player.registry["gfx_spell"], 0)
			end
			
			printf = 0
		elseif (speech=="spell") then
			player:sendMinitext("Spell #: "..player.registry["gfx_spell"])
			player:selfAnimation(player.ID, player.registry["gfx_spell"], 0)
			printf = 0
		elseif (string.match(speech, "spell (%d+)") ~= nil and string.sub(speech, 0, 5) == "spell") then
			player.registry["gfx_spell"] = tonumber(string.match(speech, "spell (%d+)"))
			player:selfAnimation(player.ID, player.registry["gfx_spell"], 0)
			printf = 0
		elseif (speech=="nsound") then
			if (player.registry["gfx_sound"] == 1518) then
				player:sendMinitext("You may not go over 1518 on sound effects.")
			else
				if (player.registry["gfx_sound"] == 147) then
					player.registry["gfx_sound"] = 200
				elseif (player.registry["gfx_sound"] == 206) then
					player.registry["gfx_sound"] = 300
				elseif (player.registry["gfx_sound"] == 313) then
					player.registry["gfx_sound"] = 331
				elseif (player.registry["gfx_sound"] == 371) then
					player.registry["gfx_sound"] = 401
				elseif (player.registry["gfx_sound"] == 421) then
					player.registry["gfx_sound"] = 500
				elseif (player.registry["gfx_sound"] == 514) then
					player.registry["gfx_sound"] = 600
				elseif (player.registry["gfx_sound"] == 603) then
					player.registry["gfx_sound"] = 700
				elseif (player.registry["gfx_sound"] == 740) then
					player.registry["gfx_sound"] = 900
				elseif (player.registry["gfx_sound"] == 910) then
					player.registry["gfx_sound"] = 1001
				else
					player.registry["gfx_sound"] = player.registry["gfx_sound"] + 1
				end
				
				player:playSound(player.registry["gfx_sound"])
			end
			
			printf = 0
		elseif (speech=="psound") then
			if (player.registry["gfx_sound"] == 0) then
				player:sendMinitext("You may not have negative sound effects.")
			else
				if (player.registry["gfx_sound"] == 200) then
					player.registry["gfx_sound"] = 147
				elseif (player.registry["gfx_sound"] == 300) then
					player.registry["gfx_sound"] = 206
				elseif (player.registry["gfx_sound"] == 331) then
					player.registry["gfx_sound"] = 313
				elseif (player.registry["gfx_sound"] == 401) then
					player.registry["gfx_sound"] = 371
				elseif (player.registry["gfx_sound"] == 500) then
					player.registry["gfx_sound"] = 421
				elseif (player.registry["gfx_sound"] == 600) then
					player.registry["gfx_sound"] = 514
				elseif (player.registry["gfx_sound"] == 700) then
					player.registry["gfx_sound"] = 603
				elseif (player.registry["gfx_sound"] == 900) then
					player.registry["gfx_sound"] = 740
				elseif (player.registry["gfx_sound"] == 1001) then
					player.registry["gfx_sound"] = 910
				else
					player.registry["gfx_sound"] = player.registry["gfx_sound"] - 1
				end
				
				player:playSound(player.registry["gfx_sound"])
			end
			
			printf = 0
		elseif (speech=="sound") then
			player:sendMinitext("Sound #: "..player.registry["gfx_sound"])
			player:playSound(player.registry["gfx_sound"])
			printf = 0
		elseif (string.match(speech, "sound (%d+)") ~= nil and string.sub(speech, 0, 5) == "sound") then
			player.registry["gfx_sound"] = tonumber(string.match(speech, "sound (%d+)"))
			player:playSound(player.registry["gfx_sound"])
			printf = 0
		elseif (string.match(speech, "dps (%d+)") ~= nil and string.sub(speech, 0, 3) == "dps") then
			if (player.registry["dps_potion"] > 0) then
				player.sendMinitext("You've already used that.")
			else
				player.registry["dps_potion_len"] = tonumber(string.match(speech, "dps (%d+)"))
				player.registry["dps_potion"] = os.time() + player.registry["dps_potion_len"]
			end
			
			printf = 0
		end
	end
	
	--MAPPER
	if (player:staff("map", 1) == true) then
		if (string.match(lspeech,"/tile (%d+)") ~= nil) then
			local j = string.match(lspeech,"/tile (%d+)")
			local x,y
			j = j * 1--j is tile being searched.
			x = (j / 11) + 1
			y = (j % 11)
			if (y == 0) then
				y = y + 11
				x = x - 1
			end
			player:warp(10001,x,y)
			player:talk(0,"Tile "..j.." found.")
			printf=0
		elseif (lspeech == "obj") then
			player:talk(0, "Object: "..getObject(player.m, player.x, player.y))
			printf = 0
			
		elseif (lspeech == "tile") then
			player:talk(0, "Tile: "..getTile(player.m, player.x, player.y))
			printf = 0
		elseif (lspeech == "pass") then
			local pass = getPass(player.m, player.x, player.y)
			local string2 = ""
			if (pass == 0) then
				string2 = "True"
			else
				string2 = "False"
			end
			player:talk(0, "Pass: "..string2)
			printf = 0
		elseif (string.match(lspeech,"/obj (%d+)") ~= nil) then
			local j = string.match(lspeech,"/obj (%d+)")
			j = j * 1--j is tile being searched.
			if (j > 19543) then
				player:talk(0,"Object "..j.." not found. (19543)")
				printf=0
			else
				player:warp(10000,j,12)
				player:talk(0,"Object "..j.." found.")
				printf=0
			end
		elseif (string.match(player.speech, "/testmap (.+)") ~= nil and string.sub(player.speech, 1, 8) == "/testmap") then
			local map = string.match(player.speech, "/testmap (.+)")
			local mapNum = 30000 + player.ID
			
			if (string.byte(map, string.len(map) - 3) == 46) then
				os.execute("cd ../mithiamaps;svn up;cd ../mithia")
				setMap(mapNum, "../mithiamaps/"..map)
				player:warp(mapNum, 5, 5)
				player.spell = 1
			else
				player:sendMinitext("Only .map is supported, sorry.")
			end
			
			printf = 0
		elseif (lspeech == "/tp" and (player.gmLevel > 0 or player:staff("map") >= 3)) then
			player.optFlags = 128
			player:calcStat()
			printf = 0
		elseif (string.match(lspeech, "warp (%d+ %d+ %d+)") ~= nil and string.sub(lspeech, 1, 4) == "warp") then
			local map = tonumber(string.match(player.speech, "warp (%d+) %d+ %d+"))
			local x = tonumber(string.match(player.speech, "warp %d+ (%d+) %d+"))
			local y = tonumber(string.match(player.speech, "warp %d+ %d+ (%d+)"))
			
			player:warp(map, x, y)
			printf = 0
		end
	end--END MAPPER
	
	--PLAYER COMMANDS // PLAYER SAY
	
	--[[Channels]]--
	local channel = string.sub(lspeech,1,4)
	local chparse1 = string.sub(lspeech,5,6)
	--[[Chat Channel toggles]]--
	if (player.registry["chann_en"] == 2) then
		talkType = 10
	elseif (player.registry["chann_es"] == 2)  then
		talkType = 11
	elseif (player.registry["chann_fr"] == 2) then
		talkType = 12
	elseif (player.registry["chann_cn"] == 2) then
		talkType = 13
	elseif (player.registry["chann_pt"] == 2) then
		talkType = 14
	elseif (player.registry["chann_id"] == 2) then
		talkType = 15
	end

	if (channel == "/ch ") then
		if (chparse1 == "en") then
			if (length == 6) then
				if (player.registry["chann_es"] == 2)
				or (player.registry["chann_fr"] == 2) or (player.registry["chann_cn"] == 2)
				or (player.registry["chann_pt"] == 2) or (player.registry["chann_id"] == 2) then
					player:sendMinitext("You can only have one language channel toggle active at a time.")
					printf=0
				elseif (player.registry["chann_en"] == 2) then
					player.registry["chann_en"] = 1
					player:sendMinitext("English channel toggled off.")
					printf=0
				else
					player.registry["chann_en"] = 2
					player:sendMinitext("English channel toggled on.")
					printf=0
				end
			elseif (string.sub(lspeech,7,7) == " ") then
				if (string.sub(lspeech,8) == "join") then
					player:sendMinitext("You have joined the channel.")
					player.registry["chann_en"] = 1
					printf=0
				elseif (string.sub(lspeech,8) == "leave") then
					player:sendMinitext("You have left the channel.")
					player.registry["chann_en"] = 0
					printf=0
				else
					player:sendMinitext("Invalid parameter.")
					printf=0
				end
			end
		elseif (chparse1 == "es") then
			if (length == 6) then
				if (player.registry["chann_en"] == 2)
				or (player.registry["chann_fr"] == 2) or (player.registry["chann_cn"] == 2)
				or (player.registry["chann_pt"] == 2) or (player.registry["chann_id"] == 2) then
					player:sendMinitext("You can only have one language channel toggle active at a time.")
					printf=0
				elseif (player.registry["chann_es"] == 2) then
					player.registry["chann_es"] = 1
					player:sendMinitext("Spanish channel toggled off.")
					printf=0
				else
					player.registry["chann_es"] = 2
					player:sendMinitext("Spanish channel toggled on.")
					printf=0
				end
			elseif (string.sub(lspeech,7,7) == " ") then
				if (string.sub(lspeech,8) == "join") then
					player:sendMinitext("You have joined the channel.")
					player.registry["chann_es"] = 1
					printf=0
				elseif (string.sub(lspeech,8) == "leave") then
					player:sendMinitext("You have left the channel.")
					player.registry["chann_es"] = 0
					printf=0
				else
					player:sendMinitext("Invalid parameter.")
					printf=0
				end
			end
		elseif (chparse1 == "fr") then
			if (length == 6) then
				if (player.registry["chann_en"] == 2)
				or (player.registry["chann_es"] == 2) or (player.registry["chann_cn"] == 2)
				or (player.registry["chann_pt"] == 2) or (player.registry["chann_id"] == 2) then
					player:sendMinitext("You can only have one language channel toggle active at a time.")
					printf=0
				elseif (player.registry["chann_fr"] == 2) then
					player.registry["chann_fr"] = 1
					player:sendMinitext("French channel toggled off.")
					printf=0
				else
					player.registry["chann_fr"] = 2
					player:sendMinitext("French channel toggled on.")
					printf=0
				end
			elseif (string.sub(lspeech,7,7) == " ") then
				if (string.sub(lspeech,8) == "join") then
					player:sendMinitext("You have joined the channel.")
					player.registry["chann_fr"] = 1
					printf=0
				elseif (string.sub(lspeech,8) == "leave") then
					player:sendMinitext("You have left the channel.")
					player.registry["chann_fr"] = 0
					printf=0
				else
					player:sendMinitext("Invalid parameter.")
					printf=0
				end
			end
		elseif (chparse1 == "cn") then
			if (length == 6) then
				if (player.registry["chann_en"] == 2)
				or (player.registry["chann_es"] == 2) or (player.registry["chann_fr"] == 2)
				or (player.registry["chann_pt"] == 2) or (player.registry["chann_id"] == 2) then
					player:sendMinitext("You can only have one language channel toggle active at a time.")
					printf=0
				elseif (player.registry["chann_cn"] == 2) then
					player.registry["chann_cn"] = 1
					player:sendMinitext("Chinese channel toggled off.")
					printf=0
				else
					player.registry["chann_cn"] = 2
					player:sendMinitext("Chinese channel toggled on.")
					printf=0
				end
			elseif (string.sub(lspeech,7,7) == " ") then
				if (string.sub(lspeech,8) == "join") then
					player:sendMinitext("You have joined the channel.")
					player.registry["chann_cn"] = 1
					printf=0
				elseif (string.sub(lspeech,8) == "leave") then
					player:sendMinitext("You have left the channel.")
					player.registry["chann_cn"] = 0
					printf=0
				else
					player:sendMinitext("Invalid parameter.")
					printf=0
				end
			end
		elseif (chparse1 == "pt") then
			if (length == 6) then
				if (player.registry["chann_en"] == 2)
				or (player.registry["chann_es"] == 2) or (player.registry["chann_fr"] == 2)
				or (player.registry["chann_cn"] == 2) or (player.registry["chann_id"] == 2) then
					player:sendMinitext("You can only have one language channel toggle active at a time.")
					printf=0
				elseif (player.registry["chann_pt"] == 2) then
					player.registry["chann_pt"] = 1
					player:sendMinitext("Portuguese channel toggled off.")
					printf=0
				else
					player.registry["chann_pt"] = 2
					player:sendMinitext("Portuguese channel toggled on.")
					printf=0
				end
			elseif (string.sub(lspeech,7,7) == " ") then
				if (string.sub(lspeech,8) == "join") then
					player:sendMinitext("You have joined the channel.")
					player.registry["chann_pt"] = 1
					printf=0
				elseif (string.sub(lspeech,8) == "leave") then
					player:sendMinitext("You have left the channel.")
					player.registry["chann_pt"] = 0
					printf=0
				else
					player:sendMinitext("Invalid parameter.")
					printf=0
				end
			end
		elseif (chparse1 == "id") then
			if (length == 6) then
				if (player.registry["chann_en"] == 2)
				or (player.registry["chann_es"] == 2) or (player.registry["chann_fr"] == 2)
				or (player.registry["chann_cn"] == 2) or (player.registry["chann_pt"] == 2) then
					player:sendMinitext("You can only have one language channel toggle active at a time.")
					printf=0
				elseif (player.registry["chann_id"] == 2) then
					player.registry["chann_id"] = 1
					player:sendMinitext("Bahasa channel toggled off.")
					printf=0
				else
					player.registry["chann_id"] = 2
					player:sendMinitext("Bahasa channel toggled on.")
					printf=0
				end
			elseif (string.sub(lspeech,7,7) == " ") then
				if (string.sub(lspeech,8) == "join") then
					player:sendMinitext("You have joined the channel.")
					player.registry["chann_id"] = 1
					printf=0
				elseif (string.sub(lspeech,8) == "leave") then
					player:sendMinitext("You have left the channel.")
					player.registry["chann_id"] = 0
					printf=0
				else
					player:sendMinitext("Invalid parameter.")
					printf=0
				end
			end
		elseif (chparse1 == "??") then
			if (player.registry["chann_en"] > 0) then
				player:msg(0,"Listening to English Channel (en)",player.ID)
			end
			if (player.registry["chann_es"] > 0) then
				player:msg(0,"Listening to Spanish Channel (es)",player.ID)
			end
			if (player.registry["chann_fr"] > 0) then
				player:msg(0,"Listening to French Channel (fr)",player.ID)
			end
			if (player.registry["chann_cn"] > 0) then
				player:msg(0,"Listening to Chinese Channel (cn)",player.ID)
			end
			if (player.registry["chann_pt"] > 0) then
				player:msg(0,"Listening to Portuguese Channel (pt)",player.ID)
			end
			if (player.registry["chann_id"] > 0) then
				player:msg(0,"Listening to Bahasa Channel (id)",player.ID)
			end
			printf=0
		else
			player:msg(0,"Language Channels Help",player.ID)
			player:msg(0,"[/ch ?] for help with channel commands.",player.ID)
			player:msg(0,"[/ch ??] for list of joined channels.",player.ID)
			player:msg(0,"[/ch en/es/fr/cn/pt/id] to toggle ON/OFF chat in that language.",player.ID)
			player:msg(0,"[/ch +language+ join] to join +language+ channel.",player.ID)
			player:msg(0,"[/ch +language+ leave] to leave +language channel.",player.ID)
			player:msg(0,"[/+language+] to join and send a message in +language+ channel.",player.ID)
			player:msg(0,"NOTE: +language+ stands for en/es/fr/cn/pt/id",player.ID)
			printf=0
		end
	elseif (channel == "/en ") then
		talkType = 10
		player.speech = string.sub(player.speech,5)
		if (player.registry["chann_en"] < 1) then
			player.registry["chann_en"] = 1
			player:sendMinitext("You have joined the English channel.")
		end
	elseif (channel == "/es ") then
		talkType = 11
		player.speech = string.sub(player.speech,5)
		if (player.registry["chann_es"] < 1) then
			player.registry["chann_es"] = 1
			player:sendMinitext("You have joined the Spanish channel.")
		end
	elseif (channel == "/fr ") then
		talkType = 12
		player.speech = string.sub(player.speech,5)
		if (player.registry["chann_fr"] < 1) then
			player.registry["chann_fr"] = 1
			player:sendMinitext("You have joined the French channel.")
		end
	elseif (channel == "/cn ") then
		talkType = 13
		player.speech = string.sub(player.speech,5)
		if (player.registry["chann_cn"] < 1) then
			player.registry["chann_cn"] = 1
			player:sendMinitext("You have joined the Chinese channel.")
		end
	elseif (channel == "/pt ") then
		talkType = 14
		player.speech = string.sub(player.speech,5)
		if (player.registry["chann_pt"] < 1) then
			player.registry["chann_pt"] = 1
			player:sendMinitext("You have joined the Portuguese channel.")
		end
	elseif (channel == "/id ") then
		talkType = 15
		player.speech = string.sub(player.speech,5)
		if (player.registry["chann_id"] < 1) then
			player.registry["chann_id"] = 1
			player:sendMinitext("You have joined the Bahasa channel.")
		end
		
	end
	--[[End Channels]]--
	
	if (channel == "/me " and player.m ~= 7001) then
		player.speech = string.sub(player.speech,5)
		player:talk(3, player.name.." "..player.speech)
		printf = 0
	end
	
	if (string.sub(lspeech, 1, 5) == "/afk ") then
		player.afkMessage = string.sub(lspeech, 6)
		printf = 0
	end
	
	if (string.sub(lspeech, 1, 8) == "/assist " and player.registry["guide"] == 250) then
		local assist = Player(string.sub(lspeech, 9))
		
		if ((assist.m >= 7000 and assist.m < 8000) or assist.mapTitle == "Jail") then
			player:sendMinitext("You may not assist this player to his location")
			printf = 0
			return
		elseif (assist.m >= 60000) then
			player:gmMsg("Guide "..player.name.." is assisting "..assist.name.." on instance map "..assist.mapTitle.." ("..assist.m..")")
		elseif (player.mapTitle == "Jail") then
			player:sendMinitext("You may not use this ability from this map.")
			printf = 0
			return
		end
		if ((assist ~= nil and assist.level <= 25 and assist.class < 5) or (assist.registry["guide"] > 0 and assist.registry["guide"] <= 250)) then
			player.registry["assistMap"] = player.m
			player.registry["assistX"] = player.x
			player.registry["assistY"] = player.y
			player:warp(assist.m, assist.x, assist.y)
		elseif (string.sub(lspeech, 9) == "return") then
			player:warp(player.registry["assistMap"], player.registry["assistX"], player.registry["assistY"])
			player.registry["assistMap"] = 0
			player.registry["assistX"] = 0
			player.registry["assistY"] = 0
		end
		
		printf = 0
	end
	
	if (lspeech == "/ping") then
		player:sendMinitext("Ping? Pong! "..player.ping.."ms")
		printf = 0
	end
	--[[if (lspeech==light) then
		player:talk(2,"~"..light.."~")
		lux.cast(player)
		printf = 0
		--LAG]]--
	--[[elseif(string.find(lspeech,"lag")~=nil)then--or(string.find(player.speech,"lag")~=nil)then
		if(string.find(lspeech," lag"))then
			player:speak("There is no lag.",player.talkType)
			return
		elseif(string.byte(lspeech,1)==108) and (string.byte(lspeech,2)==97) and (string.byte(lspeech,3)==103)then
			player:speak("There is no lag.",player.talkType)
			return
		end]]--
	if (player.state == 0) then
		if (speech == ":A") or (lspeech == "lol") then
			player:sendAction(11,80)
		elseif (speech == ":B") then
			player:sendAction(12,80)
		elseif (speech == ":C") then
			player:sendAction(13,80)
		elseif (speech == ":D") then
			player:sendAction(14,80)
		elseif (speech == ":E") then
			player:sendAction(15,80)
		elseif (speech == ":F") then
			player:sendAction(16,80)
		elseif (speech == ":G") then
			player:sendAction(17,80)
		elseif (lspeech == ":h") then	
			player:sendAction(18,80)
		elseif (speech == ":I") then
			player:sendAction(19,80)
		elseif (speech == ":J") then	
			player:sendAction(20,80)
		elseif (speech == ":K") then
			player:sendAction(21,80)
		elseif (speech == ":L") then
			player:sendAction(22,80)
		elseif (speech == ":M") then
			player:sendAction(9,80)
		elseif (speech == ":N") then
			player:sendAction(10,80)
		elseif (speech == ":O") then
			player:sendAction(23,80)
		elseif (speech == ":P") then
			player:sendAction(24,80)
		end
	end
	
	if (string.byte(speech,1)==61 and player.gmLevel >= 50) then--== 99)then
		if ((string.byte(speech,2)>=48) and (string.byte(speech,2)<=57)) or ((string.byte(speech,2)==40) and ((string.byte(speech,3)>=48) and (string.byte(speech,3)<=57))) or ((string.byte(speech,2)==46) and ((string.byte(speech,3)>=48) and (string.byte(speech,3)<=57)))then
		--Math operations
		--=61 +43 -45 *42 /47 ^94
		--Numbers 0-9, 48-57
			local eq = string.gsub(speech,"=","")
			local f, l, x
			--eq = "1*"..eq
			f = ""
			for l in string.gmatch(eq,".") do
				f = f..l
			end
		
			x = string.gsub(""..f.." = $return "..f.."$", "%$(.-)%$", function (s) return loadstring(s)() end)
			
			player:sendMinitext("Answer is: "..x)
			printf=0
		end
	elseif (lspeech == "test") then
		--player:speak(player.speech, 0)
		player:talk(2, "Test complete.")
		printf = 0
	elseif (lspeech == "firstlogin") then
		--opening_scene.firstlogin(player)
	--[[elseif (string.find(speech,"Fuck")) then
		player.speech = string.gsub(speech,"Fuck","Darn")
		caps = 1
	elseif (string.find(speech,"FUCK")) then
		player.speech = string.gsub(speech,"FUCK","DARN")
		caps = 1
	elseif (caps == 0) then
		if (string.find(lspeech,"fuck")) then
			player.speech = string.gsub(lspeech,"fuck","darn")
		end]]--
	end

	--PRINT
	if (printf==1)then
		player:speak(player.speech,talkType)
		--[[
		player.speech = string.gsub(player.speech,"'"," quote ")
		player.speech = string.gsub(player.speech,"<","less than")
		player.speech = string.gsub(player.speech,">","more than")
		player.speech = string.gsub(player.speech,"*","asterisk")
		if (player.talkType == 0) then
			os.execute("echo "..player.name.." says: "..player.speech)
		else
			os.execute("echo "..player.name.." yells: "..player.speech)
		end]]--
	else
		return
	end
end