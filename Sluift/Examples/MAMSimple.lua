-- Querying the archive for messages 
-- Usage: ./sluift MAMSimple.lua <jid> <password> <query_dest>

sluift.debug = true

c = sluift.new_client(arg[1], arg[2])

c:connect();

c:set_mam{mam={}, to=arg[3]}

c:for_each_message(function(e)
	if e.payloads[1].tag == 'fin' then return true end
	if e.payloads[1]._type == 'mam_result' then
		print(e.payloads[1].payload.stanza.payloads[1].text)
	end
end)

c:disconnect()
