--[[
	Copyright (c) 2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[
	
	Ad-hoc command example.

	This script logs into an XMPP server, and executes a
	multi-step ad-hoc command.
	
	The following environment variables are used:
	* SLUIFT_JID, SWIFT_PASS: JID and password to log in with
	* SLUIFT_DEBUG: Sets whether debugging should be turned on

--]]

require "sluift"

sluift.debug = os.getenv("SLUIFT_DEBUG") or false

c = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
c:connect(function () 
	to = sluift.jid.domain(os.getenv("SLUIFT_JID"))

	-- Get the list of commands
	commands = assert(c:get_disco_items{ to = to, disco_items = {
		node = sluift.disco.features.COMMANDS }})
	print(commands)

	-- Get the initial form
	result = assert(c:set_command{ to = to, command = {
			action = 'execute', node = 'http://jabber.org/protocol/admin#get-user-roster'}})

	-- Fill in the form
	submission = result.form:create_submission()
	submission.accountjid = os.getenv("SLUIFT_JID")

	-- Submit the form
	result = assert(c:set_command{ to = to, command = {
			action = 'complete', node = 'http://jabber.org/protocol/admin#get-user-roster',
			session_id = result.session_id, form = submission}})
	for _, v in ipairs(result.form.roster.values) do
		print(v)
	end
end)
