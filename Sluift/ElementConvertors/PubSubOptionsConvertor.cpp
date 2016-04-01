/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOptionsConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubOptionsConvertor::PubSubOptionsConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubOptions>("pubsub_options"),
        convertors(convertors) {
}

PubSubOptionsConvertor::~PubSubOptionsConvertor() {
}

std::shared_ptr<PubSubOptions> PubSubOptionsConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubOptions> result = std::make_shared<PubSubOptions>();
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
    lua_getfield(L, -1, "data");
    if (!lua_isnil(L, -1)) {
        if (std::shared_ptr<Form> payload = std::dynamic_pointer_cast<Form>(convertors->convertFromLuaUntyped(L, -1, "form"))) {
            result->setData(payload);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "subscription_id");
    if (lua_isstring(L, -1)) {
        result->setSubscriptionID(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubOptionsConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubOptions> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getNode().c_str());
    lua_setfield(L, -2, "node");
    lua_pushstring(L, payload->getJID().toString().c_str());
    lua_setfield(L, -2, "jid");
    if (convertors->convertToLuaUntyped(L, payload->getData()) > 0) {
        lua_setfield(L, -2, "data");
    }
    if (payload->getSubscriptionID()) {
        lua_pushstring(L, (*payload->getSubscriptionID()).c_str());
        lua_setfield(L, -2, "subscription_id");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubOptionsConvertor::getDocumentation() const {
    return Documentation(
        "PubSubOptions",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
        "- `jid`: jid (string)\n"
        "- `data`: @{Form}\n"
        "- `subscription_id`: string (Optional)\n"
    );
}
