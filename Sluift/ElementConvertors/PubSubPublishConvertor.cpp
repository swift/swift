/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubPublishConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubPublishConvertor::PubSubPublishConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubPublish>("pubsub_publish"),
        convertors(convertors) {
}

PubSubPublishConvertor::~PubSubPublishConvertor() {
}

std::shared_ptr<PubSubPublish> PubSubPublishConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubPublish> result = std::make_shared<PubSubPublish>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "items");
    if (lua_type(L, -1) == LUA_TTABLE) {
        std::vector< std::shared_ptr<PubSubItem> > items;
        for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            if (!lua_isnil(L, -1)) {
                if (std::shared_ptr<PubSubItem> payload = std::dynamic_pointer_cast<PubSubItem>(convertors->convertFromLuaUntyped(L, -1, "pubsub_item"))) {
                    items.push_back(payload);
                }
            }
            lua_pop(L, 1);
        }

        result->setItems(items);
    }
    lua_pop(L, 1);
    return result;
}

void PubSubPublishConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubPublish> payload) {
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
}

boost::optional<LuaElementConvertor::Documentation> PubSubPublishConvertor::getDocumentation() const {
    return Documentation(
        "PubSubPublish",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
        "- `items`: array<@{PubSubItem}>\n"
    );
}
