/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>
#include <boost/optional.hpp>

#include <Swiften/Parser/PresenceParser.h>

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
			std::cerr << "Unknown Presence type: " << *type << std::endl;
			getStanzaGeneric()->setType(Presence::Available);
		}
	}
	else {
		getStanzaGeneric()->setType(Presence::Available);
	}
}

}
