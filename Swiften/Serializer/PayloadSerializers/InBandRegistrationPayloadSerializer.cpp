/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/PayloadSerializers/InBandRegistrationPayloadSerializer.h>

#include <boost/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializers/FormSerializer.h>

namespace Swift {

InBandRegistrationPayloadSerializer::InBandRegistrationPayloadSerializer() {
}

std::string InBandRegistrationPayloadSerializer::serializePayload(boost::shared_ptr<InBandRegistrationPayload> registration)	const {
	XMLElement registerElement("query", "jabber:iq:register");

	if (registration->isRegistered()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("registered")));
	}

	if (registration->isRemove()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("remove")));
	}

	if (registration->getInstructions()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("instructions", "", *registration->getInstructions())));
	}


	if (registration->getUsername()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("username", "", *registration->getUsername())));
	}

	if (registration->getNick()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("nick", "", *registration->getNick())));
	}

	if (registration->getPassword()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("password", "", *registration->getPassword())));
	}

	if (registration->getName()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("name", "", *registration->getName())));
	}

	if (registration->getFirst()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("first", "", *registration->getFirst())));
	}

	if (registration->getLast()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("last", "", *registration->getLast())));
	}

	if (registration->getEMail()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("email", "", *registration->getEMail())));
	}

	if (registration->getAddress()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("address", "", *registration->getAddress())));
	}

	if (registration->getCity()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("city", "", *registration->getCity())));
	}

	if (registration->getState()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("state", "", *registration->getState())));
	}

	if (registration->getZip()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("zip", "", *registration->getZip())));
	}

	if (registration->getPhone()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("phone", "", *registration->getPhone())));
	}

	if (registration->getURL()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("url", "", *registration->getURL())));
	}

	if (registration->getDate()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("date", "", *registration->getDate())));
	}

	if (registration->getMisc()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("misc", "", *registration->getMisc())));
	}

	if (registration->getText()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("text", "", *registration->getText())));
	}

	if (registration->getKey()) {
		registerElement.addNode(XMLElement::ref(new XMLElement("key", "", *registration->getKey())));
	}

	if (Form::ref form = registration->getForm()) {
		registerElement.addNode(boost::make_shared<XMLRawTextNode>(FormSerializer().serialize(form)));
	}

	return registerElement.serialize();
}

}
