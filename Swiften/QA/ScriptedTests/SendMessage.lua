--
-- Copyright (c) 2010 Remko Tronçon
-- Licensed under the GNU General Public License v3.
-- See Documentation/Licenses/GPLv3.txt for more information.
--

require "sluift"

-- sluift.debug = true
client1_jid = os.getenv("SWIFT_CLIENTTEST_JID") .. "/Client1"
client2_jid = os.getenv("SWIFT_CLIENTTEST_JID") .. "/Client2"
password = os.getenv("SWIFT_CLIENTTEST_PASS")

print "Connecting client 1"
client1 = sluift.new_client(client1_jid, password)
client1:connect()
client1:send_presence("I'm here")

print "Connecting client 2"
client2 = sluift.new_client(client2_jid, password)
client2:connect()
client2:send_presence("I'm here")

print "Checking version of client 2 from client 1"
client2:set_version({name = "Sluift Test", version = "1.0"})
client2_version = client1:get_version(client2_jid)
assert(client2_version["name"] == "Sluift Test")
assert(client2_version["version"] == "1.0")

print "Sending message from client 1 to client 2"
client1:send_message(client2_jid, "Hello")
received_message = client2:for_event(function(event) 
		if event["type"] == "message" and event["from"] == client1_jid then
			return event["body"]
		end
	end, 10000)
assert(received_message == "Hello")

print "Retrieving the roster"
roster = client1:get_roster()
tprint(roster)

client1:disconnect()
client2:disconnect()
