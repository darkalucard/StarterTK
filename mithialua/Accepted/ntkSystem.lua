powerBoard = function(player,target)
	if((player.m ~= 7001 and player.gmLevel < 50) or (player.m == 7001 and player.registry["arenaHost"] == 0 and player.gmLevel < 50)) then
		player:sendMinitext("You may not call for powerboards on this map.")
		return
	end
	if(target~=nil) then
		if(player.pbColor==60 or player.pbColor==61 or player.pbColor==63 or player.pbColor==65 or player.pbColor==0 or player.gmLevel==99) then
			target.armorColor=player.pbColor
			target:updateState()
		end
	end
	player:powerBoard()
	--use player.pbColor for the color to change
	--also, set the armorColor yourself, and udpate their state
end