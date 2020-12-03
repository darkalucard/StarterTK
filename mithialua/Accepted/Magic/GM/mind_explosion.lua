mind_explosion = {
	cast=function(player,target)

	if(player.state==1) then
		player:sendMinitext("Spirits can't do that.")
		return
	end

		local d= { }
                local mobs= { }
		local m=player.m
		local curx=player.x
		local cury=player.y
                local damage=player.magic

 		for x=-10,10 do
			for y=-10,10 do
				d=player:getObjectsInCell(m,curx+x,cury+y,BL_PC)
				mobs=player:getObjectsInCell(m,curx+x,cury+y,BL_MOB)
				if(player.pvp==true) then
				if(#d>0) then
					for z=1,#d do
		                                if(d[z].ID ~= player.ID) then
							d[z].attacker=player.ID
					       		d[z]:removeHealth(damage)  
                                                end

					end
				end
				end
				if(#mobs>0) then
					for z=1,#mobs do
							mobs[z].attacker=player.ID
					       		mobs[z]:removeHealth(damage)  

					end
				end
		

				
			end
		end
		player:sendAnimation(85)
		player:removeMagic(player.magic)

		--[[
		if(player:objectCanMove(player.x,player.y-1,0)==true) then
			player:sendMinitext("Can move NORTH")
		else
			player:sendMinitext("can NOT move NORTH")
		end
		if(player:objectCanMove(player.x+1,player.y,1)==true) then
			player:sendMinitext("Can move EAST")
		else
			player:sendMinitext("can NOT move EAST")
		end
		if(player:objectCanMove(player.x,player.y+1,2)==true) then
			player:sendMinitext("Can move SOUTH")
		else
			player:sendMinitext("can NOT move SOUTH")
		end
		if(player:objectCanMove(player.x-1,player.y,4)==true) then
			player:sendMinitext("Can move WEST")
		else
			player:sendMinitext("can NOT move WEST")
		end
		if(player:objectCanMove(player.x,player.y,0)==true) then
			player:sendMinitext("Symmetry: Can move SOUTH")
		else
			player:sendMinitext("Symmetry: can NOT move SOUTH")
		end
		]]--
		--[[
			local find={}
			local ptable = {}
			for x=-10,10 do
				for y=-10,10 do
					find=player:getObjectsInCell(player.m,player.x+x,player.y+y,BL_PC)
					if(#find>0) then
						--player:talk(0,""..find[z].ID.."")
						for z=1,#find do
							table.insert(ptable,""..find[z].ID.."")
						end
					end
				end
			end
			if(#ptable)>0 then
				local ra=math.random(1,#ptable)
				Player(ptable[ra]):talk(0,"Debug")
			end
		]]--
		--[[
		local find = {}
		find=player:getObjectsInArea(BL_PC)
			if(#find>0) then
				for z=1,#find do
					find[z]:setDuration("bomb_spell",10000)
					player:sendMinitext(""..find[z].name.." : "..find[z].ac..".")
				end
			end

		   if(player.pvp==false) then
		   	player:sendMinitext("pvp is off")
		   else
			player:sendMinitext("pvp is on")
		   end
		]]--
		--for z=1,player.mapRegistry["auctionhousecurrentnumber"] do
		--	if(player.mapRegistry["auctionhouse"..z.."id"]==0 and player.mapRegistry["auctionhouse"..z.."price"]==0 and player.mapRegistry["auctionhouse"..z.."count"]==0) then
		--		player.mapRegistry["auctionhouse"..z.."sellerid"]=0
		--	end
		--end
		--
		--local a=2
		--player:talk(0,"price: "..player.mapRegistry["auctionhouse"..a.."price"]..".")
		--player:talk(0,"id: "..player.mapRegistry["auctionhouse"..a.."id"]..".")
		--player:talk(0,"count: "..player.mapRegistry["auctionhouse"..a.."count"]..".")
		--player:talk(0,"sellerid: "..player.mapRegistry["auctionhouse"..a.."sellerid"]..".")		
		--player:sendAction(18,125)
                --player:playSound(101)
                --player:sendAnimation(85)
		--[[

		for x=1,250 do
		--	player.invis=9
		--	player.state=2
		--	player:updateState()
		--	player:swing()
		--end
		--player.invis=1
		--if(player.state~=1) then
		--	player.state=0
		--end
		--player:updateState()

		]]--
		--local t = os.date('*t')
		--player:sendMinitext(""..t.hour.."")

		--player:talk(0,""..player.mapRegistry["battlefieldannounce"].."")
--[[
		local tar={}
		for x=-5,5 do
			for y=-5,5 do
				tar=player:getObjectsInCell(player.m,player.x+x,player.y+y,BL_MOB)
				if(#tar>0) then
					for z=1,#tar do
						throw(0,player.m,player.x,player.y,tar[z].x,tar[z].y,2341,0,0)
					end
				end
			end
		end
]]--
		



	end,
	recast=function(player)
	end,
	uncast=function(player)
	end,
	fix=function(player)
	end
}