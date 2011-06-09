/*
 * Copyright (c) 2010 Kevin Smith
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/DelaySerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/String.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Base/DateTime.h>

namespace Swift {

DelaySerializer::DelaySerializer() : GenericPayloadSerializer<Delay>() {
}

std::string DelaySerializer::serializePayload(boost::shared_ptr<Delay> delay)  const {
	XMLElement delayElement("delay", "urn:xmpp:delay");
	if (delay->getFrom()) {
		delayElement.setAttribute("from", delay->getFrom()->toString());
	}
	delayElement.setAttribute("stamp", dateTimeToString(delay->getStamp()));
	return delayElement.serialize();
}

}
