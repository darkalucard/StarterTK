way42 = {
	cast=function(player,target)
                local center={}
                local up={}
                local right={}
                local down={}
                local left={} 		
                local m=player.m
                local centerx=target.x
                local centery=target.y
                local upx=target.x
                local upy=target.y-1
                local rightx=target.x+1
                local righty=target.y
                local downx=target.x
                local downy=target.y+1
                local leftx=target.x-1
                local lefty=target.y
        if(target.blType==BL_MOB) then
        center=player:getObjectsInCell(m,centerx,centery,BL_MOB)
        up=player:getObjectsInCell(m,upx,upy,BL_MOB)
        right=player:getObjectsInCell(m,rightx,righty,BL_MOB)
        down=player:getObjectsInCell(m,downx,downy,BL_MOB)
        left=player:getObjectsInCell(m,leftx,lefty,BL_MOB)

        if(#center>0) then
                if(center[1].blType==BL_MOB) then
                      center[1]:sendAnimation(7)
                end
        end
        if(#up>0) then
		if(up[1].blType==BL_MOB) then
                      up[1]:sendAnimation(7)
                end
        end
        if(#right>0) then
                if(right[1].blType==BL_MOB) then
                      right[1]:sendAnimation(7)
                end
        end
        if(#down>0) then
                if(down[1].blType==BL_MOB) then
                      down[1]:sendAnimation(7)
                end
        end
        if(#left>0) then
                if(left[1].blType==BL_MOB) then
                      left[1]:sendAnimation(7)
                end
        end
        else player:sendMinitext("Something went wrong.")
        end
        end,
	recast=function(player,target)
	end,
	uncast=function(player,target)
	end,
	fix=function(player,target)
	end
          
}