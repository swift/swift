/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventRedirectConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubEventRedirectConvertor::PubSubEventRedirectConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubEventRedirect>("pubsub_event_redirect"),
		convertors(convertors) {
}

PubSubEventRedirectConvertor::~PubSubEventRedirectConvertor() {
}

boost::shared_ptr<PubSubEventRedirect> PubSubEventRedirectConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubEventRedirect> result = boost::make_shared<PubSubEventRedirect>();
	lua_getfield(L, -1, "uri");
	if (lua_isstring(L, -1)) {
		result->setURI(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void PubSubEventRedirectConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubEventRedirect> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getURI().c_str());
	lua_setfield(L, -2, "uri");
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventRedirectConvertor::getDocumentation() const {
	return Documentation(
		"PubSubEventRedirect",
		"This table has the following fields:\n\n"
		"- `uri`: string\n"
	);
}
