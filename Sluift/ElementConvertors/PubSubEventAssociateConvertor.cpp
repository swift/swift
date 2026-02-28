/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventAssociateConvertor.h>

#include <memory>

#include <lua.hpp>

using namespace Swift;

PubSubEventAssociateConvertor::PubSubEventAssociateConvertor() :
        GenericLuaElementConvertor<PubSubEventAssociate>("pubsub_event_associate") {
}

PubSubEventAssociateConvertor::~PubSubEventAssociateConvertor() {
}

std::shared_ptr<PubSubEventAssociate> PubSubEventAssociateConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEventAssociate> result = std::make_shared<PubSubEventAssociate>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubEventAssociateConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEventAssociate> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getNode().c_str());
    lua_setfield(L, -2, "node");
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventAssociateConvertor::getDocumentation() const {
    return Documentation(
        "PubSubEventAssociate",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
    );
}
