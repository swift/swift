/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/LuaUtils.h>

#include <lua.hpp>

#include <boost/scope_exit.hpp>
#include <Sluift/Lua/Exception.h>
#include <iostream>
#include <cassert>
#include <sstream>
#include <boost/numeric/conversion/cast.hpp>
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
