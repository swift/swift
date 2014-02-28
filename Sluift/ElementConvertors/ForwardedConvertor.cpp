/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/numeric/conversion/cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <lua.hpp>
#include <Swiften/Base/foreach.h>
#include <Sluift/ElementConvertors/ForwardedConvertor.h>
#include <Sluift/LuaElementConvertors.h>
#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Presence.h>
#include <Swiften/Elements/Message.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

ForwardedConvertor::ForwardedConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<Forwarded>("forwarded"),
		convertors(convertors) {
}

ForwardedConvertor::~ForwardedConvertor() {
}

boost::shared_ptr<Forwarded> ForwardedConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<Forwarded> result = boost::make_shared<Forwarded>();
	lua_getfield(L, -1, "delay");
	if (!lua_isnil(L, -1)) {
		boost::shared_ptr<Delay> delay = boost::dynamic_pointer_cast<Delay>(convertors->convertFromLuaUntyped(L, -1, "delay"));
		if (!!delay) {
			result->setDelay(delay);
		}
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "stanza");
	if (!lua_isnil(L, -1)) {
		boost::shared_ptr<Stanza> stanza = boost::dynamic_pointer_cast<Stanza>(convertors->convertFromLua(L, -1));
		if (!!stanza) {
			result->setStanza(stanza);
		}
		lua_pop(L, 1);
		return result;
	}
	return result;
}

void ForwardedConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<Forwarded> payload) {
	lua_createtable(L, 0, 0);
	if (convertors->convertToLuaUntyped(L, payload->getDelay()) > 0) {
		lua_setfield(L, -2, "delay");
	}
	boost::shared_ptr<Stanza> stanza = payload->getStanza();
	if (!!stanza) {
		if (convertors->convertToLua(L, stanza) > 0) {
			lua_setfield(L, -2, "stanza");
		}
	}
}

boost::optional<LuaElementConvertor::Documentation> ForwardedConvertor::getDocumentation() const {
	return Documentation(
		"Forwarded",
		"This table has the following fields:\n\n"
		"- `delay`: @{Delay} (Optional)\n"
		"- `stanza`: @{Stanza} (Optional)\n"
	);
}
