/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/SecurityLabel.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SerializingParser;

    class SWIFTEN_API SecurityLabelParser : public GenericPayloadParser<SecurityLabel> {
        public:
            SecurityLabelParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);
            std::shared_ptr<SecurityLabel> getLabelPayload() const;
        private:
            enum Level {
                TopLevel = 0,
                PayloadLevel = 1,
                DisplayMarkingOrLabelLevel = 2,
                SecurityLabelLevel = 3
            };
            int level_;
            SerializingParser* labelParser_;
            std::string currentText_;
    };
}
