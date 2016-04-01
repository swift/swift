/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventRetractConvertor.h>

#include <memory>

#include <lua.hpp>

using namespace Swift;

PubSubEventRetractConvertor::PubSubEventRetractConvertor() :
        GenericLuaElementConvertor<PubSubEventRetract>("pubsub_event_retract") {
}

PubSubEventRetractConvertor::~PubSubEventRetractConvertor() {
}

std::shared_ptr<PubSubEventRetract> PubSubEventRetractConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEventRetract> result = std::make_shared<PubSubEventRetract>();
    lua_getfield(L, -1, "id");
    if (lua_isstring(L, -1)) {
        result->setID(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubEventRetractConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEventRetract> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getID().c_str());
    lua_setfield(L, -2, "id");
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventRetractConvertor::getDocumentation() const {
    return Documentation(
        "PubSubEventRetract",
        "This table has the following fields:\n\n"
        "- `id`: string\n"
    );
}
