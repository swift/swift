/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventPurgeConvertor.h>

#include <memory>

#include <lua.hpp>

using namespace Swift;

PubSubEventPurgeConvertor::PubSubEventPurgeConvertor() :
        GenericLuaElementConvertor<PubSubEventPurge>("pubsub_event_purge") {
}

PubSubEventPurgeConvertor::~PubSubEventPurgeConvertor() {
}

std::shared_ptr<PubSubEventPurge> PubSubEventPurgeConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEventPurge> result = std::make_shared<PubSubEventPurge>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubEventPurgeConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEventPurge> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getNode().c_str());
    lua_setfield(L, -2, "node");
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventPurgeConvertor::getDocumentation() const {
    return Documentation(
        "PubSubEventPurge",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
    );
}
