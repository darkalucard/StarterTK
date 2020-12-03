approach = {
cast=function(player,target)
	local nP=Player(player.question)
	if(nP~=nil) then
		player:warp(nP.m,nP.x,nP.y)
	else
		player:sendMinitext(nP.." is not online.")
	end
end
}