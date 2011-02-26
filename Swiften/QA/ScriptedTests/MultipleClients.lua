--
-- Copyright (c) 2010 Remko Tronçon
-- Licensed under the GNU General Public License v3.
-- See Documentation/Licenses/GPLv3.txt for more information.
--

require "sluift"

-- sluift.debug = true
num_clients = 10

print("Connecting clients")
clients = {}
for i = 1, num_clients do
	jid = os.getenv("SWIFT_CLIENTTEST_JID") .. "/Client" .. i
	client = sluift.new_client(jid, os.getenv("SWIFT_CLIENTTEST_PASS"))
	client:set_options({compress = false})
	client:async_connect()
	table.insert(clients, client)
end

print("Waiting for clients to be connected")
for i, client in ipairs(clients) do
	client:wait_connected()
	client:send_presence("Hello")
end

print("Disconnecting clients")
for i, client in ipairs(clients) do
	client:disconnect()
end

print("Done")
