/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/SoftwareVersionConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/Lua/Check.h>

using namespace Swift;

SoftwareVersionConvertor::SoftwareVersionConvertor() : GenericLuaElementConvertor<SoftwareVersion>("software_version") {
}

SoftwareVersionConvertor::~SoftwareVersionConvertor() {
}

std::shared_ptr<SoftwareVersion> SoftwareVersionConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<SoftwareVersion> result = std::make_shared<SoftwareVersion>();
    lua_getfield(L, -1, "name");
    if (!lua_isnil(L, -1)) {
        result->setName(std::string(Lua::checkString(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "version");
    if (!lua_isnil(L, -1)) {
        result->setVersion(std::string(Lua::checkString(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "os");
    if (!lua_isnil(L, -1)) {
        result->setOS(std::string(Lua::checkString(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void SoftwareVersionConvertor::doConvertToLua(lua_State* L, std::shared_ptr<SoftwareVersion> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getName().c_str());
    lua_setfield(L, -2, "name");
    lua_pushstring(L, payload->getVersion().c_str());
    lua_setfield(L, -2, "version");
    lua_pushstring(L, payload->getOS().c_str());
    lua_setfield(L, -2, "os");
}
