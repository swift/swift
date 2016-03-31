/*
 * Copyright (c) 2013-2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/ByteArray.h>

struct lua_State;

namespace Swift {
    namespace Lua {
        void checkType(lua_State* L, int arg, int type);
        int checkIntNumber(lua_State* L, int arg);
        std::string checkString(lua_State* L, int arg);
        ByteArray checkByteArray(lua_State* L, int arg);

        void* checkUserDataRaw(lua_State* L, int arg);

        template<typename T>
        T** checkUserData(lua_State* L, int arg) {
            return reinterpret_cast<T**>(checkUserDataRaw(L, arg));
        }
    }
}
