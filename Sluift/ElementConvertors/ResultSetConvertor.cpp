/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/numeric/conversion/cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <lua.hpp>
#include <Sluift/ElementConvertors/ResultSetConvertor.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

ResultSetConvertor::ResultSetConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<ResultSet>("result_set"),
		convertors(convertors) {
}

ResultSetConvertor::~ResultSetConvertor() {
}

boost::shared_ptr<ResultSet> ResultSetConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<ResultSet> result = boost::make_shared<ResultSet>();
	lua_getfield(L, -1, "max_items");
	if (lua_isstring(L, -1)) {
		result->setMaxItems(boost::numeric_cast<int>(lua_tonumber(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "count");
	if (lua_isnumber(L, -1)) {
		result->setCount(boost::numeric_cast<int>(lua_tonumber(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "first_id_index");
	if (lua_isstring(L, -1)) {
		result->setFirstIDIndex(boost::numeric_cast<int>(lua_tonumber(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "first_id");
	if (lua_isstring(L, -1)) {
		result->setFirstID(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "last_id");
	if (lua_isstring(L, -1)) {
		result->setLastID(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "after");
	if (lua_isstring(L, -1)) {
		result->setAfter(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void ResultSetConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<ResultSet> payload) {
	lua_createtable(L, 0, 0);
	if (payload->getMaxItems()) {
		lua_pushnumber(L, *payload->getMaxItems());
		lua_setfield(L, -2, "max_items");
	}
	if (payload->getCount()) {
		lua_pushnumber(L, *payload->getCount());
		lua_setfield(L, -2, "count");
	}
	if (payload->getFirstIDIndex()) {
		lua_pushnumber(L, *payload->getFirstIDIndex());
		lua_setfield(L, -2, "first_id_index");
	}
	if (payload->getFirstID()) {
		lua_pushstring(L, (*payload->getFirstID()).c_str());
		lua_setfield(L, -2, "first_id");
	}
	if (payload->getLastID()) {
		lua_pushstring(L, (*payload->getLastID()).c_str());
		lua_setfield(L, -2, "last_id");
	}
	if (payload->getAfter()) {
		lua_pushstring(L, (*payload->getAfter()).c_str());
		lua_setfield(L, -2, "after");
	}
}

boost::optional<LuaElementConvertor::Documentation> ResultSetConvertor::getDocumentation() const {
	return Documentation(
		"ResultSet",
		"This table has the following fields:\n\n"
		"- `max_items`: number (Optional)\n"
		"- `count`: number (Optional)\n"
		"- `first_id_index`: number (Optional)\n"
		"- `first_id`: string (Optional)\n"
		"- `last_id`: string (Optional)\n"
		"- `after`: string (Optional)\n"
	);
}
