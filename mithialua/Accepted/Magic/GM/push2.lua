push2 = {
	cast=function(player,target)
		if(player.state==1) then
			player:sendMinitext("Spirits can't do that")
			return
		end
		if(player.state==3) then
			player:sendMinitext("You can not cast this spell on a mount.")
			return
		end
		
		local m=player.m
		local x=player.x
		local y=player.y

		if(player.side==0) then y=y-1 end
		if(player.side==1) then x=x+1 end
		if(player.side==2) then y=y+1 end
		if(player.side==3) then x=x-1 end
		
		local d=player:getObjectsInCell(m,x,y,BL_PC)

		if(#d==0) then
			player:sendMinitext("There is no target in front of you.")
			return
		end

		-- Facing North
		if(player.side==0) then
			local checkplayernorth=player:getObjectsInCell(player.m,player.x,player.y-2,BL_PC)
			local checkmobnorth=player:getObjectsInCell(player.m,player.x,player.y-2,BL_MOB)
			local checknpcnorth=player:getObjectsInCell(player.m,player.x,player.y-2,BL_NPC)
			local checkplayereast=player:getObjectsInCell(player.m,player.x+1,player.y-1,BL_PC)
			local checkmobeast=player:getObjectsInCell(player.m,player.x+1,player.y-1,BL_MOB)
			local checknpceast=player:getObjectsInCell(player.m,player.x+1,player.y-1,BL_NPC)
			local checkplayerwest=player:getObjectsInCell(player.m,player.x-1,player.y-1,BL_PC)
			local checkmobwest=player:getObjectsInCell(player.m,player.x-1,player.y-1,BL_MOB)
			local checknpcwest=player:getObjectsInCell(player.m,player.x-1,player.y-1,BL_NPC)

			if(#checkplayernorth+#checkmobnorth+#checknpcnorth==0 and player:objectCanMove(player.x,player.y-2,0)==true) then-- and getPass(player.m,player.x,player.y-2)==0) then
				player:sendMinitext("You cast Push.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x,player.y-2)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			elseif(#checkplayereast+#checkmobeast+#checknpceast==0 and player:objectCanMove(player.x+1,player.y-1,1)==true) then-- and getPass(player.m,player.x+1,player.y-1)==0) then
				player:sendMinitext("You cast Push.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x+1,player.y-1)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			elseif(#checkplayerwest+#checkmobwest+#checknpcwest==0 and player:objectCanMove(player.x-1,player.y-1,3)==true) then-- and getPass(player.m,player.x-1,player.y-1)==0) then
				player:sendMinitext("You cast Push.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x-1,player.y-1)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			else
				--player:sendMinitext("There is nowhere for you to land!")
				d[1]:sendAnimation(10)
				return
			end

		--Facing East
		elseif(player.side==1) then

			local checkplayereast=player:getObjectsInCell(player.m,player.x+2,player.y,BL_PC)
			local checkmobeast=player:getObjectsInCell(player.m,player.x+2,player.y,BL_MOB)
			local checknpceast=player:getObjectsInCell(player.m,player.x+2,player.y,BL_NPC)

			local checkplayernorth=player:getObjectsInCell(player.m,player.x+1,player.y-1,BL_PC)
			local checkmobnorth=player:getObjectsInCell(player.m,player.x+1,player.y-1,BL_MOB)
			local checknpcnorth=player:getObjectsInCell(player.m,player.x+1,player.y-1,BL_NPC)

			local checkplayersouth=player:getObjectsInCell(player.m,player.x+1,player.y+1,BL_PC)
			local checkmobsouth=player:getObjectsInCell(player.m,player.x+1,player.y+1,BL_MOB)
			local checknpcsouth=player:getObjectsInCell(player.m,player.x+1,player.y+1,BL_NPC)

			if(#checkplayereast+#checkmobeast+#checknpceast==0 and player:objectCanMove(player.x+2,player.y,1)==true) then-- and getPass(player.m,player.x+2,player.y)==0) then
				player:sendMinitext("You cast Push.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x+2,player.y)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			elseif(#checkplayernorth+#checkmobnorth+#checknpcnorth==0 and player:objectCanMove(player.x+1,player.y-1,0)==true) then-- and getPass(player.m,player.x+1,player.y-1)==0) then
				player:sendMinitext("You cast Push.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x+1,player.y-1)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			elseif(#checkplayersouth+#checkmobsouth+#checknpcsouth==0 and player:objectCanMove(player.x+1,player.y+1,2)==true and player:objectCanMove(player.x+1,player.y,0)==true) then-- and getPass(player.m,player.x+1,player.y+1)==0) then
				player:sendMinitext("You cast Push.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x+1,player.y+1)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			else
				--player:sendMinitext("There is nowhere for you to land!")
				d[1]:sendAnimation(10)
				return
			end
		--Facing South
		elseif(player.side==2) then

			local checkplayersouth=player:getObjectsInCell(player.m,player.x,player.y+2,BL_PC)
			local checkmobsouth=player:getObjectsInCell(player.m,player.x,player.y+2,BL_MOB)
			local checknpcsouth=player:getObjectsInCell(player.m,player.x,player.y+2,BL_NPC)

			local checkplayereast=player:getObjectsInCell(player.m,player.x+1,player.y+1,BL_PC)
			local checkmobeast=player:getObjectsInCell(player.m,player.x+1,player.y+1,BL_MOB)
			local checknpceast=player:getObjectsInCell(player.m,player.x+1,player.y+1,BL_NPC)

			local checkplayerwest=player:getObjectsInCell(player.m,player.x-1,player.y+1,BL_PC)
			local checkmobwest=player:getObjectsInCell(player.m,player.x-1,player.y+1,BL_MOB)
			local checknpcwest=player:getObjectsInCell(player.m,player.x-1,player.y+1,BL_NPC)


			if(#checkplayersouth+#checkmobsouth+#checknpcsouth==0 and player:objectCanMove(player.x,player.y+2,2)==true and player:objectCanMove(player.x,player.y+1,0)==true) then-- and getPass(player.m,player.x,player.y+2)==0) then
				player:sendMinitext("You cast Sneak attack.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x,player.y+2)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			elseif(#checkplayereast+#checkmobeast+#checknpceast==0 and player:objectCanMove(player.x+1,player.y+1,1)==true) then-- and getPass(player.m,player.x+1,player.y+1)==0) then
				player:sendMinitext("You cast Sneak attack.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x+1,player.y+1)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			elseif(#checkplayerwest+#checkmobwest+#checknpcwest==0 and player:objectCanMove(player.x-1,player.y+1,3)==true) then-- and getPass(player.m,player.x-1,player.y+1)==0) then
				player:sendMinitext("You cast Sneak attack.")
				player:playSound(337)
				d[1]:warp(player.m,player.x-1,player.y+1)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			else
				--player:sendMinitext("There is nowhere for you to land!")
				d[1]:sendAnimation(10)
				return
			end
		--Facing West
		elseif(player.side==3) then

			local checkplayerwest=player:getObjectsInCell(player.m,player.x-2,player.y,BL_PC)
			local checkmobwest=player:getObjectsInCell(player.m,player.x-2,player.y,BL_MOB)
			local checknpcwest=player:getObjectsInCell(player.m,player.x-2,player.y,BL_NPC)

			local checkplayernorth=player:getObjectsInCell(player.m,player.x-1,player.y-1,BL_PC)
			local checkmobnorth=player:getObjectsInCell(player.m,player.x-1,player.y-1,BL_MOB)
			local checknpcnorth=player:getObjectsInCell(player.m,player.x-1,player.y-1,BL_NPC)

			local checkplayersouth=player:getObjectsInCell(player.m,player.x-1,player.y+1,BL_PC)
			local checkmobsouth=player:getObjectsInCell(player.m,player.x-1,player.y+1,BL_MOB)
			local checknpcsouth=player:getObjectsInCell(player.m,player.x-1,player.y+1,BL_NPC)




			if(#checkplayerwest+#checkmobwest+#checknpcwest==0 and player:objectCanMove(player.x-2,player.y,3)==true) then-- and getPass(player.m,player.x-2,player.y)==0) then
				player:sendMinitext("You cast Sneak attack.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x-2,player.y)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			elseif(#checkplayernorth+#checkmobnorth+#checknpcnorth==0 and player:objectCanMove(player.x-1,player.y-1,0)==true) then-- and getPass(player.m,player.x-1,player.y-1)==0) then
				player:sendMinitext("You cast Sneak attack.")
				player:playSound(337)
				d[1]:warp(player.m,player.x-1,player.y-1)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			elseif(#checkplayersouth+#checkmobsouth+#checknpcsouth==0 and player:objectCanMove(player.x-1,player.y+1,2)==true and player:objectCanMove(player.x-1,player.y,0)==true) then-- and getPass(player.m,player.x-1,player.y+1)==0) then
				player:sendMinitext("You cast Sneak attack.")
				player:playSound(337)	
				d[1]:warp(player.m,player.x-1,player.y+1)
				player:sendAction(1,14)
				d[1]:sendAnimation(10)
			else
				--player:sendMinitext("There is nowhere for you to land!")
				d[1]:sendAnimation(10)
				return
			end
		end		
	end,
	requirements=function(player)
		local l=48
		local i={0,5,66,6}
		local ia={2000,100,10,1}
		local d={"Push\n\nPush your enemy.","You shall sacrifice (100) Chestnut, (10) Amethyst, (1) Squirrel fur and 2000 coins to learn this ability."}
		return l,i,ia,d
	end

}