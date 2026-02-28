/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ContainerPayload.h>
#include <Swiften/Elements/PubSubPayload.h>

namespace Swift {
    class SWIFTEN_API PubSub : public ContainerPayload<PubSubPayload> {
        public:
            PubSub();
            virtual ~PubSub();
    };
}
