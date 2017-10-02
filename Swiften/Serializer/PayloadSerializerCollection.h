/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class PayloadSerializer;

    class SWIFTEN_API PayloadSerializerCollection {
        public:
            PayloadSerializerCollection();
            virtual ~PayloadSerializerCollection();

            void addSerializer(PayloadSerializer* factory);
            void removeSerializer(PayloadSerializer* factory);
            PayloadSerializer* getPayloadSerializer(std::shared_ptr<Payload>) const;

        private:
            std::vector<PayloadSerializer*> serializers_;
    };
}
