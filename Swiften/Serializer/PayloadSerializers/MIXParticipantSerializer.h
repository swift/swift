/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MIXParticipant.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;
    class SWIFTEN_API MIXParticipantSerializer : public GenericPayloadSerializer<MIXParticipant> {
        public:
            MIXParticipantSerializer(PayloadSerializerCollection* serializers);

            virtual std::string serializePayload(std::shared_ptr<MIXParticipant> participant)  const;
        private:
            PayloadSerializerCollection* serializers_;
    };
}
