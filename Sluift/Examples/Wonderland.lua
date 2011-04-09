--
-- Copyright (c) 2011 Remko Tronçon
-- Licensed under the GNU General Public License v3.
-- See Documentation/Licenses/GPLv3.txt for more information.
--

-- This script creates the wonderland world example.
--

require "sluift"
--sluift.debug = true

characters = {
	{jid = "alice@wonderland.lit", name = "Alice", groups = {}, presence = "<presence/>"},
	{jid = "hatter@wonderland.lit", name = "Mad Hatter", groups = {}, presence = "<presence><show>away</show><status>At the Tea Party</status></presence>"},
	{jid ="queen@wonderland.lit", name = "Queen of Hearts", groups = {}, presence = "<presence><show>dnd</show><status>Executing</status></presence>"},
	{jid = "rabbit@wonderland.lit", name = "White Rabbit", groups = {"Animals"}, presence = "<presence><status>Oh dear!</status></presence>"},
	{jid = "turtle@wonderland.lit", name = "Mock Turtle", groups = {"Animals"}, presence = "<presence/>"},
}

clients = {}
for _, character in ipairs(characters) do
	print("Connecting " .. character["name"] .. "...")
	client = sluift.new_client(character["jid"], os.getenv("SLUIFT_PASS"))
	client:set_options({compress = false, tls = false})
	client:connect()
	client:get_contacts()
	client:send(character["presence"])
	table.insert(clients, client)
	for _, contact in ipairs(characters) do
		if contact["jid"] ~= character["jid"] then
			client:add_contact(contact)
		end
	end
end

print("Confirming subscriptions")
for _, client in ipairs(clients) do
	for _, contact in ipairs(characters) do
		client:confirm_subscription(contact["jid"])
	end
end

print("Done. Waiting ...")
while true do
	for _, client in ipairs(clients) do
		client:for_event(function(e) 
				if e["type"] == "message" then client:send_message(e["from"], "Off with their heads!") end 
			end, 1000)
	end
	sluift.sleep(1000)
end
