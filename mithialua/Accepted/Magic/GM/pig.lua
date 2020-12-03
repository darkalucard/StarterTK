pig = {
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

		if(target:hasDuration("pig")) then
			player:sendMinitext("Pig is already in effect.")
			return
		end
		

		     target:sendAnimation(13)
                     player:playSound(65)
         
		     if(player.ID~=target.ID) then
   			target:sendMinitext("Oink oink, Slob!")
		     end
		if(target.blType==BL_PC) then
                     target:setDuration("pig",100000)
		     player:sendMinitext("You cast Pig.")
                     player:sendAction(6,35)       
                     target.state = 4
                     target.disguise = math.random(1,1031)
		     target.registry["morphedanimal"]=target.disguise
                     target.disguiseColor = math.random(0,20)
                     target:sendStatus()  
                     target:updateState()
		end      
        end,
	while_cast=function(player)

	               player.state=4
                       player.disguise=player.registry["morphedanimal"]

		        
         

                        --if(math.random(0,10) == 4) then
                        --  player:talk(2,"*Ribbit*")
                        --end

                        --if(math.random(0,10) == 5) then
                        --  player:talk(2,"Snort, Snort!")
                        --end
                        

	end,
	uncast=function(player)
	
                        player:sendMinitext("You have returned to normal.")
			player.registry["morphedanimal"]=0
		        player.state=0
                        player.disguise=0
		        player:sendAnimation(13)	
                        player:sendStatus()  
                        player:updateState()  
	end,
	requirements=function(player)
		local l=31
		local i={0,5,66,6}
		local ia={800,100,5,1}
		local d={"Minor shielding\n\nA minor magical shield to reduce damage taken.","You shall sacrifice (100) Chestnut, (5) Amethyst, (1) Squirrel fur and 800 coins to learn this ability"}
		return l,i,ia,d
	end       
}
