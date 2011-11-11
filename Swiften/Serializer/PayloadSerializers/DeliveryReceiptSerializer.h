/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

#pragma once

#include <Swiften/Serializer/GenericPayloadSerializer.h>
#include <Swiften/Elements/DeliveryReceipt.h>

namespace Swift {
	class DeliveryReceiptSerializer : public GenericPayloadSerializer<DeliveryReceipt> {
		public:
			DeliveryReceiptSerializer();

			virtual std::string serializePayload(boost::shared_ptr<DeliveryReceipt> receipt) const;
	};
}
