login = function(player)
	--FIRST LOGIN
	local logincount = player.registry["login_count"]
	
	if (not player:hasLegend("born")) then
		player:addLegend("Born. "..curT(), "born", 0, 80)
	end
	
	if (logincount > 0) then
		logincount = logincount + 1
		if (player:staff() == true) then
		--
		else
			local friends = player:getFriends()
			
			if (#friends > 0) then
				for i = 1, #friends do
					player:msg(4, ""..player.name.." has logged in.", friends[i].ID)
				end
			end
		end
		player.registry["login_count"] = logincount
	end
	
	if (player.registry["instanceMap"] > 0) then
		player:warp(player.registry["instanceMap"], player.registry["instanceX"], player.registry["instanceY"])
		player.registry["instanceMap"] = 0
		player.registry["instanceX"] = 0
		player.registry["instanceY"] = 0
	end
	
	if (player.m == 10000) then
		objBrowser()
	elseif (player.m == 10001) then
		tileBrowser()
	end
		
end

logout = function(player)
	player.registry["ambush_timer"] = 0
	if (player.gmLevel < 50) then
		if (player:staff() == true) then
			--staff doesn't broadcast
		else
			local friends = player:getFriends()
			
			if (#friends > 0) then
				for i = 1, #friends do
					player:msg(4, ""..player.name.." has logged out.", friends[i].ID)
				end
			end
		end
	end
	
	if (player.m >= 60000) then
		player.registry["instanceMap"] = player.m
		player.registry["instanceX"] = player.x
		player.registry["instanceY"] = player.y
		player:warp(40, math.random(75, 82), math.random(17, 19))
	end
	
	if (player.m > 30000) then
		player:warp(10010, 8, 7)
	end
	
	if (player.m >= 20000 and player.m <= 20020) then
		player.registry["login_count"] = 0
		player.state = 0
		player:warp(144, 59, 40)
	end
	
	if (player.m >= 21000 and player.m <= 21020) then
		player:warp(1, 28, 62)
	end
end

mapWeather = function()
	local x, weather
	weather = math.random(1,5)
	if(math.random(4) == 2) then
		if(weather > 3 and weather < 5 and getCurSeason() == "Spring") then
			setWeather(0,0,0)
		elseif(weather >= 2 and weather < 4 and getCurSeason() == "Summer") then
			setWeather(0,0,0)
		elseif(weather == 4 and getCurSeason() == "Fall") then
			setWeather(0,0,0)
		elseif(weather > 3 and getCurSeason() == "Winter") then
			setWeather(0,0,0)
		elseif(weather <= 3 and weather > 1 and getCurSeason() == "Spring") then
			setWeather(0,0,1)
		elseif(weather == 1 and getCurSeason() == "Summer") then
			setWeather(0,0,1)
		elseif(weather >= 2 and weather < 4 and getCurSeason() == "Fall") then
			setWeather(0,0,1)
		elseif(weather == 1 and (getCurSeason() == "Spring" or getCurSeason() == "Fall")) then
			setWeather(0,0,2)
		elseif(weather <= 3 and getCurSeason() == "Winter") then
			setWeather(0,0,2)
		elseif(weather == 5 and getCurSeason() == "Spring") then
			setWeather(0,0,3)
		elseif(weather >= 4 and getCurSeason() == "Summer") then
			setWeather(0,0,3)
		elseif(weather == 5 and getCurSeason() == "Fall") then
			setWeather(0,0,3)
		else
			setWeather(0,0,getWeather(0,0))
		end
	else
		setWeather(0,0,getWeather(0,0))
	end
end

mapEnter = function(player)
	local m = player.m
	--player:sendMinitext("Entering")
	if (m == 10000) then
		objBrowser()
	elseif (m == 10001) then
		tileBrowser()
	elseif (m == 10010) then
		if ((player:staff("pm") > 0) or (player:staff("map") > 0) or (player:staff("lore") > 0) or (player:staff("script") > 0)) then
			player:sendMinitext("Welcome "..player.name..".")
		else
			broadcast(-1,""..player.name.." is attempting to hack into the meeting room.")
			player:warp(1,15,15)
			--SEND TO JAIL
		end
	elseif (m >= 60000) then
		doInstanceSpawns(player)
	end
	--player:talk(0,"Entered")
end

mapLeave = function(player)
	local m = player.m
	
	instanceMapLeave(player)
	
	--player:sendMinitext("Leaving")
	--player:talk(0,"Left")
end

onScriptedTile = function(player)
	--Runs before warps
	local x = player.x
	local y = player.y
	local m = player.m
	local level = player.level
	--player:talk(0,"Map: "..m.." X: "..x.." Y: "..y)
	--[[if (player.state == 0) then
		player.steps = player.steps + 1
	end]]--
	
	--TEST LINE
	--player:talk(2,"DS: "..player.dmgShield.." PD: "..player.physDeduct)
	onSign(player, 2)
	
	if (m==10001) then--TILE BROWSER
		local j = getTile(m,x,y)
		if (j ~= 0) then
			player:sendMinitext("Tile: "..j)
		end
	elseif (m >= 60000) then--INSTANCES
		checkInstanceWarps(player)
	end
end

onTurn = function(player)
	onSign(player, 3)
end