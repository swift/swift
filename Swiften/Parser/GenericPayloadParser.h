/*
 * Copyright (c) 2010-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParser.h>

namespace Swift {
    /**
     * A generic payload parser for payloads of the given type.
     *
     * This class provides getPayloadInternal() for retrieving the actual
     * payload.
     */
    template<typename PAYLOAD_TYPE>
    class GenericPayloadParser : public PayloadParser {
        public:
            GenericPayloadParser() : PayloadParser() {
                payload_ = std::make_shared<PAYLOAD_TYPE>();
            }

            virtual std::shared_ptr<Payload> getPayload() const {
                return payload_;
            }

            virtual std::shared_ptr<PAYLOAD_TYPE> getPayloadInternal() const {
                return payload_;
            }

        private:
            std::shared_ptr<PAYLOAD_TYPE> payload_;
    };
}
