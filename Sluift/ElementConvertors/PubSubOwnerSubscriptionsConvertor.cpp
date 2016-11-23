/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerSubscriptionsConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubOwnerSubscriptionsConvertor::PubSubOwnerSubscriptionsConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubOwnerSubscriptions>("pubsub_owner_subscriptions"),
        convertors(convertors) {
}

PubSubOwnerSubscriptionsConvertor::~PubSubOwnerSubscriptionsConvertor() {
}

std::shared_ptr<PubSubOwnerSubscriptions> PubSubOwnerSubscriptionsConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubOwnerSubscriptions> result = std::make_shared<PubSubOwnerSubscriptions>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    if (lua_type(L, -1) == LUA_TTABLE) {
        std::vector< std::shared_ptr<PubSubOwnerSubscription> > items;
        for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            if (!lua_isnil(L, -1)) {
                if (std::shared_ptr<PubSubOwnerSubscription> payload = std::dynamic_pointer_cast<PubSubOwnerSubscription>(convertors->convertFromLuaUntyped(L, -1, "pubsub_owner_subscription"))) {
                    items.push_back(payload);
                }
            }
            lua_pop(L, 1);
        }

        result->setSubscriptions(items);
    }
    return result;
}

void PubSubOwnerSubscriptionsConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubOwnerSubscriptions> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getNode().c_str());
    lua_setfield(L, -2, "node");
    if (!payload->getSubscriptions().empty()) {
        {
            int i = 0;
            for (auto&& item : payload->getSubscriptions()) {
                if (convertors->convertToLuaUntyped(L, item) > 0) {
                    lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
                    ++i;
                }
            }
        }
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerSubscriptionsConvertor::getDocumentation() const {
    return Documentation(
        "PubSubOwnerSubscriptions",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
        "- `subscriptions`: array<@{PubSubOwnerSubscription}>\n"
    );
}
