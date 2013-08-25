/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/Override.h>
#include <Swiften/Base/API.h>
#include <Sluift/Lua/FunctionRegistry.h>
#include <lua.hpp>
#include <string>

namespace Swift {
	namespace Lua {
		class FunctionRegistration {
			public:
				FunctionRegistration(const std::string& name, lua_CFunction function, const std::string& type);
				~FunctionRegistration();
		};
	}
}
#define SLUIFT_LUA_FUNCTION(TYPE, NAME) \
	static int TYPE##_##NAME(lua_State* L); \
	static int TYPE##_##NAME##_wrapper(lua_State* L); \
	static ::Swift::Lua::FunctionRegistration TYPE##_##NAME##_registration( #NAME , TYPE##_##NAME##_wrapper, #TYPE); \
	static int TYPE##_##NAME##_wrapper(lua_State* L) { \
		try { \
			return TYPE ## _ ## NAME (L); \
		} \
		catch (const std::exception& e) { \
			return luaL_error(L, e.what()); \
		} \
	} \
	static int TYPE ## _ ## NAME (lua_State* L)
