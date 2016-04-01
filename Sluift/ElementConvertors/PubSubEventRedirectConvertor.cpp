/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventRedirectConvertor.h>

#include <memory>

#include <lua.hpp>

using namespace Swift;

PubSubEventRedirectConvertor::PubSubEventRedirectConvertor() :
        GenericLuaElementConvertor<PubSubEventRedirect>("pubsub_event_redirect") {
}

PubSubEventRedirectConvertor::~PubSubEventRedirectConvertor() {
}

std::shared_ptr<PubSubEventRedirect> PubSubEventRedirectConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEventRedirect> result = std::make_shared<PubSubEventRedirect>();
    lua_getfield(L, -1, "uri");
    if (lua_isstring(L, -1)) {
        result->setURI(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PubSubEventRedirectConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEventRedirect> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getURI().c_str());
    lua_setfield(L, -2, "uri");
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventRedirectConvertor::getDocumentation() const {
    return Documentation(
        "PubSubEventRedirect",
        "This table has the following fields:\n\n"
        "- `uri`: string\n"
    );
}
