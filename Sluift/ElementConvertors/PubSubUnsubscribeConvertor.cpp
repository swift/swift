/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubUnsubscribeConvertor.h>

#include <memory>

#include <lua.hpp>

using namespace Swift;

PubSubUnsubscribeConvertor::PubSubUnsubscribeConvertor() :
        GenericLuaElementConvertor<PubSubUnsubscribe>("pubsub_unsubscribe") {
}

PubSubUnsubscribeConvertor::~PubSubUnsubscribeConvertor() {
}

std::shared_ptr<PubSubUnsubscribe> PubSubUnsubscribeConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubUnsubscribe> result = std::make_shared<PubSubUnsubscribe>();
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
    lua_getfield(L, -1, "subscription_id");
    if (lua_isstring(L, -1)) {
        result->setSubscriptionID(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubUnsubscribeConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubUnsubscribe> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getNode()) {
        lua_pushstring(L, (*payload->getNode()).c_str());
        lua_setfield(L, -2, "node");
    }
    lua_pushstring(L, payload->getJID().toString().c_str());
    lua_setfield(L, -2, "jid");
    if (payload->getSubscriptionID()) {
        lua_pushstring(L, (*payload->getSubscriptionID()).c_str());
        lua_setfield(L, -2, "subscription_id");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubUnsubscribeConvertor::getDocumentation() const {
    return Documentation(
        "PubSubUnsubscribe",
        "This table has the following fields:\n\n"
        "- `node`: string (Optional)\n"
        "- `jid`: jid (string)\n"
        "- `subscription_id`: string (Optional)\n"
    );
}
