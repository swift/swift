/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the simplified BSD license.
 * See Documentation/Licenses/BSD-simplified.txt for more information.
 */

/*
 * Copyright (c) 2014-2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/GenericPayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/JingleFileTransferDescriptionParser.h>

namespace Swift {

    class PayloadParserFactoryCollection;

    class SWIFTEN_API JingleFileTransferDescriptionParserFactory : public PayloadParserFactory {
        public:
            JingleFileTransferDescriptionParserFactory(PayloadParserFactoryCollection* factories) : factories(factories) {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
                return element == "description" && ns == "urn:xmpp:jingle:apps:file-transfer:4";
            }

            virtual PayloadParser* createPayloadParser() {
                return new JingleFileTransferDescriptionParser(factories);
            }

        private:
            PayloadParserFactoryCollection* factories;

    };
}


