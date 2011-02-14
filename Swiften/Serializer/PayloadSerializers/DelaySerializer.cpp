/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include "Swiften/Serializer/PayloadSerializers/DelaySerializer.h"

#include <boost/shared_ptr.hpp>

#include <Swiften/Base/String.h>
#include "Swiften/Serializer/XML/XMLElement.h"

namespace Swift {

DelaySerializer::DelaySerializer() : GenericPayloadSerializer<Delay>() {
}

std::string DelaySerializer::serializePayload(boost::shared_ptr<Delay> delay)  const {
	XMLElement delayElement("delay", "urn:xmpp:delay");
	if (delay->getFrom()) {
		delayElement.setAttribute("from", delay->getFrom()->toString());
	}
	std::string stampString = boostPTimeToXEP0082(delay->getStamp());
	delayElement.setAttribute("stamp", stampString);
	return delayElement.serialize();
}

std::string DelaySerializer::boostPTimeToXEP0082(const boost::posix_time::ptime& time) {
	std::string stampString = std::string(boost::posix_time::to_iso_extended_string(time));
	String::replaceAll(stampString, ',', ".");
	stampString += "Z";
	return stampString;
}

}
