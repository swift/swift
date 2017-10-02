/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/PrivateStorage.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API PrivateStorageSerializer : public GenericPayloadSerializer<PrivateStorage> {
        public:
            PrivateStorageSerializer(PayloadSerializerCollection* serializers);

            virtual std::string serializePayload(std::shared_ptr<PrivateStorage>)  const;

        private:
            PayloadSerializerCollection* serializers;
    };
}
