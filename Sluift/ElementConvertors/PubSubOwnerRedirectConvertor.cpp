/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerRedirectConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubOwnerRedirectConvertor::PubSubOwnerRedirectConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubOwnerRedirect>("pubsub_owner_redirect"),
		convertors(convertors) {
}

PubSubOwnerRedirectConvertor::~PubSubOwnerRedirectConvertor() {
}

boost::shared_ptr<PubSubOwnerRedirect> PubSubOwnerRedirectConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubOwnerRedirect> result = boost::make_shared<PubSubOwnerRedirect>();
	lua_getfield(L, -1, "uri");
	if (lua_isstring(L, -1)) {
		result->setURI(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void PubSubOwnerRedirectConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubOwnerRedirect> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getURI().c_str());
	lua_setfield(L, -2, "uri");
}
