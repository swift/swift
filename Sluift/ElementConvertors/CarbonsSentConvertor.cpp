/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/CarbonsSentConvertor.h>

#include <memory>
#include <lua.hpp>

#include <Swiften/Elements/Forwarded.h>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

CarbonsSentConvertor::CarbonsSentConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<CarbonsSent>("carbons_sent"),
        convertors(convertors) {
}

CarbonsSentConvertor::~CarbonsSentConvertor() {
}

std::shared_ptr<CarbonsSent> CarbonsSentConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<CarbonsSent> result = std::make_shared<CarbonsSent>();
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

void CarbonsSentConvertor::doConvertToLua(lua_State* L, std::shared_ptr<CarbonsSent> payload) {
    lua_createtable(L, 0, 0);
    if (convertors->convertToLuaUntyped(L, payload->getForwarded()) > 0) {
        lua_setfield(L, -2, "payload");
    }
}

boost::optional<LuaElementConvertor::Documentation> CarbonsSentConvertor::getDocumentation() const {
    return Documentation(
        "CarbonsSent",
        "This table has the following fields:\n\n"
        "- `payload`: @{Forwarded}\n"
    );
}
