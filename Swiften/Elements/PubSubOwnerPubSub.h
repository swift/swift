/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ContainerPayload.h>
#include <Swiften/Elements/PubSubOwnerPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubOwnerPubSub : public ContainerPayload<PubSubOwnerPayload> {
        public:
            PubSubOwnerPubSub();
            virtual ~PubSubOwnerPubSub();
    };
}
