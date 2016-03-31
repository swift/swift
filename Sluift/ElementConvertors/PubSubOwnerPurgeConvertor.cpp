/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerPurgeConvertor.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <lua.hpp>

using namespace Swift;

PubSubOwnerPurgeConvertor::PubSubOwnerPurgeConvertor() :
        GenericLuaElementConvertor<PubSubOwnerPurge>("pubsub_owner_purge") {
}

PubSubOwnerPurgeConvertor::~PubSubOwnerPurgeConvertor() {
}

boost::shared_ptr<PubSubOwnerPurge> PubSubOwnerPurgeConvertor::doConvertFromLua(lua_State* L) {
    boost::shared_ptr<PubSubOwnerPurge> result = boost::make_shared<PubSubOwnerPurge>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubOwnerPurgeConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubOwnerPurge> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getNode().c_str());
    lua_setfield(L, -2, "node");
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerPurgeConvertor::getDocumentation() const {
    return Documentation(
        "PubSubOwnerPurge",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
    );
}
