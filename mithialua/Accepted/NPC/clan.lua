clan = {
check = function(player)
	local clans = {7}
	if (#clans > 0) then
		for i = 1, #clans do
			if (player.clan == clans[i]) then
				return true
			end
		end
	end
	return false
end,



menu = function(player, npc)
	local npcGraphics = {graphic = convertGraphic(npc.look, "monster"), color = npc.lookColor}
	local clanTitleLen = 18
	local clanLeader = player.registry["clan"..player.clan.."leader"]
	local clanOpts = {}
	local options = {}
	local menu = ""
	local input = ""
	local response
	
	player.npcGraphic = npcGraphics.graphic
	player.npcColor = npcGraphics.color
	player.dialogType = 0
	
	if (player.clan == 0) then
		return
	end
	
	if (clan.check(player) == true) then--clan
		--Level 1 invite
		--Level 2 kick
		--Level 3 title
		--Level 4 bank
		if (clanLeader == 1) then
			table.insert(options, "Invite")
		elseif (clanLeader == 2) then
			table.insert(options, "Invite")
			table.insert(options, "Kick")
		elseif (clanLeader == 3) then
			table.insert(options, "Invite")
			table.insert(options, "Kick")
			table.insert(options, "Clan Title")
		elseif (clanLeader == 4) then
			table.insert(options, "Invite")
			table.insert(options, "Kick")
			table.insert(options, "Clan Title")
			table.insert(options, "Clan Bank")
		elseif (clanLeader == 250) then
			table.insert(options, "Invite")
			table.insert(options, "Kick")
			table.insert(options, "Clan Title")
			table.insert(options, "Clan Bank")
			table.insert(options, "Promote")
		end
		
		table.insert(options, "Leave")
		menu = player:menuString("What would you like to do?", options)
		if (menu == "Invite") then
			input = player:input("Who would you like to invite?")
			if (clan.isOnline(input) == true) then
				if (Player(input).coRef > 0) then
					player:dialogSeq({npcG, input.." is currently busy."}, 1)
					return
				else
					addChatLog(player.ID, player.name.." has invited "..input.." to join "..player.clanName..".")
					player:dialogSeq({npcG, input.." has been invited to "..player.clanName.."."}, 1)
					response = async(clan.invite(Player(input), npc, player))
					
					if (response == false) then
						player:dialogSeq({npcG, "They have declined."}, 1)
					end
				end
			else
				player:dialogSeq({npcG, input.." is not online."}, 1)
				return
			end
		elseif (menu == "Kick") then
			input = player:input("Who would you like to kick?")
			if (clan.isOnline(input) == true) then
				response = async(clan.kick(Player(input), npc, player))

				if (response == true) then
					player:dialogSeq({npcG, input.." has been kicked from "..player.clanName.."."}, 1)
					addChatLog(player.ID, input.." has been kicked from "..player.clanName.." by "..player.name..".")
				elseif (response == false) then
					player:dialogSeq({npcG, "You can't kick a leader."}, 1)
				end
			else
				player:dialogSeq({npcG, input.." is not online."}, 1)
				return
			end
		elseif (menu == "Clan Title") then
			player:dialogSeq({npcG, "Changing clan titles has a cost of 10,000 coins.\n\nOnly proceed if you are willing to pay this fee."}, 1)
			if (player.money < 10000) then--10k fee
				player:dialogSeq({npcG, "You do not have 10,000 coins."}, 1)
				return
			end
			response = player:input("Who's title would you like to change?")
			local tempPlayer = Player(response)
			if (tempPlayer ~= nil) then
				if (tempPlayer.clan ~= player.clan) then
					player:dialogSeq({npcG, "You can not change the clan title to someone does not belong to your clan."}, 1)
					return
				end
				response = player:input("What title would you like to give "..tempPlayer.clanTitle.." "..tempPlayer.name.."?")
				if (string.len(response) <= clanTitleLen and tempPlayer ~= nil) then
					if (player.money < 10000) then
						player:dialogSeq({npcG, "You do not have 10,000 coins."}, 1)
						addChatLog(player, player.name.." attempted to get a free clan title.")
						player:gmMsg(player.name.." attempted to get a free clan title.")
						return
					end
					player:removeGold(10000)
					tempPlayer.clanTitle = response
					tempPlayer:sendMinitext(player.name.." has changed your title to "..response..".")
					player:popUp("You have changed "..tempPlayer.name.."'s title to "..response..".")
					addChatLog(player.ID, player.name.." has changed "..tempPlayer.name.."'s clan title to "..response..".")
				else
					player:dialogSeq({npcG, response.." is too long."}, 1)
					return
				end
			else
				player:dialogSeq({npcG, response.." is not online"}, 1)
				return
			end
		elseif (menu == "Leave") then
			addChatLog(player.ID, player.name.." has left "..player.clanName..".")
			clan.leave(player, npc)
		end

	else--RING
		if (clanLeader == 250) then
			table.insert(options, "Invite")
			table.insert(options, "Kick")
			table.insert(options, "Clan Title")
		end
		table.insert(options, "Leave")
		menu = player:menuString("What would you like to do?", options)
		if (menu == "Invite") then
			input = player:input("Who would you like to invite?")
			if (clan.isOnline(input) == true) then
				if (Player(input).coRef > 0) then
					player:dialogSeq({npcG, input.." is currently busy."}, 1)
					return
				else
					addChatLog(player.ID, player.name.." has invited "..input.." to join "..player.clanName..".")
					player:dialogSeq({npcG, input.." has been invited to "..player.clanName.."."}, 1)
					response = async(clan.invite(Player(input), npc, player))
					
					if (response == false) then
						player:dialogSeq({npcG, "They have declined."}, 1)
					end
				end
			else
				player:dialogSeq({npcG, input.." is not online."}, 1)
				return
			end
		elseif (menu == "Clan Title") then
			player:dialogSeq({npcG, "Changing clan titles has a cost of 50,000 coins.\n\nOnly proceed if you are willing to pay this fee."}, 1)
			if (player.money < 50000) then--50k fee (RINGS)
				player:dialogSeq({npcG, "You do not have 50,000 coins."}, 1)
				return
			end
			response = player:input("Who's title would you like to change?")
			local tempPlayer = Player(response)
			if (tempPlayer ~= nil) then
				if (tempPlayer.clan ~= player.clan) then
					player:dialogSeq({npcG, "You can not change the clan title to someone does not belong to your clan."}, 1)
					return
				end
				response = player:input("What title would you like to give "..tempPlayer.clanTitle.." "..tempPlayer.name.."?")
				if (string.len(response) <= clanTitleLen and tempPlayer ~= nil) then
					if (player.money < 50000) then
						player:dialogSeq({npcG, "You do not have 50,000 coins."}, 1)
						addChatLog(player.ID, player.name.." attempted to get a free ring title.")
						player:gmMsg(player.name.." attempted to get a free ring title.")
						return
					end
					player:removeGold(50000)
					tempPlayer.clanTitle = response
					tempPlayer:sendMinitext(player.name.." has changed your title to "..response..".")
					player:popUp("You have changed "..tempPlayer.name.."'s title to "..response..".")
					addChatLog(player.ID, player.name.." has changed "..tempPlayer.name.."'s clan title to "..response..".")
				else
					player:dialogSeq({npcG, response.." is too long."}, 1)
					return
				end
			else
				player:dialogSeq({npcG, response.." is not online"}, 1)
				return
			end
		elseif (menu == "Kick") then
			input = player:input("Who would you like to kick?")
			if (clan.isOnline(input) == true) then
				response = async(clan.kick(Player(input), npc, player))

				if (response == true) then
					player:dialogSeq({npcG, input.." has been kicked from "..player.clanName.."."}, 1)
					addChatLog(player.ID, input.." has been kicked from "..player.clanName.." by "..player.name..".")
				elseif (response == false) then
					player:dialogSeq({npcG, "You can't kick a leader."}, 1)
				end
			else
				player:dialogSeq({npcG, input.." is not online."}, 1)
				return
			end
		elseif (menu == "Leave") then
			clan.leave(player, npc)
		end
	end
	
	--[[
	if (player.clan == 2) then--testing
		if (clanLeader >= 0) then--Member
			table.insert(clanOpts, "Board")
		end
		if (clanLeader >= 1) then--Recruiter
			table.insert(clanOpts, "Invite")
		end
		if (clanLeader >= 2) then--Council
			table.insert(clanOpts, "Clan Title")
			table.insert(clanOpts, "Kick")
		end
		if (clanLeader == 250) then--Leader
			table.insert(clanOpts, "Promote")
			table.insert(clanOpts, "Demote")
			table.insert(clanOpts, "Leadership")
		end
		if (#clanOpts < 1) then
			return
		end
		menu = player:menuString("Greetings "..player.name..".", clanOpts)
		
		if (menu == "Board") then
		elseif (menu == "Invite") then
			input = player:input("Who would you like to invite?")
			if (clan.isOnline(input) == true) then
				player:dialogSeq({npcG, input.." has been invited to "..player.clanName.."."}, 1)
				options = {"Accept", "Decline"}
				Player(input):menu(player.name.." has invited you to join "..player.clanName..".", options)
				
			else
				player:dialogSeq({npcG, input.." is not online."}, 1)
				return
			end
		elseif (menu == "Clan Title") then
		elseif (menu == "Kick") then
		elseif (menu == "Promote") then
		elseif (menu == "Demote") then
		elseif (menu == "Leadership") then
		end
		return
	end]]--
end,


invite = async(function(player, npc, inviter)
	local npcGraphics = {graphic = convertGraphic(npc.look, "monster"), color = npc.lookColor}
	
	player.npcGraphic = npcGraphics.graphic
	player.npcColor = npcGraphics.color
	player.dialogType = 0
	
	if (player ~= nil and player.clan == 0) then
		local options = {"Accept", "Decline"}
		local menu = player:menu(inviter.name.." has invited you to join "..inviter.clanName..".", options)
		if (menu == 1) then
			player.clan = inviter.clan
			player:status()
			return true
		else
			return false
		end
	end
end),



kick = async(function(player, npc, inviter)
	local npcGraphics = {graphic = convertGraphic(npc.look, "monster"), color = npc.lookColor}
	
	player.npcGraphic = npcGraphics.graphic
	player.npcColor = npcGraphics.color
	player.dialogType = 0
	
	if (player ~= nil and player.clan == inviter.clan) then
		if (player.registry["clan"..player.clan.."leader"] ~= 250) then
			player.clanTitle = ""
			player.clan = 0
			player:status()
			player:popUp("You have been removed from your clan.")
			return true
		else
			return false
		end
	end
end),



leave = function(player, npc)
	local npcGraphics = {graphic = convertGraphic(npc.look, "monster"), color = npc.lookColor}
	local clanLeader = player.registry["clan"..player.clan.."leader"]
	
	player.npcGraphic = npcGraphics.graphic
	player.npcColor = npcGraphics.color
	player.dialogType = 0
	
	if (clanLeader == 250) then
		player:dialogSeq({npcG, "You may not leave while you are the leader."}, 1)
		return
	else
		options = {"Leave", "Nevermind"}
		menu = player:menu("Are you sure you want to leave your clan?", options)
		if (menu == 1) then
			player.clanTitle = ""
			player.clan = 0
		elseif (menu == 2) then
		
		end
	end
end,



isOnline = function(playerName)
	local ret = false
	if (Player(playerName) ~= nil) then--online
		return true
	end
	return ret
end
}