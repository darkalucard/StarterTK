fox_ball = {
on_spawn = function(mob)
	mob:setDuration("fox_ball_duration", 120000)
	
	if (mob.mapRegistry[mob.yname.."s"] <= 0) then
		mob.mapRegistry[mob.yname.."s"] = 1
	else
		mob.mapRegistry[mob.yname.."s"] = mob.mapRegistry[mob.yname.."s"] + 1
	end
end,

attack = function(mob, target)
	local moved
	local block
	
	if(target) then
		threat.calcHighestThreat(mob)
		block = mob:getBlock(mob.target)
		
		if (block ~= nil) then
			target = block
		else
			mob:talk(2, "I have an invalid target, report me to a GM. (ID: "..mob.ID.." Target ID: "..mob.target)
		end
		
		moved=FindCoords(mob,target)
		
		if(moved and mob.target ~= mob.owner) then
			fox_ball.suicide(mob, target)
		else
			mob.state = MOB_ALIVE
		end
	else
		mob.state = MOB_ALIVE
	end
end,

suicide = function(mob, target)
	mob.attacker = target.ID
	mob:flushDuration(7023)
end
}

fox_ball_duration = {
while_cast = function(mob)
	local pcBlocks
	
	if (mob.mapRegistry["fox_ball_refresh"] == 0) then
		mob.mapRegistry["fox_ball_refresh"] = os.time() + 15
	elseif (mob.mapRegistry["fox_ball_refresh"] <= os.time) then
		pcBlocks = mob:getObjectsInSameMap(BL_PC)
		
		if (#pcBlocks > 0) then
			for i = 1, #pcBlocks do
				pcBlocks[i]:refresh()
			end
		end
		
		mob.mapRegistry["fox_ball_refresh"] = os.time() + 15
	end
end,

uncast = function(mob)
	mob.health = 0
	mob:removeHealth(mob.health)
	mob.mapRegistry[mob.yname.."s"] = mob.mapRegistry[mob.yname.."s"] - 1
end
}