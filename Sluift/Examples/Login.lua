--[[
	Copyright (c) 2010-2014 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[

	This script logs into an XMPP server, and sends initial presence
	Useful as initialization script for an interactive session ('-i'),
	or as a starting point for scripts.

	The following environment variables are used:
	* SLUIFT_JID, SWIFT_PASS: JID and password to log in with
	* SLUIFT_DEBUG: Sets whether debugging should be turned on

--]]

require "sluift"
sluift.debug = os.getenv("SLUIFT_DEBUG") or false

print("Connecting " .. os.getenv("SLUIFT_JID") .. " ...")
c = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
c:set_options{compress = false, tls = false}
c:connect()
c:send_presence("")

print("Connected ...")
print("Use the 'c' variable to communicate.")
