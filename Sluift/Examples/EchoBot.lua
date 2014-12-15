--[[
	Copyright (c) 2010-2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[
	
	An XMPP Echoing Bot

	This script logs into an XMPP server, sends initial presence,
	and then waits for incoming messages, and echoes them back.
	
	The following environment variables are used:
	* SLUIFT_JID, SWIFT_PASS: JID and password to log in with
	* SLUIFT_DEBUG: Sets whether debugging should be turned on

--]]

require "sluift"

sluift.debug = os.getenv("SLUIFT_DEBUG") or false

c = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
c:connect(function () 
	c:set_version{name = "EchoBot", version = "0.1"}
	c:send_presence("Send me a message")
	for message in c:messages() do
		c:send_message{to = message["from"], body = message["body"]}
	end
end)
