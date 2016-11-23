/*
 * Copyright (c) 2014-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/ForwardedConvertor.h>

#include <memory>

#include <boost/numeric/conversion/cast.hpp>

#include <lua.hpp>

#include <Swiften/Elements/Delay.h>
#include <Swiften/Elements/IQ.h>
#include <Swiften/Elements/Message.h>
#include <Swiften/Elements/Presence.h>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

ForwardedConvertor::ForwardedConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<Forwarded>("forwarded"),
        convertors(convertors) {
}

ForwardedConvertor::~ForwardedConvertor() {
}

std::shared_ptr<Forwarded> ForwardedConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<Forwarded> result = std::make_shared<Forwarded>();
    lua_getfield(L, -1, "delay");
    if (!lua_isnil(L, -1)) {
        std::shared_ptr<Delay> delay = std::dynamic_pointer_cast<Delay>(convertors->convertFromLuaUntyped(L, -1, "delay"));
        if (!!delay) {
            result->setDelay(delay);
        }
    }
    lua_pop(L, 1);
    lua_getfield(L, -1, "stanza");
    if (!lua_isnil(L, -1)) {
        std::shared_ptr<Stanza> stanza = std::dynamic_pointer_cast<Stanza>(convertors->convertFromLua(L, -1));
        if (!!stanza) {
            result->setStanza(stanza);
        }
        lua_pop(L, 1);
        return result;
    }
    return result;
}

void ForwardedConvertor::doConvertToLua(lua_State* L, std::shared_ptr<Forwarded> payload) {
    lua_createtable(L, 0, 0);
    if (convertors->convertToLuaUntyped(L, payload->getDelay()) > 0) {
        lua_setfield(L, -2, "delay");
    }
    std::shared_ptr<Stanza> stanza = payload->getStanza();
    if (!!stanza) {
        if (convertors->convertToLua(L, stanza) > 0) {
            lua_setfield(L, -2, "stanza");
        }
    }
}

boost::optional<LuaElementConvertor::Documentation> ForwardedConvertor::getDocumentation() const {
    return Documentation(
        "Forwarded",
        "This table has the following fields:\n\n"
        "- `delay`: @{Delay} (Optional)\n"
        "- `stanza`: @{Stanza} (Optional)\n"
    );
}
