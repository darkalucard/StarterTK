gm_do = {
cast = async(function(player,target)
	local ready
	local tempplayer
	local map
	local dialog = "What to do?"
	local opts = {}
	local mob
	local where
	local who
	local npcG = {graphic = convertGraphic(39, "monster"), color = 16}
	player.npcGraphic = npcG.graphic
	player.npcColor = npcG.color
	player.dialogType = 0
	if (player.gmLevel < 1) then
		return
	end
	
	--player.lastClick = 0
	
	table.insert(opts,"spawn")
	table.insert(opts,"where")
	table.insert(opts,"speed")
	table.insert(opts,"info")
	local what = player:menuString(dialog,opts)
	if(what == "spawn") then
		local spawn = player:input("Spawn what?")
		if (spawn == "tavern") then
			who = player:input("On?")
			if (Player(who) ~= nil) then
				tempplayer = Player(who)
				tempplayer:warp(46, 11, 11)
			else
				player:popUp(who.." is not online.")
			end
		elseif (spawn == "item") then
			who = player:input("On?")
			if (Player(who) ~= nil) then
				tempplayer = Player(who)
				what = player:input("What?")
				if (Item(what) ~= nil) then
					mob = player:input("How many?")
					mob = tonumber(mob)
					tempplayer:addItem(what, mob)
					player:popUp(mob.." "..what.." have been delivered to "..who..".")
				else
					player:popUp("Error, "..what.." does not exist")
				end
			else
				player:popUp(who.." is not online.")
			end
			return
		elseif (spawn == "xp") then
			who = player:input("On?")
			if (Player(who) ~= nil) then
				tempplayer = Player(who)
				mob = player:input("How many?")
				mob = tonumber(mob)
				tempplayer:giveXP(mob)
				player:popUp(mob.."exp has been delivered to "..who..".")
			else
				player:popUp(who.." is not online.")
			end
			return
		end
		mob = player:input("How many?")
		where = player:input("Where?")
		if(where == "reference") then
			local xref1 = player:input("Reference X?") * 1
			local yref1 = player:input("Reference Y?") * 1
			local from = player:input("Reference from?")
			if(from == "nope") then
				xref = (player.x + xref1)
				yref = (player.y + yref1)
				map = player.m
			else
				tempplayer = Player(from)
				xref = (tempplayer.x + xref1)
				yref = (tempplayer.y + yref1)
				map = tempplayer.m
			end
			ready = 1
		else
			tempplayer = Player(where)
			xref = tempplayer.x
			yref = tempplayer.y
			map = tempplayer.m
			ready = 1
		end
		if(ready == 1) then
			spawn = (spawn * 1)
			xref = (xref * 1)
			yref = (yref * 1)
			mob = (mob * 1)
			map = (map * 1)
			player:spawn(spawn,xref,yref,mob,map)
		end
	--end spawn
	elseif(what == "where") then
		who = player:input("Locate who?")
		target=Player(who)
		if(target~=nil) then
			player:sendMinitext(target.name.." is located at:")
			player:sendMinitext("Map: "..target.m.." \a X: "..target.x.." \a Y: "..target.y)
		end
	elseif(what == "speed") then
		local speed=player:input("Set or Use?")
		if(tostring(speed)=="set") then
			local set=player:input("Increase, decrease or normalize?")
			if(set=="+") or (string.lower(set)=="increase") or (string.lower(set)=="inc") or (string.lower(set)=="i") then
				local amt = player:input("Increase by how much?")
				player.mapRegistry['spd'] = amt
				speed = "use"
			elseif (set=="-") or (string.lower(set)=="decrease") or (string.lower(set)=="dec") or (string.lower(set)=="d")  then
				local amt = player:input("Decrease by how much?")
				amt = amt * -1
				player.mapRegistry['spd'] = amt
				speed = "use"
			elseif (set=="=") or (string.lower(set)=="normalize") or (string.lower(set)=="norm") or (string.lower(set)=="n")  then
				player.mapRegistry['spd'] = 80
				speed = "use"
			end
		end
		if(speed=="use") then
			local amt = player.mapRegistry['spd']
			local who = player:input("Use on?")
			local victim = Player(string.lower(who))
			if (player.mapRegistry['spd'] ~= nil) then
				if (player.mapRegistry['spd'] == 80) then
					victim.speed = 80
					player:sendMinitext(who.."'s new speed is: "..victim.speed..".")
					player.mapRegistry['spd'] = 0
					victim:updateState()
					return
				end
				if (((victim.speed + amt) <= 255) and (victim.speed + amt >= 0)) or (((victim.speed - amt) <= 255) and (victim.speed - amt >= 0)) then
					victim.speed = victim.speed - amt
					player:sendMinitext(who.."'s new speed is: "..victim.speed..".")
					--player.mapRegistry['spd'] = 0
					victim:updateState()
				else
					player:sendMinitext("Invalid amount, current speed is: "..amt.."; and "..who.."'s speed is "..victim.speed..".")
				end
			else
				player:sendMinitext("Settings required.")
			end
		end
	elseif(what == "info") then
		local who=player:input("Info on who?")
		target=Player(who)
		if(target.blType==BL_PC) then
			target:sendMinitext(player.name.." tickles you.")
			target:sendAction(11,255)
			player:sendMinitext("Info on "..target.className.." "..target.name..".")
			player:popUp(target.name.." "..target.level.."\n\r"..target.health.."/"..target.maxHealth.." ("..target.baseHealth..") "..target.magic.."/"..target.maxMagic.." ("..target.baseMagic..")\n\rMight: "..target.might.." Will:"..target.will.." Grace:"..target.grace.."\nAC: "..target.ac.." Dam: "..target.dam.." Hit: "..target.hit.."\rMoney: "..target.money.." Bank: "..target.bankMoney.."\n\rExperience: "..target.exp)
		end 
	end
end)
}	