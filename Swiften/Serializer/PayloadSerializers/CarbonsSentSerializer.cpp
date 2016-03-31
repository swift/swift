/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/CarbonsSentSerializer.h>

namespace Swift {
    CarbonsSentSerializer::CarbonsSentSerializer(PayloadSerializerCollection* serializers) : GenericPayloadSerializer<CarbonsSent>(), serializers_(serializers) {
    }

    CarbonsSentSerializer::~CarbonsSentSerializer() {
    }

    std::string CarbonsSentSerializer::serializePayload(boost::shared_ptr<CarbonsSent> sent) const {
        XMLElement element("sent", "urn:xmpp:carbons:2");
        if (sent->getForwarded()) {
            element.addNode(boost::make_shared<XMLRawTextNode>(ForwardedSerializer(serializers_).serialize(sent->getForwarded())));
        }
        return element.serialize();
    }
}

