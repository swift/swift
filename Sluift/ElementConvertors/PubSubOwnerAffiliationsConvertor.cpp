/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubOwnerAffiliationsConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubOwnerAffiliationsConvertor::PubSubOwnerAffiliationsConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubOwnerAffiliations>("pubsub_owner_affiliations"),
        convertors(convertors) {
}

PubSubOwnerAffiliationsConvertor::~PubSubOwnerAffiliationsConvertor() {
}

std::shared_ptr<PubSubOwnerAffiliations> PubSubOwnerAffiliationsConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubOwnerAffiliations> result = std::make_shared<PubSubOwnerAffiliations>();
    lua_getfield(L, -1, "node");
    if (lua_isstring(L, -1)) {
        result->setNode(std::string(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);
    if (lua_type(L, -1) == LUA_TTABLE) {
        std::vector< std::shared_ptr<PubSubOwnerAffiliation> > items;
        for(size_t i = 0; i < lua_objlen(L, -1); ++i) {
            lua_pushnumber(L, i + 1);
            lua_gettable(L, -2);
            if (!lua_isnil(L, -1)) {
                if (std::shared_ptr<PubSubOwnerAffiliation> payload = std::dynamic_pointer_cast<PubSubOwnerAffiliation>(convertors->convertFromLuaUntyped(L, -1, "pubsub_owner_affiliation"))) {
                    items.push_back(payload);
                }
            }
            lua_pop(L, 1);
        }

        result->setAffiliations(items);
    }
    return result;
}

void PubSubOwnerAffiliationsConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubOwnerAffiliations> payload) {
    lua_createtable(L, 0, 0);
    lua_pushstring(L, payload->getNode().c_str());
    lua_setfield(L, -2, "node");
    if (!payload->getAffiliations().empty()) {
        {
            int i = 0;
            for (auto&& item : payload->getAffiliations()) {
                if (convertors->convertToLuaUntyped(L, item) > 0) {
                    lua_rawseti(L, -2, boost::numeric_cast<int>(i+1));
                    ++i;
                }
            }
        }
    }
}

boost::optional<LuaElementConvertor::Documentation> PubSubOwnerAffiliationsConvertor::getDocumentation() const {
    return Documentation(
        "PubSubOwnerAffiliations",
        "This table has the following fields:\n\n"
        "- `node`: string\n"
        "- `affiliations`: array<@{PubSubOwnerAffiliation}>\n"
    );
}
