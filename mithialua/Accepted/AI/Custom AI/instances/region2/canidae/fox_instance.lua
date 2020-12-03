fox_instance = {
on_spawn = function(mob)
	local mapTitle = string.lower(mob.mapTitle)
	local x = mob.x
	local y = mob.y
	
	if (mapTitle == "merry forest") then--1st and 5th room
		if (x >= 2 and x <= 27 and y >= 2 and y <= 10) then--1st
			fox_instance.randomBoundaries(mob, {2, 27}, {2, 10})
		elseif (y >= 17) then--5th
			fox_instance.randomBoundaries(mob, {2, 28}, {17, 28})
		end
	elseif (mapTitle == "cheerful forest") then--2nd room
		fox_instance.randomBoundaries(mob)
	elseif (mapTitle == "joyous forest") then--3rd room
		fox_instance.randomBoundaries(mob)
	elseif (mapTitle == "glade") then--Fake boss
	
	elseif (mapTitle == "twisting pine") then--4th room
		fox_instance.randomBoundaries(mob, {1, 28}, {2, 28})
	elseif (mapTitle == "clearing") then
	end
end,

randomBoundaries = function(mob, xBoundaries, yBoundaries)
	local limit = 100
	local m = mob.m
	local x = 0
	local y = 0
	local i = 0
	if (xBoundaries == nil or #xBoundaries == 0) then
		xBoundaries = {0, mob.xmax}
	end
	if (yBoundaries == nil or #yBoundaries == 0) then
		yBoundaries = {0, mob.ymax}
	end
	repeat
		x = math.random(xBoundaries[1], xBoundaries[2])
		y = math.random(yBoundaries[1], yBoundaries[2])
		i = i + 1
	until ((getPass(m, x, y) == 0 and getTile(m, x, y) == 43
	and #mob:getObjectsInCell(m, x, y, BL_PC) == 0 and #mob:getObjectsInCell(m, x, y, BL_MOB) == 0 ) or i == limit)
	if (x == 0 and y == 0) then
		--Do not warp, spawn on spawn coords.
	else
		mob:warp(m, x, y)
	end
end,

after_death = function(mob, block)
	--if (mob.m >= 6000) then
		mobs = mob:getObjectsInSameMap(BL_MOB)
		if (#mobs > 0) then
			broadcast(mob.m, "There are "..#mobs.." left to kill before you can advance to the next room.")
		else
			broadcast(mob.m, "No more foxes are left, you may advance to the next room.")
		end
	--end
	--use to enable next rooms?
end,

spawn = function(block)
--[[
	local mapTitle = string.lower(block.mapTitle)
	local merry = 87
	local cheerful = 88
	local joyous = 89
	local swift = 90
	local striking = 91
	local sly = 92
	local fake = 137
	local ninetails = 93
	
	local group = #block.group
	if (group > 8) then
		group = ((group - 8) / 10) + 1
	else
		group = 1
	end
	if (mapTitle == "merry forest") then--1st and 5th room
		block:spawn(merry, 7, 6, math.floor(7 * group), block.m)
		block:spawn(cheerful, 7, 6, math.floor(5 * group), block.m)
		block:spawn(joyous, 7, 6, math.floor(4 * group), block.m)
		block:spawn(swift, 7, 6, math.floor(3 * group), block.m)
		block:spawn(striking, 7, 6, math.floor(3 * group), block.m)
		block:spawn(sly, 7, 6, math.floor(2 * group), block.m)
		--Fifth room v v
		block:spawn(merry, 8, 22, math.floor(5 * group), block.m)
		block:spawn(cheerful, 8, 22, math.floor(5 * group), block.m)
		block:spawn(joyous, 8, 22, math.floor(2 * group), block.m)
		block:spawn(swift, 8, 22, math.floor(5 * group), block.m)
		block:spawn(striking, 8, 22, math.floor(5 * group), block.m)
		block:spawn(sly, 8, 22, math.floor(3 * group), block.m)
	elseif (mapTitle == "cheerful forest") then--2nd room
		block:spawn(merry, 8, 22, math.floor(7 * group), block.m)
		block:spawn(cheerful, 8, 22, math.floor(10 * group), block.m)
		block:spawn(joyous, 8, 22, math.floor(8 * group), block.m)
		block:spawn(swift, 8, 22, math.floor(8 * group), block.m)
		block:spawn(striking, 8, 22, math.floor(8 * group), block.m)
		block:spawn(sly, 8, 22, math.floor(4 * group), block.m)
	elseif (mapTitle == "joyous forest") then--3rd room
		block:spawn(merry, 8, 22, math.floor(5 * group), block.m)
		block:spawn(cheerful, 8, 22, math.floor(6 * group), block.m)
		block:spawn(joyous, 8, 22, math.floor(8 * group), block.m)
		block:spawn(swift, 8, 22, math.floor(7 * group), block.m)
		block:spawn(striking, 8, 22, math.floor(7 * group), block.m)
		block:spawn(sly, 8, 22, math.floor(4 * group), block.m)
	elseif (mapTitle == "glade") then--Fake boss
		block:spawn(fake, 8, 6, 1, block.m)
	elseif (mapTitle == "twisting pine") then--4th room
		block:spawn(merry, 8, 22, math.floor(8 * group), block.m)
		block:spawn(cheerful, 8, 22, math.floor(11 * group), block.m)
		block:spawn(joyous, 8, 22, math.floor(8 * group), block.m)
		block:spawn(swift, 8, 22, math.floor(9 * group), block.m)
		block:spawn(striking, 8, 22, math.floor(9 * group), block.m)
		block:spawn(sly, 8, 22, math.floor(4 * group), block.m)
	elseif (mapTitle == "clearing") then
		block:spawn(ninetails, 9, 9, 1, block.m)	
	end	
	]]--
end
}