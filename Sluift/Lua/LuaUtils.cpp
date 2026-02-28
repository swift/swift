/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/LuaUtils.h>

#include <cassert>
#include <sstream>

#include <boost/algorithm/string/trim.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/scope_exit.hpp>

#include <lua.hpp>

#include <Sluift/Lua/Exception.h>
#include <Sluift/globals.h>

using namespace Swift::Lua;

static const std::string INDENT = "  ";

void Swift::Lua::registerTableToString(lua_State* L, int index) {
    index = Lua::absoluteOffset(L, index);
    lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
    lua_getfield(L, -1, "register_table_tostring");
    lua_pushvalue(L, index);
    if (lua_pcall(L, 1, 0, 0) != 0) {
        throw Lua::Exception(lua_tostring(L, -1));
    }
    lua_pop(L, 1);
}

void Swift::Lua::registerTableEquals(lua_State* L, int index) {
    index = Lua::absoluteOffset(L, index);
    lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
    lua_getfield(L, -1, "register_table_equals");
    lua_pushvalue(L, index);
    if (lua_pcall(L, 1, 0, 0) != 0) {
        throw Lua::Exception(lua_tostring(L, -1));
    }
    lua_pop(L, 1);
}

void Swift::Lua::registerGetByTypeIndex(lua_State* L, int index) {
    index = Lua::absoluteOffset(L, index);
    lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
    lua_getfield(L, -1, "register_get_by_type_index");
    lua_pushvalue(L, index);
    if (lua_pcall(L, 1, 0, 0) != 0) {
        throw Lua::Exception(lua_tostring(L, -1));
    }
    lua_pop(L, 1);
}

boost::optional<std::string> Swift::Lua::getStringField(lua_State* L, int index, const std::string& field) {
    lua_getfield(L, index, field.c_str());
    // Seems to generate warnings with some versions of CLang that i can't turn off.
    // Leaving the more elegant code here, hoping we can re-enable it later (newer boost? c++11?).
    // The same applies to the other get*Field functions.
    //BOOST_SCOPE_EXIT(&L) { lua_pop(L,1); } BOOST_SCOPE_EXIT_END
    //return lua_isstring(L, -1) ? std::string(lua_tostring(L, -1)) : boost::optional<std::string>();

    boost::optional<std::string> result;
    if (lua_isstring(L, -1)) {
        result = std::string(lua_tostring(L, -1));
    }
    lua_pop(L, 1);
    return result;
}

boost::optional<bool> Swift::Lua::getBooleanField(lua_State* L, int index, const std::string& field) {
    lua_getfield(L, index, field.c_str());
    boost::optional<bool> result;
    if (lua_isboolean(L, -1)) {
        result = lua_toboolean(L, -1);
    }
    lua_pop(L, 1);
    return result;
}

boost::optional<int> Swift::Lua::getIntField(lua_State* L, int index, const std::string& field) {
    lua_getfield(L, index, field.c_str());
    boost::optional<int> result;
    if (lua_isnumber(L, -1)) {
        result = boost::numeric_cast<int>(lua_tonumber(L, -1));
    }
    lua_pop(L, 1);
    return result;
}

void Swift::Lua::registerHelp(lua_State* L, int index, const std::string& description, const std::string& parameters, const std::string& options) {
    index = Lua::absoluteOffset(L, index);
    lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
    lua_getfield(L, -1, "register_help");
    lua_pushvalue(L, index);

    lua_newtable(L);
    lua_pushstring(L, description.c_str());
    lua_rawseti(L, -2, 1);

    if (!parameters.empty()) {
        std::istringstream s(parameters);
        lua_newtable(L);
        int i = 1;
        for (std::string line; std::getline(s, line); ) {
            std::string trimmedLine = boost::trim_copy(line);
            if (trimmedLine.empty()) {
                continue;
            }
            size_t splitIndex = trimmedLine.find_first_of(" \t");
            std::string key;
            std::string value;
            if (splitIndex == std::string::npos) {
                key = trimmedLine;
            }
            else {
                key = trimmedLine.substr(0, splitIndex);
                value = boost::trim_copy(trimmedLine.substr(splitIndex+1));
            }
            lua_createtable(L, 2, 0);
            lua_pushstring(L, key.c_str());
            lua_rawseti(L, -2, 1);
            lua_pushstring(L, value.c_str());
            lua_rawseti(L, -2, 2);

            lua_rawseti(L, -2, i++);
        }
        lua_setfield(L, -2, "parameters");
    }
    if (!options.empty()) {
        std::istringstream s(options);
        lua_newtable(L);
        for (std::string line; std::getline(s, line); ) {
            std::string trimmedLine = boost::trim_copy(line);
            if (trimmedLine.empty()) {
                continue;
            }
            size_t splitIndex = trimmedLine.find_first_of(" \t");
            std::string key;
            std::string value;
            if (splitIndex == std::string::npos) {
                key = trimmedLine;
            }
            else {
                key = trimmedLine.substr(0, splitIndex);
                value = boost::trim_copy(trimmedLine.substr(splitIndex+1));
            }
            lua_pushstring(L, value.c_str());
            lua_setfield(L, -2, key.c_str());
        }
        lua_setfield(L, -2, "options");
    }

    if (lua_pcall(L, 2, 0, 0) != 0) {
        throw Lua::Exception(lua_tostring(L, -1));
    }
    lua_pop(L, 1);
}

void Swift::Lua::registerClassHelp(lua_State* L, const std::string& name, const std::string& description) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
    lua_getfield(L, -1, "register_class_help");
    lua_pushstring(L, name.c_str());

    lua_newtable(L);
    lua_pushstring(L, description.c_str());
    lua_rawseti(L, -2, 1);

    if (lua_pcall(L, 2, 0, 0) != 0) {
        throw Lua::Exception(lua_tostring(L, -1));
    }
    lua_pop(L, 1);
}

void Swift::Lua::registerExtraHelp(lua_State* L, int index, const std::string& name) {
    index = Lua::absoluteOffset(L, index);
    lua_rawgeti(L, LUA_REGISTRYINDEX, Sluift::globals.coreLibIndex);
    lua_getfield(L, -1, "extra_help");
    lua_getfield(L, -1, name.c_str());
    if (!lua_isnil(L, -1)) {
        lua_getfield(L, -3, "register_help");
        lua_pushvalue(L, index);
        lua_pushvalue(L, -3);
        if (lua_pcall(L, 2, 0, 0) != 0) {
            throw Lua::Exception(lua_tostring(L, -1));
        }
    }
    lua_pop(L, 3);
}

void Swift::Lua::pushStringArray(lua_State* L, const std::vector<std::string>& strings) {
    lua_createtable(L, boost::numeric_cast<int>(strings.size()), 0);
    for (size_t i = 0; i < strings.size(); ++i) {
        lua_pushstring(L, strings[i].c_str());
        lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
    }
}
