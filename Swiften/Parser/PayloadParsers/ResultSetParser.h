/*
 * Copyright (c) 2014-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ResultSet.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API ResultSetParser : public GenericPayloadParser<ResultSet> {
        public:
            ResultSetParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) override;
            virtual void handleEndElement(const std::string& element, const std::string&) override;
            virtual void handleCharacterData(const std::string& data) override;

            enum Level {
                TopLevel = 0,
                PayloadLevel = 1
            };

        private:
            std::string currentText_;
            int level_;
    };
}
