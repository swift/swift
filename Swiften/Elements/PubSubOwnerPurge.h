/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <string>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>
#include <Swiften/Elements/PubSubOwnerPayload.h>

namespace Swift {
    class SWIFTEN_API PubSubOwnerPurge : public PubSubOwnerPayload {
        public:

            PubSubOwnerPurge();

            virtual ~PubSubOwnerPurge();

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
