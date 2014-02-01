/*
 * Copyright (c) 2013-2014 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/Lua/Check.h>

#include <boost/numeric/conversion/cast.hpp>
#include <iostream>
#include <sstream>
#include <lua.hpp>

#include <Sluift/Lua/Exception.h>
#include <Swiften/Base/ByteArray.h>

using namespace Swift;


static std::string getArgTypeError(lua_State* L, int arg, int tag) {
	std::ostringstream s;
	s << "Arg " << arg << ": expected " << lua_typename(L, tag) << ", got " << luaL_typename(L, arg);
	return s.str();
}

void Lua::checkType(lua_State* L, int arg, int type) {
	if (lua_type(L, arg) !=  type) {
		throw Lua::Exception(getArgTypeError(L, arg, type));
	}
}

int Lua::checkIntNumber(lua_State* L, int arg) {
	if (!lua_isnumber(L, arg)) {
		throw Lua::Exception(getArgTypeError(L, arg, LUA_TNUMBER));
	}
	return boost::numeric_cast<int>(lua_tonumber(L, arg));
}

std::string Lua::checkString(lua_State* L, int arg) {
	const char *s = lua_tolstring(L, arg, NULL);
	if (!s) {
		throw Lua::Exception(getArgTypeError(L, arg, LUA_TSTRING));
	}
	return std::string(s);
}

ByteArray Lua::checkByteArray(lua_State* L, int arg) {
	size_t len;
	const char *s = lua_tolstring(L, arg, &len);
	if (!s) {
		throw Lua::Exception(getArgTypeError(L, arg, LUA_TSTRING));
	}
	return createByteArray(s, len);
}

void* Lua::checkUserDataRaw(lua_State* L, int arg) {
	void* userData = lua_touserdata(L, arg);
	if (!userData) {
		throw Lua::Exception(getArgTypeError(L, arg, LUA_TUSERDATA));
	}
	if (!lua_getmetatable(L, arg)) {
		throw Lua::Exception(getArgTypeError(L, arg, LUA_TUSERDATA));
	}
	lua_pop(L, 1);
	return userData;
}
