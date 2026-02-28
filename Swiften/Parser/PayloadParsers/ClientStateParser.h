/*
 * Copyright (c) 2017 Tarun Gupta.
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/ClientState.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API ClientStateParser : public GenericPayloadParser<ClientState> {
        public:
            ClientStateParser();

            virtual void handleStartElement(const std::string& element, const std::string&, const AttributeMap& attributes);
            virtual void handleEndElement(const std::string& element, const std::string&);
            virtual void handleCharacterData(const std::string& data);

        private:
            int level_ = 0;
    };
}
