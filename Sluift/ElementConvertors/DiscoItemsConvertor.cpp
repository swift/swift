/*
 * Copyright (c) 2013 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/DiscoItemsConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <Sluift/Lua/LuaUtils.h>

using namespace Swift;

DiscoItemsConvertor::DiscoItemsConvertor() : GenericLuaElementConvertor<DiscoItems>("disco_items") {
}

DiscoItemsConvertor::~DiscoItemsConvertor() {
}

boost::shared_ptr<DiscoItems> DiscoItemsConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<DiscoItems> result = boost::make_shared<DiscoItems>();
	if (boost::optional<std::string> value = Lua::getStringField(L, -1, "node")) {
		result->setNode(*value);
	}
	lua_getfield(L, -1, "items");
	if (lua_istable(L, -1)) {
		for (lua_pushnil(L); lua_next(L, -2); ) {
			result->addItem(DiscoItems::Item(
					Lua::getStringField(L, -1, "name").get_value_or(""),
					JID(Lua::getStringField(L, -1, "jid").get_value_or("")),
					Lua::getStringField(L, -1, "node").get_value_or("")));
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
	return result;
}

void DiscoItemsConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<DiscoItems> payload) {
	lua_newtable(L);
	if (!payload->getNode().empty()) {
		lua_pushstring(L, payload->getNode().c_str());
		lua_setfield(L, -2, "node");
	}
	const std::vector<DiscoItems::Item>& items = payload->getItems();
	if (!items.empty()) {
		lua_createtable(L, boost::numeric_cast<int>(items.size()), 0);
		for (size_t i = 0; i < items.size(); ++i) {
			lua_createtable(L, 0, 0);
			if (!items[i].getName().empty()) {
				lua_pushstring(L, items[i].getName().c_str());
				lua_setfield(L, -2, "name");
			}
			if (!items[i].getNode().empty()) {
				lua_pushstring(L, items[i].getNode().c_str());
				lua_setfield(L, -2, "node");
			}
			if (items[i].getJID().isValid()) {
				lua_pushstring(L, items[i].getJID().toString().c_str());
				lua_setfield(L, -2, "jid");
			}
			lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
		}
		lua_setfield(L, -2, "items");
	}
}
