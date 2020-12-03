Shoo = {
 	cast=function(player,target)

                 
    
   
                if(target.blType==BL_PC) then
	
	
	
           			target:sendMinitext(player.name .." sends you away.")
				player:sendMinitext("Poof. ^^")

                      		target:sendAnimation(143)

                      		player:sendAction(6,35)
                      		player:playSound(73)
                                target:warp(40,26,6)
                                Delirium.cast(player, target)
			        
                 end             
        end,
	requirements=function(player)
		local l=75
		local i={0,58,6}
		local ia={4000,5,1}
		local d={"Electrocute\n\nAn elemental attack to electrocute your enemy with great power.","You shall have to sacrifice (5) Tiger's eye, (1) Squirrel fur and 4000 coins to learn this ability"}
		return l,i,ia,d
	end       
}