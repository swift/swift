/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CarbonsReceivedSerializer.h>

#include <boost/make_shared.hpp>

namespace Swift {
    CarbonsReceivedSerializer::CarbonsReceivedSerializer(PayloadSerializerCollection* serializers) : GenericPayloadSerializer<CarbonsReceived>(), serializers_(serializers) {
    }

    CarbonsReceivedSerializer::~CarbonsReceivedSerializer() {
    }

    std::string CarbonsReceivedSerializer::serializePayload(boost::shared_ptr<CarbonsReceived> received) const {
        XMLElement element("received", "urn:xmpp:carbons:2");
        if (received->getForwarded()) {
            element.addNode(boost::make_shared<XMLRawTextNode>(ForwardedSerializer(serializers_).serialize(received->getForwarded())));
        }
        return element.serialize();
    }
}

