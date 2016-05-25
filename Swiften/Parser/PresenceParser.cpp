/*
 * Copyright (c) 2010-2016 Isode Limited.
 * All rights reserved.
 * See the COPYING file for more information.
 */

#include <Swiften/Parser/PresenceParser.h>

#include <boost/optional.hpp>

#include <Swiften/Base/Log.h>

namespace Swift {

PresenceParser::PresenceParser(PayloadParserFactoryCollection* factories) :
        GenericStanzaParser<Presence>(factories) {
}

void PresenceParser::handleStanzaAttributes(const AttributeMap& attributes) {
    boost::optional<std::string> type = attributes.getAttributeValue("type");
    if (type) {
        if (*type == "unavailable") {
            getStanzaGeneric()->setType(Presence::Unavailable);
        }
        else if (*type == "probe") {
            getStanzaGeneric()->setType(Presence::Probe);
        }
        else if (*type == "subscribe") {
            getStanzaGeneric()->setType(Presence::Subscribe);
        }
        else if (*type == "subscribed") {
            getStanzaGeneric()->setType(Presence::Subscribed);
        }
        else if (*type == "unsubscribe") {
            getStanzaGeneric()->setType(Presence::Unsubscribe);
        }
        else if (*type == "unsubscribed") {
            getStanzaGeneric()->setType(Presence::Unsubscribed);
        }
        else if (*type == "error") {
            getStanzaGeneric()->setType(Presence::Error);
        }
        else {
            SWIFT_LOG(error) << "Unknown Presence type: " << *type << std::endl;
            getStanzaGeneric()->setType(Presence::Available);
        }
    }
    else {
        getStanzaGeneric()->setType(Presence::Available);
    }
}

}
