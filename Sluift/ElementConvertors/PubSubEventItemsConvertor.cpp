/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventItemsConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include <Sluift/LuaElementConvertors.h>
#include <Swiften/Base/foreach.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

PubSubEventItemsConvertor::PubSubEventItemsConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<PubSubEventItems>("pubsub_event_items"),
		convertors(convertors) {
}

PubSubEventItemsConvertor::~PubSubEventItemsConvertor() {
}

boost::shared_ptr<PubSubEventItems> PubSubEventItemsConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<PubSubEventItems> result = boost::make_shared<PubSubEventItems>();
	lua_getfield(L, -1, "node");
	if (lua_isstring(L, -1)) {
		result->setNode(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "items");
	if (lua_type(L, -1) == LUA_TTABLE) {
		std::vector< boost::shared_ptr<PubSubEventItem> > items;
		for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
			lua_pushnumber(L, i + 1);
			lua_gettable(L, -2);
			if (!lua_isnil(L, -1)) {
				if (boost::shared_ptr<PubSubEventItem> payload = boost::dynamic_pointer_cast<PubSubEventItem>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_item"))) {
					items.push_back(payload);
				}
			}
			lua_pop(L, 1);
		}

		result->setItems(items);
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "retracts");
	if (lua_type(L, -1) == LUA_TTABLE) {
		std::vector< boost::shared_ptr<PubSubEventRetract> > items;
		for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
			lua_pushnumber(L, i + 1);
			lua_gettable(L, -2);
			if (!lua_isnil(L, -1)) {
				if (boost::shared_ptr<PubSubEventRetract> payload = boost::dynamic_pointer_cast<PubSubEventRetract>(convertors->convertFromLuaUntyped(L, -1, "pubsub_event_retract"))) {
					items.push_back(payload);
				}
			}
			lua_pop(L, 1);
		}

		result->setRetracts(items);
	}
	lua_pop(L, 1);
	return result;
}

void PubSubEventItemsConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<PubSubEventItems> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getNode().c_str());
	lua_setfield(L, -2, "node");
	if (!payload->getItems().empty()) {
		lua_createtable(L, boost::numeric_cast<int>(payload->getItems().size()), 0);
		{
			int i = 0;
			foreach(boost::shared_ptr<PubSubEventItem> item, payload->getItems()) {
				if (convertors->convertToLuaUntyped(L, item) > 0) {
					lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
					++i;
				}
			}
		}
		lua_setfield(L, -2, "items");
	}
	if (!payload->getRetracts().empty()) {
		lua_createtable(L, boost::numeric_cast<int>(payload->getRetracts().size()), 0);
		{
			int i = 0;
			foreach(boost::shared_ptr<PubSubEventRetract> item, payload->getRetracts()) {
				if (convertors->convertToLuaUntyped(L, item) > 0) {
					lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
					++i;
				}
			}
		}
		lua_setfield(L, -2, "retracts");
	}
}

boost::optional<LuaElementConvertor::Documentation> PubSubEventItemsConvertor::getDocumentation() const {
	return Documentation(
		"PubSubEventItems",
		"This table has the following fields:\n\n"
		"- `node`: string\n"
		"- `items`: array<@{PubSubEventItem}>\n"
		"- `retracts`: array<@{PubSubEventRetract}>\n"
	);
}
