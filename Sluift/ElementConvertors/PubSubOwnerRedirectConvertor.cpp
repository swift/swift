/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerRedirectConvertor.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <lua.hpp>

using namespace Swift;

PubSubOwnerRedirectConvertor::PubSubOwnerRedirectConvertor() :
		GenericLuaElementConvertor<PubSubOwnerRedirect>("pubsub_owner_redirect") {
}

PubSubOwnerRedirectConvertor::~PubSubOwnerRedirectConvertor() {
}

boost::shared_ptr<PubSubOwnerRedirect> PubSubOwnerRedirectConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubOwnerRedirect> result = boost::make_shared<PubSubOwnerRedirect>();
	lua_getfield(L, -1, "uri");
	if (lua_isstring(L, -1)) {
		result->setURI(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void PubSubOwnerRedirectConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubOwnerRedirect> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getURI().c_str());
	lua_setfield(L, -2, "uri");
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerRedirectConvertor::getDocumentation() const {
	return Documentation(
		"PubSubOwnerRedirect",
		"This table has the following fields:\n\n"
		"- `uri`: string\n"
	);
}
