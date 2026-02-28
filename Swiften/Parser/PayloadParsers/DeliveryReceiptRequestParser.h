/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

/*
 * Copyright (c) 2015 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DeliveryReceiptRequest.h>
#include <Swiften/Parser/GenericPayloadParser.h>

namespace Swift {
    class SWIFTEN_API DeliveryReceiptRequestParser : public GenericPayloadParser<DeliveryReceiptRequest> {
        public:
            DeliveryReceiptRequestParser();

            virtual void handleStartElement(const std::string&, const std::string&, const AttributeMap&);
            virtual void handleEndElement(const std::string&, const std::string&);
            virtual void handleCharacterData(const std::string& data);
    };
}
