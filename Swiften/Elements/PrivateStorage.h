/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/Payload.h>

namespace Swift {
    class SWIFTEN_API PrivateStorage : public Payload {
        public:
            PrivateStorage(std::shared_ptr<Payload> payload = std::shared_ptr<Payload>()) : payload(payload) {
            }

            std::shared_ptr<Payload> getPayload() const {
                return payload;
            }

            void setPayload(std::shared_ptr<Payload> p) {
                payload = p;
            }

        private:
            std::shared_ptr<Payload> payload;
    };
}
