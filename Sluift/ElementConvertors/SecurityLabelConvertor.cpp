/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/SecurityLabelConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

using namespace Swift;

SecurityLabelConvertor::SecurityLabelConvertor() :
        GenericLuaElementConvertor<SecurityLabel>("security_label") {
}

SecurityLabelConvertor::~SecurityLabelConvertor() {
}

std::shared_ptr<SecurityLabel> SecurityLabelConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<SecurityLabel> result = std::make_shared<SecurityLabel>();
    lua_getfield(L, -1, "equivalent_labels");
    if (lua_type(L, -1) == LUA_TTABLE) {
        std::vector< std::string > items;
        for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            if (lua_isstring(L, -1)) {
                items.push_back(std::string(lua_tostring(L, -1)));
            }
            lua_pop(L, 1);
        }

        result->setEquivalentLabels(items);
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "foreground_color");
    if (lua_isstring(L, -1)) {
        result->setForegroundColor(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "display_marking");
    if (lua_isstring(L, -1)) {
        result->setDisplayMarking(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "background_color");
    if (lua_isstring(L, -1)) {
        result->setBackgroundColor(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "label");
    if (lua_isstring(L, -1)) {
        result->setLabel(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    return result;
}

void SecurityLabelConvertor::doConvertToLua(lua_State* L, std::shared_ptr<SecurityLabel> payload) {
    lua_createtable(L, 0, 0);
    if (!payload->getEquivalentLabels().empty()) {
        lua_createtable(L, boost::numeric_cast<int>(payload->getEquivalentLabels().size()), 0);
        {
            int i = 0;
            for (const auto& item : payload->getEquivalentLabels()) {
                lua_pushstring(L, item.c_str());
                lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
                ++i;
            }
        }
        lua_setfield(L, -2, "equivalent_labels");
    }
    lua_pushstring(L, payload->getForegroundColor().c_str());
    lua_setfield(L, -2, "foreground_color");
    lua_pushstring(L, payload->getDisplayMarking().c_str());
    lua_setfield(L, -2, "display_marking");
    lua_pushstring(L, payload->getBackgroundColor().c_str());
    lua_setfield(L, -2, "background_color");
    lua_pushstring(L, payload->getLabel().c_str());
    lua_setfield(L, -2, "label");
}

boost::optional<LuaElementConvertor::Documentation> SecurityLabelConvertor::getDocumentation() const {
    return Documentation(
        "SecurityLabel",
        "This table has the following fields:\n\n"
        "- `equivalent_labels`: array<string>\n"
        "- `foreground_color`: string\n"
        "- `display_marking`: string\n"
        "- `background_color`: string\n"
        "- `label`: string\n"
    );
}
