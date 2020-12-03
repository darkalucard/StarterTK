encrypt = {
encrypt = function(msg, key)
	local msgTable = {}
	local msgTable2 = {}
	local i
	broadcast(-1,msg)
	msg = ""..msg
	msg = string.gsub(msg, "\\", "/")
	broadcast(-1,msg)
	
	local length = string.len(msg)
	if (length < 1) then
		return
	end

	local keyLength = string.len(key)
	if (keyLength < 1) then
		return
	end
	
	for i = 1, length do
		table.insert(msgTable, string.byte(string.sub(msg,i,i)))
		table.insert(msgTable2, string.byte(string.sub(msg,i,i)))
	end
	--Table contains byte of string
	local keyTable = {}
	local keyV = 0
	for i = 1, keyLength do
		table.insert(keyTable,""..string.byte(key,i))
	end
	for i = 1, #keyTable do
		keyV = keyV + keyTable[i]
	end
	keyV = math.floor(keyV / #keyTable)
	
	for i = 1, length do
		msgTable[i] = msgTable[i] + keyV
	end
	
	for i = 1, length do
		msgTable2[i] = msgTable[i] - keyV
	end
	
	local msg2 = ""
	local msg3 = ""
	
	for i = 1, length do
		msg2 = msg2..string.char(msgTable[i])
		msg3 = msg3..string.char(msgTable2[i])
	end
	
	broadcast(-1, "Input: "..msg)
	broadcast(-1, "Encrypted: "..msg2)
	broadcast(-1, "Decrypted: "..msg3)
--32 to 126 (space to z)

end,

decrypt = function(str, key)
	local msgTable = {}
	local i
	
	local length = string.len(msg)
	if (length < 1) then
		return
	end

	local keyLength = string.len(key)
	if (keyLength < 1) then
		return
	end
	
	for i = 1, length do
		table.insert(msgTable, string.byte(string.sub(msg,i,i)))
	end
	--Table contains byte of string
	local keyTable = {}
	local keyV = 0
	for i = 1, keyLength do
		table.insert(keyTable,""..string.byte(key,i))
	end
	for i = 1, #keyTable do
		keyV = keyV + keyTable[i]
	end
	keyV = math.floor(keyV / #keyTable)
	
	for i = 1, length do
		msgTable[i] = msgTable[i] - keyV
	end
	
	local msg2 = ""
	
	for i = 1, length do
		msg2 = msg2..string.char(msgTable[i])
	end
	
	broadcast(-1, "Input: "..msg)
	broadcast(-1, "Decrypted: "..msg2)
end
}