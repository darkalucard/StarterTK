sage = {
cast=function(player)
	local question = player.question
	local aether = 15 * 60 * 1000
	local magicCost = 100

	if (not player:canCast(1, 1, 0)) then
		return
	end
	if (player.magic < magicCost) then
		player:sendMinitext("Not enough mana.")
		return
	end	
	
	player.magic = player.magic - magicCost
	player:sendStatus()
	player:sendAction(6, 80)

	if(string.len(question) > 0) then
		player:broadcast(-1, "["..player.name.."]: "..question)
	else
		player:sendMinitext("Can't send an empty broadcast")
	end
	player:setAether("sage", aether)
end,

requirements = function(player)
	local level = 25
	local items = {0}
	local itemAmounts = {25000}
	local description = {"Broadcast a message throughout Areci Mesuv and surrounding areas.",
	"This spell has a 25,000 gold fee to be taught."}
	return level, items, itemAmounts, description
end
}