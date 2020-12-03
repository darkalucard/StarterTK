captcha = {
cast = function(player, target)
	player:gmMsg(player.name.." has used captcha on "..target.name..".", 1)
	addChatLog(player, player.name.." has used captcha on "..target.name..".")
	bot.check(target)
end
}