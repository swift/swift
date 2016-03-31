/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>

namespace Swift {
    class SWIFTEN_API FullPayloadSerializerCollection : public PayloadSerializerCollection {
        public:
            FullPayloadSerializerCollection();
            virtual ~FullPayloadSerializerCollection();

        private:
            std::vector<PayloadSerializer*> serializers_;
    };
}
