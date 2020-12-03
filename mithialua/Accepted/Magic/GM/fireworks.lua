fireworks = {
cast = function(player, target)
	if (target ~= nil) then
		target:setDuration("fireworks", 5*60*1000)
		--player.mapRegistry['fireworks'] = os.time()
		xtable = {}
		ytable = {}
	end
end,

while_cast = function(block)
	fireworks.fireworks(block)
end,

while_cast_250 = function(block)
	if (os.time() %2 == 0) then
		fireworks.fireworks(block)
	end
end,

fireworks = function(block)
	local anim = {278, 337, 389}
	local times = math.random(8, 24)
	local r = math.random(1,3)
	
	if (xtable == nil) then
		xtable = {}
		ytable = {}
	end
	
	--if (launch == nil) then
	local launch = block.registry["fireworks"]
	--end
	
	--if (os.time() % 2 == 0) then
		if (launch == 0) then
			for i = 1, times do
				local x = math.random(block.x - 9, block.x + 9)
				local y = math.random(block.y - 8, block.y + 8)
			
				table.insert(xtable, i, x)
				table.insert(ytable, i, y)
			
				block:sendAnimationXY(131, xtable[i], ytable[i])
			end
			--block:talk(0," here")
			block.registry["fireworks"] = 1
		else
			--block:talk(0," here2")
			for i = 1, #xtable do
				block:sendAnimationXY(anim[r], xtable[i], ytable[i])
				table.remove(xtable, i)
				table.remove(ytable, i)
			end
			
			block.registry["fireworks"] = 0
		end
	--end
end,
uncast = function(block)
	block.registry["fireworks"] = 0
	xtable = nil
	ytable = nil
end
}