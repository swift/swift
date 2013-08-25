/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerDefaultConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>


#include <Sluift/LuaElementConvertors.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubOwnerDefaultConvertor::PubSubOwnerDefaultConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubOwnerDefault>("pubsub_owner_default"),
		convertors(convertors) {
}

PubSubOwnerDefaultConvertor::~PubSubOwnerDefaultConvertor() {
}

boost::shared_ptr<PubSubOwnerDefault> PubSubOwnerDefaultConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubOwnerDefault> result = boost::make_shared<PubSubOwnerDefault>();
	lua_getfield(L, -1, "data");
	if (!lua_isnil(L, -1)) {
		if (boost::shared_ptr<Form> payload = boost::dynamic_pointer_cast<Form>(convertors->convertFromLuaUntyped(L, -1, "form"))) {
			result->setData(payload);
		}
	}
	lua_pop(L, 1);
	return result;
}

void PubSubOwnerDefaultConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubOwnerDefault> payload) {
	lua_createtable(L, 0, 0);
	if (convertors->convertToLuaUntyped(L, payload->getData()) > 0) {
		lua_setfield(L, -2, "data");
	}
}
