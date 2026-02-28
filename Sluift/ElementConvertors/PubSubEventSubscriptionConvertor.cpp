/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventSubscriptionConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Swiften/Base/DateTime.h>

using namespace Swift;

PubSubEventSubscriptionConvertor::PubSubEventSubscriptionConvertor() :
        GenericLuaElementConvertor<PubSubEventSubscription>("pubsub_event_subscription") {
}

PubSubEventSubscriptionConvertor::~PubSubEventSubscriptionConvertor() {
}

std::shared_ptr<PubSubEventSubscription> PubSubEventSubscriptionConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEventSubscription> result = std::make_shared<PubSubEventSubscription>();
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
    lua_getfield(L, -1, "subscription");
    if (lua_isstring(L, -1)) {
        if (std::string(lua_tostring(L, -1)) == "none") {
            result->setSubscription(PubSubEventSubscription::None);
        }
        if (std::string(lua_tostring(L, -1)) == "pending") {
            result->setSubscription(PubSubEventSubscription::Pending);
        }
        if (std::string(lua_tostring(L, -1)) == "subscribed") {
            result->setSubscription(PubSubEventSubscription::Subscribed);
        }
        if (std::string(lua_tostring(L, -1)) == "unconfigured") {
            result->setSubscription(PubSubEventSubscription::Unconfigured);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "subscription_id");
    if (lua_isstring(L, -1)) {
        result->setSubscriptionID(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "expiry");
    if (lua_isstring(L, -1)) {
        result->setExpiry(stringToDateTime(std::string(lua_tostring(L, -1))));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubEventSubscriptionConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEventSubscription> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getNode().c_str());
    lua_setfield(L, -2, "node");
    lua_pushstring(L, payload->getJID().toString().c_str());
    lua_setfield(L, -2, "jid");
    switch (payload->getSubscription()) {
        case PubSubEventSubscription::None:
            lua_pushstring(L, "none");
            break;
        case PubSubEventSubscription::Pending:
            lua_pushstring(L, "pending");
            break;
        case PubSubEventSubscription::Subscribed:
            lua_pushstring(L, "subscribed");
            break;
        case PubSubEventSubscription::Unconfigured:
            lua_pushstring(L, "unconfigured");
            break;
    }
    lua_setfield(L, -2, "subscription");
    if (payload->getSubscriptionID()) {
        lua_pushstring(L, (*payload->getSubscriptionID()).c_str());
        lua_setfield(L, -2, "subscription_id");
    }
    lua_pushstring(L, dateTimeToString(payload->getExpiry()).c_str());
    lua_setfield(L, -2, "expiry");
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventSubscriptionConvertor::getDocumentation() const {
    return Documentation(
        "PubSubEventSubscription",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
        "- `jid`: jid (string)\n"
        "- `subscription`: `\"none\"`, `\"pending\"`, `\"subscribed\"`, or `\"unconfigured\"`\n"
        "- `subscription_id`: string (Optional)\n"
        "- `expiry`: datetime (string)\n"
    );
}
