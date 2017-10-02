/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventCollectionConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubEventCollectionConvertor::PubSubEventCollectionConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubEventCollection>("pubsub_event_collection"),
        convertors(convertors) {
}

PubSubEventCollectionConvertor::~PubSubEventCollectionConvertor() {
}

std::shared_ptr<PubSubEventCollection> PubSubEventCollectionConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEventCollection> result = std::make_shared<PubSubEventCollection>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "disassociate");
    if (!lua_isnil(L, -1)) {
        if (std::shared_ptr<PubSubEventDisassociate> payload = std::dynamic_pointer_cast<PubSubEventDisassociate>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_disassociate"))) {
            result->setDisassociate(payload);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "associate");
    if (!lua_isnil(L, -1)) {
        if (std::shared_ptr<PubSubEventAssociate> payload = std::dynamic_pointer_cast<PubSubEventAssociate>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_associate"))) {
            result->setAssociate(payload);
        }
    }
    lua_pop(L, 1);
    return result;
}

void PubSubEventCollectionConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEventCollection> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getNode()) {
        lua_pushstring(L, (*payload->getNode()).c_str());
        lua_setfield(L, -2, "node");
    }
    if (convertors->convertToLuaUntyped(L, payload->getDisassociate()) > 0) {
        lua_setfield(L, -2, "disassociate");
    }
    if (convertors->convertToLuaUntyped(L, payload->getAssociate()) > 0) {
        lua_setfield(L, -2, "associate");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventCollectionConvertor::getDocumentation() const {
    return Documentation(
        "PubSubEventCollection",
        "This table has the following fields:\n\n"
        "- `node`: string (Optional)\n"
        "- `disassociate`: @{PubSubEventDisassociate}\n"
        "- `associate`: @{PubSubEventAssociate}\n"
    );
}
