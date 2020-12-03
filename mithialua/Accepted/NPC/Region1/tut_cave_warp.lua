tut_cave_warp1 = {
click = function(player,npc)
	if (npc.registry["tut_cave_warp1_count"] > 10) then
		npc.registry["tut_cave_warp1_count"] = npc.registry["tut_cave_warp1_count"] - 1
		player:sendAnimationXY(400, player.x, player.y)
		player:warp(26, math.random(1,5), math.random(18,21))
		player:sendMinitext("As you take another step you fall into another cavern.")
		player:sendAnimationXY(138, player.x, player.y)
	elseif (npc.registry["tut_cave_warp1_count"] > 7) then
		npc.registry["tut_cave_warp1_count"] = npc.registry["tut_cave_warp1_count"] - 1
		player:sendMinitext("You feel a sudden shiver.")
	else
		tut_cave_warp1.action(npc)
	end
end,

say = function(player, npc)
	local speech = player.speech
	local lspeech = string.lower(speech)

	if (lspeech == "where are you?") then
		player:sendMinitext("I am at X: "..npc.x.." Y: "..npc.y)
	end
end,




action = function(npc)
	local pass = 1
	local randX, randY
	repeat
		randX = math.random(0,npc.xmax)
		randY = math.random(0,npc.ymax)
		pass = getPass(npc.m, randX, randY)
	until pass == 0
	broadcast(npc.m,"XY: "..npc.x.." "..npc.y.." to "..randX.." "..randY)
	npc.registry["tut_cave_warp1_count"] = 16
	npc:warp(npc.m, randX, randY)
end
}