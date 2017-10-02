/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Sluift/ElementConvertors/RawXMLElementConvertor.h>

#include <memory>

#include <lua.hpp>

#include <Swiften/Elements/RawXMLPayload.h>
#include <Swiften/Serializer/PayloadSerializer.h>

#include <Sluift/Lua/Check.h>

using namespace Swift;

RawXMLElementConvertor::RawXMLElementConvertor() {
}

RawXMLElementConvertor::~RawXMLElementConvertor() {
}

std::shared_ptr<Element> RawXMLElementConvertor::convertFromLua(lua_State* L, int index, const std::string& type) {
    if (type == "xml") {
        return std::make_shared<RawXMLPayload>(std::string(Lua::checkString(L, index)));
    }
    return std::shared_ptr<Payload>();
}

boost::optional<std::string> RawXMLElementConvertor::convertToLua(lua_State* L, std::shared_ptr<Element> element) {
    std::shared_ptr<Payload> payload = std::dynamic_pointer_cast<Payload>(element);
    if (!payload) {
        return boost::optional<std::string>();
    }
    PayloadSerializer* serializer = serializers.getPayloadSerializer(payload);
    assert(serializer);
    lua_pushstring(L, serializer->serialize(payload).c_str());
    return std::string("xml");
}
