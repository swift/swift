/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CarbonsReceivedSerializer.h>

#include <memory>

namespace Swift {
    CarbonsReceivedSerializer::CarbonsReceivedSerializer(PayloadSerializerCollection* serializers) : GenericPayloadSerializer<CarbonsReceived>(), serializers_(serializers) {
    }

    CarbonsReceivedSerializer::~CarbonsReceivedSerializer() {
    }

    std::string CarbonsReceivedSerializer::serializePayload(std::shared_ptr<CarbonsReceived> received) const {
        XMLElement element("received", "urn:xmpp:carbons:2");
        if (received->getForwarded()) {
            element.addNode(std::make_shared<XMLRawTextNode>(ForwardedSerializer(serializers_).serialize(received->getForwarded())));
        }
        return element.serialize();
    }
}

