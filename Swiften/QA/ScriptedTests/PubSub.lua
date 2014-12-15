 --[[
	Copyright (c) 2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

require 'sluift'

sluift.debug = os.getenv("SLUIFT_DEBUG")

local publisher
local publisher_jid = os.getenv("SLUIFT_JID")
local pubsub_jid = os.getenv("SLUIFT_PUBSUB_SERVICE") or sluift.jid.to_bare(publisher_jid)
local pubsub
local node_id = os.getenv("SLUIFT_PUBSUB_NODE") or "http://swift.im/Swiften/QA/PubSub"
local node

local subscriber
local subscriber_jid = os.getenv("SLUIFT_JID2")
local subscriber_pubsub
local subscriber_node

local publish_item = {id = 'item_id', data = {{ _type = 'software_version', name = 'MyTest', os = 'Lua' }} }
local publish_item2 = {id = 'item_id2', data = {{ _type = 'software_version', name = 'MyTest2', os = 'Lua' }} }
local publish_item3 = {id = 'item_id3', data = {{ _type = 'software_version', name = 'MyTest3', os = 'Lua' }} }

--------------------------------------------------------------------------------
-- Helper methods
--------------------------------------------------------------------------------

function purge_pubsub_events(client)
	client:for_each_pubsub_event({timeout = 2000}, function() end)
end

--------------------------------------------------------------------------------
-- 5. Entity use cases
--------------------------------------------------------------------------------
function test_entity_use_cases() 
	node:delete()

	-- 5.2 List nodes
	assert(node:create())
	local nodes = assert(pubsub:list_nodes()).items
	local found_item = false
	for _, node in ipairs(nodes) do
		if node.node == node_id then found_item = true end
	end
	assert(found_item)
	assert(node:delete())


	-- 5.5 Discover items of node
	assert(node:create())
	assert(node:publish {items = {publish_item}})
	local items = assert(node:list_items()).items
	assert(#items == 1)
	assert(items[1].name == 'item_id')
	assert(node:delete())

	-- 5.6 Subscriptions
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	local service_subscriptions = assert(subscriber_pubsub:get_subscriptions())
	assert(#service_subscriptions == 1)
	assert(service_subscriptions[1].node == node_id)
	assert(service_subscriptions[1].jid == subscriber_jid)
	assert(service_subscriptions[1].subscription == 'subscribed')
	local node_subscriptions = assert(subscriber_node:get_subscriptions())
	assert(#node_subscriptions == 1)
	assert(node_subscriptions[1].jid == subscriber_jid)
	assert(node_subscriptions[1].subscription == 'subscribed')
	assert(node:delete())

	-- 5.7 Retrieve affiliations
	--print(pubsub:get_affiliations()) -- Freezes Isode
	--print(node:get_affiliations()) -- Freezes isode
end

--------------------------------------------------------------------------------
-- 6. Subscriber use cases
--------------------------------------------------------------------------------

function test_subscriber_use_cases()
	node:delete()

	-- 6.1 Subscribe to a node
	assert(node:create())
	local subscription = assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	-- TODO: Test subscription id etc. Doesn't work with M-link atm
	-- TODO: Test pending subscription
	-- TODO: Test configuration required
	assert(node:delete())

	-- 6.2 Unsubscribe from a node
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	assert(subscriber_node:unsubscribe({ jid = subscriber_jid }))
	assert(node:delete())

	-- 6.3 Configure subscription options
	-- TODO: Not supported by M-Link? Finish it later
	--assert(node:create())	
	--assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	--local options = assert(subscriber_node:get_subscription_options({ jid = subscriber_jid }))
	--print(options)
	--assert(node:delete())

	-- 6.4 Request default subscription configuration options
	-- TODO: Not supported by M-Link? Finish it later
	--local options = assert(subscriber_pubsub:get_default_subscription_options())
	--print(options)
	--local options = assert(subscriber_node:get_default_subscription_options())
	--print(options)
	
	-- 6.5 Retrieve items of a node
	assert(node:create())
	assert(node:set_configuration{configuration = {['pubsub#max_items'] = '10'}})
	assert(node:publish{item = publish_item})
	assert(node:publish{item = publish_item2})
	local items = assert(subscriber_node:get_items())
	assert(#items == 2)
	assert(items[1].id == 'item_id')
	assert(items[1].data[1].name == 'MyTest')
	assert(items[2].id == 'item_id2')
	assert(items[2].data[1].name == 'MyTest2')
	assert(node:delete())

	-- 6.5.7 Requesting most recent items
	assert(node:create())
	assert(node:set_configuration{configuration = {['pubsub#max_items'] = '10'}})
	assert(node:publish{item = publish_item})
	assert(node:publish{item = publish_item2})
	assert(node:publish{item = publish_item3})
	local items = assert(subscriber_node:get_items{maximum_items = 2})
	assert(#items == 2)
	assert(items[1].id == 'item_id2')
	assert(items[1].data[1].name == 'MyTest2')
	assert(items[2].id == 'item_id3')
	assert(items[2].data[1].name == 'MyTest3')
	assert(node:delete())
	
	-- 6.5.8 requesting specific item
	assert(node:create())
	assert(node:set_configuration{configuration = {['pubsub#max_items'] = '10'}})
	assert(node:publish{item = publish_item})
	assert(node:publish{item = publish_item2})
	local items = assert(subscriber_node:get_item{id = 'item_id2'})
	assert(#items == 1)
	assert(items[1].id == 'item_id2')
	assert(items[1].data[1].name == 'MyTest2')
	assert(node:delete())
end

--------------------------------------------------------------------------------
-- 7. Publisher use cases
--------------------------------------------------------------------------------

function test_publisher_use_cases()
	node:delete()

	-- 7.1 Publish item to a node
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	purge_pubsub_events(subscriber)
	assert(node:publish {items = {publish_item}})
	local event = assert(subscriber:get_next_event { type = 'pubsub' })
	assert(event.from == publisher_jid)
	assert(event._type == 'pubsub_event_items')
	assert(event.items[1].id == publish_item.id)
	assert(event.items[1].data[1].os == publish_item.data[1].os)
	assert(event.item.os == publish_item.data[1].os)
	assert(node:delete())

	-- 7.2 Delete item from a node
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	assert(node:publish {items = {publish_item}})
	assert(node:retract { id = 'item_id', notify = true })
	assert(node:delete())

	-- 7.2.2.1 Delete and notify
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	assert(node:publish {items = {publish_item}})
	purge_pubsub_events(subscriber)
	assert(node:retract { id = 'item_id', notify = true })
	local event = assert(subscriber:get_next_event { type = 'pubsub' })
	assert(event._type == 'pubsub_event_items')
	assert(event.retracts[1].id == 'item_id')
	assert(node:delete())

	-- Publish an unknown element type
	atom = [[
		<entry xmlns='http://www.w3.org/2005/Atom'>
			<title>Down the Rabbit Hole</title>
			<summary>
				Alice was beginning to get very tired of sitting by her sister on the
				bank and of having nothing to do: once or twice she had peeped into the
				book her sister was reading, but it had no pictures or conversations in
				it, "and what is the use of a book," thought Alice, "without pictures
				or conversations?'
			</summary>
			<link rel='alternate' type='text/html' 
				href='http://www.gutenberg.org/files/11/11-h/11-h.htm#link2HCH0001'/>
			<id>tag:gutenberg.org,2008:entry-1234</id>
			<published>2008-06-25T18:30:02Z</published>
			<updated>2008-06-25T18:30:02Z</updated>
		</entry>
	]]
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	purge_pubsub_events(subscriber)
	assert(node:publish { item = atom })
	local event_item = assert(subscriber:get_next_event { type = 'pubsub' }).item
	assert(event_item._type == 'dom')
	assert(event_item.ns == 'http://www.w3.org/2005/Atom')
	assert(event_item.tag == 'entry')
	assert(node:delete())
end

--------------------------------------------------------------------------------
-- 8 Owner Use Cases
--------------------------------------------------------------------------------

function test_owner_use_cases()
	node:delete()

	-- 8.1 Create a node
	--   Create node with default config
	assert(node:create())
	configuration = assert(node:get_configuration())['data']
	assert(node:delete())

	--   Create node with custom config
	print("Creating with configuration")
	assert(node:create { configuration = { ['pubsub#access_model'] = 'whitelist' } })
	configuration = assert(node:get_configuration())['data']
	assert(configuration['pubsub#access_model']['value'] == 'whitelist')
	assert(node:delete())

	-- 8.2 Configure node
	--   Set configuration
	assert(node:create())
	assert(node:set_configuration {configuration = {['pubsub#access_model'] = 'whitelist'}})
	configuration = assert(node:get_configuration())['data']
	assert(configuration['pubsub#access_model']['value'] == 'whitelist')
	assert(node:delete())

	-- Untested: 8.2.5.3 Success With Notifications
	
	-- 8.3 Request Default Node Configuration Options
	configuration = assert(pubsub:get_default_configuration())['data']
	assert(configuration['pubsub#access_model'] ~= nil)

	-- 8.4 Delete node
	--   Without redirection (see above)
	--   With redirection
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	purge_pubsub_events(subscriber)
	assert(node:delete {redirect = 'foo@bar.com'})
	-- FIXME: M-Link doesn't send out an event. Test this later.
	--local event = assert(subscriber:get_next_event { type = 'pubsub' })
	--print(event)
	--assert(event._type == 'pubsub_event_items')
	--assert(event.retracts[1].id == 'item_id')
	
	-- 8.5 Purge node items
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	-- Publishing multiple items doesn't seem to work in M-Link
	--assert(node:publish {items = { publish_item, publish_item2 }})
	assert(node:publish {items = {publish_item} })
	assert(node:publish {items = {publish_item2} })
	purge_pubsub_events(subscriber)
	if node:purge() then
		-- Hasn't worked yet. Add more to this test later (including notifications)
	else 
		print("Warning: Purge not supported. Skipping test")
	end
	assert(node:delete())
	
	-- 8.6 Manage subscription requests
	-- TODO	

	-- 8.7 Process pending subscription requests
	-- TODO
	
	-- 8.8 Manage Subscriptions
	assert(node:create())
	assert(subscriber_node:subscribe({ jid = subscriber_jid }))
	local items = assert(node:get_owner_subscriptions())
	assert(#items == 1)
	assert(items[1].jid == subscriber_jid)
	assert(items[1].subscription == "subscribed")
	assert(node:delete())
 
	-- 8.9 Manage Affiliations
	assert(node:create())
	assert(node:set_owner_affiliations{affiliations = {{jid = subscriber_jid, type = 'publisher'}}})
	local items = assert(node:get_owner_affiliations())
	assert(#items == 2)
	assert(items[1].jid == publisher_jid)
	assert(items[1].type == "owner")
	assert(items[2].jid == subscriber_jid)
	assert(items[2].type == "publisher")
	assert(node:delete())
end

function run_tests() 
	connect_options = {}

	-- Set up publisher & subscriber
	publisher = sluift.new_client(publisher_jid, os.getenv("SLUIFT_PASS"))
	assert(publisher:connect(connect_options))
	subscriber = sluift.new_client(subscriber_jid, os.getenv("SLUIFT_PASS2"))
	assert(subscriber:connect(connect_options))

	pubsub = publisher:pubsub(pubsub_jid)
	node = pubsub:node(node_id)

	subscriber_pubsub = subscriber:pubsub(pubsub_jid)
	subscriber_node = subscriber_pubsub:node(node_id)

	-- The tests
	test_entity_use_cases()
	test_subscriber_use_cases()
	test_publisher_use_cases()
	test_owner_use_cases()
end

success, err = pcall(run_tests)

if subscriber then subscriber:disconnect() end
if publisher then publisher:disconnect() end

if not success then
	print(err)
	os.exit(-1)
end
