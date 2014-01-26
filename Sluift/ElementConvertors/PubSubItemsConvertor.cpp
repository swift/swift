/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Sluift/ElementConvertors/PubSubItemsConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Sluift/LuaElementConvertors.h>
#include <Swiften/Base/foreach.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubItemsConvertor::PubSubItemsConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubItems>("pubsub_items"),
		convertors(convertors) {
}

PubSubItemsConvertor::~PubSubItemsConvertor() {
}

boost::shared_ptr<PubSubItems> PubSubItemsConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubItems> result = boost::make_shared<PubSubItems>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	if (lua_type(L, -1) == LUA_TTABLE) {
		std::vector< boost::shared_ptr<PubSubItem> > items;
		for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
			lua_pushnumber(L, i + 1);
			lua_gettable(L, -2);
			if (!lua_isnil(L, -1)) {
				if (boost::shared_ptr<PubSubItem> payload = boost::dynamic_pointer_cast<PubSubItem>(convertors->convertFromLuaUntyped(L, -1, "pubsub_item"))) {
					items.push_back(payload);
				}
			}
			lua_pop(L, 1);
		}

		result->setItems(items);
	}
	lua_getfield(L, -1, "maximum_items");
	if (lua_isnumber(L, -1)) {
		result->setMaximumItems(boost::numeric_cast<unsigned int>(lua_tonumber(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "subscription_id");
	if (lua_isstring(L, -1)) {
		result->setSubscriptionID(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void PubSubItemsConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubItems> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getNode().c_str());
	lua_setfield(L, -2, "node");
	if (!payload->getItems().empty()) {
		{
			int i = 0;
			foreach(boost::shared_ptr<PubSubItem> item, payload->getItems()) {
				if (convertors->convertToLuaUntyped(L, item) > 0) {
					lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
					++i;
				}
			}
		}
	}
	if (payload->getMaximumItems()) {
		lua_pushnumber(L, (*payload->getMaximumItems()));
		lua_setfield(L, -2, "maximum_items");
	}
	if (payload->getSubscriptionID()) {
		lua_pushstring(L, (*payload->getSubscriptionID()).c_str());
		lua_setfield(L, -2, "subscription_id");
	}
}

boost::optional<LuaElementConvertor::Documentation> PubSubItemsConvertor::getDocumentation() const {
	return Documentation(
		"PubSubItems",
		"This table has the following fields:\n\n"
		"- `node`: string\n"
		"- `items`: array<@{PubSubItem}>\n"
		"- `maximum_items`: number (Optional)\n"
		"- `subscription_id`: string (Optional)\n"
	);
}
