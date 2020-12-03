firstvoyage = {
cast = function(player)
	if (player:hasDuration("firstvoyage")) then
		return
	end
	
	player:setDuration("firstvoyage", 15000)
end,

while_cast = function(player)
	--ani 27 to 30
	local strikeX, strikeY, mobBlocks
	local rangeX = math.random(-7, 7)
	local rangeY = math.random(-7, 7)
	local loop = 1
	local i = math.random(3, 7)

	while loop <= i do
		rangeX = math.random(-7, 7)
		rangeY = math.random(-7, 7)
		
		if (rangeX == 0 and rangeY == 0) then
			local coord = math.random(0, 1)
			
			if (coord == 0) then
				while rangeX == 0 do
					rangeX = math.random(-7, 7)
				end
			else
				while rangeY == 0 do
					rangeY = math.random(-7, 7)
				end
			end
		end
		
		strikeX = player.x + rangeX
		strikeY = player.y + rangeY
		mobBlocks = player:getObjectsInCell(player.m, strikeX, strikeY, BL_MOB)
		
		if (#mobBlocks == 0) then
			local animation = math.random(27, 30)
			
			player:sendAnimationXY(290, strikeX, strikeY)
			player:sendAnimationXY(animation, strikeX, strikeY)
			loop = loop + 1
		end
	end
end
}