/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

/*
 * Copyright (c) 2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#pragma once

#include <Swiften/Base/API.h>
#include <Swiften/Elements/DeliveryReceiptRequest.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API DeliveryReceiptRequestSerializer : public GenericPayloadSerializer<DeliveryReceiptRequest> {
        public:
            DeliveryReceiptRequestSerializer();

            virtual std::string serializePayload(std::shared_ptr<DeliveryReceiptRequest> request) const;
    };
}
