--
-- Copyright (c) 2010 Remko Tronçon
-- Licensed under the GNU General Public License v3.
-- See Documentation/Licenses/GPLv3.txt for more information.
--

--
-- An XMPP Echoing Bot
--
-- This script logs into an XMPP server, sends initial presence,
-- and then waits for incoming messages, and echoes them back.
-- 
-- The following environment variables are used:
-- * SLUIFT_JID, SWIFT_PASS: JID and password to log in with
-- * SLUIFT_DEBUG: Sets whether debugging should be turned on
-- 

require "sluift"

sluift.debug = os.getenv("SLUIFT_DEBUG") or false

c = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
c:connect()
c:send_presence("Send me a message")
c:for_event(function(e) 
		if e["type"] == "message" then c:send_message(e["from"], e["body"]) end 
	end)
