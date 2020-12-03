room_hellfire = {
	cast=function(player,target)
		local center={}
		local center2={}
		local m=player.m
		local xp=player.x
		local yp=player.y
		local countdamage=0
			
	if(player.magic<6) then
		player:sendMinitext("You do not have enough mana.")
		return
	end
	
	if(player.state==1) then
		player:sendMinitext("Spirits can't do that.")
		return
	end
	if(player.state==3) then
		player:sendMinitext("You can not cast this spell on a mount.")
		return
	end

	local countdamage=player.magic*1.5
	
	center=player:getObjectsInArea(BL_PC)
	center2=player:getObjectsInArea(BL_MOB)
	if(#center>0) then
		for z=1,#center do
			if(center[z].name~=player.name) then
				if(player.pvp > 0) then
					center[z].attacker=player.ID
					center[z]:sendAnimation(8)
					center[z]:removeHealth(countdamage)		
				end
			end					
		end				
	end
	if(#center2>0) then
		for z=1,#center2 do
			center2[z].attacker=player.ID
			center2[z]:sendAnimation(8)
			center2[z]:removeHealth(countdamage)					
		end				
	end			
	
	
	player:sendAction(6,35)
	player:playSound(8)
	player:removeMagic(6)
	player:sendMinitext("You cast room hellfire.")
	player:setAether("room_hellfire",2000)
    end,
	requirements=function(player)
		local l=94
		local i={0,466,467}
		local ia={7500,5,1}
		local d={"Explode\n\nAn elemental attack of fire, at the risk of hurting and possibly killing yourself. Damage and Aether depends on your will and mana","You shall have to sacrifice (5) Bottled rum, (1) Golden helm and 7500 coins to learn this ability"}
		return l,i,ia,d
	end       
}