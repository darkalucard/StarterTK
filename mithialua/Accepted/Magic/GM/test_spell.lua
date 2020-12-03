test_spell = {
	cast=function(player,target)		
		if(player.state==1) then
			player:sendMinitext("Spirits can't do that.")
            return
        end
                
        if(player.magic<150) then
            player:sendMinitext("You do not have enough mana.")
            return
        end
		if(player.state==3) then
			player:sendMinitext("You can not cast this spell on a mount.")
			return
		end
 
		local check = {}		
			local anim = 155
				
			for x=-20,20 do
				for y=-20,20 do
					check = mob:getObjectsInCell(mob.m,mob.x+x,mob.y+y,BL_PC)
					local r = math.random(0,3)
					if(r==1)then
						mob:sendAnimationXY(anim,mob.x+x,mob.y+y)
						if(#check>0) then
							if(Player(check[1].ID).state==0)then
								Player(check[1].ID):removeHealth(Player(check[1].ID).maxHealth*.05)
							end
						end						
					end
				end
			end
								
		
			
		
			
			
                          
	end,
	uncast=function(player)
		player:sendStatus()
	end,
	--[[while_cast = function(player)
		local mobid=110
		local anim=26
		player.registry["testspellcounter"]=player.registry["testspellcounter"]+1
		if(player.registry["testspellcounter"]==1) then
			player:sendAction(18,500)
			player:spawn(mobid,player.x+5,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x+5,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x-5,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x-5,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y+5,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y+5,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y-5,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y-5,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
		elseif(player.registry["testspellcounter"]==2) then
			player:sendAction(18,500)
			player:spawn(mobid,player.x+4,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x+4,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x-4,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x-4,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y+4,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y+4,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y-4,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y-4,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
		elseif(player.registry["testspellcounter"]==3) then
			player:sendAction(18,500)
			player:spawn(mobid,player.x+3,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x+3,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x-3,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x-3,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y+3,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y+3,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y-3,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y-3,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
		elseif(player.registry["testspellcounter"]==4) then
			player:sendAction(18,500)
			player:spawn(mobid,player.x+2,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x+2,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x-2,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x-2,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y+2,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y+2,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y-2,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y-2,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
		elseif(player.registry["testspellcounter"]==5) then
			player:sendAction(18,500)
			player:spawn(mobid,player.x+1,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x+1,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x-1,player.y,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x-1,player.y,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y+1,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y+1,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
			player:spawn(mobid,player.x,player.y-1,1)
			local check = {}
			check = player:getObjectsInCell(player.m,player.x,player.y-1,BL_MOB)
			if(#check>0)then		
				check[1].attacker=player.ID
				check[1]:removeHealth(2000000000)
				check[1]:sendAnimation(anim)
			end
		end
		player:sendStatus()
    end,
	]]--
	--[[while_cast = function(player)
		local anim = math.random(1,420)
		player.registry["testspellcounter"]=player.registry["testspellcounter"]+1
		if(player.registry["testspellcounter"]<=5) then
			player:sendMinitext("test")
			player:sendAnimationXY(anim,player.x+player.registry["testspellcounter"],player.y)
			player:sendAnimationXY(anim,player.x-player.registry["testspellcounter"],player.y)
			player:sendAnimationXY(anim,player.x,player.y+player.registry["testspellcounter"])
			player:sendAnimationXY(anim,player.x,player.y-player.registry["testspellcounter"])
		elseif(player.registry["testspellcounter"]==6)then
			player:sendAnimationXY(anim,player.x+player.registry["testspellcounter"],player.y+player.registry["testspellcounter"])
			player:sendAnimationXY(anim,player.x-player.registry["testspellcounter"],player.y-player.registry["testspellcounter"])
			player:sendAnimationXY(anim,player.x-player.registry["testspellcounter"],player.y+player.registry["testspellcounter"])
			player:sendAnimationXY(anim,player.x+player.registry["testspellcounter"],player.y-player.registry["testspellcounter"])
		elseif(player.registry["testspellcounter"]==7)then
			for x=-5,5 do
				for y=-5,5 do
					player:sendAnimationXY(anim,player.x+x,player.y+y)
				end
			end
		end		
	end,
	]]--
	while_cast = function(player)

	end,
    recast = function(player)		
		player:sendStatus()
        end,
	fix = function(player)        
		player:sendStatus()
        end,
        --while_cast=function(player)
        --player:sendAnimation(34)
        --end,

	requirements=function(player)
		local l=71
		local i={0,58,6}
		local ia={4000,5,1}
		local d={"Hex\n\nA powerful Curse that effect your target's defenses.","You shall have to sacrifice (5) Tiger's eye, (1) Squirrel fur and 4000 coins to learn this ability"}
		return l,i,ia,d
	end

}



