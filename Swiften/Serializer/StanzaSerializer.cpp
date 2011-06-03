/*
 * Copyright (c) 2010 Remko Tronçon
 * Licensed under the GNU General Public License v3.
 * See Documentation/Licenses/GPLv3.txt for more information.
 */

#include <Swiften/Serializer/StanzaSerializer.h>

#include <sstream>
#include <typeinfo>
#include <iostream>

#include <Swiften/Base/foreach.h>
#include <Swiften/Serializer/XML/XMLElement.h>
#include <Swiften/Serializer/XML/XMLRawTextNode.h>
#include <Swiften/Serializer/PayloadSerializer.h>
#include <Swiften/Serializer/PayloadSerializerCollection.h>
#include <Swiften/Elements/Stanza.h>

namespace Swift {

StanzaSerializer::StanzaSerializer(const std::string& tag, PayloadSerializerCollection* payloadSerializers) : tag_(tag), payloadSerializers_(payloadSerializers) {
}

SafeByteArray StanzaSerializer::serialize(boost::shared_ptr<Element> element) const {
	boost::shared_ptr<Stanza> stanza(boost::dynamic_pointer_cast<Stanza>(element));

	XMLElement stanzaElement(tag_);
	if (stanza->getFrom().isValid()) {
		stanzaElement.setAttribute("from", stanza->getFrom());
	}
	if (stanza->getTo().isValid()) {
		stanzaElement.setAttribute("to", stanza->getTo());
	}
	if (!stanza->getID().empty()) {
		stanzaElement.setAttribute("id", stanza->getID());
	}
	setStanzaSpecificAttributes(stanza, stanzaElement);

	std::string serializedPayloads;
	foreach (const boost::shared_ptr<Payload>& payload, stanza->getPayloads()) {
		PayloadSerializer* serializer = payloadSerializers_->getPayloadSerializer(payload);
		if (serializer) {
			serializedPayloads += serializer->serialize(payload);
		}
		else {
			std::cerr << "Could not find serializer for " << typeid(*(payload.get())).name() << std::endl;
		}
	}
	if (!serializedPayloads.empty()) {
		stanzaElement.addNode(boost::shared_ptr<XMLNode>(new XMLRawTextNode(serializedPayloads)));
	}

	return createSafeByteArray(stanzaElement.serialize());
}

}
