/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerSubscriptionConvertor.h>

#include <memory>

#include <lua.hpp>

using namespace Swift;

PubSubOwnerSubscriptionConvertor::PubSubOwnerSubscriptionConvertor() :
        GenericLuaElementConvertor<PubSubOwnerSubscription>("pubsub_owner_subscription") {
}

PubSubOwnerSubscriptionConvertor::~PubSubOwnerSubscriptionConvertor() {
}

std::shared_ptr<PubSubOwnerSubscription> PubSubOwnerSubscriptionConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubOwnerSubscription> result = std::make_shared<PubSubOwnerSubscription>();
    lua_getfield(L, -1, "jid");
    if (lua_isstring(L, -1)) {
        result->setJID(JID(std::string(lua_tostring(L, -1))));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "subscription");
    if (lua_isstring(L, -1)) {
        if (std::string(lua_tostring(L, -1)) == "none") {
            result->setSubscription(PubSubOwnerSubscription::None);
        }
        if (std::string(lua_tostring(L, -1)) == "pending") {
            result->setSubscription(PubSubOwnerSubscription::Pending);
        }
        if (std::string(lua_tostring(L, -1)) == "subscribed") {
            result->setSubscription(PubSubOwnerSubscription::Subscribed);
        }
        if (std::string(lua_tostring(L, -1)) == "unconfigured") {
            result->setSubscription(PubSubOwnerSubscription::Unconfigured);
        }
    }
    lua_pop(L, 1);
    return result;
}

void PubSubOwnerSubscriptionConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubOwnerSubscription> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getJID().toString().c_str());
    lua_setfield(L, -2, "jid");
    switch (payload->getSubscription()) {
        case PubSubOwnerSubscription::None:
            lua_pushstring(L, "none");
            break;
        case PubSubOwnerSubscription::Pending:
            lua_pushstring(L, "pending");
            break;
        case PubSubOwnerSubscription::Subscribed:
            lua_pushstring(L, "subscribed");
            break;
        case PubSubOwnerSubscription::Unconfigured:
            lua_pushstring(L, "unconfigured");
            break;
    }
    lua_setfield(L, -2, "subscription");
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerSubscriptionConvertor::getDocumentation() const {
    return Documentation(
        "PubSubOwnerSubscription",
        "This table has the following fields:\n\n"
        "- `jid`: jid (string)\n"
        "- `subscription`: `\"none\"`, `\"pending\"`, `\"subscribed\"`, or `\"unconfigured\"`\n"
    );
}
