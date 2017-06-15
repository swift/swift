/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MIXJoin.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class PayloadParser;

    class SWIFTEN_API MIXJoinParser : public GenericPayloadParser<MIXJoin> {
        public:
            MIXJoinParser();
            virtual ~MIXJoinParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
            virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
            virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

        private:
            int level_;
            std::shared_ptr<PayloadParser> currentPayloadParser_;
    };
}
