/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PubSubError.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API PubSubErrorSerializer : public GenericPayloadSerializer<PubSubError> {
        public:
            PubSubErrorSerializer();
            virtual ~PubSubErrorSerializer() override;

            virtual std::string serializePayload(std::shared_ptr<PubSubError>) const override;

        private:
            static std::string serializeType(PubSubError::Type);
            static std::string serializeUnsupportedFeatureType(PubSubError::UnsupportedFeatureType);
    };
}
