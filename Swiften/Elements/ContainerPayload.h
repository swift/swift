/*
 * Copyright (c) 2013-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    template<typename T>
    class SWIFTEN_API ContainerPayload : public Payload {
        public:
            ContainerPayload() {}
            ContainerPayload(boost::shared_ptr<T> payload) : payload(payload) {}

            void setPayload(boost::shared_ptr<T> payload) {
                this->payload = payload;
            }

            boost::shared_ptr<T> getPayload() const {
                return payload;
            }

        private:
            boost::shared_ptr<T> payload;
    };
}
