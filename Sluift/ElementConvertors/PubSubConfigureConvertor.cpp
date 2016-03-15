/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubConfigureConvertor.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubConfigureConvertor::PubSubConfigureConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubConfigure>("pubsub_configure"),
		convertors(convertors) {
}

PubSubConfigureConvertor::~PubSubConfigureConvertor() {
}

boost::shared_ptr<PubSubConfigure> PubSubConfigureConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubConfigure> result = boost::make_shared<PubSubConfigure>();
	lua_getfield(L, -1, "data");
	if (!lua_isnil(L, -1)) {
		if (boost::shared_ptr<Form> payload = boost::dynamic_pointer_cast<Form>(convertors->convertFromLuaUntyped(L, -1, "form"))) {
			result->setData(payload);
		}
	}
	lua_pop(L, 1);
	return result;
}

void PubSubConfigureConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubConfigure> payload) {
	lua_createtable(L, 0, 0);
	if (convertors->convertToLuaUntyped(L, payload->getData()) > 0) {
		lua_setfield(L, -2, "data");
	}
}

boost::optional<LuaElementConvertor::Documentation> PubSubConfigureConvertor::getDocumentation() const {
	return Documentation(
		"PubSubConfigure",
		"This table has the following fields:\n\n"
		"- `data`: @{Form}\n"
	);
}
