--[[
	Copyright (c) 2013 Remko Tronçon
	Licensed under the GNU General Public License.
	See the COPYING file for more information.
--]]

local Client = {}
local PubSub = {}
local PubSubNode = {}

--------------------------------------------------------------------------------
-- Utility methods
--------------------------------------------------------------------------------

local function merge_tables(...)
	local result = {}
	for _, table in ipairs({...}) do
		for k, v in pairs(table) do
			result[k] = v
		end
	end
	return result
end

local function clone_table(table) 
 return merge_tables(table)
end

local function parse_options(unnamed_parameters, arg1, arg2)
	local options = {}
	if type(arg1) == 'table' then
		options = arg1
		f = arg2
	elseif type(arg1) == 'function' then
		f = arg1
	end
	options.f = f or options.f
	return clone_table(options)
end


local function table_value_tostring(value)
	local result = tostring(value)
	if type(value) == 'number' then return result
	elseif type(value) == 'boolean' then return result
	elseif type(value) == 'string' then return "'" .. result .. "'"
	else return '<' .. result .. '>'
	end
end

local function table_tostring(table, print_functions, indent, accumulator, history)
	local INDENT = '  '
	local accumulator = accumulator or ''
	local history = history or {}
	local indent = indent or ''
	accumulator = accumulator .. '{'
	history[table] = true
	local is_first = true
	for key, value in pairs(table) do
		if print_functions or type(value) ~= 'function' then
			if not is_first then
				accumulator = accumulator .. ','
			end
			is_first = false
			accumulator = accumulator .. '\n' .. indent .. INDENT .. '[' .. table_value_tostring(key) .. '] = '
			if type(value) == 'table' then
				if history[value] then
					accumulator = accumulator .. "..."
				else
					accumulator = table_tostring(value, print_functions, indent .. INDENT, accumulator, history)
				end
			else
				accumulator = accumulator .. table_value_tostring(value)
			end
		end
	end
	history[table] = false
	if not is_first then
		accumulator = accumulator .. '\n' .. indent
	end
	accumulator = accumulator .. '}'
	return accumulator
end

local function tprint(table)
	print(table_tostring(table, true))
end

local function register_table_tostring(table)
	if type(table) == 'table' then
		local metatable = getmetatable(table)
		if not metatable then
			metatable = {}
			setmetatable(table, metatable)
		end
		metatable.__tostring = table_tostring
	end
	return table
end

local function get_by_type(table, typ)
	for _, v in ipairs(table) do
		if v['_type'] == typ then
			return v
		end
 end
end

local function register_get_by_type_index(table)
	if type(table) == 'table' then
		local metatable = getmetatable(table)
		if not metatable then
			metatable = {}
			setmetatable(table, metatable)
		end
		metatable.__index = get_by_type
	end
	return table
end

--------------------------------------------------------------------------------
-- Client
--------------------------------------------------------------------------------

function Client.connect (client, ...)
	local options = parse_options({}, ...)
	local f = options.f
	client:async_connect(options)
	client:wait_connected()
	if f then
		local result = { xpcall(function() return f(client) end, debug.traceback) }
		client:disconnect()
		if result[1] then
			table.remove(result, 1)
			return unpack(result)
		else
			error(result[2])
		end
	end
	return true
end

function Client.events (client, options)
	local function client_events_iterator(s)
		return s['client']:get_next_event(s['options'])
	end
	return client_events_iterator, {client = client, options = options}
end

function Client.for_each_event (client, ...)
	local options = parse_options({}, ...)
	if not type(options.f) == 'function' then error('Expected function') end
	for event in client:events(options) do
		local result = options.f(event)
		if result then
			return result
		end
	end
end

for method, event_type in pairs({message = 'message', presence = 'presence', pubsub_event = 'pubsub'}) do
	Client['for_each_' .. method] = function (client, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return client:for_each_event (options)
	end

	Client['get_next_' .. method] = function (client, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return client:get_next_event(options)
	end
end

for method, event_type in pairs({messages = 'message', pubsub_events = 'pubsub'}) do
	Client[method] = function (client, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return client:events (options)
	end
end

--
-- Register get_* and set_* convenience methods for some type of queries
--
-- Example usages:
--	client:get_software_version{to = 'alice@wonderland.lit'}
--	client:set_command{to = 'alice@wonderland.lit', command = { type = 'execute', node = 'uptime' }}
--
local get_set_shortcuts = {
	get = {'software_version', 'disco_items', 'xml', 'dom', 'vcard'},
	set = {'command'}
}
for query_action, query_types in pairs(get_set_shortcuts) do
	for _, query_type in ipairs(query_types) do
		Client[query_action .. '_' .. query_type] = function (client, options)
			options = options or {}
			if type(options) ~= 'table' then error('Invalid options: ' .. options) end 
			options['query'] = merge_tables({_type = query_type}, options[query_type] or {})
			return client[query_action](client, options)
		end
	end
end

function Client.pubsub (client, jid)
	local result = { client = client, jid = jid }
	setmetatable(result, PubSub)
	return result
end

--------------------------------------------------------------------------------
-- PubSub
--------------------------------------------------------------------------------

PubSub.__index = PubSub

local function process_pubsub_event (event)
	if event._type == 'pubsub_event_items' then
		-- Add 'item' shortcut to payload of first item
		event.item = event.items and event.items[1] and 
			event.items[1].data and event.items[1].data[1]
	end
end

function PubSub.list_nodes (service, options)
	return service.client:get_disco_items(merge_tables({to = service.jid}, options))
end

function PubSub.node (service, node)
	local result = { client = service.client, jid = service.jid, node = node }
	setmetatable(result, PubSubNode)
	return result
end

local simple_pubsub_queries = {
	get_default_configuration = 'pubsub_owner_default',
	get_subscriptions = 'pubsub_subscriptions',
	get_affiliations = 'pubsub_affiliations',
	get_default_subscription_options = 'pubsub_default',
}
for method, query_type in pairs(simple_pubsub_queries) do
	PubSub[method] = function (service, options)
		options = options or {}
		return service.client:query_pubsub(merge_tables(
			{ type = 'get', to = service.jid, query = { _type = query_type } },
			options))
	end
end

for _, method in ipairs({'events', 'get_next_event', 'for_each_event'}) do
	PubSub[method] = function (node, ...)
		local options = parse_options({}, ...)
		options['if'] = function (event) 
			return event.type == 'pubsub' and event.from == node.jid and event.node == node
		end
		return node.client[method](node.client, options)
 end
end

--------------------------------------------------------------------------------
-- PubSubNode
--------------------------------------------------------------------------------

PubSubNode.__index = PubSubNode

local function pubsub_node_configuration_to_form(configuration)
	if not configuration then
		return
	end
	local fields = { {name = 'form_type', value = 'http://jabber.org/protocol/pubsub#node_config'} }
	for var, value in pairs(configuration) do
		fields[#fields+1] = { name = var, value = value }
	end
	return { type = "submit", fields = fields }
end

function PubSubNode.list_items (node, options)
	return node.client:get_disco_items(merge_tables({to = node.jid, disco_items = { node = node.node }}, options))
end

local simple_pubsub_node_queries = {
	get_configuration = 'pubsub_owner_configure',
	get_subscriptions = 'pubsub_subscriptions',
	get_affiliations = 'pubsub_affiliations',
	get_owner_subscriptions = 'pubsub_owner_subscriptions',
	get_owner_affiliations = 'pubsub_owner_affiliations',
	get_default_subscription_options = 'pubsub_default',
}
for method, query_type in pairs(simple_pubsub_node_queries) do
	PubSubNode[method] = function (node, options)
		return node.client:query_pubsub(merge_tables({ 
			type = 'get', to = node.jid, query = {
					_type = query_type, node = node.node 
			}}, options))
	end
end

function PubSubNode.get_items (node, ...)
	local options = parse_options({}, ...)
	local items = options.items or {}
	if options.maximum_items then
		items = merge_tables({maximum_items = options.maximum_items}, items)
	end
	items = merge_tables({_type = 'pubsub_items', node = node.node}, items)
	return node.client:query_pubsub(merge_tables({ 
		type = 'get', to = node.jid, query = items}, options))
end

function PubSubNode.get_item (node, ...)
	local options = parse_options({}, ...)
	if not type(options.id) == 'string' then error('Expected ID') end
	return PubSubNode.get_items(node, {items = {{id = options.id}}})
end

function PubSubNode.create (node, options)
	options = options or {}
	local configure
	if options['configuration'] then
		configure = { data = pubsub_node_configuration_to_form(options['configuration']) }
	end
	return node.client:query_pubsub(merge_tables(
		{ type = 'set', to = node.jid, query = { 
				_type = 'pubsub_create', node = node.node, configure = configure }
		}, options))
end

function PubSubNode.delete (node, options)
	options = options or {}
	local redirect
	if options['redirect'] then
		redirect = {uri = options['redirect']}
	end
	return node.client:query_pubsub(merge_tables({ type = 'set', to = node.jid, query = { 
			_type = 'pubsub_owner_delete', node = node.node, redirect = redirect 
		}}, options))
end

function PubSubNode.set_configuration(node, options)
	options = options or {}
	local configuration = pubsub_node_configuration_to_form(options['configuration'])
	return node.client:query_pubsub(merge_tables(
		{ type = 'set', to = node.jid, query = { 
				_type = 'pubsub_owner_configure', node = node.node, data = configuration }
		}, options))
end

function PubSubNode.set_owner_affiliations(node, ...)
	local options = parse_options({}, ...)
	return node.client:query_pubsub(merge_tables({ 
		type = 'set', to = node.jid, query = merge_tables({
				_type = 'pubsub_owner_affiliations', node = node.node, 
		}, options.affiliations)}, options))
end


function PubSubNode.subscribe(node, ...)
	local options = parse_options(...)
	local jid = options.jid or sluift.jid.to_bare(node.client:jid())
	return node.client:query_pubsub(merge_tables(
		{ type = 'set', to = node.jid, query = { 
				_type = 'pubsub_subscribe', node = node.node, jid = jid }
		}, options))
end

function PubSubNode.unsubscribe(node, options)
	options = options or {}
	return node.client:query_pubsub(merge_tables(
		{ type = 'set', to = node.jid, query = { 
				_type = 'pubsub_unsubscribe', node = node.node, jid = options['jid'] }
		}, options))
end

function PubSubNode.get_subscription_options (node, options)
	return node.client:query_pubsub(merge_tables(
		{ type = 'get', to = node.jid, query = { 
				_type = 'pubsub_options', node = node.node, jid = options['jid'] }
		}, options))
end

function PubSubNode.publish(node, ...)
	local options = parse_options({}, ...)
	local items = options.items or {}
	if options.item then
		if type(options.item) == 'string' or options.item._type then
			items = {{id = options.id, data = { options.item } }}
			options.id = nil 
		else
			items = { options.item }
		end
		options.item = nil
	end
	return node.client:query_pubsub(merge_tables(
		{ type = 'set', to = node.jid, query = { 
				_type = 'pubsub_publish', node = node.node, items = items }
		}, options))
end

function PubSubNode.retract(node, ...)
	local options = parse_options({}, ...)
	local items = options.items
	if options.id then
		items = {{id = options.id}}
	end
	return node.client:query_pubsub(merge_tables(
		{ type = 'set', to = node.jid, query = { 
				_type = 'pubsub_retract', node = node.node, items = items, notify = options['notify']
		}}, options))
end

function PubSubNode.purge(node, ...)
	local options = parse_options({}, ...)
	return node.client:query_pubsub(merge_tables(
		{ type = 'set', to = node.jid, query = { 
				_type = 'pubsub_owner_purge', node = node.node
		}}, options))
end

-- Iterators over events
for _, method in ipairs({'events', 'get_next_event', 'for_each_event'}) do
	PubSubNode[method] = function (node, ...)
		local options = parse_options({}, ...)
		options['if'] = function (event) 
			return event.type == 'pubsub' and event.from == node.jid and event.node == node.node
		end
		return node.client[method](node.client, options)
	end
end

--------------------------------------------------------------------------------
-- Service discovery
--------------------------------------------------------------------------------

local disco = {
	features = {
		DISCO_INFO = 'http://jabber.org/protocol/disco#info',
		COMMANDS = 'http://jabber.org/protocol/commands',
		USER_LOCATION = 'http://jabber.org/protocol/geoloc',
		USER_TUNE = 'http://jabber.org/protocol/tune',
		USER_AVATAR_METADATA = 'urn:xmpp:avatar:metadata',
		USER_ACTIVITY = 'http://jabber.org/protocol/activity',
		USER_PROFILE = 'urn:xmpp:tmp:profile'
	}
}

--------------------------------------------------------------------------------

return {
	Client = Client,
	register_table_tostring = register_table_tostring,
	register_get_by_type_index = register_get_by_type_index,
	process_pubsub_event = process_pubsub_event,
	tprint = tprint,
	disco = disco,
}
