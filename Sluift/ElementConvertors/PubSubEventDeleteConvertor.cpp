/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventDeleteConvertor.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubEventDeleteConvertor::PubSubEventDeleteConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubEventDelete>("pubsub_event_delete"),
		convertors(convertors) {
}

PubSubEventDeleteConvertor::~PubSubEventDeleteConvertor() {
}

boost::shared_ptr<PubSubEventDelete> PubSubEventDeleteConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubEventDelete> result = boost::make_shared<PubSubEventDelete>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "redirects");
	if (!lua_isnil(L, -1)) {
		if (boost::shared_ptr<PubSubEventRedirect> payload = boost::dynamic_pointer_cast<PubSubEventRedirect>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_redirect"))) {
			result->setRedirects(payload);
		}
	}
	lua_pop(L, 1);
	return result;
}

void PubSubEventDeleteConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubEventDelete> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getNode().c_str());
	lua_setfield(L, -2, "node");
	if (convertors->convertToLuaUntyped(L, payload->getRedirects()) > 0) {
		lua_setfield(L, -2, "redirects");
	}
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventDeleteConvertor::getDocumentation() const {
	return Documentation(
		"PubSubEventDelete",
		"This table has the following fields:\n\n"
		"- `node`: string\n"
		"- `redirects`: @{PubSubEventRedirect}\n"
	);
}
