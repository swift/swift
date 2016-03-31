/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/PubSubRetract.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PubSubRetractSerializer : public GenericPayloadSerializer<PubSubRetract> {
        public:
            PubSubRetractSerializer(PayloadSerializerCollection* serializers);
            virtual ~PubSubRetractSerializer();

            virtual std::string serializePayload(boost::shared_ptr<PubSubRetract>) const SWIFTEN_OVERRIDE;

        private:


        private:
            PayloadSerializerCollection* serializers;
    };
}
