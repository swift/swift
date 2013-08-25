/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubDefaultConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubDefaultConvertor::PubSubDefaultConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubDefault>("pubsub_default"),
		convertors(convertors) {
}

PubSubDefaultConvertor::~PubSubDefaultConvertor() {
}

boost::shared_ptr<PubSubDefault> PubSubDefaultConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubDefault> result = boost::make_shared<PubSubDefault>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "type");
	lua_pop(L, 1);
	return result;
}

void PubSubDefaultConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubDefault> payload) {
	lua_createtable(L, 0, 0);
	if (payload->getNode()) {
		lua_pushstring(L, (*payload->getNode()).c_str());
		lua_setfield(L, -2, "node");
	}
	switch (payload->getType()) {
		case PubSubDefault::None:
			lua_pushstring(L, "none");
			break;
		case PubSubDefault::Collection:
			lua_pushstring(L, "collection");
			break;
		case PubSubDefault::Leaf:
			lua_pushstring(L, "leaf");
			break;
	}
	lua_setfield(L, -2, "type");
}
