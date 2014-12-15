--[[
	Copyright (c) 2010-2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[
	
	Alternative version of EchoBot that uses with()

	This script logs into an XMPP server, sends initial presence,
	and then waits for incoming messages, and echoes them back.
	
	The following environment variables are used:
	* SLUIFT_JID, SWIFT_PASS: JID and password to log in with
	* SLUIFT_DEBUG: Sets whether debugging should be turned on

--]]

require "sluift"

sluift.debug = os.getenv("SLUIFT_DEBUG") or false

client = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
sluift.with(client, function ()
	connect()
	set_version{name = "EchoBot", version = "0.1"}
	send_presence("Send me a message")
	for message in messages() do
		send_message{to = message["from"], body = message["body"]}
	end
end)
