/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/DelaySerializer.h>

#include <memory>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <Swiften/Base/DateTime.h>
#include <Swiften/Base/String.h>
#include <Swiften/Serializer/XML/XMLElement.h>

namespace Swift {

DelaySerializer::DelaySerializer() : GenericPayloadSerializer<Delay>() {
}

std::string DelaySerializer::serializePayload(std::shared_ptr<Delay> delay)  const {
    XMLElement delayElement("delay", "urn:xmpp:delay");
    if (delay->getFrom() && delay->getFrom()->isValid()) {
        delayElement.setAttribute("from", delay->getFrom()->toString());
    }
    delayElement.setAttribute("stamp", dateTimeToString(delay->getStamp()));
    return delayElement.serialize();
}

}
