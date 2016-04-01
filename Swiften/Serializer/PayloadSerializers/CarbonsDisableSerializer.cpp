/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CarbonsDisableSerializer.h>

#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {
    CarbonsDisableSerializer::CarbonsDisableSerializer() : GenericPayloadSerializer<CarbonsDisable>() {

    }

    CarbonsDisableSerializer::~CarbonsDisableSerializer() {

    }

     std::string CarbonsDisableSerializer::serializePayload(std::shared_ptr<CarbonsDisable>)    const {
        XMLElement element("disable", "urn:xmpp:carbons:2");
        return element.serialize();
    }
}
