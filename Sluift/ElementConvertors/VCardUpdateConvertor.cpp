/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/VCardUpdateConvertor.h>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <lua.hpp>

#include <Sluift/Lua/LuaUtils.h>

using namespace Swift;

VCardUpdateConvertor::VCardUpdateConvertor() : GenericLuaElementConvertor<VCardUpdate>("vcard_update") {
}

VCardUpdateConvertor::~VCardUpdateConvertor() {
}

boost::shared_ptr<VCardUpdate> VCardUpdateConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<VCardUpdate> result = boost::make_shared<VCardUpdate>();
	if (boost::optional<std::string> value = Lua::getStringField(L, -1, "photo_hash")) {
		result->setPhotoHash(*value);
	}
	return result;
}

void VCardUpdateConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<VCardUpdate> payload) {
	lua_newtable(L);
	if (!payload->getPhotoHash().empty()) {
		lua_pushstring(L, payload->getPhotoHash().c_str());
		lua_setfield(L, -2, "photo_hash");
	}
}
