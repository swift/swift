/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventDisassociateConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubEventDisassociateConvertor::PubSubEventDisassociateConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubEventDisassociate>("pubsub_event_disassociate"),
		convertors(convertors) {
}

PubSubEventDisassociateConvertor::~PubSubEventDisassociateConvertor() {
}

boost::shared_ptr<PubSubEventDisassociate> PubSubEventDisassociateConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubEventDisassociate> result = boost::make_shared<PubSubEventDisassociate>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void PubSubEventDisassociateConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubEventDisassociate> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getNode().c_str());
	lua_setfield(L, -2, "node");
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventDisassociateConvertor::getDocumentation() const {
	return Documentation(
		"PubSubEventDisassociate",
		"This table has the following fields:\n\n"
		"- `node`: string\n"
	);
}
