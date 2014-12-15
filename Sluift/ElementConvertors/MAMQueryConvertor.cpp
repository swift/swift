/*
 * Copyright (c) 2014 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <boost/numeric/conversion/cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <lua.hpp>
#include <Sluift/ElementConvertors/MAMQueryConvertor.h>
#include <Sluift/LuaElementConvertors.h>
#include <Swiften/Elements/Form.h>
#include <Swiften/Elements/ResultSet.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

MAMQueryConvertor::MAMQueryConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<MAMQuery>("mam"),
		convertors(convertors) {
}

MAMQueryConvertor::~MAMQueryConvertor() {
}

boost::shared_ptr<MAMQuery> MAMQueryConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<MAMQuery> result = boost::make_shared<MAMQuery>();
	lua_getfield(L, -1, "query_id");
	if (lua_isstring(L, -1)) {
		result->setQueryID(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "form");
	if (!lua_isnil(L, -1)) {
		boost::shared_ptr<Form> form = boost::dynamic_pointer_cast<Form>(convertors->convertFromLuaUntyped(L, -1, "form"));
		if (!!form) {
			result->setForm(form);
		}
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "result_set");
	if (!lua_isnil(L, -1)) {
		boost::shared_ptr<ResultSet> resultSet = boost::dynamic_pointer_cast<ResultSet>(convertors->convertFromLuaUntyped(L, -1, "result_set"));
		if (!!resultSet) {
			result->setResultSet(resultSet);
		}
	}
	lua_pop(L, 1);
	return result;
}

void MAMQueryConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<MAMQuery> payload) {
	lua_createtable(L, 0, 0);
	if (payload->getQueryID()) {
		lua_pushstring(L, (*payload->getQueryID()).c_str());
		lua_setfield(L, -2, "query_id");
	}
	if (payload->getNode()) {
		lua_pushstring(L, (*payload->getNode()).c_str());
		lua_setfield(L, -2, "node");
	}
	if (convertors->convertToLuaUntyped(L, payload->getForm()) > 0) {
		lua_setfield(L, -2, "form");
	}
	if (convertors->convertToLuaUntyped(L, payload->getResultSet()) > 0) {
		lua_setfield(L, -2, "result_set");
	}
}

boost::optional<LuaElementConvertor::Documentation> MAMQueryConvertor::getDocumentation() const {
	return Documentation(
		"MAMQuery",
		"This table has the following fields:\n\n"
		"- `query_id`: string (Optional)\n"
		"- `node`: string (Optional)\n"
		"- `form`: string @{Form} (Optional)\n"
		"- `result_set`: @{ResultSet} (Optional)\n"
	);
}
