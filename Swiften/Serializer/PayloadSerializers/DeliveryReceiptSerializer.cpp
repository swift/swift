/*
 * Copyright (c) 2011 Tobias Markmann
 * Licensed under the BSD license.
 * See http://www.opensource.org/licenses/bsd-license.php for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/DeliveryReceiptSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

DeliveryReceiptSerializer::DeliveryReceiptSerializer() : GenericPayloadSerializer<DeliveryReceipt>() {
}

std::string DeliveryReceiptSerializer::serializePayload(boost::shared_ptr<DeliveryReceipt> receipt) const {
	XMLElement received("received", "urn:xmpp:receipts");
	received.setAttribute("id", receipt->getReceivedID());
	return received.serialize();
}

}
