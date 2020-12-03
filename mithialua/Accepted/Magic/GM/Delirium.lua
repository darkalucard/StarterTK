Delirium = {
	cast=function(player,target)
                if(player.state==1) then
			player:sendMinitext("Spirits can't do that.")
			return
		end

		if(target.state==1) then
			player:sendMinitext("This wouldn't be of very much help right now.")
			return
		end

		
	
		if(target.blType==BL_MOB) then
			player:sendMinitext("Something went wrong.")
			return
		end

		if(target:hasDuration("Delirium")) then
			player:sendMinitext("Delirium is already in effect.")
			return
		end
		

		     target:sendAnimation(200)
                     player:playSound(65)
         
		     if(player.ID~=target.ID) then
   			target:sendMinitext("You feel dizzy.")
		     end

		if(target.blType==BL_PC) then
                     target:setDuration("Delirium",100000)
		     player:sendMinitext("You cast Delirium.")
                     player:sendAction(6,35)   
                     target.drunk = 255    
 

		end      
        end,
	while_cast=function(player)

                        player.drunk = 255

		        
                                

	end,
	uncast=function(player)
	
                        player:sendMinitext("Your balance has returned.")
		        player.drunk = 0
	end,
	requirements=function(player)
		local l=31
		local i={0,5,66,6}
		local ia={800,100,5,1}
		local d={"Minor shielding\n\nA minor magical shield to reduce damage taken.","You shall sacrifice (100) Chestnut, (5) Amethyst, (1) Squirrel fur and 800 coins to learn this ability"}
		return l,i,ia,d
	end       
}