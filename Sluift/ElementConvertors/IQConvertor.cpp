/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/IQConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

IQConvertor::IQConvertor(LuaElementConvertors* convertors) :
        StanzaConvertor<IQ>("iq"),
        convertors(convertors) {
}

IQConvertor::~IQConvertor() {
}

std::shared_ptr<IQ> IQConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<IQ> result = getStanza(L, convertors);
    lua_getfield(L, -1, "type");
    if (lua_isstring(L, -1)) {
        result->setType(IQConvertor::convertIQTypeFromString(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void IQConvertor::doConvertToLua(lua_State* L, std::shared_ptr<IQ> stanza) {
    pushStanza(L, stanza, convertors);
    const std::string type = IQConvertor::convertIQTypeToString(stanza->getType());
    lua_pushstring(L, type.c_str());
    lua_setfield(L, -2, "type");
}

boost::optional<LuaElementConvertor::Documentation> IQConvertor::getDocumentation() const {
    return Documentation(
        "IQ",
        "This table has the following fields:\n\n"
        "- `type`: string\n"
        "- `id`: string\n"
        "- `from`: string\n"
        "- `to`: string\n"
        "- `payloads`: array<@{Payload}>\n"
    );
}

std::string IQConvertor::convertIQTypeToString(IQ::Type type) {
    switch (type) {
        case IQ::Get: return "get";
        case IQ::Set: return "set";
        case IQ::Result: return "result";
        case IQ::Error: return "error";
    }
    assert(false);
    return "";
}

IQ::Type IQConvertor::convertIQTypeFromString(const std::string& type) {
    if (type == "get") {
        return IQ::Get;
    }
    else if (type == "set") {
        return IQ::Set;
    }
    else {
        throw Lua::Exception("Illegal query type: '" + type + "'");
    }
}
