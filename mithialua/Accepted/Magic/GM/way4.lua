way4 = {
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
        		
		local caster=player.ID	
		local duration=900000
				
		if(target.blType==BL_PC) then			   	
			   	target:playSound(55)
				player:sendAction(6,35)				
			   	target:setDuration("way4",duration)
				player:removeMagic(150)               
				local startexp = target.exp			
				target.registry["casterexp"]=caster
				target.registry["startingexptest"]=startexp
				
				--player:sendMinitext(""..startexp)
				--player:sendMinitext(""..target.registry["startingexptest"])
				
				player:sendMinitext("You cast exp calc on "..target.name)				
				target:sendStatus()
			else
				player:sendMinitext("Something went wrong.")
			end                         
	end,


	uncast=function(player)
	if(player.registry["casterexp"]~=nil)then
		local endexp = player.exp
		--Player(player.registry["casterexp"]):sendMinitext("End exp "..endexp)
		if(player.registry["startingexptest"]~=nil)then
			local value = endexp-player.registry["startingexptest"]
			Player(player.registry["casterexp"]):sendMinitext(""..player.name..": "..value)
			player:talk(0,"EXP in 15min: "..value)
			end
		end
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