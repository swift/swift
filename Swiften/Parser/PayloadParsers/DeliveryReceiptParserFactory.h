/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

/*
 * Copyright (c) 2015-2017 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Parser/PayloadParserFactory.h>
#include <Swiften/Parser/PayloadParsers/DeliveryReceiptParser.h>

namespace Swift {
    class SWIFTEN_API DeliveryReceiptParserFactory : public PayloadParserFactory {
        public:
            DeliveryReceiptParserFactory() {
            }

            virtual bool canParse(const std::string& element, const std::string& ns, const AttributeMap&) const {
                return ns == "urn:xmpp:receipts" && element == "received";
            }

            virtual PayloadParser* createPayloadParser() {
                return new DeliveryReceiptParser();
            }

    };
}
