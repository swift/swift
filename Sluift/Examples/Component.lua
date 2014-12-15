--[[
	Copyright (c) 2014 Edwin Mons and Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[
	
	Component example.

	This script connects to an XMPP server as a component, and listens to
	messages received.
	
	The following environment variables are used:
	* SLUIFT_COMP_DOMAIN: Component domain name
	* SLUIFT_COMP_SECRET: Component secret
	* SLUIFT_COMP_HOST: XMPP server host name
	* SLUIFT_COMP_PORT: XMPP server component port
	* SLUIFT_JID: Recipient of presence and initial message
	* SLUIFT_DEBUG: Sets whether debugging should be turned on

--]]

require "sluift"

sluift.debug = os.getenv("SLUIFT_DEBUG") or false

config = {
	domain = os.getenv('SLUIFT_COMP_DOMAIN'),
	secret = os.getenv('SLUIFT_COMP_SECRET'),
	host = os.getenv('SLUIFT_COMP_HOST'),
	port = os.getenv('SLUIFT_COMP_PORT'),
	jid = os.getenv('SLUIFT_JID')
}

-- Create the component, and connect
comp = sluift.new_component(config.domain, config.secret);
comp:connect(config)

-- Send initial presence and message
-- Assumes the remote client already has this component user on his roster
comp:send_presence{from='root@' .. config.domain, to=config.jid}
comp:send_message{from='root@' .. config.domain, to=config.jid, body='Component active'}

-- Listen for messages, and reply if one is received
for message in comp:messages() do
	print("Received a message from " .. message.from)
	comp:send_message{to=message.from, from=message.to, body='I received: ' .. message['body']}

	-- Send out a ping to demonstrate we can do more than just send messages
	comp:get{to=message.from, query='<ping xmlns="urn:xmpp:ping"/>'}
end

comp:disconnect()

