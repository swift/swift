/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PresenceConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PresenceConvertor::PresenceConvertor(LuaElementConvertors* convertors) :
        StanzaConvertor<Presence>("presence"),
        convertors(convertors) {
}

PresenceConvertor::~PresenceConvertor() {
}

std::shared_ptr<Presence> PresenceConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<Presence> result = getStanza(L, convertors);
    lua_getfield(L, -1, "type");
    if (lua_isstring(L, -1)) {
        result->setType(convertPresenceTypeFromString(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void PresenceConvertor::doConvertToLua(lua_State* L, std::shared_ptr<Presence> stanza) {
    pushStanza(L, stanza, convertors);
    const std::string type = convertPresenceTypeToString(stanza->getType());
    lua_pushstring(L, type.c_str());
    lua_setfield(L, -2, "type");
}

boost::optional<LuaElementConvertor::Documentation> PresenceConvertor::getDocumentation() const {
    return Documentation(
        "Presence",
        "This table has the following fields:\n\n"
        "- `type`: string\n"
        "- `id`: string\n"
        "- `from`: string\n"
        "- `to`: string\n"
        "- `payloads`: array<@{Payload}>\n"
    );
}

std::string PresenceConvertor::convertPresenceTypeToString(Presence::Type type) {
    switch (type) {
        case Presence::Available: return "available";
        case Presence::Error: return "error";
        case Presence::Probe: return "probe";
        case Presence::Subscribe: return "subscribe";
        case Presence::Subscribed: return "subscribed";
        case Presence::Unavailable: return "unavailable";
        case Presence::Unsubscribe: return "unsubscribe";
        case Presence::Unsubscribed: return "unsubscribed";
    }
    assert(false);
    return "";
}

Presence::Type PresenceConvertor::convertPresenceTypeFromString(const std::string& type) {
    if (type == "available") {
        return Presence::Available;
    }
    else if (type == "error") {
        return Presence::Error;
    }
    else if (type == "probe") {
        return Presence::Probe;
    }
    else if (type == "subscribe") {
        return Presence::Subscribe;
    }
    else if (type == "subscribed") {
        return Presence::Subscribed;
    }
    else if (type == "unavailable") {
        return Presence::Unavailable;
    }
    else if (type == "unsubscribe") {
        return Presence::Unsubscribe;
    }
    else if (type == "unsubscribed") {
        return Presence::Unsubscribed;
    }
    else {
        throw Lua::Exception("Illegal presence type: '" + type + "'");
    }
}
