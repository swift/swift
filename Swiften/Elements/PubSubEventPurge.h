/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubEventPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubEventPurge : public PubSubEventPayload {
        public:

            PubSubEventPurge();

            virtual ~PubSubEventPurge();

            const std::string& getNode() const {
                return node;
            }

            void setNode(const std::string& value) {
                this->node = value ;
            }


        private:
            std::string node;
    };
}
