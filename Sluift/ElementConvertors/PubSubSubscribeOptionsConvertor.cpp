/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubSubscribeOptionsConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>




#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubSubscribeOptionsConvertor::PubSubSubscribeOptionsConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubSubscribeOptions>("pubsub_subscribe_options"),
		convertors(convertors) {
}

PubSubSubscribeOptionsConvertor::~PubSubSubscribeOptionsConvertor() {
}

boost::shared_ptr<PubSubSubscribeOptions> PubSubSubscribeOptionsConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubSubscribeOptions> result = boost::make_shared<PubSubSubscribeOptions>();
	lua_getfield(L, -1, "required");
	if (lua_isboolean(L, -1)) {
		result->setRequired(lua_toboolean(L, -1));
	}
	lua_pop(L, 1);
	return result;
}

void PubSubSubscribeOptionsConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubSubscribeOptions> payload) {
	lua_createtable(L, 0, 0);
	lua_pushboolean(L, payload->isRequired());
	lua_setfield(L, -2, "required");
}

boost::optional<LuaElementConvertor::Documentation> PubSubSubscribeOptionsConvertor::getDocumentation() const {
	return Documentation(
		"PubSubSubscribeOptions",
		"This table has the following fields:\n\n"
		"- `required`: boolean\n"
	);
}
