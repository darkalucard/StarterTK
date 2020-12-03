bot = {
check = function(target)
	--lastAction check
	
	if (target.gmLevel > 0 or target.afk == true) then
		target:gmMsg(""..target.name.." is a GM or is AFK.")
		return
	end

	if (target.afkTime > 1) then
		target:gmMsg(""..target.name.."'s last action was more than 10 seconds ago.")
		return
	end
	
	if (target.registry["botCheck"] > 0) then
		target:gmMsg(""..target.name.." is currently ongoing a bot check.")
		return
	end
	
	if (target.coRef > 0) then
		target:gmMsg(""..target.name.." is in async. (Busy)")
		target:freeAsync()
		target:gmMsg(""..target.name.." has been removed from async.")
		--return
	end
	
	bot.mathCheck(target)
end,

mathCheck = async(function(target)
	target.registry["botCheckTimer"] = os.time() + 30
	target.registry["botCheck"] = 1
	target:setTimer(2, 30)
	local str = ""
	local i = 0
	local rand1 = math.random(5,7)
	local rand2 = math.random(6,10)
	local check = ""..rand1.." + "..rand2.."?"
	local answer = tostring(rand1 + rand2)
	
	target.dialogType = 0
	
	repeat
		str = tostring(target:input("Submit your answer "..check))
		i = i + 1
		target:gmMsg("Bot Check #"..i.." ("..target.name.."): "..str)
		if (target.registry["botCheck"] == 0) then
			target:gmMsg("Bot Check for "..target.name.." expired on attempt #"..i..".")
			addChatLog(target, "Bot Check for "..target.name.." expired on attempt #"..i..".")
			--[[
			target:freeAsync()
			bot.mathCheck(target)
			]]--
		end
		if (i % 10 == 0) then
			addChatLog(target, ""..target.name.." answered bot check on attempt #"..i.." with answer: "..answer..".")
		end
	until (str == answer or target.registry["botCheckTimer"] < os.time())
	
	if (target.registry["botCheck"] == 1 and str == answer) then-- and target.registry["botCheckTimer"] > os.time()) then
		target:gmMsg(""..target.name.." answered bot check on attempt #"..i.." with correct answer: "..answer..".")
		addChatLog(target, ""..target.name.." answered bot check on attempt #"..i.." with correct answer: "..answer..".")
		target.registry["botCheck"] = 0
		target.registry["botCheckTimer"] = 0
		target:setTimer(2, 0)
	else
		target:gmMsg(""..target.name.." failed bot check on attempt #"..i..".")
		addChatLog(target, ""..target.name.." failed bot check on attempt #"..i..".")
	end
end)
}