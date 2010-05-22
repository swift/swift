/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/MUCPayloadSerializer.h"

#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {

MUCPayloadSerializer::MUCPayloadSerializer() : GenericPayloadSerializer<MUCPayload>() {
}

String MUCPayloadSerializer::serializePayload(boost::shared_ptr<MUCPayload> payload)  const {
	XMLElement mucElement("x", "http://jabber.org/protocol/muc");
	return mucElement.serialize();
}

}
