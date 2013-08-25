/*
 * Copyright (c) 2013 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/VCardConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <Sluift/Lua/LuaUtils.h>
#include <Swiften/Base/ByteArray.h>

using namespace Swift;

VCardConvertor::VCardConvertor() : GenericLuaElementConvertor<VCard>("vcard") {
}

VCardConvertor::~VCardConvertor() {
}

boost::shared_ptr<VCard> VCardConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<VCard> result = boost::make_shared<VCard>();
	lua_getfield(L, -1, "photo");
	if (lua_isstring(L, -1)) {
		size_t len;
		const char* data = lua_tolstring(L, -1, &len);
		result->setPhoto(createByteArray(data, len));
	}
	lua_pop(L, 1);

	// TODO

	return result;
}

void VCardConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<VCard> payload) {
	lua_newtable(L);
	if (!payload->getPhoto().empty()) {
		lua_pushlstring(L, 
				reinterpret_cast<const char*>(vecptr(payload->getPhoto())), 
				payload->getPhoto().size());
		lua_setfield(L, -2, "photo");
	}
}
