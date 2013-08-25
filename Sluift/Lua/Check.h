/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

struct lua_State;

namespace Swift {
	namespace Lua {
		void checkType(lua_State* L, int arg, int type);
		int checkIntNumber(lua_State* L, int arg);
		std::string checkString(lua_State* L, int arg);

		void* checkUserDataRaw(lua_State* L, int arg, const char* tableName);

		template<typename T>
		T** checkUserData(lua_State* L, int arg, const char* tableName) {
			return reinterpret_cast<T**>(checkUserDataRaw(L, arg, tableName));
		}
	}
}
