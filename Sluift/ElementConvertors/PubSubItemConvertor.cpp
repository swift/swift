/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubItemConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubItemConvertor::PubSubItemConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubItem>("pubsub_item"),
        convertors(convertors) {
}

PubSubItemConvertor::~PubSubItemConvertor() {
}

std::shared_ptr<PubSubItem> PubSubItemConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubItem> result = std::make_shared<PubSubItem>();
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

void PubSubItemConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubItem> payload) {
    lua_createtable(L, 0, 0);
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
    lua_pushstring(L, payload->getID().c_str());
    lua_setfield(L, -2, "id");
}

boost::optional<LuaElementConvertor::Documentation> PubSubItemConvertor::getDocumentation() const {
    return Documentation(
        "PubSubItem",
        "This table has the following fields:\n\n"
        "- `data`: array<element (table)>\n"
        "- `id`: string\n"
    );
}
