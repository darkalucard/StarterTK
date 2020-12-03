direction_warp = {
	cast=function(player,target)
	local text=player.question
	local x, y, m
	m = player.m
	text = text * 1
		if(text~=nil) then
			if (player.side == 0) then
				y = player.y - text
				x = player.x
			elseif (player.side == 1) then
				y = player.y
				x = player.x + text
			elseif (player.side == 2) then
				y = player.y + text
				x = player.x
			else
				y = player.y
				x = player.x - text
			end
		else
			return
			player:sendMinitext("Please enter a numerical value.")
		end
		if (x < 0) or (x > player.xmax) or (y < 0) or (y > player.ymax) then
			player:sendMinitext("Error, can not leave map boundaries.")
			return
		end
		player:warp(player.m, x,y)
		player:sendAction(6,35)             
	end
}