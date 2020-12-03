where = {
	cast=function(player,target)
		target=Player(player.question)
		if(target~=nil) then
			player:sendMinitext(target.name.." is located at:")
			player:sendMinitext("Map: "..target.m.." \a X: "..target.x.." \a Y: "..target.y)
		end
	end
}	