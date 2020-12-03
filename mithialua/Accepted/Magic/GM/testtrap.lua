testtrap = {
action = function(npc, player)
	if (player) then
		npc:talk(0, "I am performing action, and my owner is: "..player.name)
	else
		npc:talk(0, "I am performing action.")
	end
end,

endAction = function(npc, player)
	if (player) then
		npc:talk(0, "I have ended, and my owner is: "..player.name)
	else
		npc:talk(0, "I have ended.")
	end
	
	npc:delete()
end,
}