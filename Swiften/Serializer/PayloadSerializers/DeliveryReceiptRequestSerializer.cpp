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

#include <Swiften/Serializer/PayloadSerializers/DeliveryReceiptRequestSerializer.h>

#include <Swiften/Base/Log.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

DeliveryReceiptRequestSerializer::DeliveryReceiptRequestSerializer() : GenericPayloadSerializer<DeliveryReceiptRequest>() {
}

std::string DeliveryReceiptRequestSerializer::serializePayload(std::shared_ptr<DeliveryReceiptRequest> /* request*/) const {
    XMLElement requestXML("request", "urn:xmpp:receipts");
    return requestXML.serialize();
}

}
