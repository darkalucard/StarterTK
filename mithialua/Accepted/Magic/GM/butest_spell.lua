butest_spell = {
		cast=async(function(player,target)
		local opts = { }
		table.insert(opts,"Yes")
		table.insert(opts,"No")
		
		local menuOption=player:menuString2("Hi priest",opts)
		local mid1=player:input("groom")
		local menuOption2=Player(mid1):menuString2("Hi",opts)
		local mid2=player:input("bride")
		local menuOption3=Player(mid2):menuString2("Hi",opts)
		if menuOption2=="Yes" and menuOption3=="Yes" then
			player:dialogSeq({"both said yes"})
		elseif menuOption2=="Yes" and menuOption3=="No" then
			player:dialogSeq({""..mid1.." said yes, "..mid2.." said no"})
		elseif menuOption2=="No" and menuOption3=="Yes" then
			player:dialogSeq({""..mid1.." said no, "..mid2.." said yes"})
		elseif menuOption2=="No" and menuOption3=="No" then
			player:dialogSeq({"Both said no"})
		end
	


                end)    
}



