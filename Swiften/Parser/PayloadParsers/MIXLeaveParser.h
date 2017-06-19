/*
 * Copyright (c) 2017 Tarun Gupta
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <memory>

#include <Swiften/Base/API.h>
#include <Swiften/Base/Override.h>
#include <Swiften/Elements/MIXLeave.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {

    class SWIFTEN_API MIXLeaveParser : public GenericPayloadParser<MIXLeave> {
        public:
            MIXLeaveParser();
            virtual ~MIXLeaveParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes) SWIFTEN_OVERRIDE;
            virtual void handleEndElement(const std::string& element, const std::string&) SWIFTEN_OVERRIDE;
            virtual void handleCharacterData(const std::string& data) SWIFTEN_OVERRIDE;

        private:
            int level_;
    };
}
