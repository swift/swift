/*
 * Copyright (c) 2015-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/CarbonsSent.h>
#include <Swiften/Elements/Forwarded.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializers/ForwardedSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>

namespace Swift {
    class SWIFTEN_API CarbonsSentSerializer : public GenericPayloadSerializer<CarbonsSent> {
        public:
            CarbonsSentSerializer(PayloadSerializerCollection* serializers);
            virtual ~CarbonsSentSerializer();

            virtual std::string serializePayload(std::shared_ptr<CarbonsSent> sent) const;

        private:
            PayloadSerializerCollection* serializers_;
    };
}

