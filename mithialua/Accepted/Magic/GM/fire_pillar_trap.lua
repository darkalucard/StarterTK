fire_pillar_trap = {
cast = function(player)
	local h = {}

	player:addNPC("fire_pillar_trap", player.m, player.x, player.y, 0, 180000,player.ID)
	player:sendAction(6,10)
	h = player:getObjectsInCell(player.m, player.x, player.y, BL_ITEM)
	
	if (#h < 1) then
		player:dropItem(130,1)
	end
end,




click = function(target,npc)
	local x = npc.x
	local y = npc.y
	local g = {}
	local h, i, j
	local x1 = {x-1,x,x+1,x,x}
	local y1 = {y,y,y,y-1,y+1}
	local item
	
	for i = 1,5 do
		g = npc:getObjectsInCell(npc.m, x1[i], y1[i], BL_PC)
		for j = 1,#g do
			if (g[j].pvp > 0 or g[j].PK > 0 or Player(npc.owner):getPK(g[j].ID) == true) then
				Player(g[j].ID).attacker = npc.owner
				g[j]:removeHealth(2)
				g[j]:sendAnimation(195)
			end
		end
	end
	
	for i = 1,5 do
		g = npc:getObjectsInCell(npc.m, x1[i], y1[i], BL_MOB)
		for j = 1,#g do
			Mob(g[j].ID).attacker = npc.owner
			g[j]:removeHealth(2)
			g[j]:sendAnimation(195)
		end
	end
	
	g = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_NPC)
	
	if (#g == 1) then
		g = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
		
		for i = 1, #g do
			item = FloorItem(g[i].ID)
			
			if (Item(item.id).type == 20) then
				g[i]:delete()
				break
			end
		end
	else
		for i = 1, #g do
			h = i
			
			if (string.find(NPC(g[i].ID).name,"trap")) then
				break
			elseif (h == #g) then
				g = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
				
				for i = 1, #g do
					item = FloorItem(g[i].ID)
					
					if (Item(item.id).type == 20) then
						g[i]:delete()
						break
					end
				end
			end
		end
	end
	--target.paralyzed=true--remove
	--target:setDuration("stop",3000)--remove
	target:sendAnimation(285)
	npc:delete()
end,

action = function(npc,player)
end,

endAction = function(npc, player)
	local g = {}
	
	g = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_NPC)
	
	if (#g == 1) then
		g = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
		
		for i = 1, #g do
			item = FloorItem(g[i].ID)
			
			if (Item(item.id).type == 20) then
				g[i]:delete()
				break
			end
		end
	else
		for i = 1, #g do
			h = i
			
			if (string.find(NPC(g[i].ID).name,"trap")) then
				break
			elseif (h == #g) then
				g = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
				
				for i = 1, #g do
					item = FloorItem(g[i].ID)
					
					if (Item(item.id).type == 20) then
						g[i]:delete()
						break
					end
				end
			end
		end
	end
	
	npc:delete()
end
}