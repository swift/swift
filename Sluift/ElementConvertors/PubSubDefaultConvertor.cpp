/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubDefaultConvertor.h>

#include <memory>

#include <lua.hpp>

using namespace Swift;

PubSubDefaultConvertor::PubSubDefaultConvertor() :
        GenericLuaElementConvertor<PubSubDefault>("pubsub_default") {
}

PubSubDefaultConvertor::~PubSubDefaultConvertor() {
}

std::shared_ptr<PubSubDefault> PubSubDefaultConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubDefault> result = std::make_shared<PubSubDefault>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "type");
    if (lua_isstring(L, -1)) {
        if (std::string(lua_tostring(L, -1)) == "none") {
            result->setType(PubSubDefault::None);
        }
        if (std::string(lua_tostring(L, -1)) == "collection") {
            result->setType(PubSubDefault::Collection);
        }
        if (std::string(lua_tostring(L, -1)) == "leaf") {
            result->setType(PubSubDefault::Leaf);
        }
    }
    lua_pop(L, 1);
    return result;
}

void PubSubDefaultConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubDefault> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getNode()) {
        lua_pushstring(L, (*payload->getNode()).c_str());
        lua_setfield(L, -2, "node");
    }
    switch (payload->getType()) {
        case PubSubDefault::None:
            lua_pushstring(L, "none");
            break;
        case PubSubDefault::Collection:
            lua_pushstring(L, "collection");
            break;
        case PubSubDefault::Leaf:
            lua_pushstring(L, "leaf");
            break;
    }
    lua_setfield(L, -2, "type");
}

boost::optional<LuaElementConvertor::Documentation> PubSubDefaultConvertor::getDocumentation() const {
    return Documentation(
        "PubSubDefault",
        "This table has the following fields:\n\n"
        "- `node`: string (Optional)\n"
        "- `type`: `\"none\"`, `\"collection\"`, or `\"leaf\"`\n"
    );
}
