/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <lua.hpp>

#include <Sluift/Lua/Check.h>
#include <Sluift/Lua/LuaUtils.h>
#include <Sluift/LuaElementConvertor.h>

namespace Swift {
    template<typename T>
    class GenericLuaElementConvertor : public LuaElementConvertor {
        public:
            GenericLuaElementConvertor(const std::string& type) : type(type) {
            }

            virtual ~GenericLuaElementConvertor() override {}

            virtual std::shared_ptr<Element> convertFromLua(lua_State* L, int index, const std::string& payloadType) override {
                if (payloadType == type) {
                    Lua::checkType(L, index, LUA_TTABLE);
                    lua_pushvalue(L, index);
                    std::shared_ptr<Element> result = doConvertFromLua(L);
                    lua_pop(L, 1);
                    return result;
                }
                return std::shared_ptr<Element>();
            }

            virtual boost::optional<std::string> convertToLua(
                    lua_State* L, std::shared_ptr<Element> payload) override {
                if (std::shared_ptr<T> actualPayload = std::dynamic_pointer_cast<T>(payload)) {
                    doConvertToLua(L, actualPayload);
                    assert(lua_type(L, -1) == LUA_TTABLE);
                    return type;
                }
                return NO_RESULT;
            }

        protected:
            virtual std::shared_ptr<T> doConvertFromLua(lua_State*) = 0;
            virtual void doConvertToLua(lua_State*, std::shared_ptr<T>) = 0;

        private:
            std::string type;
    };
}
