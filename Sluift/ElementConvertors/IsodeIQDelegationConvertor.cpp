/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/IsodeIQDelegationConvertor.h>

#include <boost/smart_ptr/make_shared.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

IsodeIQDelegationConvertor::IsodeIQDelegationConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<IsodeIQDelegation>("isode_iq_delegation"),
		convertors(convertors) {
}

IsodeIQDelegationConvertor::~IsodeIQDelegationConvertor() {
}

boost::shared_ptr<IsodeIQDelegation> IsodeIQDelegationConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<IsodeIQDelegation> result = boost::make_shared<IsodeIQDelegation>();
	lua_getfield(L, -1, "forward");
	if (!lua_isnil(L, -1)) {
		if (boost::shared_ptr<Forwarded> payload = boost::dynamic_pointer_cast<Forwarded>(convertors->convertFromLuaUntyped(L, -1, "forwarded"))) {
			result->setForward(payload);
		}
	}
	lua_pop(L, 1);
	return result;
}

void IsodeIQDelegationConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<IsodeIQDelegation> payload) {
	lua_createtable(L, 0, 0);
	if (convertors->convertToLuaUntyped(L, payload->getForward()) > 0) {
		lua_setfield(L, -2, "forward");
	}
}

boost::optional<LuaElementConvertor::Documentation> IsodeIQDelegationConvertor::getDocumentation() const {
	return Documentation(
		"IsodeIQDelegation",
		"This table has the following fields:\n\n"
		"- `forward`: @{Forwarded}\n"
	);
}
