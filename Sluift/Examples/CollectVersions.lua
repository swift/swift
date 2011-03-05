--
-- Copyright (c) 2010 Remko Tronçon
-- Licensed under the GNU General Public License v3.
-- See Documentation/Licenses/GPLv3.txt for more information.
--

-- This script logs into an XMPP server, and collects statistics about
-- the server software of all contacts in your roster

require "sluift"

c = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
c:connect()

versions = {}
for jid, _ in pairs(c:get_contacts()) do
	v = c:get_version(sluift.jid_domain(jid))
	if v then versions[v["name"]] = (versions[v["name"]] or 0) + 1 end
end
for name, count in pairs(versions) do print(name .. ": " .. count) end

c:disconnect()
