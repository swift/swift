/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CarbonsEnableSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    CarbonsEnableSerializer::CarbonsEnableSerializer() : GenericPayloadSerializer<CarbonsEnable>() {
    }

    CarbonsEnableSerializer::~CarbonsEnableSerializer() {

    }

    std::string CarbonsEnableSerializer::serializePayload(boost::shared_ptr<CarbonsEnable>)    const {
        XMLElement element("enable", "urn:xmpp:carbons:2");
        return element.serialize();
    }
}
