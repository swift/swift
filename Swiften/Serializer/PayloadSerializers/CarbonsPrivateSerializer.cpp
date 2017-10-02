/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CarbonsPrivateSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    CarbonsPrivateSerializer::CarbonsPrivateSerializer() : GenericPayloadSerializer<CarbonsPrivate>() {
    }

    CarbonsPrivateSerializer::~CarbonsPrivateSerializer() {

    }

    std::string CarbonsPrivateSerializer::serializePayload(std::shared_ptr<CarbonsPrivate>)    const {
        XMLElement element("private", "urn:xmpp:carbons:2");
        return element.serialize();
    }
}
