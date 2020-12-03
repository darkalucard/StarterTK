info = {
cast=function(player,target)
	if(target.blType==BL_PC) then
		--[[local pN = player.name
		local tL = target.level
		local tCN = target.className
		local tN = target.name
		local tCH = target.health
		local tMH = target.maxHealth
		local tBH = target.baseHealth
		local tCM= target.magic
		local tMM = target.maxMagic
		local tBM = target.baseMagic
		local tM = target.might
		local tbM = target.baseMight
		local tW = target.will
		local tbW = target.baseWill
		local tG = target.grace
		local tbG = target.baseGrace
		local tAC = target.ac
		local tBAC = target.baseAC
		local tMR = target.resist
		local tBMR = target.baseResist
		local tDAM = target.dam
		local tH = target.hit
		local tC = target.money
		local tbC = target.bankMoney
		local tE = target.exp
		local tD = target.dodge
		local tB = target.block
		local tP = target.parry
		local tSk = target.skinColor
		local tDis = target.disguise
		local tMAP = target.m
		local tX = target.x
		local tY = target.y]]--
		local tImm = target.registry["immortal"]
		--local tGM = target.gmLevel
		local popup = ""
		local vitastr = tostring(target.vRegenAmount)
		local manastr = tostring(target.mRegenAmount)
		local vitafind = string.find(vitastr, "%p")
		local manafind = string.find(manastr, "%p")
		if (vitafind ~= nil) then
			vitafind = vitafind + 1
		end
		if (manafind ~= nil) then
			manafind = manafind + 1
		end
		vitastr = string.sub(vitastr, 0, vitafind)
		manastr = string.sub(manastr, 0, manafind)
		local pkstr = ""
		local pkdurastr = ""
		if (target.PK == 0) then
			pkstr = "Normal"
		elseif (target.PK == 1) then
			pkstr = "PK"
			if (target.durationPK / 1000 > 120) then
				pkdurastr = "\nPK Duration: "..tostring((target.durationPK / 1000) / 60)
			else
				pkdurastr = "\nPK Duration: "..tostring(target.durationPK / 1000)
			end
		else
			pkstr = "Bounty"
			if (target.durationPK / 1000 > 120) then
				pkdurastr = "\nPK Duration: "..tostring((target.durationPK / 1000) / 60)
			else
				pkdurastr = "\nPK Duration: "..tostring(target.durationPK / 1000)
			end
		end
			
		if (tImm == 1) then
			tImm = "Yes"
		else
			tImm = "No"
		end
		--target:sendMinitext(player.name.." makes you tingle.")
		local expSold = target.expSold
		local quantifier
		if (expSold < 1000000) then
			quantifier = "K"
			expSold = expSold / 1000
		elseif (expSold < 1000000000) then
			quantifier = "M"
			expSold = expSold / 1000000
		else
			quantifier = "B"
			expSold = expSold / 1000000000
		end
		expSold = string.format("%.2f", expSold)
		
		target:sendMinitext(player.name.." tickles you.")
		target:sendAction(11,255)
		player:sendMinitext("Info on "..target.className.." "..target.name..".")
		popup = popup.."<b>GM Info\n\nImmortal: "..tImm.." GM: "..target.gmLevel.."\nMoney: "..target.money.." Bank: "..target.bankMoney.."\nExperience: "..target.exp
		popup = popup.."\nSkin: "..target.skinColor.." Disguise: "..target.disguise.."\nSpeed: "..target.speed.." Base Speed: "..target.speed.."\nMap: "..target.m.." X: "..target.x.." Y: "..target.y
		popup = popup.."\n\n<b>"..target.name.."'s Base Stats\n\nWeapon Damage: "..target.minDam.." - "..target.maxDam.."\nVita/Mana: "..target.baseHealth.."/"..target.baseMagic
		popup = popup.."\nMight/Grace/Will: "..target.might.."/"..target.grace.."/"..target.will.."\nAC/MR: "..target.ac.."/"..target.resist.."\nPD: "..target.physDeduct
		popup = popup.."\nProtection: "..target.protection.."\nRegen: "..target.regen.."\nVita/Mana Regen: "..target.vRegen.."/"..target.mRegen
		popup = popup.."\n\n<b>Current Stats\n\nLevel: "..target.level.."\nClass: "..target.className.."\nWeapon Damage: "..target.minDam.." - "..target.maxDam.."\nPierce: "..target.pierce.."\nVita/Mana: "..target.health.."/"..target.magic
		popup = popup.."\nMight/Grace/Will: "..target.might.."/"..target.grace.."/"..target.will.."\nHit/Miss/Dam: "..target.hit.."/"..(target.miss / 100).."/"..target.dam.."\nAC/MR: "..target.ac.."/"..target.resist
		popup = popup.."\nPD: "..target.physDeduct.."\nProtection: "..target.protection.."\nDodge/Block/Parry: "..target.dodge.."/"..target.block.."/"..target.parry.."\nDS: "..target.dmgShield.."\nRegen: "..target.regen
		popup = popup.."\nVita/Mana Regen: "..target.vRegen.."/"..target.mRegen.."\nVita/Mana per 5s: "..vitastr.."/"..manastr--.."\nWeap/Armor Dura Mods: "..target.weapDuraMod.."/"..target.armorDuraMod
		popup = popup.."\nPK Status: "..pkstr..pkdurastr--.."\n<b>AP/SP: "..target.ap.."/"..target.sp
	
		popup = popup.."\n\nExp Sold: "..expSold.." "..quantifier		
		player:popUp(popup)
	end             
end       
}