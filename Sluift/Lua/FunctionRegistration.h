/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <lua.hpp>

#include <Swiften/Base/API.h>

#include <Sluift/Lua/FunctionRegistry.h>

namespace Swift {
    namespace Lua {
        class FunctionRegistration {
            public:
                FunctionRegistration(
                        const std::string& name, lua_CFunction function, const std::string& type,
                        const std::string& helpDescription, const std::string& helpParameters, const std::string& helpOptions);
                ~FunctionRegistration();
        };
    }
}


#define SLUIFT_LUA_FUNCTION_WITH_HELP(TYPE, NAME, HELP_DESCRIPTION, HELP_PARAMETERS, HELP_OPTIONS) \
    static int TYPE##_##NAME(lua_State* L); \
    static int TYPE##_##NAME##_wrapper(lua_State* L); \
    static ::Swift::Lua::FunctionRegistration TYPE##_##NAME##_registration( #NAME , TYPE##_##NAME##_wrapper, #TYPE, HELP_DESCRIPTION, HELP_PARAMETERS, HELP_OPTIONS); \
    static int TYPE##_##NAME##_wrapper(lua_State* L) { \
        try { \
            return TYPE ## _ ## NAME (L); \
        } \
        catch (const std::exception& e) { \
            return luaL_error(L, e.what()); \
        } \
    } \
    static int TYPE ## _ ## NAME (lua_State* L)

#define SLUIFT_LUA_FUNCTION(TYPE, NAME) \
    SLUIFT_LUA_FUNCTION_WITH_HELP(TYPE, NAME, "", "", "")
