warrior_test = {
	cast=function(player,target)
	if(player.magic<90) then
		player:sendMinitext("Your will is too weak.")
		return
	end
	if(player.enchant>1) then
		player:sendMinitext("An enchantment is already in effect.")
		return
	end
	if(player.state==1) then
		player:sendMinitext("Spirits can't do that.")
		return
	end
	if(player.state==3) then
		player:sendMinitext("You can not cast this spell on a mount.")
		return
	end
		player:sendAction(6,35)
		player:removeMagic(90)
		if(player.level < 99) then
			player.enchant=7
		elseif(player.level == 99) then
			player.enchant=9*(3+(player.maxMagic/4000)+(player.maxHealth/8000))/4
		end
		player.backstab=1
		player.flank=1
		player:sendMinitext("You enter a rage... swinging wildly around you.")
		
	end,
	recast=function(player)
		player.backstab=1
		player.flank=1
	end,
	uncast=function(player)
		player.backstab=0
		player.flank=0
	end,
	fix=function(player)
		player.backstab=1
		player.flank=1
	end,
	requirements=function(player)
		local l=55
		local i={0,61}
		local ia={2200,5}
		local d={"Enchant weapon\n\nInfuse greater magic into your weapon to make it more performant.","You shall sacrifice (5) Chrysocolla and 2200 coins to learn this ability."}
		return l,i,ia,d
	end
}