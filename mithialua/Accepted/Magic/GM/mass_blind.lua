mass_blind = {
	cast=function(player,target)

	if(player.state==1) then
		player:sendMinitext("Spirits can't do that.")
		return
	end

		local d= { }
		local m=player.m
		local curx=player.x
		local cury=player.y

		

		for x=-10,10 do
			for y=-10,10 do
				d=player:getObjectsInCell(m,curx+x,cury+y,BL_PC)
				if(#d>0) then
					for z=1,#d do
		                                if(d[z].ID ~= player.ID) then
					       		Blind.cast(player, d[z])
                                                end

					end
				end
				
			end
		end




		player:sendAction(18,125)

	end,
	recast=function(player)
	end,
	uncast=function(player)
	end,
	fix=function(player)
	end
}