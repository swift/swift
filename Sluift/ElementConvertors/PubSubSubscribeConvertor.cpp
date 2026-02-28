/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubSubscribeConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubSubscribeConvertor::PubSubSubscribeConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubSubscribe>("pubsub_subscribe"),
        convertors(convertors) {
}

PubSubSubscribeConvertor::~PubSubSubscribeConvertor() {
}

std::shared_ptr<PubSubSubscribe> PubSubSubscribeConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubSubscribe> result = std::make_shared<PubSubSubscribe>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "jid");
    if (lua_isstring(L, -1)) {
        result->setJID(JID(std::string(lua_tostring(L, -1))));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "options");
    if (!lua_isnil(L, -1)) {
        if (std::shared_ptr<PubSubOptions> payload = std::dynamic_pointer_cast<PubSubOptions>(convertors->convertFromLuaUntyped(L, -1, "pubsub_options"))) {
            result->setOptions(payload);
        }
    }
    lua_pop(L, 1);
    return result;
}

void PubSubSubscribeConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubSubscribe> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getNode()) {
        lua_pushstring(L, (*payload->getNode()).c_str());
        lua_setfield(L, -2, "node");
    }
    lua_pushstring(L, payload->getJID().toString().c_str());
    lua_setfield(L, -2, "jid");
    if (convertors->convertToLuaUntyped(L, payload->getOptions()) > 0) {
        lua_setfield(L, -2, "options");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubSubscribeConvertor::getDocumentation() const {
    return Documentation(
        "PubSubSubscribe",
        "This table has the following fields:\n\n"
        "- `node`: string (Optional)\n"
        "- `jid`: jid (string)\n"
        "- `options`: @{PubSubOptions}\n"
    );
}
