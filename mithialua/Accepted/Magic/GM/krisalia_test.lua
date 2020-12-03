krisalia_test = {
	cast= function(player,target)
	Mob(1173741823).registry["opening"] = 0
--[[ Some checks
		local p = Player('Krisalia')
		local i = 5
		local m = p.m
		local h, j, k, l
		while i > 0 do
			local x = p.x + 1
			local y = p.y
			 h = p:getObjectsInCell(m,x,y,BL_PC)
			 j = p:getObjectsInCell(m,x,y,BL_MOB)
			 k = p:getObjectsInCell(m,x,y,BL_NPC)
			 l = p:getObjectsInCell(m,p.x,p.y,BL_PC)

			if((type(h[1]) == type(l[1])) or (type(j[1]) == type(l[1])) or (type(k[1]) == type(l[1]))) then
				p:talk(0,"Error "..x.." Player= "..type(h[1]).." MOB= "..type(j[1]).." NPC= "..type(k[1])..".")		
			else
			p:warp(m,x,y)
			p:talk(0,"i="..i)
			end
			i = i - 1
		end

	end)
	]]--
--[[Charge no loop
		local p = player
		local m = player.m
		local h, j, k, l, x1, y1
		local x = player.x
		local y = player.y
		local i = 0
		local check = 0
		--local x1 = 0
		--local y1 = 0
		if(i == 0) then
			if(p.side==0) then y=p.y-1 end
			if(p.side==1) then x=p.x+1 end
			if(p.side==2) then y=p.y+1 end
			if(p.side==3) then x=p.x-1 end
				h = p:getObjectsInCell(m,x,y,BL_PC)
				j = p:getObjectsInCell(m,x,y,BL_MOB)
				k = p:getObjectsInCell(m,x,y,BL_NPC)
				l = p:getObjectsInCell(m,p.x,p.y,BL_PC)
			if((type(h[1]) == type(l[1])) or (type(j[1]) == type(l[1])) or (type(k[1]) == type(l[1]))) then
				p:sendMinitext("Can't charge from upclose.")
				return
			else
				i = 1
			end
		end
		if(i == 1) then
			if(p.side==0) then y=p.y-2 end
			if(p.side==1) then x=p.x+2 end
			if(p.side==2) then y=p.y+2 end
			if(p.side==3) then x=p.x-2 end
			if(p.side==0) then
				y1=y+1
				x1=x
			end
			if(p.side==1) then
				x1=x-1
				y1=y
			end
			if(p.side==2) then
				y1=y-1
				x1=x
			end
			if(p.side==3) then
				x1=x+1
				y1=y
			end
				h = p:getObjectsInCell(m,x,y,BL_PC)
				j = p:getObjectsInCell(m,x,y,BL_MOB)
				k = p:getObjectsInCell(m,x,y,BL_NPC)
				l = p:getObjectsInCell(m,p.x,p.y,BL_PC)
			if((type(h[1]) == type(l[1])) or (type(j[1]) == type(l[1])) or (type(k[1]) == type(l[1]))) then
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					check = 1
					i = 10
				else
					i = 10
					p:sendMinitext("Something went wrong.")
				end
			else
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					i = 2
				else
					i=10
					p:sendMinitext("Something went wrong.")
					return
				end
			end
		end
		if(i == 2) then
			if(p.side==0) then y=p.y-3 end
			if(p.side==1) then x=p.x+3 end
			if(p.side==2) then y=p.y+3 end
			if(p.side==3) then x=p.x-3 end
			if(p.side==0) then
				y1=y+1
				x1=x
			end
			if(p.side==1) then
				x1=x-1
				y1=y
			end
			if(p.side==2) then
				y1=y-1
				x1=x
			end
			if(p.side==3) then
				x1=x+1
				y1=y
			end
				h = p:getObjectsInCell(m,x,y,BL_PC)
				j = p:getObjectsInCell(m,x,y,BL_MOB)
				k = p:getObjectsInCell(m,x,y,BL_NPC)
				l = p:getObjectsInCell(m,p.x,p.y,BL_PC)
			if((type(h[1]) == type(l[1])) or (type(j[1]) == type(l[1])) or (type(k[1]) == type(l[1]))) then
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					check = 1
					i = 10
				else
					i = 10
					p:sendMinitext("Something went wrong.")
					return
				end
			else
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					i = 3
				else
					i=10
					p:sendMinitext("Something went wrong.")
					return
				end
			end
		end
		if(i == 3) then
			if(p.side==0) then y=p.y-4 end
			if(p.side==1) then x=p.x+4 end
			if(p.side==2) then y=p.y+4 end
			if(p.side==3) then x=p.x-4 end
			if(p.side==0) then
				y1=y+1
				x1=x
			end
			if(p.side==1) then
				x1=x-1
				y1=y
			end
			if(p.side==2) then
				y1=y-1
				x1=x
			end
			if(p.side==3) then
				x1=x+1
				y1=y
			end				h = p:getObjectsInCell(m,x,y,BL_PC)
				j = p:getObjectsInCell(m,x,y,BL_MOB)
				k = p:getObjectsInCell(m,x,y,BL_NPC)
				l = p:getObjectsInCell(m,p.x,p.y,BL_PC)
			if((type(h[1]) == type(l[1])) or (type(j[1]) == type(l[1])) or (type(k[1]) == type(l[1]))) then
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					check = 1
					i = 10
				else
					i = 10
					p:sendMinitext("Something went wrong.")
				end
			else
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					i = 4
				else
					i=10
					p:sendMinitext("Something went wrong.")
					return
				end
			end
		end
		if(i == 4) then
			if(p.side==0) then y=p.y-5 end
			if(p.side==1) then x=p.x+5 end
			if(p.side==2) then y=p.y+5 end
			if(p.side==3) then x=p.x-5 end
			if(p.side==0) then
				y1=y+1
				x1=x
			end
			if(p.side==1) then
				x1=x-1
				y1=y
			end
			if(p.side==2) then
				y1=y-1
				x1=x
			end
			if(p.side==3) then
				x1=x+1
				y1=y
			end
				h = p:getObjectsInCell(m,x,y,BL_PC)
				j = p:getObjectsInCell(m,x,y,BL_MOB)
				k = p:getObjectsInCell(m,x,y,BL_NPC)
				l = p:getObjectsInCell(m,p.x,p.y,BL_PC)
			if((type(h[1]) == type(l[1])) or (type(j[1]) == type(l[1])) or (type(k[1]) == type(l[1]))) then
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					check = 1
					i = 10
				else
					i = 10
					p:sendMinitext("Something went wrong.")
				end
			else
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					i = 5
				else
					i=10
					p:sendMinitext("Something went wrong.")
					return
				end
			end
		end

		if(i == 5) then
			if(p.side==0) then y=p.y-6 end
			if(p.side==1) then x=p.x+6 end
			if(p.side==2) then y=p.y+6 end
			if(p.side==3) then x=p.x-6 end
			if(p.side==0) then
				y1=y+1
				x1=x
			end
			if(p.side==1) then
				x1=x-1
				y1=y
			end
			if(p.side==2) then
				y1=y-1
				x1=x
			end
			if(p.side==3) then
				x1=x+1
				y1=y
			end
				h = p:getObjectsInCell(m,x,y,BL_PC)
				j = p:getObjectsInCell(m,x,y,BL_MOB)
				k = p:getObjectsInCell(m,x,y,BL_NPC)
				l = p:getObjectsInCell(m,p.x,p.y,BL_PC)
			if((type(h[1]) == type(l[1])) or (type(j[1]) == type(l[1])) or (type(k[1]) == type(l[1]))) then
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					check = 1
					i = 10
				else
					i = 10
					p:sendMinitext("Something went wrong.")
					return
				end
			else
				if((getPass(m,x1,y1)==0) and (p:objectCanMove(x1,y1,p.side)==true)) then
					--p:sendMinitext("No target in range.")
					p:sendMinitext("Something went wrong.")
					return
				else
					p:sendMinitext("Something went wrong.")
					return
				end
			end
		end
		if(check == 1) then
			if(p.side==0) then y=y1 end
			if(p.side==1) then x=x1 end
			if(p.side==2) then y=y1 end
			if(p.side==3) then x=x1 end
			player:warp(m,x,y)
			player:talk(2,"Chaaaarge!")
		end
	]]--
	--[[
	if(target.blType==BL_PC) then
		if(player.pvp==true) then
			target:sendMinitext(player.name .." casts shinies on you.")
			target:sendAnimation(262)
			target:playSound(55)
			player:sendAction(6,35)
			if(target:hasDuration("krisalia_test")) then
				player:sendMinitext("A curse is already active on the target [shinies].")
				target.ac=target.registry['baseac']+(10*target.registry['hexstacks'])
				target:setDuration("krisalia_test",(target.durationAmount(1038) + 15000))
				target.registry['hexstacks'] = (target.registry['hexstacks'] + 1)
			else
				target.registry['hexstacks'] = 1
				target.registry['baseac'] = target.ac
				target.ac=target.ac+10
			   	target:setDuration("krisalia_test",15000)
			end
			player:addMana(-150)
            player:sendMinitext("You cast shinies.")
			target:sendStatus()
		else
			player:sendMinitext("Something went wrong.")
		end
	elseif(target.blType==BL_MOB) then
		if(target:hasDuration("krisalia_test")) then player:sendMinitext("Already shinied.") return end
	
		target.ac=target.ac+10
		target:sendAnimation(262)
		target:playSound(55)
		player:sendAction(6,35)
		target:setDuration("krisalia_test",15000)
		target:repeatAnimation("krisalia_test",34)
        player:addMana(-150)
        player:sendMinitext("You cast shinies.")
	end
                          
	end,
	uncast=function(player)
		player.ac=player.ac-(10*player.registry['hexstacks'])
		player.registry['hexstacks'] = 0
		player:sendStatus()
	end,

    recast = function(player)
        player.ac=player.ac+(10*player.registry['hexstacks'])
		player:sendStatus()
    end]]--
	--if (Player('eito'): objectCanMoveFrom(Player('eito').x,Player('eito').y,Player('eito').side) == true) then Player('eito'):talk(0,'true') else Player('eito'):talk(0,'false') end
end
}