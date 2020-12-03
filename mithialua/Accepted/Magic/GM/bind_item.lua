bind_item = {
cast = async(function(player,target)
	if(player:getInventoryItem(0)~=nil) then
		local item=player:getInventoryItem(0)
		local name=player:input("Who?")
		--item.owner_id=player.ID
		item.realName=name
		player:updateInv()
	else
		player:sendMinitext("No item in slot A.")
	end
end)
}  