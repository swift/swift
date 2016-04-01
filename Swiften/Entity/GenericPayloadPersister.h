/*
 * Copyright (c) 2011-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Entity/PayloadPersister.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>

namespace Swift {
    template<typename PAYLOAD, typename PARSER, typename SERIALIZER>
    class SWIFTEN_API GenericPayloadPersister : public PayloadPersister {
        public:
            GenericPayloadPersister() {
            }

        public:
            std::shared_ptr<PAYLOAD> loadPayloadGeneric(const boost::filesystem::path& path) {
                return std::dynamic_pointer_cast<PAYLOAD>(loadPayload(path));
            }

        protected:
            virtual const PayloadSerializer* getSerializer() const {
                return &serializer;
            }

            virtual PayloadParser* createParser() const {
                return new PARSER();
            }

        private:
            SERIALIZER serializer;
    };
}
