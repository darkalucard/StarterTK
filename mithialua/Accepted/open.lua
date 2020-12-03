onOpen = function(player)
	local side = player.side
	local m = player.m
	local x = player.x
	local y = player.y
	
	if (side == 0) then
		y = y - 1
	elseif (side == 1) then
		x = x + 1
	elseif (side == 2) then
		y = y + 1
	elseif (side == 3) then
		x = x - 1
	else
		broadcast(-1,""..player.name.." is hacking.")
	end

	local obj = getObject(m, x, y)
	local pass = getPass(m, x, y)
	
	--By Coords
	if (m == 30) or (m == 31) then--Sheperding town
		if (y == 69) and ((x == 9) or (x == 10)) then
			if (player:hasItem("r1_shep_town_key", 1) == true) then
				if (obj == 8191) then
					setObject(30, 9, y, 8194)
					setObject(30, 10, y, 0)
					setObject(31, 9, y, 8194)
					setObject(31, 10, y, 0)
					return
				elseif (obj == 0) or (obj == 8194) then
					setObject(30, 9, y, 8191)
					setObject(30, 10, y, 8191)
					setObject(31, 9, y, 8191)
					setObject(31, 10, y, 8191)
					return
				end
			else
				player:sendMinitext("These gates are locked.")
				return
			end
		end
	elseif (m == 40) then
		if (x == 3) then
			if (y >= 52) and (y <= 54) then
				if (pass == 1) then
					for y = 52, 54 do
						setPass(m, x, y, 0)
					end
				else
					for y = 52, 54 do
						setPass(m, x, y, 1)
					end
				end
			end
		elseif (x == 154) then
			if (y >= 52) and (y <= 54) then
				if (pass == 1) then
					for y = 52, 54 do
						setPass(m, x, y, 0)
					end
				else
					for y = 52, 54 do
						setPass(m, x, y, 1)
					end
				end
			end
		end
	end
		
	--By Object
	openDoors(player)
end

function openDoors(player)
	local side = player.side
	local m = player.m
	local x = player.x
	local y = player.y
	
	if (side == 0) then
		y = y - 1
	elseif (side == 1) then
		x = x + 1
	elseif (side == 2) then
		y = y + 1
	elseif (side == 3) then
		x = x - 1
	else
		broadcast(-1,""..player.name.." is hacking.")
	end
	
	local obj = getObject(m, x, y)
	
	if (m == 42) then
		if ((x == 9 and y == 3 ) or (x == 10 and y == 3)) then
			if (obj == 2545) then
				setObject(m, x - 1, y, 2545)
				setObject(m, x, y, 0)
				setObject(m, x + 1, y, 0)
				setObject(m, x + 2, y, 2551)
			elseif (obj == 2551) then
				setObject(m, x - 2, y, 2545)
				setObject(m, x - 1, y, 0)
				setObject(m, x, y, 0)
				setObject(m, x + 1, y, 2551)
			elseif (obj == 0 and x == 9) then
				setObject(m, x - 1, y, 2544)
				setObject(m, x, y, 2545)
				setObject(m, x + 1, y, 2551)
				setObject(m, x + 2, y, 2552)
			elseif (obj == 0 and x == 10) then
				setObject(m, x - 2, y, 2544)
				setObject(m, x - 1, y, 2545)
				setObject(m, x, y, 2551)
				setObject(m, x + 1, y, 2552)
			end
		end
	end
	
	if (obj == 51) then
		setObject(m, x, y, 114)
	elseif (obj == 57) then
		setObject(m, x, y, 115)
	elseif (obj == 53) then
		setObject(m, x, y, 102)
		setObject(m, x + 1, y, 103)
		setObject(m, x + 2, y, 104)
	elseif (obj == 54) then
		setObject(m, x - 1, y, 102)
		setObject(m, x, y, 103)
		setObject(m, x + 1, y, 104)
	elseif (obj == 55) then
		setObject(m, x - 2, y, 102)
		setObject(m, x - 1, y, 103)
		setObject(m, x, y, 104)
	elseif (obj == 76) then
		setObject(m, x, y, 116)
	elseif (obj == 78) then
		setObject(m, x, y, 105)
		setObject(m, x + 1, y, 106)
		setObject(m, x + 2, y, 107)
	elseif (obj == 79) then
		setObject(m, x - 1, y, 105)
		setObject(m, x, y, 106)
		setObject(m, x + 1, y, 107)
	elseif (obj == 80) then
		setObject(m, x, y, 105)
		setObject(m, x + 1, y, 106)
		setObject(m, x + 2, y, 107)
	elseif (obj == 82) then
		setObject(m, x, y, 117)
	elseif (obj == 97) then
		setObject(m, x - 1, y, 108)
		setObject(m, x, y, 109)
		setObject(m, x + 1, y, 110)
	elseif (obj == 100) then
		setObject(m, x - 1, y, 111)
		setObject(m, x, y, 112)
		setObject(m, x + 1, y, 113)
	elseif (obj == 102) then
		setObject(m, x, y, 53)
		setObject(m, x + 1, y, 54)
		setObject(m, x + 2, y, 55)
	elseif (obj == 103) then
		setObject(m, x - 1, y, 53)
		setObject(m, x, y, 54)
		setObject(m, x + 1, y, 55)
	elseif (obj == 104) then
		setObject(m, x - 2, y, 53)
		setObject(m, x - 1, y, 54)
		setObject(m, x, y, 55)
	elseif (obj == 105) then
		setObject(m, x, y, 78)
		setObject(m, x + 1, y, 79)
		setObject(m, x + 2, y, 80)
	elseif (obj == 106) then
		setObject(m, x - 1, y, 78)
		setObject(m, x, y, 79)
		setObject(m, x + 1, y, 80)
	elseif (obj == 107) then
		setObject(m, x - 2, y, 78)
		setObject(m, x - 1, y, 79)
		setObject(m, x, y, 80)
	elseif (obj == 109) then
		setObject(m, x - 1, y, 96)
		setObject(m, x, y, 97)
		setObject(m, x + 1, y, 98)
	elseif (obj == 112) then
		setObject(m, x - 1, y, 99)
		setObject(m, x, y, 100)
		setObject(m, x + 1, y, 101)
	elseif (obj == 114) then
		setObject(m, x, y, 51)
	elseif (obj == 115) then
		setObject(m, x, y, 57)
	elseif (obj == 116) then
		setObject(m, x, y, 76)
	elseif (obj == 117) then
		setObject(m, x, y, 82)
	elseif (obj >= 340) and (obj <= 341) then
		setObject(m, x, y, obj + 20)
	elseif (obj == 342) or (obj == 343) then
		setObject(m, x, y, obj + 22)
	elseif (obj == 344) or (obj == 345) then
		setObject(m, x, y, obj + 18)
	elseif (obj == 346) or (obj == 347) then
		setObject(m, x, y, obj + 20)
	elseif (obj >= 348) and (obj <= 349) then
		setObject(m, x, y, obj + 22)
	elseif (obj >= 350) and (obj <= 353) then
		setObject(m, x, y, obj + 24)
	elseif (obj == 354) or (obj == 355) then
		setObject(m, x, y, obj + 14)
	elseif (obj >= 360) and (obj <= 361) then
		setObject(m, x, y, obj - 20)
	elseif (obj == 362) or (obj == 363) then
		setObject(m, x, y, obj - 18)
	elseif (obj == 364) or (obj == 365) then
		setObject(m, x, y, obj - 22)
	elseif (obj == 366) or (obj == 367) then
		setObject(m, x, y, obj - 20)
	elseif (obj == 368) or (obj == 369) then
		setObject(m, x, y, obj - 14)
	elseif (obj >= 370) and (obj <= 371) then
		setObject(m, x, y, obj - 22)
	elseif (obj >= 374) and (obj <= 377) then
		setObject(m, x, y, obj - 24)
	elseif (obj == 378) or (obj == 379) then
		setObject(m, x, y, obj + 16)
	elseif (obj == 380) or (obj == 381) then
		setObject(m, x, y, obj + 107)
	elseif (obj == 394) or (obj == 395) then
		setObject(m, x, y, obj - 16)
	elseif (obj == 487) or (obj == 488) then
		setObject(m, x, y, obj - 107)
	elseif (obj == 17408) then
		setObject(m, x - 1, y, 17416)
		setObject(m, x, y, 17417)
		setObject(m, x + 1, y, 17418)
	elseif (obj == 17417) then
		setObject(m, x - 1, y, 17407)
		setObject(m, x, y, 17408)
		setObject(m, x + 1, y, 17409)
	elseif (obj == 17423) then
		setObject(m, x, y, 17428)
		setObject(m, x + 1, y, 17429)
	elseif (obj == 17425) then
		setObject(m, x, y, 17430)
		setObject(m, x + 1, y, 17431)
	elseif (obj == 17428) then
		setObject(m, x, y, 17423)
		setObject(m, x + 1, y, 17424)
	elseif (obj == 17430) then
		setObject(m, x, y, 17425)
		setObject(m, x + 1, y, 17426)
	elseif (obj == 17670) then
		for i = 0, 3 do
			setObject(m, x + i, y, 17680 + i)
		end
	elseif (obj == 17671) then
		for i = 0, 3 do
			setObject(m, x - 1 + i, y, 17680 + i)
		end
	elseif (obj == 17672) then
		for i = 0, 3 do
			setObject(m, x - 2 + i, y, 17680 + i)
		end
	elseif (obj == 17673) then
		for i = 0, 3 do
			setObject(m, x - 3 + i, y, 17680 + i)
		end
	elseif (obj == 17680) then
		for i = 0, 3 do
			setObject(m, x + i, y, 17670 + i)
		end
	elseif (obj == 17681) then
		for i = 0, 3 do
			setObject(m, x - 1 + i, y, 17670 + i)
		end
	elseif (obj == 17682) then
		for i = 0, 3 do
			setObject(m, x - 2 + i, y, 17670 + i)
		end
	elseif (obj == 17683) then
		for i = 0, 3 do
			setObject(m, x - 3 + i, y, 17670 + i)
		end
	end
end