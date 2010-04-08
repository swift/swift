/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <iostream>

#include "Swiften/Parser/PresenceParser.h"

namespace Swift {

PresenceParser::PresenceParser(PayloadParserFactoryCollection* factories) : 
		GenericStanzaParser<Presence>(factories) {
}

void PresenceParser::handleStanzaAttributes(const AttributeMap& attributes) {
	AttributeMap::const_iterator type = attributes.find("type");
	if (type != attributes.end()) {
		if (type->second == "unavailable") {
			getStanzaGeneric()->setType(Presence::Unavailable);
		}
		else if (type->second == "probe") {
			getStanzaGeneric()->setType(Presence::Probe);
		}
		else if (type->second == "subscribe") {
			getStanzaGeneric()->setType(Presence::Subscribe);
		}
		else if (type->second == "subscribed") {
			getStanzaGeneric()->setType(Presence::Subscribed);
		}
		else if (type->second == "unsubscribe") {
			getStanzaGeneric()->setType(Presence::Unsubscribe);
		}
		else if (type->second == "unsubscribed") {
			getStanzaGeneric()->setType(Presence::Unsubscribed);
		}
		else if (type->second == "error") {
			getStanzaGeneric()->setType(Presence::Error);
		}
		else {
			std::cerr << "Unknown Presence type: " << type->second << std::endl;
			getStanzaGeneric()->setType(Presence::Available);
		}
	}
	else {
		getStanzaGeneric()->setType(Presence::Available);
	}
}

}
