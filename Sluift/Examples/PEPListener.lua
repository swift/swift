--[[
	Copyright (c) 2010-2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[

	PEP Listener

	Listens to a series of PEP events of all contacts.

	The following environment variables are used:
	- SLUIFT_JID, SWIFT_PASS: JID and password to log in with
	- SLUIFT_DEBUG: Sets whether debugging should be turned on

--]]

require "sluift"

sluift.debug = os.getenv("SLUIFT_DEBUG") or false

pep_protocols = {
	[sluift.disco.features.USER_LOCATION] = true,
	[sluift.disco.features.USER_TUNE] = true,
	[sluift.disco.features.USER_ACTIVITY] = true,
	[sluift.disco.features.USER_AVATAR_METADATA] = true,
	[sluift.disco.features.USER_PROFILE] = true,
}

client = sluift.new_client(os.getenv("SLUIFT_JID"), os.getenv("SLUIFT_PASS"))
client:connect(function (c) 
	features = {sluift.disco.features.DISCO_INFO}
	for protocol in pairs(pep_protocols) do
		features[#features+1] = protocol .. '+notify'
	end

	c:set_caps_node("http://swift.im/PEPListener")
	c:set_disco_info({identities = {{name = 'PEPListener'}}, features = features})
	c:send_presence()
	for event in c:pubsub_events() do
		if event._type == 'pubsub_event_items' and pep_protocols[event.node] then
			print("<" .. event.from .. "> " .. tostring(event.item))
		end
	end
end)
