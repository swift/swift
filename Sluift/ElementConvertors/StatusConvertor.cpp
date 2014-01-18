/*
 * Copyright (c) 2014 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/StatusConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <Sluift/Lua/Check.h>

using namespace Swift;

StatusConvertor::StatusConvertor() : GenericLuaElementConvertor<Status>("status") {
}

StatusConvertor::~StatusConvertor() {
}

boost::shared_ptr<Status> StatusConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<Status> result = boost::make_shared<Status>();
	lua_getfield(L, -1, "text");
	if (lua_isstring(L, -1)) {
		result->setText(lua_tostring(L, -1));
	}
	lua_pop(L, 1);
	return result;
}

void StatusConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<Status> payload) {
	lua_createtable(L, 0, 0);
	lua_pushstring(L, payload->getText().c_str());
	lua_setfield(L, -2, "text");
}
