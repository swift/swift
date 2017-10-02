/*
 * Copyright (c) 2013-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>
#include <vector>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    template<typename T>
    class SWIFTEN_API ContainerPayload : public Payload {
        public:
            ContainerPayload() {}
            ContainerPayload(std::shared_ptr<T> payload) : payload(payload) {}

            void setPayload(std::shared_ptr<T> payload) {
                this->payload = payload;
            }

            std::shared_ptr<T> getPayload() const {
                return payload;
            }

        private:
            std::shared_ptr<T> payload;
    };
}
