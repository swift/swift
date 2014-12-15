/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/DiscoInfoConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <Sluift/Lua/LuaUtils.h>

using namespace Swift;

DiscoInfoConvertor::DiscoInfoConvertor() : GenericLuaElementConvertor<DiscoInfo>("disco_info") {
}

DiscoInfoConvertor::~DiscoInfoConvertor() {
}

boost::shared_ptr<DiscoInfo> DiscoInfoConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<DiscoInfo> result = boost::make_shared<DiscoInfo>();
	if (boost::optional<std::string> value = Lua::getStringField(L, -1, "node")) {
		result->setNode(*value);
	}

	lua_getfield(L, -1, "identities");
	if (lua_istable(L, -1)) {
		for (lua_pushnil(L); lua_next(L, -2); ) {
			result->addIdentity(DiscoInfo::Identity(
					Lua::getStringField(L, -1, "name").get_value_or(""),
					Lua::getStringField(L, -1, "category").get_value_or("client"),
					Lua::getStringField(L, -1, "type").get_value_or("pc"),
					Lua::getStringField(L, -1, "language").get_value_or("")));
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);

	lua_getfield(L, -1, "features");
	if (lua_istable(L, -1)) {
		for (lua_pushnil(L); lua_next(L, -2); ) {
			if (lua_isstring(L, -1)) {
				result->addFeature(lua_tostring(L, -1));
			}
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);

	// TODO: Extension

	return result;
}

void DiscoInfoConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<DiscoInfo> payload) {
	lua_newtable(L);
	if (!payload->getNode().empty()) {
		lua_pushstring(L, payload->getNode().c_str());
		lua_setfield(L, -2, "node");
	}

	const std::vector<DiscoInfo::Identity>& identities = payload->getIdentities();
	if (!identities.empty()) {
		lua_createtable(L, boost::numeric_cast<int>(identities.size()), 0);
		for (size_t i = 0; i < identities.size(); ++i) {
			lua_createtable(L, 0, 0);
			if (!identities[i].getName().empty()) {
				lua_pushstring(L, identities[i].getName().c_str());
				lua_setfield(L, -2, "name");
			}
			if (!identities[i].getCategory().empty()) {
				lua_pushstring(L, identities[i].getCategory().c_str());
				lua_setfield(L, -2, "category");
			}
			if (!identities[i].getType().empty()) {
				lua_pushstring(L, identities[i].getType().c_str());
				lua_setfield(L, -2, "type");
			}
			if (!identities[i].getLanguage().empty()) {
				lua_pushstring(L, identities[i].getLanguage().c_str());
				lua_setfield(L, -2, "language");
			}
			lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
		}
		lua_setfield(L, -2, "identities");
	}

	const std::vector<std::string>& features = payload->getFeatures();
	if (!features.empty()) {
		lua_createtable(L, boost::numeric_cast<int>(features.size()), 0);
		for (size_t i = 0; i < features.size(); ++i) {
			lua_pushstring(L, features[i].c_str());
			lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
		}
		lua_setfield(L, -2, "features");
	}
	
	// TODO: Extension
}

boost::optional<LuaElementConvertor::Documentation> DiscoInfoConvertor::getDocumentation() const {
	return Documentation(
			"DiscoInfo", 
			"Represents `disco#info` service discovery data.\n\n"
			"This table has the following structure:\n\n"
			"- `node`: string\n"
			"- `identities`: array(table)\n"
			"  - `name`: string\n"
			"  - `category`: string\n"
			"  - `type`: string\n"
			"  - `language`: string\n"
			"- `features`: array(string)\n"
	);
}
