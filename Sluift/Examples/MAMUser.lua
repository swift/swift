-- Querying for all messages to/from a particular JID 
-- Usage: ./sluift MAMUser.lua <jid> <password> <query_dest> <query_jid>

sluift.debug = true

c = sluift.new_client(arg[1], arg[2])

c:connect();

fields = {
	with = arg[4]
}

query_form = sluift.create_form{fields, form_type="urn:xmpp:mam:0"}

c:set_mam{mam={form=query_form}, to=arg[3]}

c:for_each_message(function(e)
	if e.payloads[1].tag == 'fin' then return true end
	if e.payloads[1]._type == 'mam_result' then
		print(e.payloads[1].payload.stanza.payloads[1].text)
	end
end)

c:disconnect()
