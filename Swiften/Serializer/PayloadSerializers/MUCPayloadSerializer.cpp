/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/MUCPayloadSerializer.h"

#include "Swiften/Serializer/XML/XMLElement.h"
#include <Swiften/Base/String.h>

namespace Swift {

MUCPayloadSerializer::MUCPayloadSerializer() : GenericPayloadSerializer<MUCPayload>() {
}

std::string MUCPayloadSerializer::serializePayload(boost::shared_ptr<MUCPayload> muc)  const {
	XMLElement mucElement("x", "http://jabber.org/protocol/muc");
	boost::shared_ptr<XMLElement> historyElement(new XMLElement("history"));
	bool history = false;
	if (muc->getMaxChars() >= 0) {
		historyElement->setAttribute("maxchars", boost::lexical_cast<std::string>(muc->getMaxChars()));
		history = true;
	}
	if (muc->getMaxStanzas() >= 0) {
		historyElement->setAttribute("maxstanzas", boost::lexical_cast<std::string>(muc->getMaxStanzas()));
		history = true;
	}
	if (muc->getSeconds() >= 0) {
		historyElement->setAttribute("seconds", boost::lexical_cast<std::string>(muc->getSeconds()));
		history = true;
	}
	if (muc->getSince() != boost::posix_time::not_a_date_time) {
		std::string sinceString = std::string(boost::posix_time::to_iso_extended_string(muc->getSince()));
		String::replaceAll(sinceString, ',', ".");
		sinceString += "Z";
		historyElement->setAttribute("since", sinceString);
		history = true;
	}
	if (history) {
		mucElement.addNode(historyElement);
	}
	return mucElement.serialize();
}

}
