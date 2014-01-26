/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventRetractConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubEventRetractConvertor::PubSubEventRetractConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubEventRetract>("pubsub_event_retract"),
		convertors(convertors) {
}

PubSubEventRetractConvertor::~PubSubEventRetractConvertor() {
}

boost::shared_ptr<PubSubEventRetract> PubSubEventRetractConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubEventRetract> result = boost::make_shared<PubSubEventRetract>();
	lua_getfield(L, -1, "id");
	if (lua_isstring(L, -1)) {
		result->setID(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void PubSubEventRetractConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubEventRetract> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getID().c_str());
	lua_setfield(L, -2, "id");
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventRetractConvertor::getDocumentation() const {
	return Documentation(
		"PubSubEventRetract",
		"This table has the following fields:\n\n"
		"- `id`: string\n"
	);
}
