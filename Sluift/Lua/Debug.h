/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <lua.hpp>
#include <iostream>

namespace Swift {
	namespace Lua {
		inline void dumpStack(lua_State *L) {
			for (int i = 1; i <= lua_gettop(L); i++) {
				int type = lua_type(L, i);
				std::cout << i << ": [" << lua_typename(L, type) << "] ";
				switch (type) {
					case LUA_TSTRING: std::cout << lua_tostring(L, i); break;
					case LUA_TNUMBER: std::cout << lua_tonumber(L, i); break;
					case LUA_TBOOLEAN: std::cout << lua_toboolean(L, i); break;
					default: break;
				}
				std::cout << std::endl;
			}
		}
	}
}
