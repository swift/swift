/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <boost/numeric/conversion/cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <lua.hpp>
#include <Sluift/ElementConvertors/MAMArchivedConvertor.h>

#pragma clang diagnostic ignored "-Wunused-private-field"

using namespace Swift;

MAMArchivedConvertor::MAMArchivedConvertor(LuaElementConvertors* convertors) : 
		GenericLuaElementConvertor<MAMArchived>("mam_archived"),
		convertors(convertors) {
}

MAMArchivedConvertor::~MAMArchivedConvertor() {
}

boost::shared_ptr<MAMArchived> MAMArchivedConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<MAMArchived> result = boost::make_shared<MAMArchived>();
	lua_getfield(L, -1, "by");
	if (lua_isstring(L, -1)) {
		result->setBy(JID(std::string(lua_tostring(L, -1))));
	}
	lua_pop(L, 1);
	lua_getfield(L, -1, "id");
	if (lua_isstring(L, -1)) {
		result->setID(std::string(lua_tostring(L, -1)));
	}
	lua_pop(L, 1);
	return result;
}

void MAMArchivedConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<MAMArchived> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getBy().toString().c_str());
	lua_setfield(L, -2, "by");
	lua_pushstring(L, payload->getID().c_str());
	lua_setfield(L, -2, "id");
}

boost::optional<LuaElementConvertor::Documentation> MAMArchivedConvertor::getDocumentation() const {
	return Documentation(
		"MAMArchived",
		"This table has the following fields:\n\n"
		"- `by`: string\n"
		"- `id`: string\n"
	);
}
