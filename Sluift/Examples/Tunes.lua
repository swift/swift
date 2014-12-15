--[[
	Copyright (c) 2014 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

--[[

	Tune Publisher/Listener

	Publishes the currently playing tune in iTunes, and prints 
	the playing tunes of contacts

	The following environment variables are used:
	- SLUIFT_JID, SWIFT_PASS: JID and password to log in with
	- SLUIFT_DEBUG: Sets whether debugging should be turned on

--]]

require 'sluift'

sluift.debug = os.getenv('SLUIFT_DEBUG') or false

client = sluift.new_client(os.getenv('SLUIFT_JID'), os.getenv('SLUIFT_PASS'))
client:connect(function (c) 
	-- Send initial presence (with service discovery information)
	c:set_caps_node('http://swift.im/Tunes')
	c:set_disco_info{
		identities = {{name = 'Tunes'}}, 
		features = {
			sluift.disco.features.DISCO_INFO,
			sluift.disco.features.USER_TUNE .. '+notify'
	}}
	c:send_presence{priority = -1}

	local pubsub = c:pubsub(sluift.jid.to_bare(c:jid()))
	local tunes_node = pubsub:node(sluift.disco.features.USER_TUNE)
	local current_track = nil
	while true do
		-- Publish currently playing tune
		if sluift.itunes then
			local track = sluift.itunes.get_current_track()
			if track ~= current_track then
				tunes_node:publish{item = {
					_type = 'user_tune',
					title = track.name,
					artist = track.artist,
					track = track.track_number,
					source = track.album,
					length = track.length,
					rating = track.rating and track.rating / 10 or nil
				}}
				current_track = track
			end
		end

		-- Print incoming events for a while
		for event in c:pubsub_events{timeout = 1000} do
			if event._type == 'pubsub_event_items' 
					and event.node == sluift.disco.features.USER_TUNE
					and event.item.title then
				print(event.from .. ' is playing "' 
					.. (event.item.artist or '<Unknown Artist>') .. ' - ' 
					.. event.item.title .. '"')
			end
		end
	end
end)
