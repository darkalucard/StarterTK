tut_quest_bird_worshipper = {
click = async(function(player,npc)
	local t={graphic=convertGraphic(604,"monster"),color=4}
	local s={graphic=convertGraphic(1010,"monster"),color=0}
	local c={graphic=convertGraphic(183,"monster"),color=3}
	local m={graphic=convertGraphic(183,"monster"),color=5}
	local w={graphic=convertGraphic(442,"monster"),color=30}
	local n={graphic=convertGraphic(1,"item"),color=0}
	local wk={graphic=convertGraphic(428,"item"),color=31}
	
	player.npcGraphic = t.graphic
	player.npcColor = t.color
	player.dialogType = 0
	
	if (player.level >= 2) then
		if (player.quest["tut_quest_bird_worshipper"] == 0) then
			player:dialogSeq({t,"I'm worried, we haven't seen our weather bird lately.",
			s,"He keeps the island safe from the weather wizard.",
			t,"The only way we can reach his nest is through the tunnel, but it has collapsed because of the many worms.",
			c,"These worms are really a big problem because I can't visit the weather bird to see how he is doing or to feed him.",
			w,"Some people say that this red worm was spotted when the tunnels caved in.",
			t,"Without the weather bird, we will be in deep trouble..."},1)
			player.quest["tut_quest_bird_worshipper"] = 1
			player:flushKills(9)
			player:flushKills(10)
		elseif (player.quest["tut_quest_bird_worshipper"] == 1) then
			player:dialogSeq({s,"Please find our weather bird, he should be through the underground tunnels.",
			t,"Let me know if you find him."},1)
		elseif (player.quest["tut_quest_bird_worshipper"] == 2 and player.quest["tut_quest_noling"] >= 3) then
			if (player:hasLegend("tut_quest_bird")) then
				player:dialogSeq({t,"Thank you for feeding the weather bird, I have faith it will keep us from harm.",
				t,"Here, have this as a token of our gratitude."},1)
				player:addGold(150)
				player:giveXP(400)
				player.quest["tut_quest_bird_worshipper"] = 3
				player.quest["tut_quest_noling"] = 5
			end
		elseif (player.quest["tut_quest_bird_worshipper"] == 3) then
			if (player:killCount(7) < 6) then
				player:dialogSeq({t,"Hello again,",
				t,"We are very grateful to you for helping us feed the weather bird, and I'm wondering if you could give us a hand with something else.",
				m,"Could you slay six Crawlers to help us make our way through the tunnel?",
				t,"Let us know every six you kill so we can send a team with supplies to the weather bird."
				},1)
			elseif (player:killCount(7) >= 6) then
				--if (player.quest["tut_quest_bird_w_timer"] < os.time()) then
					--reward quest
					player.quest["tut_quest_bird_w2"] = player.quest["tut_quest_bird_w2"] + 1
					player:flushKills(7)
					player:giveXP(250)
					
					if (player.quest["tut_quest_bird_w2"] == 2) then
						player:dialogSeq({t,"You have continously helped us in clearing the way to the weather bird, we pray to the weather bird for your safety."},1)
						
						if (player:hasSpace(171,1)) then
							player:addItem(171,1)
						else
							player:dialog("You did not have enough space to hold it.", {})
							player:dropItem(171,1,player.ID)
						end
					elseif (player.quest["tut_quest_bird_w2"] == 3) then
						player:dialogSeq({t,"You have continously helped us in clearing the way to the weather bird, we pray to the weather bird for your safety.",
						t,"It seems our continuous prayers have granted you a blessing from the weather bird.",
						wk,"You may have these."},1)
						
						if (player:hasSpace(171,5)) then
							player:addItem(171,5)
						else
							player:dialog("You did not have enough space to hold them.", {})
							player:dropItem(171,5,player.ID)
						end
					elseif (player.quest["tut_quest_bird_w2"] == 5) then
						player:dialogSeq({t,"You have continously helped us in clearing the way to the weather bird, we pray to the weather bird for your safety.",
						s,"It seems our continuous prayers have granted you a blessing from the weather bird.",
						wk,"You may have these."},1)
						
						if (player:hasSpace(171,20)) then
							player:addItem(171,20)
						else
							player:dialogSeq({{}, "You did not have enough space to hold them."}, 1)
							player:dropItem(171,20,player.ID)
						end
						player:addLegend("Received blessing from the weather bird followers. "..curT(),"tut_quest_bird_w",22,16)
					else
						if (math.random(20) <= 15) then
							player:dialogSeq({t,"Thank you for continuously helping us clear the underground tunnel.",
							t,"we pray to the weather bird for your safety."},1)
							
							if (player:hasSpace(171,1)) then
								player:addItem(171,1)
							else
								player:dialog("You did not have enough space to hold it.", {})
								player:dropItem(171,1,player.ID)
							end
						else
							player:dialog("Thank you for continuously helping us clear the underground tunnel. We pray to the weather bird in your name.", {})
						end
					end
				--else
				--	player:dialog("We do not have enough supplies ready yet, come back later.", {})
				--end
			else
				player:dialog("How you got here, no one knows; tell a GM you hit the void.", {})
			end
		else--Not in quest or quest completed.
			player:dialog("The weather bird is the only thing that keeps us safe from the tempests.", {})
		end
	else
		player:dialog("If only you were stronger I would dare ask you... (2nd Insight)", {})
	end
	
end)
}