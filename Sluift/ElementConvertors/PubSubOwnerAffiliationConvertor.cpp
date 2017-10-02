/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerAffiliationConvertor.h>

#include <memory>

#include <lua.hpp>

using namespace Swift;

PubSubOwnerAffiliationConvertor::PubSubOwnerAffiliationConvertor() :
        GenericLuaElementConvertor<PubSubOwnerAffiliation>("pubsub_owner_affiliation") {
}

PubSubOwnerAffiliationConvertor::~PubSubOwnerAffiliationConvertor() {
}

std::shared_ptr<PubSubOwnerAffiliation> PubSubOwnerAffiliationConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubOwnerAffiliation> result = std::make_shared<PubSubOwnerAffiliation>();
    lua_getfield(L, -1, "jid");
    if (lua_isstring(L, -1)) {
        result->setJID(JID(std::string(lua_tostring(L, -1))));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "type");
    if (lua_isstring(L, -1)) {
        if (std::string(lua_tostring(L, -1)) == "none") {
            result->setType(PubSubOwnerAffiliation::None);
        }
        if (std::string(lua_tostring(L, -1)) == "member") {
            result->setType(PubSubOwnerAffiliation::Member);
        }
        if (std::string(lua_tostring(L, -1)) == "outcast") {
            result->setType(PubSubOwnerAffiliation::Outcast);
        }
        if (std::string(lua_tostring(L, -1)) == "owner") {
            result->setType(PubSubOwnerAffiliation::Owner);
        }
        if (std::string(lua_tostring(L, -1)) == "publisher") {
            result->setType(PubSubOwnerAffiliation::Publisher);
        }
        if (std::string(lua_tostring(L, -1)) == "publish_only") {
            result->setType(PubSubOwnerAffiliation::PublishOnly);
        }
    }
    lua_pop(L, 1);
    return result;
}

void PubSubOwnerAffiliationConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubOwnerAffiliation> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getJID().toString().c_str());
    lua_setfield(L, -2, "jid");
    switch (payload->getType()) {
        case PubSubOwnerAffiliation::None:
            lua_pushstring(L, "none");
            break;
        case PubSubOwnerAffiliation::Member:
            lua_pushstring(L, "member");
            break;
        case PubSubOwnerAffiliation::Outcast:
            lua_pushstring(L, "outcast");
            break;
        case PubSubOwnerAffiliation::Owner:
            lua_pushstring(L, "owner");
            break;
        case PubSubOwnerAffiliation::Publisher:
            lua_pushstring(L, "publisher");
            break;
        case PubSubOwnerAffiliation::PublishOnly:
            lua_pushstring(L, "publish_only");
            break;
    }
    lua_setfield(L, -2, "type");
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerAffiliationConvertor::getDocumentation() const {
    return Documentation(
        "PubSubOwnerAffiliation",
        "This table has the following fields:\n\n"
        "- `jid`: jid (string)\n"
        "- `type`: `\"none\"`, `\"member\"`, `\"outcast\"`, `\"owner\"`, `\"publisher\"`, or `\"publish_only\"`\n"
    );
}
