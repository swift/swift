/*
 * Copyright (c) 2018 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ReferencePayload.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

    class PayloadParserFactoryCollection;

    class SWIFTEN_API ReferencePayloadParser : public GenericPayloadParser<ReferencePayload> {
        public:

            ReferencePayloadParser(PayloadParserFactoryCollection* factories);

            virtual void handleStartElement(const std::string& element, const std::string& ns, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string& ns);
            virtual void handleCharacterData(const std::string& data);

        private:

            ReferencePayload::Type getTypeFromString(const std::string& typeString) const;
            int level_ = 0;
            const int topLevel_ = 0;
            const int payloadLevel_ = 1;
            PayloadParserFactoryCollection* factories_;
            std::shared_ptr<PayloadParser> currentPayloadParser_;
    };
}
