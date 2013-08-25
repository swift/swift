/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventAssociateConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubEventAssociateConvertor::PubSubEventAssociateConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubEventAssociate>("pubsub_event_associate"),
		convertors(convertors) {
}

PubSubEventAssociateConvertor::~PubSubEventAssociateConvertor() {
}

boost::shared_ptr<PubSubEventAssociate> PubSubEventAssociateConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubEventAssociate> result = boost::make_shared<PubSubEventAssociate>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void PubSubEventAssociateConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubEventAssociate> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getNode().c_str());
	lua_setfield(L, -2, "node");
}
