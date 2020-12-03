light = {
cast=function(player,target)
	local ani = 135
	local will = player.will
	local x = player.x
	local y = player.y
	if (will == 0) then
		will = 1
	end
	local duration = 10000 * math.ceil(will / 10)
	local mana_cost = (5 + player.will) * -1
	if (player:hasDuration("light")) then
		if (player.side == 0) then
			player:sendAnimationXY(ani,x,y-2)
		elseif (player.side == 1) then
			player:sendAnimationXY(ani,x+2,y)
		elseif (player.side == 2) then
			player:sendAnimationXY(ani,x,y+2)
		elseif (player.side == 3) then
			player:sendAnimationXY(ani,x-2,y)
		end
		player:sendMinitext("You already cast that.")
		return
	end
	if (player.magic<mana_cost) then
		player:sendMinitext("Your will is too weak.")
		return
	end
	if (player.state==1) then
		player:sendMinitext("Spirits can't do that.")
		return
	end
	if (player.state==3) then
		player:sendMinitext("You can not cast this spell on a mount.")
		return
	end
	player:sendAction(6,35)
	player:addMana(mana_cost)
	player:setDuration("light",duration)
	player:sendAnimation (ani,2)
	if (player.side == 0) then
		player:sendAnimationXY(ani,x,y-2)
	elseif (player.side == 1) then
		player:sendAnimationXY(ani,x+2,y)
	elseif (player.side == 2) then
		player:sendAnimationXY(ani,x,y+2)
	elseif (player.side == 3) then
		player:sendAnimationXY(ani,x-2,y)
	end
	player:talk(2,"~lux~")
	player:sendMinitext("You cast Light.")
end,
recast=function(player)
	local ani = 135
	player:sendAnimation (ani,2)
end,
uncast=function(player)
	local ani = 135
	player:sendAnimation(ani,-1)
	player:sendAnimation(ani,4)
end,
while_cast=function(player)
	local ani = 135
	duration = 2*math.ceil(player.will/10)
	player:sendAnimation (ani,-1)
	player:sendAnimation (ani,duration)
		for _,gID in pairs(player.group) do
			local gMember=Player(gID)
			if(gMember.state~=1) then
				if(gMember.m==player.m) then
					if(gMember.ID~=player.ID) then
						gMember:sendAnimation(ani,-1)
						gMember:sendAnimation(ani,duration)
						if (os.time() %5 == 0) then
							gMember:sendMinitext(player.name .." provides you with light.")
						end
					end
				end
			end
		end		
end,

on_say = function(player)
	if (string.lower(player.speech) == "lux") then
		light.cast(player)
		player.speech = ""
	end
end,

on_forget = function(player)
	player.registry["learned_light"] = 0
end,

requirements=function(player)
	local level = 5
	local items = {}
	local itemAmounts = {}
	local description = {"Provides light in dark places"}
	return level, items, itemAmounts, description
end
}