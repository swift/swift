/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/UnitTest/PayloadsSerializer.h>

#include <cppunit/extensions/HelperMacros.h>

#include <Swiften/Serializer/PayloadSerializer.h>

namespace Swift {

std::string PayloadsSerializer::serialize(std::shared_ptr<Payload> payload) {
    PayloadSerializer* serializer = serializers.getPayloadSerializer(payload);
    if (serializer) {
        return serializer->serialize(payload);
    }
    else {
        CPPUNIT_ASSERT(false);
        return "";
    }
}


}
