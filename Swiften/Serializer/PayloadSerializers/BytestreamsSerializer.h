/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Bytestreams.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class PayloadSerializerCollection;

    class SWIFTEN_API BytestreamsSerializer : public GenericPayloadSerializer<Bytestreams> {
        public:
            BytestreamsSerializer();

            virtual std::string serializePayload(boost::shared_ptr<Bytestreams>)  const;
    };
}
