/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/numeric/conversion/cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <lua.hpp>
#include <Sluift/ElementConvertors/MAMResultConvertor.h>
#include <Sluift/LuaElementConvertors.h>
#include <Swiften/Elements/Forwarded.h>


#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

MAMResultConvertor::MAMResultConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<MAMResult>("mam_result"),
		convertors(convertors) {
}

MAMResultConvertor::~MAMResultConvertor() {
}

boost::shared_ptr<MAMResult> MAMResultConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<MAMResult> result = boost::make_shared<MAMResult>();
	lua_getfield(L, -1, "payload");
	if (!lua_isnil(L, -1)) {
		boost::shared_ptr<Forwarded> payload = boost::dynamic_pointer_cast<Forwarded>(convertors->convertFromLuaUntyped(L, -1, "payload"));
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

void MAMResultConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<MAMResult> payload) {
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
