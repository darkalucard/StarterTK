Blind = {
	cast=function(player,target)
                if(player.state==1) then
			player:sendMinitext("Spirits can't do that.")
			return
		end


		if(target:hasDuration("Blind")) then
			player:sendMinitext("Blind is already in effect.")
			return
		end
		

		     target:sendAnimation(11)
                     player:playSound(65)
         
		     if(player.ID~=target.ID) then
   			target:sendMinitext("You have lost your sight.")
		     end

		if(target.blType==BL_PC) then
                     target:setDuration("Blind",100000)
		     player:sendMinitext("You cast Blind.")
                     player:sendAction(6,35)   
 
		end     

               target.blind = 1 
           
        end,
	while_cast=function(player)

                          target.blind = 1 

		        
                                

	end,
	uncast=function(player)
	
                        player:sendMinitext("You can see again.")
		        player.blind = 0
	end,
	requirements=function(player)
		local l=31
		local i={0,5,66,6}
		local ia={800,100,5,1}
		local d={"Minor shielding\n\nA minor magical shield to reduce damage taken.","You shall sacrifice (100) Chestnut, (5) Amethyst, (1) Squirrel fur and 800 coins to learn this ability"}
		return l,i,ia,d
	end       
}