/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubSubscriptionsConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Sluift/LuaElementConvertors.h>
#include <Swiften/Base/foreach.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubSubscriptionsConvertor::PubSubSubscriptionsConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubSubscriptions>("pubsub_subscriptions"),
		convertors(convertors) {
}

PubSubSubscriptionsConvertor::~PubSubSubscriptionsConvertor() {
}

boost::shared_ptr<PubSubSubscriptions> PubSubSubscriptionsConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubSubscriptions> result = boost::make_shared<PubSubSubscriptions>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	if (lua_type(L, -1) == LUA_TTABLE) {
		std::vector< boost::shared_ptr<PubSubSubscription> > items;
		for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
			lua_pushnumber(L, i + 1);
			lua_gettable(L, -2);
			if (!lua_isnil(L, -1)) {
				if (boost::shared_ptr<PubSubSubscription> payload = boost::dynamic_pointer_cast<PubSubSubscription>(convertors->convertFromLuaUntyped(L, -1, "pubsub_subscription"))) {
					items.push_back(payload);
				}
			}
			lua_pop(L, 1);
		}

		result->setSubscriptions(items);
	}
	return result;
}

void PubSubSubscriptionsConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubSubscriptions> payload) {
	lua_createtable(L, 0, 0);
	if (payload->getNode()) {
		lua_pushstring(L, (*payload->getNode()).c_str());
		lua_setfield(L, -2, "node");
	}
	if (!payload->getSubscriptions().empty()) {
		{
			int i = 0;
			foreach(boost::shared_ptr<PubSubSubscription> item, payload->getSubscriptions()) {
				if (convertors->convertToLuaUntyped(L, item) > 0) {
					lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
					++i;
				}
			}
		}
	}
}
