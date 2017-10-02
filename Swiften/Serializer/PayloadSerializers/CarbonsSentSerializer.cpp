/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CarbonsSentSerializer.h>

namespace Swift {
    CarbonsSentSerializer::CarbonsSentSerializer(PayloadSerializerCollection* serializers) : GenericPayloadSerializer<CarbonsSent>(), serializers_(serializers) {
    }

    CarbonsSentSerializer::~CarbonsSentSerializer() {
    }

    std::string CarbonsSentSerializer::serializePayload(std::shared_ptr<CarbonsSent> sent) const {
        XMLElement element("sent", "urn:xmpp:carbons:2");
        if (sent->getForwarded()) {
            element.addNode(std::make_shared<XMLRawTextNode>(ForwardedSerializer(serializers_).serialize(sent->getForwarded())));
        }
        return element.serialize();
    }
}

