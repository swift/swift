/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/MAMFinConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Swiften/Elements/ResultSet.h>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

MAMFinConvertor::MAMFinConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<MAMFin>("mam_fin"),
        convertors(convertors) {
}

MAMFinConvertor::~MAMFinConvertor() {
}

std::shared_ptr<MAMFin> MAMFinConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<MAMFin> result = std::make_shared<MAMFin>();
    lua_getfield(L, -1, "query_id");
    if (lua_isstring(L, -1)) {
        result->setQueryID(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "complete");
    if (lua_isboolean(L, -1)) {
        result->setComplete(lua_toboolean(L, -1) != 0);
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "stable");
    if (!lua_isnil(L, -1)) {
        result->setStable(lua_toboolean(L, -1) != 0);
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "result_set");
    if (!lua_isnil(L, -1)) {
        std::shared_ptr<ResultSet> resultSet = std::dynamic_pointer_cast<ResultSet>(convertors->convertFromLuaUntyped(L, -1, "result_set"));
        if (!!resultSet) {
            result->setResultSet(resultSet);
        }
    }
    lua_pop(L, 1);
    return result;
}

void MAMFinConvertor::doConvertToLua(lua_State* L, std::shared_ptr<MAMFin> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getQueryID()) {
        lua_pushstring(L, (*payload->getQueryID()).c_str());
        lua_setfield(L, -2, "query_id");
    }
    lua_pushboolean(L, payload->isComplete());
    lua_setfield(L, -2, "complete");
    lua_pushboolean(L, payload->isStable());
    lua_setfield(L, -2, "stable");
    if (convertors->convertToLuaUntyped(L, payload->getResultSet()) > 0) {
        lua_setfield(L, -2, "result_set");
    }
}

boost::optional<LuaElementConvertor::Documentation> MAMFinConvertor::getDocumentation() const {
    return Documentation(
        "MAMFin",
        "This table has the following fields:\n\n"
        "- `query_id`: string (Optional)\n"
        "- `complete`: boolean (Optional)\n"
        "- `stable`: boolean (Optional)\n"
        "- `result_set`: @{ResultSet} (Optional)\n"
    );
}
