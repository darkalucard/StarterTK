follow = {
cast = function(player,target)
	target:setDuration("follow", 60000, player.ID)
	player:sendMinitext("You force "..target.name.." to follow you.")
	target:sendMinitext(""..player.name.." has forced you to follow them.")
end,

while_cast = function(player,caster)
	player:warp(caster.m, caster.x, caster.y)
end,

uncast = function(player,caster)
	--player:talk(0,""..caster.name)
	--caster:sendMinitext(""..player.name.." stops following you.")
end
}