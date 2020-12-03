noling = {
click = async(function(player,npc)
	local t = {graphic = 0, color = 0}
	local g = {graphic = convertGraphic(596, "monster"), color = 27}
	local bc = {graphic = convertGraphic(2267, "item"), color = 20}
	local bw = {graphic = convertGraphic(604, "monster"), color = 4}
	local wc = {graphic = convertGraphic(428, "item"), color = 31}
	local f1 = {graphic = convertGraphic(838, "monster"), color = 0}
	
	player.dialogType = 1
	
	if (player.quest["tut_quest_noling"] == 0) then
		player:dialogSeq({t, "Looking for adventure, eh? Let's see if you've got what it takes.",
		g, "Find this girl and giver her a hand. She's looking really distressed lately."}, 1)
		player.quest["tut_quest_noling"] = 1
	elseif (player.quest["tut_quest_noling"] == 1 and player.quest["tut_quest_girl"] < 5) then
		player:dialogSeq({g, "Not up to the challenge? She's there, waiting."}, 0)
	elseif (player.quest["tut_quest_noling"] == 2) then
		if (player:hasItem(120, 1) == true) then
			player:dialogSeq({bc, "Ooh, that's a nice coconut you've found there. Watch this! -Noling strikes swiftly at the coconut-",
			bc, "Just stick your head in there and you've got some nice protective gear. This excess is waste. -Noling chucks the excess toward the trees-",
			m, "Don't worry, the squirrels will get it."}, 1)
			player:removeItem(120, 1)
			player:addItem(400000001, 1)
			player:giveXP(250)
		else
			player:dialogSeq({bc, "Looks like you've lost your coconut getting here. Better luck next time."}, 1)
			player.giveXP(175)
		end
		
		player.quest["tut_quest_noling"] = 3
		player.quest["tut_quest_girl"] = 6
	elseif (player.quest["tut_quest_noling"] == 3) then
		player:dialogSeq({bw, "Now for the fun stuff. Go find this guy up north by the cave. The island really needs your help on this."}, 1)
		player.quest["tut_quest_noling"] = 4
	elseif (player.quest["tut_quest_noling"] == 4 and player.quest["tut_quest_bird_worshipper"] < 3) then
		player:dialogSeq({bw, "Don't feel like helping? Well that's unfortunate... Better hop to it if you want off this island."}, 1)
	elseif (player.quest["tut_quest_noling"] == 5 or (player.quest["tut_quest_bird_worshipper"] == 3 and player.quest["tut_quest_noling"] < 6)) then
		if (player:hasItem(171, 1) == true) then
			player:dialogSeq({t, "Is that a weather crystal you've got there? These are very dangerous in the wrong hands. Check this out. -Noling crushes the hilt of his blade-",
			wc, "These can be used to change the weather in your area. Very deadly under certain circumstances."}, 1)
			player.mapRegistry["artificial_weather_timer"] = 0
			weather_crystal.use(player)
			player:addGold(350)
		end
		
		player.quest["tut_quest_noling"] = 6
		player:dialogSeq({t, "Now that you know what it takes to survive in this world, maybe you are ready to see the rest of it.",
		t, "Speak with me again and I shall send you on your way."}, 1)
	elseif (player.quest["tut_quest_noling"] == 6) then
		player:dialogSeq({t, "Are you fully prepared to leave? The seas are mighty rough these days.",
		f1, "This appears to be the end of the training, let's see what awaits us next.",
		t, "((Push next to set sail.))",
		t, "Off with ya, have a safe trip and may the weather bird watch over you."}, 1)
		
		if (player.level < 5) then
			player:dialogSeq({t, "I'm sorry but you're not quite ready to leave yet. You'll need a bit more experience than that.",
			t, "Look around the island and see if any of the natives need assistance."}, 1)
		else
			local map = tutorial_boat.getfreemap()
		
			if (map ~= nil) then
				tutorial_boat.startscene(player, map)
			else
				player:dialogSeq({t, "Sorry, all of our boats are out at the moment. Please try again soon."}, 1)
			end
		end
	end
	--10-20 Green cocos OR 3 yellow coco OR bug liver OR dung OR weather crystal
end),

action = function(npc)
	local moved = true
	local oldside = npc.side
	local checkmove = math.random(0, 10)
	local mob = {}
	local item = {}
	local t = curTime()
	
	if (t >= 20 or t <= 5) then
		if (npc.registry["light"] <= os.time() and npc.registry["light"] > 0) then
			npc.registry["light"] = 0
		end
		if (npc.registry["light"] == 0) then
			npc.registry["light"] = os.time() + 62
			npc:talk(2, "~lux~")
			npc:sendAnimation(135, 60)
		end
	end
	
	if(checkmove >= 4) then
		npc.side = math.random(0, 3)
		npc:sendSide()
	
		if(npc.side == oldside) then
			if (npc.side == 0) then
				if (distanceXY(npc, npc.startX, npc.startY + 1) <= 4) then
					mob = npc:getObjectsInCell(npc.m, npc.x, npc.y - 1, BL_MOB)
					if (#mob > 0) then
						npc:sendAction(1, 14)
						npc:playSound(2)
						Mob(mob[1].ID):removeHealth(Mob(mob[1].ID).health)
					end
					moved = npc:move()
					item = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
					if (#item > 0) then
						npc:sendAction(4, 80)
						item[1]:delete()
					end
				end
			elseif (npc.side == 1) then
				if (distanceXY(npc, npc.startX - 1, npc.startY) <= 4) then
					mob = npc:getObjectsInCell(npc.m, npc.x + 1, npc.y, BL_MOB)
					if (#mob > 0) then
						npc:sendAction(1, 14)
						npc:playSound(2)
						Mob(mob[1].ID):removeHealth(Mob(mob[1].ID).health)
					end
					moved = npc:move()
					item = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
					if (#item > 0) then
						npc:sendAction(4, 80)
						item[1]:delete()
					end
				end
			elseif (npc.side == 2) then
				if (distanceXY(npc, npc.startX, npc.startY - 1) <= 4) then
					mob = npc:getObjectsInCell(npc.m, npc.x, npc.y + 1, BL_MOB)
					if (#mob > 0) then
						npc:sendAction(1, 14)
						npc:playSound(2)
						Mob(mob[1].ID):removeHealth(Mob(mob[1].ID).health)
					end
					moved = npc:move()
					item = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
					if (#item > 0) then
						npc:sendAction(4, 80)
						item[1]:delete()
					end
				end
			elseif (npc.side == 3) then
				if (distanceXY(npc, npc.startX + 1, npc.startY) <= 4) then
					mob = npc:getObjectsInCell(npc.m, npc.x - 1, npc.y, BL_MOB)
					if (#mob > 0) then
						npc:sendAction(1, 14)
						npc:playSound(2)
						Mob(mob[1].ID):removeHealth(Mob(mob[1].ID).health)
					end
					moved = npc:move()
					item = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
					if (#item > 0) then
						npc:sendAction(4, 80)
						item[1]:delete()
					end
				end
			end
		end
	else
		if (npc.side == 0) then
			if (distanceXY(npc, npc.startX, npc.startY + 1) <= 4) then
				mob = npc:getObjectsInCell(npc.m, npc.x, npc.y - 1, BL_MOB)
				if (#mob > 0) then
					npc:sendAction(1, 14)
					npc:playSound(2)
					Mob(mob[1].ID):removeHealth(Mob(mob[1].ID).health)
				end
				moved = npc:move()
				item = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
				if (#item > 0) then
					npc:sendAction(4, 80)
					item[1]:delete()
				end
			end
		elseif (npc.side == 1) then
			if (distanceXY(npc, npc.startX - 1, npc.startY) <= 4) then
				mob = npc:getObjectsInCell(npc.m, npc.x + 1, npc.y, BL_MOB)
				if (#mob > 0) then
					npc:sendAction(1, 14)
					npc:playSound(2)
					Mob(mob[1].ID):removeHealth(Mob(mob[1].ID).health)
				end
				moved = npc:move()
				item = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
				if (#item > 0) then
					npc:sendAction(4, 80)
					item[1]:delete()
				end
			end
		elseif (npc.side == 2) then
			if (distanceXY(npc, npc.startX, npc.startY - 1) <= 4) then
				mob = npc:getObjectsInCell(npc.m, npc.x, npc.y + 1, BL_MOB)
				if (#mob > 0) then
					npc:sendAction(1, 14)
					npc:playSound(2)
					Mob(mob[1].ID):removeHealth(Mob(mob[1].ID).health)
				end
				moved = npc:move()
				item = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
				if (#item > 0) then
					npc:sendAction(4, 80)
					item[1]:delete()
				end
			end
		elseif (npc.side == 3) then
			if (distanceXY(npc, npc.startX + 1, npc.startY) <= 4) then
				mob = npc:getObjectsInCell(npc.m, npc.x - 1, npc.y, BL_MOB)
				if (#mob > 0) then
					npc:sendAction(1, 14)
					npc:playSound(2)
					Mob(mob[1].ID):removeHealth(Mob(mob[1].ID).health)
				end
				moved = npc:move()
				item = npc:getObjectsInCell(npc.m, npc.x, npc.y, BL_ITEM)
				if (#item > 0) then
					npc:sendAction(4, 80)
					item[1]:delete()
				end
			end
		end
	end
end
}