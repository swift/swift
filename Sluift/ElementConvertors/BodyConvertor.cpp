/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/BodyConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/Lua/LuaUtils.h>

using namespace Swift;

BodyConvertor::BodyConvertor() : GenericLuaElementConvertor<Body>("body") {
}

BodyConvertor::~BodyConvertor() {
}

std::shared_ptr<Body> BodyConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<Body> result = std::make_shared<Body>();
    if (boost::optional<std::string> value = Lua::getStringField(L, -1, "text")) {
        result->setText(*value);
    }
    return result;
}

void BodyConvertor::doConvertToLua(lua_State* L, std::shared_ptr<Body> payload) {
    lua_createtable(L, 0, 0);
    if (!payload->getText().empty()) {
        lua_pushstring(L, payload->getText().c_str());
        lua_setfield(L, -2, "text");
    }
}
