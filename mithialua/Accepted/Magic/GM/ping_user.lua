ping_user = {
	cast=function(player,target)
         

	
		
	
		if(target.blType==BL_MOB) then
			player:sendMinitext("LOL, zero ping.")
			return
		end

		if(target:hasDuration("ping_user")) then
			if (player.sex == 0) then
				player:sendMinitext("Patience, sir, there is still a ping in progress...")
		    else
		        player:sendMinitext("Patience, ma'am, there is still a ping in progress...")
		    end
			return
		end
		

		     target:sendAnimation(13)
                     player:playSound(65)
         
		     if(player.ID~=target.ID) then
   			target:sendMinitext("Oink oink, Slob!")
		     end


		if(target.blType==BL_PC) then
                     target:setDuration("ping_user",10000)
		     player:sendMinitext("Ping? Pong! ^^")
                     player:sendAction(6,35)     
                     target:talk(2, "Ping is " .. target.ping .. "ms")  
                   

		end      
    end,
	while_cast=function(player)

              
           player:talk(2, "Ping is " .. player.ping .. "ms")  
                        

	end,
	uncast=function(player)
	
                 

	end,
	requirements=function(player)
		local l=31
		local i={0,5,66,6}
		local ia={800,100,5,1}
		local d={"Minor shielding\n\nA minor magical shield to reduce damage taken.","You shall sacrifice (100) Chestnut, (5) Amethyst, (1) Squirrel fur and 800 coins to learn this ability"}
		return l,i,ia,d
	end       
}