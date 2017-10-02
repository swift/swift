/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/CarbonsReceivedConvertor.h>

#include <memory>
#include <lua.hpp>

#include <Swiften/Elements/Forwarded.h>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

CarbonsReceivedConvertor::CarbonsReceivedConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<CarbonsReceived>("carbons_received"),
        convertors(convertors) {
}

CarbonsReceivedConvertor::~CarbonsReceivedConvertor() {
}

std::shared_ptr<CarbonsReceived> CarbonsReceivedConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<CarbonsReceived> result = std::make_shared<CarbonsReceived>();
    lua_getfield(L, -1, "payload");
    if (!lua_isnil(L, -1)) {
        std::shared_ptr<Forwarded> payload = std::dynamic_pointer_cast<Forwarded>(convertors->convertFromLuaUntyped(L, -1, "payload"));
        if (!!payload) {
            result->setForwarded(payload);
        }
    }
    lua_pop(L, 1);
    return result;
}

void CarbonsReceivedConvertor::doConvertToLua(lua_State* L, std::shared_ptr<CarbonsReceived> payload) {
    lua_createtable(L, 0, 0);
    if (convertors->convertToLuaUntyped(L, payload->getForwarded()) > 0) {
        lua_setfield(L, -2, "payload");

    }
}

boost::optional<LuaElementConvertor::Documentation> CarbonsReceivedConvertor::getDocumentation() const {
    return Documentation(
        "CarbonsReceived",
        "This table has the following fields:\n\n"
        "- `payload`: @{Forwarded}\n"
    );
}
