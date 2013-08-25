/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#pragma once

#include <lua.hpp>
#include <boost/optional.hpp>

struct lua_State;

namespace Swift {
	namespace Lua {
		/**
		 * Can be used as __tostring metamethod on a table.
		 */
		int convertTableToString(lua_State* L);

		void registerTableToString(lua_State* L, int index);
		void registerGetByTypeIndex(lua_State* L, int index);

		inline int absoluteOffset(lua_State* L, int index) {
			return index > 0 ? index : lua_gettop(L) + index + 1;
		}

		boost::optional<std::string> getStringField(lua_State* L, int index, const std::string&);
		boost::optional<bool> getBooleanField(lua_State* L, int index, const std::string&);
		boost::optional<int> getIntField(lua_State* L, int index, const std::string&);
	}
}
