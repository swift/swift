/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PresenceSerializer.h>
#include <Swiften/Serializer/XML/XMLElement.h>

#include <boost/shared_ptr.hpp>

namespace Swift {

PresenceSerializer::PresenceSerializer(PayloadSerializerCollection* payloadSerializers) : 
		GenericStanzaSerializer<Presence>("presence", payloadSerializers) {
}

void PresenceSerializer::setStanzaSpecificAttributesGeneric(
		boost::shared_ptr<Presence> presence, 
		XMLElement& element) const {
	switch (presence->getType()) {
		case Presence::Unavailable: element.setAttribute("type","unavailable"); break;
		case Presence::Probe: element.setAttribute("type","probe"); break;
		case Presence::Subscribe: element.setAttribute("type","subscribe"); break;
		case Presence::Subscribed: element.setAttribute("type","subscribed"); break;
		case Presence::Unsubscribe: element.setAttribute("type","unsubscribe"); break;
		case Presence::Unsubscribed: element.setAttribute("type","unsubscribed"); break;
		case Presence::Error: element.setAttribute("type","error"); break;
		case Presence::Available: break;
	}
}

}
