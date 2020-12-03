experience_calc = {
	cast=function(player,target)		
		if(player.state==1) then
			player:sendMinitext("Spirits can't do that.")
            return
        end                
        if(player.magic<150) then
            player:sendMinitext("You do not have enough mana.")
            return
        end
		if(player.state==3) then
			player:sendMinitext("You can not cast this spell on a mount.")
			return
		end
		if(target.state==1) then
			player:sendMinitext("Something went wrong.")
			return
		end
        
		if(player.exp<2147483647) then
			player.registry["expcalcstart"] = player.exp
		else
			player.registry["expcalcstart"] = 2147483647
			player.registry["expcalcstart2"] = player.exp-2147483647
		end
		
		player:playSound(55)
		player:sendAnimation(295)
		player:sendAction(6,35)					
		player:setDuration("experience_calc",900000)		
		player:sendMinitext("You cast experience calc")		
		player:sendMinitext("StartEXP: "..player.registry["expcalcstart"]+player.registry["expcalcstart2"])	
		player:sendStatus()                     
	end,


	uncast=function(player)
		local endexp = player.exp
		local value = endexp-player.registry["expcalcstart"]-player.registry["expcalcstart2"]	
		if(player.registry["expcalcstart"]~=nil)then
			player:sendMinitext("   [EXPERIENCE REPORT] ")
			player:sendMinitext("-------------------------")
			player:sendMinitext("StartEXP: "..player.registry["expcalcstart"]+player.registry["expcalcstart2"])
			player:sendMinitext("EndEXP: "..endexp)
			player:sendMinitext("EXP in 15m: "..value)	
			player:sendMinitext("-------------------------")
		else
			player:sendMinitext("ERR: NO REG VALUE")
		end
		player.registry["expcalcstart"]=0
		player.registry["expcalcstart2"]=0
	end,
	while_cast=function(player)
--[[
For Jeff:

Calling player(player will not work, you need to use a capital P to call a global player object.
Also ["caster"] is not the correct registry
this line should read :

Player(player.registry["casterexp"]):sendMinitext("whatever")

You also need to absolutely check for any possible nil values before executing or script will break, so that becomes :

if(Player(player.registry["casterexp"])~=nil) then
	Player(player.registry["casterexp"]):sendMinitext("whatever")
end

]]--

	--counter = counter + 1
	--if(counter == duration -1)then	
		--endexp = player.exp	
		--value = endexp - startexp
		
		--player:sendStatus()
		--end
	end,	

    recast = function(player)
		player.registry["startingexptest"]=0
		player:sendStatus()
        end,
	fix = function(player)        
		player:sendStatus()
        end,
        --while_cast=function(player)
        --player:sendAnimation(34)
        --end,

	requirements=function(player)
		local l=71
		local i={0,58,6}
		local ia={4000,5,1}
		local d={"Hex\n\nA powerful Curse that effect your target's defenses.","You shall have to sacrifice (5) Tiger's eye, (1) Squirrel fur and 4000 coins to learn this ability"}
		return l,i,ia,d
	end

}