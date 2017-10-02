/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <lua.hpp>
#include <boost/optional.hpp>
#include <string>
#include <vector>

#if LUA_VERSION_NUM < 502
#define lua_pushglobaltable(L) lua_pushvalue(L, LUA_GLOBALSINDEX)
#endif

namespace Swift {
    namespace Lua {
        /**
         * Can be used as __tostring metamethod on a table.
         */
        int convertTableToString(lua_State* L);

        void registerTableToString(lua_State* L, int index);
        void registerTableEquals(lua_State* L, int index);
        void registerGetByTypeIndex(lua_State* L, int index);
        void registerHelp(lua_State* L, int index,
                const std::string& description, const std::string& parameters, const std::string& options);
        void registerClassHelp(lua_State* L, const std::string& name, const std::string& description);
        void registerExtraHelp(lua_State* L, int index, const std::string& name);

        inline int absoluteOffset(lua_State* L, int index) {
            return index > 0 ? index : lua_gettop(L) + index + 1;
        }

        boost::optional<std::string> getStringField(lua_State* L, int index, const std::string&);
        boost::optional<bool> getBooleanField(lua_State* L, int index, const std::string&);
        boost::optional<int> getIntField(lua_State* L, int index, const std::string&);

        void pushStringArray(lua_State* L, const std::vector<std::string>& strings);
    }
}
