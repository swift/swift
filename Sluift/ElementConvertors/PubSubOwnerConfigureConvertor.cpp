/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerConfigureConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubOwnerConfigureConvertor::PubSubOwnerConfigureConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubOwnerConfigure>("pubsub_owner_configure"),
        convertors(convertors) {
}

PubSubOwnerConfigureConvertor::~PubSubOwnerConfigureConvertor() {
}

std::shared_ptr<PubSubOwnerConfigure> PubSubOwnerConfigureConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubOwnerConfigure> result = std::make_shared<PubSubOwnerConfigure>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "data");
    if (!lua_isnil(L, -1)) {
        if (std::shared_ptr<Form> payload = std::dynamic_pointer_cast<Form>(convertors->convertFromLuaUntyped(L, -1, "form"))) {
            result->setData(payload);
        }
    }
    lua_pop(L, 1);
    return result;
}

void PubSubOwnerConfigureConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubOwnerConfigure> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getNode()) {
        lua_pushstring(L, (*payload->getNode()).c_str());
        lua_setfield(L, -2, "node");
    }
    if (convertors->convertToLuaUntyped(L, payload->getData()) > 0) {
        lua_setfield(L, -2, "data");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerConfigureConvertor::getDocumentation() const {
    return Documentation(
        "PubSubOwnerConfigure",
        "This table has the following fields:\n\n"
        "- `node`: string (Optional)\n"
        "- `data`: @{Form}\n"
    );
}
