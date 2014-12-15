--[[
	
	Copyright (c) 2010-2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.

	This script logs into an XMPP server, and collects statistics about
	the server software of all contacts in your roster

--]]

require 'sluift'

c = sluift.new_client(os.getenv('SLUIFT_JID'), os.getenv('SLUIFT_PASS'))
c:connect(function () 
	versions = {}
	for jid in pairs(c:get_contacts()) do
		local v = c:get_software_version {to = sluift.jid.domain(jid), timeout = 3000} or {name = 'Unknown'}
		versions[v['name']] = (versions[v['name']] or 0) + 1
	end
	for name, count in pairs(versions) do print(name .. ': ' .. count) end
end)
