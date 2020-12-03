dev_inquiry = {
cast=async(function(player,target)
	local input1
	local input2
	
	input1 = player:input("Type (mob,item,npc)?")
	
	if(input1~=nil) then
		if (input1 == "npc") then
			input2 = player:input("NPC id?")
			player:sendMinitext(input2.." "..NPC(input2).name)
		elseif (input1 == "mob") then
			input2 = player:input("mobID?")
			player:sendMinitext(input2.." "..Mob(input2).name)
		elseif (input1 == "item") then
			input2 = player:input("Item id?")
			player:sendMinitext(input2.." "..Item(input2).name)
		end
	end
end)
}	