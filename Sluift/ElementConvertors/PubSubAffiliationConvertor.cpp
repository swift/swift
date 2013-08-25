/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubAffiliationConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubAffiliationConvertor::PubSubAffiliationConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubAffiliation>("pubsub_affiliation"),
		convertors(convertors) {
}

PubSubAffiliationConvertor::~PubSubAffiliationConvertor() {
}

boost::shared_ptr<PubSubAffiliation> PubSubAffiliationConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubAffiliation> result = boost::make_shared<PubSubAffiliation>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "type");
	lua_pop(L, 1);
	return result;
}

void PubSubAffiliationConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubAffiliation> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getNode().c_str());
	lua_setfield(L, -2, "node");
	switch (payload->getType()) {
		case PubSubAffiliation::None:
			lua_pushstring(L, "none");
			break;
		case PubSubAffiliation::Member:
			lua_pushstring(L, "member");
			break;
		case PubSubAffiliation::Outcast:
			lua_pushstring(L, "outcast");
			break;
		case PubSubAffiliation::Owner:
			lua_pushstring(L, "owner");
			break;
		case PubSubAffiliation::Publisher:
			lua_pushstring(L, "publisher");
			break;
		case PubSubAffiliation::PublishOnly:
			lua_pushstring(L, "publish_only");
			break;
	}
	lua_setfield(L, -2, "type");
}
