--[[
	Copyright (c) 2013-2017 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

local sluift = select(1, ...)
local _G = _G
local pairs, ipairs, print, tostring, type, error, assert, next, rawset, xpcall, unpack, io = pairs, ipairs, print, tostring, type, error, assert, next, rawset, xpcall, unpack, io
local setmetatable, getmetatable = setmetatable, getmetatable
local string = require "string"
local table = require "table"
local debug = require "debug"
_ENV = nil

--------------------------------------------------------------------------------
-- Table utility methods
--------------------------------------------------------------------------------

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

local function register_table_tostring(table, print_functions)
	if type(table) == 'table' then
		local metatable = getmetatable(table)
		if not metatable then
			metatable = {}
			setmetatable(table, metatable)
		end
		if print_functions then
			metatable.__tostring = function(table) return table_tostring(table, true) end
		else
			metatable.__tostring = table_tostring
		end
	end
end

-- FIXME: Not really a good or efficiant equals, but does the trick for now
local function table_equals(t1, t2) 
	return tostring(t1) == tostring(t2) 
end

local function register_table_equals(table)
	if type(table) == 'table' then
		local metatable = getmetatable(table)
		if not metatable then
			metatable = {}
			setmetatable(table, metatable)
		end
		metatable.__eq = table_equals
	end
end

local function merge_tables(...)
	local result = {}
	for _, table in ipairs({...}) do
		for k, v in pairs(table) do
			result[k] = v
		end
	end
	return result
end

local function copy(object)
	if type(object) == 'table' then
		local copy = {}
		for key, value in pairs(object) do
			copy[key] = value
		end
		return copy
	else
		return object
	end
end

local function clear(table)
	setmetatable(table, nil)
	for key, value in pairs(table) do
		rawset(table, key, nil)
	end
end

local function trim(string)
	return string:gsub("^%s*(.-)%s*$", "%1")
end

local function keys(table)
	local result = {}
	for key in pairs(table) do
		result[#result+1] = key
	end
	return result
end

local function insert_all(table, values)
	for _, value in pairs(values) do
		table[#table+1] = value
	end
end

--------------------------------------------------------------------------------
-- Help
--------------------------------------------------------------------------------

-- Contains help for native methods that we want access to from here
local extra_help = {}
local component_extra_help = {}
local help_data = {}
local help_classes = {}
local help_class_metatables = {}

local _H

local function get_synopsis(description) 
	return description:gsub("[\n\r].*", "")
end

local function format_description(text)
	local result = {}
	local trim_whitespace
	for line in (text .. "\n"):gmatch"(.-)\n" do
		if not trim_whitespace and line:find('[^%s]') then
			trim_whitespace = line:match("^(%s*)")
		end
		if trim_whitespace then
			line = line:gsub("^" .. trim_whitespace, "")
		end
		table.insert(result, line)
	end
	return trim(table.concat(result, "\n"))
end

local function strip_links(text)
	return text:gsub("(@{(%w*)})", "`%2`")
end

local function register_help(target, help) 
	assert(target)
	if not help then
		help = _H
	end
	assert(help)

	-- Transform description into canonical representation
	local parameters = {}
	for _, parameter in pairs(help.parameters or {}) do
		local parameter_description = parameter[2]
		if parameter_description and #parameter_description == 0 then
			parameter_description = nil
		end
		if type(parameter) == "table" then
			parameters[#parameters+1] = { name = parameter[1], description = parameter_description }
		else
			parameters[#parameters+1] = { name = parameter }
		end
	end
	local options = {}
	for option_name, option_description in pairs(help.options or {}) do
		if type(option_description) == "table" then
			options[#options+1] = { name = option_description.name, description = option_description.description }
		else
			options[#options+1] = { name = option_name, description = option_description }
		end
	end
	local description = format_description(help[1] or help.description or "")
	local synopsis = get_synopsis(description)
	if #description == 0 then
		synopsis = nil
		description = nil
	end
	local data = {
		description = description,
		synopsis = synopsis,
		parameters = parameters,
		options = options,
		classes = help.classes
	}
	register_table_tostring(data, true)
	help_data[target] = data
end

local function register_class_help(class, help)
	help_classes[#help_classes+1] = class
	register_help(class, help)
end

local function register_class_table_help(target, class, help)
	register_help(target, help)
	help_class_metatables[class] = target
	register_class_help(class, help)
end

_H = {
	[[ 
		Retrieves the help information from `target`.

		Returns a table with the following fields: 

		- `description`: the description of `target`
		- `parameters`: an array of parameters of `target` represented as tables with `name` and `description` fields.
		- `options`: an array of options (named parameters) of `target` represented as tables with `name` and 
		  `description` fields.
		- `methods`: an array of methods
		- `fields`: an array of fields
	]],
	parameters = { {"target", "The target to retrieve help of"} }
}
local function get_help(target) 
	if not target then error("Nil argument or argument missing") end
	local help = help_data[target] or help_data[getmetatable(target)] or {}

	-- Collect child methods and fields
	local children = {}
	if type(target) == "table" then children = target end
	local mt
	if type(target) == "string" then
		mt = help_class_metatables[target]
	else
		mt = getmetatable(target)
	end
	if mt and type(mt.__index) == "table" then
		children = merge_tables(children, mt.__index)
	end

	local methods = {}
	local fields = {}
	for name, value in pairs(children) do
		if name:sub(1, 1) ~= "_" then 
			if type(value) == "function" then
				methods[#methods+1] = { name = name, ref = value }
			else
				fields[#fields+1] = { name = name, description = nil }
			end
		end
	end
	if next(methods) ~= nil then
		help.methods = methods
	end
	if next(fields) ~= nil then
		help.fields = fields
	end
	if next(help) then
		return help
	else
		return nil
	end
end
register_help(get_help)

_H = {
	[[ 
		Prints the help of `target`.

		`target` can be any object. When `target` is a string, prints the help of the class with
		the given name.
	]],
	parameters = { {"target", "The target to retrieve help of"} }
}
local function help(target)
	print()
	if not target then 
		print("Call `help(target)` to get the help of a specific `target`.")
		print("`target` can be any object. When `target` is a string, prints")
		print("the help of the class with the given name.")
		print()
		print("For general information about sluift, type:")
		print("  help(sluift)")
		print()
		return
	end
	local data = get_help(target)
	if not data then
		print("No help available\n")
		return
	end

	-- Collect help of children
	local methods = {}
	for _, method in pairs(data.methods or {}) do
		local description
		local method_help = get_help(method.ref)
		if method_help and method_help.description then
			description = method_help.synopsis
		end
		methods[#methods+1] = { name = method.name, description = description }
	end
	local fields = copy(data.fields or {})

	table.sort(methods, function (a, b) return (a.name or "") < (b.name or "") end)
	table.sort(fields, function (a, b) return (a.name or "") < (b.name or "") end)

	local classes = {}
	for _, class in pairs(data.classes or {}) do
		classes[#classes+1] = { name = class, description = get_help(class).synopsis }
	end

	print(strip_links(data.description) or "(No description available)")
	for _, p in ipairs({
			{"Parameters", data.parameters}, {"Options", data.options}, {"Methods", methods}, {"Fields", fields}, {"Classes", classes}}) do
		if p[2] and next(p[2]) ~= nil then
			print()
			print(p[1] .. ":")
			for _, parameter in ipairs(p[2]) do
				if parameter.description then
					print("  " .. parameter.name .. ": " .. strip_links(parameter.description))
				else
					print("  " .. parameter.name)
				end
			end
		end
	end

	print()
end
register_help(help)

--------------------------------------------------------------------------------
-- Utility methods
--------------------------------------------------------------------------------

_H = {
	[[ Perform a shallow copy of `object`. ]],
	parameters = {{"object", "the object to copy"}}
}
register_help(copy)

_H = {
	[[ Pretty-print a table ]],
	parameters = {{"table", "the table to print"}}
}
local function tprint(table)
	print(table_tostring(table, true))
end
register_help(tprint)

local function remove_help_parameters(elements, table)
	if type(elements) ~= "table" then
		elements = {elements}
	end
	local result = copy(table)
	for k, v in ipairs(table) do
		for _, element in ipairs(elements) do
			if v.name == element then
				result[k] = nil
			end
		end
	end
	return result
end

local function parse_options(unnamed_parameters, arg1, arg2)
	local options = {}
	local f
	if type(arg1) == 'table' then
		options = arg1
		f = arg2
	elseif type(arg1) == 'function' then
		f = arg1
	end
	options.f = f or options.f
	return copy(options)
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

local function call(options)
	local f = options[1]
	local result = { xpcall(f, debug.traceback) }
	if options.finally then
		options.finally()
	end
	if result[1] then
		table.remove(result, 1)
		return unpack(result)
	else
		error(result[2])
	end
end

local function read_file(file)
	local f = io.open(file, 'rb')
	local result = f:read('*all')
	f:close()
	return result
end

_H = {
	[[ Generate a form table, suitable for PubSubConfiguration and MAMQuery ]],
	parameters = { {"fields", "The fields that will be converted into a form table"},
		       {"form_type", "If specified, add a form_type field with this value"},
		       {"type", "Form type, e.g. 'submit'"} }
}
local function create_form(...)
	local options = parse_options({}, ...)
	local result = { fields = {} }
	-- FIXME: make nicer when parse_options binds positional arguments to names
	if options.fields then
		for var, value in pairs(options.fields) do
			result.fields[#result.fields+1] = { name = var, value = value }
		end
	elseif options[1] then
		for var, value in pairs(options[1]) do
			result.fields[#result.fields+1] = { name = var, value = value }
		end
	end
	if options.form_type then
		result.fields[#result.fields+1] = { name = 'FORM_TYPE', value = options.form_type }
	end
	result['type'] = options.type
	return result
end

--------------------------------------------------------------------------------
-- Metatables
--------------------------------------------------------------------------------

_H = {
	[[ Client interface ]]
}
local Client = {
	_with_prompt = function(client) return client:jid() end
}
Client.__index = Client
register_class_table_help(Client, "Client")

_H = {
	[[ Component interface ]]
}
local Component = {
	_with_prompt = function(component) return component:jid() end
}
Component.__index = Component
register_class_table_help(Component, "Component")


_H = {
	[[ Interface to communicate with a PubSub service ]]
}
local PubSub = {}
PubSub.__index = PubSub
register_class_table_help(PubSub, "PubSub")

_H = {
	[[ Interface to communicate with a PubSub node on a service ]]
}
local PubSubNode = {}
PubSubNode.__index = PubSubNode
register_class_table_help(PubSubNode, "PubSubNode")


--------------------------------------------------------------------------------
-- with
--------------------------------------------------------------------------------

local original_G

local function with (target, f)
	-- Dynamic scope
	if f then
		with(target)
		return call{f, finally = function() with() end}
	end

	-- No scope
	if target then
		if not original_G then
			original_G = copy(_G)
			setmetatable(original_G, getmetatable(_G))
			clear(_G)
		end

		setmetatable(_G, { 
			__index = function(_, key)
				local value = target[key]
				if value then
					if type(value) == 'function' then
						-- Add 'self' argument to all functions
						return function(...) return value(target, ...) end
					else
						return value
					end
				else
					return original_G[key]
				end
			end,
			__newindex = original_G,
			_completions = function ()
				local result = {}
				if type(target) == "table" then
					insert_all(result, keys(target))
				end
				local mt = getmetatable(target)
				if mt and type(mt.__index) == 'table' then
					insert_all(result, keys(mt.__index))
				end
				insert_all(result, keys(original_G))
				return result
			end
		})

		-- Set prompt
		local prompt = nil
		
		-- Try '_with_prompt' in metatable
		local target_metatable = getmetatable(target)
		if target_metatable then
			if type(target_metatable._with_prompt) == "function" then
				prompt = target_metatable._with_prompt(target)
			else
				prompt = target_metatable._with_prompt
			end
		end

		if not prompt then
			-- Use tostring()
			local target_string = tostring(target)
			if string.len(target_string) > 25 then
				prompt = string.sub(target_string, 0, 22) .. "..."
			else
				prompt = target_string
			end
		end
		rawset(_G, "_PROMPT", prompt .. '> ')
	else
		-- Reset _G
		clear(_G)
		for key, value in pairs(original_G) do
			_G[key] = value
		end
		setmetatable(_G, original_G)
	end
end

--------------------------------------------------------------------------------
-- Client
--------------------------------------------------------------------------------

extra_help = {
	["Client.get_next_event"] = {
		[[ Returns the next event. ]],
		parameters = { "self" },
		options = {
			type = "The type of event to return (`message`, `presence`, `pubsub`). When omitted, all event types are returned.",
			timeout = "The amount of time to wait for events.",
			["if"] = "A function to filter events. When this function, called with the event as a parameter, returns true, the event will be returned"
		}
	},
	["Client.get"] = {
		[[ Sends a `get` query. ]],
		parameters = { "self" },
		options = {
			to = "The JID of the target to send the query to",
			query = "The query to send",
			timeout = "The amount of time to wait for the query to finish",
		}
	},
	["Client.set"] = {
		[[ Sends a `set` query. ]],
		parameters = { "self" },
		options = {
			to = "The JID of the target to send the query to",
			query = "The query to send.",
			timeout = "The amount of time to wait for the query to finish.",
		}
	},
	["Client.async_connect"] = {
		[[ 
			Connect to the server asynchronously.
			
			This method immediately returns.
		]],
		parameters = { "self" },
		options = {
			host = "The host to connect to. When omitted, is determined by resolving the client JID.",
			port = "The port to connect to. When omitted, is determined by resolving the client JID."
		}
	}
}

_H = {
	[[
		Connect to the server.

		This method blocks until the connection has been established.
	]],
	parameters = { "self" },
	options = extra_help["Client.async_connect"].options
}
function Client:connect (...)
	local options = parse_options({}, ...)
	local f = options.f
	self:async_connect(options)
	self:wait_connected()
	if f then
		return call {function() return f(self) end, finally = function() self:disconnect() end}
	end
	return true
end
register_help(Client.connect)


_H = {
	[[
		Returns an iterator over all events.

		This function blocks until `timeout` is reached (or blocks forever if it is omitted).
	]],
	parameters = { "self" },
	options = extra_help["Client.get_next_event"].options
}
function Client:events (options)
	local function client_events_iterator(s)
		return s['client']:get_next_event(s['options'])
	end
	return client_events_iterator, {client = self, options = options}
end
register_help(Client.events)


_H = {
	[[
		Calls `f` for each event.
	]],
	parameters = { "self" },
	options = merge_tables(get_help(Client.events).options, {
		f = "The functor to call with each event. Required."
	})
}
function Client:for_each_event (...)
	local options = parse_options({}, ...)
	if not type(options.f) == 'function' then error('Expected function') end
	for event in self:events(options) do
		local result = options.f(event)
		if result then
			return result
		end
	end
end
register_help(Client.for_each_event)

for method, event_type in pairs({message = 'message', presence = 'presence', pubsub_event = 'pubsub'}) do
	_H = {
		"Call `f` for all events of type `" .. event_type .. "`.",
		parameters = { "self" },
		options = remove_help_parameters("type", get_help(Client.for_each_event).options)
	}
	Client['for_each_' .. method] = function (client, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return client:for_each_event (options)
	end
	register_help(Client['for_each_' .. method])

	_H = {
		"Get the next event of type `" .. event_type .. "`.",
		parameters = { "self" },
		options = remove_help_parameters("type", extra_help["Client.get_next_event"].options)
	}
	Client['get_next_' .. method] = function (client, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return client:get_next_event(options)
	end
	register_help(Client['get_next_' .. method])
end

for method, event_type in pairs({messages = 'message', pubsub_events = 'pubsub'}) do
	_H = {
		"Returns an iterator over all events of type `" .. event_type .. "`.",
		parameters = { "self" },
		options = remove_help_parameters("type", get_help(Client.for_each_event).options)
	}
	Client[method] = function (client, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return client:events (options)
	end
	register_help(Client[method])
end

_H = {
	[[ 
		Process all pending events
	]],
	parameters = { "self" }
}
function Client:process_events ()
	for event in self:events{timeout=0} do end
end
register_help(Client.process_events)


--
-- Register get_* and set_* convenience methods for some type of queries
--
-- Example usages:
--	client:get_software_version{to = 'alice@wonderland.lit'}
--	client:set_command{to = 'alice@wonderland.lit', command = { type = 'execute', node = 'uptime' }}
--
local get_set_shortcuts = {
	get = {'software_version', 'disco_items', 'disco_info', 'xml', 'dom', 'vcard', 'mam'},
	set = {'command', 'vcard', 'mam'}
}
for query_action, query_types in pairs(get_set_shortcuts) do
	for _, query_type in ipairs(query_types) do
		_H = {
			"Sends a `" .. query_action .. "` query of type `" .. query_type .. "`.\n" ..
			"Apart from the options below, all top level elements of `" .. query_type .. "` can be passed.",
			parameters = { "self" },
			options = remove_help_parameters({"query", "type"}, extra_help["Client.get"].options),
		}
		local method = query_action .. '_' .. query_type
		Client[method] = function (client, options)
			options = options or {}
			if type(options) ~= 'table' then error('Invalid options: ' .. options) end 
			options['query'] = merge_tables({_type = query_type}, options[query_type] or {})
			return client[query_action](client, options)
		end
		register_help(Client[method])
	end
end

_H = {
	[[ Returns a @{PubSub} object for communicating with the PubSub service at `jid`. ]],
	parameters = { 
		"self", 
		{"jid", "The JID of the PubSub service"}
	}
}
function Client:pubsub (jid)
	local result = { client = self, jid = jid }
	setmetatable(result, PubSub)
	return result
end
register_help(Client.pubsub)


--------------------------------------------------------------------------------
-- Component
--------------------------------------------------------------------------------

component_extra_help = {
	["Component.get_next_event"] = {
		[[ Returns the next event. ]],
		parameters = { "self" },
		options = {
			type = "The type of event to return (`message`, `presence`). When omitted, all event types are returned.",
			timeout = "The amount of time to wait for events.",
			["if"] = "A function to filter events. When this function, called with the event as a parameter, returns true, the event will be returned"
		}
	},
	["Component.get"] = {
		[[ Sends a `get` query. ]],
		parameters = { "self" },
		options = {
			to = "The JID of the target to send the query to",
			query = "The query to send",
			timeout = "The amount of time to wait for the query to finish",
		}
	},
	["Component.set"] = {
		[[ Sends a `set` query. ]],
		parameters = { "self" },
		options = {
			to = "The JID of the target to send the query to",
			query = "The query to send.",
			timeout = "The amount of time to wait for the query to finish.",
		}
	},
	["Component.async_connect"] = {
		[[ 
			Connect to the server asynchronously.
			
			This method immediately returns.
		]],
		parameters = { "self" },
		options = {
			host = "The host to connect to.",
			port = "The port to connect to."
		}
	}
}

_H = {
	[[
		Connect to the server.

		This method blocks until the connection has been established.
	]],
	parameters = { "self" },
	options = component_extra_help["Component.async_connect"].options
}
function Component:connect (...)
	local options = parse_options({}, ...)
	local f = options.f
	self:async_connect(options)
	self:wait_connected()
	if f then
		return call {function() return f(self) end, finally = function() self:disconnect() end}
	end
	return true
end
register_help(Component.connect)


_H = {
	[[
		Returns an iterator over all events.

		This function blocks until `timeout` is reached (or blocks forever if it is omitted).
	]],
	parameters = { "self" },
	options = component_extra_help["Component.get_next_event"].options
}
function Component:events (options)
	local function component_events_iterator(s)
		return s['component']:get_next_event(s['options'])
	end
	return component_events_iterator, {component = self, options = options}
end
register_help(Component.events)


_H = {
	[[
		Calls `f` for each event.
	]],
	parameters = { "self" },
	options = merge_tables(get_help(Component.events).options, {
		f = "The functor to call with each event. Required."
	})
}
function Component:for_each_event (...)
	local options = parse_options({}, ...)
	if not type(options.f) == 'function' then error('Expected function') end
	for event in self:events(options) do
		local result = options.f(event)
		if result then
			return result
		end
	end
end
register_help(Component.for_each_event)

for method, event_type in pairs({message = 'message', presence = 'presence'}) do
	_H = {
		"Call `f` for all events of type `" .. event_type .. "`.",
		parameters = { "self" },
		options = remove_help_parameters("type", get_help(Component.for_each_event).options)
	}
	Component['for_each_' .. method] = function (component, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return component:for_each_event (options)
	end
	register_help(Component['for_each_' .. method])

	_H = {
		"Get the next event of type `" .. event_type .. "`.",
		parameters = { "self" },
		options = remove_help_parameters("type", component_extra_help["Component.get_next_event"].options)
	}
	Component['get_next_' .. method] = function (component, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return component:get_next_event(options)
	end
	register_help(Component['get_next_' .. method])
end

for method, event_type in pairs({messages = 'message'}) do
	_H = {
		"Returns an iterator over all events of type `" .. event_type .. "`.",
		parameters = { "self" },
		options = remove_help_parameters("type", get_help(Component.for_each_event).options)
	}
	Component[method] = function (component, ...)
		local options = parse_options({}, ...)
		options['type'] = event_type
		return component:events (options)
	end
	register_help(Component[method])
end

_H = {
	[[ 
		Process all pending events
	]],
	parameters = { "self" }
}
function Component:process_events ()
	for event in self:events{timeout=0} do end
end
register_help(Component.process_events)


--
-- Register get_* and set_* convenience methods for some type of queries
--
-- Example usages:
--	component:get_software_version{to = 'alice@wonderland.lit'}
--	component:set_command{to = 'alice@wonderland.lit', command = { type = 'execute', node = 'uptime' }}
--
local get_set_shortcuts = {
	get = {'software_version', 'disco_items', 'xml', 'dom', 'vcard'},
	set = {'command', 'vcard'}
}
for query_action, query_types in pairs(get_set_shortcuts) do
	for _, query_type in ipairs(query_types) do
		_H = {
			"Sends a `" .. query_action .. "` query of type `" .. query_type .. "`.\n" ..
			"Apart from the options below, all top level elements of `" .. query_type .. "` can be passed.",
			parameters = { "self" },
			options = remove_help_parameters({"query", "type"}, component_extra_help["Component.get"].options),
		}
		local method = query_action .. '_' .. query_type
		Component[method] = function (component, options)
			options = options or {}
			if type(options) ~= 'table' then error('Invalid options: ' .. options) end 
			options['query'] = merge_tables({_type = query_type}, options[query_type] or {})
			return component[query_action](component, options)
		end
		register_help(Component[method])
	end
end

--------------------------------------------------------------------------------
-- PubSub
--------------------------------------------------------------------------------

local function process_pubsub_event (event)
	if event._type == 'pubsub_event_items' then
		-- Add 'item' shortcut to payload of first item
		event.item = event.items and event.items[1] and 
			event.items[1].data and event.items[1].data[1]
	end
end

function PubSub:list_nodes (options)
	return self.client:get_disco_items(merge_tables({to = self.jid}, options))
end

function PubSub:node (node)
	local result = { client = self.client, jid = self.jid, node = node }
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

local function pubsub_node_configuration_to_form(configuration)
	return create_form{configuration, form_type="http://jabber.org/protocol/pubsub#node_config", type="submit"}
end

function PubSubNode:list_items (options)
	return self.client:get_disco_items(merge_tables({to = self.jid, disco_items = { node = self.node }}, options))
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

function PubSubNode:get_items (...)
	local options = parse_options({}, ...)
	local items = options.items or {}
	if options.maximum_items then
		items = merge_tables({maximum_items = options.maximum_items}, items)
	end
	items = merge_tables({_type = 'pubsub_items', node = self.node}, items)
	return self.client:query_pubsub(merge_tables({ 
		type = 'get', to = self.jid, query = items}, options))
end

function PubSubNode:get_item (...)
	local options = parse_options({}, ...)
	if not type(options.id) == 'string' then error('Expected ID') end
	return self:get_items{items = {{id = options.id}}}
end

function PubSubNode:create (options)
	options = options or {}
	local configure
	if options['configuration'] then
		configure = { data = pubsub_node_configuration_to_form(options['configuration']) }
	end
	return self.client:query_pubsub(merge_tables(
		{ type = 'set', to = self.jid, query = { 
				_type = 'pubsub_create', node = self.node, configure = configure }
		}, options))
end

function PubSubNode:delete (options)
	options = options or {}
	local redirect
	if options['redirect'] then
		redirect = {uri = options['redirect']}
	end
	return self.client:query_pubsub(merge_tables({ type = 'set', to = self.jid, query = { 
			_type = 'pubsub_owner_delete', node = self.node, redirect = redirect 
		}}, options))
end

function PubSubNode:set_configuration(options)
	options = options or {}
	local configuration = pubsub_node_configuration_to_form(options['configuration'])
	return self.client:query_pubsub(merge_tables(
		{ type = 'set', to = self.jid, query = { 
				_type = 'pubsub_owner_configure', node = self.node, data = configuration }
		}, options))
end

function PubSubNode:set_owner_affiliations(...)
	local options = parse_options({}, ...)
	return self.client:query_pubsub(merge_tables({ 
		type = 'set', to = self.jid, query = merge_tables({
				_type = 'pubsub_owner_affiliations', node = self.node, 
		}, options.affiliations)}, options))
end


function PubSubNode:subscribe(...)
	local options = parse_options({}, ...)
	local jid = options.jid or sluift.jid.to_bare(self.client:jid())
	return self.client:query_pubsub(merge_tables(
		{ type = 'set', to = self.jid, query = { 
				_type = 'pubsub_subscribe', node = self.node, jid = jid }
		}, options))
end

function PubSubNode:unsubscribe(options)
	options = options or {}
	return self.client:query_pubsub(merge_tables(
		{ type = 'set', to = self.jid, query = { 
				_type = 'pubsub_unsubscribe', node = self.node, jid = options['jid'], 
				subscription_id = 'subscription_id'}
		}, options))
end

function PubSubNode:get_subscription_options (options)
	return self.client:query_pubsub(merge_tables(
		{ type = 'get', to = self.jid, query = { 
				_type = 'pubsub_options', node = self.node, jid = options['jid'] }
		}, options))
end

function PubSubNode:publish(...)
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
	return self.client:query_pubsub(merge_tables(
		{ type = 'set', to = self.jid, query = { 
				_type = 'pubsub_publish', node = self.node, items = items }
		}, options))
end

function PubSubNode:retract(...)
	local options = parse_options({}, ...)
	local items = options.items
	if options.id then
		items = {{id = options.id}}
	end
	return self.client:query_pubsub(merge_tables(
		{ type = 'set', to = self.jid, query = { 
				_type = 'pubsub_retract', node = self.node, items = items, notify = options['notify']
		}}, options))
end

function PubSubNode:purge(...)
	local options = parse_options({}, ...)
	return self.client:query_pubsub(merge_tables(
		{ type = 'set', to = self.jid, query = { 
				_type = 'pubsub_owner_purge', node = self.node
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

_H = nil

extra_help['sluift'] = {
	[[
		This module provides methods for XMPP communication.

		The main entry point of this module is the `new_client` method, which creates a
		new client for communicating with an XMPP server.
	]],
	classes = help_classes
}

return {
	Client = Client,
	Component = Component,
	register_help = register_help,
	register_class_help = register_class_help,
	register_table_tostring = register_table_tostring,
	register_table_equals = register_table_equals,
	register_get_by_type_index = register_get_by_type_index,
	process_pubsub_event = process_pubsub_event,
	tprint = tprint,
	read_file = read_file,
	disco = disco,
	get_help = get_help,
	help = help,
	extra_help = extra_help,
	component_extra_help = component_extra_help,
	copy = copy,
	with = with,
	create_form = create_form
}
