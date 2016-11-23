/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventItemsConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubEventItemsConvertor::PubSubEventItemsConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubEventItems>("pubsub_event_items"),
        convertors(convertors) {
}

PubSubEventItemsConvertor::~PubSubEventItemsConvertor() {
}

std::shared_ptr<PubSubEventItems> PubSubEventItemsConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEventItems> result = std::make_shared<PubSubEventItems>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "items");
    if (lua_type(L, -1) == LUA_TTABLE) {
        std::vector< std::shared_ptr<PubSubEventItem> > items;
        for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            if (!lua_isnil(L, -1)) {
                if (std::shared_ptr<PubSubEventItem> payload = std::dynamic_pointer_cast<PubSubEventItem>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_item"))) {
                    items.push_back(payload);
                }
            }
            lua_pop(L, 1);
        }

        result->setItems(items);
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "retracts");
    if (lua_type(L, -1) == LUA_TTABLE) {
        std::vector< std::shared_ptr<PubSubEventRetract> > items;
        for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            if (!lua_isnil(L, -1)) {
                if (std::shared_ptr<PubSubEventRetract> payload = std::dynamic_pointer_cast<PubSubEventRetract>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_retract"))) {
                    items.push_back(payload);
                }
            }
            lua_pop(L, 1);
        }

        result->setRetracts(items);
    }
    lua_pop(L, 1);
    return result;
}

void PubSubEventItemsConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEventItems> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getNode().c_str());
    lua_setfield(L, -2, "node");
    if (!payload->getItems().empty()) {
        lua_createtable(L, boost::numeric_cast<int>(payload->getItems().size()), 0);
        {
            int i = 0;
            for (auto&& item : payload->getItems()) {
                if (convertors->convertToLuaUntyped(L, item) > 0) {
                    lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
                    ++i;
                }
            }
        }
        lua_setfield(L, -2, "items");
    }
    if (!payload->getRetracts().empty()) {
        lua_createtable(L, boost::numeric_cast<int>(payload->getRetracts().size()), 0);
        {
            int i = 0;
            for (auto&& item : payload->getRetracts()) {
                if (convertors->convertToLuaUntyped(L, item) > 0) {
                    lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
                    ++i;
                }
            }
        }
        lua_setfield(L, -2, "retracts");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventItemsConvertor::getDocumentation() const {
    return Documentation(
        "PubSubEventItems",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
        "- `items`: array<@{PubSubEventItem}>\n"
        "- `retracts`: array<@{PubSubEventRetract}>\n"
    );
}
