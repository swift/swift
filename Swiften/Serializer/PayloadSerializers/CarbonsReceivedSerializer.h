/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CarbonsReceived.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {
    class SWIFTEN_API CarbonsReceivedSerializer : public GenericPayloadSerializer<CarbonsReceived> {
        public:
            CarbonsReceivedSerializer(PayloadSerializerCollection* serializers);
            virtual ~CarbonsReceivedSerializer();

            virtual std::string serializePayload(std::shared_ptr<CarbonsReceived> received) const;

        private:
            PayloadSerializerCollection* serializers_;
    };
}

