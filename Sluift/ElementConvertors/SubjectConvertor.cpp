/*
 * Copyright (c) 2014 Kevin Smith and Remko Tronçon
 * Licensed under the GNU General Public License.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/SubjectConvertor.h>

#include <lua.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <Sluift/Lua/LuaUtils.h>

using namespace Swift;

SubjectConvertor::SubjectConvertor() : GenericLuaElementConvertor<Subject>("subject") {
}

SubjectConvertor::~SubjectConvertor() {
}

boost::shared_ptr<Subject> SubjectConvertor::doConvertFromLua(lua_State* L) {
	boost::shared_ptr<Subject> result = boost::make_shared<Subject>();
	if (boost::optional<std::string> value = Lua::getStringField(L, -1, "text")) {
		result->setText(*value);
	}
	return result;
}

void SubjectConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<Subject> payload) {
	lua_createtable(L, 0, 0);
	if (!payload->getText().empty()) {
		lua_pushstring(L, payload->getText().c_str());
		lua_setfield(L, -2, "text");
	}
}
