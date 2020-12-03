clear_spells = {
cast=async(function(player,target)
	local nameofPlayer=player:input("Who shall lose their spells?")
	local target = Player(nameofPlayer)
	if (target == nil) then
		return
	end
	for i = 1, 6999 do
		target:removeSpell(i)
	end        
end)       
}