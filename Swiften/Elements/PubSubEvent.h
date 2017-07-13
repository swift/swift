/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ContainerPayload.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubEventPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubEvent : public ContainerPayload<PubSubEventPayload> {
        public:
            PubSubEvent();
            virtual ~PubSubEvent();
    };
}
