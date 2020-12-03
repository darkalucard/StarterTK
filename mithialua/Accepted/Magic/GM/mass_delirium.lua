mass_delirium = {
	

	cast=function(player,target)
		check = player:getObjectsInArea(BL_PC)
		if(#check > 0) then
			for i=1,#check do
				if(check[i].state==1) then
					check[i].state=0
					check[i]:addHealth(10000000000000000)
					check[i]:addMagic(10000000000000000)
					check[i]:updateState()
				end
			end
		end
		end,
	recast=function(player)
	end,
	uncast=function(player)
	end,
	fix=function(player)
	end
}