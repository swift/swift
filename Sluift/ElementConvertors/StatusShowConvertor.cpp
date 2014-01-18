/*
 * Copyright (c) 2014 Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/StatusShowConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <Sluift/Lua/Check.h>

using namespace Swift;

StatusShowConvertor::StatusShowConvertor() : GenericLuaElementConvertor<StatusShow>("show") {
}

StatusShowConvertor::~StatusShowConvertor() {
}

boost::shared_ptr<StatusShow> StatusShowConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<StatusShow> result = boost::make_shared<StatusShow>();
	lua_getfield(L, -1, "type");
	if (lua_isstring(L, -1)) {
		if (std::string(lua_tostring(L, -1)) == "online") {
			result->setType(StatusShow::Online);
		}
		if (std::string(lua_tostring(L, -1)) == "ffc") {
			result->setType(StatusShow::FFC);
		}
		if (std::string(lua_tostring(L, -1)) == "away") {
			result->setType(StatusShow::Away);
		}
		if (std::string(lua_tostring(L, -1)) == "xa") {
			result->setType(StatusShow::XA);
		}
		if (std::string(lua_tostring(L, -1)) == "dnd") {
			result->setType(StatusShow::DND);
		}
	}
	lua_pop(L, 1);
	return result;
}

void StatusShowConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<StatusShow> payload) {
	lua_createtable(L, 0, 0);
	if (payload->getType() != StatusShow::None) {
		switch (payload->getType()) {
			case StatusShow::Online: lua_pushstring(L, "online"); break;
			case StatusShow::FFC: lua_pushstring(L, "ffc"); break;
			case StatusShow::Away: lua_pushstring(L, "away"); break;
			case StatusShow::XA: lua_pushstring(L, "xa"); break;
			case StatusShow::DND: lua_pushstring(L, "dnd"); break;
			case StatusShow::None: assert(false); break;
		}
		lua_setfield(L, -2, "type");
	}
}
