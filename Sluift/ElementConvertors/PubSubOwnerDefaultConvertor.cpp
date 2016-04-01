/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerDefaultConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubOwnerDefaultConvertor::PubSubOwnerDefaultConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubOwnerDefault>("pubsub_owner_default"),
        convertors(convertors) {
}

PubSubOwnerDefaultConvertor::~PubSubOwnerDefaultConvertor() {
}

std::shared_ptr<PubSubOwnerDefault> PubSubOwnerDefaultConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubOwnerDefault> result = std::make_shared<PubSubOwnerDefault>();
    lua_getfield(L, -1, "data");
    if (!lua_isnil(L, -1)) {
        if (std::shared_ptr<Form> payload = std::dynamic_pointer_cast<Form>(convertors->convertFromLuaUntyped(L, -1, "form"))) {
            result->setData(payload);
        }
    }
    lua_pop(L, 1);
    return result;
}

void PubSubOwnerDefaultConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubOwnerDefault> payload) {
    lua_createtable(L, 0, 0);
    if (convertors->convertToLuaUntyped(L, payload->getData()) > 0) {
        lua_setfield(L, -2, "data");
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerDefaultConvertor::getDocumentation() const {
    return Documentation(
        "PubSubOwnerDefault",
        "This table has the following fields:\n\n"
        "- `data`: @{Form}\n"
    );
}
