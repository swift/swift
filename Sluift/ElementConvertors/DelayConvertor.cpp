/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/DelayConvertor.h>

#include <boost/numeric/conversion/cast.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <lua.hpp>

#include <Swiften/Base/DateTime.h>

#include <Sluift/Lua/Check.h>

using namespace Swift;

DelayConvertor::DelayConvertor() : GenericLuaElementConvertor<Delay>("delay") {
}

DelayConvertor::~DelayConvertor() {
}

boost::shared_ptr<Delay> DelayConvertor::doConvertFromLua(lua_State* L) {
    boost::shared_ptr<Delay> result = boost::make_shared<Delay>();
    lua_getfield(L, -1, "stamp");
    if (lua_isstring(L, -1)) {
        result->setStamp(stringToDateTime(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);

    lua_getfield(L, -1, "from");
    if (lua_isstring(L, -1)) {
        result->setFrom(lua_tostring(L, -1));
    }
    lua_pop(L, 1);
    return result;
}

void DelayConvertor::doConvertToLua(lua_State* L, boost::shared_ptr<Delay> payload) {
    lua_createtable(L, 0, 0);
    if (payload->getFrom()) {
        lua_pushstring(L, (*payload->getFrom()).toString().c_str());
        lua_setfield(L, -2, "from");
    }
    lua_pushstring(L, dateTimeToString(payload->getStamp()).c_str());
    lua_setfield(L, -2, "stamp");
}
