/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/SubjectConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/Lua/LuaUtils.h>

using namespace Swift;

SubjectConvertor::SubjectConvertor() : GenericLuaElementConvertor<Subject>("subject") {
}

SubjectConvertor::~SubjectConvertor() {
}

std::shared_ptr<Subject> SubjectConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<Subject> result = std::make_shared<Subject>();
    if (boost::optional<std::string> value = Lua::getStringField(L, -1, "text")) {
        result->setText(*value);
    }
    return result;
}

void SubjectConvertor::doConvertToLua(lua_State* L, std::shared_ptr<Subject> payload) {
    lua_createtable(L, 0, 0);
    if (!payload->getText().empty()) {
        lua_pushstring(L, payload->getText().c_str());
        lua_setfield(L, -2, "text");
    }
}
