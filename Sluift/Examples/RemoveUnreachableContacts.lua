--
-- Copyright (c) 2010 Remko Tronçon
-- Licensed under the GNU General Public License v3.
-- See Documentation/Licenses/GPLv3.txt for more information.
--

-- This script logs into an XMPP server, iterates over all roster items,
-- and checks if their server is still alive. If not, the script asks you
-- whether it should remove the contact from your contact list.
-- 
-- The following environment variables are used:
-- * SLUIFT_JID, SWIFT_PASS: JID and password to log in with
-- * SLUIFT_DEBUG: Sets whether debugging should be turned on

require "sluift"
sluift.debug = os.getenv("SLUIFT_DEBUG")

print "Connecting ..."
c = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
c:connect()

print "Checking for unreachable contacts ..."
for jid, _ in pairs(c:get_contacts()) do
	_, err = c:get_version(sluift.jid_domain(jid), 10000)
	if err == "Remote server not found" or err == "Timeout" then
		print("Delete " .. jid .. " (" .. err .. ") ? [y/n/q]")
		answer = io.read()
		if answer == "y" then
			c:remove_contact(jid)
		elseif answer == "q" then
			break
		end
	end
end

print "Done. Exiting ..."
c:disconnect()
