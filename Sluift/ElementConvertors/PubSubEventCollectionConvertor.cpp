/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventCollectionConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>


#include <Sluift/LuaElementConvertors.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubEventCollectionConvertor::PubSubEventCollectionConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubEventCollection>("pubsub_event_collection"),
		convertors(convertors) {
}

PubSubEventCollectionConvertor::~PubSubEventCollectionConvertor() {
}

boost::shared_ptr<PubSubEventCollection> PubSubEventCollectionConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubEventCollection> result = boost::make_shared<PubSubEventCollection>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "disassociate");
	if (!lua_isnil(L, -1)) {
		if (boost::shared_ptr<PubSubEventDisassociate> payload = boost::dynamic_pointer_cast<PubSubEventDisassociate>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_disassociate"))) {
			result->setDisassociate(payload);
		}
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "associate");
	if (!lua_isnil(L, -1)) {
		if (boost::shared_ptr<PubSubEventAssociate> payload = boost::dynamic_pointer_cast<PubSubEventAssociate>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_associate"))) {
			result->setAssociate(payload);
		}
	}
	lua_pop(L, 1);
	return result;
}

void PubSubEventCollectionConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubEventCollection> payload) {
	lua_createtable(L, 0, 0);
	if (payload->getNode()) {
		lua_pushstring(L, (*payload->getNode()).c_str());
		lua_setfield(L, -2, "node");
	}
	if (convertors->convertToLuaUntyped(L, payload->getDisassociate()) > 0) {
		lua_setfield(L, -2, "disassociate");
	}
	if (convertors->convertToLuaUntyped(L, payload->getAssociate()) > 0) {
		lua_setfield(L, -2, "associate");
	}
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventCollectionConvertor::getDocumentation() const {
	return Documentation(
		"PubSubEventCollection",
		"This table has the following fields:\n\n"
		"- `node`: string (Optional)\n"
		"- `disassociate`: @{PubSubEventDisassociate}\n"
		"- `associate`: @{PubSubEventAssociate}\n"
	);
}
