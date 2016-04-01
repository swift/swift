/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/PubSubEventConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Sluift/LuaElementConvertors.h>

using namespace Swift;

PubSubEventConvertor::PubSubEventConvertor(LuaElementConvertors* convertors) :
        GenericLuaElementConvertor<PubSubEvent>("pubsub_event"),
        convertors(convertors) {
}

PubSubEventConvertor::~PubSubEventConvertor() {
}

std::shared_ptr<PubSubEvent> PubSubEventConvertor::doConvertFromLua(lua_State* L) {
    std::shared_ptr<PubSubEvent> result = std::make_shared<PubSubEvent>();
    if (std::shared_ptr<PubSubEventPayload> payload = std::dynamic_pointer_cast<PubSubEventPayload>(convertors->convertFromLua(L, -1))) {
        result->setPayload(payload);
    }
    return result;
}

void PubSubEventConvertor::doConvertToLua(lua_State* L, std::shared_ptr<PubSubEvent> event) {
    convertors->convertToLua(L,  event->getPayload());
}
