--[[
	Copyright (c) 2010-2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[
	This script logs into an XMPP server, iterates over all roster items,
	and checks if their server is still alive. If not, the script asks you
	whether it should remove the contact from your contact list.

	The following environment variables are used:
	* SLUIFT_JID, SWIFT_PASS: JID and password to log in with
	* SLUIFT_DEBUG: Sets whether debugging should be turned on
--]]

require 'sluift'
sluift.debug = os.getenv('SLUIFT_DEBUG')

print 'Connecting ...'
c = sluift.new_client(os.getenv('SLUIFT_JID'), os.getenv('SLUIFT_PASS'))
c:connect(function (c)
	print 'Checking for unreachable contacts ...'
	for jid in pairs(c:get_contacts()) do
		_, err = c:get_software_version {to = sluift.jid.domain(jid), timeout = 10000}
		print(err)
		if err == 'Remote server not found' or err == 'Remote server timeout' then
			print('Delete ' .. jid .. ' (' .. err .. ') ? [y/n/q]')
			answer = io.read()
			if answer == 'y' then
				c:remove_contact(jid)
			elseif answer == 'q' then
				break
			end
		end
	end
	print 'Done. Exiting ...'
end)
