map_board = {
check=function(player)
	local del = 0
	local write = 0
	if (player:staff("map",3) >= 3) then
		del = 1
	end

	if (player:staff("map") > 0) then
		write = 1
	end
	
	player.boardDel = del
	player.boardWrite = write
end
}