--[[
	Copyright (c) 2013 Isode Limited.
	All rights reserved.
	See the COPYING file for more information.
--]]

require "sluift"

local function get_anchor(...)
	return table.concat({...}, "-")
end

local function convert_links(text)
	result = text:gsub("(@{(%w*)})", "[`%2`](#%2)")
	return result
end

local function add_help(help, document, class, level)
	-- Collect help of children
	local methods = {}
	for _, method in pairs(help.methods or {}) do
		local description
		local method_help = sluift.get_help(method.ref)
		if method_help and method_help.description then
			description = method_help.synopsis
		end
		methods[#methods+1] = { name = method.name, description = description, ref = method.ref }
	end
	local fields = sluift.copy(help.fields or {})

	table.sort(methods, function (a, b) return (a.name or "") < (b.name or "") end)
	table.sort(fields, function (a, b) return (a.name or "") < (b.name or "") end)

	local classes = {}
	for _, class in pairs(help.classes or {}) do
		classes[#classes+1] = { name = class, description = sluift.get_help(class).synopsis }
	end

	table.insert(document, convert_links(help.description or ""))
	for _, p in ipairs({
			{"Parameters", help.parameters}, {"Options", help.options}, {"Fields", fields}, {"Methods", methods}}) do
		if p[2] and next(p[2]) ~= nil then
			table.insert(document, "\n\n" .. level .. " " .. p[1] .. "\n")
			for _, parameter in ipairs(p[2]) do
				parameter_name = "`" .. parameter.name .. "`"
				if p[1] == "Methods" then
					parameter_name = "[" .. parameter_name .. "](#" .. get_anchor(class, parameter.name) .. ")"
				end
				if parameter.description then
					table.insert(document, "- " .. parameter_name .. ": " .. convert_links(parameter.description))
				else
					table.insert(document, "- " .. parameter_name)
				end
			end
			if p[1] == "Methods" then
				for _, method in ipairs(p[2]) do
					table.insert(document, "\n#### <a name=\"" .. get_anchor(class, method.name) .. "\"></a> `" .. method.name .. "`\n")
					if method.ref then
						add_help(sluift.get_help(method.ref) or {}, document, class, level .. "#")
					end
				end
			end
		end
	end
end

local document = {}

table.insert(document, [[
# Sluift

This document describes the API of the `sluift` module.

The entry points of Sluift are in the `sluift` module, described below.

## `sluift` module
]])

help = sluift.get_help(sluift)
add_help(help, document, "sluift", "###")
for _, class in pairs(help.classes) do
	class_help = sluift.get_help(class)
	if class_help then
		table.insert(document, "\n## <a name=\"" .. class .. "\"></a> `" .. class .. "` class\n")
		add_help(class_help, document, class, "###")
	end
end

document = table.concat(document, "\n") .. "\n"
print(document)
