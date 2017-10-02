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
#include <Swiften/Elements/DeliveryReceipt.h>
#include <Swiften/Serializer/GenericPayloadSerializer.h>

namespace Swift {
    class SWIFTEN_API DeliveryReceiptSerializer : public GenericPayloadSerializer<DeliveryReceipt> {
        public:
            DeliveryReceiptSerializer();

            virtual std::string serializePayload(std::shared_ptr<DeliveryReceipt> receipt) const;
    };
}
