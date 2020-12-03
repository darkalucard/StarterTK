fox_cage = {
move = function(mob)
	local mobBlocks = getTargetsAround(mob, BL_MOB)
	
	if (#mobBlocks > 0) then
		for i = 1, #mobBlocks do
			if (mobBlocks[i].mobID == 141) then
				if (getDirection(mob, mobBlocks[i]) == "west") then
					fox_cage.capture(mob, mobBlocks[i])--CAPTURE
					break
				end
			end
		end
	end

end,

capture = function(mob, target)
	local pcBlocks = mob:getObjectsInArea(BL_PC)
	local holder
	
	if (#pcBlocks > 0) then
		for i = 1, #pcBlocks do
			if (distance(mob, pcBlocks[i]) <= 2) then
				holder = pcBlocks[i]
			end
		end
		if (holder ~= nil) then
			if (target.blType == BL_MOB and holder.quest["fox_lure"] == 0) then
				holder:talk(2, "Fox trapped!")
				holder:removeItem("fox_lure", 1, 6, 0)
				holder:addItem("fox_lure", 1, 0, holder.ID, ""..holder.name.."'s fox lure")
				holder.quest["fox_lure"] = 249
				target:removeHealth(target.health - 1)
				target:removeHealth(1)
				mob:removeHealth(mob.health - 1)
				mob:removeHealth(1)
				--registry
			end
		end
	end
end
}