mass_res={
	cast=function(player,target)
		check = player:getObjectsInArea(BL_PC)
		if(#check > 0) then
			for i=1,#check do
				if(check[i].state==1) then
					check[i].state=0
					check[i]:addHealth((2 ^ 32) / 2 - 1)
					check[i]:addMagic((2 ^ 32) / 2 - 1)
					check[i]:updateState()
				end
			end
		end
	end
}