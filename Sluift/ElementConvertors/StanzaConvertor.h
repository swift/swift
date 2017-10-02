/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/numeric/conversion/cast.hpp>

#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/Presence.h>

#include <Sluift/GenericLuaElementConvertor.h>
#include <Sluift/Lua/Exception.h>
#include <Sluift/LuaElementConvertors.h>

namespace Swift {
    template <typename T> class StanzaConvertor : public GenericLuaElementConvertor<T> {
        public:
            StanzaConvertor(const std::string& tag)
            : GenericLuaElementConvertor<T>(tag) {
            }

            virtual ~StanzaConvertor() {
            }

            std::shared_ptr<T> getStanza(lua_State* L, LuaElementConvertors* convertors) {
                std::shared_ptr<T> result = std::make_shared<T>();
                lua_getfield(L, -1, "id");
                if (lua_isstring(L, -1)) {
                    result->setID(lua_tostring(L, -1));
                }
                lua_pop(L, 1);
                lua_getfield(L, -1, "from");
                if (lua_isstring(L, -1)) {
                    result->setFrom(lua_tostring(L, -1));
                }
                lua_pop(L, 1);
                lua_getfield(L, -1, "to");
                if (lua_isstring(L, -1)) {
                    result->setTo(lua_tostring(L, -1));
                }
                lua_pop(L, 1);
                lua_getfield(L, -1, "payloads");
                if (lua_type(L, -1) == LUA_TTABLE) {
                    for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
                        lua_pushnumber(L, i + 1);
                        lua_gettable(L, -2);
                        if (!lua_isnil(L, -1)) {
                            std::shared_ptr<Payload> payload = std::dynamic_pointer_cast<Payload>(convertors->convertFromLua(L, -1));
                            if (!!payload) {
                                result->addPayload(payload);
                            }
                        }
                        lua_pop(L, 1);
                    }
                }
                lua_pop(L, 1);
                return result;
            }

            void pushStanza(lua_State* L, const std::shared_ptr<T> stanza, LuaElementConvertors* convertors) {
                lua_createtable(L, 0, 0);
                lua_pushstring(L, stanza->getID().c_str());
                lua_setfield(L, -2, "id");
                lua_pushstring(L, stanza->getFrom().toString().c_str());
                lua_setfield(L, -2, "from");
                lua_pushstring(L, stanza->getTo().toString().c_str());
                lua_setfield(L, -2, "to");
                if (!stanza->getPayloads().empty()) {
                    lua_createtable(L, boost::numeric_cast<int>(stanza->getPayloads().size()), 0);
                    {
                        int i = 0;
                        for (const auto& item : stanza->getPayloads()) {
                            if (convertors->convertToLua(L, item) > 0) {
                                lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
                                ++i;
                            }
                        }
                    }
                    lua_setfield(L, -2, "payloads");
                }
            }
    };
}
