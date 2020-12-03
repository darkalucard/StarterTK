--utility script
--http://lua-users.org/wiki/FormattingNumbers

function format_number(amount)
  local formatted = amount
  while true do  
    formatted, k = string.gsub(formatted, "^(-?%d+)(%d%d%d)", '%1,%2')
    if (k==0) then
      break
    end
  end
  return formatted
end


function mithia_lore_time()
    --exactly 52 weeks a year
    --12 months a year, 30,30,31,30,30,31,30,30,31,30,30,31
    --4 seasons
    -- every season starts on Lunae,
     
	local days = {"Lunae", "Martis", "Mercurii","Jovis", "Veneris", "Saturni", "Solis"}
	local months = {"Ianuarius","Februarius", "Martius", "Aprilis", "Maius", "Iunus", 
	                "Quintilis", "Augustus", "September", "October","November","December"}
	local seasons = {"Winter","Spring","Summer","Autumn"}
	local year = curYear() --gets current year
	local date = curDay() --gets current day
	local day = days[(date % 7) + 1]
	local time = curTime() --gets current time
	local season = seasons[curSeason()] --gets current Season (string)
    local dayOfMonth = date
    
    local mdate = date
    if (mdate >= 91) then
    	mdate = 90
    end
    
    if (dayOfMonth == 91) then
    	dayOfMonth = 31
    else
    	while (dayOfMonth > 30) do
			dayOfMonth = dayOfMonth - 30
		end
	end
	
    if (dayOfMonth > 30) then
    	dayOfMonth = dayOfMonth - 30
   	end
   	
    if (dayOfMonth > 30) then
    	dayOfMonth = dayOfMonth - 30
   	end
   	
    local month = months[(curSeason()-1)*3 + math.floor((mdate-1) / 30) + 1] 

	return {Year = year, SeasonOfYear = season, DayOfSeason = date, DayOfWeek = day, MonthOfYear = month, DayOfMonth = dayOfMonth, Time = time}
end
