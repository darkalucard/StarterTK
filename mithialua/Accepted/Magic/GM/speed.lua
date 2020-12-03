speed = {
	cast= async(function(player,target)
		local what=player:input("What")
		if(tostring(what)=="set") then
			local set=player:input("Increase, decrease or normalize?")
			if(set=="+") or (string.lower(set)=="increase") or (string.lower(set)=="inc") or (string.lower(set)=="i") then
				local amt = player:input("Increase by how much?")
				player.mapRegistry['spd'] = amt
				what = "use"
			elseif (set=="-") or (string.lower(set)=="decrease") or (string.lower(set)=="dec") or (string.lower(set)=="d")  then
				local amt = player:input("Decrease by how much?")
				amt = amt * -1
				player.mapRegistry['spd'] = amt
				what = "use"
			elseif (set=="=") or (string.lower(set)=="normalize") or (string.lower(set)=="norm") or (string.lower(set)=="n")  then
				player.mapRegistry['spd'] = 80
				what = "use"
			end
		end
		if(what=="use") then
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
	end)
}	