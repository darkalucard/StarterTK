getFreeInstance = function(maps)
	if (#instances == 0) then
		for i = 1, maps do
			table.insert(instances, 60000 + i - 1)
		end
		
		return 60000
	else
		compare = function(a, b)
			return a < b
		end
		
		table.sort(instances, compare)
		
		if (instances[1] ~= 60000 and instances[1] - 60000 > maps) then
			for i = 1, maps do
				table.insert(instances, 60000 + i - 1)
			end
			
			return 60000
		else
			for i = 1, #instances do
				if (i < #instances and instances[i + 1] - instances[i] > maps) then
					for j = 1, maps do
						table.insert(instances, instances[i] + j)
					end
					
					return instances[i] + 1
				end
			end
			
			if (#instances + maps < 2000) then
				for i = 1, maps do
					table.insert(instances, instances[#instances] + 1)
				end
				
				return instances[#instances] - (maps - 1)
			end
		end
	end
	
	return false
end

loadInstance = function(map, instanceName)
	local mapFile = "../mithiamaps/Accepted/"
	local warpFile = "../mithiamaps/Accepted/instances/"
	
	if (instanceName == "ruins") then
		setMap(map, mapFile.."region2/ruins/r2_ruins.map", "Ruins", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 1, mapFile.."region2/ruins/r2_ruins_eastwing.map", "East Wing", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 2, mapFile.."region2/ruins/r2_ruins_westwing.map", "West Wing", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 3, mapFile.."region2/ruins/r2_ruins_hall.map", "Hoary Hallway", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 4, mapFile.."region2/ruins/r2_ruins_passage_nostairs.map", "Precarious Passage", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		warpFile = warpFile.."region2/ruins.txt"
	elseif (instanceName == "canidae") then
		setMap(map, mapFile.."region2/canidae/r2_ce_merry.map", "Merry Forest", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 1, mapFile.."region2/canidae/r2_ce_cheerful.map", "Cheerful Forest", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 2, mapFile.."region2/canidae/r2_ce_joyous.map", "Joyous Forest", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 3, mapFile.."region2/canidae/r2_ce_glade.map", "Glade", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 4, mapFile.."region2/canidae/r2_ce_twisting.map", "Twisting Pine", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		setMap(map + 5, mapFile.."region2/canidae/r2_ce_clearing.map", "Clearing", 113, 0, 1, 0, 0, 3600000, 0, 0, 2, 1, 0, 0)
		warpFile = warpFile.."region2/canidae.txt"
	end
	
	if (setWarps(map, warpFile) ~= false) then
		return true
	end
end

unloadInstance = function(map, maps)
	local comparison = function(a, b)
		return a < b
	end
	
	for i = 1, #instances do
		if (instances[i] == map) then
			for j = 1, maps do
				removeInstanceBlocks(map + j - 1)
				table.remove(instances, i)
			end
		end
	end
	
	table.sort(instances, comparison)
end

removeInstanceBlocks = function(map)
	local blocks = f1Block:getObjectsInMap(map, BL_ALL)
	
	if (#blocks > 0) then
		for i = 1, #blocks do
			if (blocks[i].blType ~= BL_MOB) then
				blocks[i]:delete()
			end
		end
	end
end