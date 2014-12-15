--
-- Copyright (c) 2011 Isode Limited.
-- All rights reserved.
-- See the COPYING file for more information.
--

-- This script creates the wonderland world example.
--

require "sluift"
sluift.debug = true

characters = {
	["Alice"] = {
		jid = "alice@wonderland.lit", groups = {}, presence = "<presence/>"
	},
	["Mad Hatter"] = {
		jid = "hatter@wonderland.lit", groups = {}, 
		presence = "<presence><show>away</show><status>At the Tea Party</status></presence>"
	},
	["Queen of Hearts"] = {
		jid ="queen@wonderland.lit", groups = {}, 
		presence = "<presence><show>dnd</show><status>Executing</status></presence>"
	},
	["White Rabbit"] = {
		jid = "rabbit@wonderland.lit", groups = {"Animals"}, 
		presence = "<presence><status>Oh dear!</status></presence>"},
	["Mock Turtle"] = {
		jid = "turtle@wonderland.lit", groups = {"Animals"}, 
		presence = "<presence/>"
	},
}

for name, character in pairs(characters) do
	print("Connecting " .. name .. "...")
	local client = sluift.new_client(character.jid, os.getenv("SLUIFT_PASS"))
	client:set_options({compress = false, tls = false})
	client:connect()
	client:get_contacts()
	client:send(character.presence)
	for contact_name, contact in pairs(characters) do
		if contact.jid ~= character.jid then
			client:add_contact(contact)
		end
	end
	character.client = client
end

print("Confirming subscriptions")
for _, character in pairs(characters) do
	for _, contact in pairs(characters) do
		character.client:confirm_subscription(contact.jid)
	end
end

print("Setting up PubSub nodes")
local hatters_riddles = characters["Mad Hatter"].client:pubsub("pubsub.wonderland.lit"):node("hatters_riddles")
hatters_riddles:delete()
assert(hatters_riddles:create())

local queen_quotes = characters["Queen of Hearts"].client:pubsub("pubsub.wonderland.lit"):node("queen_quotes")
queen_quotes:delete()
assert(queen_quotes:create())
queen_quotes:publish{id = 'quote1', item = {_type = 'body', text = 'Off with his head!'}}
queen_quotes:publish{id = 'quote2', item = {_type = 'body', text = 'Off with her head!'}}
queen_quotes:publish{id = 'quote3', item = {_type = 'body', text = 'Off with their heads!'}}

characters['Mad Hatter'].client:pubsub():node('http://jabber.org/protocol/geoloc'):publish{
	item = {_type = 'user_location', latitude = 50.376739, longitude = -4.200709}}
characters['Queen of Hearts'].client:pubsub():node('http://jabber.org/protocol/geoloc'):publish{
	item = {_type = 'user_location', latitude = 50.551123, longitude = -4.141654}}
characters['Mock Turtle'].client:pubsub():node('http://jabber.org/protocol/geoloc'):publish{
	item = {_type = 'user_location', latitude = 50.366630, longitude = -4.134518}}
characters['White Rabbit'].client:pubsub():node('http://jabber.org/protocol/geoloc'):publish{
	item = {_type = 'user_location', latitude = 50.332907, longitude = -4.759194}}



print("Disconnecting alice")
characters['Alice'].client:disconnect()

print("Done. Waiting ...")
while true do
	for name, character in pairs(characters) do
		if name == 'Queen of Hearts' then
			for message in character.client:messages{timeout = 1000} do
				if message.body == 'publish' then
					queen_quotes:publish{item = {_type = 'body', text = 'Off with her head!'}}
					queen_quotes:publish{item = {_type = 'body', text = 'Off with his head!'}}
				else
					character.client:send_message{to = e["from"], body = "Off with their heads!"}
				end
			end
		elseif name == "Mad Hatter" then
			for message in character.client:messages{timeout = 1000} do
				if message.body == 'publish' then
					hatters_riddles:publish{item = {_type = 'body', text = 'Why is a raven like a writing desk?'}}
				end
			end
		else
			for message in character.client:messages{timeout = 100} do end
		end
	end
end
