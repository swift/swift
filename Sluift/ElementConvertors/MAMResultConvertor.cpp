/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/MAMResultConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Swiften/Elements/Forwarded.h>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

MAMResultConvertor::MAMResultConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<MAMResult>("mam_result"),
        convertors(convertors) {
}

MAMResultConvertor::~MAMResultConvertor() {
}

std::shared_ptr<MAMResult> MAMResultConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<MAMResult> result = std::make_shared<MAMResult>();
    lua_getfield(L, -1, "payload");
    if (!lua_isnil(L, -1)) {
        std::shared_ptr<Forwarded> payload = std::dynamic_pointer_cast<Forwarded>(convertors->convertFromLuaUntyped(L, -1, "payload"));
        if (!!payload) {
            result->setPayload(payload);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "id");
    if (lua_isstring(L, -1)) {
        result->setID(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "query_id");
    if (lua_isstring(L, -1)) {
        result->setQueryID(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void MAMResultConvertor::doConvertToLua(lua_State* L, std::shared_ptr<MAMResult> payload) {
    lua_createtable(L, 0, 0);
    if (convertors->convertToLuaUntyped(L, payload->getPayload()) > 0) {
        lua_setfield(L, -2, "payload");
    }
    lua_pushstring(L, payload->getID().c_str());
    lua_setfield(L, -2, "id");
    if (payload->getQueryID()) {
        lua_pushstring(L, (*payload->getQueryID()).c_str());
        lua_setfield(L, -2, "query_id");
    }
}

boost::optional<LuaElementConvertor::Documentation> MAMResultConvertor::getDocumentation() const {
    return Documentation(
        "MAMResult",
        "This table has the following fields:\n\n"
        "- `payload`: @{Forwarded}\n"
        "- `id`: string\n"
        "- `query_id`: string (Optional)\n"
    );
}
