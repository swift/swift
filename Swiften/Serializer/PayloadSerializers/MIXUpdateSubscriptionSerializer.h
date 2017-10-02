/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/MIXUpdateSubscription.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {

    class SWIFTEN_API MIXUpdateSubscriptionSerializer : public GenericPayloadSerializer<MIXUpdateSubscription> {
        public:
            MIXUpdateSubscriptionSerializer();
            virtual ~MIXUpdateSubscriptionSerializer();

            virtual std::string serializePayload(std::shared_ptr<MIXUpdateSubscription>) const override;
    };
}
