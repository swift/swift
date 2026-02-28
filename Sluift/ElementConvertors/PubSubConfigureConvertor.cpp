/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubConfigureConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubConfigureConvertor::PubSubConfigureConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubConfigure>("pubsub_configure"),
        convertors(convertors) {
}

PubSubConfigureConvertor::~PubSubConfigureConvertor() {
}

std::shared_ptr<PubSubConfigure> PubSubConfigureConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubConfigure> result = std::make_shared<PubSubConfigure>();
    lua_getfield(L, -1, "data");
    if (!lua_isnil(L, -1)) {
        if (std::shared_ptr<Form> payload = std::dynamic_pointer_cast<Form>(convertors->convertFromLuaUntyped(L, -1, "form"))) {
            result->setData(payload);
        }
    }
    lua_pop(L, 1);
    return result;
}

void PubSubConfigureConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubConfigure> payload) {
    lua_createtable(L, 0, 0);
    if (convertors->convertToLuaUntyped(L, payload->getData()) > 0) {
        lua_setfield(L, -2, "data");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubConfigureConvertor::getDocumentation() const {
    return Documentation(
        "PubSubConfigure",
        "This table has the following fields:\n\n"
        "- `data`: @{Form}\n"
    );
}
