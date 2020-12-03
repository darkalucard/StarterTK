filler = {
fill = function(player)
	local x = player.x
	local y = player.y
	local m = player.m
	local baseTile = getTile(m, x, y)
	local newTile = 7
end,

first = function(player, baseTile)
	local x = player.x
	local y = player.y
	local m = player.m
	local baseTile = getTile(m, x, y)
	local newTile = 7
	local coords = {xx = {x}, yy = {y}}
	local oldCoords = {xx = {}, yy = {}}
	filler_coords = {xx = {}, yy = {}}
	filler_coords.xx = {}
	filler_coords.yy = {}
	table.insert(coords.xx, x+1)
	table.insert(coords.yy, y)
	table.insert(coords.xx, x-1)
	table.insert(coords.yy, y)
	table.insert(coords.xx, x)
	table.insert(coords.yy, y+1)
	table.insert(coords.xx, x)
	table.insert(coords.yy, y-1)
	if (#coords.xx > 0) then
		for i = 1, #coords.xx do
			if (getTile(m, coords.xx[i], coords.yy[i]) == baseTile) then
				setTile(m, coords.xx[i], coords.yy[i], newTile)
				table.insert(oldCoords.xx, coords.xx[i])
				table.insert(oldCoords.yy, coords.yy[i])
			end
		end
		filler.repeatable(player, baseTile, newTile, oldCoords)--call cross function
	end
end,

repeatable = function(player, baseTile, newTile, coords)
	local m = player.m
	local newCoords = {xx = {}, yy = {}}
	local oldCoords = {xx = {}, yy = {}}
	if (#coords.xx > 0) then
		for i = 1, #coords.xx do
			table.insert(newCoords.xx, coords.xx[i] + 1)
			table.insert(newCoords.yy, coords.yy[i])
			table.insert(newCoords.xx, coords.xx[i] - 1)
			table.insert(newCoords.yy, coords.yy[i])
			table.insert(newCoords.xx, coords.xx[i])
			table.insert(newCoords.yy, coords.yy[i] + 1)
			table.insert(newCoords.xx, coords.xx[i])
			table.insert(newCoords.yy, coords.yy[i] - 1)
			if (getTile(m, newCoords.xx[i], newCoords.yy[i]) == baseTile) then
				setTile(m, newCoords.xx[i], newCoords.yy[i], newTile)
				table.insert(oldCoords.xx, newCoords.xx[i])
				table.insert(oldCoords.yy, newCoords.yy[i])
			else
				filler.repeatable2(player, baseTile, newTile, newCoords)
			end
		end
		--player:talk(0,""..oldCoords.xx[1].." "..oldCoords.yy[1])
		filler.repeatable2(player, baseTile, newTile, oldCoords)
	end
end,

repeatable2 = function(player, baseTile, newTile, coords)
	local m = player.m
	local newCoords = {xx = {}, yy = {}}
	local oldCoords = {xx = {}, yy = {}}
	if (#coords.xx > 0) then
		for i = 1, #coords.xx do
			table.insert(newCoords.xx, coords.xx[i] + 1)
			table.insert(newCoords.yy, coords.yy[i])
			table.insert(newCoords.xx, coords.xx[i] - 1)
			table.insert(newCoords.yy, coords.yy[i])
			table.insert(newCoords.xx, coords.xx[i])
			table.insert(newCoords.yy, coords.yy[i] + 1)
			table.insert(newCoords.xx, coords.xx[i])
			table.insert(newCoords.yy, coords.yy[i] - 1)
			if (getTile(m, newCoords.xx[i], newCoords.yy[i]) == baseTile) then
				setTile(m, newCoords.xx[i], newCoords.yy[i], newTile)
				table.insert(oldCoords.xx, newCoords.xx[i])
				table.insert(oldCoords.yy, newCoords.yy[i])
			end
		end
		for i = 1, 100 do
			filler.coords(player, oldCoords)
			--player:talk(0,""..i)
			filler.repeatable3(player, baseTile, newTile, filler_coords)
			filler.repeatable2(player, baseTile, newTile, filler_coords)
			filler.repeatable(player, baseTile, newTile, filler_coords)
		end
	end
end,

repeatable3 = function(player, baseTile, newTile, filler_coords)
	local m = player.m
	if (#filler_coords.xx > 0) then
		for i = 1, #filler_coords.xx do
			if (getTile(m, filler_coords.xx[i], filler_coords.yy[i]) == baseTile) then
				setTile(m, filler_coords.xx[i], filler_coords.yy[i], newTile)
			end
		end
		filler.repeatable2(player, baseTile, newTile, filler_coords)
	end
end,

coords = function(player, oldCoords)
	if (#oldCoords.xx > 0) then
		for	i = 1, #oldCoords.xx do
			table.insert(filler_coords.xx, oldCoords.xx[i])
			table.insert(filler_coords.yy, oldCoords.yy[i])
			--
			table.insert(filler_coords.xx, oldCoords.xx[i] + 1)
			table.insert(filler_coords.yy, oldCoords.yy[i])
			table.insert(filler_coords.xx, oldCoords.xx[i] - 1)
			table.insert(filler_coords.yy, oldCoords.yy[i])
			table.insert(filler_coords.xx, oldCoords.xx[i])
			table.insert(filler_coords.yy, oldCoords.yy[i] + 1)
			table.insert(filler_coords.xx, oldCoords.xx[i])
			table.insert(filler_coords.yy, oldCoords.yy[i] - 1)
			player:talk(0,""..#oldCoords.xx.." filler "..#filler_coords.xx.." #filler "..#filler_coords)
		end
		for i = 1, #filler_coords.xx do
			for j = 1, #filler_coords.xx do
				if (filler_coords.xx[i] == filler_coords.yy[j]) then
					table.remove(filler_coords.xx, j)
					table.remove(filler_coords.yy, j)
				end
			end
		end
	else
		player:talk(2, "Finished? o.o")
	end
end
}