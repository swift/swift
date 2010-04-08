/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/MUCPayloadSerializer.h"

namespace Swift {

MUCPayloadSerializer::MUCPayloadSerializer() : GenericPayloadSerializer<MUCPayload>() {
}

String MUCPayloadSerializer::serializePayload(boost::shared_ptr<MUCPayload>)  const {
	String result("<x xmlns='http://jabber.org/protocol/muc'/>");
	return result;
}

}
