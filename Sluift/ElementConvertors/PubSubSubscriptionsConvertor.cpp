/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubSubscriptionsConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubSubscriptionsConvertor::PubSubSubscriptionsConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubSubscriptions>("pubsub_subscriptions"),
        convertors(convertors) {
}

PubSubSubscriptionsConvertor::~PubSubSubscriptionsConvertor() {
}

std::shared_ptr<PubSubSubscriptions> PubSubSubscriptionsConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubSubscriptions> result = std::make_shared<PubSubSubscriptions>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    if (lua_type(L, -1) == LUA_TTABLE) {
        std::vector< std::shared_ptr<PubSubSubscription> > items;
        for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            if (!lua_isnil(L, -1)) {
                if (std::shared_ptr<PubSubSubscription> payload = std::dynamic_pointer_cast<PubSubSubscription>(convertors->convertFromLuaUntyped(L, -1, "pubsub_subscription"))) {
                    items.push_back(payload);
                }
            }
            lua_pop(L, 1);
        }

        result->setSubscriptions(items);
    }
    return result;
}

void PubSubSubscriptionsConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubSubscriptions> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getNode()) {
        lua_pushstring(L, (*payload->getNode()).c_str());
        lua_setfield(L, -2, "node");
    }
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

boost::optional<LuaElementConvertor::Documentation> PubSubSubscriptionsConvertor::getDocumentation() const {
    return Documentation(
        "PubSubSubscriptions",
        "This table has the following fields:\n\n"
        "- `node`: string (Optional)\n"
        "- `subscriptions`: array<@{PubSubSubscription}>\n"
    );
}
