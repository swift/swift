/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventItemConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubEventItemConvertor::PubSubEventItemConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubEventItem>("pubsub_event_item"),
        convertors(convertors) {
}

PubSubEventItemConvertor::~PubSubEventItemConvertor() {
}

std::shared_ptr<PubSubEventItem> PubSubEventItemConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEventItem> result = std::make_shared<PubSubEventItem>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "publisher");
    if (lua_isstring(L, -1)) {
        result->setPublisher(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "data");
    if (lua_type(L, -1) == LUA_TTABLE) {
        std::vector< std::shared_ptr<Payload> > items;
        for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            if (!lua_isnil(L, -1)) {
                if (std::shared_ptr<Payload> payload = std::dynamic_pointer_cast<Payload>(convertors->convertFromLua(L, -1))) {
                    items.push_back(payload);
                }
            }
            lua_pop(L, 1);
        }

        result->setData(items);
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "id");
    if (lua_isstring(L, -1)) {
        result->setID(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubEventItemConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEventItem> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getNode()) {
        lua_pushstring(L, (*payload->getNode()).c_str());
        lua_setfield(L, -2, "node");
    }
    if (payload->getPublisher()) {
        lua_pushstring(L, (*payload->getPublisher()).c_str());
        lua_setfield(L, -2, "publisher");
    }
    if (!payload->getData().empty()) {
        lua_createtable(L, boost::numeric_cast<int>(payload->getData().size()), 0);
        {
            int i = 0;
            for (auto&& item : payload->getData()) {
                if (convertors->convertToLua(L, item) > 0) {
                    lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
                    ++i;
                }
            }
        }
        lua_setfield(L, -2, "data");
    }
    if (payload->getID()) {
        lua_pushstring(L, (*payload->getID()).c_str());
        lua_setfield(L, -2, "id");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventItemConvertor::getDocumentation() const {
    return Documentation(
        "PubSubEventItem",
        "This table has the following fields:\n\n"
        "- `node`: string (Optional)\n"
        "- `publisher`: string (Optional)\n"
        "- `data`: array<element (table)>\n"
        "- `id`: string (Optional)\n"
    );
}
