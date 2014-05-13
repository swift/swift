-- A page query using Result Set Management
-- Usage: ./sluift MAMRSMPage.lua <jid> <password> <query_dest> <pages>

sluift.debug = true

c = sluift.new_client(arg[1], arg[2])

c:set_options{compress = false, tls = false}

c:connect();

query = {
	result_set={max_items=5}
}

done = false
page = 0
while not done and page < tonumber(arg[4]) do
    page = page + 1
    c:set_mam{mam=query, to=arg[3]}
    c:for_each_message(function(e)
        if e.payloads[1].tag == 'fin' then
            if e.payloads[2].last_id then
                query.result_set.after = e.payloads[2].last_id
            else
                done = true
            end
            return true
        end
        if e.payloads[1]._type == 'mam_result' then
            print(e.payloads[1].payload.stanza.payloads[1].text)
        end
    end)
end

c:disconnect()
